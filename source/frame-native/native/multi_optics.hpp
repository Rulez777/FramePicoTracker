#pragma once
// 2026-10-31 optical identity for several trackers (docs/MULTI_TRACKER_RADIO_2026-10-31.md
// "Optical identity for several trackers"). Only constructed when PICO_PEERS enrolls a secondary chip with a factory
// LED model; the single-tracker path never creates it, and OpticalWorker without it is unchanged.
//
// One stereo pair can hold the constellations of several trackers. Per pair:
//  1. Detect blobs once per upper view (cached in the IdentityBlobMask the primary detector reuses).
//  2. Claim blobs per chip from the projected LEDs of its velocity-predicted pose (gate_px); a blob two tracks both
//     reach (second nearest within contest_ratio) is contested and given to nobody (merged feet, crossing hands).
//  3. The first target keeps its own OpticalWorker; blobs claimed by another tracker are masked out of its detector.
//  4. Every tracking secondary is solved seeded from its last pose with ONLY its own blobs, then gated (fit bounds,
//     jump) and published as PTP2 with its own chip.
//  5. A secondary is bound only causally, as one native25 episode per chip: its LEDs are dark until the radio lights
//     it (command 25 {2}). lit() opens a window; the ONE new constellation absent from every pair before the ON was
//     published becomes provisional (tracked, not published). The radio then sends OFF (25 {1}): the provisional
//     constellation must vanish (dark_pairs pairs with no blob at its place); a second ON must bring back one new
//     constellation within confirm_m of it. Only then is it that chip. A tracker that is not ours (unsynchronised,
//     stray) does not follow our OFF and is rejected. One chip at a time, only while every bound tracker is held.
//     A lost track (no fit for lost_ns) is reported so the radio darkens it again; it never rebinds by position.
#include "optical_worker.hpp"
#include <deque>
#include <mutex>

namespace pico {
struct MultiChipModel {
    uint32_t chip = 0;
    std::string serial;
    LedModel model{};
};
// Radio acts on: Dark (may light), Provisional (send OFF), Darkened (send ON again), Tracking (keep light sync).
enum class MultiChipState { Dark, Acquiring, Provisional, Darkening, Darkened, Relit, Tracking };
const char *multi_chip_state_name(MultiChipState);
struct MultiOpticsCounters {
    uint64_t pairs = 0, contested = 0, primary_masked = 0, fits = 0, published = 0, fit_drops = 0, jump_drops = 0,
             provisional = 0, bound = 0, ambiguous = 0, foreign = 0, no_before = 0, primary_overlap = 0,
             not_dark = 0, not_back = 0, lost = 0, windows_expired = 0, incomplete = 0;
};
struct MultiOpticsParams {
    int64_t track_ns = 300000000;   // a pose older than this does not predict/claim blobs
    int64_t lost_ns = 1000000000;   // no accepted fit for this long: track closed, LEDs darkened by the radio
    int64_t window_ns = 1000000000; // each window (ON, OFF, second ON) after its ACK
    int64_t before_ns = 400000000;  // pairs before the first ON publication that define the "before" scene
    unsigned dark_pairs = 2;        // pairs after the OFF ACK with the provisional constellation absent
    int budget_ms = 25;             // secondary solves per pair (the first target has its own budget)
    int episode_budget_ms = 80;     // pairs inside an ON window: every unclaimed cluster pairing is blind-solved
    double gate_px = 30, own_px = 8, contest_ratio = 1.5, cluster_px = 60;
    double jump_m = .10, speed_mps = 3.0, distinct_m = .05, primary_clear_m = .10, confirm_m = .15;
};
class MultiChipOptics final : public OpticalPeers {
  public:
    using Params = MultiOpticsParams;
    // primary = the first target (its OpticalWorker solves it); secondaries = chips with a factory LED model.
    MultiChipOptics(NativeOpticalApi, MultiChipModel primary, std::vector<MultiChipModel> secondaries,
                    Params = Params{});
    void prior_imu(const std::array<uint8_t,32>& bytes);
    // Radio thread. lit(): ON acknowledged (Dark -> Acquiring, Darkened -> Relit); off(): OFF acknowledged
    // (Provisional -> Darkening); dark(): any state -> Dark (link loss, radio gave up).
    void lit(uint32_t chip, int64_t on_published, int64_t on_ack);
    void off(uint32_t chip, int64_t off_published, int64_t off_ack);
    void dark(uint32_t chip);
    void expire(int64_t now);  // closes overdue windows and lost tracks (also without camera pairs)
    MultiChipState state(uint32_t chip) const;
    std::optional<uint32_t> acquiring() const;  // the one chip inside its identity episode
    bool primary_tracked(int64_t now) const;
    bool all_bound_tracked(int64_t now) const;  // primary and every Tracking secondary held a fit within track_ns
    std::vector<uint32_t> take_lost();
    std::vector<uint32_t> take_bound();
    std::vector<uint32_t> chips() const;
    // Optics thread, outside the session lock.
    std::shared_ptr<const IdentityBlobMask> before(const std::shared_ptr<const IdentityPair> &, int64_t now) override;
    void solved(const std::shared_ptr<const IdentityPair> &, const std::optional<IdentityCandidate> &primary,
                int64_t now, bool dark_frame) override;
    // Primary pose as the OpticalWorker published it (exposure stamp).
    void primary_pose(const Rigid &world, int64_t stamp) override;
    // Session thread: hands the poses of this pair to send (PTP2 of their own chip); a failed send is logged.
    void publish(int64_t now) override;
    void discard() override;
    std::function<bool(const OpticalPoseFrame &, int64_t)> send;
    std::vector<OpticalPoseFrame> take_frames();  // tests without a send sink
    MultiOpticsCounters counters() const;
    uint64_t chip_published(uint32_t chip) const;

  private:
    struct Track {
        uint32_t chip = 0;
        LedModel model{};
        MultiChipState state = MultiChipState::Dark;
        std::shared_ptr<OpticalPrior> gyro = std::make_shared<OpticalPrior>();
        bool calibrated = false;
        std::optional<Rigid> world;
        std::array<double, 3> velocity{};
        int64_t stamp = 0, on_published = 0, on_ack = 0, off_published = 0, off_ack = 0;
        unsigned dark_seen = 0;
        uint64_t sequence = 0, published = 0;
        uint64_t epoch = 0;  // bumped by every radio state change; a pair solved across one is not committed
        std::optional<OpticalPoseFrame> last;
    };
    struct Seen { int64_t stamp; std::array<std::vector<std::array<double, 2>>, 2> clusters; };
    NativeOpticalApi api_;
    MultiChipModel primary_;
    Params p_;
    mutable std::mutex mutex_;
    std::vector<Track> tracks_;
    std::optional<Rigid> primary_world_;
    std::array<double, 3> primary_velocity_{};
    int64_t primary_stamp_ = 0;
    std::deque<Seen> seen_;
    std::vector<uint32_t> lost_, bound_;
    std::vector<OpticalPoseFrame> frames_;
    MultiOpticsCounters counters_;
    std::shared_ptr<IdentityBlobMask> mask_;
    Track *find(uint32_t);
    const Track *find(uint32_t) const;
    void close(Track &, const char *why);  // mutex held
};
} // namespace pico
