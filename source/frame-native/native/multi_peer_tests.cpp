// 2026-10-31 multi-tracker host (docs/MULTI_TRACKER_RADIO_2026-10-31.md "Host implementation"): peer table,
// multi4 prepared state, ring records of 3 interleaved peers through the real RadioLoop + FakeOpenOcd batch,
// wrong/unenrolled peer index dropped and counted, per-peer StationControl words/mailboxes, Frame N per K with
// read back, host DWT enable/restore, and the single-peer (ackring8) path unchanged. Synthetic data only.
#include "native_test_runtime.hpp"
#include "fake_openocd.hpp"
#include "imu_slot.hpp"
#include "live_prepared_state.hpp"
#include "multi_peer_session.hpp"
#include "radio_loop.hpp"
#include "station_control.hpp"
#include <iostream>
using namespace pico;
namespace {
size_t checks = 0;
void check(bool b, int line = __builtin_LINE()) {
    ++checks;
    if (!b)
        throw std::runtime_error("multi check " + std::to_string(checks) + " line " + std::to_string(line));
}
template <class F> void rejects(F f, int line = __builtin_LINE()) {
    bool b = false;
    try {
        f();
    } catch (const std::exception &) {
        b = true;
    }
    check(b, line);
}
const Bytes first{0xd4, 0xc3, 0xb2, 0xa1};
constexpr uint32_t chip1 = 0xA1B2C3E2, chip2 = 0x0bfc3001;
// C++ copy of the slot-writing half of multi4 capture() (roy_ram_station_v4_multi4.c): dma[0] = len, dma[1] = s1.
void module_slot(uint8_t *slot, const Bytes &dma, unsigned peer, uint32_t packets, uint32_t cycles, uint16_t gen) {
    const uint32_t len = dma[0], rest = len - 1;
    uint32_t imu = 0;
    for (uint32_t o = 2; o + 3 <= len + 1; o += 3 + dma[o + 2])
        if (dma[o + 1] == 2 && dma[o + 2] >= 18)
            imu = o;
    auto put = [&](int o, uint32_t v) { for (int i = 0; i < 4; ++i) slot[o + i] = uint8_t(v >> (8 * i)); };
    put(4, cycles);
    slot[8] = uint8_t(gen);
    slot[9] = uint8_t(gen >> 8);
    if (imu) {
        const uint32_t n = dma[imu + 2] >= 36 ? 36 : 18;
        put(0, packets);
        slot[10] = uint8_t(peer);
        slot[11] = uint8_t(n);
        for (uint32_t i = 0; i < n; ++i)
            slot[12 + i] = dma[imu + 3 + i];
    } else {
        put(0, 0x80000000u | (rest << 24) | (uint32_t(dma[1] == 3) << 23) | (peer << 21) | (packets & 0x1fffffu));
        for (uint32_t i = 0; i < rest; ++i)
            slot[10 + i] = dma[2 + i];
    }
}
Bytes reply(uint8_t s1, const Bytes &records) {
    Bytes d{uint8_t(records.size() + 1), s1};
    unsigned sum = 0;
    for (auto b : records) {
        d.push_back(b);
        sum += b;
    }
    d.push_back(uint8_t(sum));
    return d;
}
Bytes imu_records(uint8_t first_counter, uint8_t tag) {
    Bytes r{0, 2, 36};
    for (int k = 0; k < 2; ++k) {
        Bytes s(18);
        s[0] = tag;
        s[14] = uint8_t(k);
        s[17] = uint8_t(first_counter + k);
        r.insert(r.end(), s.begin(), s.end());
    }
    return r;
}
// Real RadioLoop on FakeOpenOcd (batched TCL ring read), one primary DataPlane.
struct Loop {
    FakeOpenOcd f;
    DebugMemory d;
    OwnerIO io;
    DataPlane plane;
    RadioLoop loop;
    uint32_t w = 0;
    Loop()
        : d([this](const std::string &c) { return f.command(c); }), io(f.io(d)), plane(Hooks{}),
          loop(io, plane, {[] {}, [](const Bytes &) {}, [](uint32_t, bool) {}, [] {}, [](const IterationLatency &) {}}) {
        f.set(roy_data, 0x314d5250);
        plane.hooks.now = [this] { return f.ns; };
        plane.clock.fit = Fit{0, 1e9, 15.625, 0};
    }
    void ring(uint32_t flag) { f.set(roy_data + 8, flag); }
    void push(const Bytes &dma, unsigned peer, uint16_t gen = 2) {
        uint8_t s[48] = {};
        module_slot(s, dma, peer, w + 1, 1000 * (w + 1), gen);
        f.put(roy_data + 380 + (w % 23) * 48, Bytes(s, s + 48));
        f.set(roy_data + 112, ++w);
    }
    void push_raw(const uint8_t *s) {
        f.put(roy_data + 380 + (w % 23) * 48, Bytes(s, s + 48));
        f.set(roy_data + 112, ++w);
    }
};
struct Bound {
    phase5::BoundInvocation b;
    Bound() {
        b.sealed_files["attempt/station-config.json"] = R"({"seed_le":"74735415"})";
        b.sealed_files["bootstrap214.profile.json"] = R"({"frequency":2,"mode":1})";
        b.sealed_files["attempt/registry.json"] = "{}";
    }
};
} // namespace
int main() {
    try {
        {
            std::string error;
            auto t = PeerTable::parse("A1B2C3D4:0::release-c2,A1B2C3E2:1::stock", nullptr, &error);
            check(error.empty() && t.multi && t.peers[1].profile == "stock");
            check(t.peers[0].profile == "release-c2");
            PeerTable::parse("A1B2C3D4:0::release-c", nullptr, &error);
            check(!error.empty());
            PeerTable::parse("A1B2C3D4:0::unknown", nullptr, &error);
            check(!error.empty());
        }
        {
            std::string error;
            auto table=PeerTable::parse("A1B2C3D4:0,A1B2C3E2:1::stock",nullptr,&error);
            rejects([&]{table.require_stock_timing();});
            FakeOpenOcd fake;
            DebugMemory debug([&](const std::string&s){return fake.command(s);});
            DataPlane primary(Hooks{});
            double t=0;
            SecondaryPeers peers;
            rejects([&]{peers.start(table,debug,primary,[&]{return t;},[]{return Bytes(16,1);});});
            check(stock_poll_compatible(9455) && stock_poll_compatible(18910));
            for(unsigned k=1;k<=4;++k)check(!stock_poll_compatible(6000*k));
            table=PeerTable::parse("A1B2C3D4:0,A1B2C3E2:1",nullptr,&error);
            unsigned sensors=0;primary.hooks.sensor=[&](const pico_sensors::Wire &){++sensors;return true;};
            peers.start(table,debug,primary,[&]{return t;},[]{return Bytes(16,1);});
            check(bool(peers.at(0).plane->hooks.sensor));
            pico_sensors::Wire wire;wire.chip=chip1;
            check(peers.at(0).plane->hooks.sensor(wire)&&sensors==1);
            peers.set_sensor_send({});check(!peers.at(0).plane->hooks.sensor);
            peers.set_sensor_send(primary.hooks.sensor);check(bool(peers.at(0).plane->hooks.sensor));
            check(dynamic_cast<QueryPeer*>(peers.at(0).engine.get())==nullptr);
            SecondaryPeers selected;
            table=PeerTable::parse("A1B2C3D4:0,A1B2C3E2:1::diagnostic7",nullptr,&error);
            selected.start(table,debug,primary,[&]{return t;},[]{return Bytes(16,1);});
            check(dynamic_cast<QueryPeer*>(selected.at(0).engine.get())!=nullptr);
        }        // --- Peer table -------------------------------------------------------------------------------------
        {
            std::string error;
            auto legacy = PeerTable::parse(nullptr, nullptr, &error);
            check(!legacy.multi && error.empty() && legacy.peers.size() == 1 && legacy.peers[0].chip == 0xA1B2C3D4 &&
                  legacy.round_robin == 1 && legacy.tracker_n() == 1);
            auto t = PeerTable::parse("A1B2C3D4:0,A1B2C3E2:2:PCTESTTRACKER0003,0BFC3001:1", nullptr, &error);
            check(error.empty() && t.multi && t.peers.size() == 3 && t.round_robin == 3 && t.tracker_n() == 5 &&
                  t.poll_period_us() == 18000);
            check(t.peers[1].chip == chip1 && t.peers[1].pico_slot == 2 && t.peers[1].serial == "PCTESTTRACKER0003" &&
                  t.peers[2].pico_slot == 1 && t.peers[0].serial == "PCTESTTRACKER0001");
            check((t.chips() == std::array<uint32_t, 4>{0xA1B2C3D4, chip1, chip2, 0}));
            auto k = PeerTable::parse("A1B2C3D4", "4", &error);
            check(error.empty() && k.multi && k.peers.size() == 1 && k.peers[0].pico_slot == 0 && k.round_robin == 4 &&
                  k.tracker_n() == 7);
            for (auto bad : {"A1B2C3E2:0", "A1B2C3D4,A1B2C3D4", "A1B2C3D4,1,2,3,4", "7919ED3G", "A1B2C3D4:5", ""}) {
                error.clear();
                auto r = PeerTable::parse(bad, nullptr, &error);
                check(!error.empty() && !r.multi && r.peers.size() == 1);  // logged, single path kept
            }
            error.clear();
            check(!PeerTable::parse("A1B2C3D4,A1B2C3E2", "1", &error).multi && !error.empty());
            // "-" keeps a module index paused so a tracker keeps its PICO slot as module index.
            error.clear();
            auto gap = PeerTable::parse("A1B2C3D4:0,-,A1B2C3E2:2", nullptr, &error);
            check(error.empty() && gap.multi && gap.peers.size() == 3 && gap.enrolled() == 2 && gap.round_robin == 3 &&
                  (gap.chips() == std::array<uint32_t, 4>{0xA1B2C3D4, 0, chip1, 0}));
            check(!PeerTable::parse("-,A1B2C3D4", nullptr, &error).multi);
            check(multi4_peer_offset(0) == 1504 && multi4_peer_offset(1) == 1536 && multi4_peer_offset(2) == 1568 &&
                  multi4_peer_offset(3) == 1632);
            check(multi4_mailbox_offset(0) == 1664 && multi4_mailbox_offset(2) == 1792 && multi4_mailbox_offset(3) == 1944);
            // 3000 us grid rule for every K: N = 2K-1, N*3000+2207 <= 6000K <= (N+1)*3000.
            for (unsigned K = 1; K <= 4; ++K) {
                PeerTable g;
                g.round_robin = K;
                const double P = g.poll_period_us();
                const unsigned N = g.tracker_n();
                check(int(P) % 3000 == 0 && N * 3000. + 2207. <= P && P <= (N + 1) * 3000.);
                PeerEngine e(first, 0, [] { return 0.; }, [] { return Bytes(16, 1); });
                configure_frame_peer(e, int(N), P, true);
                check(e.mode_configurations.at("frame")[0] == std::make_pair(uint8_t(16), Bytes{uint8_t(N), 2}));
                rejects([&] { configure_frame_peer(e, int(N) + 1, P, true); });
                rejects([&] { configure_frame_peer(e, int(N), P + 1, true); });
            }
        }
        // --- Prepared state ---------------------------------------------------------------------------------
        {
            Bound bound;
            std::ostringstream quiet;
            auto *previous = std::cerr.rdbuf(quiet.rdbuf());
            const auto legacy = live_prepared_state(bound.b, PeerTable{});
            const auto one = live_prepared_state(bound.b, PeerTable::parse("A1B2C3D4:0", nullptr, nullptr));
            const auto three = live_prepared_state(bound.b, PeerTable::parse(
                "A1B2C3D4:0,A1B2C3E2:2:PCTESTTRACKER0003,0BFC3001:1", nullptr, nullptr));
            std::cerr.rdbuf(previous);
            // Legacy: the historical PICO address of slot 0 with seed 74735415 (docs/full-id-acquisition.md).
            check(word(legacy, 1504) == 0xA1B2C3D4 && word(legacy, 1508) == 0x126c606d && word(legacy, 1512) == 0 &&
                  word(legacy, 1532) == 0 && word(legacy, 1536 + 28) == 1 && word(legacy, 1568 + 28) == 1 &&
                  word(legacy, 1888) == 1 && word(legacy, 1856) == 0x15547374);
            check(Bytes(legacy.begin() + 1632, legacy.begin() + 1664) == Bytes(32) &&
                  Bytes(legacy.begin() + 1932, legacy.end()) == Bytes(2048 - 1932));
            // multi4 with one peer: equal to legacy except paused peer 3 in ABI slot 4 (prefix RBIT8(3), reserved 1).
            std::vector<size_t> diff;
            for (size_t i = 0; i < 2048; ++i)
                if (legacy[i] != one[i])
                    diff.push_back(i);
            check((diff == std::vector<size_t>{1640, 1660}) && one[1640] == 0xc0 && one[1660] == 1);
            // Three peers: chip A1B2C3E2 in PICO slot 2 -> historical BASE 6a6c606d, prefix 0x40 (slot 2).
            check(word(three, 1536) == chip1 && word(three, 1540) == 0x6a6c606d && word(three, 1544) == 0x40 &&
                  word(three, 1564) == 0);
            check(word(three, 1568) == chip2 && word(three, 1576) == 0x80 && word(three, 1596) == 0);
            check(word(three, 1504) == 0xA1B2C3D4 && word(three, 1508) == 0x126c606d && word(three, 1888) == 3);
            check(word(three, 1632) == 0 && word(three, 1660) == 1);  // index 3 paused
            check(word(three, 1608) == 1000 && word(three, 1612) == 192000000 && word(three, 1616) == 0);  // host words intact
            const auto gap = live_prepared_state(bound.b, PeerTable::parse("A1B2C3D4:0,-,A1B2C3E2:2", nullptr, nullptr));
            check(word(gap, 1536) == 0 && word(gap, 1540) == 0 && word(gap, 1536 + 28) == 1 && word(gap, 1568) == chip1 &&
                  word(gap, 1572) == 0x6a6c606d && word(gap, 1576) == 0x40 && word(gap, 1596) == 0 && word(gap, 1888) == 3);
            const auto k4 = live_prepared_state(bound.b, PeerTable::parse("A1B2C3D4", "4", nullptr));
            check(word(k4, 1888) == 4 && word(k4, 1536 + 28) == 1 && word(k4, 1660) == 1);
        }
        // --- Single-peer regression: ackring8 ring, no divert ----------------------------------------------
        {
            Loop l;
            l.ring(imu_ring_flag23t);
            std::vector<Record> seen;
            l.plane.hooks.canonical = [&](const Record &r, const std::vector<Command> &, const std::optional<AckContext> &) {
                seen.push_back(r);
            };
            l.plane.enrolled = true;
            uint8_t s[48] = {};
            const auto dma = reply(2, imu_records(10, 1));
            module_slot(s, dma, 0, 7, 7000, 4);
            s[10] = 2;  // ackring8: channel byte 2
            l.push_raw(s);
            l.loop.step();
            check(seen.size() == 1 && seen[0].chip == 0xA1B2C3D4 && seen[0].sequence == 7 &&
                  seen[0].dma == expand_imu_slot(s, true).dma);
            check(l.plane.counters.diverted == 0 && l.plane.counters.peer_rejected == 0 && l.plane.counters.samples == 2);
            auto ev = l.plane.next_event();
            check(ev && ev->wire && word(Bytes(ev->wire->begin() + 4, ev->wire->begin() + 8)) == 0xA1B2C3D4);
            // multi4 module without a table: peer 0 is the first target, other indices are dropped.
            l.ring(imu_ring_flag_multi4);
            l.push(reply(2, imu_records(12, 1)), 0);
            l.push(reply(2, imu_records(40, 1)), 1);
            l.loop.step();
            check(seen.size() == 2 && seen[1].chip == 0xA1B2C3D4 && l.plane.counters.peer_rejected == 1);
            check(l.plane.counters.sample_gaps == 0 && l.plane.counters.samples == 4);
        }
        // --- Three interleaved peers through the ring, per-peer planes --------------------------------------
        {
            Loop l;
            l.ring(imu_ring_flag_multi4);
            l.plane.peer_chips = {0xA1B2C3D4, chip1, chip2, 0};
            std::array<DataPlane, 2> secondary;
            std::array<std::vector<Record>, 3> seen;
            l.plane.hooks.canonical = [&](const Record &r, const std::vector<Command> &, const std::optional<AckContext> &) {
                seen[0].push_back(r);
            };
            for (size_t i = 0; i < 2; ++i) {
                secondary[i].chip = i ? chip2 : chip1;
                secondary[i].shared_clock = &l.plane.clock;
                secondary[i].enrolled = true;
                secondary[i].hooks.now = [&] { return l.f.ns; };
                secondary[i].hooks.canonical = [&, i](const Record &r, const std::vector<Command> &,
                                                     const std::optional<AckContext> &a) {
                    check(a && a->generation == r.generation);  // ACK context with the shared clock
                    seen[i + 1].push_back(r);
                };
            }
            l.plane.hooks.divert = [&](const Record &r, const Stamps &st) {
                for (auto &p : secondary)
                    if (p.chip == r.chip) {
                        p.record(r, st);
                        return true;
                    }
                return false;
            };
            l.plane.enrolled = true;
            uint8_t counter[3] = {0, 100, 200};
            for (unsigned round = 0; round < 6; ++round)
                for (unsigned peer = 0; peer < 3; ++peer) {
                    l.push(reply(2, imu_records(counter[peer], uint8_t(peer + 1))), peer, uint16_t(2 + 2 * round));
                    counter[peer] = uint8_t(counter[peer] + 2);
                }
            // Empty ACK of peer 2 (raw slot, rest 0), a 1-record command reply of peer 1, then wrong indices.
            l.push(reply(3, {}), 2, 40);
            l.push(reply(2, {0, 17, 1, 7}), 1, 42);
            l.push(reply(2, imu_records(77, 9)), 3);  // index 3: no peer enrolled
            uint8_t bad[48] = {};
            module_slot(bad, reply(2, imu_records(78, 9)), 0, 99, 99, 2);
            bad[10] = 5;  // index outside 0..3
            l.push_raw(bad);
            l.loop.step();
            check(seen[0].size() == 6 && seen[1].size() == 7 && seen[2].size() == 7);
            for (size_t p = 0; p < 3; ++p)
                for (const auto &r : seen[p])
                    check(r.chip == (p == 0 ? 0xA1B2C3D4 : p == 1 ? chip1 : chip2));
            check(seen[2].back().dma == Bytes({1, 3, 0}) && decode_records(Bytes{0}).empty());
            check(seen[1].back().dma == reply(2, {0, 17, 1, 7}));
            check(l.plane.counters.diverted == 14 && l.plane.counters.peer_rejected == 2 && l.plane.counters.source == 0);
            check(l.plane.counters.samples == 12 && l.plane.counters.sample_gaps == 0);
            for (size_t i = 0; i < 2; ++i) {
                check(secondary[i].counters.samples == 12 && secondary[i].counters.sample_gaps == 0 &&
                      secondary[i].counters.no_clock == 0);
                unsigned events = 0;
                while (auto ev = secondary[i].next_event()) {
                    ++events;
                    check(ev->chip == secondary[i].chip && ev->wire &&
                          word(Bytes(ev->wire->begin() + 4, ev->wire->begin() + 8)) == secondary[i].chip &&
                          (*ev->wire)[20] == i + 2);  // gyro x low byte = the peer tag written above
                }
                check(events == 12);
            }
            // An unknown chip on the working channel without a divert target is rejected as before.
            Record foreign;
            foreign.hardware_crc = true;
            foreign.channel = 2;
            foreign.profile = 0;
            foreign.chip = 0x12345678;
            foreign.dma = reply(2, {});
            l.plane.record(foreign);
            check(l.plane.counters.source == 1 && l.plane.counters.diverted == 14);
            // A blank (never written) IMU slot is skipped, not counted as a wrong peer.
            uint8_t blank[48] = {};
            check(!expand_multi4_slot(blank, l.plane.peer_chips).rejected &&
                  expand_multi4_slot(blank, l.plane.peer_chips).record.dma.empty());
        }
        // --- Per-peer StationControl: own words, own mailbox ------------------------------------------------
        {
            FakeOpenOcd f;
            DebugMemory d([&](const std::string &c) { return f.command(c); });
            DataPlane p;
            auto hooks = [] {
                StationControlHooks h;
                h.optics_poll = [] {};
                h.reset_optics = [](uint32_t) {};
                h.mailbox = [](int slot, PeerEngine &e, MailboxWriter &w) {
                    auto m = e.mailbox();
                    w.publish(slot, m.generation, m.s1, m.payload);
                };
                h.acknowledge = [](int, std::optional<uint8_t>, uint32_t, std::optional<uint16_t>) {};
                h.advance = [](PeerEngine &e) { e.advance(); };
                h.ack = [](PeerEngine &e, uint16_t t) { return e.ack_tag(t); };
                h.receive = [](PeerEngine &e, const Record &r) {
                    return e.receive(Bytes(r.dma.begin() + 2, r.dma.end()), r.dma[1], uint16_t(r.generation), true);
                };
                return h;
            };
            PeerEngine e0(first, 0, [&] { return f.ns / 1e9; }, [] { return Bytes(16, 1); });
            PeerEngine e1(le32(chip1), 1, [&] { return f.ns / 1e9; }, [] { return Bytes(16, 2); });
            PeerEngine e3(le32(chip2), 3, [&] { return f.ns / 1e9; }, [] { return Bytes(16, 3); });
            StationControl s0(d, p, e0, hooks());
            StationControl s1(d, p, e1, hooks(), 1);
            StationControl s3(d, p, e3, hooks(), 3);
            rejects([&] { StationControl x(d, p, e1, hooks(), 2); });
            rejects([&] { StationControl x(d, p, e1, hooks(), 4); });
            rejects([&] { StationControl x(d, p, e1, hooks()); });
            rejects([&] { PeerEngine x(le32(chip1), 4, [] { return 0.; }, [] { return Bytes(16, 1); }); });
            f.set(roy_data, 0x314d5250);
            f.set(roy_data + 1536 + 12, 3);
            f.set(roy_data + 1632 + 12, 3);
            f.transcript.clear();
            s0.poll();
            check(!f.transcript.empty() && f.transcript[0] == "concat [read_memory 0x20011204 32 4]" &&
                  e0.state == PeerState::Absent);
            f.transcript.clear();
            s1.poll();
            check(f.transcript[0] == "concat [read_memory 0x20011224 32 4]" && e1.state == PeerState::Confirming);
            check(word(f.read(roy_data + 1728, 4)) == e1.generation && word(f.read(roy_data + 1728 + 4, 4)) == 0x201);
            s3.poll();
            check(e3.state == PeerState::Confirming && word(f.read(roy_data + 1944, 4)) == e3.generation &&
                  word(f.read(roy_data + 1944 + 4, 4)) == 0x201);
            // Stage loss resets the peer's own stage word only.
            f.set(roy_data + 1504 + 12, 3);
            f.set(roy_data + 1632 + 12, 0);
            s3.poll();
            check(word(f.read(roy_data + 1504 + 12, 4)) == 3);
            // Only records of the engine's own chip are taken.
            Record r;
            r.hardware_crc = true;
            r.channel = 2;
            r.profile = 0;
            r.chip = 0xA1B2C3D4;
            r.dma = reply(2, {});
            const auto replies = s1.replies;
            s1.canonical(r);
            check(s1.replies == replies);
            r.chip = chip1;
            r.slot = 9;
            s1.canonical(r);
            check(s1.replies == replies + 1);
        }
        // --- Frame N written and read back through the schema-2 mode proof ---------------------------------
        {
            double now = 1;
            PeerEngine e(le32(chip1), 1, [&] { return now; }, [] { return Bytes(16, 7); });
            configure_frame_peer(e, 5, 18000, false);
            std::vector<std::array<unsigned, 3>> reads;
            e.n_readback = [&](uint8_t req, uint8_t got, uint8_t pend) { reads.push_back({req, got, pend}); };
            uint8_t tracker_n = 1;
            auto answer = [&] {
                check(bool(e.pending));
                const auto p = *e.pending;
                Bytes rec;
                if (p.command == 1 && p.body.size() == 18 && p.body[0] == 0x7e) {
                    Bytes page{0xfe, 2};
                    page.insert(page.end(), p.body.begin() + 2, p.body.end());
                    const auto c = le32(chip1);
                    page.insert(page.end(), c.begin(), c.end());
                    page.insert(page.end(), {1, 7, tracker_n, 0});
                    rec = {0, 1, uint8_t(page.size())};
                    rec.insert(rec.end(), page.begin(), page.end());
                } else if (p.command == 16)
                    tracker_n = p.body[0];
                rec.push_back(0);
                unsigned sum = 0;
                for (size_t i = 0; i + 1 < rec.size(); ++i)
                    sum += rec[i];
                rec.back() = uint8_t(sum);
                e.receive(rec, 2, uint16_t(p.generation), true);
                now += .01;
            };
            e.start();
            Bytes info(35);
            info[0] = 0x82;
            Bytes rec{0, 1, 35};
            rec.insert(rec.end(), info.begin(), info.end());
            unsigned sum = 0;
            for (auto v : rec)
                sum += v;
            rec.push_back(uint8_t(sum));
            e.receive(rec, 2, uint16_t(e.generation), true);
            std::vector<uint8_t> commands;
            for (int i = 0; i < 12 && e.state != PeerState::WaitSync; ++i) {
                commands.push_back(e.pending->command);
                answer();
            }
            check(e.state == PeerState::WaitSync && e.mode_verified() && tracker_n == 5);
            check((commands == std::vector<uint8_t>{1, 16, 1, 16, 17, 25, 19}));
            check(reads.size() == 2 && (reads[0] == std::array<unsigned, 3>{5, 1, 0}) &&
                  (reads[1] == std::array<unsigned, 3>{5, 5, 0}));
        }
        // --- Host DWT enable before operation 0, checkpoint restore ----------------------------------------
        for (int host = 0; host < 2; ++host) {
            FakeOpenOcd f;
            DebugMemory d([&](const std::string &c) { return f.command(c); });
            SramOwner owner(f.io(d), [](const Checkpoint &) {});
            owner.host_dwt = host;
            const Bytes image(0x19000, 0x91);
            f.stock(image);
            Bytes state(2048);
            const auto m = le32(0x314d5250);
            std::copy(m.begin(), m.end(), state.begin());
            state[84] = 255;
            owner.install(image, Bytes{1, 2, 3, 4, 5}, roy_code | 1, state);
            owner.enter([] {});
            check(word(f.read(0xe000edfc, 4)) == (host ? 0x11203040u : 0x10203040u) &&
                  word(f.read(0xe0001000, 4)) == (host ? 0x76543211u : 0x76543210u));
            check(owner.trace_enabled_verified == bool(host));
            owner.restore();
            check(owner.barriers.complete() && owner.barriers.trace_controls_restored_verified &&
                  word(f.read(0xe000edfc, 4)) == 0x10203040u && word(f.read(0xe0001000, 4)) == 0x76543210u);
        }
        std::cout << "multi peer tests PASS " << checks << '\n';
        return 0;
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
