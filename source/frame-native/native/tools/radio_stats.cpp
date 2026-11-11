// radio_stats <radio.bin> [CHIP]: where IMU samples are lost, from the raw ring dump ([i64 host_ns][112-byte record]).
// CHIP (hex, 2026-10-31 multi4): only records of that tracker (record chip word, the multi4 peer table's chip).
#include "data_plane.hpp"
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <map>
#include <vector>
using namespace pico;
int main(int argc,char**argv){
    if(argc<2)return 2;
    std::ifstream f(argv[1],std::ios::binary);
    const uint32_t only=argc>2?uint32_t(std::strtoul(argv[2],nullptr,16)):0;uint64_t other_chip=0;
    char buf[120];
    uint64_t recs=0,seqgaps=0,badparse=0,imu_recs=0,samples=0,gaps=0,gap_events=0,partial=0;
    std::map<size_t,uint64_t> per_rec;std::map<int,uint64_t> gap_hist;std::map<int,uint64_t> tick_hist;std::map<int,uint64_t> cmds;
    int64_t first=0,last=0;uint32_t lastseq=0;bool hs=false;int lastc=-1;int64_t lastticks=-1;int lastslot=-1;std::map<int,uint64_t> slot_hist;std::map<int,std::vector<double>> slot_dt;uint32_t lastcyc=0;
    while(f.read(buf,120)){
        int64_t t;std::memcpy(&t,buf,8);if(!first)first=t;last=t;++recs;
        Bytes b(buf+8,buf+120);
        Record r;try{r=parse_record(b,true);}catch(...){++badparse;continue;}
        if(only&&r.chip!=only){++other_chip;continue;}
        if(r.partial)++partial;
        if(hs&&r.sequence!=lastseq+1)seqgaps+=r.sequence-lastseq-1;lastseq=r.sequence;hs=true;
        if(lastslot>=0){int d=(int(r.slot)-lastslot+32768)%32768;slot_hist[d>12?99:d]++;if(d<=12)slot_dt[d].push_back((r.cycles-lastcyc)/64.0);}lastslot=int(r.slot);lastcyc=r.cycles;
        if(r.dma.size()<3)continue;
        std::vector<Command> cs;try{cs=decode_records(Bytes(r.dma.begin()+2,r.dma.end()));}catch(...){++badparse;continue;}
        for(auto&c:cs){++cmds[c.command];
            if(c.command!=2)continue;std::vector<Sample> ss;try{ss=decode_imu(c.body);}catch(...){continue;}
            if(ss.empty())continue;++imu_recs;per_rec[ss.size()]++;
            for(auto&s:ss){++samples;
                if(lastc>=0){int d=(int(s.counter)-lastc+256)%256;gap_hist[d>20?99:d]++;if(d>1&&d<128){gaps+=d-1;++gap_events;}}
                lastc=s.counter;
                if(lastticks>=0){int64_t dt=(int64_t(s.ticks)-lastticks+533333)%533333;tick_hist[int(dt/8000)]++;}lastticks=s.ticks;}
        }
    }
    double sec=(last-first)/1e9;
    printf("span %.1f s records %lu (%.1f/s) ring seq gaps %lu partial %lu parse_fail %lu\n",sec,recs,recs/sec,seqgaps,partial,badparse);
    if(only)printf("chip %08x only: %lu records of other chips skipped\n",only,(unsigned long)other_chip);
    printf("imu commands %lu samples %lu (%.1f/s) counter gaps %lu in %lu events\n",imu_recs,samples,samples/sec,gaps,gap_events);
    printf("samples per command:");for(auto&p:per_rec)printf(" %zu:%lu",p.first,p.second);
    printf("\ncounter step:");for(auto&p:gap_hist)printf(" %d:%lu",p.first,p.second);
    printf("\nticks between samples in 0.5 ms bins (16 MHz):");for(auto&p:tick_hist)if(p.second>samples/200)printf(" %d:%lu",p.first,p.second);
    printf("\nslot step between replies:");for(auto&p:slot_hist)printf(" %d:%lu",p.first,p.second);
    printf("\nRoy us between replies by slot step (p10/p50/p90):");
    for(auto&p:slot_dt){auto v=p.second;if(v.size()<20)continue;std::sort(v.begin(),v.end());
        printf(" [%d] %.0f/%.0f/%.0f",p.first,v[v.size()/10],v[v.size()/2],v[v.size()*9/10]);}
    printf("\ncommand types:");for(auto&p:cmds)printf(" %d:%lu",p.first,p.second);printf("\n");
}
