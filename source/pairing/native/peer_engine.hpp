#pragma once
#include <cstdlib>
#include "roy_owner.hpp"
#include "sensor_status.hpp"
namespace pico {
inline bool supported_wire_revision(uint8_t revision) {
    switch(revision) {case 7: case 11: case 12: return true; default: return false;}
}
enum class PeerState {
    Absent,
    Discovering,
    OfferSent,
    Confirming,
    VerifyingMode,
    Configuring,
    WaitSync,
    Streaming,
    Stale,
    Reconnecting
};
using Configuration = std::vector<std::pair<uint8_t, Bytes>>;
struct Pending {
    uint8_t command = 0, sequence = 0, s1 = 2;
    Bytes body;
    uint32_t generation = 0;
    double first_sent = 0, last_sent = 0;
    unsigned retries = 0, retimes = 0;
    std::deque<uint32_t> attempts;
};
struct Mailbox {
    uint32_t generation;
    uint8_t s1;
    Bytes payload;
};
struct Recovery {
    unsigned attempts = 1;
    double started = 0, deadline = 0;
    bool completed = false;
};
class PeerEngine {
    std::function<double()> now_;
    std::function<Bytes()> nonce_;
    struct Resume {
        std::optional<std::string> profile;
        PeerState state;
        bool phase;
    };
    std::optional<Resume> resume_;
    std::map<uint16_t, uint8_t> ack_tags_;
    std::deque<uint16_t> ack_order_;
    void remember(uint16_t, uint8_t);
    Bytes fresh_nonce();
    void clear_mode();
    void confirmed();
    void next_config();
    void set_mode();
    bool accept_mode_set(const std::vector<Command> &);
    bool accept_mode(const std::vector<Command> &);
    bool retire_ack_tag(uint16_t);
    enum class MagWorkMode { Idle, Wait09, Delay, Wait07, Done };
    MagWorkMode mag_workmode_ = MagWorkMode::Idle;
    bool mag_workmode_enabled_ = false, imu_start_acked_ = false;
    double mag_workmode_due_ = 0;
    void mag_workmode_log(const std::string &, uint8_t sequence);

