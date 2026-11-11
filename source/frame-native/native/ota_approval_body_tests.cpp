#include "stock_ota_admission.hpp"
#include <iostream>
int main(int argc,char **argv) {
    try {
        if(argc!=2 && argc!=4)throw std::runtime_error("usage: ota_approval_body_tests SHELL_PROPOSAL_FIXTURE");
        pico::Bytes chip{0xd4, 0xc3, 0xb2, 0xa1};
        if(argc==4) {
            const std::string hex=argv[2];
            if(hex.size()!=8)throw std::runtime_error("Bad test chip");
            for(size_t i=0;i<4;++i)chip[i]=uint8_t(std::stoul(hex.substr(2*i,2),nullptr,16));
        }
        const std::string serial=argc==4?argv[3]:"PCTESTTRACKER0001";
        const std::string sha(64,'a');
        std::vector<std::string> exact{"/tmp/tool with space","--ota","quote' dollar$ backslash\\","line\nbreak",""};
        if(argc==4)exact.insert(exact.end(),{"--chip",argv[2],"--serial",serial,"--stock-approval","mock.fixture"});
        pico::StockOtaApproval good(argv[1],chip,sha,exact,serial,1700000001);
        unsigned rejected=0;
        auto rejects=[&](auto action){try{action();}catch(const std::exception&){++rejected;}};
        rejects([&]{pico::StockOtaApproval bad(argv[1],chip,sha,exact,serial,1700000121);});
        rejects([&]{pico::StockOtaApproval bad(argv[1],chip,sha,exact,serial,1699999999);});
        auto changed=exact;changed[0]+="!";
        rejects([&]{pico::StockOtaApproval bad(argv[1],chip,sha,changed,serial,1700000001);});
        rejects([&]{pico::StockOtaApproval bad(argv[1],chip,std::string(64,'b'),exact,serial,1700000001);});
        rejects([&]{pico::StockOtaApproval bad(argv[1],pico::Bytes{1,2,3,4},sha,exact,serial,1700000001);});
        if(rejected!=5)throw std::runtime_error("Approval negative regression");
        std::cout<<"PASS: actual shell proposal accepted; spaces/quote/dollar/backslash/newline/empty argv; 5 negatives\n";
        return 0;
    }catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}
}
