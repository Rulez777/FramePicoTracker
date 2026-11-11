#pragma once
#include "roy_owner.hpp"
#include "peer_engine.hpp"
#include "station_alpha/pair_slot.h"
#include <iostream>
namespace pico {
inline uint32_t pair_reverse(uint32_t x) {
    uint32_t y=0;for(unsigned i=0;i<32;++i){y=(y<<1)|(x&1);x>>=1;}return y;
}
inline uint32_t pair_base(uint32_t chip,uint32_t seed) {
    uint32_t sum=0;
    for(unsigned i=0;i<32;i+=8)sum|=uint32_t(uint8_t((chip>>i)+(seed>>i)))<<i;
    return pair_reverse(sum);
}
inline Bytes pair_state(uint32_t chip,unsigned slot) {
    if(slot>2)throw std::runtime_error("Pair requires PICO slot 0..2");  // chip 0 = scan mode: listen for a tracker in discovery, offer nothing
    Bytes s(roy_data_size);auto put=[&](size_t o,uint32_t v){auto b=le32(v);std::copy(b.begin(),b.end(),s.begin()+o);};
    put(0,0x314d5250);put(8,1);put(36,900);put(40,6000);s[84]=255;
    put(1504,chip);put(1508,pair_base(chip,0x15547374));put(1512,pair_reverse(slot)>>24);
    put(1564,1);put(1596,1);put(1608,1000);put(1612,192000000);put(1616,0);
    put(1856,0x15547374);put(1860,6000);put(1864,900);put(1876,900);
    put(1888,1);put(1892,700);put(1896,1);return s;
}
// Product is queued by FW189 605C -> 8A70 after targeted command 9.
// S1 bit0 retires notifications; it is NOT a bond marker. No OTA or mode writes.
class PairProduct {
    uint32_t chip_;std::string expected_;
  public:
    unsigned working=0,products=0,rejected=0;bool ack=false;std::string serial;
    PairProduct(uint32_t chip,std::string expected):chip_(chip),expected_(std::move(expected)){}
    void receive(const Record &r) {
        if(!r.hardware_crc||r.partial||r.profile||r.channel!=2||r.chip!=chip_||
           r.dma.size()<3||r.dma.size()!=size_t(r.dma[0])+2||r.dma[1]<2||r.dma[1]>3){++rejected;return;}
        try {
            auto records=decode_records(Bytes(r.dma.begin()+2,r.dma.end()));++working;
            for(const auto &c:records) {
                if(c.command!=2&&c.command!=10&&c.command!=11)ack=true;
                if(c.command!=1||c.body.size()!=35||c.body[0]!=0x82)continue;
                auto s=std::string(c.body.begin()+8,c.body.begin()+25);
                if(c.body[1]!=189||c.body[2]!=0||s.size()!=17||s.rfind("PC",0)!=0||
                   (!expected_.empty()&&s!=expected_)){++rejected;continue;}
                if(!std::all_of(s.begin(),s.end(),[](char c){return (c>='0'&&c<='9')||(c>='A'&&c<='Z');})){
                    ++rejected;continue;
                }
                serial=s;++products;
            }
        } catch(const std::exception&){++rejected;}
    }
};
}
