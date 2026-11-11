#pragma once
#include "native_identity.hpp"
#include "optical_compute.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace pico {
// Single calibrated view, unordered measured centroids. All refinement uses the
// runtime projection ABI (including its lens distortion), in camera coordinates.
// Seeded acceptance window around a prior (gyro prediction or last fit); tuned on captures/hand3-ack (2026-10-30).
inline double seeded_turn_deg=8, seeded_shift_m=.08;  // turn 15: 16 of 1648 mono-vs-stereo checks >10 deg/>30 mm; 8: 4 of 1397
inline bool solve_monocular(const NativeOpticalApi &api,const LedModel &model,
                            const OpticalView &view,OpticalFit &fit,const double *prior=nullptr) {
    if(!api.project || view.count<4 || view.count>32) return false;
    using Pose=std::array<double,6>;
    auto rotation=[](const Pose &p,double *r) {
        const double a=p[0],b=p[1],c=p[2],t=std::sqrt(a*a+b*b+c*c);
        const double s=t<1e-8?1:std::sin(t)/t, d=t<1e-8?.5:(1-std::cos(t))/(t*t);
        const double k[9]={0,-c,b,c,0,-a,-b,a,0};
        for(int i=0;i<3;++i) for(int j=0;j<3;++j) {
            r[3*i+j]=(i==j?1.:0.)+s*k[3*i+j];
            for(int l=0;l<3;++l) r[3*i+j]+=d*k[3*i+l]*k[3*l+j];
        }
    };
    auto project=[&](const Pose &p,double *uv) {
        double r[9],xyz[24]; rotation(p,r);
        for(int i=0;i<8;++i) for(int j=0;j<3;++j) {
            xyz[3*i+j]=p[3+j];
            for(int k=0;k<3;++k) xyz[3*i+j]+=r[3*j+k]*model[3*i+k];
        }
        for(int i=0;i<8;++i) if(xyz[3*i+2]<=.01) return false;
        if(api.project(&view.camera,xyz,8,uv)) return false;
        for(int i=0;i<16;++i) if(!std::isfinite(uv[i])) return false;
        return true;
    };
    struct Match { double d; int led,blob; };
    auto assign=[&](const double *uv,std::array<int,8> &ids) {
        std::vector<Match> edges;
        for(int i=0;i<8;++i) for(size_t j=0;j<view.count;++j) {
            const double x=uv[2*i]-view.pixels[2*j],y=uv[2*i+1]-view.pixels[2*j+1];
            edges.push_back({x*x+y*y,i,int(j)});
        }
        std::sort(edges.begin(),edges.end(),[](const Match &a,const Match &b){return a.d<b.d;});
        ids.fill(-1); std::array<bool,32> used{};
        for(const auto &e:edges) if(ids[e.led]<0 && !used[e.blob]) {ids[e.led]=e.blob;used[e.blob]=true;}
    };
    const int n=int(std::min(size_t(8),view.count));
    auto error=[&](const double *uv,const std::array<int,8> &ids) {
        double e=0;
        for(int i=0;i<8;++i) if(ids[i]>=0) for(int c=0;c<2;++c) {
            const double d=uv[2*i+c]-view.pixels[2*ids[i]+c];e+=d*d;
        }
        return e;
    };
    double u=0,v=0,span=0,diameter=0;
    for(size_t i=0;i<view.count;++i) {u+=view.pixels[2*i];v+=view.pixels[2*i+1];
        for(size_t j=0;j<i;++j) span=std::max(span,std::hypot(view.pixels[2*i]-view.pixels[2*j],view.pixels[2*i+1]-view.pixels[2*j+1]));}
    u/=view.count;v/=view.count;
    for(int i=0;i<8;++i) for(int j=0;j<i;++j) {
        double d=0;for(int c=0;c<3;++c) d+=std::pow(model[3*i+c]-model[3*j+c],2);
        diameter=std::max(diameter,std::sqrt(d));
    }
    if(span<3 || diameter<1e-4) return false;
    struct Solution {Pose p;double e;std::array<int,8> ids;};
    std::vector<Solution> solutions;
    constexpr double pi=3.14159265358979323846;
    // Deterministic SO(3) multistart, with several range hypotheses. No prior
    // stereo pose is required, so acquisition works outside the overlap.
    // Levenberg-Marquardt from p with nearest-blob reassignment each step; false only when the budget is exhausted.
    auto refine=[&](Pose &p) {
        double lambda=.001;
        for(int iter=0;iter<45;++iter) {
            if(OpticalComputeBudget::exhausted()){log_optical_drop("Pair solver budget exhausted");return false;}
            double uv[16];if(!project(p,uv)) break;
            std::array<int,8> ids;assign(uv,ids);
            double a[6][7]{},jac[16][6]{};
            for(int k=0;k<6;++k) {
                Pose q=p;const double h=k<3?1e-5:1e-6;q[k]+=h;double shifted[16];
                if(!project(q,shifted)) continue;
                for(int i=0;i<16;++i) jac[i][k]=(shifted[i]-uv[i])/h;
            }
            for(int i=0;i<8;++i) if(ids[i]>=0) for(int c=0;c<2;++c) {
                const int row=2*i+c;const double residual=view.pixels[2*ids[i]+c]-uv[row];
                for(int j=0;j<6;++j) {a[j][6]+=jac[row][j]*residual;
                    for(int k=0;k<6;++k) a[j][k]+=jac[row][j]*jac[row][k];}
            }
            for(int k=0;k<6;++k) a[k][k]+=lambda*(1+a[k][k]);
            bool ok=true;
            for(int k=0;k<6;++k) {
                int pivot=k;for(int j=k+1;j<6;++j) if(std::abs(a[j][k])>std::abs(a[pivot][k])) pivot=j;
                if(std::abs(a[pivot][k])<1e-15) {ok=false;break;}
                for(int j=k;j<7;++j) std::swap(a[k][j],a[pivot][j]);
                double d=a[k][k];for(int j=k;j<7;++j) a[k][j]/=d;
                for(int i=0;i<6;++i) if(i!=k) {d=a[i][k];for(int j=k;j<7;++j) a[i][j]-=d*a[k][j];}
            }
            if(!ok) break;
            Pose q=p;for(int k=0;k<6;++k) q[k]+=a[k][6];
            double trial[16];
            if(project(q,trial) && error(trial,ids)<error(uv,ids)) {p=q;lambda=std::max(1e-8,lambda*.3);}
            else lambda=std::min(1e8,lambda*10);
        }
        return true;
    };
    auto residual=[&](const Pose &p,double &e) {
        double uv[16];if(!project(p,uv)) return false;
        std::array<int,8> ids;assign(uv,ids);e=std::sqrt(error(uv,ids)/n);return true;
    };
    auto emit=[&](const Pose &p,double e,bool seeded) {
        double r[9];rotation(p,r);
        double local[16]={r[0],r[1],r[2],p[3],r[3],r[4],r[5],p[4],r[6],r[7],r[8],p[5],0,0,0,1};
        fit={};for(int i=0;i<4;++i) for(int j=0;j<4;++j) for(int k=0;k<4;++k)
            fit.pose[4*i+j]+=view.head_from_camera[4*i+k]*local[4*k+j];
        fit.count=n;fit.residual=e;fit.condition=std::pow(p[5]/diameter,2); // conservative range/size conditioning
        fit.position_std=std::max(.006,2*p[5]/span*std::max(.5,e));
        fit.rotation_std_deg=std::max(.5,180/pi*std::max(.5,e)/span);
        fit.continuation=seeded?1:0;
        return true;
    };
    // Seeded: the prior pose (WORLD<-tracker, e.g. gyro prediction or the last fit) picks the branch that a blind
    // search reports as ambiguous. Accepted only when it reprojects within 2.5 px and stays near the prior.
    if(prior) {
        const double *w=view.head_from_camera;double rl[9],tl[3],d[3]={prior[3]-w[3],prior[7]-w[7],prior[11]-w[11]};
        for(int i=0;i<3;++i) {tl[i]=w[i]*d[0]+w[4+i]*d[1]+w[8+i]*d[2];
            for(int j=0;j<3;++j) rl[3*i+j]=w[i]*prior[j]+w[4+i]*prior[4+j]+w[8+i]*prior[8+j];}
        const double angle=std::acos(std::clamp((rl[0]+rl[4]+rl[8]-1)/2,-1.,1.));
        if(angle<3.1) {
            const double s=angle<1e-8?.5:angle/(2*std::sin(angle));
            // Full prior first; then the prior rotation with the position from the blobs (a gyro prediction keeps
            // its rotation through a long loss while its position is stale). The rotation alone picks the branch.
            for(int k=0;k<4;++k) {
                if(k==0 && tl[2]<=.05) continue;
                const double z=std::sqrt(view.camera.fx*view.camera.fy)*diameter/span*(k==1?1.:k==2?.65:1.6);
                Pose seed{s*(rl[7]-rl[5]),s*(rl[2]-rl[6]),s*(rl[3]-rl[1]),tl[0],tl[1],tl[2]};
                if(k) {seed[3]=(u-view.camera.cx)/view.camera.fx*z;seed[4]=(v-view.camera.cy)/view.camera.fy*z;seed[5]=z;}
                Pose p=seed;double e=0;
                if(!refine(p)) return false;
                double r0[9],r1[9],trace=0;rotation(seed,r0);rotation(p,r1);for(int i=0;i<9;++i) trace+=r0[i]*r1[i];
                const double turn=std::acos(std::clamp((trace-1)/2,-1.,1.))*180/pi;
                const double shift=std::sqrt(std::pow(p[3]-tl[0],2)+std::pow(p[4]-tl[1],2)+std::pow(p[5]-tl[2],2));
                if(residual(p,e) && e<=2.5 && turn<=seeded_turn_deg && (k || shift<=seeded_shift_m)) return emit(p,e,true);
            }
        }
    }
    for(int axis=0;axis<26;++axis) for(int angle=0;angle<4;++angle) for(double scale:{.65,1.,1.6}) {
        int ax=axis%3-1,ay=(axis/3)%3-1,az=axis/9-1;
        if(ax==0 && ay==0 && az==0) az=1;
        const double norm=std::sqrt(double(ax*ax+ay*ay+az*az));
        double z=std::sqrt(view.camera.fx*view.camera.fy)*diameter/span*scale;
        Pose p{ax/norm*angle*pi/2,ay/norm*angle*pi/2,az/norm*angle*pi/2,
               (u-view.camera.cx)/view.camera.fx*z,(v-view.camera.cy)/view.camera.fy*z,z};
        if(!refine(p)) return false;
        double uv[16];if(!project(p,uv)) continue;
        std::array<int,8> ids;assign(uv,ids);double e=std::sqrt(error(uv,ids)/n);
        if(e<=2.5) solutions.push_back({p,e,ids});
    }
    if(solutions.empty()) return false;
    auto innovation=[&](const Pose& p) {
        if(!prior) return 0.;
        double r[9],trace=0; rotation(p,r);
        for(int i=0;i<3;++i) for(int j=0;j<3;++j) for(int k=0;k<3;++k)
            trace+=prior[4*i+j]*view.head_from_camera[4*i+k]*r[3*k+j];
        return std::acos(std::clamp((trace-1)*.5,-1.,1.));
    };
    std::sort(solutions.begin(),solutions.end(),[&](const Solution &a,const Solution &b){
        if(prior && innovation(a.p)!=innovation(b.p)) return innovation(a.p)<innovation(b.p);
        return a.e<b.e;
    });
    const auto &best=solutions.front();double r[9];rotation(best.p,r);
    for(const auto &s:solutions) if(s.e<=best.e+.15) {
        if(prior && innovation(s.p)>innovation(best.p)+.00873) continue;
        double other[9];rotation(s.p,other);double trace=0,dist=0;
        for(int i=0;i<9;++i) trace+=r[i]*other[i];
        for(int i=3;i<6;++i) dist+=std::pow(s.p[i]-best.p[i],2);
        if(trace<2.97 || dist>.0004) return false; // distinct low-residual solution
    }
    return emit(best.p,best.e,false);
}
}
