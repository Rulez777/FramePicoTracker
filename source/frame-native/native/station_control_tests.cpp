#include "native_test_runtime.hpp"
#include "fake_openocd.hpp"
#include "radio_loop.hpp"
#include "station_control.hpp"
#include <iostream>
using namespace pico;
namespace {
size_t checks = 0;
void check(bool b) {
    ++checks;
    if (!b)
        throw std::runtime_error("station check " + std::to_string(checks));
}
template <class F> void rejects(F f) {
    bool b = false;
    try {
        f();
    } catch (const std::exception &) {
        b = true;
    }
    check(b);
}
} // namespace
int main() {
    try {
        FakeOpenOcd f;
        DebugMemory d([&](const std::string &c) { return f.command(c); });
        PeerEngine e(
            {0xd4, 0xc3, 0xb2, 0xa1}, 0, [&] { return f.ns / 1e9; }, [] { return Bytes(16, 1); });
        DataPlane p;
        std::vector<std::string> events;
        StationControl s(d, p, e,
                         {[&] { events.push_back("optics"); },
                          [&](uint32_t chip) {
                              check(chip == 0xA1B2C3D4);
                              events.push_back("reset");
                          },
                          [&](int slot, PeerEngine &engine, MailboxWriter &writer) {
                              auto m = engine.mailbox();
                              writer.publish(slot, m.generation, m.s1, m.payload);
                              events.push_back("mailbox");
                          },
                          [&](int, std::optional<uint8_t>, uint32_t, std::optional<uint16_t>) {
                              events.push_back("ack");
                          },
                          [&](PeerEngine &engine) { engine.advance(); },
                          [&](PeerEngine &engine, uint16_t tag) { return engine.ack_tag(tag); },
                          [&](PeerEngine &engine, const Record &r) {
                              return engine.receive(Bytes(r.dma.begin() + 2, r.dma.end()), r.dma[1],
                                                    static_cast<uint16_t>(r.tx_generation()),
                                                    r.hardware_crc);
                          }});
        p.hooks.now = [&] { return f.ns; };
        p.hooks.canonical = [&](const Record &r, const std::vector<Command> &,
                                const std::optional<AckContext> &) {
            events.push_back("canonical");
            s.canonical(r);
        };
        p.hooks.feedback = [&](uint8_t c) { s.feedback(c); };
        p.hooks.mailbox = [&] { s.mailbox(); };
        p.hooks.decoded = [&](const Record &, const Sample &) { events.push_back("decoded"); };
        p.enrolled = true;
        f.set(roy_data, 0x314d5250);
        f.set(roy_data + 1504 + 12, 3);
        s.poll();
        check(e.state == PeerState::Confirming);
        check(events == std::vector<std::string>({"optics", "mailbox"}));
        check(f.transcript[0] == "concat [read_memory 0x20011204 32 4]");
        // A checksum-valid real sample reaches control before decoded feedback.
        Record r;
        r.hardware_crc = true;
        r.channel = 2;
        r.profile = 0;
        r.chip = 0xA1B2C3D4;
        r.slot = 1;
        r.generation = 0;
        Bytes payload{0, 2, 18};
        payload.resize(21);
        payload.push_back(20);
        r.dma = {static_cast<uint8_t>(payload.size()), 3};
        r.dma.insert(r.dma.end(), payload.begin(), payload.end());
        events.clear();
        p.record(r);
        check(events == std::vector<std::string>({"canonical", "decoded", "mailbox"}));
        check(s.replies == 1 && p.counters.decoded == 1);
        p.record(r);
        check(s.replies == 1);
        r.slot = 2;
        p.record(r);
        check(s.replies == 2);
        // Rediscovery counter changes invalidate optical and data-plane enrollment,
        // even if the MCU stage recovered to3 between polls.
        e.state = PeerState::Streaming;
        e.phase_verified = true;
        f.set(roy_data + 1504 + 20, 1);
        events.clear();
        s.poll();
        check(events == std::vector<std::string>({"optics", "reset", "mailbox"}));
        check(!p.enrolled && !e.phase_verified && e.state == PeerState::Confirming);
        // Dead-link advance resets MCU stage and peer, never preserving enrollment.
        f.ns = 3000000000;
        events.clear();
        s.poll();
        check(word(f.read(roy_data + 1504 + 12, 4)) == 0);
        check(e.state == PeerState::Absent);
        configure_dual_mode(e, 1, 2, true);
        check(e.mode_configurations.at("stock") ==
              Configuration({{16, {1, 2}}, {17, {7}}, {25, {2}}}));
        check(e.mode_configurations.at("frame") ==
              Configuration({{16, {2, 2}}, {17, {7}}, {25, {2}}}));
        rejects([&] { configure_dual_mode(e, 1, 2, true, 8661.999); });
        configure_dual_mode(e, 1, 2, true, 8662);
        configure_dual_mode(e, 1, 2, true, 9000);
        rejects([&] { configure_dual_mode(e, 1, 2, true, 9000.001); });
        // Policy callbacks are mandatory, not placeholders for omitted guards.
        rejects([&] { StationControl missing(d, p, e, {}); });
        std::cout << checks << " station control/integration checks PASS (fake TCL)\n";
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << "\n";
        return 1;
    }
}
