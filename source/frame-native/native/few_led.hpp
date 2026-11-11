#pragma once
#include "recovery_diagnostics.hpp"
#include "native_identity.hpp"
#include "tracking_recovery.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <set>

namespace pico {
inline bool few_led_enabled() {
    const char* value=std::getenv("PICO_FEW_LED");
    return !value || std::string(value)!="0";
}
namespace few_led_detail {
inline Rigid multiply(const Rigid& a,const Rigid& b) {
    Rigid out{};
    for(int i=0;i<4;++i) for(int j=0;j<4;++j) for(int k=0;k<4;++k)
        out[4*i+j]+=a[4*i+k]*b[4*k+j];
    return out;
}
inline Rigid step(const Rigid& pose,const double* x,int dimensions) {
    Rigid out=pose;
    for(int k=0;k<3;++k) out[4*k+3]+=x[k];
    if(dimensions==6) {
        double angle=0;
        for(int k=3;k<6;++k) angle+=x[k]*x[k];
        angle=std::sqrt(angle);
        const double s=angle<1e-9?1:std::sin(angle)/angle;
        const double c=angle<1e-9?.5:(1-std::cos(angle))/(angle*angle);
        const double skew[9]{0,-x[5],x[4],x[5],0,-x[3],-x[4],x[3],0};
        double rotation[9]{};
        for(int i=0;i<3;++i) for(int j=0;j<3;++j) {
            rotation[3*i+j]=(i==j?1.:0.)+s*skew[3*i+j];
            for(int k=0;k<3;++k) rotation[3*i+j]+=c*skew[3*i+k]*skew[3*k+j];
        }
        for(int i=0;i<3;++i) for(int j=0;j<3;++j) {
            out[4*i+j]=0;
            for(int k=0;k<3;++k) out[4*i+j]+=pose[4*i+k]*rotation[3*k+j];
        }
    }
    return out;
}
inline bool inverse(double a[6][6],double inv[6][6],int n,double& condition) {
    double scale[6]{};
    for(int i=0;i<n;++i) {
        if(!std::isfinite(a[i][i]) || a[i][i]<=1e-12) return false;
        scale[i]=std::sqrt(a[i][i]);
    }
    double b[6][12]{};
    for(int i=0;i<n;++i) for(int j=0;j<n;++j) {
        b[i][j]=a[i][j]/(scale[i]*scale[j]);
        b[i][n+j]=i==j?1.:0.;
    }
    for(int k=0;k<n;++k) {
        int pivot=k;
        for(int i=k+1;i<n;++i) if(std::abs(b[i][k])>std::abs(b[pivot][k])) pivot=i;
        if(std::abs(b[pivot][k])<1e-8) return false;
        for(int j=0;j<2*n;++j) std::swap(b[k][j],b[pivot][j]);
        const double divisor=b[k][k];
        for(int j=0;j<2*n;++j) b[k][j]/=divisor;
        for(int i=0;i<n;++i) if(i!=k) {
            const double f=b[i][k];
            for(int j=0;j<2*n;++j) b[i][j]-=f*b[k][j];
        }
    }
    double norm=0,inorm=0;
    for(int i=0;i<n;++i) {
        double row=0,irow=0;
        for(int j=0;j<n;++j) {
            row+=std::abs(a[i][j]/(scale[i]*scale[j]));
            irow+=std::abs(b[i][n+j]);
            inv[i][j]=b[i][n+j]/(scale[i]*scale[j]);
        }
        norm=std::max(norm,row); inorm=std::max(inorm,irow);
    }
    condition=norm*inorm;
    return std::isfinite(condition) && condition<1e7;
}
struct Match { int view,led,blob; };
inline bool project(const NativeOpticalApi& api,const LedModel& model,const OpticalView& view,
                    const Rigid& pose,int led,double* uv,bool facing) {
    double world[3]{},camera[3]{};
    for(int i=0;i<3;++i) {
        world[i]=pose[4*i+3];
        for(int j=0;j<3;++j) world[i]+=pose[4*i+j]*model[3*led+j];
    }
    for(int i=0;i<3;++i) for(int j=0;j<3;++j)
        camera[i]+=view.head_from_camera[4*j+i]*(world[j]-view.head_from_camera[4*j+3]);
    if(camera[2]<=.03) return false;
    if(facing) {
        // Same approximate outward/downward normals as pc/optical/constellation.py.
        // Factory blobs supply positions, NOT independently verified LED normals.
        const double normal[3]{model[3*led],model[3*led+1],-.005};
        double dot=0,length=0,range=0;
        for(int i=0;i<3;++i) {
            double n=0;
            for(int j=0;j<3;++j) n+=pose[4*i+j]*normal[j];
            const double ray=view.head_from_camera[4*i+3]-world[i];
            dot+=n*ray; length+=n*n; range+=ray*ray;
        }
        if(!(length>0 && range>0) || dot/std::sqrt(length*range)<=-.2) return false;
    }
    if(api.project(&view.camera,camera,1,uv)!=0) return false;
    return std::isfinite(uv[0]) && std::isfinite(uv[1]) &&
        uv[0]>=0 && uv[0]<view.camera.width && uv[1]>=0 && uv[1]<view.camera.height;
}
}

inline bool refine_led_matches(const NativeOpticalApi& api,const LedModel& model,const OpticalView* views,
    const Rigid& prior,double dt,const std::vector<few_led_detail::Match>& matches,OpticalFit& fit,
    int& observations,bool& rotation_refined,bool translation_only=false,bool recovery=false) {
    using namespace few_led_detail;
    observations=0; rotation_refined=false;
    if(matches.size()<2 || matches.size()>16) return recovery_reject("refine/observation-count");
    std::set<int> unique; for(const auto& m:matches) unique.insert(m.led);
    auto residuals=[&](const Rigid& pose,double* values) {
        for(size_t i=0;i<matches.size();++i) {
            const auto& m=matches[i]; double uv[2];
            if(!project(api,model,views[m.view],pose,m.led,uv,false)) return false;
            for(int k=0;k<2;++k) values[2*i+k]=uv[k]-views[m.view].pixels[2*m.blob+k];
        }
        return true;
    };
    auto normal=[&](const Rigid& pose,int dimensions,double a[6][6],double* g,double* error) {
        double r[32]{},jac[32][6]{};
        if(!residuals(pose,r)) return false;
        *error=0;
        for(int k=0;k<dimensions;++k) {
            double delta[6]{},shifted[32]{};
            const double h=1e-6; delta[k]=h;
            if(!residuals(step(pose,delta,dimensions),shifted)) return false;
            for(size_t row=0;row<2*matches.size();++row) jac[row][k]=(shifted[row]-r[row])/h;
        }
        for(size_t row=0;row<2*matches.size();++row) {
            *error+=r[row]*r[row];
            for(int j=0;j<dimensions;++j) {
                g[j]-=jac[row][j]*r[row];
                for(int k=0;k<dimensions;++k) a[j][k]+=jac[row][j]*jac[row][k];
            }
        }
        return true;
    };
    Rigid pose=prior;
    int dimensions=!translation_only && unique.size()>=3?6:3;
    // Rank-deficient three-point geometry falls back to known orientation, not a damped fake rank.
    double a0[6][6]{},g0[6]{},e0=0,inv0[6][6]{},condition=0;
    if(!normal(pose,dimensions,a0,g0,&e0)) return recovery_reject("refine/projection");
    if(dimensions==6 && !inverse(a0,inv0,6,condition)) dimensions=3;
    for(int iteration=0;iteration<12;++iteration) {
        double a[6][6]{},g[6]{},error=0,inv[6][6]{},delta[6]{};
        if(!normal(pose,dimensions,a,g,&error) || !inverse(a,inv,dimensions,condition)) return recovery_reject("refine/rank-or-projection");
        double length=0;
        for(int j=0;j<dimensions;++j) {
            for(int k=0;k<dimensions;++k) delta[j]+=inv[j][k]*g[k];
            length+=delta[j]*delta[j];
        }
        if(length<1e-16) break;
        bool improved=false;
        for(int attempt=0;attempt<8;++attempt) {
            const auto trial=step(pose,delta,dimensions);
            double r[32]{},cost=0;
            if(residuals(trial,r)) {
                for(size_t row=0;row<2*matches.size();++row) cost+=r[row]*r[row];
                if(cost<error) { pose=trial; improved=true; break; }
            }
            for(double& x:delta) x*=.5;
        }
        if(!improved) break;
    }
    double a[6][6]{},g[6]{},error=0,inv[6][6]{};
    if(!normal(pose,dimensions,a,g,&error) || !inverse(a,inv,dimensions,condition)) return recovery_reject("refine/final-rank-or-projection");
    const double rms=std::sqrt(error/double(matches.size()));
    double shift=0,trace=0;
    for(int i=0;i<3;++i) {
        shift+=std::pow(pose[4*i+3]-prior[4*i+3],2);
        for(int j=0;j<3;++j) trace+=pose[4*i+j]*prior[4*i+j];
    }
    const double turn=std::acos(std::clamp((trace-1)*.5,-1.,1.));
    if(!std::isfinite(rms) || rms>1.5) return recovery_reject("refine/residual");
    if(std::sqrt(shift)>(recovery?.02+dt:std::min(.15,.02+4*dt))) return recovery_reject("refine/speed");
    if(turn>(recovery?.20:.14)) return recovery_reject("refine/orientation");
    double position_std=0,rotation_std=0;
    for(int k=0;k<dimensions;++k) if(!std::isfinite(inv[k][k]) || inv[k][k]<=0) return false;
    for(int k=0;k<3;++k) position_std=std::max(position_std,std::sqrt(inv[k][k]));
    if(dimensions==6) for(int k=3;k<6;++k) rotation_std=std::max(rotation_std,std::sqrt(inv[k][k]));
    // Half-pixel centroid noise floor. Covariance is conditional on the orientation prior for 2 LEDs.
    const double noise=std::max(.5,rms);
    position_std*=noise; rotation_std*=noise*180/3.14159265358979323846;
    if(position_std>.03 || rotation_std>5 || !identity_rigid_valid(pose)) return recovery_reject("refine/covariance-or-rigid");
    fit={}; std::copy(pose.begin(),pose.end(),fit.pose);
    fit.count=int(unique.size()); fit.residual=rms; fit.condition=condition;
    fit.position_std=position_std; fit.rotation_std_deg=dimensions==6?rotation_std:2.; fit.continuation=1;
    for(int id:unique) {
        auto& m=fit.matches[observations++]; m={id,-1,-1};
        for(const auto& match:matches) if(match.led==id) (match.view==0?m.i:m.j)=match.blob;
    }
    observations=int(matches.size()); rotation_refined=dimensions==6;
    return true;
}

// Matches are frozen at the prior. No nearest-neighbour reassignment while fitting.
// Observation count counts camera measurements (1+1 is two, but one unique LED).
inline bool solve_few_led(const NativeOpticalApi& api,const LedModel& model,const OpticalView* views,
                          const Rigid& prior,double dt,OpticalFit& fit,int& observations,
                          bool& rotation_refined,bool recovery=false) {
    using namespace few_led_detail;
    observations=0; rotation_refined=false;
    if(!api.project || !identity_rigid_valid(prior) || !std::isfinite(dt) || dt<=0 ||
       dt>=(recovery?double(prior_hold_ns())*1e-9:.3)) return false;
    for(double x:model) if(!std::isfinite(x)) return false;
    std::vector<Match> matches;

    constexpr double maximum_gate=8.;
    for(int v=0;v<2;++v) {
        if(views[v].count>32 || (views[v].count && !views[v].pixels)) return false;
        double projected[8][2]{}; bool visible[8]{};
        int claim[8]{}; std::fill(claim,claim+8,-1);
        for(int led=0;led<8;++led) visible[led]=project(api,model,views[v],prior,led,projected[led],true);
        double gates[8]{};
        for(int led=0;led<8;++led) if(visible[led]) {
            double separation=1e9;
            for(int other=0;other<8;++other) if(other!=led && visible[other])
                separation=std::min(separation,std::hypot(projected[led][0]-projected[other][0],projected[led][1]-projected[other][1]));
            // Distant foot constellations are ~9 px across. A fixed 8 px gate
            // would contest even exact priors. Narrow it; never choose a branch
            // between overlapping predictions, and drop unresolved (<1 px) LEDs.
            gates[led]=separation<1.?0.:std::min(maximum_gate,.45*separation);
        }
        for(size_t b=0;b<views[v].count;++b) {
            int id=-1,count=0;
            for(int led=0;led<8;++led) if(visible[led] && gates[led]>0 &&
                std::hypot(projected[led][0]-views[v].pixels[2*b],projected[led][1]-views[v].pixels[2*b+1])<=gates[led]) {
                id=led; ++count;
            }
            // More than one predicted LED in the gate: contested, nobody owns it.
            if(count==1) claim[id]=claim[id]==-1?int(b):-2;
        }
        for(int led=0;led<8;++led) if(claim[led]>=0) {
            matches.push_back({v,led,claim[led]});
        }
    }
    return refine_led_matches(api,model,views,prior,dt,matches,fit,observations,rotation_refined,false,recovery);
}

// Acquisition still requires a full fit. Subsequent measured fits renew the lock;
// prediction alone never does. This permits sustained tracking with few LEDs.
class FewLedHistory {
    std::optional<Rigid> full_,previous_,last_;
    int64_t full_t_=0,previous_t_=0,last_t_=0;
  public:
    void accepted(const IdentityCandidate& c) {
        if(!c.pair) return;
        last_=c.world; last_t_=c.stamp;
        if(!c.few_led || c.few_led_rotation) { previous_=full_; previous_t_=full_t_; full_=c.world; full_t_=c.stamp; }
    }
    int64_t anchor() const { return last_t_; }
    int64_t last_stamp() const { return last_t_; }
    std::optional<Rigid> predict(int64_t t,const std::optional<Rigid>& gyro,bool recovery=tracking_recovery_enabled()) const {
        if(!full_ || !last_ || t<=last_t_ || t-last_t_>=(recovery && gyro?prior_hold_ns():300000000)) return {};
        Rigid out=gyro.value_or(*last_);
        // No calibrated gyro support: finite-difference optical body angular velocity.
        if(!gyro && previous_ && full_t_>previous_t_ && full_t_-previous_t_<300000000) {
            double relative[9]{};
            for(int i=0;i<3;++i) for(int j=0;j<3;++j) for(int k=0;k<3;++k)
                relative[3*i+j]+=(*previous_)[4*k+i]*(*full_)[4*k+j];
            const double angle=std::acos(std::clamp((relative[0]+relative[4]+relative[8]-1)*.5,-1.,1.));
            if(angle<.5) {
                const double gain=(angle<1e-8?.5:angle/(2*std::sin(angle)))*double(t-last_t_)/double(full_t_-previous_t_);
                double delta[6]{0,0,0,(relative[7]-relative[5])*gain,(relative[2]-relative[6])*gain,(relative[3]-relative[1])*gain};
                out=few_led_detail::step(*last_,delta,6);
            }
        }
        for(int k=0;k<3;++k) out[4*k+3]=(*last_)[4*k+3];
        return out;
    }
};
IdentityCandidate identity_few_led_candidate(const NativeOpticalApi&,const LedModel&,
    const std::shared_ptr<const IdentityPair>&,int64_t now,const Rigid& prior,int64_t full_anchor,int64_t prior_stamp=0,bool recovery=false);
}
