#include "native_test_runtime.hpp"
#include "optical_worker.hpp"
#include "fake_openocd.hpp"
#include "lower_map.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
using namespace pico;
namespace {
size_t checks=0;
void check(bool b,int line=__builtin_LINE()) { ++checks; if(!b) throw std::runtime_error("lower check "+std::to_string(checks)+" line "+std::to_string(line)); }
constexpr uint32_t chip=0xA1B2C3D4;
constexpr int64_t base=3000000000LL, period=33333333;
// Synthetic lower camera (640x480) looking +z from the head; the tracker is 0.5 m in front of it.
const OpticalCamera lower_camera={400,400,320,240,{0,0,0,0},640,480};
int project(const OpticalCamera *c,const double *p,size_t n,double *out) {
    for(size_t i=0;i<n;++i) { if(p[3*i+2]<=0) return 1;
        out[2*i]=c->cx+c->fx*p[3*i]/p[3*i+2]; out[2*i+1]=c->cy+c->fy*p[3*i+1]/p[3*i+2]; }
    return 0;
}
// Asymmetric 3D constellation (mono_solver_tests): a planar grid would be mono-ambiguous.
LedModel model() {
    LedModel m={-.041,-.02,.003,.032,-.025,-.006,.048,.014,.009,-.026,.036,-.012,.006,.044,.018,-.046,.008,.022,.018,-.008,.031,.003,.012,-.026};
    for(int i=0;i<8;++i) m[3*i+2]*=4;
    return m;
}
std::array<double,16> lower_blobs{};
void init_blobs() {
    const auto m=model(); double xyz[24];
    for(int i=0;i<8;++i) { xyz[3*i]=m[3*i]; xyz[3*i+1]=m[3*i+1]; xyz[3*i+2]=m[3*i+2]+.5; }
    check(project(&lower_camera,xyz,8,lower_blobs.data())==0);
}
// pixels[0]: 0 = no blob; upper nonzero = four stereo blobs; lower 1 = the eight LEDs, 3 = only three.
int detect(const uint8_t *image,int h,int w,int threshold,int min_area,int max_area,double *out,int cap) {
    check(image && threshold==180 && min_area==2 && max_area==250 && cap>=8);
    if(!image[0]) return 0;
    if(h==1024) {
        check(w==1056);
        for(int i=0;i<4;++i) { out[5*i]=520+i*5; out[5*i+1]=500+i*3; out[5*i+2]=1; out[5*i+3]=255; out[5*i+4]=2; }
        return 4;
    }
    check(h==480 && w==640);
    const int n=image[0]==3?3:8;
    for(int i=0;i<n;++i) { out[5*i]=lower_blobs[2*i]; out[5*i+1]=lower_blobs[2*i+1]; out[5*i+2]=1; out[5*i+3]=255; out[5*i+4]=2; }
    return n;
}
// Stereo fake: same world position as the lower view sees, zero covariance estimate.
int solve(const double *m,const OpticalView *views,const double *prior,OpticalFit *fit) {
    check(m && views[0].count==4 && views[1].count==4);
    auto t=identity_rigid(); t[3]=views[0].head_from_camera[3]; t[11]=.5;
    std::copy(t.begin(),t.end(),fit->pose); fit->count=4;
    return 0;
}
NativeOpticalApi api{project,solve,detect};
std::shared_ptr<IdentityPair> pair(int64_t stamp,bool upper_lit,uint8_t lower_value,bool with_lower=true) {
    auto p=std::make_shared<IdentityPair>(); p->timestamp_matched=true; p->direct_capture=true;
    for(size_t i=0;i<2;++i) {
        auto &f=p->frames[i]; f.serial=i?"CAMUPPERRIGHT":"CAMUPPERLEFT";
        f.metadata="synthetic upper "+std::to_string(stamp)+std::to_string(i);
        f.timestamp_matched=true; f.stamp=stamp; f.exposure_us=800;
        f.view.camera={200,200,528,512,{0,0,0,0},1056,1024};
        f.world_from_head[3]=.25;
        f.pixels.resize(1056u*1024u); f.pixels[0]=upper_lit?255:0;
    }
    if(with_lower) for(size_t i=0;i<2;++i) {
        IdentityExposure f; f.serial=i?"CAMLOWERRIGHT":"CAMLOWERLEFT";
        f.metadata="synthetic lower "+std::to_string(stamp)+std::to_string(i);
        f.timestamp_matched=true; f.stamp=stamp+50000; f.source_raw=double(stamp+50000)/1e9;
        f.exposure_us=782.267; f.width=640; f.height=480;
        f.view.camera=lower_camera; f.world_from_head[3]=.25;
        f.pixels.resize(640u*480u); f.pixels[0]=i?0:lower_value;  // only the first lower view sees the tracker
        p->lower.push_back(std::move(f));
    }
    return p;
}
void preference() {
    auto p=pair(base,false,1);
    IdentityCandidate none,stereo,upper,lower,lower2;
    stereo.pair=p; stereo.stereo_verified=true; stereo.residual=2;
    upper.pair=p; upper.mono=true; upper.mono_view=0; upper.residual=1;
    lower.pair=p; lower.mono=true; lower.lower_view=0; lower.residual=.5;
    lower2=lower; lower2.lower_view=1; lower2.residual=.1;
    check(identity_candidate_rank(none)==3 && identity_candidate_rank(stereo)==0);
    check(identity_candidate_rank(upper)==1 && identity_candidate_rank(lower)==2);
    // Category first (upper stereo > upper mono > lower mono), residual only within a category.
    check(identity_candidate_better(stereo,upper) && !identity_candidate_better(upper,stereo));
    check(identity_candidate_better(upper,lower) && !identity_candidate_better(lower,upper));
    check(identity_candidate_better(lower2,lower) && !identity_candidate_better(lower,lower2));
    check(identity_candidate_better(lower,none) && !identity_candidate_better(none,lower));
    lower.residual=0; check(identity_candidate_better(upper,lower));
}
void lower_candidate() {
    auto p=pair(base,false,1);
    check(!identity_stereo_candidate(api,model(),p,base).pair);  // upper views see nothing
    auto c=identity_lower_candidate(api,model(),p,base);
    check(c.pair==p && c.mono && !c.stereo_verified && c.lower_view==0 && c.mono_view==-1 && c.inliers==8);
    check(std::abs(c.world[3]-.25)<.002 && std::abs(c.world[7])<.002 && std::abs(c.world[11]-.5)<.003 && c.residual<.1);
    check(identity_candidate_rank(c)==2 && c.stamp==base);
    check(identity_observation(p,0,c,base).world.has_value());
    // Provenance: out-of-range lower view, an upper view as well, or a stereo claim is dropped.
    auto d=c; d.lower_view=2; check(identity_observation(p,0,d,base).stamp==0);
    d=c; d.mono_view=0; check(identity_observation(p,0,d,base).stamp==0);
    d=c; d.stereo_verified=true; (void)identity_observation(p,0,d,base);  // a stereo flag on a lower candidate is only provenance noise now
    // Three blobs in the visible lower view, none in the other: no fit, two logged drops.
    const auto sparse=optical_drops("Four measured blobs per lower view required");
    check(!identity_lower_candidate(api,model(),pair(base,false,3),base).pair);
    check(optical_drops("Four measured blobs per lower view required")==sparse+2);
    check(!identity_lower_candidate(api,model(),pair(base,false,1,false),base).pair);
}
void lower_validity() {
    for(int mutation=0;mutation<=9;++mutation) {
        auto p=pair(base,false,1); auto &l=p->lower[0];
        switch(mutation) {
        case 1:l.pixels.pop_back();break; case 2:l.width=1056;break; case 3:l.stamp=base+5000001;break;
        case 4:p->direct_capture=false;break; case 5:l.serial="CAMUPPERLEFT";break; case 6:l.view.camera.width=1056;break;
        case 7:l.exposure_us=0;break; case 8:l.world_from_head[0]=2;break; case 9:l.stamp=base-5000000;break;
        }
        const bool valid=mutation==0 || mutation==9;
        check(identity_lower_valid(*p,l)==valid);
        // At identity level a bad lower frame invalidates the pair; the worker removes it first (logged).
        check((identity_observation(p,0,{},base).stamp!=0)==valid);
    }
}
LedEpisode episode(unsigned token=1,int64_t start=base) {
    LedEpisode e; e.token=token; e.chip=chip;
    e.off_tags={2}; e.on_tags={4}; e.off_published=start+100000000; e.off_ack=start+100000000;
    e.on_published=start+350000000; e.on_ack=start+360000000; return e;
}
// Upper cameras turned away (tracker behind them): only the lower view can prove the dark frames.
bool hidden_episode(bool with_lower,bool bright) {
    NativeLedModeIdentity m(chip,model(),api); m.configure({chip},"live");
    for(int i=0;i<=14;++i) {
        auto p=pair(base+i*period,false,0,with_lower);
        for(auto &f:p->frames) { f.view.head_from_camera=identity_rigid(); f.view.head_from_camera[5]=f.view.head_from_camera[10]=-1; }
        if(bright && i==7) p->lower[0].pixels[240*640+320]=255;
        std::optional<IdentityCandidate> c;
        if(i<4 || i>=11) {
            IdentityCandidate lit; lit.pair=p; lit.stamp=p->frames[0].stamp; lit.inliers=8; lit.world[3]=.25; lit.world[11]=.5;
            if(with_lower) { lit.mono=true; lit.lower_view=0; } else lit.stereo_verified=true;
            for(size_t k=0;k<2;++k) { lit.metadata[k]=p->frames[k].metadata; lit.stamps[k]=p->frames[k].stamp; lit.heads[k]=p->frames[k].world_from_head; }
            c=lit;
        }
        check(m.observe(identity_observation(p,i,c,p->frames[0].stamp)));
    }
    return m.evaluate(episode());
}
void lower_dark_identity() {
    check(hidden_episode(true,false));    // dark evidence from the lower view alone
    check(!hidden_episode(false,false));  // outside every upper view and no lower frame: unproven
    check(!hidden_episode(true,true));    // lit pixel inside the lower ROI on a dark frame
}
struct Fixture {
    int64_t ns=base; double seconds=double(base)/1e9;
    FakeOpenOcd fake;
    DebugMemory debug{[this](const std::string &s){return fake.command(s);}};
    MailboxWriter writer{debug};
    PeerEngine e{le32(chip),0,[this]{return seconds;},[]{return Bytes(16,7);}};
    RoyClock clock;
    NativeIdentitySession s{chip,model(),"live",api};
    Native25AckBridge bridge{s.owner,e,clock,"live",[this]{return ns;}};
    OpticalWorker *worker=nullptr;
    bool upper_lit=false;
    Fixture() { e.state=PeerState::Streaming; e.mode_required=true; e.mode_profile="frame"; e.mode_confirmed_at=seconds; e.last_reply=seconds; }
    void time(int64_t t) { ns=t; seconds=double(t)/1e9; e.last_reply=seconds; e.mode_confirmed_at=seconds; }
    void ack() {
        auto g=e.pending->generation; Record r; r.hardware_crc=true; r.channel=2; r.profile=0;
        r.chip=chip; r.generation=g; r.dma={4,2,0,0,0,0}; clock.fit=Fit{0,double(ns),15.625,0};
        bool accepted=false;
        bridge.record(r,[&]{ accepted=e.receive(Bytes(r.dma.begin()+2,r.dma.end()),2,uint16_t(g),true); }); check(accepted);
    }
    // Live measured path: stamps converted by the worker, lower frames by their source offset.
    void frame(int i,int64_t start) {
        auto stamp=start+i*period; time(stamp);
        const bool lit=i<4 || i>=11;
        auto p=pair(stamp,lit && upper_lit,lit?1:0);
        for(auto &f:p->frames) f.stamp=0;
        for(auto &f:p->lower) f.stamp=0;
        auto raw=double(stamp)/1e9;
        check(worker!=nullptr);
        worker->measured_exposure(*p,{CameraMetadata{raw,.0008},CameraMetadata{raw,.0008}},
            [](int64_t){auto head=identity_rigid();head[3]=.25;return head;});
    }
    void run_episode(int64_t start) {
        for(int i=0;i<3;++i) frame(i,start);
        time(start+100000000); check(s.begin_if_ready(e,"live",ns,true));
        check(bridge.publish(writer,0,e.mailbox())); ack();
        time(start+350000000); s.radio_tick(e,"live",ns);
        check(e.pending && e.pending->body==Bytes{2});
        check(bridge.publish(writer,0,e.mailbox()));
        time(start+360000000); ack(); check(s.owner.restore_required());
        for(int i=3;i<=14;++i) frame(i,start);
        time(start+14*period); s.radio_tick(e,"live",ns);
        check(!s.owner.restore_required() && !e.native25_exclusive);
    }
};
// Full body: the tracker is seen only by a lower camera; identity, phase and publication use the lower mono fit.
void full_body() {
    Fixture f;
    OpticalCameraClock camera({1,"boot","CLOCK_MONOTONIC","synthetic",true,0,0},"boot");
    OpticalExposureSchedule schedule(camera,f.clock,261700);
    OpticalPhaseVerification phase(261700,{chip},true);
    size_t publications=0;
    OpticalPoseFrame last;
    OpticalWorker worker(chip,model(),api,"live",camera,schedule,phase,f.s,[&]{return f.ns;},
        [&](const OpticalPoseFrame &pose,int64_t){ last=pose; ++publications; return true; });
    f.worker=&worker;
    phase.ack(chip,261700);
    f.run_episode(base);
    f.run_episode(base+1000000000LL);
    check(f.s.bindings_snapshot());  // identity bound with no upper blob at all
    auto plan=schedule.prepare(0,1,f.ns,0,chip);
    check(bool(plan) && plan->light_due);
    schedule.light_ack(chip,1); schedule.phase_ack(chip,f.ns);
    for(int i=900;i<=960;++i) f.frame(14,base+(i-14)*period);
    check(publications>=1);
    check(std::abs(last.position[0]-.25)<.002 && std::abs(last.position[1])<.002 && std::abs(last.position[2]-.5)<.003);
    // Mono covariance is stretched along the ray from the LOWER camera (origin x=.25, looking +z).
    auto cov=worker.position_covariance();
    check(cov[8]>3*cov[0] && std::abs(cov[0]-cov[4])<.01*cov[0]);
    // Upper stereo of the same exposure wins over the lower mono: isotropic stereo covariance.
    const auto before=publications;
    f.upper_lit=true; f.frame(14,base+(961-14)*period);
    cov=worker.position_covariance();
    check(publications==before+1 && cov[0]==.003*.003 && cov[8]==.003*.003);
    f.upper_lit=false; f.frame(14,base+(962-14)*period);
    cov=worker.position_covariance();
    check(publications==before+2 && cov[8]>3*cov[0]);
    // A lower frame from another exposure is removed (logged); the upper pair still counts.
    auto p=pair(f.ns+period,false,1);
    for(auto &l:p->lower) { l.stamp=0; l.source_raw+=.006; }
    for(auto &u:p->frames) u.stamp=0;
    f.time(f.ns+period);
    const auto dropped=optical_drops("Lower frame rejected (size/stamp/view)");
    auto raw=double(f.ns)/1e9;
    (void)worker.measured_exposure(*p,{CameraMetadata{raw,.0008},CameraMetadata{raw,.0008}},
        [](int64_t){auto head=identity_rigid();head[3]=.25;return head;});
    check(optical_drops("Lower frame rejected (size/stamp/view)")==dropped+2);
}
// lower_map verdict: decisive timing or brightness, never both disagreeing, enough matches.
void mapping_decision() {
    using namespace pico::lower_map;
    std::array<std::array<Evidence,2>,2> e{};
    for(auto &row:e) for(auto &x:row) { x.matched=100; x.jitter_us=50; x.score=.1; }
    check(decide(e).mapping==-1);
    e[0][0].jitter_us=e[1][1].jitter_us=5; check(decide(e).mapping==0 && decide(e).basis=="timing");
    e[0][1].score=e[1][0].score=.6; check(decide(e).mapping==-1);  // brightness says swapped
    e[0][0].jitter_us=e[1][1].jitter_us=50; check(decide(e).mapping==1 && decide(e).basis=="brightness");
    e[1][0].matched=49; check(decide(e).mapping==-1);
    check(std::abs(pearson({1,2,3,4},{2,4,6,8})-1)<1e-12 && std::isnan(pearson({1,1,1},{1,2,3})));
}
}
int main() {
    ::setenv("PICO_IDENTITY_EPISODES", "1", 0);
    try {
        init_blobs(); preference(); lower_candidate(); lower_validity(); lower_dark_identity(); mapping_decision(); full_body();
        std::cout<<"lower camera "<<checks<<" checks PASS (synthetic ABI)\n";
    } catch(const std::exception &e) { std::cerr<<e.what()<<"\n"; return 1; }
}
