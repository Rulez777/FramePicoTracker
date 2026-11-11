#include <memory>
#include <fstream>
#include <filesystem>
#include "producer_replay.hpp"
#include "radio_loop.hpp"
#include "optical_worker.hpp"
#include "live_session.hpp"
#include "live_session_owner.hpp"
#include "live_policy.hpp"
#include "phase5/orchestrator.hpp"
#include "listener_attribution.hpp"
#include "synthetic_tcl_server.hpp"
#include "imu_slot.hpp"
#include "peer_table.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <limits>
#ifndef _WIN32
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#endif
using namespace pico;
namespace {
size_t checks=0;
void check_at(bool b,int line) { ++checks; if(!b) throw std::runtime_error("worker check "+std::to_string(checks)+" line "+std::to_string(line)); }
#define check(...) check_at((__VA_ARGS__),__LINE__)
template<class F> void rejects(F f) { bool bad=false; try { f(); } catch(const std::invalid_argument &) { bad=true; } check(bad); }
constexpr uint32_t chip=0xA1B2C3D4;
constexpr int64_t base=3000000000LL, period=33333333;
// Explicit synthetic ABI doubles; no claim of Windows .so execution.
int project(const OpticalCamera *c,const double *p,size_t n,double *out) {
    for(size_t i=0;i<n;++i) { if(p[3*i+2]<=0) return 1;
        out[2*i]=c->cx+c->fx*p[3*i]/p[3*i+2]; out[2*i+1]=c->cy+c->fy*p[3*i+1]/p[3*i+2]; }
    return 0;
}
int detect(const uint8_t *image,int h,int w,int threshold,int min_area,int max_area,double *out,int cap) {
    if (!image[0]) return 0;
    check(h==1024 && w==1056 && threshold==180 && min_area==2 && max_area==250 && cap>=4 && image);
    check(image[700*w+1040]==17 && image[1040*1024+700]==0);
    for(int i=0;i<4;++i) { out[5*i]=520+i*5; out[5*i+1]=500+i*3; out[5*i+2]=1; out[5*i+3]=255; out[5*i+4]=2; }
    return 4;
}
int fit_mutation=0;
double world_shift=0;
std::function<void()> after_solve;
int solve(const double *model,const OpticalView *views,const double *prior,OpticalFit *fit) {
    // 2026-10-31 multi copy: optical_worker.cpp:126 (2026-10-30) seeds the solver with the last accepted pose, so
    // a prior is legitimate here; the old !prior assertion failed every dry run of the installed tree since then.
    (void)prior;
    check(model && views[0].count==4 && views[1].count==4);
    check(views[0].camera.width==1056 && views[0].camera.height==1024 && views[1].camera.width==1056 && views[1].camera.height==1024 && views[1].pixels[0]==520);
    auto t=identity_rigid(); t[11]=1; t[3]=views[0].head_from_camera[3]+world_shift; std::copy(t.begin(),t.end(),fit->pose); fit->count=4;
    fit->residual=fit_mutation==1?2.50001:0;
    fit->position_std=fit_mutation==2?.030001:0;
    fit->rotation_std_deg=fit_mutation==3?5.00001:0;
    fit->condition=fit_mutation==4?10000001:0;
    if(fit_mutation==5) fit->residual=std::numeric_limits<double>::quiet_NaN();
    if(after_solve) after_solve();
    return 0;
}
NativeOpticalApi api{project,solve,detect};
LedModel model() {
    LedModel m{}; for(size_t i=0;i<8;++i) { m[3*i]=(double(i%4)-1.5)*.02; m[3*i+1]=(double(i/4)-.5)*.02; } return m;
}
std::shared_ptr<IdentityPair> pair(int64_t stamp) {
    auto p=std::make_shared<IdentityPair>(); p->timestamp_matched=true;
    for(size_t i=0;i<2;++i) {
        auto &f=p->frames[i]; f.serial=i?"CAMUPPERRIGHT":"CAMUPPERLEFT";
        f.metadata="synthetic measured metadata "+std::to_string(stamp)+std::to_string(i);
        f.timestamp_matched=true; f.stamp=stamp; f.exposure_us=800;
        f.view.camera={200,200,528,512,{0,0,0,0},1056,1024};
        f.pixels.resize(1056u*1024u);
        f.pixels[700*1056+1040]=17; // asymmetric packed-row sentinel beyond x=1023
    }
    return p;
}

struct Fixture {
    LiveSessionOwner& owner;
    int64_t ns=base; double seconds=double(base)/1e9,peer_seconds=seconds;
    SyntheticTclServer server;
    FakeOpenOcd &fake=server.backend;
    std::unique_ptr<TclSocket> socket;
    DebugMemory debug{[this](const std::string &s){return owner.debugger_command(s);}};
    MailboxWriter writer{debug};
    QueryPeer* peer=nullptr;
    QueryPeer& e(){return *peer;}
    Native25AckBridge& bridge(){return owner.query_bridge();}
    RoyClock clock;
    NativeIdentitySession& s(){return owner.identity();}

    bool child_running=false,installed=false;
    std::string fault;
    void attribution(){
        ListenerIdentity launch;
        launch.child=11;launch.child_start=3;launch.argv=openocd_argv();
        launch.executable_seal="synthetic executable";launch.dependency_seals={"synthetic interface","synthetic target"};
        auto evidence=launch;evidence.owner=11;evidence.observed_start=3;
        evidence.socket_inode=4;evidence.address="127.0.0.1";evidence.port=6666;evidence.matching_listeners=1;
        if(installed&&fault=="listener-change")++evidence.owner;
        verify_listener_identity(evidence,launch); // explicit synthetic evidence, not OS attribution
    }
    OpenOcdHooks debugger_hooks(){
        return {[this]{return fake.ns;},[this](int64_t n){fake.ns+=n;},[]{return false;},
            [this](const std::vector<std::string>& argv){check(argv==openocd_argv());server.start();child_running=true;},
            [this]()->std::optional<int>{return child_running?std::optional<int>{}:std::optional<int>{0};},
            [this]{socket=std::make_unique<TclSocket>(server.port);return true;},
            [this](const std::string& c){if(c=="shutdown"){child_running=false;return std::string{};}return socket->command(c);},
            [this]{socket.reset();server.finish();},[this]{child_running=false;},
            []{throw std::runtime_error("Unexpected synthetic debugger kill");},[this]{attribution();},[]{return int64_t(11);}};
    }
    explicit Fixture(LiveSessionOwner& session) : owner(session) {

    }

