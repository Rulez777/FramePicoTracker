#include "native_test_runtime.hpp"
#include "optical_timing.hpp"
#include "pose_publication.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#ifndef _WIN32
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#endif
using namespace pico;
namespace {
int checks=0;
void check(bool b,int line=__builtin_LINE()) { ++checks; if(!b) throw std::runtime_error("check "+std::to_string(checks)+" line "+std::to_string(line)); }
template<class F> void reject(F f,int line=__builtin_LINE()) { bool bad=false; try { f(); } catch(const std::exception &) { bad=true; } check(bad,line); }
uint64_t le(const std::array<uint8_t,88> &b,size_t at,size_t n) {
    uint64_t v=0; for(size_t i=0;i<n;++i) v|=uint64_t(b[at+i])<<(8*i);
    return v;
}
void clocks() {
    CameraClockEvidence e{1,"boot","CLOCK_MONOTONIC_RAW","synthetic domain evidence",true,0,500000};
    RawBridge bridge{10000000000,11000000000,10000000000}; int calls=0;
    OpticalCameraClock c(e,"boot",[&] { ++calls; return bridge; });
    check(c.convert({9.9,.0008},11000000000)==10900000000);
    bridge.mono+=100000000; // host slew/offset is re-sampled, no frozen bridge
    check(c.convert({9.9,.0008},11100000000)==11000000000); check(calls==2);
    check(c.convert({9.75,.0008},11100000000)==10850000000);
    check(c.convert({9.749999999,.0008},11100000000)==0);
    check(c.convert({10.000000001,.0008},11100000000)==0);
    // Invalid samples are logged and dropped (0), never thrown.
    const std::array<int64_t,2> none{0,0};
    auto drops=optical_drops("Short exposure required");
    for(double exposure:{0.,.001,-.0001,std::numeric_limits<double>::quiet_NaN()})
        check(c.convert({9.9,exposure},11100000000)==0);
    check(optical_drops("Short exposure required")==drops+4);
    drops=optical_drops("Invalid RAW timestamp");
    check(c.convert({std::numeric_limits<double>::infinity(),.0008},11100000000)==0);
    check(optical_drops("Invalid RAW timestamp")==drops+1);
    drops=optical_drops("RAW bridge uncertainty exceeded");
    bridge.after+=1000001; check(c.convert({9.9,.0008},11100000000)==0);
    bridge.after=bridge.before-1; check(c.convert({9.9,.0008},11100000000)==0);
    check(optical_drops("RAW bridge uncertainty exceeded")==drops+2);
    bridge.after=bridge.before;
    auto pair=c.convert_pair({CameraMetadata{9.9,.0008},CameraMetadata{9.900001,.0008}},11100000000);
    check(pair[1]-pair[0]==1000);
    drops=optical_drops("Stereo skew exceeds source tolerance");
    check(c.convert_pair({CameraMetadata{9.9,.0008},CameraMetadata{9.900003,.0008}},11100000000)==none);
    check(optical_drops("Stereo skew exceeds source tolerance")==drops+1);
    check(c.convert_pair({CameraMetadata{9.9,.0008},CameraMetadata{9.9,.001}},11100000000)==none);
    // Unproven clock evidence is logged and the clock still converts.
    drops=optical_drops("Current-boot camera clock evidence not proven; continuing");
    { OpticalCameraClock old(e,"old",[&] { return bridge; }); check(old.convert({9.9,.0008},11100000000)==11000000000); }
    e.verified=false; { OpticalCameraClock unverified(e,"boot",[&] { return bridge; }); } e.verified=true;
    e.uncertainty=500001; { OpticalCameraClock loose(e,"boot",[&] { return bridge; }); } e.uncertainty=500000;
    e.domain="CLOCK_BOOTTIME"; { OpticalCameraClock other(e,"boot",[&] { return bridge; }); }
    check(optical_drops("Current-boot camera clock evidence not proven; continuing")==drops+4);
    OpticalCameraClock tap({1,"tap","CLOCK_MONOTONIC_RAW","xrtap-direct",true,0,0},"tap",
        []{return RawBridge{10000000000,11000000000,10000000000};});
    auto direct=tap.convert_pair({CameraMetadata{9.9,37.233e-6},CameraMetadata{9.901,37.233e-6}},11100000000);
    check(direct[1]-direct[0]==1000000);
    check(tap.convert_pair({CameraMetadata{9.9,37.233e-6},CameraMetadata{9.901000001,37.233e-6}},11100000000)==none);
}
void schedule() {
    OpticalCameraClock c({1,"boot","CLOCK_MONOTONIC","fixture",true,0,0},"boot");
    RoyClock r; OpticalExposureSchedule s(c,r,261700);
    reject([&] { s.frame_index(1); });
    check(!s.prepare(0,1,10000000000,0,7));
    for(int i=0;i<7;++i) {
        double raw=10+i/30.; auto now=nearest_even(raw*1e9);
        check(s.observe({raw,.0008},now).second==i);
    }
    const int64_t now=10200000000;
    r.fit=Fit{64000000,double(now),15.625,10000};
    auto p=s.prepare(0,1,now,0,7); check(bool(p));
    check(p->ticks==261700 && p->reference==64000000 && p->own_frame==7 && p->light_due);
    check(p->light_target==10233333333 && p->phase_target==10266666666);
    check(p->phase_window[0]==r.cycles_at(p->phase_target-12000000));
    check(p->phase_window[1]==r.cycles_at(p->phase_target-4000000));
    check(p->light_window[0]==r.cycles_at(p->light_target+1000000));
    check(p->light_window[1]==r.cycles_at(p->light_target+12000000));
    auto shifted=s.prepare(0,1,now,-600000,7); check(shifted->ticks==195033);
    auto multi=s.prepare(2,3,now,0,99); check(multi->own_frame%3==2 && multi->light_due);
    s.light_ack(7,1); s.phase_ack(7,now); check(!s.prepare(0,1,now,0,7));
    check(bool(s.prepare(0,1,now,0,8))); // stable-key isolation
    s.reset_peer(7); check(s.prepare(0,1,now,0,7)->light_due);
    r.fit->stamp=double(now-250000001);
    bool threw=false; try { (void)s.prepare(0,1,now,0,7); } catch(const std::exception &) { threw=true; }
    check(!threw); // an old Roy projection horizon never throws out of prepare
    r.fit->stamp=double(now);
    auto slope_drops=optical_drops("Roy DWT not measured 64MHz; no exposure plan this tick");
    r.fit->slope=15.63; check(!s.prepare(0,1,now,0,7)); r.fit->slope=15.625; // logged, wait for next tick
    check(optical_drops("Roy DWT not measured 64MHz; no exposure plan this tick")==slope_drops+1);
    check(bool(s.prepare(0,1,now,0,7)));
    reject([&] { s.prepare(3,3,now,0,7); });
    reject([&] { s.prepare(0,0,now,0,7); });
    // Repeated second camera does not advance anchor or period.
    check(s.observe({10.2,.0008},now).second==6);
    check(s.frame_index(10.1)==3);
    auto before=s.exposure_times();
    check(s.observe({10.3,.0008},11000000000).first==0);
    check(s.frame_index(10.3)==9 && s.exposure_times()==before);
    check(!s.prepare(0,1,11000000000,0,7)); // stale metadata cannot schedule
    // Later fresh frame retains the stale frame's index contribution.
    check(s.observe({10.4,.0008},10400000000).second==12);
    for(int i=13;i<340;++i) {
        double raw=10+i/30.; check(s.observe({raw,.0008},nearest_even(raw*1e9)).second==i);
    }
    check(s.exposure_times().size()==12); check(s.frame_index(10+338/30.)==338);
    // Once lights are acknowledged, only command10 is due after 800ms.
    const auto later=nearest_even((10+339/30.)*1e9);
    r.fit=Fit{64000000,double(later),15.625,10000};
    s.light_ack(7,1); s.phase_ack(7,later-799999999);
    check(!s.prepare(0,1,later,0,7));
    s.phase_ack(7,later-800000000);
    auto phase_only=s.prepare(0,1,later,0,7);
    check(phase_only && !phase_only->light_due && phase_only->phase_target==phase_only->light_target);
    check(s.prepare(0,2,later,0,7)->light_due); // membership count revokes light cache
    // Actual RAW-patched Roy fit: 33ppm crystal and -163ppm host slew pass.
    RoyClock raw;
    for(int i=0;i<120;++i) {
        auto cycles=uint32_t(64000000ULL+i*6400000ULL);
        auto rr=nearest_even(20000000000.+double(i)*100000000*1.000033);
        auto mm=nearest_even(30000000000.+double(i)*100000000*1.000033*.999837);
        raw.observe(cycles,mm-10000,mm+10000,rr-10000,rr+10000);
    }
    check(bool(raw.fit)); check(std::abs(raw.crystal_ppm-33)<.01);
    check(std::abs(raw.host_slew_ppm+163)<.01);
    RoyClock bad;
    for(int i=0;i<120;++i) {
        auto rr=nearest_even(20000000000.+double(i)*100000000*1.0002);
        auto mm=nearest_even(30000000000.+double(i)*100000000*1.0002);
        bad.observe(uint32_t(64000000ULL+i*6400000ULL),mm-10000,mm+10000,rr-10000,rr+10000);
    }
    check(bad.fit && bad.fit->slope == 15.625); // >100ppm crystal is logged; the fit keeps the nominal 64 MHz slope
}
void phase() {
    using State=IdentityPhaseWindow::State;
    OpticalPhaseVerification p(261700,{1,2},true);
    p.ack(1,0); p.ack(99,261700); p.ack(1,261700);
    for(int i=900;i<=960;++i) p.observe(i,true,true);
    check(p.state()==State::Waiting); p.ack(2,261700);
    p.observe(899,true,true); check(p.state()==State::Waiting);
    for(int i=900;i<960;++i) p.observe(i,true,true);
    check(p.state()==State::Verifying); p.observe(960,true,true); check(p.state()==State::Locked);
    p.observe(2000,false,false); check(p.state()==State::Locked);
    // A failed window is logged and verification restarts; it never becomes terminal.
    for(int lit:{47,48}) for(bool identity:{false,true}) {
        OpticalPhaseVerification v(0,{1},false); v.ack(1,0);
        for(int i=0;i<=60;++i) v.observe(900+i,i<lit,identity);
        const bool locked=lit>=48 && identity;
        check(v.state()==(locked?State::Locked:State::Verifying));
        v.ack(1,0);
        for(int i=2000;i<=2060;++i) v.observe(i,true,true);
        check(v.state()==State::Locked); // the next good window locks
    }
    OpticalPhaseVerification duplicate(0,{1},true); duplicate.ack(1,0);
    for(int i=0;i<100;++i) duplicate.observe(900,true,true);
    duplicate.observe(960,true,true); check(duplicate.state()==State::Verifying);
    IdentityPhaseWindow retried;
    for(int window=0;window<3;++window) for(int i=0;i<=60;++i) retried.observe(1000+window*61+i,false,false,true);
    check(retried.state==State::Verifying && retried.retries()==3);
    for(int i=0;i<=60;++i) retried.observe(2000+i,true,true,true);
    check(retried.state==State::Locked && retried.retries()==3);
    OpticalPhaseVerification dark(0,{1},true); dark.ack(1,0);
    for(int i=900;i<=1000;++i) dark.observe(i,true,true,true);
    check(dark.state()==State::Waiting);
    for(int i=1001;i<=1061;++i) dark.observe(i,true,true);
    check(dark.state()==State::Locked);
    OpticalPhaseVerification sparse(0,{1},true); sparse.ack(1,0);
    for(int i=0;i<47;++i) sparse.observe(900+i,true,true);
    sparse.observe(960,true,true); check(sparse.state()==State::Locked); // 48/61 exact boundary
    OpticalPhaseVerification any(0,{1,2},false); any.ack(2,0);
    for(int i=900;i<=960;++i) any.observe(i,true,true);
    check(any.state()==State::Locked); // explicit legacy require_all=False mode
    reject([] { OpticalPhaseVerification bad(0,{1,1},true); });
    reject([] { OpticalPhaseVerification bad(533333,{1},true); });
}
void wire() {
    reject([] { OpticalPosePublisher invalid(0); });
    OpticalPoseFrame f; f.chip=0xA1B2C3D4; f.sequence=0x0102030405060708ULL; f.stamp=1000000000;
    f.position={1,-2,.5}; f.quaternion={.5,.5,.5,.5}; f.connected=true; f.valid=true; f.quality=.75;
    auto b=f.encode(f.stamp+250000000);
    check(b[0]=='P' && b[1]=='T' && b[2]=='P' && b[3]=='2');
    check(le(b,4,4)==f.chip && le(b,8,4)==3 && le(b,12,8)==f.sequence && le(b,20,8)==uint64_t(f.stamp));
    check(le(b,28,4)==0x3f800000 && le(b,32,4)==0xc0000000 && le(b,36,4)==0x3f000000);
    for(size_t i=40;i<56;i+=4) check(le(b,i,4)==0x3f000000);
    check(le(b,80,4)==0xbf800000 && le(b,84,4)==0x3f400000);
    f.battery=0; b=f.encode(f.stamp); check(le(b,8,4)==7 && le(b,80,4)==0);
    f.battery=100; check(le(f.encode(f.stamp),80,4)==0x42c80000);
    // 2026-10-29: no 250 ms / future cut any more; only >1 s old is invalid.
    (void)f.encode(f.stamp+250000001); (void)f.encode(f.stamp-1); reject([&] { f.encode(f.stamp+1000000001); });
    f.connected=false; reject([&] { f.encode(f.stamp); }); f.connected=true;
    f.quality=0; reject([&] { f.encode(f.stamp); }); f.quality=1;
    f.quaternion={0,0,0,0}; reject([&] { f.encode(f.stamp); }); f.quaternion={0,0,0,1};
    f.position[0]=std::numeric_limits<double>::infinity(); reject([&] { f.encode(f.stamp); }); f.position[0]=0;
    f.battery=101; reject([&] { f.encode(f.stamp); }); f.battery.reset();
    f.quality=std::numeric_limits<double>::quiet_NaN(); reject([&] { f.encode(f.stamp); }); f.quality=1;
    f.quaternion[0]=std::numeric_limits<double>::quiet_NaN(); reject([&] { f.encode(f.stamp); });
    f.quaternion={0,0,0,1};
    f.position[0]=1000001; reject([&] { f.encode(f.stamp); }); f.position[0]=0;
    f.chip=0; reject([&] { f.encode(f.stamp); }); f.chip=1;
    f.valid=false; f.connected=false; f.quality=0; check(le(f.encode(f.stamp),8,4)==0);
#ifndef _WIN32
    // Real UDP synthetic invalid-status frame; no device, optical or RF access.
    int fd=socket(AF_INET,SOCK_DGRAM,0); check(fd>=0);
    sockaddr_in a{}; a.sin_family=AF_INET; a.sin_port=0; a.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
    check(bind(fd,reinterpret_cast<sockaddr *>(&a),sizeof(a))==0);
    timeval timeout{1,0}; check(setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout))==0);
    socklen_t address_size=sizeof(a);
    check(getsockname(fd,reinterpret_cast<sockaddr *>(&a),&address_size)==0);
    check(ntohs(a.sin_port)!=0);
    OpticalPosePublisher publisher(ntohs(a.sin_port)); check(publisher.send(f,f.stamp));
    std::array<uint8_t,88> received{}; check(recv(fd,received.data(),received.size(),0)==88);
    check(received==f.encode(f.stamp));
    // A stale pose is logged and dropped by send(); encode() itself still validates.
    check(!publisher.send(f,f.stamp+1000000001) && publisher.dropped()==1);  // only >1 s old is dropped now
    close(fd);
#endif
}
}
int main() { try { clocks(); schedule(); phase(); wire(); std::cout<<"optical timing/publication "<<checks<<" checks PASS\n"; }
catch(const std::exception &e) { std::cerr<<e.what()<<"\n"; return 1; } }

