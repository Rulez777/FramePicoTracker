#pragma once
#include "peer_engine.hpp"
#include <mutex>
namespace pico {
struct LedEpisode {
    unsigned token = 0;
    uint32_t chip = 0;
    std::set<uint16_t> off_tags, on_tags;
    std::optional<int64_t> off_ack, on_ack, off_published, on_published;
    int64_t off_error = 0, on_error = 0;
};
class LedModeEpisodes {
    uint32_t chip_;
  public:
    static constexpr int64_t hold_ns = 250000000, maximum_off_ns = 800000000;
    std::optional<LedEpisode> active;
    std::vector<LedEpisode> episodes;
    bool restore_required = false, failed = false;
    explicit LedModeEpisodes(uint32_t);
    void begin(uint32_t, bool mode1, bool controllers_off, bool geometry_ready);
    void published(const Bytes &, uint32_t generation, int64_t stamp);
    bool can_ack(uint32_t chip, const Bytes &, uint32_t tag, int64_t source,
                 bool crc, int64_t error = 0) const;
    bool acknowledge(uint32_t, const Bytes &, uint32_t, int64_t, bool, int64_t = 0);
    bool restoration_due(int64_t) const;
    void deadline(int64_t);
    void restored_application(bool fresh_mode1, bool measured_return);
    bool identity_verified() const { return false; }
};
// Radio thread owns scheduling. Optical consumers only take copied evidence.
// Object and its bridge must outlive all installed station callbacks.
class NativeLedRadioOwner {
    uint32_t chip_;
    std::string link_;
    mutable std::mutex evidence_;
    LedModeEpisodes ledger_;
    void engine(PeerEngine &, const std::string &);
    friend class Native25AckBridge;
  public:
    bool restore_started = false, abort_requested = false;
    bool relight = false;  // set when ON goes out: the session re-sends the light-sync command (2026-10-29)
    std::string failure;
    NativeLedRadioOwner(uint32_t, std::string);
    bool begin(PeerEngine &, const std::string &, int64_t, bool controllers_off, bool geometry);  // false = not ready, try later
    void tick(PeerEngine &, const std::string &, int64_t);
    void published(PeerEngine &, uint32_t, const Bytes &, int64_t);
    void optical_return(PeerEngine &, const std::string &, bool measured_return);
    std::optional<LedEpisode> snapshot() const;
    bool restore_required() const;
    size_t completed() const;
};
class Native25AckBridge {
    NativeLedRadioOwner &owner_;
    PeerEngine &engine_;
    RoyClock &clock_;
    std::function<int64_t()> now_;
    std::optional<AckContext> context_;
    bool recording_ = false;
    bool acknowledge(uint16_t, const std::function<bool()> &);
  public:
    Native25AckBridge(NativeLedRadioOwner &, PeerEngine &, RoyClock &, const std::string &,
                      std::function<int64_t()> now = monotonic_ns);
    ~Native25AckBridge();
    Native25AckBridge(const Native25AckBridge &) = delete;
    Native25AckBridge &operator=(const Native25AckBridge &) = delete;
    // Wrap StationControl::canonical, preserving its ordinary diagnostics path.
    void record(const Record &, const std::function<void()> &previous);
    // Call in StationControlHooks::mailbox with the actual engine mailbox.
    bool publish(MailboxWriter &, int slot, const Mailbox &,
                 std::optional<PhaseReference> = {}, std::optional<CycleWindow> = {});
};
// Temporal prefilter only: input frames must already pass the full optical
// admission. No pixel/pose inference, identity credit or restoration here.
struct IdentityFrameTime {
    int64_t stamp = 0, index = 0, error = 0;
    bool measured_world = false;
};
struct IdentityWindow {
    std::vector<size_t> before, after, dark;
    int64_t off_bound = 0, on_bound = 0;
};
std::optional<IdentityWindow> identity_window(const LedEpisode &, uint32_t chip,
    const std::vector<IdentityFrameTime> &, int64_t period_ns = 33333333);
} // namespace pico
