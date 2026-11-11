#include "ota_service_state.hpp"
#include "phase5/json.hpp"
#include <iostream>
#include <iterator>
namespace {
phase5::Value json(const std::string &p) {
    std::ifstream f(p,std::ios::binary);if(!f)throw std::runtime_error("Missing reference: "+p);
    const std::string text((std::istreambuf_iterator<char>(f)),{});
    return phase5::Json(text).parse();
}
uint32_t num(const phase5::Value &v) {return uint32_t(std::stoull(v.text,nullptr,0));}
void check(bool b,const char *why){if(!b)throw std::runtime_error(why);}
}
int main(int argc,char **argv) {
    try {
        if(argc!=3)throw std::runtime_error("usage: ota_service_state_tests /REF/outputs/phase189 WRITTEN_STATE.bin");
        const std::string root=argv[1];
        const auto config=json(root+"/ota-proposal/station-config.json");
        const auto profile=json(root+"/ota-proposal/bootstrap205.profile.json");
        const auto report=json(root+"/remote-attempt/radio-run.json");
        const auto &peers=report.object.at("station_peers").array;
        check(report.object.at("tracker_ota").object.at("state").text=="finish_verified","Historical OTA not successful");
        check(report.object.at("deployment").object.at("elf_sha256").text==pico::OtaRadioAdapter::module_sha,"Historical module mismatch");
        pico::Bytes expected(2048);
        auto put=[&](size_t o,uint32_t n){for(unsigned i=0;i<4;++i)expected[o+i]=uint8_t(n>>(8*i));};
        // Independent reconstruction of phase189 initialize_v4/loader, taking
        // actual config/profile/peer values from retained successful evidence.
        put(0,0x314d5250);put(8,1);expected[84]=255;
        put(36,num(config.object.at("slot_us")));put(40,num(config.object.at("interval_us")));
        size_t off=44;
        for(const char *key:{"frequency","mode","pcnf0","pcnf1","base0","prefix0","crccnf","crcpoly","crcinit","whiteiv"}) {
            put(off,num(profile.object.at(key)));off+=4;
        }
        for(size_t i=0;i<3;++i) {
            const auto &p=peers.at(i).object;
            put(1504+32*i,i?0:pico::word(pico::unhex(p.at("id_le").text)));
            put(1508+32*i,num(p.at("base")));put(1512+32*i,num(p.at("prefix")));
            put(1532+32*i,i?1:0);
        }
        const auto seed=pico::unhex(config.object.at("seed_le").text);
        std::copy(seed.begin(),seed.end(),expected.begin()+1856);
        // Independently verify address transform against recorded peer base.
        const auto chip=pico::unhex("d4c3b2a1");uint32_t sum=0,reverse=0;
        for(unsigned i=0;i<4;++i)sum|=uint32_t(uint8_t(chip[i]+seed[i]))<<(8*i);
        for(unsigned i=0;i<32;++i){reverse=(reverse<<1)|(sum&1);sum>>=1;}
        check(reverse==pico::word(expected,1508),"Historical address/seed mismatch");
        put(1608,1000);put(1612,40u*64000000u);
        put(1616,num(config.object.at("mpsl_priority")));
        put(1860,num(config.object.at("steady_interval_us")));put(1864,num(config.object.at("slot_us")));
        put(1876,num(config.object.at("slot_us")));put(1888,num(config.object.at("slot_cycle")));
        put(1892,num(config.object.at("slot_us"))-num(config.object.at("slot_end_margin_us")));
        check(config.object.at("service_poll").text=="true","Expected historical Service poll");put(1896,1);
        std::ifstream written(argv[2],std::ios::binary);
        check(bool(written),"Written state unavailable");
        const pico::Bytes actual((std::istreambuf_iterator<char>(written)),{});
        check(actual==pico::make_ota_service_state(),"Written state differs from generator");
        check(actual==expected,"State differs from historical reconstruction");
        pico::require_ota_target_state(actual);
        unsigned rejected=0;
        for(size_t o:{size_t(0),size_t(36),size_t(40),size_t(1504),size_t(1532),size_t(1536),size_t(1564),size_t(1568),size_t(1596),size_t(1880),size_t(1884),size_t(1888)}) {
            auto bad=actual;bad[o]^=1;
            try{pico::require_ota_target_state(bad);}catch(const std::exception&){++rejected;}
        }
        check(rejected==12,"Live admission negative regression");
        std::cout<<"PASS: written 2048 historical bytes, recorded address, live target checks, 12 negative cases\n";
        return 0;
    }catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}
}
