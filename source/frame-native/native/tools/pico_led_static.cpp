// Offline ELF64 symbol/constant inventory; not a control-flow decompiler.
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>
struct Elf {
    std::vector<uint8_t> bytes;
    explicit Elf(const char* path) {
        std::ifstream f(path,std::ios::binary);
        if(!f) throw std::runtime_error("unreadable ELF");
        bytes.assign(std::istreambuf_iterator<char>(f),{});
        if(bytes.size()<64 || std::memcmp(bytes.data(),"\177ELF\2\1",6)) throw std::runtime_error("ELF64 LE required");
    }
    uint64_t get(uint64_t at,int n) const {
        if(at+unsigned(n)>bytes.size()) throw std::runtime_error("ELF bounds");
        uint64_t x=0;
        for(int i=0;i<n;++i) x|=uint64_t(bytes[size_t(at)+i])<<(8*i);
        return x;
    }
    std::string str(uint64_t at) const {
        std::string s;
        while(at<bytes.size() && bytes[size_t(at)]) s+=char(bytes[size_t(at++)]);
        return s;
    }
    uint64_t section(int i) const { return get(40,8)+uint64_t(i)*get(58,2); }
    uint64_t offset(uint64_t va) const {
        for(int i=0;i<int(get(60,2));++i) {
            const auto s=section(i),base=get(s+16,8),size=get(s+32,8);
            if(get(s+4,4)!=8 && va>=base && va-base<size) return get(s+24,8)+va-base;
        }
        throw std::runtime_error("unmapped virtual address");
    }
};
int main(int argc,char** argv) { try {
    for(int file=1;file<argc;++file) {
        Elf e(argv[file]); std::cout<<"FILE "<<argv[file]<<'\n';
        for(int i=0;i<int(e.get(60,2));++i) {
            const auto sh=e.section(i),type=e.get(sh+4,4);
            if(type!=2 && type!=11) continue;
            const auto strings=e.get(e.section(int(e.get(sh+40,4)))+24,8);
            const auto entry=e.get(sh+56,8);
            if(!entry) continue;
            for(uint64_t a=e.get(sh+24,8),end=a+e.get(sh+32,8);a+entry<=end;a+=entry) {
                const auto name=e.str(strings+e.get(a,4));
                std::string lower=name;
                std::transform(lower.begin(),lower.end(),lower.begin(),[](unsigned char c){return char(std::tolower(c));});
                bool interesting=false;
                for(const char* word:{"pnp","inlier","blob","constellation","algorithm","optics","swift","trackerpose"})
                    if(lower.find(word)!=std::string::npos) interesting=true;
                if(!interesting) continue;
                const auto va=e.get(a+8,8),size=e.get(a+16,8);
                std::cout<<"0x"<<std::hex<<va<<" size=0x"<<size<<' '<<name<<'\n';
                if(!va || !size || size>0x4000 || (e.get(a+4,1)&15)!=2) continue;
                // Recognise only AArch64 CMP (SUBS immediate, Rd=ZR). A nearby 2/3/4 is NOT an LED threshold proof.
                const auto off=e.offset(va);
                for(uint64_t k=0;k+4<=size && off+k+4<=e.bytes.size();k+=4) {
                    const auto ins=uint32_t(e.get(off+k,4));
                    if((ins&0x7f00001fu)==0x7100001fu && !(ins&(1u<<22))) {
                        const auto value=(ins>>10)&4095u;
                        if(value<=8) std::cout<<"  0x"<<va+k<<" raw=0x"<<ins<<" cmp "<<((ins>>31)?'x':'w')<<std::dec<<((ins>>5)&31)<<", #"<<value<<std::hex<<'\n';
                    }
                }
            }
        }
    }
} catch(const std::exception& e) { std::cerr<<e.what()<<'\n'; return 1; }
}
