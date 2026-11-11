#include "imu_slot.hpp"
#include <cstdio>
#include <cstdlib>
#include <algorithm>
static int checks = 0;
static void check(bool ok, const char *what) {
    ++checks;
    if (!ok) {
        std::fprintf(stderr, "fail %s\n", what);
        std::exit(1);
    }
}
int main() {
    uint8_t slot[48] = {};
    slot[0] = 9;
    slot[4] = 0x78;
    slot[5] = 0x56;
    slot[6] = 0x34;
    slot[7] = 0x12;
    slot[8] = 0x34;
    slot[9] = 0x12;
    slot[10] = 2;
    slot[11] = 36;
    for (int i = 0; i < 36; ++i)
        slot[12 + i] = uint8_t(i + 1);
    slot[12 + 14] = slot[12 + 15] = slot[12 + 16] = 0;
    slot[12 + 32] = slot[12 + 33] = slot[12 + 34] = 0;
    slot[12 + 17] = 11;
    slot[12 + 35] = 12;
    auto empty = pico::expand_imu_slot(slot + 0);
    (void)empty;
    uint8_t blank[48] = {};
    check(pico::expand_imu_slot(blank).dma.empty(), "blank");
    auto r = pico::expand_imu_slot(slot);
    check(r.hardware_crc && !r.partial && r.channel == 2 && r.chip == 0xA1B2C3D4, "identity");
    check(r.sequence == 9 && r.cycles == 0x12345678 && r.generation == 0x1234 && (r.crc >> 16) == 0x1234, "time+generation");
    check(r.dma.size() == 42 && r.dma[0] == 40 && r.dma[1] == 2 && r.dma[3] == 2 && r.dma[4] == 36, "dma");
    check(r.dma[5 + 17] == 11 && r.dma[5 + 35] == 12, "counters");
    unsigned sum = 0;
    for (size_t i = 2; i + 1 < r.dma.size(); ++i)
        sum += r.dma[i];
    check(r.dma.back() == uint8_t(sum), "checksum");
    auto cmds = pico::decode_records(pico::Bytes(r.dma.begin() + 2, r.dma.end()));
    check(cmds.size() == 1 && cmds[0].command == 2 && cmds[0].body.size() == 36, "command");
    auto samples = pico::decode_imu(cmds[0].body);
    check(samples.size() == 2 && samples[0].counter == 11 && samples[1].counter == 12, "samples");
    {   // ackring4 raw slot: the command-1 reply seen in native-live-212752 (first 16 DMA bytes from D1932..1944).
        uint8_t raw[48] = {};
        const uint8_t head[16] = {0x27, 0x03, 0x00, 0x01, 0x23, 0x82, 0xbd, 0x00,
                                  0x31, 0x32, 0x33, 0x33, 0x31, 0x50, 0x43, 0x32};
        uint8_t dma[41] = {};
        for (int i = 0; i < 16; ++i) dma[i] = head[i];
        for (int i = 16; i < 40; ++i) dma[i] = uint8_t(0x40 + i);
        unsigned s = 0;
        for (int i = 2; i < 40; ++i) s += dma[i];
        dma[40] = uint8_t(s);
        const uint32_t word0 = 0x80000000u | (38u << 24) | (1u << 23) | 77u;  // rest 38 = DMA bytes 2..39, s1 3
        for (int i = 0; i < 4; ++i) raw[i] = uint8_t(word0 >> (8 * i));
        raw[4] = 0x44; raw[5] = 0x33; raw[6] = 0x22; raw[7] = 0x11;  // cycles
        raw[8] = 0x06; raw[9] = 0x02;                                // generation 0x0206
        for (int i = 0; i < 38; ++i) raw[10 + i] = dma[2 + i];
        auto q = pico::expand_imu_slot(raw);
        check(q.sequence == 77 && q.hardware_crc && q.chip == 0xA1B2C3D4 && q.channel == 2, "raw identity");
        check(q.cycles == 0x11223344 && q.generation == 0x0206 && (q.crc >> 16) == 0x0206, "raw attribution");
        check(q.dma.size() == 41 && std::equal(q.dma.begin(), q.dma.end(), dma), "raw bytes");
        auto rc = pico::decode_records(pico::Bytes(q.dma.begin() + 2, q.dma.end()));
        check(rc.size() == 1 && rc[0].command == 1 && rc[0].body.size() == 35, "raw command 1");
        raw[3] = 0x80 | 39;  // rest 39 does not fit in the 38 payload bytes
        check(pico::expand_imu_slot(raw).dma.empty(), "raw too long");
        raw[3] = 0x80;       // rest 0
        { auto e = pico::expand_imu_slot(raw); check(e.dma.size() == 3 && e.dma[0] == 1 && e.dma[1] == 3 && e.dma[2] == 0 && pico::decode_records(pico::Bytes(e.dma.begin() + 2, e.dma.end())).empty(), "raw empty reply kept as an ACK (ackring8)"); }
    }
    {   // ackring7 tail: a one-sample slot with a 4-byte command-23 record after command 2.
        uint8_t one[48] = {};
        one[0] = 5;
        one[8] = 0x07;
        one[10] = 2;
        one[11] = 18;
        for (int i = 0; i < 18; ++i)
            one[12 + i] = uint8_t(0x10 + i);
        one[12 + 14] = one[12 + 15] = one[12 + 16] = 0;
        one[30] = 4;
        const uint8_t rec23[4] = {0x05, 23, 1, 0x2a};
        for (int i = 0; i < 4; ++i)
            one[31 + i] = rec23[i];
        auto t = pico::expand_imu_slot(one, true);
        check(t.dma.size() == 28 && t.dma[0] == 26 && t.dma[1] == 2 && t.dma[4] == 18, "tail dma");
        unsigned st = 0;
        for (size_t i = 2; i + 1 < t.dma.size(); ++i)
            st += t.dma[i];
        check(t.dma.back() == uint8_t(st), "tail checksum");
        auto tc = pico::decode_records(pico::Bytes(t.dma.begin() + 2, t.dma.end()));
        check(tc.size() == 2 && tc[0].command == 2 && tc[0].body.size() == 18 && tc[1].command == 23 &&
                  tc[1].body.size() == 1 && tc[1].body[0] == 0x2a, "tail commands");
        // ackring5 flag: slot[30] is a stale sample byte and must be ignored.
        auto s5 = pico::expand_imu_slot(one);
        check(s5.dma.size() == 24 && s5.dma[0] == 22, "no tail without the ackring7 flag");
        one[30] = 18;  // the module never writes 18..255; ignore it
        check(pico::expand_imu_slot(one, true).dma.size() == 24, "tail out of range");
        one[11] = 36;
        one[30] = 4;   // two samples fill the slot: byte 30 is sample data
        check(pico::expand_imu_slot(one, true).dma.size() == 42, "no tail for two samples");
        check(pico::imu_ring_depth_for(pico::imu_ring_flag23t) == 23, "ackring7 depth");
    }
    std::printf("PASS %d\n", checks);
    return 0;
}
