// radio_tail <radio.bin> [last N=60]: the last N tracker replies (host time from the end, slot step, raw DMA bytes,
// decoded commands with body bytes), to see what the tracker answered right before it went silent.
#include "data_plane.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <vector>
using namespace pico;
int main(int argc,char**argv){
    if(argc<2)return 2;
    const size_t n=argc>2?size_t(std::atoi(argv[2])):60;
    std::ifstream f(argv[1],std::ios::binary);
    struct R{int64_t t;Bytes b;};std::vector<R> all;char buf[120];
    while(f.read(buf,120)){int64_t t;std::memcpy(&t,buf,8);all.push_back({t,Bytes(buf+8,buf+120)});}
    if(all.empty())return 3;
    const int64_t tEnd=all.back().t;int lastslot=-1;
    for(size_t i=all.size()>n?all.size()-n:0;i<all.size();++i){
        Record r;try{r=parse_record(all[i].b,true);}catch(...){std::printf("%5zu parse fail\n",i);continue;}
        int d=lastslot>=0?(int(r.slot)-lastslot+32768)%32768:0;lastslot=int(r.slot);
        std::printf("%5zu t-%7.3fs seq %u slot+%d ch %u crc %d dma[%zu]:",i,(tEnd-all[i].t)/1e9,r.sequence,d,unsigned(r.channel),int(r.hardware_crc),r.dma.size());
        for(size_t k=0;k<r.dma.size()&&k<24;++k)std::printf(" %02x",r.dma[k]);
        if(r.dma.size()>=3){try{auto cs=decode_records(Bytes(r.dma.begin()+2,r.dma.end()));std::printf(" |");for(auto&c:cs){std::printf(" cmd%d[%zu]",c.command,c.body.size());for(size_t k=0;k<c.body.size()&&k<6;++k)std::printf("%02x",c.body[k]);}}catch(...){std::printf(" | decode fail");}}
        std::printf("\n");}
}
