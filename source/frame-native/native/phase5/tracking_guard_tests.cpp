#include "../native_test_runtime.hpp"
#include "tracking_guard.hpp"
#include "orchestrator.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
using namespace phase5;
namespace {
int checks=0;
void check(bool x){++checks;if(!x)throw std::runtime_error("tracking guard check "+std::to_string(checks));}
template<class F>void reports(F f){std::ostringstream log;auto* previous=std::cerr.rdbuf(log.rdbuf());try{f();}catch(...){std::cerr.rdbuf(previous);throw;}std::cerr.rdbuf(previous);check(!log.str().empty());}
std::string sample(int64_t stamp,bool hmd=true,bool controller=false){return "{\"t_ns\":"+std::to_string(stamp)+",\"devices\":[{\"index\":0,\"kind\":1,\"connected\":true,\"valid\":"+(hmd?"true":"false")+",\"result\":200,\"matrix\":[1,0,0,0,0,1,0,0,0,0,1,0]}"+(controller?",{\"index\":1,\"kind\":2,\"connected\":true,\"valid\":false,\"result\":0,\"matrix\":[1,0,0,0,0,1,0,0,0,0,1,0]}":"")+"]}";}
}
int main(){try{
    BenchTrackingGuard g;
    reports([&]{g.check(0,false,true);});reports([&]{g.check(0,true,true);});
    g.ingest(sample(1000000000));check(g.check(1000000000,true,true));
    check(g.check(1249999999,true,true));reports([&]{g.check(1250000000,true,true);});
    check(!g.check(1250000000,true));check(!g.check(1750000000,true));reports([&]{g.check(1750000001,true);});
    reports([&]{g.ingest(sample(1000000000));});reports([&]{g.ingest(sample(999999999));});
    BenchTrackingGuard c;c.ingest(sample(2000000000,true,true));reports([&]{c.check(2000000000,true,true);});
    BenchTrackingGuard h;h.ingest(sample(1,false));reports([&]{h.check(1,true,true);});
    for(auto bad:{"{}","{\"t_ns\":true,\"devices\":[]}","{\"t_ns\":1.0,\"devices\":[]}","{\"t_ns\":1,\"devices\":false}","{\"t_ns\":1,\"devices\":[{}]}"}){
        BenchTrackingGuard b;reports([&]{b.ingest(bad);});
    }
    BenchTrackingGuard b;reports([&]{b.ingest(std::string(65537,' '));});
    auto v=parse(sample(1));v.object["devices"].array[0].object["connected"]=parse("1");
    reports([&]{BenchTrackingGuard x;x.ingest(encode(v));});
    // Drain-to-latest adapters cannot erase an intermediate unsafe bench row.
    {BenchTrackingGuard x;x.ingest(sample(1,true,true));x.ingest(sample(2));check(x.check(2,true,true));}
    {BenchTrackingGuard x;x.ingest(sample(1,false));x.ingest(sample(2));check(x.check(2,true,true));}
    for(int fault=0;fault<6;++fault){auto row=parse(sample(1));auto& d=row.object["devices"].array[0];
        if(fault==0)d.object["index"]=parse("1");
        if(fault==1)d.object["kind"]=parse("0");
        if(fault==2)d.object["kind"]=parse("99");
        if(fault==3){auto extra=d;extra.object["index"]=parse("1");row.object["devices"].array.push_back(extra);}
        if(fault==4)d.object["result"]=parse("201");
        if(fault==5)row.object["devices"].array.clear();
        BenchTrackingGuard x;x.ingest(encode(row));reports([&]{x.check(1,true,true);});
    }
    auto joint=[](int64_t stamp,bool valid=true,int replacement=2,double motion=0.){
        auto row=parse(sample(stamp));
        for(int index:{1,replacement}){
            auto d=row.object["devices"].array[0];d.object["kind"]=parse("2");d.object["index"]=parse(std::to_string(index));
            d.object["valid"]=parse(valid?"true":"false");d.object["matrix"].array[3]=parse(std::to_string(motion));
            row.object["devices"].array.push_back(d);
        }return encode(row);
    };
    CoexistenceTrackingGuard j;j.ingest(joint(1000000000));check(j.check(1000000000,true,true));
    j.ingest(joint(1150000000));check(j.check(1150000000,true,true)); // exactly 150ms allowed
    j.ingest(joint(1150000001));reports([&]{j.check(1150000001,true);});
    j.ingest(joint(1150000002,true,2,.001));check(j.check(1150000002,true,true));
    reports([&]{j.check(1150000002,false);});reports([&]{j.check(1150000001,true);});
    CoexistenceTrackingGuard lost;lost.ingest(joint(1));check(lost.check(1,true,true));
    lost.ingest(joint(2,false));check(!lost.check(2,true));reports([&]{lost.check(2,true,true);});
    check(!lost.check(500000002,true));reports([&]{lost.check(500000003,true);});
    CoexistenceTrackingGuard replacement;replacement.ingest(joint(1));check(replacement.check(1,true,true));
    replacement.ingest(joint(2,true,3,.01));reports([&]{replacement.check(2,true,true);});
    CoexistenceTrackingGuard stale;stale.ingest(joint(1));reports([&]{stale.check(250000001,true,true);});
    v=parse(sample(1));v.object["devices"].array.push_back(v.object["devices"].array[0]);
    reports([&]{BenchTrackingGuard x;x.ingest(encode(v));});
    std::cout<<"tracking guard "<<checks<<" checks PASS\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
