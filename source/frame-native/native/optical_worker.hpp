#pragma once
#include "optical_timing.hpp"
#include "optical_prior.hpp"
#include "few_led.hpp"
#include "session_state_guard.hpp"
#include "pose_publication.hpp"
#include <functional>
#include <optional>
#include <string>
#include <stdexcept>

namespace pico {
// 2026-10-31 multi-tracker optics (multi_optics.hpp). Installed only when PICO_PEERS enrolls a secondary chip with an
// LED model; unset keeps every step of the single-tracker exposure path as before.
struct OpticalPeers {
    virtual ~OpticalPeers() = default;
    // Outside the session lock, before the first target's solve: blobs other trackers claim are masked out of it.
    virtual std::shared_ptr<const IdentityBlobMask> before(const std::shared_ptr<const IdentityPair> &, int64_t now) = 0;
    // Outside the session lock, after it: the other trackers of the same pair.
    virtual void solved(const std::shared_ptr<const IdentityPair> &, const std::optional<IdentityCandidate> &primary,
                        int64_t now, bool dark) = 0;
    virtual void primary_pose(const Rigid &world, int64_t stamp) = 0;
    // Session lock held: publish the poses of the other trackers (also when the first target had no pose).
    virtual void publish(int64_t now) = 0;
    virtual void discard() = 0;  // state changed during the solve: the pair's poses are dropped
};
// Serialized decoded-exposure worker. Radio ownership/ticks remain with the
// existing NativeIdentitySession owner; no device or launch entry point.
class OpticalWorker {
    std::function<bool(const std::function<void()>&)> compute_ = [](const std::function<void()>& work){work();return true;};
    OpticalPrior gyro_prior_;
    FewLedHistory few_history_;
    uint32_t chip_;
    LedModel model_;
    NativeOpticalApi api_;
    std::string link_;
    const OpticalCameraClock &camera_;
    OpticalExposureSchedule &schedule_;
    OpticalPhaseVerification &phase_;
    NativeIdentitySession &identity_;
    std::function<int64_t()> now_;
    std::function<bool(const OpticalPoseFrame &,int64_t)> publish_;
    std::optional<OpticalPoseFrame> last_;
    std::optional<Rigid> prior_world_;
    int64_t prior_ns_=0;
    std::array<double, 9> position_covariance_{};
    std::array<double, 9> velocity_covariance_{};
    bool recovery_only_ = false;
    uint64_t sequence_ = 0;
    uint64_t stale_rejects_ = 0;
    uint64_t head_drops_ = 0;
    uint64_t speed_drops_ = 0, fit_drops_ = 0, rejects_ = 0;
    uint64_t diag_mono_fits_ = 0, diag_stereo_fits_ = 0, diag_lower_fits_ = 0;
    uint64_t diag_fits_ = 0, diag_nofit_ = 0, diag_identity_ = 0, diag_verified_ = 0;  // live diagnostics
    std::string diag_reason_;
    bool stopped_ = false, active_ = false;
    OpticalPeers *peers_ = nullptr;
    bool exposure_at(IdentityPair, const std::array<CameraMetadata,2> &,
                     const std::array<int64_t,2> &, bool);
    bool drop(const char *reason); // log, count, return false: data checks never throw
  public:
    OpticalWorker(uint32_t, LedModel, NativeOpticalApi, std::string,
        const OpticalCameraClock &, OpticalExposureSchedule &,
        OpticalPhaseVerification &, NativeIdentitySession &,
        std::function<int64_t()>, std::function<bool(const OpticalPoseFrame &,int64_t)>);
    bool prior_calibration(const std::string& path) { return gyro_prior_.load(path,chip_); }
    void optical_peers(OpticalPeers *peers) { peers_ = peers; }  // must outlive the worker's exposures
    void prior_imu(const std::array<uint8_t,32>& bytes) { gyro_prior_.wire(bytes.data(),bytes.size()); }
    // Owner releases all recursive state acquisitions only around immutable pair computation.
    void session_compute(std::recursive_mutex& shared,std::function<uint64_t()> epoch,
                         std::function<bool()> allowed){
        compute_=[&shared,epoch=std::move(epoch),allowed=std::move(allowed)](const std::function<void()>& work){
            const auto before=epoch();
            {SessionStateGuard::UnlockedIO unlocked(shared);work();}
            return before==epoch() && allowed();
        };
    }
    const std::array<double, 9>& position_covariance() const { return position_covariance_; }
    const std::array<double, 9>& velocity_covariance() const { return velocity_covariance_; }
    uint64_t stale_drops() const { return stale_rejects_; }
    uint64_t head_drops() const { return head_drops_; }
    uint64_t speed_drops() const { return speed_drops_; }
    uint64_t prior_rotation_rejects() const { return gyro_prior_.rejected; }
    uint64_t prior_reanchors() const { return gyro_prior_.reanchors; }
    uint64_t fit_drops() const { return fit_drops_; }
    uint64_t rejected_drops() const { return rejects_; } // metadata/identity/encode/after-stop drops
    void begin_led_recovery() { recovery_only_ = true; identity_.begin_recovery(); }
    // Owns a snapshot; candidate, identity and publication use that exact pair.
    // Returns true only after a valid pose reaches the publication callback.
    bool exposure(IdentityPair, const std::array<CameraMetadata,2> &, bool dark_frame = false);
    // Convert once, then bind the measured head at those exact source stamps.
    // A second RAW bridge sample can differ by nanoseconds and must not replace
    // the conversion used for the original pixel pair.
    bool measured_exposure(IdentityPair, const std::array<CameraMetadata,2> &,
        const std::function<Rigid(int64_t)> &head, bool dark_frame = false);
    bool try_measured_exposure(IdentityPair, const std::array<CameraMetadata,2> &,
        const std::function<std::optional<Rigid>(int64_t)> &head, bool dark_frame = false);
    // Serialized owner-thread worker: no background work survives this barrier.
    // Refuse reentrant shutdown rather than releasing dependencies mid-solve.
    void stop_join(){stopped_=true;if(active_)throw std::runtime_error("Optical worker still active; retain dependencies");}
};
} // namespace pico
