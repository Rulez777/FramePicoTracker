#include "device_config.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include "guard_evidence.hpp"
#include "optical_worker.hpp"
#include <chrono>
#include <cstdlib>
#include <thread>
#include "optical_compute.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace pico {
namespace {
void require(bool b,const char *why) { if(!b) throw std::invalid_argument(why); }
std::array<double,4> quaternion(const Rigid &m) {
    // Stable largest-diagonal conversion, row-major standing-space rotation.
    std::array<double,4> q{};
    double trace=m[0]+m[5]+m[10];
    if(trace>0) {
        double s=2*std::sqrt(trace+1);
        q={(m[9]-m[6])/s,(m[2]-m[8])/s,(m[4]-m[1])/s,s/4};
    } else {
        int i=m[5]>m[0]?1:0; if(m[10]>m[5*i]) i=2;
        int j=(i+1)%3,k=(i+2)%3;
        double s=2*std::sqrt(1+m[5*i]-m[5*j]-m[5*k]);
        q[i]=s/4; q[j]=(m[4*i+j]+m[4*j+i])/s;
        q[k]=(m[4*i+k]+m[4*k+i])/s; q[3]=(m[4*k+j]-m[4*j+k])/s;
    }
    return q;
}
bool fit_valid(const IdentityCandidate &c) {
    return std::isfinite(c.residual) && c.residual>=0 && c.residual<=2.5 &&
        std::isfinite(c.condition) && c.condition>=0 && c.condition<=1e7 &&
        std::isfinite(c.position_std) && c.position_std>=0 && c.position_std<=.03 &&
        std::isfinite(c.rotation_std_deg) && c.rotation_std_deg>=0 && c.rotation_std_deg<=5;
}
}
OpticalWorker::OpticalWorker(uint32_t chip,LedModel model,NativeOpticalApi api,std::string link,
    const OpticalCameraClock &camera,OpticalExposureSchedule &schedule,OpticalPhaseVerification &phase,
    NativeIdentitySession &identity,std::function<int64_t()> now,
    std::function<bool(const OpticalPoseFrame &,int64_t)> publish)
    :chip_(chip),model_(model),api_(api),link_(std::move(link)),camera_(camera),schedule_(schedule),
     phase_(phase),identity_(identity),now_(std::move(now)),publish_(std::move(publish)) {
    require(chip_==pico::devices::chip() && !link_.empty() && api_.project && api_.detect && api_.solve && now_ && publish_,
        "Explicit first-target worker dependencies required");
}
bool OpticalWorker::drop(const char *reason) { ++rejects_; log_optical_drop(reason); return false; }
bool OpticalWorker::exposure(IdentityPair input,const std::array<CameraMetadata,2> &metadata,bool dark) {
    if(active_)return drop("Optical worker reentered; exposure dropped");
    if(stopped_) return drop("Optical worker stopped; late exposure dropped");
    active_=true;
    struct Leave { bool& active; ~Leave(){active=false;} } leave{active_};
    const auto stamps=camera_.convert_pair(metadata,now_());
    if(!stamps[0]||!stamps[1]){++stale_rejects_;return false;}
    return exposure_at(std::move(input),metadata,stamps,dark);
}
bool OpticalWorker::measured_exposure(IdentityPair input,const std::array<CameraMetadata,2> &metadata,
    const std::function<Rigid(int64_t)> &head,bool dark) {
    require(bool(head),"Measured exposure head dependency unavailable");
    return try_measured_exposure(std::move(input),metadata,
        [&](int64_t stamp)->std::optional<Rigid>{return head(stamp);},dark);
}
bool OpticalWorker::try_measured_exposure(IdentityPair input,const std::array<CameraMetadata,2> &metadata,
    const std::function<std::optional<Rigid>(int64_t)> &head,bool dark) {
    if(active_)return drop("Optical worker reentered; exposure dropped");
    // Live diagnostics: one line per 30 pairs (stderr).
    static uint64_t calls=0; if(++calls%30==1) std::ofstream("/tmp/pico-diag.log",std::ios::app)<<"optical_diag calls="<<calls<<" stale="<<stale_rejects_<<" head_drops="<<head_drops_<<" fits="<<diag_fits_<<" mono_fits="<<diag_mono_fits_<<" stereo_fits="<<diag_stereo_fits_<<" lower_fits="<<diag_lower_fits_<<" prior_rotation_rejects="<<gyro_prior_.rejected<<" prior_reanchors="<<gyro_prior_.reanchors<<" nofit="<<diag_nofit_<<" identity_ok="<<diag_identity_<<" verified="<<diag_verified_<<" phase_state="<<int(phase_.state())<<" last_nofit="<<diag_reason_<<std::endl;
    require(bool(head),"Measured exposure dependencies unavailable");
    if(stopped_) return drop("Optical worker stopped; late exposure dropped");
    active_=true;
    struct Leave { bool& active; ~Leave(){active=false;} } leave{active_};
    const auto stamps=camera_.convert_pair(metadata,now_());
    if(!stamps[0]||!stamps[1]){++stale_rejects_;return false;}
    input.clock_error=camera_.uncertainty();
    for(size_t i=0;i<2;++i) {
        require(input.frames[i].stamp==0,"Measured exposure already converted");
        input.frames[i].stamp=stamps[i];
        const auto pose=head(stamps[i]);
        if(!pose){++head_drops_;log_optical_drop("Measured head bracket missing");return false;}
        input.frames[i].world_from_head=*pose;
    }
    // Lower frames share the exposure clock: convert by their source offset from frames[0].
    for(auto &f:input.lower) {
        f.stamp=stamps[0]+int64_t(std::llround((f.source_raw-metadata[0].raw)*1e9));
        const auto pose=head(f.stamp);
        if(!pose){f.stamp=0;log_optical_drop("Measured head bracket missing for lower frame");continue;}
        f.world_from_head=*pose;
    }
    return exposure_at(std::move(input),metadata,stamps,dark);
}
bool OpticalWorker::exposure_at(IdentityPair input,const std::array<CameraMetadata,2> &metadata,
    const std::array<int64_t,2> &stamps,bool dark) {
    require(!stopped_ && active_,"Optical worker stopped or inactive");
    const auto entered=now_();
    // Diagnostic: wall time of each pair through convert/solve/identity (reported with optical_diag).
    struct Timer { std::chrono::steady_clock::time_point t0=std::chrono::steady_clock::now(); ~Timer(){
        static double sum=0,mx=0; static uint64_t n=0; const double ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-t0).count();
        sum+=ms; mx=std::max(mx,ms); if(++n%30==0){std::ofstream("/tmp/pico-diag.log",std::ios::app)<<"optical_time n="<<n<<" mean_ms="<<sum/30<<" max_ms="<<mx<<std::endl; sum=0; mx=0;} } } timer;
    bool matched=true;
    for(size_t i=0;i<2;++i) matched=matched && input.frames[i].stamp==stamps[i] &&
        std::abs(input.frames[i].exposure_us-metadata[i].exposure*1e6)<1e-6;
    pico::guard_evidence::emit(23,"stereo-time-exposure-age",matched);
    if(!matched) return drop("Decoded exposure metadata mismatch");
    // An unusable lower frame never costs the upper pair: it is logged and left out.
    for(auto it=input.lower.begin();it!=input.lower.end();)
        if(identity_lower_valid(input,*it)) ++it;
        else { log_optical_drop("Lower frame rejected (size/stamp/view)"); it=input.lower.erase(it); }
    auto pair=std::make_shared<const IdentityPair>(std::move(input));
    // Python measured pipeline rejects (drops) a stale pair; it never aborts the session on one.
    auto stale=[&](int64_t now){ for(const auto &f:pair->frames) if(now<f.stamp || now-f.stamp>250000000) { if(++stale_rejects_%30==1) std::cerr<<"optical_stale_rejects="<<stale_rejects_<<" age_ms="<<double(now-f.stamp)/1e6<<'\n';
    return true; }
    return false; };
    if(stale(entered)) return false;
    // Validate before detector access; an invalid pair is logged there and dropped here.
    if(!identity_observation(pair,0,{},entered).stamp) return drop("Identity pair rejected before detection");
    const auto observed=schedule_.observe(metadata[0],entered);
    if(!observed.first){++stale_rejects_;return false;}
    const auto index=observed.second;
    std::optional<IdentityCandidate> candidate;
    const auto gyro_prediction=gyro_prior_.predict(std::min(stamps[0],stamps[1]));
    const auto few_prediction=few_led_enabled()?few_history_.predict(std::min(stamps[0],stamps[1]),gyro_prediction):std::optional<Rigid>{};
    const bool current=compute_([&]{
    // Multi-tracker optics: other trackers' blobs never reach the first target's solve (no mask: unchanged).
    std::shared_ptr<const IdentityBlobMask> mask;
    if(peers_) mask=peers_->before(pair,entered);
    IdentityBlobMask::Scope masked(mask.get());
    OpticalComputeBudget budget;
    if(const char* delay=std::getenv("PICO_TEST_OPTICS_DELAY_MS")) std::this_thread::sleep_for(std::chrono::milliseconds(std::atoi(delay))); // bench-gate: emulates a cluttered view
    try {
        // Last pose as solver prior: it resolves the symmetric LED ring that a blind search calls ambiguous.
        const bool use_prior=prior_world_ && entered-prior_ns_<=300000000;
        const bool recovery=tracking_recovery_enabled() && gyro_prediction && !dark && !recovery_only_;
        candidate=identity_stereo_candidate(api_,model_,pair,entered,gyro_prediction?gyro_prediction->data():
            (use_prior && !tracking_recovery_enabled() && !few_led_enabled()?prior_world_->data():nullptr));
        if(recovery && candidate->pair && !gyro_prior_.consistent(candidate->world,candidate->stamp,gyro_prediction)) {
            log_optical_drop("Recovery fit differs from calibrated gyro orientation"); candidate=IdentityCandidate{};
        }
        if(recovery && !candidate->pair && few_prediction)
            candidate=identity_few_led_candidate(api_,model_,pair,entered,*few_prediction,
                few_history_.anchor(),few_history_.last_stamp(),true);
        if(!candidate->pair && few_prediction && !dark && !recovery_only_) {
            candidate=identity_few_led_candidate(api_,model_,pair,entered,*few_prediction,few_history_.anchor(),few_history_.last_stamp());
        }
        if(!candidate->stereo_verified && !candidate->mono) {
            // No measured fit (e.g. fewer than four blobs): logged, kept as unlit identity evidence.
            diag_reason_=optical_last_drop(); candidate.reset();
        } else {
            const bool valid=fit_valid(*candidate);
            pico::guard_evidence::emit(18,"covariance",valid);
            if(!valid) { ++fit_drops_; log_optical_drop("Optical fit residual/condition/covariance bound"); candidate.reset(); }
        }
    } catch(const std::invalid_argument &e) { diag_reason_=e.what(); /* measured no-fit still feeds dark identity evidence */ }
    // Full body: trackers below the head are mostly outside the upper views. The lower views are solved only
    // when the upper ones gave no admitted fit (upper stereo > upper mono > lower mono, then residual).
    if(!candidate && !pair->lower.empty() && !OpticalComputeBudget::exhausted()) {
        try {
            auto lower=identity_lower_candidate(api_,model_,pair,entered);
            if(lower.pair) {
                if(!fit_valid(lower)) { ++fit_drops_; log_optical_drop("Lower optical fit residual/condition/covariance bound"); }
                else if(!candidate || identity_candidate_better(lower,*candidate)) candidate=std::move(lower);
            }
        } catch(const std::invalid_argument &e) { diag_reason_=e.what(); }
    }
    if(!candidate && !pair->lower.empty() && OpticalComputeBudget::exhausted())
        log_optical_drop("Pair solver budget skipped lower fallback");
    }); // shared state reacquired before identity, phase, or publication
    if(!current){ if(peers_)peers_->discard(); return drop("Optical state changed during solve; pair dropped"); }
    if(candidate && !gyro_prior_.supported_candidate(*candidate,gyro_prediction,
       tracking_recovery_enabled() || few_led_enabled())) {
        log_optical_drop("Optical fix dropped: missing gyro support or accelerometer tilt disagreement"); candidate.reset();
    }
    if(candidate && tracking_recovery_enabled() && gyro_prediction &&
       !gyro_prior_.consistent(candidate->world,candidate->stamp,gyro_prediction)) {
        log_optical_drop("Final optical fit differs from calibrated gyro orientation"); candidate.reset();
    }
    if(candidate && !gyro_prior_.admit(candidate->world,candidate->stamp,gyro_prediction)) candidate.reset();
    else if(!candidate) gyro_prior_.missing();
    // The other trackers of this pair: solved outside the lock, published on every exit below.
    struct PeersPublish { OpticalPeers* peers; const std::function<int64_t()>& now; ~PeersPublish(){ if(peers) peers->publish(now()); } };
    const PeersPublish peers_publish{peers_&&current&&compute_([&]{peers_->solved(pair,candidate,entered,dark);})?peers_:nullptr,now_};
    if(peers_&&!peers_publish.peers) peers_->discard();
    if(candidate) { ++diag_fits_; if(candidate->lower_view>=0) ++diag_lower_fits_; else if(candidate->mono) ++diag_mono_fits_; else ++diag_stereo_fits_; } else ++diag_nofit_;
    const auto solved=now_();
    if(stale(solved)) return false;
    // Post-solve age, both exposures.
    if(!identity_observation(pair,index,candidate,solved).stamp) return drop("Identity pair rejected after solve");
    if (recovery_only_) {
        identity_.recovery_optical(pair, index, candidate, solved);
        return false;
    }
    if(!identity_.optical(pair,index,candidate,solved)) return false;
    ++diag_identity_;
    const bool verified=identity_.verified(chip_,index,link_,solved);
    if(verified) ++diag_verified_;
    phase_.observe(index,bool(candidate),verified||bool(candidate),dark);
    // Owner rule 2026-10-29: publish once the tracker is identified; the phase lock (48 lit of 61) only refines
    // timing and took >60 s with the tracker in hand, holding back every pose.
    // Owner rule 2026-10-30: one tracker on air needs no LED-off identity proof; it held back every pose for most of a run.
    if(!candidate || dark) return false;
    OpticalPoseFrame frame;
    frame.chip=chip_; frame.sequence=sequence_; frame.stamp=candidate->stamp;
    frame.connected=frame.valid=true;
    frame.quality=std::min(1.,candidate->inliers/8.)*std::exp(-candidate->residual/2);
    if(candidate->few_led) {
        // Keep the installed PTP2 ABI: its driver rejects new flag bits. Low quality plus explicit journal.
        frame.quality=std::min(.24,candidate->matched_observations/32.)*std::exp(-candidate->residual/2);
        std::cerr<<"optical_fit kind=few-LED-prior prior="<<(gyro_prediction?"gyro":"optical-rate")<<" stamp="<<candidate->stamp<<" leds="<<candidate->inliers
            <<" blobs="<<candidate->matched_observations<<" rotation_refined="<<candidate->few_led_rotation
            <<" residual_px="<<candidate->residual<<" position_std="<<candidate->position_std<<'\n';
    }
    frame.quaternion=quaternion(candidate->world);
    for(size_t i=0;i<3;++i) frame.position[i]=candidate->world[4*i+3];
    if(last_) {
        if(frame.stamp<=last_->stamp) return false;
        const double dt=double(frame.stamp-last_->stamp)/1e9;
        double distance=0;
        for(size_t i=0;i<3;++i) distance+=std::pow(frame.position[i]-last_->position[i],2);
        distance=std::sqrt(distance);
        // Owner rule 2026-10-29: no speed gate; fast hand motion and mono jumps are published, the driver fuses them.
        if(!(dt>=.25 || distance<=std::max(.05,8*dt))) ++speed_drops_;  // counted only
        if(dt>.005 && dt<.25 && distance/dt<=8) {
            const auto alpha=1-std::exp(-dt/.05);
            for(size_t i=0;i<3;++i) frame.velocity[i]=(1-alpha)*last_->velocity[i]+
                alpha*(frame.position[i]-last_->position[i])/dt;
        }
    }
    const auto sent=now_();
    if(stale(sent)) return false;
    if(!identity_observation(pair,index,candidate,sent).stamp) return drop("Identity pair rejected before publication");
    // Always validate even for injected publication sinks; an invalid pose is dropped.
    try { (void)frame.encode(sent); }
    catch(const std::invalid_argument &) { return drop("Invalid PTP2 pose"); }
    ++sequence_;
    if(!publish_(frame,sent)) return false;
    // measured_stereo_pipeline.py camera(): sigma=max(.003, estimate).
    const double sigma = std::max(.003, candidate->position_std);
    position_covariance_.fill(0);
    velocity_covariance_.fill(0);
    for (size_t i = 0; i < 3; ++i) {
        position_covariance_[4 * i] = sigma * sigma;
        velocity_covariance_[4 * i] = .2 * .2;
    }
    if(candidate->mono) {
        const auto &f=candidate->lower_view>=0?pair->lower[size_t(candidate->lower_view)]:pair->frames[size_t(candidate->mono_view)];
        double ray[3]{},norm=0;
        for(int i=0;i<3;++i) {
            double origin=f.world_from_head[4*i+3];
            for(int j=0;j<3;++j) origin+=f.world_from_head[4*i+j]*f.view.head_from_camera[4*j+3];
            ray[i]=frame.position[i]-origin; norm+=ray[i]*ray[i];
        }
        if(norm>0) for(int i=0;i<3;++i) for(int j=0;j<3;++j)
            position_covariance_[3*i+j]= (i==j?.25*sigma*sigma:0)+.75*sigma*sigma*ray[i]*ray[j]/norm;
    }
    last_=frame;
    prior_world_=candidate->world; prior_ns_=candidate->stamp;
    gyro_prior_.accepted(candidate->world,candidate->stamp);
    if(few_led_enabled()) few_history_.accepted(*candidate);
    if(peers_) peers_->primary_pose(candidate->world,candidate->stamp);
    return true;
}
} // namespace pico

#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_optical_worker, "optical-worker");
PICO_LIVE_IMPLEMENTATION(pico_impl_covariance, "covariance");
