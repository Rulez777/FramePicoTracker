#include "tracker_ota_fake.hpp"
#include "tracker_ota_live.hpp"
#include "stock_ota_fake.hpp"
#include <iostream>
#include <iterator>
int main(int argc,char **argv) {
    try {
        if(argc>1 && std::string(argv[1])=="--ota") return pico::ota_live_entry(argc,argv);
        if(argc==8 && std::string(argv[1])=="--dry-run-stock") {
            pico::run_stock_ota_fake(pico::ota_file(argv[2]),argv[3],pico::ota_file(argv[4]),
                pico::unhex(argv[5]),argv[6],argv[7]);return 0;
        }
        if (argc!=4 || std::string(argv[1])!="--dry-run") {
            std::cerr<<"usage: tracker_ota_tool --dry-run IMAGE ALLOWLIST\n"; return 2;
        }
        std::ifstream in(argv[2],std::ios::binary);
        if (!in) throw std::runtime_error("Cannot read image");
        pico::Bytes image((std::istreambuf_iterator<char>(in)),{});
        double t=0;
        pico::TrackerOta sender(image,argv[3],[&]{return t;});
        const pico::Bytes nonce(16,0x42),chip{1,2,3,4};
        sender.begin(pico::fake_service_proof(nonce,chip),nonce,chip,0);
        unsigned last=0;
        pico::run_fake_ota(sender,[&](unsigned done,unsigned total){
            if (done!=last && (done==total || done/100!=last/100))
                std::cout<<"progress "<<done<<"/"<<total<<"\n";
            last=done;
        });
        std::cout<<"finish_verified fake-only; boot_verified=false activation_verified=false\n";
        return 0;
    } catch(const std::exception &e) {std::cerr<<e.what()<<'\n';return 1;}
}
