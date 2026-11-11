#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <vector>
#include "native25.hpp"
#include <array>
#include <atomic>
#include <memory>

namespace pico {
// Exact ctypes ABI from pico_optics_py.py and detector_py.py. The two
// pico_detect exports have DIFFERENT ABIs: only the uint8 detector is used here.
struct OpticalCamera { double fx, fy, cx, cy, k[4]; int width, height; };
struct OpticalView { OpticalCamera camera; double head_from_camera[16]; size_t count; const double *pixels; };
struct OpticalMatch { int led, i, j; };
struct OpticalFit {
    double pose[16]; int count; OpticalMatch matches[8];
    double residual, condition, position_std, rotation_std_deg;
    int triangulation_candidates, hypotheses, continuation;
};
static_assert(sizeof(OpticalCamera)==72 && offsetof(OpticalFit,matches)==132 &&
              offsetof(OpticalFit,residual)==232 && sizeof(OpticalFit)==280,
              "Native optics ctypes ABI layout changed");
struct NativeOpticalApi {
    using Project = int (*)(const OpticalCamera *, const double *, size_t, double *);
    using Solve = int (*)(const double *, const OpticalView *, const double *, OpticalFit *);
    // Detector ABI: contiguous uint8 rows, height FIRST, width SECOND; row stride = width.
    using Detect = int (*)(const uint8_t *, int height, int width, int threshold, int min_area, int max_area, double *, int);
    Project project = nullptr;
    Solve solve = nullptr;
    Detect detect = nullptr;
};
// Must outlive every matcher/session using api(). Separate handles are required
// because both ELF files export pico_detect with incompatible signatures.
class NativeOpticalLibraries {
    void *optics_ = nullptr, *detector_ = nullptr;
    NativeOpticalApi api_;
  public:
    NativeOpticalLibraries(const std::string &optics_path, const std::string &detector_path);
    ~NativeOpticalLibraries();
    NativeOpticalLibraries(const NativeOpticalLibraries &) = delete;
    NativeOpticalLibraries &operator=(const NativeOpticalLibraries &) = delete;
    NativeOpticalApi api() const { return api_; }
};
using Rigid = std::array<double, 16>;
using LedModel = std::array<double, 24>;
Rigid identity_rigid();
bool identity_rigid_valid(const Rigid &);
struct IdentityView {
    OpticalCamera camera{};
    Rigid head_from_camera = identity_rigid();
    std::array<double, 2> pixel_offset{};
};
struct IdentityExposure {
    std::string serial, metadata; // exact canonical source metadata
    int width = 1056, height = 1024;
    bool timestamp_matched = false;
    int64_t stamp = 0;
    double exposure_us = 0;
    double source_raw = 0; // lower frames: source seconds, converted relative to frames[0]
    Rigid world_from_head = identity_rigid();
    IdentityView view;
    std::vector<uint8_t> pixels;
};
struct IdentityPair {
    bool direct_capture = false;
    bool timestamp_matched = false;
    int64_t clock_error = 0;
    std::array<IdentityExposure, 2> frames;
    // Same-exposure lower IR frames (640x480, full body below the head). Optional; mono only.
    std::vector<IdentityExposure> lower;
};
struct IdentityCandidate {
    const char* solver_kind = "blind full"; // diagnostics only, never part of PTP2
    double residual = 0, condition = 0, position_std = 0, rotation_std_deg = 0;
    std::shared_ptr<const IdentityPair> pair;
    std::array<std::string, 2> metadata;
    std::array<int64_t, 2> stamps{};
    std::array<Rigid, 2> heads;
    int64_t stamp = 0;
    bool stereo_verified = false;
    bool mono = false;
    int mono_view = -1;
    int lower_view = -1; // mono fit on pair->lower[lower_view]; mono_view stays -1
    int inliers = 0;
    bool prior_continuation = false; // three-point evidence is admitted only from seeded continuation
    bool seeded = false; // selected solution used a pose prior (including full stereo)
    bool few_led = false; // locked, prior-constrained reprojection fit; never blind acquisition
    bool few_led_rotation = false;
    int matched_observations = 0; // camera blobs, distinct from unique LED inliers
    Rigid world = identity_rigid();
};
struct IdentityObservation {
    int64_t stamp = 0, index = 0, error = 0;
    std::optional<Rigid> world;
    IdentityPair pair; // owns full images, never aliases mutable camera buffers
};
IdentityObservation identity_observation(const std::shared_ptr<const IdentityPair> &, int64_t index,
      const std::optional<IdentityCandidate> &, int64_t now);
// Stale/future or invalid measured pairs return stamp=0 (logged, counted) and
// are never admitted by observe(). Only a null pair or negative index throws.
uint64_t identity_stale_drops();
// Shared log-and-drop journal for optical/identity data checks: counts every
// drop per reason and writes one stderr line on the first and every 30th.
void log_optical_drop(const char *reason);
uint64_t optical_drops();
uint64_t optical_drops(const std::string &reason);
std::string optical_last_drop();
// Native detector plus the ray-domain filter of every view, before the cluster cap: 5 values per blob (x, y with
// the view's pixel offset, radius, peak, area). False (logged) on detector failure, as detect_view.
bool identity_detect_blobs(const NativeOpticalApi &, const IdentityExposure &, std::vector<double> &blobs);
// Multi-tracker optics (multi_optics.hpp, 2026-10-31): the upper-view blobs of one pair, detected once, with the blobs
// another tracker claims left out of this thread's detect_view. Never installed on the single-tracker path, where
// detect_view is unchanged.
struct IdentityBlobMask {
    NativeOpticalApi::Detect detector = nullptr;  // the cache is reused only by the same detector
    std::array<const uint8_t *, 2> source{};  // frames[v].pixels.data() the cached blobs belong to
    std::array<std::vector<double>, 2> blobs;
    std::array<std::vector<bool>, 2> excluded;
    inline static thread_local const IdentityBlobMask *current = nullptr;
    struct Scope {
        const IdentityBlobMask *previous;
        explicit Scope(const IdentityBlobMask *mask) : previous(current) { current = mask; }
        ~Scope() { current = previous; }
        Scope(const Scope &) = delete;
        Scope &operator=(const Scope &) = delete;
    };
};
// Stereo when both views have four blobs; calibrated monocular acquisition otherwise.
IdentityCandidate identity_stereo_candidate(const NativeOpticalApi &, const LedModel &,
    const std::shared_ptr<const IdentityPair> &, int64_t now, const double *prior=nullptr, bool allow_three=true);
// Mono solve on every lower frame with >=4 blobs; the lowest-residual fit, or an empty candidate.
IdentityCandidate identity_lower_candidate(const NativeOpticalApi &, const LedModel &,
    const std::shared_ptr<const IdentityPair> &, int64_t now);
// One candidate per exposure: upper stereo > upper mono > lower mono, then lower residual.
int identity_candidate_rank(const IdentityCandidate &);
bool identity_candidate_better(const IdentityCandidate &, const IdentityCandidate &);
// A lower frame usable with this pair (direct capture, 640x480, calibrated view, stamp within
// 5 ms of frames[0]: same IR exposure, the next one is >=8 ms away).
bool identity_pixel_in_ray_domain(const OpticalCamera &camera,double x,double y);
bool identity_lower_valid(const IdentityPair &, const IdentityExposure &);
uint8_t identity_box_mean11(const std::vector<uint8_t> &, int x, int y);
struct IdentityConfirmation { LedEpisode episode; std::vector<int64_t> lit, dark; };
class NativeLedModeIdentity {
    uint32_t chip_;
    LedModel model_;
    NativeOpticalApi api_;
    std::string link_;
    bool configured_ = false;
    bool dark_pixels(const IdentityObservation &, const IdentityObservation &, const IdentityObservation &) const;
  public:
    static constexpr int64_t period_ns = 33333333;
    std::vector<IdentityObservation> observations;
    std::map<unsigned, IdentityConfirmation> confirmations;
    NativeLedModeIdentity(uint32_t, LedModel, NativeOpticalApi);
    void configure(const std::vector<uint32_t> &, const std::string &);
    bool observe(IdentityObservation);
    bool evaluate(const LedEpisode &);
    bool verified(uint32_t, int64_t index, const std::string &, int64_t now) const;
    bool bound() const { return configured_ && confirmations.size() >= 2; }
};
// Fixed phase verification from phase_search.py. A failed 61-frame window is
// logged and a new window starts at the next acknowledged frame; the run keeps
// searching until Locked. Exhausted is never entered (kept for callers' code).
class IdentityPhaseWindow {
    std::optional<int64_t> start_;
    std::set<int64_t> seen_, lit_;
    uint64_t retries_ = 0;
  public:
    enum class State { Waiting, Verifying, Locked, Exhausted };
    State state = State::Waiting;
    void observe(int64_t index, bool lit, bool identity_verified, bool phase_acked);
    uint64_t retries() const { return retries_; }
};
class NativeIdentitySession {
    uint32_t chip_;
    std::string link_;
    mutable std::mutex optical_;
    NativeLedModeIdentity matcher_;
    std::atomic<int64_t> ready_stamp_{0};
    std::atomic<unsigned> restored_token_{0};
    std::atomic<bool> recovery_only_{false};
    std::atomic<bool> invalid_{false}, binding_{false};
  public:
    NativeLedRadioOwner owner;
    NativeIdentitySession(uint32_t, LedModel, std::string, NativeOpticalApi);
    bool optical(const std::shared_ptr<const IdentityPair> &, int64_t index,
                 const std::optional<IdentityCandidate> &, int64_t now);
    bool begin_if_ready(PeerEngine &, const std::string &, int64_t now, bool controllers_off);
    void radio_tick(PeerEngine &, const std::string &, int64_t now);
    // Shared production/wrapper dispatch: tick restoration before considering a
    // new episode, and never start through pending or queued commands.
    bool drive_radio(PeerEngine &, const std::string &, int64_t now);
    bool verified(uint32_t, int64_t index, const std::string &, int64_t now) const;
    bool bindings_snapshot() const { return !invalid_ && binding_; }
    void begin_recovery();
    bool recovery_optical(const std::shared_ptr<const IdentityPair>&, int64_t,
        const std::optional<IdentityCandidate>&, int64_t);
    bool recovery_tick(PeerEngine&, const std::string&, int64_t);
    void invalidate(); // engine/link/disconnect, no reuse; mode loss only pauses new episodes
    void restart();    // optics reset (2026-10-29): clear optical history and accept new frames; LED ledger kept
};
class QueryPeer;
// Explicit runtime installation boundary. Station canonical/mailbox and
// QueryHooks.native_radio_tick route through these methods; normal exposure
// dispatch must honor engine.native25_exclusive. No device access on construction.
class SingleTrackerNativeIdentityHooks {
    QueryPeer &engine_;
    NativeIdentitySession session_;
    Native25AckBridge bridge_;
    std::string link_;
    std::atomic<bool> prepared_{false}, invalid_{false};
  public:
    SingleTrackerNativeIdentityHooks(QueryPeer &, RoyClock &, LedModel, std::string link,
                                    NativeOpticalApi, std::function<int64_t()> now = monotonic_ns);
    bool prepare(bool pipeline_has_target);
    void configure(const std::vector<uint32_t> &);
    void disconnect();
    void invalidate();
    bool optical(uint32_t chip, const std::shared_ptr<const IdentityPair> &, int64_t index,
                 const std::optional<IdentityCandidate> &, int64_t now);
    bool radio_tick(int64_t now, bool strict_controllers_off_guard, bool optical_worker_healthy);
    bool verified(uint32_t chip, int64_t index, int64_t now) const;
    bool bindings_snapshot() const;
    void record(const Record &, const std::function<void()> &);
    bool publish(MailboxWriter &, int slot, const Mailbox &,
                 std::optional<PhaseReference> = {}, std::optional<CycleWindow> = {});
    bool restore_required() const { return session_.owner.restore_required(); }
};
} // namespace pico



#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_phase, "phase");
