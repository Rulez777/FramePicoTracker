#include "sensors.hpp"
#include "sensor_status.hpp"
#include "imu_slot.hpp"
#include "fake_openocd.hpp"
#include "radio_loop.hpp"
#include "station_alpha/alpha_capture.h"
#include "station_sensors/sensor_capture.h"
#include <fstream>
#include <iostream>
using namespace pico;
static unsigned checks=0;
static void check(bool x){++checks;if(!x)throw std::runtime_error("alpha session check "+std::to_string(checks));}
// Model of the module's complete-envelope writer, including its capacity guard.
static Bytes capture(const Bytes& d,unsigned peer=0) {
    if(d.size()<3||d[0]<1||d[0]>71||d.size()!=size_t(d[0])+2)return {};
    unsigned sum=0;for(unsigned i=2;i<=d[0];++i)sum+=d[i];
    if(uint8_t(sum)!=d.back())return {};
    Bytes slot(80);auto put=[&](unsigned o,uint32_t v){auto b=le32(v);std::copy(b.begin(),b.end(),slot.begin()+o);};
    put(0,0x80000000u|((d[0]-1u)<<24)|(unsigned(d[1]&3)<<19)|(peer<<21)|123);
    put(4,456);slot[8]=6;std::copy(d.begin()+2,d.end()-1,slot.begin()+10);return slot;
}
static Bytes packet(const Bytes& b) {
    Bytes d{uint8_t(b.size()+4),3,0,2,uint8_t(b.size())};d.insert(d.end(),b.begin(),b.end());
    unsigned sum=0;for(size_t i=2;i<d.size();++i)sum+=d[i];
    d.push_back(uint8_t(sum));return d;
}
int main(int argc,char**argv){try {
    const std::array<uint32_t,4> chips{0xA1B2C3D4,2,3,0};
    for(unsigned revision:{7u,11u,12u,0u})for(unsigned count=0;count<=3;++count)for(unsigned mag=0;mag<2;++mag) {
        if(revision!=12 && count>2)continue;
        if(revision!=12 && count==0&&!mag)continue;
        const auto fmt=sensor_format_for_revision(revision,revision==0);
        Bytes b;
        if(revision==12)b={uint8_t(0xc8|count|(mag?4:0)),0x34,0x12};
        for(unsigned i=0;i<count;++i) {
            Bytes sample(18);sample[0]=uint8_t(i+1);sample[12]=0x34;sample[13]=0x12;sample[14]=1;sample[17]=uint8_t(254+i);
            if(revision==12){b.insert(b.end(),sample.begin(),sample.begin()+12);b.insert(b.end(),sample.begin()+14,sample.end());}
            else b.insert(b.end(),sample.begin(),sample.end());
        }
        if(mag)b.insert(b.end(),{1,128,2,128,3,128});
        const auto d=packet(b);const auto expected=decode_sensors(b,fmt);
        check(pico_alpha_ack(d.data(),d[0])==pico_sensor_ack(d.data(),d[0]));
        for(unsigned peers:{1u,2u,3u})for(unsigned peer=0;peer<peers;++peer) {
            const auto slot=capture(d,peer);check(slot.size()==80);
            const auto r=expand_alpha_slot(slot.data(),chips,true);check(r.dma==d&&r.chip==chips[peer]);
            auto commands=decode_records(Bytes(r.dma.begin()+2,r.dma.end()));check(commands.size()==1);
            auto actual=decode_sensors(commands[0].body,fmt);check(actual.mag==expected.mag&&actual.imu.size()==count);
            std::vector<pico_sensors::Wire> telemetry;unsigned decoded=0,sent=0;
            Hooks h;h.now=[]{return 10000000000LL;};h.sensor=[&](const auto&w){telemetry.push_back(w);return true;};
            h.decoded=[&](const Record&,const Sample& s){check(pti1(r.chip,s,123456)==pti1(r.chip,expected.imu.at(decoded++),123456));};
            h.send=[&](const auto& wire){check(wire==pti1(r.chip,expected.imu.at(sent++),10000000000ULL));return true;};
            DataPlane p(h);p.chip=chips[peer];p.enrolled=true;p.sensor_format=fmt;
            p.clock.fit=Fit{456,10000000000.,15.625,0};p.record(r);
            check(decoded==count);check(telemetry.size()==count+mag+(revision==12&&count==0?1u:0u));
            check(sent==count);
            for(const auto&w:telemetry)check(w.chip==chips[peer]);
            FakeOpenOcd fake;fake.set(roy_data+8,alpha_ring_multi);fake.set(roy_data+112,15);fake.set(roy_data+116,14);
            fake.put(roy_data+380,slot);DebugMemory debug([&](const auto&s){return fake.command(s);});
            const auto batch=debug.ring_batch({});check(Bytes(batch.ring.begin(),batch.ring.begin()+80)==slot);
        }
    }
    // Exhaustive malformed record lengths: identical bounded ACK decisions.
    for(unsigned n=0;n<256;++n){Bytes d(260);d[0]=8;d[3]=2;d[4]=uint8_t(n);check(pico_alpha_ack(d.data(),8)==0);}
    check(sensor_format_for_revision(99)==SensorFormat::Unknown);
    {
        SensorStatus status;Bytes nonce(16,1),chip{1,2,3,4},b{0xfe,3};
        b.insert(b.end(),nonce.begin(),nonce.end());b.insert(b.end(),chip.begin(),chip.end());b.insert(b.end(),{1,12,3,6,0,0,254,0,0,0});
        check(status.accept(b,nonce,chip,3,1));b[28]=3;check(status.accept(b,nonce,chip,3,2));
        check(status.mag_observations==2&&status.mag_delta==5&&status.mag_interval==1);
        b[18]^=1;check(!status.accept(b,nonce,chip,3,3));check(status.mag_observations==2);
    }
    for(unsigned marker=0;marker<256;++marker)for(unsigned n=0;n<66;++n) {
        Bytes d(n+6);d[0]=uint8_t(n+4);d[3]=2;d[4]=uint8_t(n);if(n)d[5]=uint8_t(marker);
        check(pico_alpha_ack(d.data(),d[0])==pico_sensor_ack(d.data(),d[0]));
    }
    // One owner loop, three simultaneous decoder revisions, independent planes.
    for(unsigned peers:{2u,3u}) for(bool batched:{false,true}) for(bool mixed:{false,true}) {
        std::vector<std::unique_ptr<DataPlane>> planes;
        unsigned decoded=0,sensors=0;
        for(unsigned peer=0;peer<peers;++peer) {
            Hooks h;h.now=[]{return 10000000000LL;};h.decoded=[&](const auto&,const auto&){++decoded;};
            h.sensor=[&,peer](const auto&w){check(w.chip==chips[peer]);++sensors;return true;};
            auto p=std::make_unique<DataPlane>(h);p->chip=chips[peer];p->enrolled=true;
            p->sensor_format=sensor_format_for_revision(!mixed||peer==0?12:peer==1?7:11);planes.push_back(std::move(p));
        }
        planes[0]->hooks.divert=[&](const Record&r,Stamps){for(unsigned i=1;i<peers;++i)if(planes[i]->chip==r.chip){planes[i]->record(r);return true;}
        return false;};
        FakeOpenOcd fake;fake.set(roy_data,0x314d5250);fake.set(roy_data+8,alpha_ring_multi);
        for(unsigned peer=0;peer<peers;++peer) {
                        const bool compact=!mixed||peer==0;
            Bytes b=compact?Bytes{uint8_t(mixed?0xc9:0xcf),0x34,0x12}:Bytes{};
            for(unsigned k=0;k<(mixed?1u:3u);++k) {
                Bytes sample(compact?16:18);sample.back()=uint8_t(k+1);
                b.insert(b.end(),sample.begin(),sample.end());
            }
            if(!mixed)b.insert(b.end(),{1,128,2,128,3,128});
            const auto slot=capture(packet(b),peer);
            fake.put(roy_data+380+80*peer,slot);
        }
        fake.set(roy_data+112,peers);planes[0]->peer_chips=chips;
        DebugMemory memory([&](const auto&s){return fake.command(s);});
        auto io=fake.io(memory);if(!batched)io.command={};
        RadioLoop loop(io,*planes[0],{[]{},[](const Bytes&){},[](uint32_t,bool){},[]{},[](const IterationLatency&){}},true,false);
        loop.step();check(decoded==peers*(mixed?1:3)&&sensors==peers*(mixed?1:4));
    }
    {
        auto slot=capture(packet(Bytes{0xc8,0,0}),3);check(expand_alpha_slot(slot.data(),chips,true).dma.empty());
        slot[3]=uint8_t(0x80|71);check(expand_alpha_slot(slot.data(),chips,true).dma.empty());
        for(unsigned s1=0;s1<4;++s1) {
            auto d=packet(Bytes{0xc8,0,0});d[1]=uint8_t(s1);slot=capture(d);
            check(expand_alpha_slot(slot.data(),chips,false).dma==d);
        }
    }
    for(unsigned flag:{alpha_ring_single,alpha_ring_multi}){check(imu_ring_depth_for(flag)==14);check(imu_ring_stride_for(flag)==80);}
    uint64_t records=0,samples=0;
    for(int file=1;file<argc;++file) {
        std::ifstream in(argv[file],std::ios::binary);check(bool(in));int64_t time;Bytes raw(112);
        while(in.read(reinterpret_cast<char*>(&time),8)&&in.read(reinterpret_cast<char*>(raw.data()),112)) {
            const auto r=parse_record(raw,true);if(r.channel!=2||r.dma.size()<3)continue;
            auto slot=capture(r.dma);if(slot.empty())continue;
            const auto replay=expand_alpha_slot(slot.data(),chips,false);if(replay.dma!=r.dma){std::cerr<<"replay mismatch file="<<argv[file]<<" record="<<records<<" s1="<<unsigned(r.dma[1])<<" len="<<r.dma.size()<<"\n";}check(replay.dma==r.dma);++records;
            for(const auto& c:decode_records(Bytes(r.dma.begin()+2,r.dma.end())))if(c.command==2) {
                const auto a=decode_sensors(c.body,SensorFormat::Diagnostic7),b=decode_sensors(c.body,SensorFormat::Stock189);
                check(a.mag==b.mag&&a.imu.size()==b.imu.size());
                for(size_t i=0;i<a.imu.size();++i){check(pti1(r.chip,a.imu[i],uint64_t(time))==pti1(r.chip,b.imu[i],uint64_t(time)));++samples;}
            }
        }
    }
    if(argc==1)std::cout<<"NOT RUN: real rev7 replay (supply radio.bin paths)\n";
    std::cout<<"alpha_session PASS "<<checks<<" real_records="<<records<<" real_PTI1_samples="<<samples<<"\n";
    return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<"\n";return 1;}}
