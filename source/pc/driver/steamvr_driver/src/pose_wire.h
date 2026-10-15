/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
 */
#pragma once
#include <cstdint>
#include <cstring>
#include <cmath>

// PTP2 little-endian ABI, Linux CLOCK_MONOTONIC nanoseconds on Frame.
#pragma pack(push, 1)
struct PoseFrameWire {
    char magic[4];
    uint32_t chip, flags;
    uint64_t sequence, sourceTime;
    float position[3], quaternion[4], linearVelocity[3], angularVelocity[3];
    float batteryPercent, quality;
};
#pragma pack(pop)
static_assert(sizeof(PoseFrameWire)==88, "PTP2 ABI");

inline bool DecodePose(const void *data, size_t size, uint64_t now, PoseFrameWire &p) {
    if (size!=sizeof(p)) return false;
    std::memcpy(&p,data,sizeof(p));
    if (std::memcmp(p.magic,"PTP2",4) || !p.chip || (p.flags & ~7u)
        || !p.sourceTime || p.sourceTime>now || now-p.sourceTime>250000000u
        || ((p.flags&2u) && (!(p.flags&1u) || p.quality<=0))) return false;
    float values[15];
    std::memcpy(values,static_cast<const char*>(data)+28,sizeof(values));
    for (float value:values) if (!std::isfinite(value) || std::abs(value)>1e6) return false;
    double norm=0;
    for(float q:p.quaternion) norm+=q*q;
    return norm>=.81 && norm<=1.21 && p.quality>=0 && p.quality<=1
        && ((p.flags&4u) ? p.batteryPercent>=0 && p.batteryPercent<=100 : p.batteryPercent==-1);
}
