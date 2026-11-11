#include "image_admission.hpp"
#include <fstream>
#include <iostream>
using namespace pico;
static Bytes load(const char* path){std::ifstream f(path,std::ios::binary);if(!f)throw std::runtime_error(path);
return Bytes((std::istreambuf_iterator<char>(f)),{});}
int main(int argc,char**argv){try{
    if(argc!=4)throw std::runtime_error("usage: alpha_module_tests STOCK SINGLE MULTI");
    const auto stock=load(argv[1]);
    for(int i=2;i<4;++i){const auto elf=load(argv[i]);const auto m=admit_original210(stock,elf);
        if((m.code.size()+3)/4*4>2044)throw std::runtime_error("checkpoint overflow");
        auto corrupt=elf;corrupt.back()^=1;bool denied=false;try{admit_original210(stock,corrupt);}catch(...){denied=true;}
        if(!denied)throw std::runtime_error("corrupt ELF admitted");
        std::cout<<argv[i]<<" sha="<<sha256(elf)<<" code="<<m.code.size()<<" aligned_spare="<<2044-(m.code.size()+3)/4*4<<" PASS\n";
    }return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<"\n";return 1;}}
