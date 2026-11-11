#include "native_test_runtime.hpp"
#include "optical_worker.hpp"
#include "optical_compute.hpp"
#include "session_state_guard.hpp"
#include <future>
#include <fstream>
#include <thread>
#include "fake_openocd.hpp"
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
void check(bool b,int line=__builtin_LINE()) { ++checks; if(!b) throw std::runtime_error("worker check "+std::to_string(checks)+" line "+std::to_string(line)); }
template<class F> void rejects(F f,int line=__builtin_LINE()) { bool bad=false; try { f(); } catch(const std::invalid_argument &) { bad=true; } check(bad,line); }
constexpr uint32_t chip=0xA1B2C3D4;
constexpr int64_t base=3000000000LL, period=33333333;
// Explicit synthetic ABI doubles; no claim of Windows .so execution.
int fit_mutation=0;
int project(const OpticalCamera *c,const double *p,size_t n,double *out) {
    if(fit_mutation==8 || fit_mutation==9) return 1; // No geometrically supported continuation.
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
double world_shift=0;
std::function<void()> after_solve;
int solve(const double *model,const OpticalView *views,const double *prior,OpticalFit *fit) {
    check(model && views[0].count==4 && views[1].count==4);
    check(views[0].camera.width==1056 && views[0].camera.height==1024 && views[1].camera.width==1056 && views[1].camera.height==1024 && views[1].pixels[0]==520);
    auto t=identity_rigid(); t[11]=1; t[3]=views[0].head_from_camera[3]+world_shift; std::copy(t.begin(),t.end(),fit->pose); fit->count=4;
    if(fit_mutation==8) fit->pose[0]=fit->pose[5]=-1; // Synthetic 180-degree optical branch.
    fit->residual=fit_mutation==1?2.50001:0;
    fit->position_std=fit_mutation==2?.030001:(fit_mutation==6?.02:0);
    fit->rotation_std_deg=fit_mutation==3?5.00001:0;
    fit->condition=fit_mutation==4?10000001:0;
    if(fit_mutation==5) fit->residual=std::numeric_limits<double>::quiet_NaN();
    if(after_solve) after_solve();
    return fit_mutation==7 || (fit_mutation==9 && !prior)?1:0;
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
    int64_t ns=base; double seconds=double(base)/1e9;
    FakeOpenOcd fake;
    DebugMemory debug{[this](const std::string &s){return fake.command(s);}};
    MailboxWriter writer{debug};
    PeerEngine e{le32(chip),0,[this]{return seconds;},[]{return Bytes(16,7);}};
    RoyClock clock;
    NativeIdentitySession s;
    Native25AckBridge bridge{s.owner,e,clock,"live",[this]{return ns;}};
    explicit Fixture(NativeOpticalApi optical_api=api) : s(chip,model(),"live",optical_api) {
        e.state=PeerState::Streaming; e.mode_required=true; e.mode_profile="frame"; e.mode_confirmed_at=seconds; e.last_reply=seconds;
    }
    void time(int64_t t) { ns=t; seconds=double(t)/1e9; e.last_reply=seconds; e.mode_confirmed_at=seconds; }
    void ack() {
        auto g=e.pending->generation; Record r; r.hardware_crc=true; r.channel=2; r.profile=0;
        r.chip=chip; r.generation=g; r.dma={4,2,0,0,0,0}; clock.fit=Fit{0,double(ns),15.625,0};
        bool accepted=false;
        bridge.record(r,[&]{ accepted=e.receive(Bytes(r.dma.begin()+2,r.dma.end()),2,uint16_t(g),true); }); check(accepted);
    }
    OpticalWorker *worker=nullptr;
    bool measured=false;
    bool feed_gyro=false;
    int accel_y=0;
    int64_t imu_stamp=0;
    void frame(int i,int64_t start,int64_t offset=0) {
        (void)offset;
        auto stamp=start+i*period; time(stamp);
        auto p=pair(stamp);
        for(auto &f:p->frames) { f.pixels[0]=(i<4 || i>=11)?255:0; f.world_from_head[3]=.25; }
        auto raw=double(stamp)/1e9;
        check(worker!=nullptr);
        if(feed_gyro) {
            if(!imu_stamp) imu_stamp=stamp-4000000;
            while(imu_stamp<=stamp) {
                std::array<uint8_t,32> wire{}; std::memcpy(wire.data(),"PTI1",4);
                auto put=[&](int at,uint64_t value,int size){for(int j=0;j<size;++j)wire[size_t(at+j)]=uint8_t(value>>(8*j));};
                put(4,chip,4); put(12,uint64_t(imu_stamp),8); put(28,uint16_t(accel_y),2); worker->prior_imu(wire);
                imu_stamp+=4000000;
            }
        }
        if(measured){
            for(auto& frame:p->frames)frame.stamp=0;
            worker->measured_exposure(*p,{CameraMetadata{raw,.0008},CameraMetadata{raw,.0008}},
                [&](int64_t source){check(source==stamp);auto head=identity_rigid();head[3]=.25;return head;});
        }else{
            worker->exposure(*p,{CameraMetadata{raw,.0008},CameraMetadata{raw,.0008}});
        }
    }    void run_episode(int64_t start,int64_t offset) {
        for(int i=0;i<3;++i) frame(i,start,offset);
        time(start+100000000); check(s.begin_if_ready(e,"live",ns,true));
        check(bridge.publish(writer,0,e.mailbox())); ack();
        time(start+350000000); s.radio_tick(e,"live",ns);
        check(e.pending && e.pending->body==Bytes{2});
        check(bridge.publish(writer,0,e.mailbox()));
        time(start+360000000); ack(); check(s.owner.restore_required());
        for(int i=3;i<=14;++i) frame(i,start,offset);
        check(s.owner.restore_required()); // optical cannot retire/clear owner
        time(start+14*period); s.radio_tick(e,"live",ns);
        check(!s.owner.restore_required() && !e.native25_exclusive);
    }
};

void gyro_flip_regression() {
    if(!tracking_recovery_enabled()) return;
    Fixture f; f.feed_gyro=true;
    OpticalCameraClock camera({1,"boot","CLOCK_MONOTONIC","synthetic",true,0,0},"boot");
    OpticalExposureSchedule schedule(camera,f.clock,261700);
    OpticalPhaseVerification phase(261700,{chip},true);
    size_t publications=0;
    OpticalWorker worker(chip,model(),api,"live",camera,schedule,phase,f.s,[&]{return f.ns;},
        [&](const OpticalPoseFrame& pose,int64_t) {
            check(pose.quaternion==std::array<double,4>{0,0,0,1}); ++publications; return true;
        });
    f.worker=&worker;
    {std::ofstream out("build/worker-recovery-calibration.txt"); out<<"A1B2C3D4 1 0 0 0 1 0 0 0 1 0.001 0 0 0 0\n";}
    check(worker.prior_calibration("build/worker-recovery-calibration.txt"));
    f.run_episode(base,0);
    const auto start=base+1000000000;
    for(int i=11;i<=20;++i) f.frame(i,start);
    check(publications>0);
    const auto before=publications;
    fit_mutation=8;
    for(int i=21;i<=30;++i) f.frame(i,start);
    check(publications==before); // Repeated wrong branches must not re-anchor after five frames.
    fit_mutation=0;
    f.frame(31,start);
    check(publications>before); // Drop the sample, keep running, recover on consistent evidence.
}

void missing_gyro_regression(bool tilt) {
    if(!tracking_recovery_enabled() && !few_led_enabled()) return;
    Fixture f;
    OpticalCameraClock camera({1,"boot","CLOCK_MONOTONIC","synthetic",true,0,0},"boot");
    OpticalExposureSchedule schedule(camera,f.clock,261700);
    OpticalPhaseVerification phase(261700,{chip},true);
    size_t publications=0;
    OpticalWorker worker(chip,model(),api,"live",camera,schedule,phase,f.s,[&]{return f.ns;},
        [&](const OpticalPoseFrame&,int64_t){++publications;return true;});
    f.worker=&worker; f.run_episode(base,0);
    const auto start=base+1000000000;
    for(int i=11;i<=20;++i) f.frame(i,start);
    check(publications>0);
    if(tilt) {
        {std::ofstream out("build/worker-recovery3-calibration.txt");out<<"A1B2C3D4 1 0 0 0 1 0 0 0 1 0.001 0 0 0 0\n";}
        check(worker.prior_calibration("build/worker-recovery3-calibration.txt")); // no optical anchor
        f.feed_gyro=true; f.accel_y=1024;
    }
    const auto before=publications;
    fit_mutation=tilt?8:9;
    f.frame(21,start);
    check(publications==before); // round 2 publishes the unsupported/tilted fix
    fit_mutation=0; f.frame(22,start);
    check(publications>before); // sample drop does not stop the session
}


void replay(bool measured) {
    Fixture f;
    f.measured=measured;
    OpticalCameraClock camera({1,"boot","CLOCK_MONOTONIC","synthetic",true,0,0},"boot");
    OpticalExposureSchedule schedule(camera,f.clock,261700);
    OpticalPhaseVerification phase(261700,{chip},true);
    size_t publications=0;
    OpticalPoseFrame last;
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
    OpticalPosePublisher publisher(ntohs(address.sin_port));
#endif
    OpticalWorker worker(chip,model(),api,"live",camera,schedule,phase,f.s,[&]{return f.ns;},
        [&](const OpticalPoseFrame &pose,int64_t now) {
            auto bytes=pose.encode(now);
            if(const char* path=std::getenv("PICO_TEST_POSES_FILE")) {
                std::ofstream out(path,std::ios::binary|std::ios::app);
                out.write(reinterpret_cast<const char*>(bytes.data()),std::streamsize(bytes.size()));
                check(bool(out));
            }
            check(bytes[0]=='P' && bytes[3]=='2' && bytes[8]==3);
            if(world_shift==0) check(pose.position[0]==.25 && pose.position[2]==1);  // the shifted jump publishes too now
            check(pose.quaternion==std::array<double,4>{0,0,0,1});
#ifndef _WIN32
            check(publisher.send(pose,now));
            std::array<uint8_t,88> received{};
            check(recv(receiver.fd,received.data(),received.size(),0)==88);
            check(received==bytes);
#endif
            last=pose; ++publications; return true;
        });
    f.worker=&worker;
    phase.ack(chip,261700);
    f.run_episode(base,0);
    f.run_episode(base+1000000000LL,30);
    // 2026-10-29: poses publish once identity is verified (no phase-lock gate), so count from here.
    check(f.s.bindings_snapshot()); const auto published_before_lock=publications;
    auto plan=schedule.prepare(0,1,f.ns,0,chip);
    check(bool(plan) && plan->light_due && plan->count==1);
    schedule.light_ack(chip,1); schedule.phase_ack(chip,f.ns);
    check(!schedule.prepare(0,1,f.ns,0,chip));
    for(int i=900;i<=960;++i) f.frame(14,base+(i-14)*period);
    check(phase.state()==IdentityPhaseWindow::State::Locked);
    check(publications>published_before_lock && last.stamp==base+960*period);
    const auto published_at_lock=publications;
    check(worker.position_covariance()[0] == .003 * .003);
    check(worker.position_covariance()[4] == .003 * .003);
    check(worker.position_covariance()[8] == .003 * .003);
    check(worker.position_covariance()[1] == 0);
    check(worker.velocity_covariance()[0] == .2 * .2);
    check(worker.velocity_covariance()[4] == .2 * .2);
    check(worker.velocity_covariance()[8] == .2 * .2);
    fit_mutation = 6;
    f.frame(14,base+(961-14)*period); check(publications==published_at_lock+1);
    check(worker.position_covariance()[0] == .02 * .02);
    fit_mutation = 0;
    // Prepared data must remain current and bind the actual exposure metadata.
    auto p=pair(f.ns); double raw=double(f.ns)/1e9;
    // Stereo skew and metadata mismatch are logged and dropped, never thrown.
    auto drops=worker.stale_drops();
    check(!worker.exposure(*p,{CameraMetadata{raw+.001,.0008},CameraMetadata{raw,.0008}}));
    check(worker.stale_drops()==drops+1);
    auto rejected=worker.rejected_drops();
    check(!worker.exposure(*p,{CameraMetadata{raw,.0007},CameraMetadata{raw,.0007}}));
    check(worker.rejected_drops()==rejected+1 && publications==published_at_lock+1);
    auto old=f.ns; f.ns+=250000001;
    drops=worker.stale_drops();
    check(!worker.exposure(*p,{CameraMetadata{raw,.0008},CameraMetadata{raw,.0008}}));
    check(worker.stale_drops()==drops+1);
    f.ns=old;
    check(!worker.exposure(*p,{CameraMetadata{raw+.001,.0008},CameraMetadata{raw+.001,.0008}}));
    check(worker.stale_drops()==drops+2);
    // ABI fit quality failures never publish, despite an already bound identity.
    for(int mutation=1;mutation<=5;++mutation) {
        fit_mutation=mutation; f.frame(14,base+(961+mutation-14)*period);
        check(publications==published_at_lock+1);
        check(worker.position_covariance()[0] == .02 * .02);
    }
    check(worker.fit_drops()==5);
    fit_mutation=0;
    // A late solver must recheck the actual pair age before identity/publication.
    after_solve=[&]{f.ns+=250000001;};
    f.frame(14,base+(967-14)*period);
    check(publications==published_at_lock+1); after_solve={};
    // Establish a fresh successful anchor, then reject a >8m/s identity jump.
    f.frame(14,base+(968-14)*period); check(publications==published_at_lock+2);
    check(worker.position_covariance()[0] == .003 * .003);
    // No speed gate any more (owner rule): a >8 m/s jump is counted and still published.
    world_shift=1;
    f.frame(14,base+(969-14)*period);
    check(publications==published_at_lock+3 && worker.speed_drops()==1); world_shift=0;
    f.frame(14,base+(970-14)*period); check(publications==published_at_lock+4);
    f.s.invalidate(); f.frame(14,base+(971-14)*period); check(publications==published_at_lock+4);
    worker.stop_join();worker.stop_join();
    // A late exposure after stop is logged and dropped.
    rejected=worker.rejected_drops();
    f.frame(14,base+(972-14)*period); check(publications==published_at_lock+4 && worker.rejected_drops()==rejected+1);
}
void live_entry_cadence() {
    Fixture f;
    OpticalCameraClock camera({1,"boot","CLOCK_MONOTONIC","synthetic",true,0,0},"boot");
    OpticalExposureSchedule schedule(camera,f.clock,261700);
    OpticalPhaseVerification phase(261700,{chip},true);
    OpticalWorker worker(chip,model(),api,"live",camera,schedule,phase,f.s,[&]{return f.ns;},
        [](const OpticalPoseFrame&,int64_t){return true;});
    const int64_t origin=base+4000000000LL;
    for(int i=0;i<=120;++i) {
        const auto t=origin+int64_t(i)*50000000;
        check(f.clock.observe(nearest_even(double(t-origin)/15.625),t,t,t,t)==(i>=6));
    }
    auto time=[&](int64_t t) {
        f.time(t);
        check(f.clock.observe(nearest_even(double(t-origin)/15.625),t,t,t,t));
    };
    auto ack=[&](int64_t lag,bool expected) {
        Record r;r.hardware_crc=true;r.channel=2;r.profile=0;r.chip=chip;
        r.generation=f.e.pending->generation;r.dma={4,2,0,0,0,0};
        r.cycles=f.clock.cycles_at(f.ns-lag);
        bool accepted=false;
        f.bridge.record(r,[&]{accepted=f.e.receive(Bytes(r.dma.begin()+2,r.dma.end()),2,uint16_t(r.generation),true);});
        check(accepted==expected);
    };
    auto frame=[&](int i,int64_t start) {
        const auto raw=double(start+int64_t(i)*period)/1e9;
        const auto stamp=nearest_even(raw*1e9);
        time(stamp+12000000);
        auto p=pair(stamp);
        for(auto& image:p->frames) image.pixels[0]=(i<4||i>=11)?255:0;
        worker.exposure(*p,{CameraMetadata{raw,.0008},CameraMetadata{raw,.0008}},i>=4&&i<=10);
    };
    for(int episode=0;episode<2;++episode) {
        const auto start=origin+6000000000LL+int64_t(episode)*1000000000;
        for(int i=0;i<3;++i)frame(i,start);
        time(start+80000000);check(f.s.begin_if_ready(f.e,"live",f.ns,true));
        check(f.bridge.publish(f.writer,0,f.e.mailbox()));
        // Uncertainty only widens the ACK window; an ACK whose upper bound still
        // precedes the publication is dropped and the command stays pending.
        check(f.clock.fit->uncertainty>0);
        ack(50000000,false);
        check(f.s.owner.restore_required() && f.e.pending);
        time(f.ns+1000000);ack(1000,true);
        for(int i=3;i<=9;++i)frame(i,start);
        time(start+330000000);f.s.radio_tick(f.e,"live",f.ns);
        check(f.e.pending&&f.e.pending->body==Bytes{2});
        check(f.bridge.publish(f.writer,0,f.e.mailbox()));
        check(f.clock.fit->uncertainty>0);
        ack(50000000,false);
        check(f.s.owner.restore_required() && f.e.pending);
        time(f.ns+1000000);ack(1000,true);
        for(int i=10;i<=14;++i)frame(i,start);
        f.s.radio_tick(f.e,"live",f.ns);
        check(!f.s.owner.restore_required());
    }
    check(f.s.bindings_snapshot());
    check(worker.stale_drops()==0&&worker.head_drops()==0);
}
void unlocked_slow_solve(bool invalidate=false,bool reject=false){
    Fixture f;
    OpticalCameraClock camera({1,"boot","CLOCK_MONOTONIC","synthetic",true,0,0},"boot");
    OpticalExposureSchedule schedule(camera,f.clock,261700);
    OpticalPhaseVerification phase(261700,{chip},true);
    OpticalWorker worker(chip,model(),api,"live",camera,schedule,phase,f.s,[&]{return f.ns;},
        [](const OpticalPoseFrame&,int64_t){return true;});
    std::recursive_mutex shared;
    uint64_t epoch=0;
    worker.session_compute(shared,[&]{return epoch;},[]{return true;});
    std::promise<void> solving,radio_locked;
    auto entered=solving.get_future();auto acquired=radio_locked.get_future();
    bool free_during_solve=false;
    after_solve=[&]{
        solving.set_value();
        free_during_solve=acquired.wait_for(std::chrono::seconds(2))==std::future_status::ready;
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
        // A slow solve: outlast the pair budget whatever its length (15 ms before, 100 ms since 2026-10-30 19:24).
        while(OpticalComputeBudget::current && !OpticalComputeBudget::exhausted()) std::this_thread::sleep_for(std::chrono::milliseconds(5));
    };
    std::thread radio([&]{entered.wait();SessionStateGuard lock(shared);if(invalidate)++epoch;
    radio_locked.set_value();});
    auto p=pair(f.ns);for(auto& frame:p->frames)frame.pixels[0]=255;
    const auto skipped=optical_drops("Pair solver budget skipped lower fallback");
    if(reject){
        fit_mutation=7;p->direct_capture=true;
        auto lower=p->frames[0];lower.serial="synthetic-lower";lower.width=640;lower.height=480;
        lower.view.camera.width=640;lower.view.camera.height=480;lower.pixels.resize(640*480);
        p->lower.push_back(std::move(lower));
    }
    {
        SessionStateGuard lock(shared);SessionStateGuard nested(shared);
        (void)worker.exposure(*p,{CameraMetadata{double(f.ns)/1e9,.0008},CameraMetadata{double(f.ns)/1e9,.0008}});
    }
    radio.join();after_solve={};fit_mutation=0;check(free_during_solve);
    if(reject)check(optical_drops("Pair solver budget skipped lower fallback")==skipped+1);
    if(invalidate)check(worker.rejected_drops()==1);
}
std::vector<double> clutter_blobs,expected_ring;
bool cluster_solved=false;
int clutter_detect(const uint8_t*,int,int,int,int,int,double* out,int cap){
    if(size_t(cap)<clutter_blobs.size()/5)return -1;
    std::copy(clutter_blobs.begin(),clutter_blobs.end(),out);return int(clutter_blobs.size()/5);
}
int cluster_solve(const double*,const OpticalView* views,const double*,OpticalFit*){
    for(int v=0;v<2;++v){
        check(views[v].count==8);
        check(std::equal(expected_ring.begin(),expected_ring.end(),views[v].pixels));
    }
    cluster_solved=true;return 1; // no fabricated pose needed to prove solver input
}
void clutter_ring(){
    std::vector<double> blobs;
    for(int i=0;i<60;++i)blobs.insert(blobs.end(),{double(20+(i%10)*100),double(20+(i/10)*130),1.5,255,7});
    std::vector<double> ring;
    for(int i=0;i<8;++i){
        const double a=i*3.141592653589793/4,x=505+18*std::cos(a),y=475+18*std::sin(a);
        blobs.insert(blobs.end(),{x,y,1.5,220,7});ring.insert(ring.end(),{x,y});
    }
    const auto before=optical_drops("Candidate cluster cap dropped blobs");
    {const auto picked=compact_led_cluster(blobs);bool all=true;for(size_t r=0;r<ring.size();r+=2){bool hit=false;for(size_t p=0;p<picked.size();p+=2)hit=hit||(picked[p]==ring[r]&&picked[p+1]==ring[r+1]);
    all=all&&hit;}check(all&&picked.size()<=24);}
    check(optical_drops("Candidate cluster cap dropped blobs")==before+1);
    clutter_blobs=blobs;expected_ring=ring;cluster_solved=false;
    (void)identity_stereo_candidate(NativeOpticalApi{project,cluster_solve,clutter_detect},model(),pair(base),base);
    check(cluster_solved);
    OpticalComputeBudget budget;
    check(OpticalComputeBudget::start_solve());check(OpticalComputeBudget::start_solve());
    check(!OpticalComputeBudget::start_solve());
}
void raw_measured_pair(){
    Fixture f;
    int bridges=0,heads=0;
    OpticalCameraClock camera({1,"boot","CLOCK_MONOTONIC_RAW","synthetic changing bridge",true,0,500000},
        "boot",[&]{++bridges;return RawBridge{f.ns,f.ns+bridges,f.ns};});
    OpticalExposureSchedule schedule(camera,f.clock,261700);
    OpticalPhaseVerification phase(261700,{chip},true);
    OpticalWorker worker(chip,model(),api,"live",camera,schedule,phase,f.s,[&]{return f.ns;},
        [](const OpticalPoseFrame&,int64_t){return true;});
    auto p=pair(0);
    const double raw=double(f.ns-100000000)/1e9;
    check(!worker.try_measured_exposure(*p,{CameraMetadata{raw,.0008},CameraMetadata{raw,.0008}},
        [](int64_t)->std::optional<Rigid>{return {};}));
    check(worker.head_drops()==1);bridges=0;
    (void)worker.measured_exposure(*p,{CameraMetadata{raw,.0008},CameraMetadata{raw,.0008}},
        [&](int64_t stamp){++heads;check(stamp==f.ns-100000000+1);return identity_rigid();});
    check(heads==2&&bridges==2); // pair once, independent exposure schedule once
    p->frames[0].stamp=1;
    rejects([&]{worker.measured_exposure(*p,{CameraMetadata{raw,.0008},CameraMetadata{raw,.0008}},
        [](int64_t){return identity_rigid();});});
}
}
int main(int argc,char** argv) {
#ifdef _WIN32
    if(!std::getenv("PICO_IDENTITY_EPISODES")) _putenv_s("PICO_IDENTITY_EPISODES", "1");
#else
    ::setenv("PICO_IDENTITY_EPISODES", "1", 0);
#endif
    try {
        if(argc==2 && std::string(argv[1])=="--missing-gyro-regression-only") {missing_gyro_regression(false);std::cout<<"missing gyro worker PASS\n";return 0;}
        if(argc==2 && std::string(argv[1])=="--tilt-regression-only") {missing_gyro_regression(true);std::cout<<"tilt worker PASS\n";return 0;}
        unlocked_slow_solve(); unlocked_slow_solve(true); unlocked_slow_solve(false,true); clutter_ring();
        if(argc==2 && std::string(argv[1])=="--lock-regression-only"){std::cout<<"optical lock/cluster "<<checks<<" checks PASS (synthetic ABI)\n";return 0;}
         live_entry_cadence(); raw_measured_pair(); replay(false); replay(true); gyro_flip_regression(); missing_gyro_regression(false); missing_gyro_regression(true); std::cout<<"optical worker "<<checks<<" checks PASS (synthetic ABI/session replay)\n"; }
    catch(const std::exception &e) { std::cerr<<e.what()<<"\n"; return 1; }
}
