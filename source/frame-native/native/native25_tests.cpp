#include "native_test_runtime.hpp"
#include "native25.hpp"
#include "query_peer.hpp"
#include "fake_openocd.hpp"
#include <algorithm>
#include <iostream>
#include <limits>
using namespace pico;
namespace {
size_t checks = 0;
void check(bool b, int line = __builtin_LINE()) { ++checks; if (!b) throw std::runtime_error("native25 check " + std::to_string(checks) + " line " + std::to_string(line)); }
template<class F> void rejects(F f, int line = __builtin_LINE()) { bool hit = false; try { f(); } catch (const std::exception &) { hit = true; } check(hit, line); }
constexpr uint32_t chip = 0xA1B2C3D4;
constexpr int64_t base = 1000000000;
void ledger_tests() {
    rejects([] { LedModeEpisodes bad(0); });
    LedModeEpisodes l(chip);
    for (unsigned mask = 0; mask < 7; ++mask)
        rejects([&] { l.begin(chip, mask & 1, mask & 2, mask & 4); });
    rejects([&] { l.begin(chip + 1, true, true, true); });
    check(!l.identity_verified() && !l.restoration_due(base));
    l.begin(chip, true, true, true);
    check(l.restore_required && l.active->token == 1);
    rejects([&] { l.begin(chip, true, true, true); });
    for (uint32_t tag : {0u, 1u, 3u, 65536u})
        rejects([&] { l.published({1}, tag, base); });
    rejects([&] { l.published({3}, 2, base); });
    rejects([&] { l.published({1}, 2, 0); });
    l.published({1}, 0x10002, base);
    l.published({1}, 4, base + 100000000);
    check(l.active->off_published == base && l.active->off_tags == std::set<uint16_t>({2, 4}));
    check(!l.restoration_due(base + 249999999));
    check(l.restoration_due(base + 250000000));
    check(!l.restoration_due(base - 1));
    for (uint32_t tag : {0u, 3u, 6u, 65538u})
        check(!l.acknowledge(chip, {1}, tag, base + 1, true));
    check(!l.acknowledge(chip + 1, {1}, 2, base + 1, true));
    check(!l.acknowledge(chip, {1}, 2, base + 1, false));
    check(l.acknowledge(chip, {1}, 2, base + 1000000, true, 1000000));
    check(l.acknowledge(chip, {1}, 4, base + 150000000, true));
    check(l.active->off_ack == base + 1000000 && l.active->off_error == 1000000);
    l.published({2}, 6, base + 250000000);
    rejects([&] { l.published({1}, 8, base + 260000000); });
    check(!l.acknowledge(chip, {2}, 4, base + 260000000, true));
    check(l.acknowledge(chip, {2}, 6, base + 260000000, true));
    check(l.restore_required && !l.identity_verified());
    l.restored_application(true, false);  // no measured lit return yet: keep waiting
    check(l.active && l.restore_required && l.episodes.empty());
    l.deadline(base + 800000000);
    check(!l.failed);
    l.restored_application(true, true);
    check(!l.active && !l.restore_required && l.episodes.size() == 1);
    l.begin(chip, true, true, true);
    l.published({1}, 8, base);
    l.published({2}, 10, base + 250000000);
    check(l.acknowledge(chip, {2}, 10, base + 260000000, true));
    l.restored_application(true, true);
    l.begin(chip, true, true, true);  // no episode cap: re-identification may repeat
    check(l.active->token == 3 && l.episodes.size() == 2 && l.restore_required);
    // A measured lit return without ON ACK / fresh mode is logged and accepted.
    LedModeEpisodes visible(chip);
    visible.begin(chip, true, true, true);
    visible.published({1}, 2, base);
    visible.restored_application(false, true);
    check(!visible.active && !visible.restore_required && visible.episodes.size() == 1);
    LedModeEpisodes lost(chip);
    lost.begin(chip, true, true, true);
    lost.published({1}, 2, base);
    lost.deadline(base + 800000001);  // overdue OFF is logged; restoration is forced
    check(lost.failed && lost.restore_required && lost.active && !lost.identity_verified());
    lost.restored_application(true, true);
    check(!lost.failed && !lost.active && !lost.restore_required);
}
struct Fixture {
    int64_t ns = base;
    double seconds = 1;
    FakeOpenOcd fake;
    DebugMemory debug;
    MailboxWriter writer;
    PeerEngine e;
    RoyClock clock;
    NativeLedRadioOwner owner;
    Native25AckBridge bridge;
    Fixture() : debug([this](const std::string &s) { return fake.command(s); }), writer(debug),
        e(le32(chip), 0, [this] { return seconds; }, [] { return Bytes(16, 7); }),
        owner(chip, "live"), bridge(owner, e, clock, "live", [this] { return ns; }) {
        e.state = PeerState::Streaming;
        e.mode_required = true;
        e.mode_profile = "frame";
        e.mode_confirmed_at = seconds;
        e.last_reply = seconds;
        clock.fit = Fit{0, double(ns), 15.625, 0};
    }
    bool begin() { return owner.begin(e, "live", ns, true, true); }
    bool publish() { return bridge.publish(writer, 0, e.mailbox()); }
    Record record(uint32_t tag) {
        Record r;
        r.hardware_crc = true; r.channel = 2; r.profile = 0; r.chip = chip;
        r.generation = tag;
        r.dma = {4, 2, 0, 0, 0, 0}; // command0 empty, checksum0
        return r;
    }
    bool ack(Record r) {
        bool result = false;
        bridge.record(r, [&] { result = e.receive(Bytes(r.dma.begin()+2, r.dma.end()),
            r.dma[1], static_cast<uint16_t>(r.generation), r.hardware_crc); });
        return result;
    }
    void time(int64_t delta) { ns = base + delta; seconds = double(ns) / 1e9; }
    void source(int64_t stamp, double error = 0) { clock.fit = Fit{0, double(stamp), 15.625, error}; }
};
void bridge_owner_tests() {
    {
        Fixture f;
        f.e.queued.push_back({17, {7}});
        check(!f.begin());  // busy pipeline: not ready, try later
        check(!f.owner.restore_required() && !f.e.native25_exclusive);
        f.e.queued.clear(); f.e.last_reply = .75;
        check(!f.begin());  // reply not fresh: not ready
        check(!f.owner.restore_required());
        f.e.last_reply = 1; check(f.begin());
        check(f.e.native25_exclusive && f.owner.restore_required());
        const auto g = f.e.pending->generation;
        check(!f.e.ack_tag(static_cast<uint16_t>(g)));
        check(!f.ack(f.record(g))); // allocated is not published
        check(f.publish());
        check(!f.publish());
        check(f.owner.snapshot()->off_published == base);
        // Every canonical context condition is necessary. Prior diagnostics run.
        for (int i = 0; i < 12; ++i) {
            auto r = f.record(g);
            switch(i) {
            case 0:r.hardware_crc=false;break; case 1:r.partial=true;break;
            case 2:r.channel=1;break;case 3:r.profile=1;break;case 4:r.chip++;break;
            case 5:r.dma.clear();break;case 6:r.dma[0]++;break;case 7:r.dma[1]=4;break;
            case 8:r.dma.back()=1;break;case 9:r.generation=0;break;
            case 10:r.generation=65536+g;break;case 11:r.generation=g+2;break;
            }
            bool previous = false;
            f.bridge.record(r, [&] { previous = true; check(!f.e.ack_tag(static_cast<uint16_t>(r.generation))); });
            check(previous && f.e.pending.has_value() && !f.owner.snapshot()->off_ack);
        }
        // Exact tag attributes the ACK; uncertainty only widens the window. An ACK whose
        // upper bound still precedes publication, or with no clock, is dropped.
        f.source(base - 2, 1); check(!f.ack(f.record(g)));
        f.source(base - 1000002, 1000001); check(!f.ack(f.record(g)));
        f.clock.fit.reset(); check(!f.ack(f.record(g)));
        check(f.e.pending && !f.owner.snapshot()->off_ack);
        f.source(base + 100000000); f.time(249999999);
        f.owner.tick(f.e, "live", f.ns); check(f.e.pending->body == Bytes{1});
        f.time(250000000); f.owner.tick(f.e, "live", f.ns);
        check(f.e.pending->generation == g && f.e.sequences[25] == 0);
        check(f.ack(f.record(g)));
        check(!f.e.pending && f.owner.snapshot()->off_ack == base + 100000000);
        check(f.e.sequences[25] == 1 && !f.e.native25_ack_retiring);
        f.e.queued.push_back({17, {7}});
        f.owner.tick(f.e, "live", f.ns); check(!f.e.pending);
        f.e.queued.clear(); f.owner.tick(f.e, "live", f.ns);
        check(f.e.pending->body == Bytes{2} && f.e.pending->sequence == 1);
        check(f.owner.restore_started && f.owner.restore_required());
        check(f.publish());
        const auto on = f.e.pending->generation;
        f.time(300000000); f.source(f.ns);
        check(f.ack(f.record(on)));
        check(f.owner.restore_required());
        f.owner.optical_return(f.e, "live", false);  // no lit return yet: keep waiting
        check(f.owner.restore_required() && f.owner.completed() == 0);
        f.owner.optical_return(f.e, "live", true);
        check(!f.owner.restore_required() && !f.e.native25_exclusive && f.owner.completed() == 1);
    }
    {
        Fixture f; f.begin(); f.publish();
        f.e.pending.reset(); // disconnect-like disappearance is not exact retirement
        f.time(250000000); f.owner.tick(f.e, "live", f.ns);
        check(!f.e.pending && !f.owner.restore_started);
        f.time(800000000); f.owner.tick(f.e, "live", f.ns);
        check(!f.e.pending && !f.owner.restore_started);
        // Overdue OFF without ACK: logged, ON is sent unconditionally, no abort.
        f.time(800000001); f.owner.tick(f.e, "live", f.ns);
        check(!f.owner.abort_requested && f.owner.restore_started);
        check(f.e.pending && f.e.pending->body == Bytes{2});
        check(f.owner.restore_required() && f.e.native25_exclusive);
    }
    {
        Fixture f; f.begin(); f.publish();
        f.e.begin(19, {0}); f.time(250000000);
        f.owner.tick(f.e, "live", f.ns);  // foreign pending command: logged, ON waits
        check(!f.owner.abort_requested && f.owner.restore_required());
        check(f.e.pending->command == 19 && !f.owner.restore_started);
        f.e.pending.reset();
        f.time(800000001); f.owner.tick(f.e, "live", f.ns);
        check(f.e.pending && f.e.pending->body == Bytes{2} && f.owner.restore_started);
    }
    {
        Fixture f; f.begin(); f.publish();
        f.owner.tick(f.e, "other-link", f.ns);  // link mismatch is logged
        check(!f.owner.abort_requested && f.owner.restore_required());
        check(f.e.pending && f.e.pending->body == Bytes{1});
    }
    // TCL request failure before/after commit leaves no publication credit;
    // retry has to complete the actual odd/data/even transaction.
    for (size_t point = 1; point <= 1; ++point) for (bool after : {false, true}) {
        Fixture f; f.begin(); f.fake.fail_at = point; f.fake.fail_after = after;
        rejects([&] { f.publish(); });
        check(!f.owner.snapshot()->off_published && f.owner.restore_required());
        f.fake.fail_at = 0; check(f.publish());
        check(f.owner.snapshot()->off_published == base);
    }
    {
        Fixture f; f.begin(); f.publish();
        auto g = f.e.pending->generation;
        f.e.retime(); auto allocated = f.e.pending->generation;
        check(!f.ack(f.record(allocated)));
        f.time(100000000); check(f.publish());
        check(f.owner.snapshot()->off_published == base && f.owner.snapshot()->off_tags.size() == 2);
        f.source(f.ns); check(f.ack(f.record(g))); // exact older published attempt still permitted
    }
    {
        Fixture f;
        rejects([&] { f.bridge.record(f.record(2), [&] { f.bridge.record(f.record(2), [] {}); }); });
        rejects([&] { f.bridge.record(f.record(2), [] { throw std::runtime_error("diagnostic failure"); }); });
        f.begin(); f.publish(); f.source(base + 1); f.ns++;
        check(f.ack(f.record(f.e.pending->generation))); // context cleared after both exceptions
    }
    {
        Fixture f; f.begin();
        auto m = f.e.mailbox(); m.payload = {0};
        rejects([&] { f.bridge.publish(f.writer, 0, m); });
        check(f.fake.operations == 0 && !f.owner.snapshot()->off_published);
    }
}
void query_retirement_test() {
    int64_t ns = base;
    double t = 1;
    unsigned ticks = 0;
    NativeLedRadioOwner owner(chip, "live");
    QueryPeer *peer = nullptr;
    QueryHooks hooks;
    hooks.bench = [] { return BenchImuEvidence{}; };
    hooks.fresh_guard = [] {};
    hooks.native_radio_tick = [&] {
        ++ticks;
        check(peer && !peer->native25_ack_retiring);
        owner.tick(*peer, "live", ns);
        return owner.snapshot().has_value();
    };
    hooks.proof = [](unsigned, const Bytes &, const Page &) {};
    hooks.action = [](uint8_t, const Bytes &) {};
    hooks.prerequisites = [](const std::vector<std::pair<unsigned, Page>> &, const std::string &) {};
    hooks.producer_lifetime = [](double) {};
    QueryPeer e(le32(chip), 0, [&] { return t; }, [] { return Bytes(16, 1); }, hooks);
    peer = &e;
    e.state = PeerState::Streaming; e.mode_required = true;
    e.mode_profile = "frame"; e.mode_confirmed_at = t; e.last_reply = t;
    e.diagnostic_index = 22;
    RoyClock clock;
    clock.fit = Fit{0, double(base + 100000000), 15.625, 0};
    {
    Native25AckBridge bridge(owner, e, clock, "live", [&] { return ns; });
    FakeOpenOcd fake;
    DebugMemory debug([&](const std::string &s) { return fake.command(s); });
    MailboxWriter writer(debug);
    owner.begin(e, "live", ns, true, true);
    const auto off = e.pending->generation;
    check(bridge.publish(writer, 0, e.mailbox()));
    ns += 250000000; t = 1.25;
    Record r;
    r.hardware_crc = true; r.channel = 2; r.profile = 0; r.chip = chip;
    r.generation = off; r.dma = {4, 2, 0, 0, 0, 0};
    bridge.record(r, [&] { check(e.receive({0, 0, 0, 0}, 2, static_cast<uint16_t>(off), true)); });
    check(ticks == 0 && !e.pending && owner.snapshot()->off_ack == base + 100000000);
    e.advance();
    check(ticks == 1 && e.pending && e.pending->body == Bytes{2});
    }
    // Removing an active bridge must leave no raw shortcut to retire ON.
    check(owner.abort_requested && owner.restore_required());
    check(e.pending && !e.ack_tag(static_cast<uint16_t>(e.pending->generation)));
}
LedEpisode episode() {
    LedEpisode e; e.chip=chip; e.token=1; e.off_tags={2}; e.on_tags={4};
    e.off_published=base+100000000; e.off_ack=base+100000000;
    e.on_published=base+350000000; e.on_ack=base+350000000;
    return e;
}
std::vector<IdentityFrameTime> frames() {
    std::vector<IdentityFrameTime> v;
    for (int64_t i=0; i<18; ++i) {
        const auto t=base+i*33333333;
        v.push_back({t,i,0,t<base+100000000 || t>base+383333333});
    }
    return v;
}
void window_tests() {
    auto e=episode(); auto v=frames();
    auto w=identity_window(e,chip,v);
    check(w && w->before==std::vector<size_t>({1,2,3}) && w->after==std::vector<size_t>({12,13,14}) &&
          w->dark==std::vector<size_t>({6,7,8,9}));
    // r12: late ACK cannot shift pre-OFF bracket after first publication.
    e.off_ack=base+150000000;
    v[4].measured_world=true;
    auto r12=identity_window(e,chip,v);
    check(r12 && r12->off_bound==*e.off_published && r12->before==w->before);
    // r12 ON bound: frames after publication cannot be counted as dark, even
    // when ON transport evidence arrives later.
    e.on_ack=base+450000000;
    r12=identity_window(e,chip,v);
    check(r12 && r12->on_bound==*e.on_published && r12->dark==std::vector<size_t>({7,8,9}));
    e=episode();
    for (int which=0; which<13; ++which) {
        auto bad=e; auto vf=v;
        switch(which) {
        case 0:bad.off_ack.reset();break;case 1:bad.on_ack.reset();break;
        case 2:bad.off_published.reset();break;case 3:bad.on_published.reset();break;
        case 4:bad.off_tags.clear();break;case 5:bad.on_tags.clear();break;
        case 6:bad.chip++;break;case 7:bad.on_ack=*bad.off_ack+100000000;break;
        case 8:bad.on_ack=*bad.off_ack+500000000;break;
        case 9:bad.off_error=-1;break;  // negative is invalid; large uncertainty is allowed now
        case 10:vf[6].measured_world=true;break;
        case 11:vf[2].measured_world=false;break;
        case 12:vf[7].error=-1;break;
        }
        check(!identity_window(bad,chip,vf));
    }
    v=frames(); v.erase(v.begin()+7); check(!identity_window(e,chip,v));
    v=frames(); v[7].index++; check(!identity_window(e,chip,v));
    v=frames(); v[7].stamp=v[6].stamp; check(!identity_window(e,chip,v));
    v=frames(); v[7].stamp=v[6].stamp+26666666; check(!identity_window(e,chip,v));
    v=frames(); v[7].stamp=v[6].stamp+40000000; check(!identity_window(e,chip,v));
    v=frames(); check(!identity_window(e,chip,v,33333334));
    e.off_error=1; e.on_error=1; check(identity_window(e,chip,v).has_value());
    e.on_ack=*e.off_ack+100000002; check(!identity_window(e,chip,v));
    e.on_ack=*e.off_ack+499999998; check(!identity_window(e,chip,v));
}
}
int main() {
    try { ledger_tests(); bridge_owner_tests(); query_retirement_test(); window_tests();
        std::cout << "native25: " << checks << " checks PASS\n"; return 0;
    } catch (const std::exception &e) { std::cerr << e.what() << '\n'; return 1; }
}
