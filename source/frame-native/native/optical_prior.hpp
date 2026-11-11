#pragma once
#include "device_config.hpp"
#include "native_identity.hpp"
#include "tracking_recovery.hpp"
#include "recovery_diagnostics.hpp"
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <deque>
#include <fstream>
#include <mutex>
#include <optional>
#include <sstream>

namespace pico {
// Exposure-time, body-rate propagation. Translation is held, never coasted.
// The wire timestamp is MONOTONIC source time, not UDP receive time.
class OpticalPrior {
    struct Rate { int64_t t; std::array<double,3> w, a; };
    mutable std::mutex mutex_;
    std::deque<Rate> rates_;
    double r_[9]{}, bias_[3]{}, scale_=0, offset_=0;
    uint32_t chip_=0;
    std::optional<Rigid> anchor_, suspect_;
    int64_t anchor_t_=0, suspect_t_=0;
    unsigned agrees_=0;
    bool retain_all_=false;
    static Rigid relative(const Rigid& a,const Rigid& b) {
        auto o=identity_rigid();
        for(int i=0;i<3;++i)for(int j=0;j<3;++j){o[4*i+j]=0;for(int k=0;k<3;++k)o[4*i+j]+=a[4*k+i]*b[4*k+j];}
        return o;
    }
    std::optional<Rigid> propagate(Rigid out,int64_t from,int64_t to,bool recovery) const {
        if(to<from || to-from>(recovery?prior_hold_ns():1000000000) || rates_.empty())return {};
        auto it=std::upper_bound(rates_.begin(),rates_.end(),from,[](int64_t t,const Rate& s){return t<s.t;});
        if(it==rates_.begin()){recovery_reject("gyro/no-left-support");return {};}
        --it;
        int64_t t=from;
        while(t<to){
            auto next=std::next(it);
            const int64_t end=next==rates_.end()?to:std::min(to,next->t);
            // Missing support invalidates only this prior; never stops the run.
            if(end-it->t>60000000){recovery_reject(next==rates_.end()?"gyro/late-tail":"gyro/interior-hole");return {};}
            const double dt=double(end-t)*1e-9;
            double v[3],a=0;for(int k=0;k<3;++k){v[k]=it->w[k]*dt;a+=v[k]*v[k];}
            a=std::sqrt(a);
            const double s=a<1e-10?1:std::sin(a)/a, c=a<1e-10?.5:(1-std::cos(a))/(a*a);
            const double K[9]{0,-v[2],v[1],v[2],0,-v[0],-v[1],v[0],0};
            double d[9];
            for(int i=0;i<3;++i)for(int j=0;j<3;++j){d[3*i+j]=(i==j?1.:0.)+s*K[3*i+j];for(int k=0;k<3;++k)d[3*i+j]+=c*K[3*i+k]*K[3*k+j];}
            Rigid old=out;
            for(int i=0;i<3;++i)for(int j=0;j<3;++j){out[4*i+j]=0;for(int k=0;k<3;++k)out[4*i+j]+=old[4*i+k]*d[3*k+j];}
            t=end;if(t<to)it=next;
        }
        return out;
    }
  public:
    explicit OpticalPrior(bool retain_all=false):retain_all_(retain_all) {}
    struct Integral { Rigid rotation=identity_rigid(); int64_t covered_ns=0; };
    // Hindsight only: integrate bracketed, <=12 ms sample intervals. Holes and
    // unbracketed tails contribute no rotation and no coverage, never interpolation.
    Integral integral(int64_t from,int64_t to) const {
        std::lock_guard<std::mutex> lock(mutex_);
        Integral result;
        if(to<=from) return result;
        auto first=std::upper_bound(rates_.begin(),rates_.end(),from,[](int64_t t,const Rate& s){return t<s.t;});
        if(first!=rates_.begin()) --first;
        for(auto it=first;it!=rates_.end();++it) {
            auto next=std::next(it);
            if(next==rates_.end() || it->t>=to) break;
            const int64_t start=std::max(from,it->t),end=std::min(to,next->t);
            // PICO_HINDSIGHT_MAX_GAP_MS (2026-11-02): old captures carry ~82 IMU samples/s with 15-30 ms holes.
            static const int64_t max_gap=[]{const char* v=std::getenv("PICO_HINDSIGHT_MAX_GAP_MS");return v?int64_t(std::atof(v)*1e6):int64_t(12000000);}();
            if(end<=start || next->t-it->t>max_gap) continue;
            // Reuse exactly the causal body-rate exponential, without its horizon.
            const auto step=propagate(result.rotation,start,end,true);
            if(step) { result.rotation=*step; result.covered_ns+=end-start; }
        }
        return result;
    }
    // Same calibrated body basis as driver fusion; specific force points UP in
    // Standing (+Y). Only fresh slow-motion, near-1g observations constrain tilt.
    std::optional<double> tilt_deg(const Rigid& pose,int64_t stamp) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it=std::upper_bound(rates_.begin(),rates_.end(),stamp,[](int64_t t,const Rate& s){return t<s.t;});
        if(it==rates_.begin()) return {};
        --it;
        if(stamp-it->t>60000000) return {};
        double an=0,wn=0,up=0;
        for(int k=0;k<3;++k) { an+=it->a[k]*it->a[k]; wn+=it->w[k]*it->w[k]; up+=pose[4+k]*it->a[k]; }
        an=std::sqrt(an);
        // PTI1 has no sensor ID. This task admits only the measured first target,
        // A1B2C3D4 / ICM-42686 (0x44), 1024 LSB/g. Never guess another chip's scale.
        if(chip_!=pico::devices::chip() || wn>.25 || !(std::abs(an/1024.-1)<.15)) return {};
        return std::acos(std::clamp(up/an,-1.,1.))*180/std::acos(-1.);
    }
    bool supported_candidate(const IdentityCandidate& c,const std::optional<Rigid>& prediction,bool enabled) const {
        if(!enabled) return true;
        if(c.seeded || c.few_led || c.prior_continuation) {
            if(prediction) return true; // predict enforces sample support + anchor age bound
            recovery_reject("final/missing-gyro"); return false;
        }
        if(!prediction) {
            const auto tilt=tilt_deg(c.world,c.stamp);
            if(tilt && *tilt>15.) { recovery_reject("final/accelerometer-tilt"); return false; }
        }
        return true;
    }
    uint64_t imu=0, rejected=0, reanchors=0;
    static double consistency_bound_deg(double age_seconds) {
        return std::min(30.,10.+2.*std::max(0.,age_seconds));
    }
    double consistency_bound_deg(int64_t stamp) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return consistency_bound_deg(double(std::max(int64_t(0),stamp-anchor_t_))*1e-9);
    }
    bool consistent(const Rigid& fit,int64_t stamp,const std::optional<Rigid>& prediction) const {
        return !prediction || angle(*prediction,fit)<=consistency_bound_deg(stamp);
    }
    static double angle(const Rigid& a,const Rigid& b) {
        double trace=0;for(int i=0;i<3;++i)for(int k=0;k<3;++k)trace+=a[4*k+i]*b[4*k+i];
        return std::acos(std::clamp((trace-1)/2,-1.,1.))*180/std::acos(-1.);
    }
    bool load(const std::string& path,uint32_t chip) {
        std::lock_guard<std::mutex> lock(mutex_);
        chip_=0;rates_.clear();anchor_.reset();missing();
        std::ifstream f(path);std::string line;
        while(std::getline(f,line)){
            std::istringstream in(line);uint32_t id=0;double r[9]{},b[3]{},s=0,o=0;
            if(!(in>>std::hex>>id>>std::dec)||(id!=chip&&id!=0))continue;  // 00000000 = generic line for any tracker (keep it last)
            bool ok=true;for(double& x:r)ok=ok&&bool(in>>x);
            ok=ok&&bool(in>>s);
            for(double& x:b)ok=ok&&bool(in>>x);
            ok=ok&&bool(in>>o);
            if(!ok||!std::isfinite(s)||s<=0||!std::isfinite(o)||std::abs(o)>1e9)break;
            for(double x:r)ok=ok&&std::isfinite(x);
            for(double x:b)ok=ok&&std::isfinite(x);
            if(!ok)break;
            std::copy(r,r+9,r_);std::copy(b,b+3,bias_);scale_=s;offset_=o;chip_=chip;rates_.clear();anchor_.reset();suspect_.reset();agrees_=0;return true;
        }
        log_optical_drop("Gyro prior calibration unavailable/invalid; disabled");return false;
    }
    void wire(const uint8_t* bytes,size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        if(!chip_||size!=32||std::memcmp(bytes,"PTI1",4))return;
        auto get=[&](size_t p,size_t n){uint64_t x=0;for(size_t i=0;i<n;++i)x|=uint64_t(bytes[p+i])<<(8*i);
        return x;};
        if(get(4,4)!=chip_)return;
        const double stamp=double(get(12,8))+offset_;
        if(!std::isfinite(stamp)||stamp<=0||stamp>=double(INT64_MAX))return;
        Rate rate{int64_t(std::llround(stamp)),{}, {}};double g[3];
        for(int k=0;k<3;++k)g[k]=int16_t(get(20+2*k,2))-bias_[k];
        for(int i=0;i<3;++i)for(int j=0;j<3;++j)rate.w[i]+=scale_*r_[3*i+j]*g[j];
        for(int i=0;i<3;++i)for(int j=0;j<3;++j)rate.a[i]+=r_[3*i+j]*int16_t(get(26+2*j,2));
        auto it=std::lower_bound(rates_.begin(),rates_.end(),rate.t,[](const Rate& s,int64_t t){return s.t<t;});
        if(it!=rates_.end()&&it->t==rate.t)return;
        rates_.insert(it,rate);++imu;
        while(!retain_all_&&!rates_.empty()&&rates_.back().t-rates_.front().t>std::max(int64_t(3000000000LL),prior_hold_ns()+100000000))rates_.pop_front();
    }
    std::optional<Rigid> predict(int64_t t,bool recovery=tracking_recovery_enabled()) const {
        std::lock_guard<std::mutex> lock(mutex_);
        if(!anchor_) { recovery_reject("gyro/no-anchor"); return {}; }
        auto out=propagate(*anchor_,anchor_t_,t,recovery);
        if(!out) recovery_reject(t-anchor_t_>(recovery?prior_hold_ns():1000000000)?"gyro/expired":"gyro/unsupported");
        return out;
    }
    void missing() { suspect_.reset();agrees_=0; }
    bool admit(const Rigid& fit,int64_t t,const std::optional<Rigid>& prior) {
        if(prior&&angle(*prior,fit)>25){
            const auto correction=relative(*prior,fit);
            if(suspect_&&t>suspect_t_&&t-suspect_t_<=200000000&&angle(*suspect_,correction)<=10)++agrees_;
            else agrees_=1;
            suspect_=correction;suspect_t_=t;
            if(agrees_<5){++rejected;log_optical_drop("Optical rotation differs from gyro prior by >25 degrees");return false;}
            ++reanchors;
        }
        missing();return true;
    }
    void accepted(const Rigid& pose,int64_t t) {
        std::lock_guard<std::mutex> lock(mutex_);anchor_=pose;anchor_t_=t;
    }
};
} // namespace pico
