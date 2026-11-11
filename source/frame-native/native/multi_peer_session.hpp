#pragma once
#include "sensors.hpp"
// 2026-10-31 multi-tracker host: radio/IMU stack of the multi4 peers 1..3 (docs/MULTI_TRACKER_RADIO_2026-10-31.md
// "Host implementation"). Peer 0 stays the existing first-target pipeline (QueryPeer, native25 identity, optics).
// Each further peer gets its own PeerEngine (sequences, pending command, nonce, Frame mode proof with N read back),
// StationControl (its ABI4 peer words and mailbox) and DataPlane (counter feedback, reorder, PTI1 with its chip).
// The primary DataPlane hands every working-channel record of an enrolled secondary chip to divert().
// Optics: with multi-tracker optics (dark_leds) their LEDs start dark (configuration 25 {1}) and the hooks below carry
// light sync and the identity episodes (multi_led_schedule.hpp); without it they stream IMU only, LEDs as before.
#include "peer_table.hpp"
#include "station_control.hpp"
#include "query_peer.hpp"
#include <fstream>
#include <memory>
#include <sstream>

namespace pico {
struct SecondaryPeer {
    unsigned index = 0;
    PeerEntry entry;
    std::unique_ptr<PeerEngine> engine;
    std::unique_ptr<DataPlane> plane;
    std::unique_ptr<StationControl> control;
    bool product = false;
    std::optional<uint8_t> n_read;
    uint64_t n_readbacks = 0, n_mismatches = 0;
    // Optional optics hooks (multi_led_schedule.hpp); unset = the radio-only behaviour.
    std::function<void(int, PeerEngine &, MailboxWriter &)> mailbox_hook;
    std::function<void(std::optional<uint8_t>, uint32_t, std::optional<uint16_t>)> ack_hook;
};
inline void log_multi_peer(const std::string &line) {
    std::ofstream("/tmp/pico-diag.log", std::ios::app) << "multi_peer: " << line << std::endl;
}
// "requested" = configured N (command 16 body[0]); "read" = schema-2 body[24]; logged for every proof.
inline std::function<void(uint8_t, uint8_t, uint8_t)> n_readback_logger(uint32_t chip, uint64_t *count = nullptr,
                                                                        uint64_t *mismatches = nullptr,
                                                                        std::optional<uint8_t> *last = nullptr) {
    return [chip, count, mismatches, last](uint8_t requested, uint8_t read, uint8_t pending) {
        if (count)
            ++*count;
        if (mismatches && read != requested)
            ++*mismatches;
        if (last)
            *last = read;
        std::ostringstream o;
        o << "n_readback chip=" << std::hex << chip << std::dec << " requested=" << unsigned(requested)
          << " read=" << unsigned(read) << " pending=" << unsigned(pending)
          << (read == requested ? " ok" : " MISMATCH");
        log_multi_peer(o.str());
    };
}
class SecondaryPeers {
    std::vector<std::unique_ptr<SecondaryPeer>> peers_;

    static void route(SecondaryPeer &p, const std::vector<Command> &commands) {
        for (const auto &c : commands) {
            const auto &b = c.body;
            if (c.command != 1 || b.size() != 35 || b[0] != 0x82)
                continue;
            const std::string serial(b.begin() + 8, b.begin() + 25);
            if (!p.entry.serial.empty() && serial != p.entry.serial) {
                log_multi_peer("peer " + std::to_string(p.index) + " product serial " + serial + " is not " +
                               p.entry.serial + "; IMU not routed");
                continue;
            }
            if (!p.product)
                log_multi_peer("peer " + std::to_string(p.index) + " product " + serial);
            p.product = true;
        }
        auto &e = *p.engine;
        const bool streaming = e.state == PeerState::WaitSync || e.state == PeerState::Streaming;
        // Configuration ends with command 19 (IMU start), so WaitSync means the tracker was told to stream.
        if (streaming)
            e.sample_feedback_enabled = true;
        const bool allowed = p.product && streaming && (e.stock_firmware || e.mode_verified());
        p.plane->enrolled = allowed;
        p.plane->feedback_enabled = allowed;
    }

