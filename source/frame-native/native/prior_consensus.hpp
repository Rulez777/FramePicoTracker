#pragma once
#include "few_led.hpp"
#include "optical_compute.hpp"
#include "recovery_diagnostics.hpp"

namespace pico {
// Deterministic two-correspondence hypotheses. Extra image blobs never become
// mandatory observations. Every final inlier is a unique measured centroid.
inline bool solve_prior_consensus(const NativeOpticalApi& api,const LedModel& model,
    const OpticalView* views,const Rigid& prior,double dt,OpticalFit& fit,int& observations,bool& rotation) {
    using namespace few_led_detail;
    observations=0; rotation=false;
    if(!api.project || !identity_rigid_valid(prior) || !std::isfinite(dt) || dt<=0 || dt*1e9>=double(prior_hold_ns())) return false;
    for(double x:model) if(!std::isfinite(x)) return false;
    for(int v=0;v<2;++v) {
        if(views[v].count>48 || (views[v].count && !views[v].pixels)) return false;
        for(size_t b=0;b<2*views[v].count;++b) if(!std::isfinite(views[v].pixels[b])) return false;
    }
    double projected[2][8][2]{},gate[2][8]{}; bool visible[2][8]{};
    for(int v=0;v<2;++v) for(int led=0;led<8;++led) {
        visible[v][led]=project(api,model,views[v],prior,led,projected[v][led],true);
        if(!visible[v][led]) continue;
        // Project the physical uncertainty envelope (~1 m/s), so gate grows
        // with age, focal length and inverse range, including fisheye geometry.
        gate[v][led]=8;
        for(int axis=0;axis<3;++axis) for(int sign:{-1,1}) {
            auto displaced=prior; displaced[4*axis+3]+=sign*(.02+dt);
            double uv[2];
            if(project(api,model,views[v],displaced,led,uv,false))
                gate[v][led]=std::max(gate[v][led],std::hypot(uv[0]-projected[v][led][0],uv[1]-projected[v][led][1]));
            else gate[v][led]=std::max(gate[v][led],double(views[v].camera.width));
        }
    }
    auto seed_consistent=[&](int v,int led,size_t blob) {
        return visible[v][led] && std::hypot(projected[v][led][0]-views[v].pixels[2*blob],
            projected[v][led][1]-views[v].pixels[2*blob+1])<=gate[v][led];
    };
    auto consensus=[&](const Rigid& pose,double maximum_radius) {
        std::vector<Match> out;
        for(int v=0;v<2;++v) {
            double uv[8][2]{}; bool valid[8]{}; int claim[8]; std::fill(claim,claim+8,-1);
            for(int led=0;led<8;++led) valid[led]=project(api,model,views[v],pose,led,uv[led],true);
            for(size_t b=0;b<views[v].count;++b) {
                int id=-1,count=0;
                for(int led=0;led<8;++led) if(valid[led]) {
                    double separation=1e9;
                    for(int other=0;other<8;++other) if(other!=led && valid[other])
                        separation=std::min(separation,std::hypot(uv[led][0]-uv[other][0],uv[led][1]-uv[other][1]));
                    const double radius=separation<1?0:std::min(maximum_radius,.4*separation);
                    if(radius>0 && std::hypot(uv[led][0]-views[v].pixels[2*b],uv[led][1]-views[v].pixels[2*b+1])<=radius) { id=led; ++count; }
                }
                if(count==1) claim[id]=claim[id]==-1?int(b):-2;
            }
            for(int led=0;led<8;++led) if(claim[led]>=0) out.push_back({v,led,claim[led]});
        }
        return out;
    };
    struct Hypothesis { double cost; Match a,b; };
    std::vector<Hypothesis> hypotheses;
    // Same-view two LED hypotheses tolerate translation and range change.
    for(int v=0;v<2;++v) for(int a=0;a<8;++a) for(int b=a+1;b<8;++b) if(visible[v][a] && visible[v][b]) {
        if(OpticalComputeBudget::exhausted()) return recovery_reject("consensus/budget");
        const double px=projected[v][b][0]-projected[v][a][0],py=projected[v][b][1]-projected[v][a][1],plen=std::hypot(px,py);
        if(plen<2) continue;
        for(size_t i=0;i<views[v].count;++i) if(seed_consistent(v,a,i)) for(size_t j=0;j<views[v].count;++j) if(i!=j && seed_consistent(v,b,j)) {
            const double x=views[v].pixels[2*j]-views[v].pixels[2*i],y=views[v].pixels[2*j+1]-views[v].pixels[2*i+1],len=std::hypot(x,y);
            if(len<2 || len<.4*plen || len>2.5*plen) continue;
            const double cosine=(px*x+py*y)/(plen*len);
            if(cosine<.97) continue;
            const double movement=std::hypot(projected[v][a][0]-views[v].pixels[2*i],projected[v][a][1]-views[v].pixels[2*i+1]);
            hypotheses.push_back({(1-cosine)*100+std::abs(std::log(len/plen))+.001*movement,{v,a,int(i)},{v,b,int(j)}});
        }
    }
    std::stable_sort(hypotheses.begin(),hypotheses.end(),[](const auto& a,const auto& b){return a.cost<b.cost;});
    if(hypotheses.size()>256) hypotheses.resize(256);
    if(hypotheses.empty()) recovery_reject("consensus/no-pair-hypotheses");
    OpticalFit best{}; int best_n=0; bool best_rotation=false,ambiguous=false;
    double best_turn=1e9;
    auto refine=[&](const Rigid& seed,const std::vector<Match>& matches,OpticalFit& candidate,int& n,bool& refined) {
        if(refine_led_matches(api,model,views,seed,dt,matches,candidate,n,refined,false,true)) return true;
        // Pixel noise and nearly planar LEDs may not constrain rotation. Keep
        // calibrated gyro orientation, while still requiring measured translation,
        // unique matches and the same residual/position covariance/speed limits.
        auto fixed=prior;
        for(int k=0;k<3;++k) fixed[4*k+3]=seed[4*k+3];
        if(!refine_led_matches(api,model,views,fixed,dt,matches,candidate,n,refined,true,true)) return false;
        ++recovery_diagnostics["consensus/gyro-fixed-refine"];
        return true;
    };
    auto consider=[&](const Rigid& seed) {
        auto matches=consensus(seed,4.5);
        if(matches.size()<3) { recovery_reject("consensus/initial-inliers"); return; }
        OpticalFit candidate{}; int n=0; bool refined=false;
        if(!refine(seed,matches,candidate,n,refined)) {
            recovery_reject("consensus/initial-refine"); return;
        }
        Rigid pose; std::copy(std::begin(candidate.pose),std::end(candidate.pose),pose.begin());
        // Reassign once after fitting, then freeze the final inlier set.
        matches=consensus(pose,2.5);
        if(matches.size()<3) { recovery_reject("consensus/final-inliers"); return; }
        if(!refine(pose,matches,candidate,n,refined)) {
            recovery_reject("consensus/final-refine"); return;
        }
        double shift=0,trace=0;
        for(int i=0;i<3;++i) {
            shift+=std::pow(candidate.pose[4*i+3]-prior[4*i+3],2);
            for(int j=0;j<3;++j) trace+=candidate.pose[4*i+j]*prior[4*i+j];
        }
        const double turn=std::acos(std::clamp((trace-1)*.5,-1.,1.));
        if(std::sqrt(shift)>.02+dt || turn>.20) { recovery_reject("consensus/speed-or-orientation"); return; }
        if(n<best_n) return;
        double distance=0;
        for(int k=0;k<3;++k) distance+=std::pow(candidate.pose[4*k+3]-best.pose[4*k+3],2);
        double agreement=0;
        for(int i=0;i<3;++i) for(int j=0;j<3;++j) agreement+=candidate.pose[4*i+j]*best.pose[4*i+j];
        // Gyro chooses the rotational branch among equally supported hypotheses.
        // A translation ambiguity with indistinguishable orientation still drops.
        if(best_n==n && std::abs(turn-best_turn)<.00873 &&
            (std::sqrt(distance)>.03 || std::acos(std::clamp((agreement-1)*.5,-1.,1.))>.0873) &&
            std::abs(candidate.residual-best.residual)<=.25) ambiguous=true;
        if(n>best_n || turn<best_turn-.00873) ambiguous=false;
        if(n>best_n || turn<best_turn-1e-6 || (std::abs(turn-best_turn)<=1e-6 && candidate.residual<best.residual)) {
            best=candidate; best_n=n; best_rotation=refined; best_turn=turn;
        }
    };
    consider(prior);
    for(const auto& h:hypotheses) {
        if(OpticalComputeBudget::exhausted()) break;
        OpticalFit hypothesis{}; int n=0; bool refined=false;
        if(!refine_led_matches(api,model,views,prior,dt,{h.a,h.b},hypothesis,n,refined,true,true)) continue;
        Rigid seed; std::copy(std::begin(hypothesis.pose),std::end(hypothesis.pose),seed.begin());
        consider(seed);
        if(best_n==int(std::min<size_t>(8,views[0].count)+std::min<size_t>(8,views[1].count)) && best.residual<.05) break;
    }
    if(best_n<3) return recovery_reject("consensus/no-fit");
    if(ambiguous) return recovery_reject("consensus/ambiguous");
    fit=best; observations=best_n; rotation=best_rotation; return true;
}
}
