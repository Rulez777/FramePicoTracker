#include "native_test_runtime.hpp"
#include "fake_openocd.hpp"
#include "peer_engine.hpp"
#include "guard_evidence.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono>
using namespace pico;
namespace {
size_t checks = 0;
std::string context;
void check(bool b, int line = __builtin_LINE()) {
    ++checks;
    if (!b)
        throw std::runtime_error("peer check " + std::to_string(checks) + " line " + std::to_string(line) + " " + context);
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
Bytes packet(uint8_t command, const Bytes &body) {
    Bytes b{0, command, static_cast<uint8_t>(body.size())};
    b.insert(b.end(), body.begin(), body.end());
    unsigned sum = 0;
    for (auto x : b)
        sum += x;
    b.push_back(static_cast<uint8_t>(sum));
    return b;
}
struct Fixture {
    double t = 0;
    uint8_t nonce = 0;
    uint8_t revision = 7;
    std::vector<std::string> events;
    PeerEngine e;
    Fixture()
        : e(
              {0xd4, 0xc3, 0xb2, 0xa1}, 0, [this] { return t; },
              [this] { return Bytes(16, ++nonce); }) {
        e.recovery_event = [this](const std::string &s, unsigned, uint8_t, const Bytes &) {
            events.push_back(s);
        };
    }
    void mode() {
        e.mode_required = true;
        e.desired_mode = "frame";
        e.mode_query_schema = 2;
        e.mode_configurations = {{"stock", {{16, {1, 2}}}},
                                 {"frame", {{16, {2, 2}}, {17, {7}}, {25, {2}}}}};
        e.query_mode();
    }
    Bytes proof(uint8_t profile = 1, uint8_t n = 2, uint8_t pending = 0) {
        Bytes b{0xfe, 2};
        b.insert(b.end(), e.mode_nonce->begin(), e.mode_nonce->end());
        b.insert(b.end(), e.chip.begin(), e.chip.end());
        b.insert(b.end(), {profile, revision, n, pending});
        return packet(1, b);
    }
    void fresh() {
        e.mode_required = true;
        e.mode_profile = "frame";
        e.mode_confirmed_at = t;
        e.state = PeerState::Streaming;
        e.last_reply = t;
        e.phase_verified = true;
    }
};
} // namespace
int main() {
    try {
        { // Initial mode-query nonce refusals must be visible to family 12.
            Fixture f;f.mode();
            std::vector<bool> decisions;
            guard_evidence::sink=[&](unsigned family,const std::string& guard,bool accepted){
                if(family==12 && guard=="nonce-link")decisions.push_back(accepted);
            };
            auto body=decode_records(f.proof()).front().body;
            body[2]^=1;
            f.e.receive(packet(1,body),2,0,true);
            check(decisions==std::vector<bool>{false});
            check(f.e.mode_nonce.has_value() && !f.e.mode_profile);
            f.e.receive(f.proof(),2,0,true);
            check(decisions==std::vector<bool>({false,true}));
            check(!f.e.mode_nonce && f.e.mode_profile==std::optional<std::string>("frame"));
            guard_evidence::sink={};
        }
        {
            Fixture f;
            auto m = f.e.begin(17, {7});
            check(m.generation == 2 && m.s1 == 2 && m.payload == Bytes({0, 17, 1, 7, 25}));
            f.e.begin(25, {2});
            check(f.e.mailbox().payload == Bytes({0, 25, 1, 2, 28}));
            f.e.begin(19, {0});
            check(f.e.mailbox().payload == Bytes({0, 19, 1, 0, 20}));
        }
        {
            Fixture f;
            f.e.generation = 65534;
            check(f.e.next_generation() == 65538);
            f.e.generation = 0xfffffffe;
            check(f.e.next_generation() == 2);
        }
        {
            Fixture f;
            auto m = f.e.start();
            check(f.e.state == PeerState::Confirming && !f.e.pending && m.payload == Bytes{0} &&
                  m.generation == 2);
            Bytes product(35);
            product[0] = 0x82;
            check(!f.e.receive(packet(1, product), 3, 0, true));
            check(f.e.pending->command == 17 && f.e.sequences[9] == 1);
            check(f.e.ack_tag(static_cast<uint16_t>(f.e.pending->generation)));
            check(f.e.pending->command == 25);
            check(f.e.ack_tag(static_cast<uint16_t>(f.e.pending->generation)));
            check(f.e.pending->command == 19);
            check(f.e.ack_tag(static_cast<uint16_t>(f.e.pending->generation)));
            check(!f.e.pending && f.e.state == PeerState::WaitSync);
            check(!f.e.phase_verified);
            f.e.queue_exposure_sync(533332, 1, true, true);
            check(f.e.pending->command == 14);
            f.e.ack_tag(static_cast<uint16_t>(f.e.pending->generation));
            check(f.e.pending->command == 10);
            check(f.e.mailbox().payload == Bytes({0, 10, 4, 0x54, 0x23, 8, 0, 0x8d}));
            f.e.ack_tag(static_cast<uint16_t>(f.e.pending->generation));
            check(f.e.phase_verified);
            f.e.receive(packet(2, Bytes(18)), 3, 0, true);
            check(f.e.state == PeerState::Streaming);
        }
        for (double age : {.5, .500001, -.000001}) {
            Fixture f;
            Bytes product(35);
            product[0] = 0x82;
            f.e.receive(packet(1, product), 3, 0, true);
            f.t = age;
            f.e.start();
            check(bool(f.e.pending) == (age == .5));
        }
        {
            Fixture f;
            f.e.configure(uint8_t(2), true);
            check(f.e.configuration == Configuration({{16, {2, 2}}, {17, {7}}, {25, {2}}}));
            f.e.configure(uint8_t(1), false, uint8_t(0x81));
            check(f.e.configuration == Configuration({{16, {1, 2}}, {17, {0x81}}, {25, {2}}}));
        }
        {
            Fixture f;
            f.e.configure(uint8_t(2), false);
            check(f.e.configuration.back() == std::make_pair(uint8_t(19), Bytes{0}));
        }
        {
            Fixture f;
            auto m = f.e.begin(17, {7});
            const auto first = m.generation;
            for (int i = 0; i < 9; ++i) {
                f.t += .1;
                f.e.retime();
            }
            check(f.e.pending->attempts.size() == 8);
            check(f.e.pending->sequence == 0);
            check(f.e.pending->first_sent > .29 && f.e.pending->first_sent < .31);
            check(!f.e.ack_tag(static_cast<uint16_t>(first)));
            const auto tag = static_cast<uint16_t>(f.e.pending->attempts.front());
            check(f.e.ack_tag(tag));
            check(f.e.acked_generation == tag && f.e.sequences[17] == 1);
        }
        {
            Fixture f;
            f.e.begin(17, {7});
            const auto old = f.e.generation;
            f.e.ack_notification = true;
            auto m = f.e.mailbox();
            check(m.generation == old + 2 && m.s1 == 3);
            check(f.e.pending->attempts.size() == 2);
            check(f.e.ack_tag(static_cast<uint16_t>(old)));
        }
        {
            Fixture f;
            f.e.sequences[17] = 63;
            f.e.begin(17, {7});
            check(f.e.mailbox().payload[0] == 63);
            f.e.ack_tag(static_cast<uint16_t>(f.e.generation));
            check(f.e.sequences[17] == 0);
        }
        {
            Fixture f;
            f.e.begin(19, {0});
            f.e.command19_hold_s = .5;
            check(!f.e.ack_tag(2));
            f.t = .5;
            check(f.e.ack_tag(2));
        }
        {
            Fixture f;
            f.e.begin(17, {7});
            check(!f.e.receive({0}, 1, 2, true));
            check(!f.e.receive({0}, 2, 2, false));
            check(!f.e.receive({1}, 2, 2, true));
            check(f.e.rejected == 3 && !f.e.last_reply);
        }
        for (double age : {2.999999, 3.0}) {
            Fixture f;
            f.e.start();
            f.t = age;
            f.e.advance();
            check((f.e.state == PeerState::Reconnecting) == (age == 3));
        }
        for (double age : {2.999999, 3.0}) {
            Fixture f;
            f.e.begin(17, {7});
            f.t = age;
            f.e.advance();
            check((!f.e.pending) == (age == 3));
        }
        {
            Fixture f;
            f.e.begin(17, {7});
            f.t = .049999;
            f.e.advance();
            check(f.e.retry_count == 0);
            f.t = .05;
            f.e.advance();
            check(f.e.retry_count == 1);
        }
        // Query nonce/chip/schema/revision/length/mode must all match. ACK never proves mode.
        for (unsigned corrupt = 0; corrupt < 8; ++corrupt) {
            Fixture f;
            f.mode();
            auto p = f.proof();
            if (corrupt == 0)
                p[3 + 2] ^= 1;
            if (corrupt == 1)
                p[3 + 18] ^= 1;
            if (corrupt == 2)
                p[3 + 1] = 1;
            if (corrupt == 3)
                p[3 + 23] = 1;
            if (corrupt == 4)
                p[3 + 22] = 2;
            if (corrupt == 5) {
                p.erase(p.end() - 2);
                --p[2];
            }
            if (corrupt == 6)
                f.t = 8;
            if (corrupt == 7)
                p[3] = 0xfd;
            unsigned sum = 0;
            for (size_t i = 0; i + 1 < p.size(); ++i)
                sum += p[i];
            p.back() = static_cast<uint8_t>(sum);
            check(!f.e.ack_tag(static_cast<uint16_t>(f.e.generation)));
            f.e.receive(p, 3, 0, true);
            check(!f.e.mode_confirmed_at);
        }
        {
            Fixture f;
            f.mode();
            f.t = 1;
            f.e.receive(f.proof(), 3, 0, true);
            check(f.e.mode_verified() && f.e.mode_profile == std::optional<std::string>("frame"));
            check(f.e.mode_confirmed_at == 0);
            check(f.e.pending->command == 16 && f.e.state == PeerState::Configuring);
        }
        {
            Fixture f;
            f.mode();
            f.e.receive(f.proof(1, 2, 1), 3, 0, true);
            check(f.e.pending->command == 1 && f.e.sequences[1] == 1 &&
                  f.e.mode_nonce == Bytes(16, 2));
            check(f.e.mode_switch_started == 0);
        }
        {
            Fixture f;
            f.mode();
            f.e.receive(f.proof(1, 1, 0), 3, 0, true);
            check(f.e.mode_preparing && f.e.pending->command == 16);
            f.e.ack_tag(static_cast<uint16_t>(f.e.generation));
            check(!f.e.mode_preparing && f.e.pending->command == 1);
        }
        {
            Fixture f;
            f.mode();
            f.e.required_query_profile = "frame";
            f.e.receive(f.proof(0, 1), 3, 0, true);  // non-Frame proof is logged, not fatal
            check(true);
        }
        for (uint8_t revision : {uint8_t(7),uint8_t(11),uint8_t(12)}) {
            Fixture f;
            f.revision=revision;
            f.mode();
            f.e.receive(f.proof(0, 1), 3, 0, true);
            check(f.e.firmware_revision==revision);
            check(f.e.mode_set_target == std::optional<std::string>("frame"));
            check(f.e.pending->body.size() == 24 && f.e.pending->body[0] == 0x7d);
            Bytes body{0xfd, 2};
            body.insert(body.end(), f.e.mode_nonce->begin(), f.e.mode_nonce->end());
            body.insert(body.end(), f.e.chip.begin(), f.e.chip.end());
            body.insert(body.end(), {0, 1, 0, revision});
            f.e.receive(packet(1, body), 3, 0, true);
            check(!f.e.mode_set_target && f.e.pending->body[0] == 0x7e);
            check(!f.e.mode_verified());
        }
        // Startup retries: first0, second8, third16, exhausted24, live replies only.
        {
            Fixture f;
            f.mode();
            check(!f.e.advance_initial_query());
            check(f.e.initial_recovery->deadline == 24);
            Bytes previous = *f.e.mode_nonce;
            for (int i = 1; i <= 2; ++i) {
                f.t = i * 8;
                f.e.last_reply = f.t - .001;
                check(f.e.advance_initial_query());
                check(f.e.pending->sequence == i);
                check(*f.e.mode_nonce != previous);
                previous = *f.e.mode_nonce;
                check(f.e.initial_recovery->attempts == unsigned(i + 1));
            }
            // Exhaustion is logged and a fresh 24s recovery window keeps querying.
            f.t = 24;
            f.e.last_reply = 24;
            check(f.e.advance_initial_query());
            check(f.e.state == PeerState::VerifyingMode && f.e.pending &&
                  f.e.pending->command == 1 && !f.e.mode_verified());
            check(*f.e.mode_nonce != previous && f.e.mode_error.empty());
            check(f.e.initial_recovery->attempts == 2 && f.e.initial_recovery->deadline == 48);
            check(f.events == std::vector<std::string>({"initial_query", "new_sequence_query",
                                                        "new_sequence_query", "exhausted",
                                                        "new_sequence_query"}));
        }
        for (double age : {-1., 2.999999, 3.0}) {
            Fixture f;
            f.mode();
            f.t = 8;
            f.e.last_reply = 8 - age;
            check(f.e.advance_initial_query() == (age == 2.999999));
            if (age != 2.999999) {
                f.e.advance();
                check(f.e.state == PeerState::Reconnecting);
            }
        }
        {
            Fixture f;
            f.mode();
            f.e.advance_initial_query();
            f.t = 24;
            f.e.last_reply = 24;
            check(f.e.advance_initial_query());  // deadline logged, window restarted
            check(f.e.initial_recovery->attempts == 2 && f.e.initial_recovery->deadline == 48);
            check(f.e.pending && f.e.pending->command == 1);
        }
        {
            Fixture f;
            f.mode();
            f.t = -.001;
            rejects([&] { f.e.advance_initial_query(); });
        }
        {
            Fixture f;
            f.mode();
            f.e.mode_query_timeout_s = 7;
            rejects([&] { f.e.advance_initial_query(); });
        }
        {
            Fixture f;
            f.mode();
            f.e.sequences[1] = 63;
            f.e.query_mode();
            f.t = 8;
            f.e.last_reply = 8;
            check(f.e.advance_initial_query());
            check(f.e.pending->sequence == 0);
        }
        {
            double t = 0;
            PeerEngine e(
                {0xd4, 0xc3, 0xb2, 0xa1}, 0, [&] { return t; }, [] { return Bytes(16, 1); });
            e.mode_required = true;
            e.desired_mode = "frame";
            e.query_mode();
            e.recovery_event = [](const std::string &, unsigned, uint8_t, const Bytes &) {};
            t = 8;
            e.last_reply = t;
            // A repeated nonce is logged; the retry is still sent.
            check(e.advance_initial_query());
            check(e.state == PeerState::VerifyingMode && e.pending && e.pending->sequence == 1);
            check(e.initial_recovery->attempts == 2);
        }
        // Finite leases; continuity is the explicit original wrapper policy.
        {
            Fixture f;
            f.fresh();
            f.t = 29.999999;
            check(f.e.mode_verified());
            f.t = 30;
            check(!f.e.mode_verified());
            f.e.last_reply = 30;
            f.e.advance();
            check(f.e.state == PeerState::Reconnecting);
        }
        {
            Fixture f;
            f.fresh();
            f.t = 10;
            f.e.last_reply = 10;
            f.e.advance();
            check(f.e.pending && f.e.pending->body[0] == 0x7e);
        }
        {
            Fixture f;
            f.fresh();
            f.e.continuity_lease = true;
            f.t = 35;
            f.e.last_reply = 35;
            f.e.advance();
            check(f.e.mode_verified() && !f.e.pending);
            f.t = 38;
            f.e.advance();
            check(f.e.state == PeerState::Reconnecting);
        }
        {
            Fixture f;
            f.mode();
            f.e.mode_switch_started = 0;
            f.t = 20;
            f.e.last_reply = 20;
            f.e.advance();
            check(f.e.state == PeerState::Reconnecting);
        }
        {
            Fixture f;
            f.fresh();
            f.e.sample_feedback_enabled = true;
            f.e.native25_exclusive = true;
            f.e.sample_feedback(255);
            check(!f.e.pending && f.e.sample_feedback_counter == 255);
            f.e.native25_exclusive = false;
            f.e.advance();
            check(f.e.pending->command == 11 && f.e.pending->body == Bytes{255});
            check(f.e.sample_feedback_publications == 1);
        }
        {
            Fixture f;
            f.e.sample_feedback(0);
            check(!f.e.sample_feedback_counter);
            rejects([&] { f.e.sample_feedback(-1); });
            rejects([&] { f.e.sample_feedback(256); });
        }
        {
            Fixture f;
            f.fresh();
            f.e.native25_exclusive = true;
            f.e.queued = {{10, {0}}, {17, {7}}, {14, {0}}};
            f.e.queue_exposure_sync(0, 3, true, true, 255);
            check(f.e.queued == std::deque<std::pair<uint8_t, Bytes>>(
                                    {{17, {7}}, {14, {1, 3, 255}}, {10, {0, 0, 0, 0}}}));
            const auto before = f.e.queued;
            f.e.queue_exposure_sync(533333, 1, true, true);  // invalid phase: logged, skipped
            check(f.e.queued == before);
            f.e.queue_exposure_sync(0, 0, true, true);  // invalid count: logged, skipped
            check(f.e.queued == before);
            f.e.queue_exposure_sync(5, 1, false, true);  // unverified phase: logged, still queued
            check(f.e.queued == std::deque<std::pair<uint8_t, Bytes>>(
                                    {{17, {7}}, {14, {1, 1, 0}}, {10, {5, 0, 0, 0}}}));
        }
        // Actual odd/data/even commands, zero padding, LE timing and cache-on-success.
        {
            FakeOpenOcd f;
            DebugMemory d([&](const std::string &c) { return f.command(c); });
            MailboxWriter w(d);
            check(w.publish(0, 2, 2, {0, 17, 1, 7, 25}));
            check(f.transcript.size() == 1);
            check(f.transcript[0] == "write_memory 0x20011298 32 {0x3}; write_memory 0x2001129c 32 "
                                     "{0x205 0x7011100 0x19 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 "
                                     "0x0 0x0 0x0}; write_memory 0x20011298 32 {0x2}");
            check(!w.publish(0, 2, 2, {0, 17, 1, 7, 25}) && f.operations == 1);
            f.fail_at = 2;
            f.fail_after = true;
            rejects([&] { w.publish(0, 4, 3, {0}); });
            f.fail_at = 0;
            check(w.publish(0, 4, 3, {0}));
            check(w.publish(2, 8, 2, {0, 10, 4, 0, 0, 0, 0, 14}, PhaseReference{0x12345678, 533332},
                            CycleWindow{0xfffffff0, 16}));
            check(word(f.read(roy_data + 1664 + 128 + 4, 4)) == 0x30208);
            check(word(f.read(roy_data + 1664 + 128 + 8 + 48, 4)) == 0x12345678);
            // multi4 2026-10-31: mailbox 3 exists (DATA+1944), mailbox 4 does not.
            check(w.publish(3, 6, 2, {0}) && word(f.read(roy_data + 1944, 4)) == 6 &&
                  word(f.read(roy_data + 1944 + 4, 4)) == 0x201);
            rejects([&] { w.publish(4, 0, 2, {0}); });
            rejects([&] { w.publish(-1, 0, 2, {0}); });
            rejects([&] { w.publish(0, 1, 2, {0}); });
            rejects([&] { w.publish(0, 0, 1, {0}); });
            rejects([&] { w.publish(0, 0, 2, {}); });
            rejects([&] { w.publish(0, 0, 2, Bytes(57)); });
            rejects([&] { w.publish(0, 0, 2, {0}, PhaseReference{0, 0}); });
            rejects([&] { w.publish(0, 0, 2, {0}, {}, CycleWindow{0, 0}); });
            rejects([&] { w.publish(0, 0, 2, {0}, {}, CycleWindow{0, 0x80000000}); });
        }
        // Later19 is gated independently of omit_command19; no optical substitute.
        {
            Fixture f;
            f.fresh();
            f.e.continuity_lease = true;
            LaterImuStart gate;
            BenchImuEvidence b;
            b.forbid_controllers = true;
            b.min_controllers = 0;
            b.guard_mode = "strict";
            b.temporal_identity_verified = true;
            b.pose_valid = true;
            b.phase = "locked";
            b.bootstrap_producer_verified = true;
            b.samples = {{1, 7, true, true}, {2, 7, true, true}};
            b.upstream = {{"irq_gate_00441", 1}, {"irq_gate_00fa7", 1}, {"irq_gate_01fb0", 1}};
            unsigned guarded = 0, persisted = 0;
            auto guard = [&] { ++guarded; };
            auto persist = [&](double age) {
                check(age == 3);
                ++persisted;
            };
            check(!gate.start(f.e, b, guard, persist));
            for (int i = 1; i <= 12; ++i) {
                f.t = i * .25;
                f.e.last_reply = f.t;
                bool started = gate.start(f.e, b, guard, persist);
                check(started == (i == 12));
            }
            check(f.e.pending->command == 19 && f.e.sample_feedback_enabled && persisted == 1 &&
                  guarded == 13);
        }
        for (unsigned reason = 0; reason < 10; ++reason) {
            Fixture f;
            f.fresh();
            f.e.continuity_lease = true;
            LaterImuStart gate;
            BenchImuEvidence b;
            b.forbid_controllers = true;
            b.min_controllers = 0;
            b.guard_mode = "strict";
            b.temporal_identity_verified = true;
            b.pose_valid = true;
            b.phase = "locked";
            b.bootstrap_producer_verified = true;
            b.samples.resize(2);
            b.upstream = {{"irq_gate_00441", 1}, {"irq_gate_00fa7", 1}, {"irq_gate_01fb0", 1}};
            gate.bench_admission(f.e, b, [] {});
            f.t = 3;
            f.e.last_reply = 3;
            if (reason == 0)
                b.forbid_controllers = false;
            if (reason == 1)
                b.min_controllers = 1;
            if (reason == 2)
                b.guard_mode = "observe";
            if (reason == 3)
                f.e.mode_profile = "stock";
            if (reason == 4)
                b.bootstrap_producer_verified = false;
            if (reason == 5)
                b.samples.resize(1);
            if (reason == 6)
                b.upstream["extra"] = 1;
            if (reason == 7)
                b.upstream["irq_gate_00fa7"] = 0;
            if (reason == 8)
                b.producer_verified_at = -.000001;
            if (reason == 9)
                f.e.begin(17, {7});
            // Evidence gaps are logged; only a non-Frame mode (3) or a busy pipeline (9) waits.
            const bool started = gate.start(f.e, b, [] {}, [](double) {});
            check(started == (reason != 3 && reason != 9));
            check(f.e.sample_feedback_enabled == started);
            check(!started || (f.e.pending && f.e.pending->command == 19));
        }
        {
            Fixture f;
            f.e.blind_ack = true;
            f.e.begin(17, {7});
            const auto old = static_cast<uint16_t>(f.e.generation);
            f.e.pending.reset();
            for (unsigned i = 0; i < 32; ++i) {
                f.e.next_generation();
                f.e.mailbox();
            }
            f.e.ack_notification = true;
            f.e.ack_tag(old);
            check(f.e.ack_notification);
            f.e.ack_tag(static_cast<uint16_t>(f.e.generation));
            check(!f.e.ack_notification);
        }
        {
            Fixture f;
            f.e.mode_required = true;
            f.e.mode_query_schema = 1;
            f.e.mode_configurations = {{"stock", {{16, {1, 2}}}}};
            f.e.query_mode();
            Bytes b{0xfe, 1};
            b.insert(b.end(), f.e.mode_nonce->begin(), f.e.mode_nonce->end());
            b.insert(b.end(), f.e.chip.begin(), f.e.chip.end());
            b.insert(b.end(), {0, 1});
            f.e.receive(packet(1, b), 3, 0, true);
            check(f.e.mode_profile == std::optional<std::string>("stock") &&
                  f.e.pending->command == 16);
        }
        {
            Fixture f;
            f.fresh();
            f.e.phase_verified = true;
            f.e.mode_configurations = {{"frame", {{16, {2, 2}}}}};
            f.e.mode_query_schema = 2;
            f.e.query_mode();
            f.t = 1;
            f.e.receive(f.proof(), 3, 0, true);
            check(f.e.state == PeerState::Streaming && f.e.phase_verified && !f.e.pending);
        }
        {
            Fixture f;
            f.fresh();
            f.e.query_mode();
            f.t = 8;
            f.e.last_reply = 8;
            f.e.advance();
            check(!f.e.pending && f.e.mode_verified() && f.e.mode_query_failures == 1 &&
                  f.e.mode_retry_after == 10);
            f.t = 9.99999;
            f.e.last_reply = f.t;
            f.e.advance();
            check(!f.e.pending);
            f.t = 10;
            f.e.last_reply = f.t;
            f.e.advance();
            check(f.e.pending->command == 1);
        }
        {
            Fixture f;
            f.mode();
            f.e.advance_initial_query();
            f.e.receive(f.proof(), 3, 0, true);
            check(!f.e.advance_initial_query() && f.e.initial_recovery->completed);
        }
        {
            Fixture f;
            f.fresh();
            f.e.native25_exclusive = true;
            f.e.begin(17, {7});
            f.t = 3;
            f.e.advance();
            check(f.e.state == PeerState::Reconnecting && !f.e.sample_feedback_enabled);
        }
        for (unsigned reason = 0; reason < 7; ++reason) {
            BenchImuEvidence b;
            b.samples = {{1, 7, true, true}, {2, 7, true, true}};
            b.upstream = {{"gate", 1}};
            if (reason == 0)
                b.samples.resize(1);
            if (reason == 1)
                b.samples[1].counter = 1;
            if (reason == 2)
                b.samples[1].mode = 0;
            if (reason == 3)
                b.samples[0].enabled = false;
            if (reason == 4)
                b.samples[1].post_gate = false;
            if (reason == 5)
                b.upstream.clear();
            if (reason == 6)
                b.upstream["gate"] = 0;
            verify_bootstrap_producer(b, 10);  // gap is logged, evidence time recorded
            check(b.bootstrap_producer_verified && b.producer_verified_at == 10);
        }
        {
            BenchImuEvidence b;
            b.samples = {{1, 7, true, true}, {2, 7, true, true}};
            b.upstream = {{"gate", 1}};
            verify_bootstrap_producer(b, 10);
            check(b.bootstrap_producer_verified && b.producer_verified_at == 10);
        }
        {
            FakeOpenOcd f;
            DebugMemory d([&](const std::string &c) { return f.command(c); });
            MailboxWriter w(d);
            check(w.publish(0, 2, 3, Bytes(56, 0xff)));
            check(w.publish(0, 4, 2, Bytes(40), {}, CycleWindow{0, 0x7fffffff}));
            rejects([&] { w.publish(0, 6, 2, Bytes(41), {}, CycleWindow{0, 1}); });
            rejects(
                [&] { w.publish(0, 6, 2, {0, 10, 4, 0, 0, 0, 0, 14}, PhaseReference{0, 533333}); });
        }
        for(unsigned build:{0u,0x13u})for(unsigned count:{1u,2u,3u})for(unsigned id=1;id<=count;++id) {
            Fixture f;f.e.chip=le32(id);f.e.firmware_revision=12;
            const auto status_dir=std::filesystem::temp_directory_path()/
                ("pico-peer-status-tests-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
            std::filesystem::create_directories(status_dir);
            f.e.sensor_status_directory=status_dir.string();
            f.e.state=PeerState::Streaming;f.e.mode_profile="frame";f.e.continuity_lease=true;
            for(unsigned page=0;page<(build==0x13?10u:8u);++page) {
                f.e.last_reply=f.t;f.e.advance();
                check(f.e.sensor_nonce.has_value() && f.e.pending->body.size()==19);
                check(f.e.pending->body[18]==page);
                Bytes reply{0xfe,3};reply.insert(reply.end(),f.e.sensor_nonce->begin(),f.e.sensor_nonce->end());
                reply.insert(reply.end(),f.e.chip.begin(),f.e.chip.end());reply.insert(reply.end(),{1,12,uint8_t(page),6,0,0,0,0,0,0});
                if(page==3)reply[30]=uint8_t(build);
                if(page==8){reply[26]=1;reply[27]=1;reply[28]=176;}
                if(page==9)reply[26]=uint8_t(id);
                auto bad=reply;bad[2]^=1;f.e.receive(packet(1,bad),2,uint16_t(f.e.generation),true);
                check(f.e.sensor_nonce.has_value());
                f.e.receive(packet(1,reply),2,uint16_t(f.e.generation),false);check(f.e.sensor_nonce.has_value());
                f.e.receive(packet(1,reply),2,uint16_t(f.e.generation),true);
                check(!f.e.sensor_nonce && !f.e.pending);f.t+=1.1;
                char name[64];std::snprintf(name,sizeof(name),"pico-tracker-status-%08X.txt",id);
                std::ifstream file(status_dir/name);
                const std::string saved((std::istreambuf_iterator<char>(file)),{});
                file.close();
                check(saved.find("page_mask="+std::to_string((1u<<(page+1))-1)+"\n")!=std::string::npos);
                if(page==9) {check(saved.find("build_id=19\n")!=std::string::npos);check(saved.find("mag_seq="+std::to_string(id)+"\n")!=std::string::npos);}
                std::filesystem::remove(status_dir/name); // Windows rename also needs a new destination
            }
            check(f.e.sensor_status.mask==(build==0x13?1023u:255u));
            f.e.last_reply=f.t;f.e.advance();check(f.e.sensor_nonce.has_value());
            f.t+=1.1;f.e.last_reply=f.t;f.e.advance();
            check(!f.e.sensor_nonce && !f.e.pending && f.e.state==PeerState::Streaming);
            check(f.e.sensor_page==1); // timeout on page zero must not starve pages 1..7
            std::filesystem::remove(status_dir);
        }
        std::cout << checks
                  << " peer/mailbox/mode checks PASS (source-derived vectors, fake TCL)\n";
        return 0;
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}