  public:
    // now/nonce as the primary QueryPeer.
    // dark_leds: configuration sends LED mode 25 {1} instead of {2}; the optics light each tracker in its episode.
    void start(const PeerTable &table, DebugMemory &debug, DataPlane &primary, std::function<double()> now,
               std::function<Bytes()> nonce, bool dark_leds = false) {
        if (!peers_.empty())
            throw std::runtime_error("Secondary peers already started");
        table.require_stock_timing();
        for (unsigned i = 1; i < table.peers.size(); ++i) {
            if (!table.peers[i].chip)
                continue;  // "-": paused module slot
            auto p = std::make_unique<SecondaryPeer>();
            auto &peer = *p;
            peer.index = i;
            peer.entry = table.peers[i];
            QueryHooks q;
            q.bench = [] { BenchImuEvidence b; b.forbid_controllers = true; b.min_controllers = 0; return b; };
            q.fresh_guard = [] {};
            q.native_radio_tick = [] { return false; };
            q.proof = [](unsigned, const Bytes &, const Page &) {};
            q.action = [](uint8_t, const Bytes &) {};
            q.prerequisites = [](const std::vector<std::pair<unsigned, Page>> &, const std::string &s) {
                log_multi_peer(s);
            };
            q.producer_lifetime = [](double) {};
            std::unique_ptr<PeerEngine> query;
            if(peer.entry.profile=="diagnostic7" || peer.entry.profile=="release-c2") {
                auto diagnostic=std::make_unique<QueryPeer>(le32(peer.entry.chip),int(i),now,nonce,q);
                diagnostic->bootstrap_n=uint8_t(table.tracker_n());
                query=std::move(diagnostic);
            } else query=std::make_unique<PeerEngine>(le32(peer.entry.chip),int(i),now,nonce);
            // Configuration-only Alpha peers need their own IMU start and independent mode.
            // QueryPeer retains its diagnostic plan, including its delayed command 19.
            const bool diagnostic = peer.entry.profile=="diagnostic7" || peer.entry.profile=="release-c2";
            if (!diagnostic) {
                const char *value = std::getenv("PICO_INDEPENDENT_MODE");
                const uint8_t independent = value && std::string(value)=="0" ? 0 : 1;
                query->configuration.insert(query->configuration.end()-1, {1, {0x31, independent}});
            }
            if (dark_leds)
                for (auto &c : query->configuration)
                    if (c.first == 25) c.second = {1};
            configure_frame_peer(*query, int(table.tracker_n()), table.poll_period_us(), diagnostic);
            query->breadcrumb_capable = peer.entry.profile == "release-c2";
            if (peer.entry.profile == "stock") {
                query->stock_firmware = true;
                query->mode_required = false;
                query->desired_mode.reset();
                query->configuration = {{16, {1, 2}}, {17, {7}}, {19, {0}}};
                log_multi_peer("stock chip=" + std::to_string(peer.entry.chip) +
                    " requested_N=1 readback=unavailable mode=unproven cadence=4727.5us"
                    " multi4_round_us=" + std::to_string(table.poll_period_us()));
            }
            peer.engine = std::move(query);
            peer.engine->n_readback =
                n_readback_logger(peer.entry.chip, &peer.n_readbacks, &peer.n_mismatches, &peer.n_read);
            Hooks h;
            h.now = primary.hooks.now;
            h.sensor = primary.hooks.sensor;
            h.canonical = [&peer](const Record &r, const std::vector<Command> &c, const std::optional<AckContext> &) {
                peer.control->canonical(r);
                peer.plane->sensor_format=sensor_format_for_revision(peer.engine->firmware_revision,peer.engine->stock_firmware);
                if(peer.engine->sensor_status.mask&1)peer.plane->imu_id=peer.engine->sensor_status.raw[1];
                route(peer, c);
            };
            h.feedback = [&peer](uint8_t counter) { peer.control->feedback(counter); };
            h.mailbox = [&peer] { peer.control->mailbox(); };
            peer.plane = std::make_unique<DataPlane>(std::move(h));
            peer.plane->chip = peer.entry.chip;
            peer.plane->shared_clock = &primary.clock;
            peer.plane->feedback_enabled = false;
            StationControlHooks sh;
            sh.optics_poll = [] {};
            sh.reset_optics = [&peer](uint32_t) { peer.product = false; };
            sh.mailbox = [&peer](int slot, PeerEngine &e, MailboxWriter &w) {
                if (peer.mailbox_hook) return peer.mailbox_hook(slot, e, w);
                const auto box = e.mailbox();
                w.publish(slot, box.generation, box.s1, box.payload);
            };
            sh.acknowledge = [&peer](int, std::optional<uint8_t> command, uint32_t cycles, std::optional<uint16_t> tag) {
                if (peer.ack_hook) peer.ack_hook(command, cycles, tag);
            };
            sh.advance = [](PeerEngine &e) { e.advance(); };
            sh.ack = [](PeerEngine &e, uint16_t tag) { return e.ack_tag(tag); };
            sh.receive = [](PeerEngine &e, const Record &r) {
                return e.receive(Bytes(r.dma.begin() + 2, r.dma.end()), r.dma[1], uint16_t(r.generation),
                                 r.hardware_crc);
            };
            peer.control = std::make_unique<StationControl>(debug, *peer.plane, *peer.engine, std::move(sh), i);
            std::ostringstream o;
            o << "peer " << i << " chip=" << std::hex << peer.entry.chip << std::dec << " N=" << table.tracker_n()
              << " period_us=" << table.poll_period_us() << (dark_leds ? " LEDs dark (25 {1})" : "") << " started";
            log_multi_peer(o.str());
            peers_.push_back(std::move(p));
        }
    }
    // PTI1 sink for every secondary plane (the session's UDP publisher); unset planes only encode.
    void set_send(const std::function<bool(const std::array<uint8_t, 32> &)> &send) {
        for (auto &p : peers_)
            p->plane->hooks.send = send;
    }
    void set_sensor_send(const std::function<bool(const pico_sensors::Wire &)> &send) {
        for(auto &p:peers_) p->plane->hooks.sensor=send;
    }
    bool divert(const Record &r, const Stamps &stamps) {
        for (auto &p : peers_)
            if (p->entry.chip == r.chip) {
                p->plane->record(r, stamps);
                return true;
            }
        return false;
    }
    void poll() {
        for (auto &p : peers_)
            try {
                const auto before = p->engine->state;
                p->control->poll();
                if (before != p->engine->state && p->engine->state == PeerState::Reconnecting)
                    log_multi_peer("peer " + std::to_string(p->index) + " link lost; waiting for rediscovery");
            } catch (const std::exception &e) {  // owner rule: logged, the other trackers keep running
                log_multi_peer("peer " + std::to_string(p->index) + " poll: " + e.what());
            }
    }
    size_t size() const { return peers_.size(); }
    SecondaryPeer &at(size_t i) { return *peers_.at(i); }
    void clear() { peers_.clear(); }
};
} // namespace pico
