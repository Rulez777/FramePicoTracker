// radio_peers <radio.bin>: replies per Record.chip / profile (multi4 ring: who answered), with command types per chip.
#include "data_plane.hpp"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <map>
using namespace pico;
int main(int argc,char**argv){
    if(argc<2)return 2;
    std::ifstream f(argv[1],std::ios::binary);char buf[120];
    std::map<std::pair<uint32_t,int>,uint64_t> n;std::map<std::pair<uint32_t,int>,std::map<int,uint64_t>> cmds;std::map<std::pair<uint32_t,int>,double> first,last;
    while(f.read(buf,120)){int64_t t;std::memcpy(&t,buf,8);Record r;try{r=parse_record(Bytes(buf+8,buf+120),true);}catch(...){continue;}
        auto k=std::make_pair(r.chip,r.profile);++n[k];if(!first.count(k))first[k]=t/1e9;last[k]=t/1e9;
        if(r.dma.size()>=3){try{for(auto&c:decode_records(Bytes(r.dma.begin()+2,r.dma.end())))++cmds[k][c.command];}catch(...){}}}
    for(auto&p:n){std::printf("chip %08x profile %d: %lu replies over %.1f s |",p.first.first,p.first.second,p.second,last[p.first]-first[p.first]);
        for(auto&c:cmds[p.first])std::printf(" %d:%lu",c.first,c.second);std::printf("\n");}
}