    // Explicit synthetic peer replies, delivered through the owner's canonical
    // receive path. No mode timestamp/state is pre-confirmed or refreshed here.
    void record(const Bytes& payload,uint32_t generation=0){
        Record r;r.hardware_crc=true;r.channel=2;r.profile=0;r.chip=chip;
        r.generation=generation;r.slot=++reply_slot;
        r.dma={static_cast<uint8_t>(payload.size()),2};
        r.dma.insert(r.dma.end(),payload.begin(),payload.end());
        owner.query_record(r);
    }
    uint32_t reply_slot=0;uint8_t nonce=0;
    uint8_t tracker_n=1;  // multi4: the emulated tracker's wake divider, set by command 16, reported by schema 2
    unsigned timers=0,orders=0;
    struct QueryEvent {std::string name;unsigned attempt;uint8_t sequence;Bytes nonce;double time;};
    std::vector<QueryEvent> query_events;
    Bytes packet(const Bytes& body){Bytes p{0,1,static_cast<uint8_t>(body.size())};p.insert(p.end(),body.begin(),body.end());unsigned sum=0;for(auto v:p)sum+=v;
    p.push_back(static_cast<uint8_t>(sum));return p;}
    Bytes reply(unsigned schema,const Bytes& n){
        Bytes b{0xfe,static_cast<uint8_t>(schema)};b.insert(b.end(),n.begin(),n.end());
        auto c=le32(chip);b.insert(b.end(),c.begin(),c.end());
        b.resize(schema==2?26:schema==4?25:32);
        auto set=[&](size_t off,uint32_t v){auto x=le32(v);std::copy(x.begin(),x.end(),b.begin()+off);};
        if(schema==2){b[22]=1;b[23]=PeerTable::configured().multi?12:fault=="frame-release"?11:7;b[24]=PeerTable::configured().multi?tracker_n:e().active_query()?1:2;
            if(e().active_query()&&fault=="query-native-n")b[24]=3;
            if(e().active_query()&&fault=="query-native-pending"){b[24]=2;b[25]=1;}}
        if(schema==3){b[22]=7;b[24]=1;b[26]=static_cast<uint8_t>(reply_slot);b[29]=1;}
        if(schema==4)b[22]=b[23]=b[24]=1;
        if(schema==6){b[23]=3;b[25]=static_cast<uint8_t>(timers++%2);set(26,500+timers);b[30]=4;b[31]=2;}
        if(schema==7){set(22,100);set(26,533333);}
        if(schema==8){set(22,0x40001104);set(26,0x40009048);b[30]=8;}
        if(schema==9){set(22,10+10*orders++);set(26,3);}
        if(schema==10){set(22,2);set(26,1);}
        if(schema==11){set(22,4);set(26,5);b[31]=1;}
        if(schema==12){set(22,0x42435231);set(26,10);b[30]=2;b[31]=19;}
        if(schema==13){b[22]=20;b[23]=1;set(24,4);set(28,2);}
        return b;
    }
    void startup(){
        e().start();Bytes product(35);product[0]=0x82;record(packet(product));
        for(unsigned i=0;i<400&&!(e().diagnostics_ready()&&!e().pending);++i){
            if(e().active_query()&&fault=="query-link-loss"){
                peer_seconds+=3;owner.query_poll();throw std::runtime_error("Link fault was accepted");
            }
            owner.query_poll();
            if(e().pending){auto p=*e().pending;
                if(p.command==1&&p.body.size()==18&&p.body[0]==0x7e){
                    bool missing=e().mode_nonce&&(fault=="query-recovery-exhaust"||
                        (fault=="query-recovery-success"&&(!e().initial_recovery||e().initial_recovery->attempts<3)));
                    if(!missing){
                        auto b=reply(p.body[1],Bytes(p.body.begin()+2,p.body.end()));
                        if(e().active_query()&&fault=="query-stale-nonce")b[2]^=1;
                        if(e().active_query()&&fault=="query-wrong-chip")b[18]^=1;
                        record(packet(b),p.generation);
                    }
                }
                else{if(p.command==16&&!p.body.empty())tracker_n=p.body[0];
                record(Bytes{0,0,0,0},p.generation);}
            }
            time(ns+100000000);
        }
        check(e().diagnostics_ready()&&!e().pending&&e().native_ir31_applied&&e().continuity_lease);
        if(fault=="query-recovery-success"){
            check(query_events.size()==3&&e().initial_recovery&&e().initial_recovery->completed);
            for(size_t i=1;i<query_events.size();++i){
                check(query_events[i].attempt==i+1&&query_events[i].sequence!=query_events[i-1].sequence&&
                    query_events[i].nonce!=query_events[i-1].nonce);
            }
            check(query_events.back().time-query_events.front().time<24);
        }
    }

