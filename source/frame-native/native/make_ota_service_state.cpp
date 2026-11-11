#include "device_config.hpp"
#include "ota_service_state.hpp"
#include <iostream>
int main(int argc,char **argv) {
    try {
        if(argc!=2 && argc!=3)throw std::runtime_error("usage: make_ota_service_state NEW_OUTPUT.bin [WIRE_CHIP]");
        std::ifstream existing(argv[1],std::ios::binary);
        if(existing)throw std::runtime_error("Output already exists");
        const auto chip=pico::unhex(argc==3?std::string(argv[2]):pico::devices::chip_le_hex());
        const auto state=pico::make_ota_service_state(&std::cout,chip);
        std::ofstream out(argv[1],std::ios::binary);out.write(reinterpret_cast<const char*>(state.data()),state.size());
        out.close();if(!out)throw std::runtime_error("State write failed");
        std::cout<<"service38 state verified: "<<state.size()<<" bytes; target "<<pico::ota_hex(chip)<<" only\n";
        return 0;
    }catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}
}
