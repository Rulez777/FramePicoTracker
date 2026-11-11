#pragma once
#include "device_config.hpp"
#include "tracker_ota_radio.hpp"
#include <ostream>
namespace pico {
struct OtaStateField { size_t offset; uint32_t value; const char *meaning; };
// Reviewed phase189 initialize_v4 + loader service/OTA overrides. See OTA_ALPHA11_RUN.md.
inline const std::vector<OtaStateField>& ota_state_fields() {
    static const std::vector<OtaStateField> fields{
        {0,0x314d5250,"ABI4 magic"},{8,1,"profile enabled (legacy State.enabled)"},
        {36,900,"slot us"},{40,4727,"Service poll us"},
        {44,1,"profile frequency"},{48,4,"profile mode"},{52,262152,"profile pcnf0"},
        {56,17039510,"profile pcnf1"},{60,1428974110,"profile base0"},
        {64,192,"profile prefix0"},{68,2,"profile crccnf"},{72,69665,"profile crcpoly"},
        {76,65535,"profile crcinit"},{80,0,"profile whiteiv"},{84,255,"session sentinel (byte + zero padding)"},
        {1504,pico::devices::chip(),"peer0 target chip"},{1508,0,"peer0 RBIT32(bytewise chip+seed), computed from the chip"},
        {1512,0,"peer0 prefix"},{1532,0,"peer0 unpaused"},
        {1544,128,"empty peer1 prefix RBIT8(1)"},{1564,1,"empty peer1 paused"},
        {1576,64,"empty peer2 prefix RBIT8(2)"},{1596,1,"empty peer2 paused"},
        {1608,1000,"host watchdog slot allowance (renewed by owner.repeat)"},
        {1612,2560000000u,"OTA reply timeout 40s at 64MHz"},{1616,0,"MPSL priority"},
        {1856,0x15547374,"station seed wire 74735415"},{1860,4727,"legacy steady interval us"},
        {1864,900,"legacy steady slot us"},{1876,900,"handshake slot us"},
        {1880,0,"phase adjustment disabled"},{1884,0,"phase reference disabled"},
        {1888,1,"slot cycle K1"},{1892,800,"RX deadline: 900 minus phase189 margin100"},
        {1896,1,"legacy service_poll flag"}};
    return fields;
}
inline unsigned ota_pico_slot(const Bytes &chip) {
    if(const int slot=pico::devices::slot_of(chip);slot>=0)return unsigned(slot); // trackers listed in devices.conf
    throw std::runtime_error("Unreviewed OTA target");
}
inline uint32_t ota_reverse(uint32_t x) {
    uint32_t y=0;for(unsigned i=0;i<32;++i){y=(y<<1)|(x&1);x>>=1;}return y;
}
inline uint32_t ota_peer_base(const Bytes &chip) {
    const auto seed=unhex("74735415");Bytes sum(4);
    for(unsigned i=0;i<4;++i)sum[i]=uint8_t(chip.at(i)+seed[i]);
    return ota_reverse(word(sum));
}
inline void require_ota_target_state(const Bytes &s,const Bytes &chip=pico::devices::chip_le()) {
    require_ota_service_state(s);
    const auto slot=ota_pico_slot(chip);
    if(word(s)!=0x314d5250 || word(s,1504)!=word(chip) || word(s,1532) ||
       word(s,1508)!=ota_peer_base(chip) || word(s,1512)!=(ota_reverse(slot)>>24) ||
       word(s,1856)!=0x15547374)
        throw std::runtime_error("OTA target peer0 mismatch");
    for(size_t b:{size_t(1536),size_t(1568)})
        if(word(s,b)||word(s,b+28)!=1)throw std::runtime_error("OTA secondary peer enabled");
}
inline Bytes make_ota_service_state(std::ostream *log=nullptr,const Bytes &chip=pico::devices::chip_le()) {
    const auto slot=ota_pico_slot(chip);
    Bytes s(roy_data_size,0); // counters, DMA, records, mailboxes, stages and trace saves start empty
    for(const auto &f:ota_state_fields()) {
        uint32_t value=f.value;
        if(f.offset==1504)value=word(chip);
        if(f.offset==1508)value=ota_peer_base(chip);
        if(f.offset==1512)value=ota_reverse(slot)>>24;
        const auto v=le32(value);std::copy(v.begin(),v.end(),s.begin()+f.offset);
        if(log && value)*log<<f.offset<<" = "<<value<<" : "<<f.meaning<<'\n';
    }
    if(log)*log<<"OTA target wire="<<ota_hex(chip)<<" pico_slot="<<slot
               <<" base="<<word(s,1508)<<" prefix="<<word(s,1512)<<'\n';
    require_ota_target_state(s,chip);return s;
}
}
