// 2026-11-02 multi data plane (MULTI_DATAPLANE_BRIEF.md, outputs/fullbody-20261102/DATAPLANE_REPORT.md):
// 1. first target after a link loss: the release plan (and command 19) is sent again in multi; the old behaviour
//    (single path, flag unset) is reproduced: no second command 19, IMU routing stays closed.
// 2. PICO_MULTI_N wake-grid rule (peer_table.hpp / configure_frame_peer).
// 3. synthetic K=2/K=3 Alpha streams through the production slot decoder and DataPlane divert: per-chip samples,
//    no cross-attribution; host throughput at the model reply rate and at the measured every-other-poll cadence.
// 4. optional: replay of real radio.bin dumps (argv) through expand_alpha_slot + DataPlane divert, per-chip counts
//    against an independent decode of the same bodies.
#include "imu_slot.hpp"
#include "live_imu_routing.hpp"
#include "peer_table.hpp"
#include "query_peer.hpp"
#include "sensors.hpp"
#include "station_control.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
using namespace pico;
namespace {
unsigned checks = 0;
void check(bool b, int line = __builtin_LINE()) {
    ++checks;
    if (!b)
        throw std::runtime_error("multi_dataplane check " + std::to_string(checks) + " line " + std::to_string(line));
}
template <class F> void rejects(F f, int line = __builtin_LINE()) {
    bool thrown = false;
    try {
        f();
    } catch (const std::exception &) {
        thrown = true;
    }
    check(thrown, line);
}
Bytes packet(const Bytes &body, uint8_t command = 1) {
    Bytes p{0, command, uint8_t(body.size())};
    p.insert(p.end(), body.begin(), body.end());
    unsigned sum = 0;
    for (auto v : p)
        sum += v;
    p.push_back(uint8_t(sum));
    return p;
}
const Bytes first_chip{0xd4, 0xc3, 0xb2, 0xa1};
Command product() {
    Bytes b(35);
    b[0] = 0x82;
    b[1] = 0xbd;  // firmware 189
    const std::string serial = "PCTESTTRACKER0001";
    std::copy(serial.begin(), serial.end(), b.begin() + 8);
    return {0, 1, b};
}
// Release-plan fixture of the first target: Alpha (revision 12), multi4 K=3 (N=5, 18000 us).
struct FirstTarget {
    double t = 0;
    uint8_t nonce = 0;
    std::vector<std::pair<uint8_t, Bytes>> actions;
    LiveImuRouting routing;
    DataPlane plane;
    QueryPeer e;
    explicit FirstTarget(bool restart)
        : e(first_chip, 0, [this] { return t; }, [this] { return Bytes(16, ++nonce); },
            {[] {
                 BenchImuEvidence b;
                 b.forbid_controllers = true;
                 b.min_controllers = 0;
                 b.guard_mode = "strict";
                 return b;
             },
             [] {}, [] { return false; }, [](unsigned, const Bytes &, const Page &) {},
             [this](uint8_t c, const Bytes &b) {
                 actions.push_back({c, b});
                 routing.action(c, b);
             },
             [](const std::vector<std::pair<unsigned, Page>> &, const std::string &) {}, [](double) {}}) {
        configure_frame_peer(e, 5, 18000, true);
        e.bootstrap_n = uint8_t(5);
        e.firmware_revision = 12;
        e.restart_plan_after_reconnect = restart;
        e.recovery_event = [](const std::string &, unsigned, uint8_t, const Bytes &) {};
        linked();
    }
    // What accept_mode/next_config leave behind once a reconnected tracker proved Frame mode and took command 16.
    void linked() {
        e.mode_profile = "frame";
        e.mode_confirmed_at = t;
        e.state = PeerState::WaitSync;
        e.continuity_lease = true;
    }
    Bytes reply(unsigned schema, const Bytes &n) {
        Bytes b{0xfe, uint8_t(schema)};
        b.insert(b.end(), n.begin(), n.end());
        b.insert(b.end(), e.chip.begin(), e.chip.end());
        b.resize(26);
        b[22] = 1;
        b[23] = 12;
        b[24] = 5;
        return b;
    }
    void step() {
        e.last_reply = t;
        e.advance();
        if (e.pending) {
            const auto p = *e.pending;
            (void)e.mailbox();
            if (e.active_query())
                e.receive(packet(reply(p.body[1], Bytes(p.body.begin() + 2, p.body.end()))), 3,
                          uint16_t(p.generation), true);
            else
                e.ack_tag(uint16_t(p.generation));
        }
        routing.canonical(plane, e, {product()});
        t = std::max(t + .1, e.diagnostic_due);
    }
    size_t count(uint8_t command) const {
        return size_t(std::count_if(actions.begin(), actions.end(), [&](const auto &a) { return a.first == command; }));
    }
    void run(double seconds) {
        const double end = t + seconds;
        while (t < end)
            step();
    }
};
void first_target_restart() {
    const Configuration plan{{uint8_t(17), {7}},       {uint8_t(10), Bytes(4)},     {uint8_t(25), {2}},
                             {uint8_t(14), {1, 1, 0}}, {uint8_t(1), {0x31, 1}}, {uint8_t(19), {0}}};
    for (bool restart : {false, true}) {
        std::ostringstream quiet;
        auto *previous = std::cerr.rdbuf(quiet.rdbuf());
        try {
            FirstTarget x(restart);
            x.run(30);
            check(x.actions == plan);
            check(x.e.sample_feedback_enabled && x.plane.enrolled && x.plane.feedback_enabled);
            // Body-worn link loss (session-215848: 3 s without replies, module re-offer, product reply again).
            x.e.disconnect();
            x.routing.reset();  // StationControl::reset -> reset_optics -> imu_routing_.reset()
            x.e.state = PeerState::Absent;
            x.step();
            check(!x.plane.enrolled && !x.e.sample_feedback_enabled);
            x.linked();
            x.run(30);
            if (!restart) {  // the failure of session-215848: link back, never streaming again
                check(x.count(19) == 1 && x.e.plan_restarts == 0);
                check(!x.e.sample_feedback_enabled && !x.plane.enrolled);
            } else {
                check(x.count(19) == 2 && x.e.plan_restarts == 1);
                check(Configuration(x.actions.begin() + 6, x.actions.end()) == plan);
                check(x.e.sample_feedback_enabled && x.plane.enrolled && x.plane.feedback_enabled);
                // A second loss restarts once more; no restart without a loss.
                x.run(10);
                check(x.count(19) == 2);
                x.e.disconnect();
                x.routing.reset();
                x.e.state = PeerState::Absent;
                x.step();
                x.linked();
                x.run(30);
                check(x.count(19) == 3 && x.e.plan_restarts == 2 && x.plane.enrolled);
            }
        } catch (...) {
            std::cerr.rdbuf(previous);
            throw;
        }
        std::cerr.rdbuf(previous);
    }
    // A loss before the plan began (diagnostic_index 0) runs the ordinary plan once.
    {
        std::ostringstream quiet;
        auto *previous = std::cerr.rdbuf(quiet.rdbuf());
        FirstTarget x(true);
        x.e.disconnect();
        x.e.state = PeerState::Absent;
        x.step();
        x.linked();
        x.run(30);
        std::cerr.rdbuf(previous);
        check(x.count(19) == 1 && x.e.plan_restarts == 0 && x.plane.enrolled);
    }
}
void wake_grid() {
    check(PeerTable::on_wake_grid(5, 18000) && PeerTable::on_wake_grid(2, 18000) && PeerTable::on_wake_grid(1, 18000));
    check(PeerTable::on_wake_grid(3, 12000) && PeerTable::on_wake_grid(1, 12000) && PeerTable::on_wake_grid(1, 6000));
    check(!PeerTable::on_wake_grid(4, 18000) && !PeerTable::on_wake_grid(2, 12000) && !PeerTable::on_wake_grid(0, 18000));
    check(!PeerTable::on_wake_grid(6, 18000) && !PeerTable::on_wake_grid(41, 126000));
    PeerTable k3 = PeerTable::parse("A1B2C3D4:0,A1B2C3E2:2,A1B2C3E1:1", nullptr, nullptr);
    check(k3.tracker_n() == 5);
    k3.n_override = 2;
    check(k3.tracker_n() == 2);
    for (unsigned n : {1u, 2u, 5u}) {
        PeerEngine e(Bytes{1, 2, 3, 4}, 1, [] { return 0.0; }, [] { return Bytes(16, 1); });
        configure_frame_peer(e, int(n), 18000, false);
        check(e.mode_configurations.at("frame").front() == std::make_pair(uint8_t(16), Bytes{uint8_t(n), 2}));
    }
    PeerEngine e(Bytes{1, 2, 3, 4}, 1, [] { return 0.0; }, [] { return Bytes(16, 1); });
    rejects([&] { configure_frame_peer(e, 4, 18000, false); });
    rejects([&] { configure_frame_peer(e, 2, 12000, false); });
    rejects([&] { configure_frame_peer(e, 6, 18000, false); });
}
// The module's complete-envelope writer (station_alpha/multi.c capture()).
Bytes slot_of(const Bytes &dma, unsigned peer, uint32_t sequence, uint32_t cycles) {
    Bytes slot(80);
    auto put = [&](unsigned o, uint32_t v) {
        const auto b = le32(v);
        std::copy(b.begin(), b.end(), slot.begin() + o);
    };
    put(0, 0x80000000u | ((dma[0] - 1u) << 24) | (unsigned(dma[1] & 3) << 19) | (peer << 21) | (sequence & 0x7ffff));
    put(4, cycles);
    slot[8] = 6;
    std::copy(dma.begin() + 2, dma.end() - 1, slot.begin() + 10);
    return slot;
}
Bytes alpha_reply(uint8_t &counter, unsigned samples, bool mag) {
    Bytes b{uint8_t(0xc8 | samples | (mag ? 4 : 0)), 0x34, 0x12};
    for (unsigned i = 0; i < samples; ++i) {
        Bytes s(16);
        s[0] = uint8_t(i + 1);
        s[12] = 1;  // sync ticks
        s[15] = ++counter;
        b.insert(b.end(), s.begin(), s.end());
    }
    if (mag)
        b.insert(b.end(), {1, 128, 2, 128, 3, 128});
    Bytes d{uint8_t(b.size() + 4), 3, 0, 2, uint8_t(b.size())};
    d.insert(d.end(), b.begin(), b.end());
    unsigned sum = 0;
    for (size_t i = 2; i < d.size(); ++i)
        sum += d[i];
    d.push_back(uint8_t(sum));
    return d;
}
// Primary plane (index 0) diverting to its secondaries, as LiveSessionOwner + SecondaryPeers wire them.
struct Planes {
    std::array<uint32_t, 4> chips{};
    std::vector<std::unique_ptr<DataPlane>> planes;
    std::map<uint32_t, uint64_t> decoded, sent, wrong;
    explicit Planes(const std::array<uint32_t, 4> &c) : chips(c) {
        for (unsigned i = 0; i < 4 && chips[i]; ++i) {
            Hooks h;
            h.now = [] { return int64_t(20000000000LL); };
            const uint32_t chip = chips[i];
            h.decoded = [this, chip](const Record &r, const Sample &) {
                ++decoded[chip];
                if (r.chip != chip)
                    ++wrong[chip];
            };
            h.send = [this, chip](const std::array<uint8_t, 32> &w) {
                const uint32_t c = uint32_t(w[4]) | uint32_t(w[5]) << 8 | uint32_t(w[6]) << 16 | uint32_t(w[7]) << 24;
                ++sent[chip];
                if (c != chip)
                    ++wrong[chip];
                return true;
            };
            auto p = std::make_unique<DataPlane>(h);
            p->chip = chip;
            p->enrolled = true;
            p->sensor_format = SensorFormat::Alpha12;
            planes.push_back(std::move(p));
        }
        planes[0]->clock.fit = Fit{0, 10000000000., 15.625, 0};
        planes[0]->peer_chips = chips;
        for (size_t i = 1; i < planes.size(); ++i)
            planes[i]->shared_clock = &planes[0]->clock;
        planes[0]->hooks.divert = [this](const Record &r, const Stamps &s) {
            for (size_t i = 1; i < planes.size(); ++i)
                if (planes[i]->chip == r.chip) {
                    planes[i]->record(r, s);
                    return true;
                }
            return false;
        };
    }
    void slot(const Bytes &s) {
        const auto r = expand_alpha_slot(s.data(), chips, true);
        if (!r.dma.empty())
            planes[0]->record(r);
    }
};
void synthetic_streams() {
    const std::array<uint32_t, 4> chips{0xA1B2C3D4, 0xA1B2C3E2, 0xA1B2C3E1, 0};
    for (unsigned k : {2u, 3u}) {
        for (bool every_other : {false, true}) {
            std::array<uint32_t, 4> enrolled = chips;
            if (k == 2)
                enrolled[2] = 0;
            Planes x(enrolled);
            std::array<uint8_t, 4> counter{};
            const unsigned slots = 10 * 166;  // 10 s of 6-ms slots
            unsigned reply_count[4] = {};
            uint64_t total[4] = {};
            for (unsigned s = 0; s < slots; ++s) {
                const unsigned peer = s % k, poll = s / k;
                if (every_other && poll % 2)
                    continue;  // measured 2026-11-02: streaming trackers answer every second poll at N=2K-1
                const bool mag = poll % 2 == 1;
                x.slot(slot_of(alpha_reply(counter[peer], mag ? 2 : 3, mag), peer, s, s * 384000u));
                ++reply_count[peer];
                total[peer] += mag ? 2 : 3;
            }
            for (unsigned i = 0; i < k; ++i) {
                const uint32_t c = enrolled[i];
                check(x.wrong[c] == 0);
                check(x.decoded[c] == total[i] && x.sent[c] == total[i]);
                check(x.planes[i]->counters.sample_gaps == 0);
                const double hz = x.sent[c] / 10.0;
                std::cout << "synthetic K=" << k << (every_other ? " every-other-poll" : " every-poll") << " chip "
                          << std::hex << c << std::dec << " replies/s " << reply_count[i] / 10.0 << " samples/s " << hz
                          << "\n";
                if (!every_other)
                    check(hz >= (k == 3 ? 100 : 150));  // host side keeps the model rate
                else
                    check(hz < (k == 3 ? 100 : 150));   // the measured radio cadence alone caps it below target
            }
            // Unenrolled peer index: never attributed to another chip.
            const auto before = x.decoded;
            x.slot(slot_of(alpha_reply(counter[3], 3, false), 3, 1, 1));
            check(x.decoded == before);
        }
    }
}
// Independent per-chip sample count of the Alpha bodies in a dump (marker & 3 per valid command 2).
void replay(const char *path) {
    std::ifstream in(path, std::ios::binary);
    check(bool(in));
    const std::array<uint32_t, 4> chips{0xA1B2C3D4, 0xA1B2C3E2, 0xA1B2C3E1, 0};
    Planes x(chips);
    std::map<uint32_t, uint64_t> expected, records, tick_range, dropped, odd_s1;
    unsigned tick_examples = 0;
    std::map<uint32_t, double> first, last;
    int64_t time;
    Bytes raw(112);
    uint32_t sequence = 0;
    while (in.read(reinterpret_cast<char *>(&time), 8) && in.read(reinterpret_cast<char *>(raw.data()), 112)) {
        Record r;
        try {
            r = parse_record(raw, true);
        } catch (const std::exception &) {
            continue;
        }
        if (time <= 0 || r.channel != 2 || r.dma.size() < 3 || r.dma.size() != size_t(r.dma[0]) + 2 || r.dma[0] > 71)
            continue;
        unsigned peer = 4;
        for (unsigned i = 0; i < 3; ++i)
            if (chips[i] == r.chip)
                peer = i;
        if (peer > 2)
            continue;
        ++records[r.chip];
        if (r.dma[1] != 2 && r.dma[1] != 3) {  // DataPlane drops these as malformed (live path identical)
            ++odd_s1[r.chip];
            continue;
        }
        if (!first.count(r.chip))
            first[r.chip] = double(time) / 1e9;
        last[r.chip] = double(time) / 1e9;
        try {
            for (const auto &c : decode_records(Bytes(r.dma.begin() + 2, r.dma.end())))
                if (c.command == 2 && c.body.size() >= 3 && (c.body[0] & 0xf8) == 0xc8 &&
                    c.body.size() == 3 + 16 * (c.body[0] & 3u) + ((c.body[0] & 4) ? 6u : 0u)) {
                    // decode_imu drops the whole body when a sample's 24-bit sync ticks is >= 533333 (data_plane.cpp)
                    bool ticks_ok = true;
                    for (unsigned s = 0; s < (c.body[0] & 3u); ++s) {
                        const size_t o = 3 + 16 * s + 12;
                        const uint32_t ticks = c.body[o] | uint32_t(c.body[o + 1]) << 8 | uint32_t(c.body[o + 2]) << 16;
                        if (ticks >= 533333) {
                            ticks_ok = false;
                            ++tick_range[r.chip];
                            if (tick_examples < 5) {
                                ++tick_examples;
                                std::cout << "  ticks out of range chip " << std::hex << r.chip << std::dec << " ticks "
                                          << ticks << " counter " << unsigned(c.body[o + 3]) << "\n";
                            }
                        }
                    }
                    if (ticks_ok)
                        expected[r.chip] += c.body[0] & 3u;
                    else
                        dropped[r.chip] += c.body[0] & 3u;
                }
        } catch (const std::exception &) {
        }
        x.planes[0]->clock.fit = Fit{int64_t(r.cycles), double(time), 15.625, 0};  // PTI1 stamps near the dump's host time
        x.slot(slot_of(r.dma, peer, ++sequence, r.cycles));
    }
    for (unsigned i = 0; i < 3; ++i) {
        const uint32_t c = chips[i];
        check(x.wrong[c] == 0);
        if (x.decoded[c] != expected[c])
            std::cerr << "replay chip " << std::hex << c << std::dec << " decoded " << x.decoded[c] << " expected "
                      << expected[c] << " malformed " << x.planes[i]->counters.malformed << " checksum "
                      << x.planes[i]->counters.checksum << " source " << x.planes[i]->counters.source << "\n";
        check(x.decoded[c] == expected[c]);
        const double span = last[c] - first[c] > 0 ? last[c] - first[c] : 1;
        std::cout << "replay " << path << " chip " << std::hex << c << std::dec << " records " << records[c]
                  << " decoded " << x.decoded[c] << " (" << x.decoded[c] / span << "/s over " << span << " s)"
                  << " PTI1 " << x.sent[c] << " bodies dropped for ticks>=533333: " << x.planes[i]->counters.malformed
                  << " (" << dropped[c] << " samples), records with S1 not 2/3: " << odd_s1[c] << "\n";
        check(x.planes[i]->counters.malformed == 0 || dropped[c] > 0);
    }
}
} // namespace
int main(int argc, char **argv) {
    try {
        first_target_restart();
        wake_grid();
        synthetic_streams();
        for (int i = 1; i < argc; ++i)
            replay(argv[i]);
        if (argc == 1)
            std::cout << "NOT RUN: radio.bin replay (supply PICO_RADIO_DUMP files)\n";
        std::cout << "multi_dataplane PASS " << checks << "\n";
        return 0;
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}
