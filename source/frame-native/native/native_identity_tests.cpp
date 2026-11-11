#include "native_test_runtime.hpp"
#include "native_identity.hpp"
#include "query_peer.hpp"
#include "fake_openocd.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <future>
#include <chrono>
using namespace pico;
namespace {
size_t checks=0;
void check(bool b,int line=__builtin_LINE()) { ++checks; if(!b) throw std::runtime_error("identity check "+std::to_string(checks)+" line "+std::to_string(line)); }
template<class F> void rejects(F f,int line=__builtin_LINE()) { bool rejected=false; try { f(); } catch(const std::exception &) { rejected=true; } check(rejected,line); }
constexpr uint32_t chip=0xA1B2C3D4;
constexpr int64_t base=3000000000LL, period=33333333;
// Explicit synthetic ABI doubles; no claim of Windows .so execution.
int project(const OpticalCamera *c,const double *p,size_t n,double *out) {
    for(size_t i=0;i<n;++i) { if(p[3*i+2]<=0) return 1;
        out[2*i]=c->cx+c->fx*p[3*i]/p[3*i+2]; out[2*i+1]=c->cy+c->fy*p[3*i+1]/p[3*i+2]; }
    return 0;
}
int project_error(const OpticalCamera *,const double *,size_t,double *) { return 1; }
std::promise<void> *project_entered=nullptr;
std::shared_future<void> project_release;
std::atomic<bool> project_first{false};
int project_blocked(const OpticalCamera *c,const double *p,size_t n,double *out) {
    if(!project_first.exchange(true)) { project_entered->set_value(); project_release.wait(); }
    return project(c,p,n,out);
}
int detect(const uint8_t *image,int h,int w,int threshold,int min_area,int max_area,double *out,int cap) {
    check(h==1024 && w==1056 && threshold==180 && min_area==2 && max_area==250 && cap>=4 && image);
    check(image[700*w+1040]==17 && image[1040*1024+700]==0);
    for(int i=0;i<4;++i) { out[5*i]=520+i*5; out[5*i+1]=500+i*3; out[5*i+2]=1; out[5*i+3]=255; out[5*i+4]=2; }
    return 4;
}
int solve(const double *model,const OpticalView *views,const double *prior,OpticalFit *fit) {
    check(model && !prior && views[0].count==4 && views[1].count==4);
    check(views[0].camera.width==1056 && views[0].camera.height==1024 && views[1].camera.width==1056 && views[1].camera.height==1024 && views[1].pixels[0]==520);
    auto t=identity_rigid(); t[11]=1; std::copy(t.begin(),t.end(),fit->pose); fit->count=4; return 0;
}
int detect_three(const uint8_t *,int,int,int,int,int,double *out,int) {
    for(int i=0;i<3;++i) { out[5*i]=520+i*5; out[5*i+1]=500; out[5*i+2]=1; out[5*i+3]=255; out[5*i+4]=2; }
    return 3;
}
int solve_rejected(const double *,const OpticalView *,const double *,OpticalFit *) { return 1; }
int solve_three(const double*,const OpticalView* views,const double* prior,OpticalFit* fit) {
    check(prior && views[0].count==3 && views[1].count==3);
    std::copy(prior,prior+16,fit->pose);fit->count=3;fit->continuation=1;return 0;
}
int blind_status=0;
int solve_symmetric(const double*,const OpticalView*,const double* prior,OpticalFit* fit) {
    if(!prior && blind_status)return blind_status;
    auto pose=identity_rigid();
    if(prior)std::copy(prior,prior+16,pose.begin());
    else {pose[0]=-1;pose[5]=-1;}
    std::copy(pose.begin(),pose.end(),fit->pose);fit->count=4;fit->continuation=prior?1:0;return 0;
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
IdentityCandidate candidate(const std::shared_ptr<const IdentityPair> &p) {
    IdentityCandidate c; c.pair=p; c.stamp=p->frames[0].stamp; c.inliers=4; c.stereo_verified=true; c.world[11]=1;
    for(size_t i=0;i<2;++i) { c.metadata[i]=p->frames[i].metadata; c.stamps[i]=p->frames[i].stamp; c.heads[i]=p->frames[i].world_from_head; }
    return c;
}
LedEpisode episode(unsigned token=1,int64_t start=base) {
    LedEpisode e; e.token=token; e.chip=chip;
    e.off_tags={2}; e.on_tags={4}; e.off_published=start+100000000; e.off_ack=start+100000000;
    e.on_published=start+350000000; e.on_ack=start+360000000; return e;
}
void populate(NativeLedModeIdentity &m,int64_t start=base,int64_t first_index=0,int mutation=0) {
    for(int i=0;i<=14;++i) {
        if(mutation==1 && i==7) continue;
        auto p=pair(start+i*period);
        bool lit=i<4 || i>=11;
        std::optional<IdentityCandidate> c=lit?std::optional<IdentityCandidate>(candidate(p)):std::nullopt;
        if(mutation==2 && i==7) p->frames[1].pixels[512*1056+528]=255;
        if(mutation==3 && c && i==2) c->world[3]=.15;
        if(mutation==4 && c) c->world[3]=2.*double(i*period)/1e9;
        if(mutation==5 && c) c->stereo_verified=false;
        if(mutation==12 && c) { c->stereo_verified=false; c->mono=true; c->mono_view=0; }
        if(mutation==6 && i==7) { c=candidate(p); }
        if(mutation==7) { for(auto &f:p->frames) f.world_from_head[3]=100; }
        if(mutation==7 && c) c=candidate(p);
        if(mutation==8 && i==7) p->frames[0].pixels.assign(1056u*1024u,255); // constant bright has ZERO contrast
        if((mutation==9 || mutation==10) && i==7) p->frames[1].pixels[512*1056+528]=mutation==9?30:31;
        if(mutation==11) {
            for(auto &f:p->frames) { f.world_from_head[3]=1000000; f.world_from_head[12]=.000001; }
            if(c) c=candidate(p); // passes rigid tolerance but full inverse is singular
        }
        // An unverified lit candidate drops that pair (logged); it is never admitted.
        const bool admitted=m.observe(identity_observation(p,first_index+i,c,p->frames[0].stamp));
        check(admitted || (mutation==5 && c));
    }
}
void admission_tests() {
    auto p=pair(base); auto c=candidate(p);
    auto o=identity_observation(p,0,c,base+250000000); check(o.world.has_value());
    p->frames[0].pixels[0]=255; check(o.pair.frames[0].pixels[0]==0); p->frames[0].pixels[0]=0;
    for(int mutation=0;mutation<25;++mutation) {
        auto q=pair(base); auto d=candidate(q); int64_t now=base, index=0;
        switch(mutation) {
        case 0:q->timestamp_matched=false;break; case 1:q->frames[0].timestamp_matched=false;break;
        case 2:q->frames[0].serial=q->frames[1].serial;break; case 3:q->frames[0].pixels.pop_back();break;
        case 4:q->frames[0].width=1055;break; case 5:q->frames[1].exposure_us=749;break;
        case 6:q->frames[1].exposure_us=851;break; case 7:q->clock_error=500001;break;
        case 8:q->clock_error=-1;break; case 9:q->frames[1].stamp+=2001;now+=2001;break;
        case 10:now+=250000001;break; case 11:now-=1;break;
        case 12:q->frames[0].world_from_head[0]=2;break; case 13:d.world[0]=-1;break;
        case 14:d.stereo_verified=false;break; case 15:d.inliers=3;break;
        case 16:d.pair=p;break; case 17:d.metadata[0]+="changed";break;
        case 18:d.heads[0][3]=1;break; case 19:d.stamp+=2001;break; case 20:index=-1;break;
        case 23:std::swap(q->frames[0].width,q->frames[0].height);break; case 24:std::swap(q->frames[1].view.camera.width,q->frames[1].view.camera.height);break;
        case 21:q->frames[1].view.camera.width=1055;break; case 22:q->frames[0].view.pixel_offset={.25,.25};break;
        }
        if(mutation==20) { rejects([&]{identity_observation(q,index,d,now);}); continue; } // caller invariant
        // Measured-data failures drop the pair with one logged, counted reason.
        const auto stale=identity_stale_drops(), dropped=optical_drops();
        check(identity_observation(q,index,d,now).stamp==0);
        check(optical_drops()==dropped+1);
        check(identity_stale_drops()==stale+(mutation==10||mutation==11?1:0));
    }
    rejects([&]{identity_observation(nullptr,0,{},base);});
    p=pair(base); c=identity_stereo_candidate(api,model(),p,base); check(c.stereo_verified && c.inliers==4);
    check(identity_observation(p,0,c,base).world.has_value());
    p->frames[0].exposure_us=750; p->frames[1].exposure_us=850; p->clock_error=500000;
    check(!identity_observation(p,0,{},base).world);
    p->frames[1].stamp+=2000; check(identity_observation(p,0,{},base+2000).stamp==base);
    auto direct=pair(base);direct->direct_capture=true;
    for(auto& frame:direct->frames)frame.exposure_us=37.233;
    direct->frames[1].stamp+=1000000;
    check(identity_observation(direct,0,{},base+1000000).stamp==base);
    direct->frames[1].stamp+=1;
    check(identity_observation(direct,0,{},base+1000001).stamp==0);
    { // A lens-rim glint outside the KB domain must not fail the whole view (2026-10-30, r13-style rc=3).
        OpticalCamera c{100,100,0,0,{-0.1,0,0,0},1056,1024};
        check(identity_pixel_in_ray_domain(c,100,0));
        check(!identity_pixel_in_ray_domain(c,150,0));
    }
    // Fewer than four blobs: no fit, logged and counted, never thrown.
    const auto sparse=optical_drops("Four measured blobs per stereo view required");
    auto none=identity_stereo_candidate({project,solve,detect_three},model(),pair(base),base);
    check(!none.stereo_verified && optical_drops("Four measured blobs per stereo view required")==sparse+1);
    const auto seed=identity_rigid();
    auto three=identity_stereo_candidate({project,solve_three,detect_three},model(),pair(base),base,seed.data());
    check(three.stereo_verified && three.inliers==3 && three.prior_continuation);
    check(identity_observation(three.pair,0,three,base).world.has_value());
    three.prior_continuation=false;
    check(!identity_observation(three.pair,0,three,base).stamp);
    for(int status:{0,11,12}) {
        blind_status=status;
        auto recovered=identity_stereo_candidate({project,solve_symmetric,detect},model(),pair(base),base,seed.data());
        check(recovered.stereo_verified && recovered.world==seed && recovered.prior_continuation);
    }
    const auto rejected=optical_drops("Native stereo solve rejected");
    none=identity_stereo_candidate({project,solve_rejected,detect},model(),pair(base),base);
    check(!none.stereo_verified && optical_drops("Native stereo solve rejected")==rejected+1);
    auto invalid=model(); invalid[0]=std::numeric_limits<double>::quiet_NaN();
    rejects([&]{ NativeLedModeIdentity m(chip,invalid,api); });
    rejects([&]{ NativeLedModeIdentity m(0,model(),api); });
    rejects([&]{ NativeLedModeIdentity m(chip,model(),{}); });
}
void contrast_tests() {
    std::vector<uint8_t> p(1056u*1024u,0);
    p[0]=255; check(identity_box_mean11(p,0,0)==76); // replicated ORIGINAL border: 36 copies
    p.assign(p.size(),255); check(identity_box_mean11(p,528,512)==255);
    p.assign(p.size(),0); p[512*1056+527]=255;
    check(identity_box_mean11(p,528,512)==2); // neighborhood outside hypothetical ROI
    p[512*1056+528]=30; check(p[512*1056+528]<=identity_box_mean11(p,528,512)+30);
    p.assign(p.size(),0);p[1023*1056+1055]=255;
    check(identity_box_mean11(p,1055,1023)==76); // bottom-right, replicated border
    p.assign(p.size(),0);p[700*1056+1040]=255;
    check(identity_box_mean11(p,1040,700)==2); // x beyond height, asymmetric row/column
    rejects([&]{ identity_box_mean11(p,1024,1055); });
    rejects([&]{ identity_box_mean11(p,-1,0); });
}
void matcher_tests() {
    for(int mutation=0;mutation<=12;++mutation) {
        NativeLedModeIdentity m(chip,model(),api); m.configure({chip},"live");
        populate(m,base,0,mutation);
        check(m.evaluate(episode())==(mutation==0 || mutation==8 || mutation==9 || mutation==12));
        check(!m.bound()); check(!m.verified(chip,14,"live",base+14*period));
    }
    NativeLedModeIdentity bad(chip,model(),{project_error,solve,detect}); bad.configure({chip},"live");
    populate(bad); check(!bad.evaluate(episode()));
    NativeLedModeIdentity m(chip,model(),api); m.configure({chip},"live"); populate(m);
    check(m.evaluate(episode())); check(!m.evaluate(episode()));
    populate(m,base+1000000000LL,30); check(m.evaluate(episode(2,base+1000000000LL))); check(m.bound());
    auto last=base+1000000000LL+14*period;
    check(m.verified(chip,44,"live",last)); check(m.verified(chip,44,"live",last+250000000));
    check(!m.verified(chip,44,"live",last+250000001)); check(!m.verified(chip,44,"live",last-1));
    check(!m.verified(chip,43,"live",last)); check(!m.verified(chip+1,44,"live",last));
    check(!m.verified(chip,44,"new",last));
    m.configure({chip},"live"); check(m.bound());
    m.configure({chip},"new"); check(!m.bound() && m.observations.empty() && m.confirmations.empty());
    m.configure({chip,chip+1},"new"); check(!m.observe(identity_observation(pair(base),0,{},base)));
    rejects([&]{m.configure({chip},"");});
    for(int count:{47,48}) for(bool identity:{false,true}) {
        IdentityPhaseWindow w;
        w.observe(900,true,true,false); check(w.state==IdentityPhaseWindow::State::Waiting);
        for(int i=0;i<61;++i) w.observe(900+i,i<count,identity,true);
        const bool locked=count==48 && identity;
        // A failed window is logged and retried; Exhausted is never entered.
        check(w.state==(locked?IdentityPhaseWindow::State::Locked:IdentityPhaseWindow::State::Verifying));
        check(w.retries()==(locked?0u:1u));
        w.observe(2000,true,true,true); check(w.state==(locked?IdentityPhaseWindow::State::Locked:IdentityPhaseWindow::State::Verifying));
        for(int i=2001;i<=2060;++i) w.observe(i,true,true,true);
        check(w.state==IdentityPhaseWindow::State::Locked);
    }
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
    void frame(int i,int64_t start,int64_t offset=0) {
        auto p=pair(start+i*period); std::optional<IdentityCandidate> c;
        if(i<4 || i>=11) c=candidate(p);
        check(s.optical(p,offset+i,c,p->frames[0].stamp));
    }
    void run_episode(int64_t start,int64_t offset) {
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
void session_tests() {
    {
        Fixture f;
        for (int i = 0; i < 3; ++i) f.frame(i, base);
        f.time(base + 100000000);
        check(f.s.begin_if_ready(f.e, "live", f.ns, true));
        check(f.bridge.publish(f.writer, 0, f.e.mailbox()));
        f.ack();
        f.time(base + 350000000);
        f.s.radio_tick(f.e, "live", f.ns);
        check(f.bridge.publish(f.writer, 0, f.e.mailbox()));
        f.time(base + 360000000);
        f.ack();
        for (int i = 3; i <= 14; ++i) f.frame(i, base);
        f.s.begin_recovery(); // full measured token was already produced
        f.time(base + 14 * period);
        check(!f.s.recovery_tick(f.e, "live", f.ns));
        check(!f.s.owner.restore_required());
        check(!f.s.bindings_snapshot());
    }
    // Cleanup evidence is independent of identity admission, but uses exactly
    // the Python episode/restore predicate (including real dark pixels).
    for (int fault = 0; fault < 5; ++fault) {
        Fixture f;
        for (int i = 0; i < 3; ++i) f.frame(i, base);
        f.time(base + 100000000);
        check(f.s.begin_if_ready(f.e, "live", f.ns, true));
        check(f.bridge.publish(f.writer, 0, f.e.mailbox()));
        f.ack();
        f.s.begin_recovery();
        check(!f.s.bindings_snapshot());
        check(!f.s.begin_if_ready(f.e, "live", f.ns, true));
        f.time(base + 350000000);
        check(f.s.recovery_tick(f.e, "live", f.ns));
        check(f.bridge.publish(f.writer, 0, f.e.mailbox()));
        f.time(base + 360000000);
        if (fault != 1) f.ack();
        for (int i = 3; i <= 14; ++i) {
            if (fault == 2 && i == 7) continue; // missing dark exposure
            auto p = pair(base + i * period);
            if (fault == 3 && i == 7) p->frames[0].pixels[512 * 1056 + 528] = 255;
            std::optional<IdentityCandidate> c;
            if (i < 4 || i >= 11) c = candidate(p);
            check(f.s.recovery_optical(p, i, c, p->frames[0].stamp));
        }
        check(f.s.owner.restore_required()); // optical cannot retire protocol
        f.time(base + 14 * period);
        // fault 4: a lapsed mode at the measured lit return is logged; the visibly lit
        // LEDs still complete restoration (native25 owner rule).
        if (fault == 4) f.e.mode_confirmed_at.reset();
        f.s.recovery_tick(f.e, "live", f.ns);
        check(f.s.owner.restore_required() == (fault == 1 || fault == 2 || fault == 3));
        check(!f.s.bindings_snapshot());
        check(!f.s.verified(chip, 14, "live", f.ns));
    }
    // The unified owner uses the same episode driver as the installed wrapper.
    { Fixture ready;
      for(int i=0;i<3;++i)ready.frame(i,base);
      ready.time(base+100000000);
      ready.e.queued.push_back({14,{1,1,0}});
      check(!ready.s.drive_radio(ready.e,"live",ready.ns));
      check(!ready.s.owner.restore_required());
      ready.e.queued.clear();
      check(ready.s.drive_radio(ready.e,"live",ready.ns));
      check(ready.e.native25_exclusive && ready.s.owner.restore_required()); }
    { Fixture absent;
      absent.time(base+100000000);
      check(!absent.s.drive_radio(absent.e,"live",absent.ns));
      check(!absent.e.native25_exclusive && !absent.s.owner.restore_required()); }
    Fixture f; check(!f.s.begin_if_ready(f.e,"live",base,true));
    f.run_episode(base,0); check(!f.s.verified(chip,14,"live",f.ns));
    f.run_episode(base+1000000000LL,30);
    check(f.s.owner.completed()==2 && f.s.bindings_snapshot());
    check(f.s.verified(chip,44,"live",f.ns)); check(!f.s.verified(chip,43,"live",f.ns));
    f.s.invalidate(); check(!f.s.bindings_snapshot() && !f.s.verified(chip,44,"live",f.ns));
    check(!f.s.begin_if_ready(f.e,"live",f.ns,true));
    Fixture g;
    for(int i=0;i<3;++i) g.frame(i,base);
    check(!g.s.begin_if_ready(g.e,"wrong",base+100000000,true));
    check(!g.s.begin_if_ready(g.e,"live",base+2*period+250000001,true));
    auto malformed=pair(base+3*period); malformed->frames[0].pixels.clear();
    check(!g.s.optical(malformed,3,{},base+3*period));
    check(!g.s.begin_if_ready(g.e,"live",base+100000000,true));
    Fixture h; for(int i=0;i<3;++i) h.frame(i,base);
    h.time(base+100000000); check(h.s.begin_if_ready(h.e,"live",h.ns,true));
    check(h.bridge.publish(h.writer,0,h.e.mailbox())); h.ack(); h.s.invalidate();
    h.time(base+350000000); h.s.radio_tick(h.e,"live",h.ns);
    check(h.s.owner.restore_required() && h.e.pending->body==Bytes{2});
    check(!h.s.optical(pair(h.ns),99,{},h.ns));
    // The overdue OFF is logged (no throw); the pending ON stays and the invalidated
    // session never releases the LED restoration.
    h.time(base+900000001); h.s.radio_tick(h.e,"live",h.ns);
    check(h.s.owner.restore_required() && h.e.pending && h.e.pending->body==Bytes{2});
    // An unverified mode only pauses new episodes: logged, not invalidated, resumes when verified.
    Fixture paused; for(int i=0;i<3;++i) paused.frame(i,base);
    paused.time(base+100000000); paused.e.mode_profile="stock";
    const auto mode_drops=optical_drops("Identity radio tick without verified frame mode");
    check(!paused.s.drive_radio(paused.e,"live",paused.ns));
    check(optical_drops("Identity radio tick without verified frame mode")==mode_drops+1);
    check(!paused.s.owner.restore_required() && !paused.s.owner.abort_requested);
    paused.e.mode_profile="frame";
    check(paused.s.drive_radio(paused.e,"live",paused.ns) && paused.s.owner.restore_required());
}
void hooks_tests() {
    double now=3; QueryHooks qh;
    qh.bench=[]{return BenchImuEvidence{};}; qh.fresh_guard=[]{}; qh.native_radio_tick=[]{return false;};
    qh.proof=[](unsigned,const Bytes &,const Page &){}; qh.action=[](uint8_t,const Bytes &){};
    qh.prerequisites=[](const std::vector<std::pair<unsigned,Page>> &,const std::string &){};
    qh.producer_lifetime=[](double){};
    QueryPeer e(le32(chip),0,[&]{return now;},[]{return Bytes(16,7);},qh);
    e.state=PeerState::Streaming; e.mode_required=true; e.mode_profile="frame"; e.mode_confirmed_at=now; e.last_reply=now;
    RoyClock clock;
    SingleTrackerNativeIdentityHooks hooks(e,clock,model(),"fresh-engine-token",api);
    check(!hooks.prepare(true)); e.diagnostic_index=22;
    check(!hooks.prepare(false)); check(hooks.prepare(true));
    hooks.configure({chip}); check(!hooks.bindings_snapshot());
    for(int i=0;i<3;++i) { auto p=pair(base+i*period); check(hooks.optical(chip,p,i,candidate(p),p->frames[0].stamp)); }
    check(hooks.radio_tick(base+100000000,true,true)); check(e.native25_exclusive && hooks.restore_required());
    // A failed controller/HMD guard or worker health is logged; restoration keeps ticking.
    const auto guard_drops=optical_drops();
    check(hooks.radio_tick(base+100000001,false,true) && hooks.restore_required());
    check(optical_drops()==guard_drops+1 && hooks.bindings_snapshot()==false);
    const auto wrong=optical_drops("Unexpected optical target; pair dropped");
    { auto p=pair(base+3*period); check(!hooks.optical(chip+1,p,3,{},p->frames[0].stamp)); }
    check(optical_drops("Unexpected optical target; pair dropped")==wrong+1);
    hooks.disconnect(); check(!hooks.bindings_snapshot() && hooks.restore_required());
    check(!hooks.prepare(true)); hooks.configure({chip,chip+1}); // logged, never thrown
    check(!hooks.verified(chip,2,base+100000000));
}
void concurrent_session_test() {
    std::promise<void> entered,release;
    project_entered=&entered; project_release=release.get_future().share(); project_first=false;
    Fixture f({project_blocked,solve,detect});
    for(int i=0;i<3;++i) f.frame(i,base);
    f.time(base+100000000); check(f.s.begin_if_ready(f.e,"live",f.ns,true));
    check(f.bridge.publish(f.writer,0,f.e.mailbox())); f.ack();
    f.time(base+350000000); f.s.radio_tick(f.e,"live",f.ns);
    check(f.bridge.publish(f.writer,0,f.e.mailbox())); f.time(base+360000000); f.ack();
    for(int i=3;i<14;++i) f.frame(i,base);
    // No check() in worker threads: keep test counter single-thread-owned.
    auto p=pair(base+14*period); auto c=candidate(p);
    auto optical=std::async(std::launch::async,[&]{return f.s.optical(p,14,c,p->frames[0].stamp);});
    const bool evaluating=entered.get_future().wait_for(std::chrono::seconds(2))==std::future_status::ready;
    f.time(base+14*period);
    auto radio=std::async(std::launch::async,[&]{f.s.radio_tick(f.e,"live",f.ns);});
    const bool independent=radio.wait_for(std::chrono::seconds(2))==std::future_status::ready;
    const bool retained=independent && f.s.owner.restore_required();
    release.set_value(); radio.get(); const bool accepted=optical.get();
    check(evaluating && independent && retained && accepted);
    check(f.s.owner.restore_required()); f.s.radio_tick(f.e,"live",f.ns); check(!f.s.owner.restore_required());
    project_entered=nullptr;
}
void real_library_tests() {
    rejects([]{NativeOpticalLibraries absent("missing-optics-library","missing-detector-library");});
#ifndef _WIN32
    // Mandatory on Linux: resolve each same-named export from its OWN ELF.
    NativeOpticalLibraries libraries("../candidate/release/optical/libpico_optics.so","../candidate/release/optical/libpico_detector.so");
    auto real=libraries.api();
    check(real.project && real.solve && real.detect);
    auto p=pair(base); double points[3]={0,0,1},pixel[2];
    check(real.project(&p->frames[0].view.camera,points,1,pixel)==0 && pixel[0]==528 && pixel[1]==512);
    double blobs[1280]; check(real.detect(p->frames[0].pixels.data(),1024,1056,180,2,250,blobs,256)==0);
    // Actual uint8 ABI must recover x/y beyond the transposed width boundary.
    for(int y=699;y<=701;++y)for(int x=1039;x<=1041;++x)p->frames[0].pixels[size_t(y)*1056+size_t(x)]=255;
    check(real.detect(p->frames[0].pixels.data(),1024,1056,180,2,250,blobs,256)==1);
    check(std::abs(blobs[0]-1040)<.01 && std::abs(blobs[1]-700)<.01 && blobs[4]==9);
    NativeLedModeIdentity m(chip,model(),real); m.configure({chip},"live"); populate(m); check(m.evaluate(episode()));
#endif
}
}
int main(int argc,char** argv) {
#ifdef _WIN32
    if(!std::getenv("PICO_IDENTITY_EPISODES")) _putenv_s("PICO_IDENTITY_EPISODES", "1");
#else
    ::setenv("PICO_IDENTITY_EPISODES", "1", 0);
#endif
    try { admission_tests();
        if(argc==2 && std::string(argv[1])=="--admission-only") {std::cout<<"native identity admission PASS "<<checks<<" checks\n";return 0;}
        contrast_tests(); matcher_tests(); session_tests(); hooks_tests(); concurrent_session_test(); real_library_tests();
        std::cout<<"native identity PASS "<<checks<<" checks\n"; return 0;
    } catch(const std::exception &e) { std::cerr<<e.what()<<"\n"; return 1; }
}

