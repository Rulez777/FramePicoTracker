#include "xrtap_config.hpp"
#include <iostream>
#ifndef _WIN32
#include <cstdio>
#include <cstdlib>
#include <future>
using namespace pico;
namespace {
int checks=0;
void check(bool b,int line=__builtin_LINE()){++checks;if(!b)throw std::runtime_error("xrtap check "+std::to_string(checks)+" line "+std::to_string(line));}
template<class F>void await_at(int line,F f){
    auto end=XrtapSource::now()+2000000000LL;
    while(!f()){
        if(XrtapSource::now()>end)throw std::runtime_error("xrtap fixture timeout at line "+std::to_string(line));
        std::this_thread::sleep_for(std::chrono::microseconds(500));
    }
}
#define await(f) await_at(__LINE__,f)
// Real shared mapping and writer protocol, entirely synthetic pixels/times.
struct Writer {
    char path[64]="/tmp/pico-xrtap-XXXXXX";
    int fd=-1;
    xrtap_shm* shm=nullptr;
    Writer(){
        fd=mkstemp(path);
        if(fd<0||ftruncate(fd,sizeof(xrtap_shm)))throw std::runtime_error("fixture file");
        auto p=mmap(nullptr,sizeof(xrtap_shm),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
        if(p==MAP_FAILED)throw std::runtime_error("fixture mmap");
        shm=static_cast<xrtap_shm*>(p);
        shm->magic=XRTAP_MAGIC;shm->version=XRTAP_VERSION;
        const char* devices[]={"/dev/video0","/dev/video3","/dev/video9","/dev/video13"};
        for(unsigned n=0;n<4;++n)std::strcpy(shm->nodes[n].device,devices[n]);
        __atomic_store_n(&shm->nodes_used,4u,__ATOMIC_RELEASE);
    }
    ~Writer(){if(shm)munmap(shm,sizeof(xrtap_shm));
    if(fd>=0)close(fd);
    unlink(path);}
    void frame(unsigned n,int64_t ts,uint8_t value,bool torn=false){
        auto& node=shm->nodes[n];auto number=__atomic_load_n(&node.write_seq,__ATOMIC_ACQUIRE);
        auto& s=node.slots[number%XRTAP_SLOTS];
        __atomic_store_n(&s.seq,s.seq+1,__ATOMIC_RELEASE);
        __atomic_thread_fence(__ATOMIC_SEQ_CST);
        s.frame_number=number;s.v4l2_timestamp_ns=ts;
        s.width=n<2?1056:640;s.height=n<2?1024:480;s.stride=s.width;s.bytes=s.width*s.height;
        std::memset(s.pixels,value,s.bytes);
        __atomic_thread_fence(__ATOMIC_SEQ_CST);
        if(!torn)__atomic_store_n(&s.seq,s.seq+1,__ATOMIC_RELEASE);
        __atomic_store_n(&node.write_seq,number+1,__ATOMIC_RELEASE);
    }
    void pair(int64_t ts,uint8_t value){frame(0,ts,value);frame(1,ts+80000,value);}
    // Cadence classification: a normal pair 8.4 ms before makes the pair at ts the short IR one.
    void ir_pair(int64_t ts,uint8_t value){pair(ts-8400000,100);pair(ts,value);}
};
void capture(){
    Writer writer;XrtapConfig config;config.path=writer.path;config.lower_enabled=true;config.offset_ns=123000;
    XrtapSource source(config);
    await([&]{return __atomic_load_n(&writer.shm->reader_heartbeat_ns,__ATOMIC_ACQUIRE)>0;});
    auto heartbeat=__atomic_load_n(&writer.shm->reader_heartbeat_ns,__ATOMIC_ACQUIRE);
    await([&]{return __atomic_load_n(&writer.shm->reader_heartbeat_ns,__ATOMIC_ACQUIRE)>heartbeat;});
    int64_t t=10000000000LL;
    for(int i=0;i<8;++i){
        writer.pair(t,100);t+=8500000;
        writer.pair(t,2);t+=25000000;
        await([&]{return source.counts().pairs==uint64_t(i+1);});
    }
    auto pairs=source.take_pairs();check(pairs.size()==2);
    check(source.counts().dropped==6);check(source.counts().non_ir==16);
    check(pairs[0].frames[0].decode_index==13&&pairs[1].frames[0].decode_index==15);
    auto& frame=pairs.back().frames[0];
    check(frame.source_time_verified&&frame.pixels.size()==1056u*1024u&&frame.pixels[0]==2);
    check(std::abs(*frame.metadata.timestamp-double(t-25000000+123000)*1e-9)<1e-10);
    check(std::abs(*pairs.back().frames[1].metadata.timestamp-*frame.metadata.timestamp-.000080)<1e-10);
    check(std::abs(*frame.metadata.exposure-782.267e-6)<1e-12);
    check(*frame.metadata.sensor=="CAMUPPERLEFT");check(source.take_pairs().empty());
    // Only the cadence (short gap after the previous frame of the camera) marks the IR frame.
    writer.ir_pair(t+16000000,1);await([&]{return source.counts().pairs==9;});
    (void)source.take_pairs();
    // Normal frames >=15 ms after the last IR frame, then short frames whose stereo skew exceeds 1 ms.
    writer.frame(0,t+40000000,100);writer.frame(1,t+40000000,100);writer.frame(0,t+48400000,1);writer.frame(1,t+48400000+1001000,1);
    await([&]{return source.counts().unpaired>0;});check(source.counts().pairs==9);
    writer.ir_pair(t+100000000,1);await([&]{return source.counts().pairs==10;});
    (void)source.take_pairs();
    writer.frame(0,t+133333333,1,true);writer.frame(1,t+133333333,1,true);
    await([&]{return source.counts().torn==2;});check(source.take_pairs().empty());
    // Published even seqlock with an overwritten frame identity is also rejected.
    auto& node=writer.shm->nodes[0];
    auto next=__atomic_load_n(&node.write_seq,__ATOMIC_ACQUIRE);
    auto& overwritten=node.slots[next%XRTAP_SLOTS];
    __atomic_store_n(&overwritten.seq,2ULL,__ATOMIC_RELEASE);
    overwritten.frame_number=next+100;overwritten.width=1056;overwritten.height=1024;
    overwritten.stride=1056;overwritten.bytes=1056u*1024u;
    __atomic_store_n(&node.write_seq,next+1,__ATOMIC_RELEASE);
    await([&]{return source.counts().torn==3;});check(source.take_pairs().empty());
    // A first lower frame per node has no cadence yet: classified normal, never copied or paired.
    writer.frame(2,t,42);writer.frame(3,t,43);
    await([&]{return source.counts().lower_non_ir==2;});
    check(source.counts().lower_pairs==0&&source.counts().pairs==10);
}
// Lower nodes: same cadence rule, own pairing queue, own offset and node->serial mapping.
struct LowerWriter : Writer {
    void ir(int64_t ts,uint8_t a,uint8_t b,int64_t skew=50000){
        frame(2,ts-8400000,100);frame(3,ts-8400000+skew,100);frame(2,ts,a);frame(3,ts+skew,b);
    }
};
void lower(){
    LowerWriter writer;XrtapConfig config;config.path=writer.path;config.lower_enabled=true;config.lower_offset_ns=-2000000;config.lower_wait_ns=0;
    XrtapSource source(config);
    const int64_t t=10000000000LL;
    auto stamp=[](int64_t raw){return double(raw-2000000)*1e-9;};
    writer.ir(t,42,43);
    await([&]{return source.counts().lower_pairs==1;});
    auto c=source.counts();
    check(c.lower_non_ir==2&&c.non_ir==0&&c.pairs==0&&c.lower_unpaired==0);
    auto pair=source.lower_for(stamp(t));
    check(pair.has_value());
    check(pair->frames[0].pixels.size()==640u*480u&&pair->frames[0].pixels[0]==42&&pair->frames[1].pixels[0]==43);
    check(*pair->frames[0].metadata.sensor=="CAMLOWERRIGHT"&&*pair->frames[1].metadata.sensor=="CAMLOWERLEFT");
    check(std::abs(*pair->frames[0].metadata.timestamp-stamp(t))<1e-10);
    check(std::abs(*pair->frames[1].metadata.timestamp-stamp(t+50000))<1e-10);
    check(pair->frames[0].source_time_verified&&std::abs(*pair->frames[0].metadata.exposure-782.267e-6)<1e-12);
    // Taken once; a second request for the same exposure is a counted miss.
    check(!source.lower_for(stamp(t)));
    c=source.counts();check(c.lower_matched==1&&c.lower_missed==1);
    // Association bound with the upper exposure: 4 ms.
    writer.ir(t+33333333,1,1);await([&]{return source.counts().lower_pairs==2;});
    check(!source.lower_for(stamp(t+33333333)+.004001));
    check(source.lower_for(stamp(t+33333333)+.003999).has_value());
    // Two nodes more than 1 ms apart never pair.
    writer.ir(t+66666666,1,1,1001000);await([&]{return source.counts().lower_unpaired>0;});
    check(source.counts().lower_pairs==2);
    // Nearest wins; it and every older retained pair leave the queue.
    writer.ir(t+100000000,5,5);writer.ir(t+133333333,6,6);
    await([&]{return source.counts().lower_pairs==4;});
    pair=source.lower_for(stamp(t+133333333));check(pair&&pair->frames[0].pixels[0]==6);
    check(!source.lower_for(stamp(t+100000000)));
    // Upper pairing is unaffected by lower traffic, and lower frames never enter the upper queue.
    writer.ir_pair(t+150000000,9);await([&]{return source.counts().pairs==1;});
    auto upper=source.take_pairs();check(upper.size()==1&&upper[0].frames[0].pixels.size()==1056u*1024u);
}
void lower_wait(){
    LowerWriter writer;XrtapConfig config;config.path=writer.path;config.lower_enabled=true;config.lower_wait_ns=500000000;
    XrtapSource source(config);
    const int64_t t=10000000000LL;
    auto stamp=[&](int64_t raw){return double(raw+config.lower_offset_ns)*1e-9;};
    // No live lower stream yet: no wait at all.
    auto start=XrtapSource::now();check(!source.lower_for(stamp(t)));check(XrtapSource::now()-start<250000000);
    writer.ir(t,1,1);await([&]{return source.counts().lower_pairs==1;});
    // Live stream: a late pair of the requested exposure is awaited and delivered.
    auto late=std::async(std::launch::async,[&]{return source.lower_for(stamp(t+33333333));});
    check(late.wait_for(std::chrono::milliseconds(20))==std::future_status::timeout);
    writer.ir(t+33333333,7,7);
    check(late.wait_for(std::chrono::seconds(2))==std::future_status::ready);
    auto got=late.get();check(got&&got->frames[0].pixels[0]==7);
    // A newer pair already queued means the requested one will never come: no wait.
    writer.ir(t+100000000,8,8);await([&]{return source.counts().lower_pairs==3;});
    start=XrtapSource::now();check(!source.lower_for(stamp(t+66666666)));check(XrtapSource::now()-start<250000000);
    // Stop wins over a pending wait.
    auto stopped=std::async(std::launch::async,[&]{return source.lower_for(stamp(t+200000000));});
    std::this_thread::sleep_for(std::chrono::milliseconds(20));source.stop_consumer();
    check(stopped.wait_for(std::chrono::milliseconds(250))==std::future_status::ready&&!stopped.get());
}
void lower_config(){
    // Swapped mapping, disabled lower path, and configuration parsing.
    {
        LowerWriter writer;XrtapConfig config;config.path=writer.path;config.lower_enabled=true;config.lower_wait_ns=0;
        std::swap(config.lower_cameras[0].second,config.lower_cameras[1].second);
        XrtapSource source(config);writer.ir(10000000000LL,1,2);
        await([&]{return source.counts().lower_pairs==1;});
        auto pair=source.lower_for(double(10000000000LL+config.lower_offset_ns)*1e-9);
        check(pair&&*pair->frames[0].metadata.sensor=="CAMLOWERLEFT"&&pair->frames[0].pixels[0]==1);
    }
    {
        LowerWriter writer;XrtapConfig config;config.path=writer.path;config.lower_enabled=false;
        XrtapSource source(config);writer.ir(10000000000LL,1,2);writer.ir_pair(10000000000LL,3);
        await([&]{return source.counts().pairs==1;});
        check(source.counts().lower_non_ir==0&&source.counts().lower_pairs==0);
        check(!source.lower_for(double(10000000000LL+config.lower_offset_ns)*1e-9));
    }
    auto parsed=xrtap_config(phase5::Json("{}").parse());
    check(!parsed.lower_enabled&&parsed.lower_mapping_verified&&parsed.lower_offset_ns==-10137000);  // 2026-10-31: off, content-verified map
    check(parsed.lower_cameras[0].first=="/dev/video9"&&parsed.lower_cameras[0].second=="CAMLOWERRIGHT");
    check(parsed.lower_cameras[1].first=="/dev/video13"&&parsed.lower_cameras[1].second=="CAMLOWERLEFT");
    check(xrtap_config(phase5::Json(R"({"xrtap_lower":true})").parse()).lower_enabled);
    parsed=xrtap_config(phase5::Json(R"({"xrtap_lower_cameras":{"/dev/video9":"CAMLOWERRIGHT","/dev/video13":"CAMLOWERLEFT"},)"
        R"("xrtap_lower_offset_ns":-1000,"xrtap_lower_mapping_verified":true,"xrtap_lower_upper_ns":2000000})").parse());
    for(const auto& entry:parsed.lower_cameras)
        check((entry.first=="/dev/video9"&&entry.second=="CAMLOWERRIGHT")||(entry.first=="/dev/video13"&&entry.second=="CAMLOWERLEFT"));
    check(parsed.lower_offset_ns==-1000&&parsed.lower_mapping_verified&&parsed.lower_upper_ns==2000000);
    check(!xrtap_config(phase5::Json(R"({"xrtap_lower":false})").parse()).lower_enabled);
    for(const char* bad:{R"({"xrtap_lower":true,"xrtap_lower_cameras":{"/dev/video9":"CAMLOWERRIGHT","/dev/video13":"CAMLOWERRIGHT"}})",
                         R"({"xrtap_lower":true,"xrtap_lower_cameras":{"/dev/video9":"CAMUPPERLEFT","/dev/video13":"CAMLOWERRIGHT"}})",
                         R"({"xrtap_lower":true,"xrtap_lower_cameras":{"/dev/video9":"CAMLOWERRIGHT"}})",
                         R"({"xrtap_lower":true,"xrtap_lower_upper_ns":8000000})",R"({"xrtap_lower":true,"xrtap_lower_wait_ns":-1})"}) {
        bool rejected=false;try{(void)xrtap_config(phase5::Json(bad).parse());}catch(const std::runtime_error&){rejected=true;}
        check(rejected);
    }
}
void layouts(){
    Writer writer;XrtapConfig config;config.path=writer.path;
    writer.shm->magic=0;
    bool bad=false;try{XrtapSource source(config);}catch(const std::runtime_error&){bad=true;}check(bad);
    writer.shm->magic=XRTAP_MAGIC;writer.shm->version=99;
    bad=false;try{XrtapSource source(config);}catch(const std::runtime_error&){bad=true;}check(bad);
    writer.shm->version=1;bad=false;try{XrtapSource source(config);}catch(const std::runtime_error&){bad=true;}check(bad);  // v1 layout differs
    writer.shm->version=XRTAP_VERSION;check(ftruncate(writer.fd,offsetof(xrtap_shm,event_seq))==0);
    std::swap(config.cameras[0].second,config.cameras[1].second);
    XrtapSource source(config);writer.ir_pair(10000000000LL,0);
    await([&]{return source.counts().pairs==1;});
    auto pairs=source.take_pairs();check(*pairs[0].frames[0].metadata.sensor=="CAMUPPERLEFT");
    check(*pairs[0].frames[0].metadata.timestamp>*pairs[0].frames[1].metadata.timestamp);
}
void notification(){
    Writer writer;XrtapConfig config;config.path=writer.path;
    XrtapSource source(config);
    auto waiting=std::async(std::launch::async,[&]{return source.wait_pairs();});
    check(waiting.wait_for(std::chrono::milliseconds(20))==std::future_status::timeout);
    writer.ir_pair(10000000000LL,7);
    const bool notified=waiting.wait_for(std::chrono::seconds(2))==std::future_status::ready;
    if(!notified)source.stop_consumer();
    auto pairs=waiting.get();check(notified&&pairs.size()==1);
    check(pairs.front().frames[0].pixels.front()==7);
    // A pair queued before wait must not lose its notification.
    writer.ir_pair(10033333333LL,8);
    await([&]{return source.counts().pairs==2;});
    check(source.wait_pairs().size()==1);
    // Simulate a camera-rate consumer while no radio poll runs at all.
    std::atomic<unsigned> consumed{0};
    auto consumer=std::async(std::launch::async,[&]{
        unsigned n=0;while(true){auto batch=source.wait_pairs();if(batch.empty())return n;
        n+=unsigned(batch.size());consumed=n;}
    });
    struct StopConsumer { XrtapSource& source; ~StopConsumer(){source.stop_consumer();} } stop{source};
    for(unsigned i=0;i<12;++i){
        writer.ir_pair(10066666666LL+i*33333333LL,9);
        await([&]{return source.counts().pairs==3+i;});
        await([&]{return consumed==i+1;});
    }
    // Empty wait is explicitly woken by cleanup, with no camera deadline.
    source.stop_consumer();
    check(consumer.wait_for(std::chrono::seconds(2))==std::future_status::ready);
    check(consumer.get()==12);
    check(source.wait_pairs().empty());source.stop_consumer();
    // Stop wins over backlog as well.
    Writer queued;config.path=queued.path;XrtapSource stopped(config);
    queued.ir_pair(10000000000LL,1);await([&]{return stopped.counts().pairs==1;});
    stopped.stop_consumer();check(stopped.wait_pairs().empty());
}
void preflight(){
    auto optics=phase5::Json("{}").parse();check(use_xrtap(optics));
    optics=phase5::Json(R"({"camera_source":"recording"})").parse();check(!use_xrtap(optics));
    Writer writer;XrtapConfig config;config.path=writer.path;
    XrtapSource source(config);int written=0;auto last=XrtapSource::now()-20000000;
    xrtap_preflight(source,[&]{
        if(written<6&&XrtapSource::now()-last>=20000000){
            writer.ir_pair(10000000000LL+written*33333333LL,0);
            last=XrtapSource::now();++written;
        }
    });
    check(written==6&&source.counts().pairs==6);
    // A bright-only source never passes acquisition: the wait is logged and continues (heartbeat still
    // runs) until the owner stops it through guard().
    struct OwnerStop{};
    Writer bright;config.path=bright.path;XrtapSource waiting(config);
    std::vector<std::string> lines;bool stopped=false;
    try{xrtap_preflight(waiting,[&]{
            if(!bright.shm->nodes[0].write_seq){bright.pair(10000000000LL,100);bright.pair(10033333333LL,100);}
            if(lines.size()>=3)throw OwnerStop{};
        },[&](const std::string& line){lines.push_back(line);},200000000,100000000);}
    catch(const OwnerStop&){stopped=true;}
    check(stopped&&lines.size()==3&&waiting.counts().pairs==0);
    check(lines[0].find("no matched IR stereo pair")!=std::string::npos&&lines[0].find("still waiting")!=std::string::npos);
    check(XrtapSource::now()-__atomic_load_n(&bright.shm->reader_heartbeat_ns,__ATOMIC_ACQUIRE)<100000000);
    // One pair, then fewer than six within the stream window: logged, and the wait completes once six arrive.
    Writer slow;config.path=slow.path;XrtapSource late(config);
    lines.clear();int sent=0;auto previous=XrtapSource::now()-20000000;
    xrtap_preflight(late,[&]{
        if((sent==0||(!lines.empty()&&sent<6))&&XrtapSource::now()-previous>=20000000){
            slow.ir_pair(10000000000LL+sent*33333333LL,0);previous=XrtapSource::now();++sent;
        }
    },[&](const std::string& line){lines.push_back(line);},2000000000,100000000);
    check(sent==6&&late.counts().pairs==6&&!lines.empty()&&lines[0].find("fewer than six")!=std::string::npos);
}
}
int main(){try{capture();lower();lower_wait();lower_config();layouts();notification();preflight();std::cout<<checks<<" xrtap checks PASS (synthetic shared memory)\n";return 0;}
catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
#else
int main(){std::cout<<"xrtap shared-memory fixture requires Linux\n";}
#endif
