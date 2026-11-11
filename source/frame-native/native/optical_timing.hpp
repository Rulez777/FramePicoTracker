#pragma once
#include <array>
#include <cstdint>
#include <deque>
#include <functional>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "native_identity.hpp"
#include <mutex>

namespace pico {
// Values are sampled on this host, around each conversion, never startup offsets.
struct RawBridge { int64_t before, mono, after; };
struct CameraMetadata { double raw, exposure; };
struct CameraClockEvidence {
    int version = 0;
    std::string boot, domain, method;
    bool verified = false;
    int64_t offset = 0, uncertainty = 0;
};
class OpticalCameraClock {
    CameraClockEvidence evidence_;
    std::function<RawBridge()> bridge_;
    bool offset_valid_ = true;
  public:
    // Unproven evidence is logged, never refused; invalid samples convert to 0.
    OpticalCameraClock(CameraClockEvidence, const std::string &current_boot,
                       std::function<RawBridge()> = {});
    int64_t convert(CameraMetadata, int64_t now) const;
    std::array<int64_t,2> convert_pair(const std::array<CameraMetadata,2> &, int64_t now) const;
    int64_t uncertainty() const { return evidence_.uncertainty; }
};
struct ExposurePlan {
    int ticks = 0, position = 0, count = 0;
    uint32_t reference = 0;
    std::array<uint32_t,2> phase_window{}, light_window{};
    int64_t own_frame = 0, light_target = 0, phase_target = 0;
    uint32_t key = 0;
    bool light_due = false;
};
// No command dispatch here. The radio owner alone commits acknowledged state.
// Stale observations count toward frame indices but never replace timing metadata.
class OpticalExposureSchedule {
    const OpticalCameraClock &camera_;
    const RoyClock &roy_; // caller serializes RoyClock access with radio sampling
    mutable std::mutex mutex_;
    int phase_;
    std::optional<double> last_raw_;
    int64_t last_index_ = 0;
    double period_ = 1./30;
    std::deque<double> intervals_;
    std::deque<std::pair<double,int64_t>> anchors_;
    std::deque<int64_t> exposures_;
    std::optional<CameraMetadata> metadata_;
    std::map<uint32_t,int> lights_;
    std::map<uint32_t,int64_t> sync_;
    int64_t index_unlocked(double) const;
  public:
    OpticalExposureSchedule(const OpticalCameraClock &, const RoyClock &, int phase);
    std::pair<int64_t,int64_t> observe(CameraMetadata, int64_t now);
    int64_t frame_index(double raw) const;
    std::vector<int64_t> exposure_times() const;
    std::optional<ExposurePlan> prepare(int position, int count, int64_t now,
                                      int phase_adjust, uint32_t stable_key) const;
    void light_ack(uint32_t key, int count);
    void phase_ack(uint32_t key, int64_t now);
    void reset_peer(uint32_t key);
};
// Exact fixed-phase behavior of current phase_search.py; ACK is phase/peer bound.
class OpticalPhaseVerification {
    mutable std::mutex mutex_;
    int phase_;
    bool require_all_, applied_ = false;
    std::set<uint32_t> required_, acked_;
    IdentityPhaseWindow window_;
  public:
    OpticalPhaseVerification(int phase, std::vector<uint32_t> active, bool require_all);
    void ack(uint32_t chip, int phase);
    void observe(int64_t index, bool lit, bool identity_verified, bool dark_frame = false);
    IdentityPhaseWindow::State state() const;
};
} // namespace pico