  public:
    const bool stock_mode_requested;
    bool stock_mode_verified = false;
    bool stock_independent_acked = false;
    uint8_t firmware_revision = 7;
    SensorStatus sensor_status;
    std::string sensor_status_directory="/tmp";
    std::optional<Bytes> sensor_nonce;
    unsigned sensor_page=0;
    double sensor_due=0, sensor_sent=0;
    bool stock_firmware = false, breadcrumb_capable = false;
    Bytes chip;
    int slot;
    PeerState state = PeerState::Absent;
    std::array<uint8_t, 30> sequences{};
    std::optional<Pending> pending;
    uint32_t generation = 0;
    size_t configure_index = 0;
    std::optional<double> last_reply, last_sample, confirm_started;
    bool ack_notification = false, phase_verified = false, blind_ack = false,
         continuity_lease = false;
    std::optional<Bytes> product_info;
    std::optional<uint16_t> acked_generation;
    std::optional<uint8_t> published_s1;
    unsigned transport_acks = 0, retry_count = 0, rejected = 0;
    Configuration configuration = {{17, {7}}, {25, {2}}, {19, {0}}};
    std::deque<std::pair<uint8_t, Bytes>> queued;
    bool mode_required = false, mode_preparing = false, native25_exclusive = false;
    bool native25_ack_retiring = false;
    // Installed native25 bridge wraps the real retirement (including synchronous
    // virtual advance). Unset retains the pre-episode protocol behavior.
    std::function<bool(uint16_t, const std::function<bool()> &)> native25_ack_bridge;
    std::optional<Bytes> mode_nonce;
    std::optional<std::string> mode_profile, desired_mode, required_query_profile, mode_set_target;
    std::optional<double> mode_confirmed_at, mode_switch_started;
    std::map<std::string, Configuration> mode_configurations;
    uint8_t mode_query_schema = 0;
    double mode_refresh_s = 10, mode_lease_s = 30, mode_query_timeout_s = 8,
           mode_retry_backoff_s = 2, mode_retry_after = 0, mode_switch_timeout_s = 20,
           command19_hold_s = 0;
    std::string mode_error;
    unsigned mode_query_failures = 0;
    bool sample_feedback_enabled = false;
    std::optional<uint8_t> sample_feedback_counter;
    uint64_t sample_feedback_publications = 0;
    std::optional<Recovery> initial_recovery;
    std::function<void(const std::string &, unsigned, uint8_t, const Bytes &)> recovery_event;
    // multi4 (owner rule: an unread write is not a tested condition): every schema-2 mode proof reports the
    // configured N (command 16 body[0]) and the N the tracker reads back (body[24]) with its pending flag (body[25]).
    std::function<void(uint8_t requested, uint8_t read, uint8_t pending)> n_readback;
    PeerEngine(Bytes, int, std::function<double()>, std::function<Bytes()>);
    virtual ~PeerEngine() = default;
    Bytes nonce_bytes() { return fresh_nonce(); }
    double now() const { return now_(); }
    void disconnect();
    Mailbox start();
    uint32_t next_generation();
    Mailbox begin(uint8_t, const Bytes &);
    std::optional<uint32_t> retime();
    Mailbox mailbox();
    virtual bool receive(const Bytes &, int s1, uint16_t tag, bool crc_valid);
    virtual bool ack_tag(uint16_t);
    virtual void advance();
    void sample_feedback(int);
    void queue_exposure_sync(int ticks, int online_count, bool timing_verified, bool own_frame,
                             int reason = 0);
    bool mode_verified() const;
    uint8_t query_schema() const;
    void query_mode();
    void request_mode(const std::string &);
    bool advance_initial_query(); // installed wrapper calls this BEFORE advance
    // Runtime configuration helper: omit19 affects config only; never enables later19.
    void configure(std::optional<uint8_t> decimation, bool omit19,
                   std::optional<uint8_t> imu_mode = {}, uint8_t m = 2);
};
struct PhaseReference {
    uint32_t cycles;
    uint32_t ticks;
};
struct CycleWindow {
    uint32_t start, end;
};
class MailboxWriter {
    DebugMemory &debug_;
    struct Value {
        uint32_t generation, meta;
        Bytes bytes;
    };
    std::map<int, Value> sent_;

  public:
    explicit MailboxWriter(DebugMemory &d) : debug_(d) {}
    bool publish(int, uint32_t, int, const Bytes &, std::optional<PhaseReference> phase = {},
                 std::optional<CycleWindow> window = {});
    void forget(int slot) { sent_.erase(slot); }
};
// Typed evidence inputs from the (not yet ported) diagnostic/optical owner.
struct ProducerSample {
    uint32_t counter = 0;
    int mode = 0;
    bool enabled = false, post_gate = false;
};
struct BenchImuEvidence {
    bool forbid_controllers = false;
    int min_controllers = -1;
    std::string guard_mode;
    bool temporal_identity_verified = false, pose_valid = false;
    std::string phase;
    bool bootstrap_producer_verified = false;
    double producer_verified_at = 0;
    std::vector<ProducerSample> samples;
    std::map<std::string, int> upstream;
};
class LaterImuStart {
    std::optional<double> reply_stable_;
    bool started_ = false;

  public:
    bool bench_admission(PeerEngine &, const BenchImuEvidence &,
                         const std::function<void()> &fresh_guard);
    bool prepare(PeerEngine &, const BenchImuEvidence &, const std::function<void()> &fresh_guard,
                 const std::function<void(double)> &persist_evidence);
    bool start(PeerEngine &, const BenchImuEvidence &, const std::function<void()> &fresh_guard,
               const std::function<void(double)> &persist_evidence);
};
// query_loader.receive's FINAL_GATE_INDEX predicate, before later19 admission.
void verify_bootstrap_producer(BenchImuEvidence &, double now);
} // namespace pico
