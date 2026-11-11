#pragma once
#include "peer_engine.hpp"
namespace pico {
struct StationControlHooks {
    std::function<void()> optics_poll;
    std::function<void(uint32_t)> reset_optics;
    std::function<void(int, PeerEngine &, MailboxWriter &)> mailbox;
    std::function<void(int, std::optional<uint8_t>, uint32_t, std::optional<uint16_t>)> acknowledge;
    // Installed query/native25 policies must wrap engine advance/receive/ACK.
    // Mandatory callbacks prevent accidentally using base policy in the pilot.
    std::function<void(PeerEngine &)> advance;
    std::function<bool(PeerEngine &, uint16_t)> ack;
    std::function<bool(PeerEngine &, const Record &)> receive;
};
// Admitted target control adapter, one per module peer. Index 0 is the first target (exact chip/slot as
// before); multi4 indices 1..3 (peer_table.hpp) read their own ABI4 peer words and mailbox.
// Enrollment/product/optical capability binding remain separate; this class never enrolls a peer itself.
class StationControl {
    DebugMemory &debug_;
    DataPlane &plane_;
    PeerEngine &engine_;
    MailboxWriter writer_;
    StationControlHooks hooks_;
    unsigned index_;
    uint32_t peer_offset_;
    uint32_t stage_ = 0, rx_ = 0, ack_ = 0;
    std::optional<uint32_t> search_, answered_slot_;
    void reset();

  public:
    uint64_t replies = 0, ack_tag_changes = 0;
    StationControl(DebugMemory &, DataPlane &, PeerEngine &, StationControlHooks, unsigned index = 0);
    unsigned index() const { return index_; }
    void poll();
    void canonical(const Record &);
    void mailbox();
    void feedback(uint8_t);
};
void configure_dual_mode(PeerEngine &, int stock_n, int frame_n, bool omit19,
                         double poll_period_us = 8800);
// multi4: every tracker is polled every K x 6000 us, which fits no Stock window for K >= 2. Frame mode only, N
// written by command 16 and verified by the schema-2 mode query (body[24], PeerEngine::accept_mode).
void configure_frame_peer(PeerEngine &, int frame_n, double poll_period_us, bool omit19);
} // namespace pico
