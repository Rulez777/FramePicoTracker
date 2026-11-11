#include "device_config.hpp"
#include "station_control.hpp"
#include "peer_table.hpp"
namespace pico {
StationControl::StationControl(DebugMemory &d, DataPlane &p, PeerEngine &e, StationControlHooks h,
                               unsigned index)
    : debug_(d), plane_(p), engine_(e), writer_(d), hooks_(std::move(h)), index_(index),
      peer_offset_(multi4_peer_offset(index)) {
    if (index_ == 0 && (engine_.slot != 0 || engine_.chip != pico::devices::chip_le()))
        throw std::invalid_argument("Exact first target/slot required");
    if (index_ != 0 && (index_ >= multi4_peer_limit || engine_.slot != int(index_) ||
                        engine_.chip == pico::devices::chip_le()))
        throw std::invalid_argument("multi4 peer index must match its engine slot (1..3, not the first target)");
    if (!hooks_.optics_poll || !hooks_.reset_optics || !hooks_.mailbox || !hooks_.acknowledge ||
        !hooks_.advance || !hooks_.ack || !hooks_.receive)
        throw std::invalid_argument("Missing installed station policy");
}
void StationControl::reset() {
    engine_.disconnect();
    engine_.state = PeerState::Absent;
    plane_.reset_peer();
    hooks_.reset_optics(word(engine_.chip));
}
void StationControl::mailbox() { hooks_.mailbox(int(index_), engine_, writer_); }
void StationControl::feedback(uint8_t counter) { engine_.sample_feedback(counter); }
void StationControl::poll() {
    hooks_.optics_poll();
    // Only the stage/rx/search/tag words (+1516..+1531 for peer 0) are used; the radio loop
    // carries this exact read in its next batched command (DebugMemory prefetch).
    const auto packed = debug_.read_many({{roy_data + peer_offset_ + 12, 16}});
    const auto stage = word(packed, 0), search = word(packed, 8), rx = word(packed, 4),
               tag = word(packed, 12) >> 16;
    const bool rediscovered = search_ && search != *search_;
    if ((stage_ >= 3 && stage < 3) ||
        (rediscovered && engine_.state != PeerState::Absent &&
         engine_.state != PeerState::Reconnecting && engine_.state != PeerState::Confirming))
        reset();
    search_ = search;
    stage_ = stage;
    if (engine_.state == PeerState::Absent && stage >= 2)
        engine_.start();
    if (rx != rx_ && stage >= 3)
        engine_.last_reply = engine_.now();
    rx_ = rx;
    if (tag != ack_) {
        ack_ = tag;
        ++ack_tag_changes;
        const auto command =
            engine_.pending ? std::optional<uint8_t>(engine_.pending->command) : std::nullopt;
        if (tag && stage >= 3 && hooks_.ack(engine_, static_cast<uint16_t>(tag)))
            hooks_.acknowledge(int(index_), command, rx, engine_.acked_generation);
    }
    hooks_.advance(engine_);
    if (engine_.state == PeerState::Reconnecting) {
        debug_.write(roy_data + peer_offset_ + 12, Bytes(4));
        reset();
    }
    mailbox();
}
void StationControl::canonical(const Record &r) {
    // Same strict query_loader safe_record predicates, even when called directly.
    if (!r.hardware_crc || r.partial || r.channel != 2 || r.profile != 0 || r.chip != word(engine_.chip) ||
        r.dma.size() < 3 || r.dma.size() != size_t(r.dma[0]) + 2 ||
        (r.dma[1] != 2 && r.dma[1] != 3))
        return;
    try {
        decode_records(Bytes(r.dma.begin() + 2, r.dma.end()));
    } catch (const std::exception &) {
        return;
    }
    if (!answered_slot_ || r.slot != *answered_slot_) {
        answered_slot_ = r.slot;
        ++replies;
    }
    if (engine_.state == PeerState::Absent || engine_.state == PeerState::Reconnecting)
        engine_.start();
    const auto command =
        engine_.pending ? std::optional<uint8_t>(engine_.pending->command) : std::nullopt;
    if (hooks_.receive(engine_, r))
        hooks_.acknowledge(int(index_), command, r.cycles, engine_.acked_generation);
    // The caller's DataPlane invokes mailbox AFTER decoding/feedback. Product
    // admission is supplied by its installed canonical callback, never inferred.
}
void configure_dual_mode(PeerEngine &e, int stock_n, int frame_n, bool omit, double period) {
    if (stock_n < 0 || stock_n > 40 || frame_n < 0 || frame_n > 40 ||
        !(stock_n * 4727.5 + 3934.5 <= period && period <= (stock_n + 1) * 4727.5) ||
        !(frame_n * 3000. + 2207. <= period && period <= (frame_n + 1) * 3000.))
        throw std::invalid_argument(
            "Station poll period must fit BOTH Stock and Frame receive windows");
    e.mode_required = true;
    e.desired_mode = e.stock_mode_requested ? "stock" : "frame";
    e.mode_query_schema = 2;
    Configuration base;
    for (const auto &c : e.configuration)
        if (c.first != 16 && !(omit && c.first == 19))
            base.push_back(c);
    for (const auto &entry : std::map<std::string, int>{{"stock", stock_n}, {"frame", frame_n}}) {
        Configuration c = {{16, {static_cast<uint8_t>(entry.second), 2}}};
        c.insert(c.end(), base.begin(), base.end());
        e.mode_configurations[entry.first] = c;
    }
}
void configure_frame_peer(PeerEngine &e, int frame_n, double period, bool omit) {
    // Window rule, or (PICO_MULTI_N, peer_table.hpp) a poll every m-th tracker window on the (N+1)*3000 grid.
    if (frame_n < 0 || frame_n > 40 ||
        !((frame_n * 3000. + 2207. <= period && period <= (frame_n + 1) * 3000.) ||
          PeerTable::on_wake_grid(unsigned(frame_n), period)))
        throw std::invalid_argument("Station poll period must fit the Frame receive window");
    e.mode_required = true;
    e.desired_mode = e.stock_mode_requested ? "stock" : "frame";
    e.mode_query_schema = 2;
    Configuration base;
    for (const auto &c : e.configuration)
        if (c.first != 16 && !(omit && c.first == 19))
            base.push_back(c);
    // A Stock-mode proof only leads to the Frame switch (set_mode); its N is the same requested value.
    Configuration c = {{16, {static_cast<uint8_t>(frame_n), 2}}};
    c.insert(c.end(), base.begin(), base.end());
    e.mode_configurations["stock"] = c;
    e.mode_configurations["frame"] = c;
}
} // namespace pico

#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_station, "station");
