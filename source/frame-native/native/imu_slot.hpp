#pragma once
#include "device_config.hpp"
#include "data_plane.hpp"
namespace pico {
// State.enabled (offset 8). The owner checks magic at offset 0 and the request
// result, not this word. Prepared state writes 1; the short-ring module replaces it.
inline constexpr uint32_t imu_ring_flag = 0x494d5532u;
inline constexpr uint32_t imu_ring_depth = 16;
inline constexpr uint32_t imu_ring_stride = 48;
// ackring5 (2026-10-30): the same slots, 23 deep (23 x 48 = 1104 of the 1120 record bytes): a host iteration may
// take up to ~138 ms at 167 replies/s instead of ~96 ms. The flag in State.enabled selects the depth.
inline constexpr uint32_t imu_ring_flag23 = 0x494d5533u;
// ackring7: the same 23 slots; a one-sample IMU slot also carries the records after command 2 (command 23) in
// slot[31..] with their length in slot[30]. Own flag: ackring5 leaves stale sample bytes in slot[30].
inline constexpr uint32_t imu_ring_flag23t = 0x494d5534u;
// multi4 (2026-10-31, docs/MULTI_TRACKER_RADIO_2026-10-31.md): the same 23 slots plus the peer index (IMU slot
// byte 10, raw slot word0 bits 21..22), no ackring7 tail. expand_multi4_slot maps the index to the chip.
inline constexpr uint32_t imu_ring_flag_multi4 = 0x494d5535u;
inline constexpr uint32_t alpha_ring_single=0x494d5536u, alpha_ring_multi=0x494d5537u;
inline uint32_t imu_ring_stride_for(uint32_t flag) {return flag==alpha_ring_single||flag==alpha_ring_multi?80u:48u;}
inline constexpr uint32_t imu_ring_depth_max = 23;
inline constexpr uint32_t imu_ring_depth_for(uint32_t enabled) {
    return enabled==alpha_ring_single||enabled==alpha_ring_multi ? 14u : enabled == imu_ring_flag ? imu_ring_depth
           : enabled == imu_ring_flag23 || enabled == imu_ring_flag23t || enabled == imu_ring_flag_multi4 ? 23u : 0u;
}
// Short-ring slots (station module ackring4, 2026-10-30). Both kinds carry the Roy cycles and the mailbox
// generation (record crc>>16) so ack_context() attributes the reply as for a 112-byte record; only
// checksum-valid working-channel (2) replies are published.
//  IMU slot: sequence u32, cycles u32, generation u16, channel u8, n u8, body[36]; n is 18 or 36, the
//            command-2 body (one or two 18-byte samples).
//  raw slot: word0 = bit31 | rest<<24 | (s1==3)<<23 | sequence (23 bits), cycles u32, generation u16,
//            payload[rest] (DMA bytes 2..len of a reply without command 2, rest 1..38). Length, s1 and the
//            checksum are rebuilt here.
//  tail (ackring7 only): n == 18 and slot[30] = e (1..17): DMA bytes after the command-2 record, appended before
//            the rebuilt checksum.
inline Record expand_imu_slot(const uint8_t *p, bool tail = false) {
    Record r;
    auto u32le = [&](int o) {
        return uint32_t(p[o]) | uint32_t(p[o + 1]) << 8 | uint32_t(p[o + 2]) << 16 | uint32_t(p[o + 3]) << 24;
    };
    const uint32_t word0 = u32le(0);
    r.cycles = u32le(4);
    r.generation = uint32_t(p[8]) | uint32_t(p[9]) << 8;
    r.crc = r.generation << 16;
    r.channel = 2;
    r.chip = pico::devices::chip();
    r.hardware_crc = true;
    r.profile = 0;
    if (word0 >> 31) {
        const size_t rest = (word0 >> 24) & 0x7f;
        if (rest > 38)  // ackring8: rest 0 is an empty reply (tracker ACK), ackring4..7 dropped it
            return Record{};
        r.sequence = word0 & 0x7fffffu;
        Bytes dma(rest + 3);
        dma[0] = uint8_t(rest + 1);
        dma[1] = (word0 >> 23) & 1 ? 3 : 2;
        unsigned sum = 0;
        for (size_t i = 0; i < rest; ++i)
            sum += dma[2 + i] = p[10 + i];
        dma.back() = uint8_t(sum);
        r.dma = std::move(dma);
        return r;
    }
    const uint8_t n = p[11];
    if ((n != 18 && n != 36) || p[10] != 2)
        return Record{};
    r.sequence = word0;
    const uint8_t e = tail && n == 18 && p[30] >= 1 && p[30] <= 17 ? p[30] : 0;
    Bytes dma(size_t(n) + 6 + e);
    dma[0] = uint8_t(n + 4 + e);
    dma[1] = 2;
    dma[2] = 0;
    dma[3] = 2;
    dma[4] = n;
    for (uint8_t i = 0; i < n; ++i)
        dma[5 + i] = p[12 + i];
    for (uint8_t i = 0; i < e; ++i)
        dma[5 + n + i] = p[31 + i];
    unsigned sum = 0;
    for (size_t i = 2; i + 1 < dma.size(); ++i)
        sum += dma[i];
    dma.back() = uint8_t(sum);
    r.dma = std::move(dma);
    return r;
}
// Complete raw envelopes from the sensor-preserving ackring8/multi4 extensions.
inline Record expand_alpha_slot(const uint8_t *p,const std::array<uint32_t,4>& chips,bool multi) {
    const uint32_t w=uint32_t(p[0])|uint32_t(p[1])<<8|uint32_t(p[2])<<16|uint32_t(p[3])<<24;
    const unsigned peer=(w>>21)&3, rest=(w>>24)&127;
    if(!(w>>31)||rest>70 || (!multi && peer) || (multi && (peer>=3||!chips[peer])))return {};
    Record r;r.sequence=w&0x7ffff;r.channel=2;r.hardware_crc=true;r.profile=0;
    r.chip=multi?chips[peer]:pico::devices::chip();
    r.cycles=uint32_t(p[4])|uint32_t(p[5])<<8|uint32_t(p[6])<<16|uint32_t(p[7])<<24;
    r.generation=uint32_t(p[8])|uint32_t(p[9])<<8;r.crc=r.generation<<16;
    r.dma={uint8_t(rest+1),uint8_t((w>>19)&3)};
    unsigned sum=0;for(unsigned i=0;i<rest;++i){sum+=p[10+i];r.dma.push_back(p[10+i]);}
    r.dma.push_back(uint8_t(sum));return r;
}
// multi4 slot -> record of the enrolled chip. rejected = peer index outside 0..3 or without an enrolled chip:
// the slot is dropped and counted by the caller, never attributed to another tracker.
struct MultiSlot {
    Record record;  // empty dma = drop
    unsigned peer = 0;
    bool rejected = false;
};
inline MultiSlot expand_multi4_slot(const uint8_t *p, const std::array<uint32_t, 4> &chips) {
    uint8_t q[48];
    for (int i = 0; i < 48; ++i)
        q[i] = p[i];
    MultiSlot m;
    if (q[3] >> 7) {
        m.peer = (q[2] >> 5) & 3;  // word0 bits 21..22
        q[2] &= 0x9f;              // leave the 21-bit sequence for expand_imu_slot
    } else {
        m.peer = q[10];
        if (m.peer >= chips.size() || q[11] == 0) {  // n == 0: never written (blank slot), not a peer error
            m.rejected = q[11] != 0;
            return m;
        }
        q[10] = 2;  // expand_imu_slot checks the channel byte
    }
    if (!chips[m.peer]) {
        m.rejected = true;
        return m;
    }
    m.record = expand_imu_slot(q, false);
    if (!m.record.dma.empty())
        m.record.chip = chips[m.peer];
    return m;
}
} // namespace pico
