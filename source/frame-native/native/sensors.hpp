#pragma once
#include "data_plane.hpp"
#include <algorithm>
#include <cstring>
#include <limits>
namespace pico {
inline SensorFormat sensor_format_for_revision(unsigned revision,bool stock=false) {
    if(stock)return SensorFormat::Stock189;
    switch(revision){case 7:return SensorFormat::Diagnostic7;case 11:return SensorFormat::Release11;
        case 12:return SensorFormat::Alpha12;default:return SensorFormat::Unknown;}
}
inline const char* sensor_format_name(unsigned revision,bool stock=false) {
    if(stock)return "stock189";
    switch(revision){case 7:return "diagnostic7";case 11:return "release11";case 12:return "alpha12";default:return "unknown";}
}
// Stock/rev7/rev11 retain their layout. Alpha requires attributed revision 12
// AND the compact marker with its exact length; legacy gyro bytes can match it.

struct SensorBlock { std::vector<Sample> imu; std::optional<std::array<uint16_t,3>> mag; std::optional<int16_t> temperature; };
inline SensorBlock decode_sensors(const Bytes &b,SensorFormat format=SensorFormat::Stock189) {
    if(format==SensorFormat::Unknown) throw std::runtime_error("unsupported sensor format");
    SensorBlock out;
    bool magnetic=false;
    if(format==SensorFormat::Alpha12) {
        if(b.size()<3 || (b[0]&0xf8)!=0xc8 || b.size()!=3+16*(b[0]&3)+((b[0]&4)?6u:0u))
            throw std::runtime_error("Alpha 1 marker/length");
        out.temperature=int16_t(uint16_t(b[1]|unsigned(b[2])<<8));
        for(size_t i=0;i<(b[0]&3);++i) {
            const size_t o=3+16*i;
            Bytes legacy(b.begin()+o,b.begin()+o+12);
            legacy.push_back(b[1]);legacy.push_back(b[2]);
            legacy.insert(legacy.end(),b.begin()+o+12,b.begin()+o+16);
            out.imu.push_back(decode_imu(legacy).front());
        }
        magnetic=(b[0]&4)!=0;
    } else { out.imu=decode_imu(b);magnetic=b.size()%18==6; }
    if(magnetic) {
        std::array<uint16_t,3> m{}; const size_t o=b.size()-6;
        for(size_t i=0;i<3;++i) m[i]=uint16_t(b[o+2*i]|unsigned(b[o+2*i+1])<<8);
        out.mag=m;
    }
    return out;
}
// FW189 6844 builder: 13-byte discovery body at DMA+2, status at body+10.
// Battery level and the two raw power bits are all it actually copies; no mV/%.
inline std::optional<uint32_t> discovery_sensor_chip(const Bytes&b) {
    if(b.size()!=15 || b[0]!=13 || b[2]!=2 || b[3]!=0 || b[4]!=10 || b[5]!=0 || b[13]!=0xbd || b[14]!=0) return {};
    // 0x7D18 writes FICR DEVICEID[1] at body+5. A search-channel reply can be from another chip.
    const uint32_t id=uint32_t(b[7])|uint32_t(b[8])<<8|uint32_t(b[9])<<16|uint32_t(b[10])<<24;
    return id?std::optional<uint32_t>(id):std::nullopt;
}
inline bool decode_discovery_power(const Record&r,pico_sensors::Wire&w) {
    const auto id=discovery_sensor_chip(r.dma);
    if(!id || *id!=r.chip) return false;
    const auto&b=r.dma;
    w.flags|=pico_sensors::Power; w.batteryLevel=b[12]&15; w.chargerRaw=(b[12]>>4)&3; return true;
}
}
