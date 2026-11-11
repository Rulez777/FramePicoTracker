// Read-only recorded-data audit. No device access and no fitted scale.
#include "sensors.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <cmath>
#include <algorithm>
using namespace pico;
int main(int argc,char**argv) {
    if(argc!=2)return 2;
    std::ifstream in(argv[1],std::ios::binary);if(!in)return 3;
    std::map<std::string,size_t> tails;std::vector<double> norms;
    size_t records=0,bad=0,repeats=0,rails=0;
    std::map<uint32_t,std::array<uint16_t,3>> previous;
    std::map<unsigned,size_t> lengths;
    std::map<uint32_t,size_t> chips;
    int64_t stamp=0;Bytes bytes(112);
    while(in.read(reinterpret_cast<char*>(&stamp),8)) {
        if(!in.read(reinterpret_cast<char*>(bytes.data()),112))return 4;
        ++records;
        try {
            const auto r=parse_record(bytes,true);
            if(!r.hardware_crc || r.channel!=2 || r.dma.size()<3)continue;
            const auto cmds=decode_records(Bytes(r.dma.begin()+2,r.dma.end()));
            for(const auto& c:cmds) if(c.command==2) {
                const auto block=decode_sensors(c.body,SensorFormat::Stock189);
                if(!block.mag)continue;
                const auto m=*block.mag;
                if(previous.count(r.chip) && previous.at(r.chip)==m)++repeats;
                previous[r.chip]=m;++chips[r.chip];++lengths[unsigned(c.body.size())];
                if(std::any_of(m.begin(),m.end(),[](auto v){return v==0 || v==65535;}))++rails;
                if(norms.empty())std::cout<<"first_chip="<<std::hex<<r.chip<<std::dec
                    <<" first_dma="<<hex(r.dma.data(),r.dma.size())<<"\n";
                ++tails[hex(c.body.data()+c.body.size()-6,6)];
                double norm=0;for(auto v:m){const auto u=pico_sensors::Microtesla(v);norm+=u*u;}
                norms.push_back(std::sqrt(norm));
            }
        }catch(const std::exception&){++bad;}
    }
    std::sort(norms.begin(),norms.end());
    std::cout<<"records="<<records<<" malformed="<<bad<<" tails="<<norms.size()<<" repeats="<<repeats<<" distinct="<<tails.size()<<"\n";
    for(const auto& v:tails)std::cout<<"tail="<<v.first<<" count="<<v.second<<"\n";
    for(const auto& v:chips)std::cout<<"chip="<<std::hex<<v.first<<std::dec<<" tails="<<v.second<<"\n";
    for(const auto& v:lengths)std::cout<<"body_length="<<v.first<<" tails="<<v.second<<"\n";
    std::cout<<"rail_observations="<<rails<<" calibration_corrected_norm=unavailable (no chip calibration/mounting supplied)\n";
    if(!norms.empty())std::cout.precision(12),std::cout<<"raw_uT min="<<norms.front()<<" p50="<<norms[norms.size()/2]<<" p90="<<norms[(norms.size()-1)*9/10]<<" max="<<norms.back()<<" valid_10_100="<<std::count_if(norms.begin(),norms.end(),[](double v){return v>=10&&v<=100;})<<"\n";
}
