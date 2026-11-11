#include "peer_engine.hpp"
#include "sensors.hpp"
#include <cstdlib>
#include <iostream>
using namespace pico;
static unsigned checks=0;
static void check(bool b) { ++checks;if(!b)throw std::runtime_error("mag check "+std::to_string(checks)); }
static void option(const char* v) {
#ifdef _WIN32
    _putenv_s("PICO_MAG_WORKMODE",v);
#else
    if(*v)setenv("PICO_MAG_WORKMODE",v,1);else unsetenv("PICO_MAG_WORKMODE");
#endif
}
static Bytes packet(uint8_t command,const Bytes& body) {
    Bytes b{0,command,uint8_t(body.size())};b.insert(b.end(),body.begin(),body.end());
    unsigned sum=0;for(auto v:b)sum+=v;
    b.push_back(uint8_t(sum));return b;
}
struct FakeTracker {
    double t=10; uint8_t expected=0; unsigned applied=0;
    PeerEngine e;
    FakeTracker(int slot=0):e({1,2,3,uint8_t(slot+1)},slot,[this]{return t;},[]{return Bytes(16,1);}) {
        e.firmware_revision=12;e.mode_profile="frame";e.mode_confirmed_at=t;
        e.continuity_lease=true;e.phase_verified=true;e.state=PeerState::Streaming;
        e.sensor_due=10000;e.last_reply=t;
    }
    void tick(double dt=0) {t+=dt;e.last_reply=t;e.advance();}
    void apply(bool ack=true) {
        const auto m=e.mailbox();const auto commands=decode_records(m.payload);check(commands.size()==1);
        const auto& c=commands.front();
        if(c.command==17) {check(c.sequence()==expected);expected=uint8_t((expected+1)&63);++applied;}
        if(ack)check(e.receive({0},2,uint16_t(m.generation),true));
    }
    void sample() {
        // Alpha layout stays revision-12 at WorkMode 09; reduced cadence is in sensor ticks.
        Bytes body{0xc9,0x34,0x12};body.resize(19);body[15]=100;body[18]=1;
        const auto b=decode_sensors(body,SensorFormat::Alpha12);check(b.imu.size()==1&&b.imu[0].temperature==0x1234);
        e.receive(packet(2,body),2,0,true);
    }
    void start() { e.begin(19,{0});apply();sample(); }
};
int main() {try {
    for(const char* disabled:{"","0","true"}) {
        option(disabled);FakeTracker f;f.start();f.tick();check(!f.e.pending);check(f.applied==0);
    }
    option("1");
    {
        // WorkMode 09 changes ODR, not the Alpha envelope. Exercise the real
        // data plane at 40 ms then 5 ms cadence, including temperature and MAG.
        unsigned decoded=0, sent=0; int64_t ns=10000000000LL;
        Hooks h;h.now=[&]{return ns;};h.decoded=[&](const Record&,const Sample& s){
            check(s.temperature==0x1234);++decoded;
        };h.send=[&](const auto&){++sent;return true;};
        DataPlane plane(h);plane.chip=1;plane.enrolled=true;plane.sensor_format=SensorFormat::Alpha12;
        plane.clock.fit=Fit{0,double(ns),15.625,0};
        uint32_t ticks=0;
        for(unsigned i=0;i<4;++i) {
            const unsigned dt=i<2?40000:5000;ticks+=dt;ns+=int64_t(dt)*1000;
            Bytes body{0xcd,0x34,0x12};body.resize(19);
            body[15]=uint8_t(ticks);body[16]=uint8_t(ticks>>8);body[17]=uint8_t(ticks>>16);body[18]=uint8_t(i+1);
            body.insert(body.end(),{1,128,2,128,3,128});
            auto payload=packet(2,body);Record r;r.chip=1;r.channel=2;r.hardware_crc=true;r.profile=0;r.cycles=ticks*64;
            r.dma={uint8_t(payload.size()),2};r.dma.insert(r.dma.end(),payload.begin(),payload.end());
            plane.record(r);
        }
        check(decoded==4&&sent==4);
    }
    {
        FakeTracker f;f.sample();f.tick();check(!f.e.pending); // command19 ACK required
        f.e.begin(19,{0});f.e.mailbox();f.sample();f.tick();check(f.e.pending->command==19);
        f.apply();check(f.e.pending&&f.e.pending->command==17&&f.e.pending->body==Bytes{9});
        const auto first=f.e.mailbox();check(f.e.pending->sequence==0);
        check(!f.e.receive({0},2,uint16_t(first.generation+20),true));
        check(!f.e.receive({0},2,uint16_t(first.generation),false));
        f.tick(.1);check(f.e.mailbox().payload==first.payload);check(f.e.pending->retries==1);
        f.apply();check(!f.e.pending);f.tick(.99);check(!f.e.pending);
        f.tick(.02);check(f.e.pending&&f.e.pending->body==Bytes{7}&&f.e.pending->sequence==1);
        f.sample();f.apply();check(f.applied==2&&f.e.sequences[17]==2);
        for(unsigned i=0;i<100;++i)f.tick(.1);
        check(!f.e.pending&&f.applied==2);
        f.e.disconnect();f.e.state=PeerState::Streaming;f.e.mode_profile="frame";f.e.mode_confirmed_at=f.t;
        f.start();f.tick();check(!f.e.pending); // same session reconnect cannot repeat
    }
    for(bool second:{false,true}) {
        FakeTracker f;f.start();f.tick();check(f.e.pending->body==Bytes{9});
        if(second){f.apply();f.tick(1.01);check(f.e.pending->body==Bytes{7});}
        f.tick(3.01);check(!f.e.pending&&f.e.state==PeerState::Streaming);
        f.tick(1);check(!f.e.pending); // timeout logs, does not disconnect or retry the episode
    }
    {
        FakeTracker f;f.e.mode_profile="stock";f.start();f.tick();check(!f.e.pending);
    }
    {
        FakeTracker a(0),b(1);a.start();b.start();a.tick();b.tick();
        check(a.e.pending->body==Bytes{9}&&b.e.pending->body==Bytes{9});
        a.apply();a.tick(1.01);a.apply();b.apply();b.tick(1.01);b.apply();
        check(a.applied==2&&b.applied==2);
    }
    {
        FakeTracker f;f.expected=63;f.e.sequences[17]=63;f.start();f.tick();f.apply();
        f.tick(1.01);check(f.e.pending->sequence==0);f.apply();check(f.e.sequences[17]==1);
    }
    {
        FakeTracker f;f.e.begin(17,{7});f.apply(); // normal initial configuration owns sequence 0
        f.e.begin(19,{0});f.apply();f.tick();check(!f.e.pending); // no IMU yet
        f.sample();f.e.queued.push_back({25,{2}});f.tick();check(f.e.pending->command==25);
        f.apply();check(f.e.pending->command==17&&f.e.pending->body==Bytes{9}&&f.e.pending->sequence==1);
        f.apply();f.tick(1.01);check(f.e.pending->sequence==2);f.apply();
    }
    option("");std::cout<<"mag_workmode PASS "<<checks<<"\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 1;} }