    void time(int64_t t) {
        ns=t; seconds=double(t)/1e9;
        // Recorded exposures may arrive out of order; the emulated radio clock
        // is monotonic and emits explicit continuous link records in gaps.
        if(peer){while(peer_seconds+1<seconds){peer_seconds+=1;record(Bytes{0,0,0,0});}
            peer_seconds=(std::max)(peer_seconds,seconds);record(Bytes{0,0,0,0});}
    }
    void ack() {
        auto g=e().pending->generation; Record r; r.hardware_crc=true; r.channel=2; r.profile=0;
        r.chip=chip; r.generation=g; r.dma={4,2,0,0,0,0}; clock.fit=Fit{0,double(ns),15.625,0};
        owner.query_record(r);check(e().acked_generation==std::optional<uint16_t>(uint16_t(g)));
    }
    OpticalWorker *worker=nullptr;
    std::function<void()> session_guard;
    void frame(int i,int64_t start,int64_t offset=0) {
        if(session_guard)session_guard();
        (void)offset;
        auto stamp=start+i*period; time(stamp);
        auto p=pair(stamp);
        for(auto &f:p->frames) { f.pixels[0]=(i<4 || i>=11)?255:0; f.world_from_head[3]=.25; }
        auto raw=double(stamp)/1e9;
        check(worker!=nullptr);
        worker->exposure(*p,{CameraMetadata{raw,.0008},CameraMetadata{raw,.0008}});
        if(session_guard)session_guard();
    }    void run_episode(int64_t start,int64_t offset) {
        for(int i=0;i<3;++i) frame(i,start,offset);
        time(start+100000000); check(s().begin_if_ready(e(),"live",ns,true));
        check(bridge().publish(writer,0,e().mailbox())); ack();
        time(start+350000000); s().radio_tick(e(),"live",ns);
        check(e().pending && e().pending->body==Bytes{2});
        check(bridge().publish(writer,0,e().mailbox()));
        time(start+360000000); ack(); check(s().owner.restore_required());
        for(int i=3;i<=14;++i) frame(i,start,offset);
        check(s().owner.restore_required()); // optical cannot retire/clear owner
        time(start+14*period); s().radio_tick(e(),"live",ns);
        check(!s().owner.restore_required() && !e().native25_exclusive);
    }
};

// multi4 (2026-10-31): synthetic trackers 1..K-1 behind the real RadioLoop/ring decoder. Each emulated tracker
// answers its own pending command through a multi4 ring slot (raw slot with peer<<21, IMU slot byte 10 = peer):
// product info, schema-2 mode proof with the N it was last told (command 16), empty ACKs, then IMU samples.
// A peer-0 IMU slot and a slot of an unenrolled index ride in the same ring. Synthetic radio only.
// LED mode: the emulated tracker switches its LEDs only when it answers a 25 command (dark from configuration).
struct MultiRingTracker { bool product=false, lit=false; uint8_t n=1, counter=0; unsigned led_commands=0, mode_sets=0; uint8_t mode=0; bool independent=false; };
struct MultiRing {
    LiveSessionOwner& owner; Fixture& f;
    uint32_t w=0,packets=0;
    std::vector<MultiRingTracker> trackers;
    uint8_t primary_counter=0;
    MultiRing(LiveSessionOwner& o,Fixture& fixture):owner(o),f(fixture),trackers(o.secondary_peers().size()){
        f.fake.set(roy_data+8,alpha_ring_multi);
        f.fake.set(roy_data+multi4_peer_offset(0)+12,3);
        w=word(f.fake.read(roy_data+112,4));
        f.fake.set(roy_data+116,w);
        auto& peers=owner.secondary_peers();
        for(size_t i=0;i<peers.size();++i)f.fake.set(roy_data+multi4_peer_offset(peers.at(i).index)+12,3);
    }
    void slot(unsigned peer,const Bytes& records,uint8_t s1,uint16_t generation,bool imu){
        const auto payload=imu?record(2,records):records;
        check(payload.size()<=70);
        uint8_t p[80]={};
        auto put=[&](int o,uint32_t v){for(int i=0;i<4;++i)p[o+i]=uint8_t(v>>(8*i));};
        // Alpha's full envelope preserves all S1 values, all 3 compact samples and MAG.
        put(0,0x80000000u|(uint32_t(payload.size())<<24)|(uint32_t(s1)<<19)|((peer&3)<<21)|(++packets&0x7ffff));
        put(4,packets*1000);p[8]=uint8_t(generation);p[9]=uint8_t(generation>>8);
        std::copy(payload.begin(),payload.end(),p+10);
        f.fake.put(roy_data+380+(w%14)*80,Bytes(p,p+80));++w;f.fake.set(roy_data+112,w);
    }
    static Bytes record(uint8_t command,const Bytes& body){Bytes r{0,command,uint8_t(body.size())};r.insert(r.end(),body.begin(),body.end());return r;}
    // One radio iteration: every emulated tracker answers its own pending command (or streams IMU).
    void step(unsigned step){
        f.fake.set(roy_data+multi4_peer_offset(0)+16,step+2);
        owner.radio_step();
        auto &primary=f.e();
        const uint16_t tag=uint16_t(primary.pending?primary.pending->generation:primary.generation);
        if(primary.pending && primary.pending->command==1 && primary.pending->body[0]==0x7e) {
            const auto &request=primary.pending->body;
            auto page=f.reply(request[1],Bytes(request.begin()+2,request.begin()+18));
            if(request.size()==19) {page[22]=1;page[23]=12;page[24]=request[18];page[25]=6;}
            slot(0,record(1,page),3,tag,false);
        } else if(primary.pending) slot(0,{},2,tag,false);
        else {
            Bytes body{0xcf,0x34,0x12};
            for(unsigned k=0;k<3;++k){Bytes sample(16);sample[15]=primary_counter++;body.insert(body.end(),sample.begin(),sample.end());}
            body.insert(body.end(),{primary_counter,128,2,128,3,128});
            slot(0,body,2,tag,true);
        }
        auto& peers=owner.secondary_peers();
        for(size_t i=0;i<peers.size();++i){
            auto& sp=peers.at(i);auto& e=*sp.engine;auto& t=trackers[i];
            const auto generation=uint16_t(e.pending?e.pending->generation:e.generation);
            f.fake.set(roy_data+multi4_peer_offset(sp.index)+16,step+2);  // module last_rx advances
            if(!t.product){
                Bytes info(35);info[0]=0x82;info[1]=189;
                const std::string serial=sp.entry.serial.empty()?"PC2310SYNTHETIC0"+std::to_string(i):sp.entry.serial;
                std::copy(serial.begin(),serial.begin()+std::min<size_t>(17,serial.size()),info.begin()+8);
                slot(sp.index,record(1,info),2,generation,false);t.product=true;
            }else if(e.pending&&e.pending->command==1&&e.pending->body.size()==18&&e.pending->body[0]==0x7e){
                const auto schema=e.pending->body[1];
                auto page=f.reply(schema,Bytes(e.pending->body.begin()+2,e.pending->body.end()));
                const auto chip_bytes=le32(sp.entry.chip);
                std::copy(chip_bytes.begin(),chip_bytes.end(),page.begin()+18);
                if(schema==2){page[22]=t.mode;page[23]=12;page[24]=t.n;page[25]=0;}
                slot(sp.index,record(1,page),3,generation,false);
                        }else if(e.pending&&e.pending->command==1&&e.pending->body.size()==24&&e.pending->body[0]==0x7d){
                const auto& request=e.pending->body;
                check(word(Bytes(request.begin()+18,request.begin()+22))==sp.entry.chip);
                Bytes page{0xfd,2};page.insert(page.end(),request.begin()+2,request.begin()+22);
                t.mode=request[22];t.n=request[23];++t.mode_sets;
                page.insert(page.end(),{0,t.mode,0,12});
                slot(sp.index,record(1,page),3,generation,false);
            }else if(e.pending&&e.pending->command==1&&e.pending->body.size()==19&&e.pending->body[0]==0x7e){
                const auto& request=e.pending->body;
                Bytes page{0xfe,3};page.insert(page.end(),request.begin()+2,request.begin()+18);
                const auto id=le32(sp.entry.chip);page.insert(page.end(),id.begin(),id.end());
                page.insert(page.end(),{1,12,request[18],6,1,0x44,7,1,1,7});
                slot(sp.index,record(1,page),3,generation,false);
            }else if(e.pending){
                if(e.pending->command==1 && e.pending->body==Bytes{0x31,1})t.independent=true;
                if(e.pending->command==16)t.n=e.pending->body[0];
                if(e.pending->command==25&&e.pending->body.size()==1){t.lit=e.pending->body[0]==2;++t.led_commands;}
                slot(sp.index,{},2,generation,false);
            }else if(sp.plane->enrolled){
                                Bytes body{0xcf,0x34,0x12}; // rev12, 3 IMU samples + magnetic tail
                for(int k=0;k<3;++k){Bytes sample(16);sample[0]=uint8_t(i+1);sample[12]=uint8_t(k);sample[15]=t.counter++;body.insert(body.end(),sample.begin(),sample.end());}
                body.insert(body.end(),{uint8_t(t.counter),128,2,128,3,128});
                slot(sp.index,body,2,generation,true);
            }
        }
    }
};
void multi_ring_exercise(LiveSessionOwner& owner,Fixture& f,MultiRing& ring){
    const auto& table=PeerTable::configured();
    auto& peers=owner.secondary_peers();
    check(peers.size()+1==table.enrolled());
    owner.radio_clock().fit=Fit{0,double(f.ns),15.625,0};
    const auto rejected_before=owner.radio_counters().peer_rejected;
    for(unsigned step=0;step<2000;++step){
        ring.step(step);
        if(step==300){
            Bytes body{0xc8,0,0};
            unsigned unenrolled=5;  // an index without a chip (or outside 0..3)
            for(unsigned k=3;k>=1;--k)if(!table.chips()[k])unenrolled=k;
            ring.slot(unenrolled,body,2,1,true);
        }
        f.fake.ns+=6000000;
        f.time(f.ns+6000000);
    }
    owner.radio_step();
    const auto& counters=owner.radio_counters();
    check(counters.peer_rejected==rejected_before+1);
    std::cout<<"multi_peer_dry_run peers="<<table.peers.size()<<" K="<<table.round_robin<<" N="<<table.tracker_n()
             <<" diverted="<<counters.diverted<<" peer_rejected="<<counters.peer_rejected;
    for(size_t i=0;i<peers.size();++i){
        auto& sp=peers.at(i);uint64_t events=0,chip_ok=0;
        while(auto ev=sp.plane->next_event()){++events;
            if(ev->chip==sp.entry.chip&&ev->wire&&word(Bytes(ev->wire->begin()+4,ev->wire->begin()+8))==sp.entry.chip)++chip_ok;}
        if(!sp.engine->stock_firmware)
            check(sp.n_read&&*sp.n_read==table.tracker_n()&&sp.n_mismatches<=1);  // first proof shows boot N
        check(ring.trackers[i].mode_sets==1 && ring.trackers[i].independent && sp.engine->firmware_revision==12);
        check(sp.plane->enrolled&&events>=20&&chip_ok==events&&sp.plane->counters.sample_gaps==0&&sp.plane->counters.source==0);
        std::cout<<" peer"<<sp.index<<"{chip="<<std::hex<<sp.entry.chip<<std::dec<<" n_read="<<(sp.n_read?int(*sp.n_read):-1)
                 <<" readbacks="<<sp.n_readbacks<<" pti1="<<events<<" state="<<int(sp.engine->state)<<"}";
    }
    check(peers.size()?counters.diverted>0:counters.diverted==0);
    std::cout<<std::endl;
}

// Multi-tracker optics (2026-10-31): after the ring exercise enrolled the secondaries (LEDs dark, 25 {1}), the
// session's own MultiLedSchedule runs one identity episode per tracker through the real StationControl/ring; the
// emulated trackers light only on an acknowledged 25 {2}. Pairs carry the secondaries' constellations, rendered by the
// REAL projection and seen by the REAL detector/solver (candidate/release/optical); the first target keeps the
// fixture's synthetic ABI (its 4 fixed blobs, pose x .25 z 1). Every PTP2 pose must carry the chip whose
// constellation it is. Synthetic only: no camera, radio or hardware.
struct MultiOpticsDry {
    std::map<uint32_t,Rigid> truth;
    std::map<uint32_t,uint64_t> poses,wrong;
    double max_error=0;
};
MultiOpticsDry* multi_dry=nullptr;  // set while the exercise runs; the publish callback records secondary poses
void multi_dry_pose(const OpticalPoseFrame& pose){
    check(multi_dry!=nullptr);
    ++multi_dry->poses[pose.chip];
    const auto t=multi_dry->truth.find(pose.chip);
    double own=1e9;
    if(t!=multi_dry->truth.end()){own=0;for(int i=0;i<3;++i)own+=std::pow(pose.position[size_t(i)]-t->second[4*size_t(i)+3],2);
    own=std::sqrt(own);}
    multi_dry->max_error=std::max(multi_dry->max_error,own<1e8?own:0.);
    if(own>.05)++multi_dry->wrong[pose.chip];
}
void multi_optics_exercise(LiveSessionOwner& owner,Fixture& f,MultiRing& ring,OpticalExposureSchedule& schedule){
    auto& peers=owner.secondary_peers();
    if(!peers.size())return;
    const auto root=std::filesystem::path(__FILE__).parent_path()/".."/"candidate"/"release"/"optical";
    NativeOpticalLibraries real((root/"libpico_optics.so").string(),(root/"libpico_detector.so").string());
    // Per chip a sub-millimetre variant of one synthetic constellation (as the factory models differ).
    auto led_model=[](double skew){
        const double mm[24]={7.4,16.0,-9.4,-4.6,13.6,-4.7,-9.3,-1.4,-9.9,-3.7,-14.4,-9.5,
                             10.2,-18.3,-9.4,22.0,-13.4,-5.0,24.7,-1.3,-9.3,19.3,13.4,-4.3};
        LedModel m{};for(size_t i=0;i<24;++i)m[i]=(mm[i]+skew*double(int(i%3)-1))/1000.;
        return m;};
    std::vector<MultiChipModel> models;
    MultiOpticsDry dry;
    multi_dry=&dry;
    struct ResetDry { ~ResetDry(){multi_dry=nullptr;} } reset_dry;
    struct Clear { ~Clear(){multi_dry=nullptr;} } clear;
    for(size_t i=0;i<peers.size();++i){
        const auto chip=peers.at(i).entry.chip;
        models.push_back({chip,peers.at(i).entry.serial,led_model(.2*double(i+1))});
        auto pose=identity_rigid();pose[3]=i%2?.24:.56;pose[7]=i%2?-.10:.06;pose[11]=.66+.04*double(i);
        dry.truth[chip]=pose;
    }
    owner.start_multi_optics(models,led_model(0),real.api(),&schedule,[&f]{return f.ns;});
    auto* optics=owner.multi_optics();
    if(!optics){
        // PICO_MULTI_OPTICS=0: the radio-only path, LEDs configured on (25 {2}) as before.
        for(size_t i=0;i<peers.size();++i){bool on=false;for(const auto& c:peers.at(i).engine->configuration)on=on||(c.first==25&&c.second==Bytes{2});
        check(on);}
        std::cout<<"multi_optics_dry_run disabled (PICO_MULTI_OPTICS=0): secondaries IMU only, configuration LEDs 25 {2}"<<std::endl;
        return;
    }
    check(owner.led_schedule()!=nullptr);
    for(size_t i=0;i<peers.size();++i){bool dark=false;for(const auto& c:peers.at(i).engine->configuration)dark=dark||(c.first==25&&c.second==Bytes{1});
    check(dark);}
    // Head at x .375, cameras at -/+.125: the fixture's synthetic primary fit (camera 0 x) is exactly x .25.
    auto render=[&](int64_t stamp){
        auto p=std::make_shared<IdentityPair>();p->timestamp_matched=true;
        for(size_t v=0;v<2;++v){
            auto& fr=p->frames[v];fr.serial=v?"CAMUPPERRIGHT":"CAMUPPERLEFT";
            fr.metadata="synthetic multi "+std::to_string(stamp)+std::to_string(v);
            fr.timestamp_matched=true;fr.stamp=stamp;fr.exposure_us=800;
            fr.view.camera={900,900,528,512,{0,0,0,0},1056,1024};
            fr.view.head_from_camera[3]=v?.125:-.125;
            fr.world_from_head[3]=.375;
            fr.pixels.assign(1056u*1024u,0);
            fr.pixels[0]=255;                 // synthetic first-target detector: lit (one pixel, below real min area)
            fr.pixels[700*1056+1040]=17;      // its packed-row sentinel
            const double cam_x=.375+(v?.125:-.125);
            for(size_t i=0;i<ring.trackers.size();++i){
                if(!ring.trackers[i].lit)continue;
                const auto& w=dry.truth.at(peers.at(i).entry.chip);const auto& m=models[i].model;
                for(size_t k=0;k<8;++k){
                    if(k==1||k==5)continue;   // two LEDs turned away (multi_optics_tests.cpp)
                    double local[3],px[2];
                    for(int r=0;r<3;++r){local[r]=w[4*size_t(r)+3];for(int c=0;c<3;++c)local[r]+=w[4*size_t(r)+size_t(c)]*m[3*k+size_t(c)];}
                    local[0]-=cam_x;
                    if(real.api().project(&fr.view.camera,local,1,px)!=0)continue;
                    const int x=int(std::lround(px[0])),y=int(std::lround(px[1]));
                    for(int dy=-1;dy<=1;++dy)for(int dx=-1;dx<=1;++dx)
                        if(x+dx>=0&&x+dx<1056&&y+dy>=0&&y+dy<1024)fr.pixels[size_t(y+dy)*1056+size_t(x+dx)]=255;
                }
            }
        }
        return p;
    };
    const auto start=f.ns;
    unsigned steps=0;
    for(;steps<1500;++steps){
        f.time(f.ns+6000000);
        ring.step(400+steps);
        f.fake.ns+=6000000;
        if(steps%5==0){
            auto p=render(f.ns);
            const auto raw=double(f.ns)/1e9;
            check(f.worker!=nullptr);
            f.worker->exposure(*p,{CameraMetadata{raw,.0008},CameraMetadata{raw,.0008}});
        }
        bool all=true;
        for(auto chip:optics->chips())all=all&&optics->state(chip)==MultiChipState::Tracking;
        if(all&&dry.poses.size()==peers.size())break;
    }
    // Ten more pairs with every tracker bound.
    for(unsigned k=0;k<50;++k){
        f.time(f.ns+6000000);ring.step(2000+k);f.fake.ns+=6000000;
        if(k%5==0){auto p=render(f.ns);const auto raw=double(f.ns)/1e9;f.worker->exposure(*p,{CameraMetadata{raw,.0008},CameraMetadata{raw,.0008}});}
    }
    const auto c=optics->counters();
    const auto& led=*owner.led_schedule();
    std::cout<<"multi_optics_dry_run chips="<<optics->chips().size()<<" seconds="<<double(f.ns-start)/1e9
             <<" episodes="<<led.episodes_started()<<" syncs_acked="<<led.syncs_acknowledged()
             <<" led_acks="<<led.commands_acknowledged()<<" provisional="<<c.provisional<<" bound="<<c.bound
             <<" primary_masked="<<c.primary_masked<<" max_error_mm="<<dry.max_error*1000;
    for(size_t i=0;i<peers.size();++i){
        const auto chip=peers.at(i).entry.chip;
        std::cout<<" "<<std::hex<<chip<<std::dec<<"{state="<<multi_chip_state_name(optics->state(chip))
                 <<" ptp2="<<dry.poses[chip]<<" wrong="<<dry.wrong[chip]<<" led_commands="<<ring.trackers[i].led_commands<<"}";
        check(optics->state(chip)==MultiChipState::Tracking && dry.poses[chip]>=10 && dry.wrong[chip]==0);
        check(ring.trackers[i].led_commands>=4);  // dark at configuration, ON, OFF, ON
    }
    std::cout<<std::endl;
    check(c.bound==peers.size() && led.syncs_acknowledged()>=peers.size() && dry.max_error<.01);
}

ProducerReplayResult replay(const std::function<void(const std::function<void()>&)>& work,const std::function<void()>& guard,const std::function<void(const std::string&)>& restored,const std::string& safety_fault={},const std::function<void()>& verify_invocation={},const std::function<void()>& verify_profile={},LiveSessionOwner* shared_owner=nullptr,int replay_seconds=180) {
    // Standalone optical tests use the same graph without applying a profile.
    LiveSessionOwner test_owner({}, {}, {});
    auto& owner=shared_owner?*shared_owner:test_owner;
    auto context=std::make_shared<Fixture>(owner);auto& f=*context;f.fault=safety_fault;
    owner.start_debugger(f.debugger_hooks(),context);
    auto read_image=[](const std::filesystem::path& path){std::ifstream in(path,std::ios::binary);if(!in)throw std::runtime_error("Protected image fixture absent");
    return Bytes(std::istreambuf_iterator<char>(in),{});};
    const auto root=std::filesystem::path(__FILE__).parent_path()/".."/"candidate";
    auto image=read_image(root/"stock.bin"),module=read_image(root/"build"/"roy_station.elf");Bytes state(2048);
    // Same private endpoint/DebugMemory as identity's mailbox and ACK bridge.
    f.fake.stock(image);
    auto magic=le32(0x314d5250);std::copy(magic.begin(),magic.end(),state.begin());state[84]=255;
    auto ops=f.fake.io(f.debug);
    ops.command=[context](const std::string& c){return context->owner.debugger_command(c);};
    auto write=ops.write;auto write_failed=std::make_shared<bool>(false);
    ops.write=[write,write_failed,safety_fault](uint32_t address,const Bytes& bytes){write(address,bytes);
        if(safety_fault=="sram-write"&&address==roy_code&&!*write_failed){*write_failed=true;throw std::runtime_error("Injected partial SRAM code write");}};
    auto checkpoint_commits=std::make_shared<unsigned>(0);
    auto& ram=owner.prepare_sram(ops,[checkpoint_commits,safety_fault](const Checkpoint& c){check(!checkpoint_json(c).empty());
        if(safety_fault=="checkpoint-failure"&&++*checkpoint_commits==3)throw std::runtime_error("Injected checkpoint persistence failure");});
    std::string barriers;
    size_t publications=0;
    struct Optics {
        OpticalCameraClock camera{{1,"boot","CLOCK_MONOTONIC","synthetic",true,0,0},"boot"};
        OpticalExposureSchedule schedule;
        OpticalPhaseVerification phase{261700,{chip},true};
        explicit Optics(RoyClock& clock):schedule(camera,clock,261700){}
    };
    auto optics=std::make_shared<Optics>(f.clock);
    std::weak_ptr<Optics> retained_optics=optics;
    auto& camera=optics->camera;auto& schedule=optics->schedule;auto& phase=optics->phase;
    // Explicit synthetic observer/camera adapters, under the same envelope a
    // production owner must use. Never infer real admission from these rows.
    std::optional<int64_t> tracking_stamp;
    auto& installed=f.installed;
    std::string observer_bytes;
    owner.start_observer({[&](size_t n){
        auto out=observer_bytes.substr(0,n);observer_bytes.erase(0,out.size());
        return ObserverRead{out,installed&&safety_fault=="observer-eof"};
    },[&]{return !(installed&&safety_fault=="observer-loss");},[]{return true;},[]{return false;},[&]{return base+ops.now();}});
    OpticalPoseFrame last;
    LiveSessionHooks safety_hooks;
    safety_hooks.sealed_invocation=[&]{
        if(verify_invocation)verify_invocation(); // outer synthetic Files, real seal verifier

    };
    safety_hooks.profile_exclusion=[&]{if(installed&&safety_fault=="profile-writer")throw std::runtime_error("Injected profile writer exclusion lost");
    if(verify_profile)verify_profile();}; // synthetic outer profile
    safety_hooks.ownership=[&]{
        SessionOwnership evidence{"/home/steamos/.local/state/frame-pico/roy.lock","inactive",1,2,3,10,11,3,true,true,{10,11}};
        if(installed&&safety_fault=="owner-unknown")evidence.radio_processes.push_back(99);
        if(installed&&safety_fault=="owner-lock-loss")evidence.lock_held=false;
        return evidence; // synthetic; no OS ownership claim
    };
    safety_hooks.listener_attribution=[&]{f.attribution();};
    safety_hooks.now=[&]{return base+ops.now()+(installed&&safety_fault=="wall-bound"?int64_t(replay_seconds)*1000000000LL:0);};
    safety_hooks.parent_alive=[&]{return !(installed&&safety_fault=="parent-loss");}; // synthetic parent
    safety_hooks.cancelled=[]{return false;}; // synthetic cancellation
    safety_hooks.observer_poll=[&](phase5::BenchTrackingGuard& tracking){
        if(installed&&safety_fault=="tracking-loss")throw std::runtime_error("Injected HMD/controller tracking loss");
        auto now=base+ops.now();if(tracking_stamp&&now==*tracking_stamp)return;
        observer_bytes="{\"t_ns\":"+std::to_string(now)+",\"devices\":[{\"index\":0,\"kind\":1,\"connected\":true,\"valid\":true,\"result\":200,\"matrix\":[1,0,0,0,0,1,0,0,0,0,1,0]}]}\n";
        if(installed&&safety_fault=="controllers-on"){
            auto row=phase5::parse(observer_bytes);auto device=row.object["devices"].array[0];device.object["index"]=phase5::parse("1");device.object["kind"]=phase5::parse("2");device.object["valid"]=phase5::parse("false");row.object["devices"].array.push_back(device);observer_bytes=phase5::encode(row)+"\n";
        }
        owner.observer_poll(tracking);
        tracking_stamp=now;
    };
    safety_hooks.observer_alive=[&]{return !(installed&&safety_fault=="observer-loss");};
    safety_hooks.worker_health=guard;
    safety_hooks.diagnostics_ready=[]{return true;};
    safety_hooks.camera_poll=[&]{if(installed&&safety_fault=="camera-stale")throw std::runtime_error("Injected OCC no-growth >2s");};
    safety_hooks.roy_poll=[](bool){}; // synthetic fit supplied by ACK fixture
    safety_hooks.publication_poll=[&]{if(publications)owner.publication_poll(f.ns,true,bool(f.s().bindings_snapshot()));};
    LiveSessionSafety safety(std::move(safety_hooks),phase,ReplaySessionLimit{replay_seconds});
    f.session_guard=[&]{safety.guard();};
    bool restore_attempted=false;
    try {
    safety.admit();owner.install_sram(safety,image,module,state);owner.enter_sram();
    installed=true;
    if(safety_fault=="seal-change"||safety_fault=="argv-change")safety.before_sram();
    if(safety_fault=="phase-exhausted"){
        phase.ack(chip,261700);for(int64_t i=900;i<=960;++i)phase.observe(i,false,false);
    }
    Hooks plane_hooks;
    plane_hooks.now=ops.now;
    plane_hooks.send=[](const std::array<uint8_t,32>&)->bool{throw std::runtime_error("Unexpected replay IMU publication");};
    LoopHooks loop_hooks;
    loop_hooks.guard=[&]{safety.guard();};
    loop_hooks.poll=[&](const Bytes&){safety.poll();};
    loop_hooks.publish=[](uint32_t,bool){};
    loop_hooks.repeat=[&]{ram.repeat();};
    loop_hooks.latency=[](const IterationLatency&){};
    owner.start_radio(ops,std::move(plane_hooks),std::move(loop_hooks));
    owner.prepare_identity(model(),safety_fault=="identity-construction"?NativeOpticalApi{}:api);
    QueryHooks qh;
    qh.bench=[]{BenchImuEvidence b;b.forbid_controllers=true;b.min_controllers=0;b.guard_mode="strict";return b;};
    qh.fresh_guard=[&]{safety.fresh_tracking();};
    // Native radio ticks are installed by the session owner.
    qh.proof=[](unsigned,const Bytes&,const Page&){};
    qh.action=[](uint8_t,const Bytes&){};
    qh.prerequisites=[](const std::vector<std::pair<unsigned,Page>>&,const std::string&){};
    qh.producer_lifetime=[](double){};
    f.peer=&owner.start_query(f.clock,[&]{return f.peer_seconds;},[&]{return Bytes(16,++f.nonce);},std::move(qh),
        [&](const std::string& name,unsigned attempt,uint8_t sequence,const Bytes& nonce){
            f.query_events.push_back({name,attempt,sequence,nonce,f.seconds});
            if(name=="exhausted"){
                check(attempt==3&&f.query_events.size()==4);
                check(f.seconds-f.query_events.front().time>=24&&f.seconds-f.query_events.front().time<24.2);
            }
        },[&]{return f.ns;});
    f.startup();
    if(safety_fault=="query-lease-expired"){
        f.e().continuity_lease=false;f.time(f.ns+31000000000LL);owner.query_poll();
        throw std::runtime_error("Expired mode lease was accepted");
    }
    const auto episode_base=f.ns;
    safety.poll();
#ifndef _WIN32
    struct Receiver {
        int fd=socket(AF_INET,SOCK_DGRAM,0);
        ~Receiver() { if(fd>=0) close(fd); }
    } receiver;
    check(receiver.fd>=0);
    sockaddr_in address{};
    address.sin_family=AF_INET; address.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
    check(bind(receiver.fd,reinterpret_cast<sockaddr *>(&address),sizeof(address))==0);
    socklen_t address_size=sizeof(address);
    check(getsockname(receiver.fd,reinterpret_cast<sockaddr *>(&address),&address_size)==0);
    timeval timeout{1,0};
    check(setsockopt(receiver.fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout))==0);
    auto publisher=std::make_shared<OpticalPosePublisher>(ntohs(address.sin_port));
#endif
    auto& worker=owner.start_worker(model(),api,camera,schedule,phase,[&]{return f.ns;},
#ifndef _WIN32
        [&,publisher](const OpticalPoseFrame &pose,int64_t now) {
#else
        [&](const OpticalPoseFrame &pose,int64_t now) {
#endif
            if(installed&&safety_fault=="publication-send")return false;
            auto bytes=pose.encode(now);
            check(bytes[0]=='P' && bytes[3]=='2');
            // Multi-tracker optics: another chip's pose through the same sink and publisher, with its own chip.
            if(pose.chip!=chip){
                check(multi_dry!=nullptr && pose.valid && bytes[8]==3 && word(Bytes(bytes.begin()+4,bytes.begin()+8))==pose.chip);
#ifndef _WIN32
                check(publisher->send(pose,now));
                std::array<uint8_t,88> received{};
                check(recv(receiver.fd,received.data(),received.size(),0)==88);
                check(received==bytes);
#endif
                multi_dry_pose(pose);return true;
            }
            if(pose.valid){
                check(bytes[8]==3);
                check(pose.position[0]==.25 && pose.position[2]==1);
                check(pose.quaternion==std::array<double,4>{0,0,0,1});
            }else check((bytes[8]&2)==0&&pose.quality==0);
#ifndef _WIN32
            check(publisher->send(pose,now));
            std::array<uint8_t,88> received{};
            check(recv(receiver.fd,received.data(),received.size(),0)==88);
            check(received==bytes);
#endif
            if(!pose.valid)return true;
            last=pose; ++publications; return true;
        },optics);
    f.worker=&worker;
    optics.reset();check(!retained_optics.expired());
    // Probe through the owned RoyClockPoll. This fixture supplies synthetic
    // DWT/RAW input; it is not measured crystal evidence or live admission.
    auto probe_io=ops;
    auto probe_read=probe_io.read;
    probe_io.read=[probe_read,safety_fault](uint32_t address,size_t count){
        if(safety_fault=="roy-probe-read")throw std::runtime_error("Injected owned Roy probe read failure");
    return probe_read(address,count);
    };
    RoyClock foreign_clock;bool foreign_rejected=false;
    try{owner.start_clock_poll(probe_io,foreign_clock,ops.now,[](const ClockProbeEvent&){});}
    catch(const std::runtime_error&){foreign_rejected=true;}
    check(foreign_rejected);
    owner.start_clock_poll(std::move(probe_io),f.clock,ops.now,[](const ClockProbeEvent&){});
    owner.clock_poll(false);f.fake.ns+=50000001;owner.clock_poll(true);
    if(safety_fault=="worker-stopped")owner.stop_worker();
    if(safety_fault=="worker-reentrant-stop")after_solve=[&]{owner.stop_worker();};
    phase.ack(chip,261700);
    f.run_episode(episode_base,0);  // 2026-10-31 multi copy: no publications==0 check, poses publish before phase lock
    f.run_episode(episode_base+1000000000LL,30);
    check(f.s().bindings_snapshot()); const auto published_before_lock=publications;  // no phase-lock gate now
    auto plan=schedule.prepare(0,1,f.ns,0,chip);
    check(bool(plan) && plan->light_due && plan->count==1);
    schedule.light_ack(chip,1); schedule.phase_ack(chip,f.ns);
    check(!schedule.prepare(0,1,f.ns,0,chip));
    for(int i=900;i<=960;++i) f.frame(14,episode_base+(i-14)*period);
    check(phase.state()==IdentityPhaseWindow::State::Locked);
    check(publications>published_before_lock && last.stamp==episode_base+960*period); const auto published_at_lock=publications;
    f.frame(14,episode_base+(961-14)*period); check(publications==published_at_lock+1);
    if(safety_fault=="replay-over-180"){
        // Inject elapsed decode time without sleeping or executing a decoder.
        for(unsigned i=0;i<181;++i){f.fake.ns+=1000000000LL;safety.guard();}
        owner.radio_step();
    }
    if(PeerTable::configured().multi){MultiRing ring(owner,f);multi_ring_exercise(owner,f,ring);multi_optics_exercise(owner,f,ring,schedule);}
    if(work)work([&]{safety.guard();owner.radio_step();});
    restore_attempted=true;(void)owner.restore_sram(safety);check(ram.barriers.complete()&&ram.may_release_owner());
    barriers=ram.barriers.json();if(restored)restored(barriers);
    } catch(...) {
        // Restore before the socket can be released on any session-stage error.
        auto failure=std::current_exception();
        if(safety_fault.rfind("query-",0)==0){
            bool poll_blocked=false,radio_blocked=false;
            try{owner.query_poll();}catch(const std::exception&){poll_blocked=true;}
            try{owner.radio_step();}catch(const std::exception&){radio_blocked=true;}
            check(poll_blocked&&radio_blocked);
        }
        // A failed normal restore retains ownership; never silently retry it.
        if(!restore_attempted){restore_attempted=true;(void)owner.restore_sram(safety);}
        check(ram.barriers.complete()&&ram.may_release_owner());
        if(restored)restored(ram.barriers.json());
        owner.close_components();
        std::rethrow_exception(failure);
    }
    owner.close_components();check(retained_optics.expired());return {publications,barriers};
}
}
size_t synthetic_optical_replay(){return replay({},[]{},{}).publications;}
ProducerReplayResult unified_synthetic_replay(const std::function<void(const std::function<void()>&)>& work,const std::function<void()>& guard,const std::function<void(const std::string&)>& restored,const std::string& safety_fault,const std::function<void()>& verify_invocation,const std::function<void()>& verify_profile,LiveSessionOwner* owner,int replay_seconds){return replay(work,guard,restored,safety_fault,verify_invocation,verify_profile,owner,replay_seconds); }
