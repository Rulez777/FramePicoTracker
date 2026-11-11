#include "native_test_runtime.hpp"
#include "data_plane.hpp"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <string>
#ifndef _WIN32
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#endif
using namespace pico;
static size_t checks = 0;
static void check(bool b, const char *label) {
    ++checks;
    if (!b)
        throw std::runtime_error(label);
}
template <class F> void rejects(F f, const char *label) {
    bool threw = false;
    try {
        f();
    } catch (const std::exception &) {
        threw = true;
    }
    check(threw, label);
}
static Bytes checksum(Bytes b) {
    unsigned s = 0;
    for (auto v : b)
        s += v;
    b.push_back(uint8_t(s));
    return b;
}
static Sample sample(uint8_t c) {
    Sample s;
    s.counter = c;
    return s;
}
static Bytes body(uint8_t c) {
    Bytes b(18);
    b[17] = c;
    return b;
}
static Record record(uint8_t c) {
    Record r;
    r.chip = 0xA1B2C3D4;
    r.channel = 2;
    r.profile = 0;
    r.hardware_crc = true;
    r.dma = {22, 2, 255, 2, 18};
    auto b = body(c);
    r.dma.insert(r.dma.end(), b.begin(), b.end());
    unsigned sum = 0;
    for (size_t i = 2; i < r.dma.size(); ++i)
        sum += r.dma[i];
    r.dma.push_back(uint8_t(sum));
    return r;
}
static void codecs() {
    // Literal wire vector: unaligned u64 at12; negative words; all padding zero.
    Sample s;
    s.counter = 255;
    s.gyro = {-32768, 32767, -1};
    s.accel = {1, -2, 0};
    auto p = pti1(0xA1B2C3D4, s, 0x0807060504030201ULL);
    check(hex(p.data(), p.size()) ==
              "50544931d4c3b2a1ff00000001020304050607080080ff7fffff0100feff0000",
          "PTI1 golden");
    check(decode_records({0}).empty(), "checksum-only empty");
    check(decode_records(checksum({255, 29, 0}))[0].sequence() == 63, "high tag valid");
    check(decode_records(checksum({255, 0, 0, 0, 0})).empty(), "zero terminator padding");
    for (auto pbad : std::vector<Bytes>{{},
                                        {1},
                                        {1, 2, 3},
                                        checksum({0, 30, 0}),
                                        checksum({0, 2, 1}),
                                        checksum({0, 0, 1}),
                                        checksum({0, 0, 0, 1}),
                                        checksum({0}),
                                        checksum({0, 1})})
        rejects([&] { decode_records(pbad); }, "bad swift");
    for (size_t n = 0; n < 45; ++n) {
        Bytes b(n);
        if (n == 6 || n == 18 || n == 24 || n == 36 || n == 42)
            check(decode_imu(b).size() == n / 18, "imu lengths");
        else
            rejects([&] { decode_imu(b); }, "bad imu length");
    }
    Bytes b = unhex("0080ff7fffff0100feff00000080542308ff"); // ticks533332, temperature -32768
    auto v = decode_imu(b);
    check(v[0].gyro == s.gyro && v[0].temperature == -32768 && v[0].ticks == 533332,
          "signed/ticks literal");
    b[14] = 0x55;
    rejects([&] { decode_imu(b); }, "533333 invalid");
    b[14] = b[15] = b[16] = 255;
    rejects([&] { decode_imu(b); }, "ffffff invalid");
    Bytes two(36);
    two[32] = 0x55;
    two[33] = 0x23;
    two[34] = 8;
    rejects([&] { decode_imu(two); }, "second invalid atomic");
    for (size_t n = 0; n < 112; ++n)
        rejects([&] { parse_record(Bytes(n), true); }, "short ring record");
    Bytes raw(112);
    raw[4] = 0x34;
    raw[5] = 0x12;
    raw[6] = 0xab;
    raw[7] = 0xcd;
    raw[8] = 70;
    raw[9] = 2;
    raw[12] = 2;
    raw[16] = 3;
    raw[18] = 0xfe;
    raw[19] = 0xff;
    raw[100] = 0xd4;
    raw[101] = 0xc3;
    raw[102] = 0xb2;
    raw[103] = 0xa1;
    raw[104] = 9;
    raw[108] = 0x34;
    raw[109] = 0x12;
    auto r = parse_record(raw, true);
    check(r.chip == 0xA1B2C3D4 && r.crc == 0xcdab1234 && r.profile == 2 && r.slot == 32767 &&
              r.callback_us == 0x1234 && r.cycles == 9 && r.hardware_crc,
          "record offsets");
    raw[18] |= 1;
    check(!parse_record(raw, true).hardware_crc, "partial CRC");
    raw[16] = 81;
    rejects([&] { parse_record(raw, true); }, "impossible length");
    check(record_dma_words(0) == 0 && record_dma_words(1) == 1 && record_dma_words(4) == 1 &&
              record_dma_words(5) == 2 && record_dma_words(0xffff0050) == 20 &&
              record_dma_words(81) == 20 && record_dma_words(0xffff) == 20,
          "dma words");
    // The radio batch reads only words 0..4, record_dma_words() and 25..27 of a
    // slot; parse_record must not depend on any other byte.
    for (uint32_t n = 0; n <= 80; ++n) {
        Bytes full(112), part(112, 0xa5);
        for (size_t k = 0; k < full.size(); ++k)
            full[k] = uint8_t(k * 13 + 1);
        full[16] = uint8_t(n);
        full[17] = 0;
        full[18] &= 0xfe;
        const size_t prefix = 20 + 4 * record_dma_words(n);
        std::copy(full.begin(), full.begin() + prefix, part.begin());
        std::copy(full.begin() + 100, full.end(), part.begin() + 100);
        const auto a = parse_record(full, true), b = parse_record(part, true);
        check(a.sequence == b.sequence && a.crc == b.crc && a.rssi == b.rssi &&
                  a.channel == b.channel && a.chip == b.chip && a.cycles == b.cycles &&
                  a.partial_bits == b.partial_bits && a.callback_us == b.callback_us &&
                  a.hardware_crc == b.hardware_crc && a.profile == b.profile && a.slot == b.slot &&
                  a.dma == b.dma && a.dma.size() == n,
              "record prefix suffices");
    }
}
static void rings() {
    size_t reads = 0, acks = 0;
    uint32_t captured = 0;
    Bytes memory(1120);
    for (size_t i = 0; i < 10; ++i)
        memory[i * 112] = uint8_t(i);
    Read read = [&](size_t o, size_t n) {
        ++reads;
        return Bytes(memory.begin() + o, memory.begin() + o + n);
    };
    auto ack = [&](uint32_t w) {
        ++acks;
        captured = w;
    };
    check(!snapshot(0, 0, read, ack) && !reads && !acks, "empty no IO");
    auto b = snapshot(11, 9, read, ack);
    check(reads == 2 && acks == 1 && captured == 11 && (*b)[1008] == 9 && (*b)[0] == 0,
          "physical wrap captured ack");
    memory[0] = 99;
    check((*b)[0] == 0, "immutable snapshot");
    check(snapshot(10, 0, read, ack)->size() == 1120, "full ring");
    check(snapshot(1, 0, read, ack)->size() == 1120, "single ring");
    // Owner rule 2026-10-29: ring loss resynchronises instead of ending the run.
    check(!snapshot(1, 2, read, ack), "sequence rollover resyncs");
    check(snapshot(11, 0, read, ack).has_value(), "overflow keeps newest ten");
    auto previous = acks;
    rejects([&] { snapshot(2, 0, [](size_t, size_t) { return Bytes{}; }, ack); }, "read failure");
    check(acks == previous, "read failure no ack");
    uint32_t producer = 2;
    auto concurrent = snapshot(
        producer, 0,
        [&](size_t o, size_t n) {
            producer = 3;
            return read(o, n);
        },
        ack);
    check(captured == 2 && producer == 3 && concurrent.has_value(),
          "ack captured producer not new producer");
    Counters counters;
    check(snapshot(11, 0, read, ack, &counters).has_value(), "loss counted");
    check(counters.ring_loss == 1, "ring loss event count");
}
static void reorders() {
    Reorder r;
    check(r.push(sample(10), 0, 100).empty(), "initial pending");
    r.push(sample(15), 1, 101);
    r.push(sample(11), 2, 102);
    r.push(sample(12), 3, 103);
    r.push(sample(10), 4, 999);
    check(r.stats.identical_repeats == 1, "duplicate");
    check(r.release(49999999).empty(), "before50ms");
    auto out = r.release(50000000);
    check(out.size() == 1 && out[0].key == 10 && out[0].cycles == 100 && out[0].stamps.insert == 0,
          "50ms retains first evidence");
    out = r.release(50000001);
    check(out.size() == 3 && out[0].key == 11 && out[2].key == 15,
          "higher due releases young lower");
    r.push(sample(10), 60000000, 0);
    check(r.stats.late_after_release == 1, "late before identical");
    for (int field = 0; field < 4; ++field) {
        Reorder c;
        auto a = sample(0), b = a;
        if (field == 0)
            b.gyro[0] = 1;
        if (field == 1)
            b.accel[0] = 1;
        if (field == 2)
            b.temperature = 1;
        if (field == 3)
            b.ticks = 1;
        c.push(a, 0, 0);
        c.push(b, 1, 0);
        c.push(a, 2, 0);
        check(c.stats.conflicting_counters == 2 && c.release(100000000, true).empty(),
              "whole sample conflict");
    }
    Reorder w;
    w.push(sample(255), 0, 1);
    w.push(sample(0), 1, 2);
    out = w.release(50000001);
    check(out.size() == 2 && out[1].key == 256, "wrap255 to0");
    Reorder neg;
    neg.push(sample(0), 0, 0);
    neg.push(sample(255), 1, 0);
    out = neg.release(50000000);
    check(out.size() == 2 && out[0].key == -1, "negative key younger released");
    Reorder tie;
    tie.push(sample(0), 0, 0);
    tie.push(sample(128), 1, 0);
    out = tie.release(50000000);
    check(out.size() == 2 && out[0].key == -128, "128 backward tie ambiguity");
    Reorder back;
    back.push(sample(2), 100, 0);
    check(back.release(0).empty() && back.release(50000099).empty(), "backward fake time no flush");
    check(back.release(0, true).size() == 1, "explicit all_pending helper");
    Reorder wraps;
    for (int i = 0; i < 1100; ++i)
        wraps.push(sample(uint8_t(i)), int64_t(i) * 1000000, 0);
    out = wraps.release(2000000000);
    check(out.back().key == 1099 && wraps.stats.released == 1100, "many wraps");
    Reorder cleanup;
    auto changed = sample(1);
    changed.ticks = 1;
    cleanup.push(sample(1), 0, 0);
    cleanup.push(changed, 1, 0);
    cleanup.push(sample(2), 2, 0);
    cleanup.release(50000002);
    cleanup.push(sample(1), 50000003, 0);
    check(cleanup.stats.conflicting_counters == 1 && cleanup.stats.late_after_release == 1,
          "conflict cleanup late precedence");
    check(cleanup.release(100000000).empty(), "empty release");
}
static void clocks() {
    RoyClock c;
    rejects([&] { c.convert(0, 0); }, "no fit");
    for (int i = 0; i < 5; ++i)
        check(!c.observe(i * 3840000, int64_t(i) * 60000000, int64_t(i) * 60000000),
              "five points insufficient");
    check(c.observe(19200000, 300000000, 300000000), "six300ms");
    check(c.convert(19200000, 550000000) == 300000000, "250ms inclusive");
    check(c.convert(19200000, 550000001) == 300000000, "no stale bound");
    check(c.convert(19200000, 299999999) == 300000000, "no future bound");
    check(c.cycles_at(550000000) == 35200000, "projection inclusive");
    check(c.cycles_at(550000001) == 35200000, "no projection horizon");
    check(!c.observe(-1, 0, 0) && !c.observe(0, 0, 10000001) && !c.observe(0, 1, 0),
          "invalid probe");
    check(c.rejected == 3, "rejection count");
    check(c.observe(23040000, 360000000, 360000000), "rejected did not advance raw");
    rejects([&] { c.observe(int64_t(23040000) + 0x80000000LL, 0, 0); }, "half wrap error");
    RoyClock w;
    w.observe(0xffffffff, 0, 0);
    w.observe(0, 1, 1);
    check(w.points() == 2, "clock unsigned wrap");
    check(nearest_even(.5) == 0 && nearest_even(1.5) == 2 && nearest_even(2.5) == 2 &&
              nearest_even(-.5) == 0 && nearest_even(-1.5) == -2,
          "ties even");
    c.fit = Fit{0, 100000000.5, 15.625, 0};
    check(c.convert(0, 100000001) == 100000000, "serialized half ns even");
    c.fit = Fit{0, 0, 15.625, 0};
    check(c.cycles_at(-15) == 0xffffffffu, "projection wrap");
    for (double width : {1000000., 1000001.}) {
        RoyClock u;
        for (int i = 0; i < 6; ++i)
            u.observe(i * 6400000, int64_t(i) * 100000000 - int64_t(width),
                      int64_t(i) * 100000000 + int64_t(width));
        check(bool(u.fit), "no uncertainty bound");
    }
    RoyClock span;
    for (int i = 0; i < 5; ++i)
        span.observe(i * 3840000, int64_t(i) * 60000000, int64_t(i) * 60000000);
    check(!span.observe(19200000, 299999999, 299999999), "span just below");
    // 150 second transcript crosses two 32-bit wraps and evicts at401.
    RoyClock longc;
    for (int i = 0; i <= 1500; ++i) {
        int64_t ns = int64_t(i) * 100000000;
        longc.observe(uint32_t(int64_t(i) * 6400000), ns, ns, ns, ns);
    }
    check(longc.points() == 400 && longc.fit && std::abs(longc.fit->slope - 15.625) < 1e-10,
          "150s clock eviction wraps");
    for (int ppm : {-501, -499, 0, 499, 501}) {
        RoyClock n;
        for (int i = 0; i < 110; ++i) {
            int64_t raw = int64_t(i) * 100000000, mono = raw + int64_t(i) * 100 * ppm;
            n.observe(i * 6400000, mono, mono, raw, raw);
        }
        check(bool(n.fit), "host slew logged, fit kept");
    }
    for (int ppm : {-101, -99, 99, 101}) {
        RoyClock n;
        for (int i = 0; i < 110; ++i) {
            int64_t t = int64_t(i) * 100000000 + int64_t(i) * 100 * ppm;
            n.observe(i * 6400000, t, t, t, t);
        }
        check(bool(n.fit), "crystal drift logged, fit kept");
    }
    RoyClock fallback;
    for (int i = 0; i < 110; ++i) {
        int64_t t = int64_t(i) * 100020000;
        fallback.observe(i * 6400000, t, t);
    }
    check(fallback.fit && fallback.fit->slope == 15.625, "200ppm without RAW keeps nominal slope");
    RoyClock missing;
    for (int i = 0; i < 110; ++i) {
        int64_t t = int64_t(i) * 100000000;
        missing.observe(i * 6400000, t, t, i == 3 ? std::nullopt : std::optional<int64_t>(t), t);
    }
    check(missing.fit.has_value(), "partial RAW falls back");
    RoyClock ten;
    check(!ten.observe(0, 0, 10000000) && ten.points() == 1, "10ms read accepted point");
    RoyClock ninety;
    for (int i = 0; i < 99; ++i) {
        int64_t t = int64_t(i) * 100020000;
        ninety.observe(i * 6400000, t, t);
    }
    check(ninety.fit && ninety.fit->slope == 15.625, "99 remains fixed slope despite5s");
    ninety.observe(99 * 6400000, 9901980000LL, 9901980000LL);
    check(ninety.fit && ninety.fit->slope == 15.625, "invalid learned rate falls back to nominal");
    RoyClock insufficient;
    insufficient.fit = Fit{0, 7, 15.625, 0};
    check(!insufficient.observe(0, 0, 0) && insufficient.fit && insufficient.fit->stamp == 7,
          "insufficient retains prior fit");
    RoyClock noisy;
    for (int i = 0; i < 120; ++i) {
        int64_t t = int64_t(i) * 100000000;
        int64_t half = i % 2 ? 100000 : 10000;
        int64_t jitter = i % 2 ? 2000 : -2000;
        noisy.observe(i * 6400000, t + jitter - half, t + jitter + half, t - half, t + half);
    }
    check(noisy.fit && noisy.uncertainty < 11000 && std::abs(noisy.fit->slope - 15.625) < .001,
          "weighted noisy fit");
    // IEEE double boundaries deliberately use literal integer times. No ppm
    // epsilon is permitted. These exact vectors exercise the comparison itself.
    for (int ppm : {-100, 100}) {
        RoyClock edge;
        for (int i = 0; i < 100; ++i) {
            int64_t t = int64_t(i) * 100000000 + int64_t(i) * 100 * ppm;
            edge.observe(i * 6400000, t, t, t, t);
        }
        check(bool(edge.fit), "crystal boundary keeps fit");
    }
    for (int ppm : {-500, 500}) {
        RoyClock edge;
        for (int i = 0; i < 100; ++i) {
            int64_t r = int64_t(i) * 100000000, t = r + int64_t(i) * 100 * ppm;
            edge.observe(i * 6400000, t, t, r, r);
        }
        check(edge.fit.has_value(), "host exact500ppm boundary");
    }
}
static void pipeline() {
    Record attributed = record(0);
    RoyClock authority;
    authority.fit = Fit{0, 0, 15.625, 0.25};
    check(!ack_context(attributed, authority, 0), "zero generation no special ACK context");
    attributed.generation = 65535;
    auto context = ack_context(attributed, authority, 0);
    check(context && context->generation == 65535 && context->source_ns == 0 &&
              context->uncertainty_ns == 1,
          "ACK tag source ceil uncertainty");
    check(ack_context(attributed, authority, 250000001).has_value(), "late ACK context kept");
    attributed.generation = 65536;
    check(!ack_context(attributed, authority, 0), "out of range generation");
    attributed.generation = 1;
    authority.fit->uncertainty = 1000001;
    check(ack_context(attributed, authority, 0).has_value(), "no ACK uncertainty limit");
    authority.fit->uncertainty = 1000000;
    check(ack_context(attributed, authority, 0).has_value(), "ACK1ms inclusive");
    Histogram age;
    age.observe(0, 1000000);
    age.observe(0, 1000001);
    age.observe(1, 0);
    age.observe(0, 250000001);
    check(age.bins[0] == 1 && age.bins[1] == 1 && age.bins[5] == 1 && age.negative == 1,
          "age histogram boundaries and future");
    int64_t now = 0;
    std::string order;
    Hooks h;
    h.now = [&] { return now; };
    h.canonical = [&](const Record &, const std::vector<Command> &,
                      const std::optional<AckContext> &) { order += 'C'; };
    h.feedback = [&](uint8_t) { order += 'F'; };
    h.mailbox = [&] { order += 'M'; };
    h.send = [&](const std::array<uint8_t, 32> &) {
        order += 'S';
        return true;
    };
    DataPlane d(h);
    d.reorder.hold_ns = 50000000;  // tests keep the timed-hold semantics; live uses 0
    d.enrolled = true;
    d.clock.fit = Fit{0, 0, 15.625, 0};
    auto r = record(255);
    r.sequence = 10;
    d.record(r);
    check(order == "CFM", "canonical feedback mailbox order");
    now = 49999999;
    r = record(0);
    r.sequence = 11;
    d.record(r);
    check(d.counters.samples == 0, "pending not counted");
    now = 50000000;
    d.record(r);
    auto e = d.next_event();
    check(e && e->sequence == 10 && e->item.sample.counter == 255 && e->wire,
          "original sequence packet evidence");
    check(d.counters.samples == 1 && d.counters.decoded == 3 && d.counters.sample_gaps == 0,
          "decoded released distinction");
    now = 100000000;
    d.record(record(1));
    e = d.next_event();
    check(e && e->item.sample.counter == 0, "counter release order");
    d.reset_peer();
    check(d.reorder.pending() == 1 && !d.enrolled, "reset preserves reorder");
    d.enrolled = true;
    now = 150000000;
    d.record(record(2));
    check(d.counters.feedback == 4, "reset last feedback counter");
    DataPlane bad(h);
    bad.reorder.hold_ns = 50000000;  // tests keep the timed-hold semantics; live uses 0
    bad.enrolled = true;
    auto b = record(0);
    b.hardware_crc = false;
    bad.record(b);
    b = record(0);
    b.chip = 0;
    bad.record(b);
    b = record(0);
    b.dma.push_back(0);
    bad.record(b);
    b = record(0);
    b.dma.back()++;
    bad.record(b);
    check(bad.counters.crc == 1 && bad.counters.source == 1 && bad.counters.malformed == 1 &&
              bad.counters.checksum == 1,
          "reject counter classes");
    DataPlane nc(h, 0);
    nc.reorder.hold_ns = 50000000;  // tests keep the timed-hold semantics; live uses 0
    nc.enrolled = true;
    now = 0;
    nc.record(record(0));
    now = 50000000;
    nc.record(record(1));
    check(nc.counters.no_clock == 1 && nc.counters.samples == 1 &&
              nc.counters.telemetry_dropped == 1,
          "bounded telemetry no-clock release");
    DataPlane stale(h);
    stale.reorder.hold_ns = 50000000;  // tests keep the timed-hold semantics; live uses 0
    stale.enrolled = true;
    stale.clock.fit = Fit{0, 0, 15.625, 0};
    now = 0;
    stale.record(record(0));
    now = 250000001;
    stale.record(record(1));
    check(stale.counters.sent > 0, "late IMU sample still sent with its true stamp");
    DataPlane unregistered(h);
    unregistered.reorder.hold_ns = 50000000;  // tests keep the timed-hold semantics; live uses 0
    unregistered.record(record(0));
    check(unregistered.counters.unregistered == 1, "unenrolled");
    DataPlane exclusive(h);
    exclusive.reorder.hold_ns = 50000000;  // tests keep the timed-hold semantics; live uses 0
    exclusive.enrolled = true;
    exclusive.exclusive = true;
    exclusive.record(record(0));
    check(!exclusive.counters.decoded, "exclusive blocks data");
    Hooks f = h;
    f.send = [](const std::array<uint8_t, 32> &) { return false; };
    DataPlane sf(f);
    sf.reorder.hold_ns = 50000000;  // tests keep the timed-hold semantics; live uses 0
    sf.enrolled = true;
    sf.clock.fit = Fit{0, 0, 15.625, 0};
    now = 0;
    sf.record(record(0));
    now = 50000000;
    sf.record(record(1));
    check(sf.counters.send_failures == 1 && sf.counters.samples == 1,
          "send failure release preserved");
    DataPlane feedback(h);
    feedback.reorder.hold_ns = 50000000;  // tests keep the timed-hold semantics; live uses 0
    feedback.enrolled = true;
    now = 0;
    for (uint8_t c :
         {uint8_t(0), uint8_t(0), uint8_t(127), uint8_t(255), uint8_t(126), uint8_t(128)}) {
        ++now;
        feedback.record(record(c));
    }
    check(feedback.counters.feedback == 3 && feedback.counters.sample_gaps == 126 &&
              feedback.last_imu_ns == 6,
          "delta0 127 128 255 accounting liveness");
    feedback.feedback_enabled = false;
    feedback.record(record(129));
    check(feedback.counters.feedback == 3, "disabled callback");
    auto wrong = record(0);
    wrong.dma[1] = 1;
    bad.record(wrong);
    wrong = record(0);
    wrong.dma[0]--;
    bad.record(wrong);
    check(bad.counters.malformed == 3, "wrong S1 and declared length");
}
static void loopback() {
#ifndef _WIN32
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    check(fd >= 0, "loopback socket");
    sockaddr_in a{};
    a.sin_family = AF_INET;
    a.sin_port = 0;
    a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    check(bind(fd, reinterpret_cast<sockaddr *>(&a), sizeof(a)) == 0, "loopback bind");
    timeval tv{1, 0};
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    auto wire = pti1(0xA1B2C3D4, sample(0), 0);
    socklen_t length = sizeof(a);
    check(getsockname(fd, reinterpret_cast<sockaddr *>(&a), &length) == 0, "loopback assigned port");
    UdpSender sender(ntohs(a.sin_port));
    check(sender.send(wire), "UDP send");
    std::array<uint8_t, 32> received{};
    check(recv(fd, received.data(), received.size(), 0) == 32 && received == wire,
          "UDP exact wire");
    close(fd);
#endif
}
int main() {
    try {
        codecs();
        rings();
        reorders();
        clocks();
        pipeline();
        loopback();
        std::cout << "PASS " << checks << " checks\n";
        return 0;
    } catch (const std::exception &e) {
        std::cerr << "FAIL after " << checks << ": " << e.what() << '\n';
        return 1;
    }
}

