#pragma once
#include <cstdint>
#include <cstring>
// PTS1 additive telemetry ABI. All integers LE; timestamps CLOCK_MONOTONIC ns.
// Unknown quantities have their validity bit clear, never inferred from zero.
namespace pico_sensors {
enum : uint16_t { Imu=1, Mag=2, Fresh=4, Power=8, Scale=16, RoyTime=32, TemperatureSnapshot=64 };
#pragma pack(push,1)
struct Wire {
    char magic[4]={'P','T','S','1'};
    uint32_t chip=0;
    uint64_t hostNs=0, receiveNs=0;
    uint32_t royCycles=0, ticks=0;
    uint16_t flags=0;
    uint8_t counter=0, imuId=0;
    int16_t gyro[3]{}, accel[3]{}, temperature=0;
    uint16_t mag[3]{};
    uint8_t batteryLevel=0, chargerRaw=0;
    uint16_t millivolts=0; // reserved: not in today's discovery reply
};
#pragma pack(pop)
static_assert(sizeof(Wire)==60,"PTS1 ABI");
inline double Temperature(int16_t raw) { return 25.0+raw/132.48; }
inline double Microtesla(uint16_t raw) { return (int(raw)-32768)*100.0/1024; }
inline double AccelLsb(uint8_t id) { return id==0x44?1024:id==0x47?2048:0; }
inline double GyroScale(uint8_t id) { return id==0x44?4000.0*3.141592653589793/180/32768:id==0x47?2000.0*3.141592653589793/180/32768:0; }
inline bool Decode(const void *data,size_t n,Wire &w) {
    if(n!=sizeof(w)) return false;
    std::memcpy(&w,data,n);
    return !std::memcmp(w.magic,"PTS1",4) && w.chip && w.hostNs && !(w.flags&~127u)
        && (!(w.flags&Fresh)||(w.flags&Mag)) && (!(w.flags&Scale)||AccelLsb(w.imuId)>0)
        && (!(w.flags&Imu)||w.ticks<533333) && (!(w.flags&Power)||(w.batteryLevel<=15 && w.chargerRaw<=3));
}
}
