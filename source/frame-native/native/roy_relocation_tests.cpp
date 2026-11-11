// Offline end-to-end install/restore against real firmware and synthetic SRAM.
#include "roy_locator.hpp"
#include "roy_owner.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <cstdlib>
using namespace pico;
static Bytes file(const char* p){std::ifstream f(p,std::ios::binary);return {(std::istreambuf_iterator<char>(f)),{}};}
int main(int argc,char**argv){try{
    if(argc!=3) throw std::runtime_error("usage: roy_relocation_tests new-flash.bin rtt.bin (source locator env first)");
    auto flash=file(argv[1]),rtt=file(argv[2]); const auto stock=flash;
    auto layout=locate_roy_buffers(locate_roy(flash),rtt);
    if(!layout.ok)throw std::runtime_error(layout.why);
    Bytes ram(0x40000,0x5a);std::map<uint32_t,uint8_t> regs;size_t writes=0,byte_writes=0;
    auto put=[&](uint32_t a,const Bytes& b){for(size_t i=0;i<b.size();i++){
        const auto at=a+uint32_t(i);if(at>=0x20000000 && at<0x20040000)ram[at-0x20000000]=b[i];else regs[at]=b[i];}};
    auto read=[&](uint32_t a,size_t n){Bytes b;for(size_t i=0;i<n;i++){
        const auto at=a+uint32_t(i);b.push_back(at<flash.size()?flash[at]:at>=0x20000000&&at<0x20040000?ram[at-0x20000000]:regs[at]);}return b;};
    put(layout.rtt,rtt);put(layout.mpsl,Bytes(48));put(layout.hook,Bytes(4));put(layout.static_word,Bytes{0,0,2,0});
    put(0xe000ed94,le32(5));const auto initial=ram;
    auto command=[&](const std::string& c){std::istringstream in(c);std::string verb,a;in>>verb>>a;
        const auto address=uint32_t(std::stoul(a,nullptr,0));
        if(verb=="mww"){std::string v;in>>v;put(address,le32(uint32_t(std::stoul(v,nullptr,0))));++writes;return std::string();}
        unsigned bits;in>>bits;
        if(verb=="read_memory"){size_t count;in>>count;std::string out;
            for(size_t i=0;i<count;i++){out+=std::to_string(word(read(address+uint32_t(i*4),4)))+" ";}
            return out;}
        if(verb!="write_memory"||(bits!=32&&bits!=8))throw std::runtime_error("unexpected Tcl "+c);
        char brace;in>>brace;if(brace!='{')throw std::runtime_error("write syntax");
        std::string v;uint32_t offset=0;
        while(in>>v){if(v=="}")break;if(v.back()=='}')v.pop_back();if(v.empty())break;
            const auto value=uint32_t(std::stoul(v,nullptr,0));put(address+offset,bits==8?Bytes{uint8_t(value)}:le32(value));offset+=bits/8;}
        ++writes;if(bits==8)++byte_writes;return std::string();
    };
    DebugMemory debug(command);int64_t now=0;
    OwnerIO io{[&](uint32_t a,size_t n){return debug.read(a,n);},[&](uint32_t a,const Bytes& b){debug.write(a,b);},command,[&]{return now;},[&](int64_t n){now+=n;}};
    Bytes state(roy_data_size);auto magic=le32(0x314d5250);std::copy(magic.begin(),magic.end(),state.begin());
    size_t checkpoints=0;
    SramOwner owner(io,[&](const Checkpoint& c){if(c.code.size()!=2044||c.data.size()!=roy_data_size)throw std::runtime_error("checkpoint capacity");++checkpoints;});
    owner.install(stock,Bytes{1,2,3,4},roy_code|1,state);owner.restore();
    if(!owner.barriers.complete()||ram!=initial||checkpoints!=3||byte_writes!=1){
        std::cerr<<owner.barriers.json()<<" ram="<<(ram==initial)<<" checkpoints="<<checkpoints<<" byte_writes="<<byte_writes<<"\n";
        for(size_t i=0;i<ram.size();i++)if(ram[i]!=initial[i]){std::cerr<<"first RAM mismatch "<<std::hex<<i+0x20000000<<"\n";break;}
        throw std::runtime_error("relocated restore mismatch");}
    const auto original=flash;const auto before=writes;
    for(bool duplicate:{false,true}){
        flash=original;
        if(duplicate)std::copy(original.begin()+0x10580,original.begin()+0x10580+48,flash.begin()+0x4000);
        else flash[0x10580]^=1;
        SramOwner refused(io,[](const Checkpoint&){});bool rejected=false;
        try{refused.install(stock,Bytes{1,2,3,4},roy_code|1,state);}catch(const std::exception&){rejected=true;}
        if(!rejected||writes!=before||ram!=initial)throw std::runtime_error("unsafe signature admission");
    }
    flash=original;
    setenv("ROY_FN_REQUEST","0x1894b",1);
    SramOwner stale(io,[](const Checkpoint&){});bool rejected=false;
    try{stale.install(stock,Bytes{1,2,3,4},roy_code|1,state);}catch(const std::exception&){rejected=true;}
    if(!rejected||writes!=before)throw std::runtime_error("stale environment admitted");
    std::cout<<"relocated install/restore, unaligned MPU byte write, missing/duplicate/stale refusal: PASS\n";
    return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 1;}}