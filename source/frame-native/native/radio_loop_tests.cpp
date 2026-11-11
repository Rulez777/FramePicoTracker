#include "native_test_runtime.hpp"
#include <algorithm>
#include "fake_openocd.hpp"
#include "radio_loop.hpp"
#include <iostream>
using namespace pico;
namespace {
size_t checks = 0;
void check(bool b, int line = __builtin_LINE()) {
    ++checks;
    if (!b)
        throw std::runtime_error("loop check " + std::to_string(checks) + " line " + std::to_string(line));
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
struct Fixture {
    FakeOpenOcd f;
    DebugMemory d;
    OwnerIO io;
    DataPlane plane;
    std::vector<std::string> events;
    std::vector<IterationLatency> stats;
    bool bad_guard = false, bad_poll = false;
    std::function<void()> on_poll;
    RadioLoop loop;
    Fixture()
        : d([this](const std::string &c) { return f.command(c); }), io(f.io(d)), plane(Hooks{}),
          loop(io, plane,
               {[this] {
                    events.push_back("guard");
                    if (bad_guard)
                        throw std::runtime_error("guard");
                },
                [this](const Bytes &) {
                    events.push_back("poll");
                    check(word(f.read(roy_data + 116, 4)) == word(f.read(roy_data + 112, 4)));
                    f.ns += 2000000;
                    if (on_poll)
                        on_poll();
                    if (bad_poll)
                        throw std::runtime_error("poll");
                },
                [this](uint32_t, bool) { events.push_back("publish"); },
                [this] { events.push_back("repeat"); },
                [this](const IterationLatency &s) { stats.push_back(s); }}) {
        f.set(roy_data, 0x314d5250);
        plane.hooks.now = [this] { return f.ns; };
        plane.hooks.discovery = [this](const Record &) { events.push_back("record"); };
    }
    void ring(uint32_t r, uint32_t w) {
        f.set(roy_data + 112, w);
        f.set(roy_data + 116, r);
        for (uint32_t i = r; i != w; ++i) {
            Bytes b(112);
            b[12] = 1;
            auto seq = le32(i);
            std::copy(seq.begin(), seq.end(), b.begin());
            f.put(roy_data + 380 + (i % 10) * 112, b);
        }
    }
};
} // namespace
int main() {
    try {
        Fixture x;
        x.ring(8, 12);
        x.f.ns = 100000000;
        x.f.set(roy_data + 28, 3);
        x.f.set(roy_data + 12, 40);
        x.loop.step();
        check(x.events == std::vector<std::string>({"guard", "poll", "publish", "record", "record",
                                                    "record", "record", "repeat"}));
        check(x.stats.size() == 1 && x.stats[0].records == 4 && x.stats[0].complete);
        // Records arrived: 0.5 ms lock hand-off instead of the 5 ms idle sleep.
        check(x.stats[0].begin == 100000000 && x.stats[0].poll_done == 102000000 &&
              x.stats[0].wake == 102500000);
        check(word(x.f.read(roy_data + 1608, 4)) == 1040);
        // Used header words, both wrapped ring parts, ACK and heartbeat: one debugger command.
        check(x.f.transcript.size() == 1 && x.f.transcript[0].rfind("set pico_o {}", 0) == 0);
        check(x.f.transcript[0].find("[read_memory 0x20010c18 32 4] [read_memory 0x20010c34 32 1] "
                                     "[read_memory 0x20010c88 32 2]") != std::string::npos);
        check(word(x.f.read(roy_data + 116, 4)) == 12);
        // 7 header words; each record (DMA length 0) words 0..4 and 25..27.
        check(x.stats[0].batched && x.stats[0].tcl_commands == 1 &&
              x.stats[0].bytes_read == (7 + 4 * 8) * 4 && x.stats[0].bytes_written == 8);
        x.events.clear();
        x.loop.step();
        check(x.loop.last.records == 0);
        check(x.loop.last.begin == 102500000);
        check(x.loop.last.wake == 109500000);  // idle: 5 ms
        check(x.events == std::vector<std::string>({"guard", "poll", "repeat"}));
        // No full-header refresh any more: the same 7 words every iteration.
        // Same batched header/ring script; the 50 ms heartbeat write is only in the first iteration.
        check(x.f.transcript.size() == 2 && x.f.transcript[1].rfind("set pico_o {}", 0) == 0 &&
              x.f.transcript[1].find("write_memory 0x20011260") == std::string::npos);
        check(x.loop.last.tcl_commands == 1 && x.loop.last.bytes_read == 28 &&
              x.loop.last.bytes_written == 0);
        x.f.ns += 2000000000;
        x.loop.step();
        check(x.f.transcript[2].find(" 32 30]") == std::string::npos && x.loop.last.bytes_read == 28);
        {
            // Partial slot reads parse exactly as full slots: DMA lengths 0..80, unread bytes
            // full of garbage, the 9 -> 0 wrap, and a partial-CRC record still counted.
            Fixture a;
            std::vector<Record> got;
            a.plane.hooks.discovery = [&](const Record &r) { got.push_back(r); };
            const uint32_t lengths[] = {0, 5, 48, 72, 80, 20};
            std::vector<Record> expected;
            a.f.set(roy_data + 112, 12);
            a.f.set(roy_data + 116, 6);
            for (uint32_t i = 6; i != 12; ++i) {
                Bytes b(112);
                for (size_t k = 0; k < b.size(); ++k)
                    b[k] = uint8_t(i * 31 + k * 7 + 1);
                // Channel 1 (discovery); word 4 = DMA length | partial bits (bit 0 = partial).
                const auto channel = le32(1), length = le32(lengths[i - 6] | (i == 11 ? 5u : 4u) << 16);
                std::copy(channel.begin(), channel.end(), b.begin() + 12);
                std::copy(length.begin(), length.end(), b.begin() + 16);
                if (i != 11)
                    expected.push_back(parse_record(b, true));
                a.f.put(roy_data + 380 + (i % 10) * 112, b);
            }
            a.loop.step();
            check(a.f.transcript.size() == 1 && a.loop.last.records == 6);
            check(got.size() == 5 && a.plane.counters.crc == 1);
            for (size_t i = 0; i < got.size(); ++i)
                check(got[i].sequence == expected[i].sequence && got[i].crc == expected[i].crc &&
                      got[i].rssi == expected[i].rssi && got[i].channel == expected[i].channel &&
                      got[i].chip == expected[i].chip && got[i].cycles == expected[i].cycles &&
                      got[i].partial_bits == expected[i].partial_bits &&
                      got[i].callback_us == expected[i].callback_us &&
                      got[i].profile == expected[i].profile && got[i].slot == expected[i].slot &&
                      got[i].dma == expected[i].dma && got[i].dma.size() == lengths[i]);
            // 7 header words; per record 8 plus its DMA words 0, 2, 12, 18 (+2 unused read
            // through to the tail), 20, 5.
            check(a.loop.last.bytes_read == (7 + 6 * 8 + 57 + 2) * 4);
            check(a.loop.last.wake - a.loop.last.end == 500000);
        }
        {
            // A DMA length over 80 still fails parse_record as with a full slot read.
            Fixture a;
            a.ring(0, 1);
            a.f.set(roy_data + 380 + 16, 81);
            rejects([&] { a.loop.step(); });
            check(word(a.f.read(roy_data + 116, 4)) == 1);
        }
        {
            // Legacy per-region path (no OwnerIO::command) keeps the original transcript.
            FakeOpenOcd f;
            DebugMemory d([&](const std::string &c) { return f.command(c); });
            OwnerIO io = f.io(d);
            io.command = nullptr;
            DataPlane plane;
            RadioLoop loop(io, plane,
                           {[] {}, [](const Bytes &) {}, [](uint32_t, bool) {}, [] {},
                            [](const IterationLatency &) {}});
            f.set(roy_data, 0x314d5250);
            f.set(roy_data + 112, 12);
            f.set(roy_data + 116, 8);
            for (uint32_t i = 8; i != 12; ++i) {
                Bytes b(112);
                b[12] = 1;
                f.put(roy_data + 380 + (i % 10) * 112, b);
            }
            f.ns = 100000000;
            loop.step();
            check(f.transcript[0] == "read_memory 0x20010c18 32 30");
            check(f.transcript[1] == "read_memory 0x20011114 32 56");
            check(f.transcript[2] == "read_memory 0x20010d94 32 56");
            check(f.transcript[3] == "write_memory 0x20010c8c 32 {0xc}");
            check(!loop.last.batched && loop.last.tcl_commands == 5 && loop.last.records == 4);
        }
        {
            Fixture a;
            a.bad_guard = true;
            rejects([&] { a.loop.step(); });
            check(a.f.transcript.empty());
            check(!a.loop.last.complete);
        }
        {
            Fixture a;
            a.ring(0, 1);
            a.bad_poll = true;
            rejects([&] { a.loop.step(); });
            check(word(a.f.read(roy_data + 116, 4)) == 1);
            check(a.events == std::vector<std::string>({"guard", "poll"}));
            check(a.f.ns == 2000000);
        }
        {
            Fixture a;
            a.ring(0, 11);
            a.loop.step();  // owner rule: ring loss is counted and resynchronised
            check(a.plane.counters.ring_loss == 1);
        }
        {
            Fixture a;
            a.ring(0, 2);
            a.f.fail_at = 1;
            rejects([&] { a.loop.step(); });
            check(word(a.f.read(roy_data + 116, 4)) == 0);
            check(a.events.size() == 1);
            // Three consecutive batch failures are logged and per-region reads take over.
            a.f.fail_at = 2;
            rejects([&] { a.loop.step(); });
            a.f.fail_at = 3;
            rejects([&] { a.loop.step(); });
            a.loop.step();
            check(a.f.transcript[3] == "read_memory 0x20010c18 32 30" && !a.loop.last.batched);
            check(word(a.f.read(roy_data + 116, 4)) == 2 && a.loop.last.records == 2);
        }
        {
            Fixture a;
            a.f.set(roy_data, 0);
            a.loop.step();  // missing module magic is logged, the loop goes on
            check(a.loop.last.complete);
        }
        {
            Fixture a;
            a.f.set(roy_data + 4, 1);
            a.loop.step();  // heartbeat flag is logged, never ends the run
            check(a.loop.last.complete);
        }
        // Exact >50ms heartbeat predicate and relative sleep; no catch-up iterations.
        {
            Fixture a;
            a.f.ns = 48000000;
            a.loop.step();
            check(a.events == std::vector<std::string>({"guard", "poll"}));
            a.events.clear();
            a.loop.step();
            check(a.events == std::vector<std::string>({"guard", "poll", "publish"}));
        }
        // Snapshot's existing fresh-session rule rejects counter wrap, as Phase 1.
        {
            Fixture a;
            a.f.set(roy_data + 112, 0);
            a.f.set(roy_data + 116, 0xffffffff);
            a.loop.step();
            check(a.plane.counters.ring_loss == 1);
        }
        {
            // Station reads repeated every iteration ride in the next batch; mailbox writes
            // from poll/dispatch leave as one command holding only the latest value.
            Fixture a;
            DebugMemory station([&a](const std::string &c) { return a.f.command(c); });
            a.f.set(roy_data + 1516, 3);
            std::vector<Bytes> seen;
            a.on_poll = [&] {
                seen.push_back(station.read_many({{roy_data + 1516, 16}}));
                station.write_many({{roy_data + 1664, Bytes(4, 1)}});
                station.write_many({{roy_data + 1664, Bytes(4, 2)}});
            };
            a.loop.step();
            check(a.f.transcript.size() == 3 && a.loop.last.tcl_commands == 3);
            check(a.f.transcript[1] == "concat [read_memory 0x20011204 32 4]");
            check(a.f.transcript[2] == "write_memory 0x20011298 32 {0x2020202}");
            a.f.set(roy_data + 1516, 4);
            a.loop.step();
            check(a.f.transcript.size() == 5 && a.loop.last.tcl_commands == 2);
            check(a.f.transcript[3].find("[read_memory 0x20011204 32 4]") != std::string::npos);
            check(seen.size() == 2 && word(seen[1]) == 4);
            check(a.loop.last.bytes_read == 28 + 16 && a.loop.last.bytes_written == 4);
        }
        {
            // A due probe is parked for the loop: alone before the batch while batches are
            // long (unknown at first), folded into the batch once the last batch took <=6 ms.
            Fixture a;
            std::vector<ClockProbeEvent> probes;
            RoyClockPoll probe(
                a.io, a.plane.clock, [&] { return a.f.ns + 10000000; },
                [&](const ClockProbeEvent &e) { probes.push_back(e); });
            a.f.set(0xe0001004, 64000000);
            a.f.ns = 50000001;
            probe.poll(true);
            check(probes.empty() && a.f.transcript.empty() && a.plane.clock.probe_due);
            a.loop.step();
            check(probes.size() == 1 && probes[0].cycles == 64000000 && !a.plane.clock.probe_due);
            check(a.f.transcript.size() == 2 && a.f.transcript[0] == "read_memory 0xe0001004 32 1");
            check(a.loop.last.probe && !a.loop.last.probe_folded && a.loop.last.tcl_commands == 2);
            a.f.ns += 60000000;
            a.f.set(0xe0001004, 65000000);
            probe.poll(true);
            a.loop.step();
            // 2026-10-31: the probe always runs alone now (folding biased the Roy clock with the fast SWD).
            check(probes.size() == 2 && probes[1].cycles == 65000000 && a.f.transcript.size() == 4);
            check(a.f.transcript[2] == "read_memory 0xe0001004 32 1");
            check(a.loop.last.probe && !a.loop.last.probe_folded && a.loop.last.tcl_commands == 2);
        }
        {
            RoyClock clock;  // no radio loop carries the probe: RoyClockPoll reads it itself
            Fixture a;
            std::vector<ClockProbeEvent> probes;
            std::vector<std::string> order;
            auto io = a.io;
            io.read = [&](uint32_t address, size_t n) {
                order.push_back("read");
                a.f.ns += 1000000;
                return a.d.read(address, n);
            };
            RoyClockPoll probe(
                io, clock,
                [&] {
                    order.push_back("raw");
                    return a.f.ns + 10000000;
                },
                [&](const ClockProbeEvent &e) { probes.push_back(e); });
            a.f.ns = 50000000;
            probe.poll(false);
            check(probes.empty() && order.empty());
            probe.poll(true);
            check(probes.empty());
            a.f.ns = 50000001;
            a.f.set(0xe0001004, 64000000);
            probe.poll(true);
            check(order == std::vector<std::string>({"raw", "read", "raw"}));
            check(probes.size() == 1 && probes[0].before == 50000001 &&
                  probes[0].after == 51000001 && probes[0].raw_before == 60000001 &&
                  probes[0].raw_after == 61000001);
            a.f.ns = 8050000000LL;
            probe.poll(true);
            a.f.ns = 8050000001LL;
            probe.poll(true);  // unadmitted clock is logged, fitting continues
        }
        std::cout << checks << " v4 loop checks PASS (injected time/fake TCL)\n";
        return 0;
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}
