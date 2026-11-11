#include "guard_evidence.hpp"
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <utility>
#include "optical_timing.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#ifndef _WIN32
#include <time.h>
#endif

namespace pico {
namespace {
void require(bool b, const char *s) { if(!b) throw std::invalid_argument(s); }
// Keep arithmetic well inside int64, including subsequent differences/additions.
constexpr int64_t bound = INT64_MAX/8;
void valid_time(int64_t t) { require(t>0 && t<bound,"Invalid optical clock value"); }
bool raw_valid(double v) { return std::isfinite(v) && v>0 && v<double(bound)/1e9; }
bool short_exposure(double e) { return std::isfinite(e) && e>0 && e<.001; }
int64_t raw_ns(double v) {
    require(std::isfinite(v) && v>0 && v<double(bound)/1e9,"Invalid RAW timestamp");
    return nearest_even(v*1e9);
}
RawBridge sample_bridge() {
#ifdef _WIN32
    throw std::runtime_error("RAW bridge requires Linux or explicit injected samples");
#else
    auto read=[](clockid_t id) {
        timespec t{};
        if(clock_gettime(id,&t)) throw std::runtime_error("Optical clock sampling failed");
    return int64_t(t.tv_sec)*1000000000+t.tv_nsec;
    };
    const auto a=read(CLOCK_MONOTONIC_RAW), m=read(CLOCK_MONOTONIC), b=read(CLOCK_MONOTONIC_RAW);
    return {a,m,b};
#endif
}
}
OpticalCameraClock::OpticalCameraClock(CameraClockEvidence e,const std::string &boot,
    std::function<RawBridge()> bridge):evidence_(std::move(e)),bridge_(std::move(bridge)) {
    // Unproven evidence is logged; conversion continues and stale results are dropped per pair.
    const bool proven=evidence_.version==1 && !boot.empty() && evidence_.boot==boot && evidence_.verified &&
        !evidence_.method.empty() && (evidence_.domain=="CLOCK_MONOTONIC_RAW" ||
        evidence_.domain=="CLOCK_MONOTONIC") && evidence_.uncertainty>=0 && evidence_.uncertainty<=500000;
    offset_valid_=evidence_.offset>-bound && evidence_.offset<bound;
    if(!proven || !offset_valid_) {
        log_optical_drop("Current-boot camera clock evidence not proven; continuing");
        std::cerr<<"camera_clock_evidence version="<<evidence_.version<<" boot_match="<<(evidence_.boot==boot)
                 <<" verified="<<evidence_.verified<<" domain="<<evidence_.domain<<" method="<<evidence_.method
                 <<" uncertainty="<<evidence_.uncertainty<<" offset="<<evidence_.offset<<'\n';
    }
    pico::guard_evidence::emit(21,"boot-clock",proven && offset_valid_);
    if(!bridge_) bridge_=sample_bridge;
}
int64_t OpticalCameraClock::convert(CameraMetadata m,int64_t now) const {
    valid_time(now);
    if(!short_exposure(m.exposure)) { log_optical_drop("Short exposure required"); return 0; }
    if(!raw_valid(m.raw)) { log_optical_drop("Invalid RAW timestamp"); return 0; }
    if(!offset_valid_) { log_optical_drop("Camera clock offset outside arithmetic bound"); return 0; }
    int64_t stamp=raw_ns(m.raw)+evidence_.offset;
    if(evidence_.domain=="CLOCK_MONOTONIC_RAW") {
        auto b=bridge_(); valid_time(b.before); valid_time(b.mono); valid_time(b.after);
        if(!(evidence_.method=="xrtap-direct" || (b.after>=b.before && b.after-b.before<=2*evidence_.uncertainty))) {
            log_optical_drop("RAW bridge uncertainty exceeded"); return 0;
        }
        stamp+=b.mono-(b.before+(b.after-b.before)/2);
    }
    if(stamp<=0 || stamp>now || now-stamp>250000000)return 0;
    return stamp;
}
std::array<int64_t,2> OpticalCameraClock::convert_pair(const std::array<CameraMetadata,2> &m,int64_t now) const {
    auto reject=[](const char *reason)->std::array<int64_t,2> {
        if(reason) log_optical_drop(reason);
        pico::guard_evidence::emit(23,"stereo-time-exposure-age",false);
        return {0,0};
    };
    if(!raw_valid(m[0].raw) || !raw_valid(m[1].raw)) return reject("Invalid RAW timestamp");
    auto a=raw_ns(m[0].raw), b=raw_ns(m[1].raw);
    if(std::abs(a-b)>(evidence_.method=="xrtap-direct"?1000000:2000)) return reject("Stereo skew exceeds source tolerance");
    if(!short_exposure(m[0].exposure) || !short_exposure(m[1].exposure)) return reject("Actual short fresh stereo exposures required");
    auto first=convert(m[0],now);
    if(!first) return reject(nullptr);
    std::array<int64_t,2> stamps{first,first+b-a};
    for(auto stamp:stamps)if(stamp<=0||stamp>now||now-stamp>250000000) return reject(nullptr);
    pico::guard_evidence::emit(23,"stereo-time-exposure-age",true);
    return stamps;
}
OpticalExposureSchedule::OpticalExposureSchedule(const OpticalCameraClock &c,const RoyClock &r,int phase)
    :camera_(c),roy_(r),phase_(phase) { require(phase>=0 && phase<533333,"Explicit phase required"); }
int64_t OpticalExposureSchedule::index_unlocked(double raw) const {
    (void)raw_ns(raw); require(!anchors_.empty(),"Exposure origin missing");
    auto best=anchors_.begin();
    for(auto i=anchors_.begin();i!=anchors_.end();++i)
        if(std::abs(i->first-raw)<std::abs(best->first-raw)) best=i;
    const auto delta=(raw-best->first)/period_;
    require(std::abs(delta)<double(bound),"Frame index overflow");
    return best->second+nearest_even(delta);
}
std::pair<int64_t,int64_t> OpticalExposureSchedule::observe(CameraMetadata m,int64_t now) {
    if(!raw_valid(m.raw)) {
        log_optical_drop("Invalid RAW timestamp; schedule sample dropped");
        std::lock_guard<std::mutex> lock(mutex_); return {0,last_index_};
    }
    std::optional<int64_t> stamp;
    try { auto value=camera_.convert(m,now); if(value)stamp=value; } catch(const std::invalid_argument &) {}
    std::lock_guard<std::mutex> lock(mutex_);
    if(last_raw_ && m.raw<=*last_raw_) {
        return {stamp.value_or(0),index_unlocked(m.raw)};
    }
    int64_t index=0;
    if(last_raw_) {
        double delta=m.raw-*last_raw_;
        { // Log only: camera frame spacing outside the 33.3 ms the tracker LED timer (533333 ticks at 16 MHz) is built for.
            static int64_t logged=0;
            if(!(delta>.032 && delta<.035) && delta<.1 && now-logged>=5000000000LL){ logged=now;
                std::ofstream("/tmp/pico-diag.log",std::ios::app)<<"camera_period_mismatch frame_ms="<<delta*1e3<<" expected_ms=33.333 (no exposure plan, optics cannot lock)"<<std::endl; }
        }
        if(delta>.032 && delta<.035) {
            intervals_.push_back(delta); if(intervals_.size()>60) intervals_.pop_front();
            double sum=0; for(auto v:intervals_) sum+=v;
            period_=sum/double(intervals_.size());
        }
        index=last_index_+std::max<int64_t>(1,nearest_even(delta/period_));
        require(index<bound,"Frame index overflow");
    }
    last_raw_=m.raw; last_index_=index; anchors_.push_back({m.raw,index});
    if(anchors_.size()>300) anchors_.pop_front();
    if(stamp) { metadata_=m; exposures_.push_back(*stamp); if(exposures_.size()>12) exposures_.pop_front(); }
    return {stamp.value_or(0),index};
}
int64_t OpticalExposureSchedule::frame_index(double raw) const {
    std::lock_guard<std::mutex> lock(mutex_); return index_unlocked(raw);
}
std::vector<int64_t> OpticalExposureSchedule::exposure_times() const {
    std::lock_guard<std::mutex> lock(mutex_); return {exposures_.begin(),exposures_.end()};
}
std::optional<ExposurePlan> OpticalExposureSchedule::prepare(int pos,int count,int64_t now,int adjust,uint32_t key) const {
    valid_time(now); require(count>0 && count<=3 && pos>=0 && pos<count,"Invalid active optical slot");
    std::lock_guard<std::mutex> lock(mutex_);
    // Log only: which input the exposure plan is waiting for (at most one line per second).
    auto waiting=[&](const char* what,double value){
        static int64_t logged=0;
        if(now-logged<1000000000) return;
        logged=now;
        std::ofstream("/tmp/pico-diag.log",std::ios::app)<<"plan_wait "<<what<<" value="<<value
            <<" roy_fit="<<bool(roy_.fit)<<" last_raw="<<bool(last_raw_)<<" intervals="<<intervals_.size()
            <<" metadata="<<bool(metadata_)<<std::endl;
    };
    if(!roy_.fit || !last_raw_ || intervals_.size()<6 || !metadata_) { waiting("inputs",0); return {}; }
    if(!(std::isfinite(roy_.fit->slope) && std::abs(roy_.fit->slope-15.625)<=.003)) {
        log_optical_drop("Roy DWT not measured 64MHz; no exposure plan this tick");
        waiting("roy_slope",roy_.fit->slope);
        return {};
    }
    auto exposure=camera_.convert(*metadata_,now);
    if(!exposure){ waiting("camera_clock",0); return {}; }
    auto ahead=std::max<int64_t>(1,int64_t(std::ceil(double(now-exposure)/1e9/period_)));
    auto frame=last_index_+ahead;
    while(frame%count!=pos) ++frame;
    auto target=exposure+nearest_even(double(frame-last_index_)*period_*1e9);
    auto light=lights_.find(key); const bool due=light==lights_.end() || light->second!=count;
    auto sync=sync_.find(key); const int64_t last=sync==sync_.end()?0:sync->second;
    if(!due && now-last<800000000) return {};
    if(target-now>200000000) { waiting("target_far_ms",double(target-now)/1e6); return {}; }
    ExposurePlan p; p.position=pos; p.count=count; p.key=key; p.light_due=due;
    p.own_frame=frame; p.light_target=target;
    p.phase_target=due?target+nearest_even(count*period_*1e9):target;
    // 2026-10-31 phase re-tuning after the fast SWD: PICO_PHASE_OFFSET (ticks, 16 MHz) shifts the fixed phase;
    // PICO_PHASE_SWEEP="from,step,dwell_s" steps the offset in-session (logged to /tmp/pico-diag.log).
    {
        static const long fixed=std::getenv("PICO_PHASE_OFFSET")?std::atol(std::getenv("PICO_PHASE_OFFSET")):0;
        static long from=0,step=0; static double dwell=0; static int64_t t0=0; static long last=-1;
        static const bool sweep=[]{const char*s=std::getenv("PICO_PHASE_SWEEP");return s&&std::sscanf(s,"%ld,%ld,%lf",&from,&step,&dwell)==3&&dwell>0;}();
        long off=fixed;
        if(sweep){ if(!t0) t0=now;
        off+=from+step*long(double(now-t0)/1e9/dwell); }
        if(off!=last){ last=off; std::ofstream("/tmp/pico-diag.log",std::ios::app)<<"phase_offset ticks="<<off<<" t="<<now<<std::endl; }
        adjust+=off;
    }
    auto ticks=(int64_t(phase_)+adjust+nearest_even(double(now-exposure)*.016))%533333;
    p.ticks=int((ticks+533333)%533333);
    try {
        p.reference=roy_.cycles_at(now);
        p.phase_window={roy_.cycles_at(p.phase_target-12000000),roy_.cycles_at(p.phase_target-4000000)};
        p.light_window={roy_.cycles_at(target+1000000),roy_.cycles_at(target+12000000)};
    } catch(const std::exception &) {
        log_optical_drop("Roy clock projection unavailable; no exposure plan this tick");
        return {};
    }
    return p;
}
void OpticalExposureSchedule::light_ack(uint32_t key,int count) {
    require(count>0 && count<=3,"Invalid light count"); std::lock_guard<std::mutex> lock(mutex_); lights_[key]=count;
}
void OpticalExposureSchedule::phase_ack(uint32_t key,int64_t now) {
    valid_time(now); std::lock_guard<std::mutex> lock(mutex_); sync_[key]=now;
}
void OpticalExposureSchedule::reset_peer(uint32_t key) {
    std::lock_guard<std::mutex> lock(mutex_); lights_.erase(key); sync_.erase(key);
}
OpticalPhaseVerification::OpticalPhaseVerification(int phase,std::vector<uint32_t> active,bool all)
    :phase_(phase),require_all_(all),required_(active.begin(),active.end()) {
    require(phase>=0 && phase<533333 && !active.empty() && active.size()<=3 &&
        required_.size()==active.size() && !required_.count(0),"Explicit phase and unique active peers required");
}
void OpticalPhaseVerification::ack(uint32_t chip,int phase) {
    std::lock_guard<std::mutex> lock(mutex_);
    if(phase!=phase_ || !required_.count(chip)) return;
    acked_.insert(chip); if(!require_all_ || acked_==required_) applied_=true;
}
void OpticalPhaseVerification::observe(int64_t index,bool lit,bool identity,bool dark) {
    std::lock_guard<std::mutex> lock(mutex_);
    if(!dark) window_.observe(index,lit,identity,applied_);
}
IdentityPhaseWindow::State OpticalPhaseVerification::state() const {
    std::lock_guard<std::mutex> lock(mutex_); return window_.state;
}
} // namespace pico


#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_optical_timing, "optical-timing");

