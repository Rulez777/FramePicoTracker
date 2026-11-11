#include "roy_locator.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
static std::vector<uint8_t> load(const char* p){std::ifstream f(p,std::ios::binary);return {(std::istreambuf_iterator<char>(f)),{}};}
int main(int argc,char**argv){
    if(argc!=3){std::cerr<<"usage: roy_layout flash.bin rtt.bin|--rtt-address\n";return 2;}
    auto r=pico::locate_roy(load(argv[1]));
    if(std::string(argv[2])=="--rtt-address"){
        if(!r.ok){std::cerr<<"roy_layout refused "<<r.why<<"\n";return 1;}
        std::cout<<"0x"<<std::hex<<r.rtt<<"\n";return 0;
    }
    r=pico::locate_roy_buffers(r,load(argv[2]));
    if(!r.ok){std::cout<<"roy_layout refused "<<r.why<<"\n";return 1;}
    std::cout<<"roy_layout ok\n";
    auto emit=[](const char* key,uint32_t v){std::cout<<key<<"=0x"<<std::hex<<v<<"\n";};
    emit("ROY_FN_OPEN",r.fn_open);emit("ROY_FN_CLOSE",r.fn_close);
    emit("ROY_FN_REQUEST",r.fn_request);emit("ROY_FN_LOCK",r.fn_lock);
    emit("ROY_FN_UNLOCK",r.fn_unlock);emit("ROY_FN_PRINT",r.fn_print);
    emit("ROY_MPSL",r.mpsl);emit("ROY_RTT",r.rtt);emit("ROY_HOOK",r.hook);
    emit("ROY_STATIC",r.static_word);emit("ROY_REGIONS",r.regions);
    emit("ROY_CODE",r.code);emit("ROY_DATA",r.data);
    emit("ROY_MPU_BASE2",(r.code&~0xfffu)|0x12u);
    // Only whole words are writable; all module state is below byte 2044.
    std::cout<<std::dec<<"ROY_CODE_SIZE="<<r.code_size<<"\nROY_DATA_SIZE="<<(r.data_size&~3u)<<"\n";
}