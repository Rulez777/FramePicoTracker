// slotline <pico-diag.log> <radio.bin> [rows]: per station_counters snapshot, a timeline of the last 16 slot
// starts (A = a reply was captured in that slot, . = no reply) and the last 8 blocks, in us relative to the newest slot.
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
int main(int argc,char**argv){
    if(argc<3)return 2;
    std::vector<uint32_t> replies;  // Roy DWT of each captured reply
    {std::ifstream f(argv[2],std::ios::binary);char b[120];
     while(f.read(b,120)){uint32_t c;std::memcpy(&c,b+8+104,4);replies.push_back(c);}}
    int want=argc>3?atoi(argv[3]):5,shown=0;
    std::ifstream d(argv[1]);std::string line;
    while(std::getline(d,line)&&shown<want){
        if(line.find("station_counters")==std::string::npos)continue;
        uint32_t w[39]={};bool ok=true;
        for(int i=0;i<39;++i){std::string k=" d"+std::to_string(1876+4*i)+"=";auto p=line.find(k);if(p==std::string::npos){ok=false;break;}w[i]=uint32_t(std::stoul(line.substr(p+k.size())));}
        if(!ok)continue;
        std::vector<uint32_t> slots,blocks;
        for(int i=0;i<16;++i)slots.push_back(w[(1932-1876)/4+i]);
        for(int i=0;i<8;++i)blocks.push_back(w[(1996-1876)/4+i]);
        uint32_t ref=*std::max_element(slots.begin(),slots.end(),[&](uint32_t a,uint32_t b){return int32_t(a-slots[0])<int32_t(b-slots[0]);});
        auto rel=[&](uint32_t c){return double(int32_t(c-ref))/64.0;};
        std::sort(slots.begin(),slots.end(),[&](uint32_t a,uint32_t b){return int32_t(a-ref)<int32_t(b-ref);});
        std::sort(blocks.begin(),blocks.end(),[&](uint32_t a,uint32_t b){return int32_t(a-ref)<int32_t(b-ref);});
        if(rel(slots.front())<-1e6)continue;
        printf("snapshot %d\n",++shown);
        size_t bi=0;
        for(size_t i=0;i<slots.size();++i){
            while(bi<blocks.size()&&int32_t(blocks[bi]-slots[i])<0){if(rel(blocks[bi])>rel(slots.front()))printf("      block %9.0f\n",rel(blocks[bi]));++bi;}
            bool a=false;for(auto r:replies){int32_t dt=int32_t(r-slots[i]);if(dt>0&&dt<64*1000){a=true;break;}}
            printf("  %c slot %9.0f  dt %6.0f\n",a?'A':'.',rel(slots[i]),i?rel(slots[i])-rel(slots[i-1]):0.0);
        }
    }
}
