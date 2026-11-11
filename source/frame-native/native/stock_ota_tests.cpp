#include "tracker_ota_live.hpp"
#include "stock_ota_fake.hpp"
#include <iostream>
using namespace pico;
int main(int argc,char **argv) {try {
    if(argc!=5)throw std::runtime_error("usage: stock_ota_tests IMAGE ALLOWLIST MODULE SCRATCH");
    const auto image=ota_file(argv[1]),module=ota_file(argv[3]);
    const std::string scratch=argv[4];
    auto check=[](bool ok){if(!ok)throw std::runtime_error("Stock regression");};
    auto rejects=[&](auto fn){bool bad=false;try{fn();}catch(const std::exception&){bad=true;}check(bad);};
    Bytes legacy(2048);
    for(const auto &f:ota_state_fields()) {
        const auto b=le32(f.value);std::copy(b.begin(),b.end(),legacy.begin()+f.offset);
    }
    { const auto base=le32(ota_peer_base(pico::devices::chip_le()));std::copy(base.begin(),base.end(),legacy.begin()+1508); } // 1508 is computed from the chip
    check(make_ota_service_state()==legacy); // old 65 state stays byte-identical
    check(ota_peer_base(unhex("d4c3b2a1"))==0x126c606d);
    check(ota_peer_base(unhex("e2c3b2a1"))==0x6a6c606d);
    check(ota_peer_base(unhex("e1c3b2a1"))==0xaa6c606d);
    for(unsigned target=2;target<=3;++target) {
        const auto chip=unhex(target==2?"e2c3b2a1":"e1c3b2a1");
        const std::string serial=target==2?"PCTESTTRACKER0003":"PCTESTTRACKER0002";
        const auto state=make_ota_service_state(nullptr,chip);
        check(word(state,1504)==(target==2?0xA1B2C3E2:0xA1B2C3E1));
        check(word(state,1512)==(target==2?0x40u:0x80u));
        rejects([&]{require_ota_target_state(state);});
        for(size_t off:{size_t(1504),size_t(1508),size_t(1512),size_t(1856),size_t(1532),size_t(1564)}) {
            auto bad=state;bad[off]^=1;rejects([&]{require_ota_target_state(bad,chip);});
        }
        const auto dir=scratch+"/"+std::to_string(target);
        for(unsigned scenario=0;scenario<4;++scenario)
            run_stock_ota_fake(image,argv[2],module,chip,serial,dir,scenario==1,scenario==2,scenario==3);
        run_stock_ota_fake(image,argv[2],module,chip,serial,dir,false,false,false,true);
        run_stock_ota_fake(image,argv[2],module,chip,serial,dir,false,false,false,false,1058);
        run_stock_ota_fake(image,argv[2],module,chip,serial,dir,false,false,false,false,3000);
        const std::string ap=dir+"/synthetic-approval.fixture";
        const std::vector<std::string> av{"fake-only","--stock-approval",ap,"--chip",ota_hex(chip),"--serial",serial};
        const auto sha=sha256(image);
        rejects([&]{StockOtaApproval bad(ap,chip,sha,av,serial,1700000121);});
        auto wrong=av;wrong[1]="--approval";
        rejects([&]{StockOtaApproval bad(ap,chip,sha,wrong,serial,1700000001);});
        rejects([&]{StockOtaApproval bad(ap,unhex("d4c3b2a1"),sha,av,serial,1700000001);});
    }
    for(unsigned occupancy:{50u,64u,65u,75u,100u}) {
        StockCadence cadence;uint32_t cycles=0xffff0000;
        for(unsigned i=0;i<4000;++i) {
            if(i%100<occupancy)cadence.observe(cycles);
            cycles+=4727*64;
        }
        check(cadence.qualified()==(occupancy>=65));
    }
    for(uint32_t period:{3000u,9455u}) {
        StockCadence cadence;for(unsigned i=0;i<4000;++i)cadence.observe(i*period*64);
        check(!cadence.qualified());
    }
    check(ota_readiness_matches("READY 42","42",119));
    check(!ota_readiness_matches("READY 42","42",121));
    std::cout<<"PASS stock targets, expected-sequence gate, S1 queue, ignored 7E, product, lost ACK, FE history, approval, cadence\n";
    return 0;
}catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}}
