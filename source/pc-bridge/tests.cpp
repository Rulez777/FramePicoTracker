#include "bridge.h"
#include "openvr.h"
#include "openvr_pose.h"
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
static unsigned checks=0;
static void check(bool ok,const char* message){++checks;if(!ok)throw std::runtime_error(message);}
static bool near(double a,double b){return std::abs(a-b)<1e-5;}
static bool same(bridge::Vec a,bridge::Vec b){return near(a.x,b.x)&&near(a.y,b.y)&&near(a.z,b.z);}
static bridge::Packet fixture() {
    bridge::Packet p;p.session=123;p.sequence=1;p.timeNs=5000000000;
    p.head.valid=p.head.connected=true;p.head.p={1,1.7,3};
    for(unsigned i=0;i<4;++i){bridge::Tracker t;t.chip=i+1;t.role=static_cast<bridge::Role>(i+1);t.pose=p.head;t.pose.v={1,2,3};t.pose.w={-1,0,2};p.trackers.push_back(t);}return p;
}
int main() try {
    using namespace bridge;
    auto p=fixture();auto b=encode(p);Packet out;
    check(b.size()==348,"bounded datagram");check(b[0]=='F'&&b[1]=='T'&&b[2]=='B'&&b[3]=='1',"magic byte order");
    check(decode(b.data(),b.size(),out),"decode");check(out.session==123&&out.timeNs==5000000000&&out.sequence==1,"64 bit fields");
    check(out.trackers.size()==4&&out.trackers[3].role==Role::Chest,"roles roundtrip");
    check(same(out.head.p,p.head.p)&&same(out.trackers[0].pose.v,{1,2,3})&&same(out.trackers[0].pose.w,{-1,0,2}),"pose/velocities roundtrip");
    for(size_t n=0;n<b.size();++n)check(!decode(b.data(),n,out),"every truncation rejected");
    auto bad=b;bad.push_back(0);check(!decode(bad.data(),bad.size(),out),"trailing byte");
    for(size_t at:{size_t(0),size_t(4),size_t(8),size_t(36),size_t(96)}){bad=b;bad[at]=255;check(!decode(bad.data(),bad.size(),out),"bad magic/version/count/flags/role");}
    p=fixture();p.trackers[1].chip=p.trackers[0].chip;bad=encode(p);check(!decode(bad.data(),bad.size(),out),"duplicate chip");
    p=fixture();p.trackers[0].chip=0;bad=encode(p);check(!decode(bad.data(),bad.size(),out),"zero chip");
    p=fixture();p.head.p.x=std::numeric_limits<double>::quiet_NaN();bad=encode(p);check(!decode(bad.data(),bad.size(),out),"NaN");
    p=fixture();p.head.q={0,0,0,0};bad=encode(p);check(!decode(bad.data(),bad.size(),out),"degenerate quaternion");
    p=fixture();p.head.q={2,0,0,0};bad=encode(p);check(!decode(bad.data(),bad.size(),out),"nonunit quaternion");
    p=fixture();p.trackers[0].pose.w.z=std::numeric_limits<double>::infinity();bad=encode(p);check(!decode(bad.data(),bad.size(),out),"infinite angular velocity");
    p=fixture();p.head.connected=false;bad=encode(p);check(!decode(bad.data(),bad.size(),out),"valid disconnected");
    p=fixture();p.head.valid=false;bad=encode(p);check(decode(bad.data(),bad.size(),out)&&!out.head.valid,"invalid HMD retained");
    p=fixture();p.trackers.clear();bad=encode(p);check(decode(bad.data(),bad.size(),out)&&out.trackers.empty(),"empty snapshot");
    auto mapping=roles("active 1 auto 3 calibrated 1 roles A1B2C3D4=pelvis A1B2C3E2=left_foot A1B2C3E1=right_foot 12345678=chest BAD=pelvis");
    check(mapping.size()==4&&mapping.at(0xA1B2C3D4)==Role::Waist&&mapping.at(0xA1B2C3E2)==Role::LeftFoot,"real status grammar");
    check(std::string(controller(Role::RightFoot))=="vive_tracker_right_foot","controller property");
    check(std::string(controller(Role::Unknown))=="vive_tracker"&&role("unknown")==Role::Unknown,"unassigned stays unassigned");
    uint32_t chip=0;check(!chipId("1234567X",chip)&&!chipId("00000000",chip),"bad serial");
    Pose frame;frame.valid=frame.connected=true;frame.p={1,1.5,-2};frame.q=normalize({0.8,0.1,0.2,0.3});
    Transform expected{normalize({0.5,0.2,-0.7,0.1}),{5,2,-3}};
    Pose pc=transform(expected,frame);auto a=alignment(pc,frame);Pose tr=frame;tr.p={-1,0.1,-2};tr.v={1,2,-3};tr.w={4,-2,1};
    check(same(transform(a,tr).p,transform(expected,tr).p),"rigid offset and non-yaw rotation");
    check(same(rotate(transform(a,tr).q,{0,0,-1}),rotate(transform(expected,tr).q,{0,0,-1})),"orientation transforms");
    check(same(transform(a,tr).v,transform(expected,tr).v)&&same(transform(a,tr).w,transform(expected,tr).w),"both velocities rotate");
    auto ident=alignment(frame,frame);check(same(transform(ident,tr).p,tr.p),"identity alignment");
    Alignment smooth;smooth.update(pc,frame,1);check(smooth.have&&same(smooth.value.p,expected.p),"first alignment immediate");
    pc.p.x+=1;smooth.update(pc,frame,1.1);check(smooth.value.p.x>expected.p.x&&smooth.value.p.x<expected.p.x+1,"alignment smoothing");
    auto save=smooth.value;pc.valid=false;smooth.update(pc,frame,1.2);check(same(save.p,smooth.value.p),"invalid HMD cannot update transform");
    Quat q=normalize({0.4,0.2,0.3,0.1});auto blended=blend(q,{-q.w,-q.x,-q.y,-q.z},0.5);check(near(blended.w,q.w),"quaternion hemisphere");
    vr::TrackedDevicePose_t vp{};vp.bPoseIsValid=vp.bDeviceIsConnected=true;
    vp.mDeviceToAbsoluteTracking.m[0][0]=1;vp.mDeviceToAbsoluteTracking.m[1][1]=-1;vp.mDeviceToAbsoluteTracking.m[2][2]=-1;
    auto converted=fromVr(vp);check(converted.valid&&near(std::abs(converted.q.x),1),"180 degree matrix conversion");
    Stream s;p=fixture();b=encode(p);check(s.ingest(b.data(),b.size(),100),"first stream");
    Alignment ready;ready.have=true;
    check(publication(s,ready,1,100).valid,"driver publication valid");
    check(!publication(s,ready,5,100).valid,"missing tracker while others alive");
    check(!publication(s,ready,1,100.6).valid&&!publication(s,ready,1,100.6).connected,"driver stream stop invalid/disconnected");
    Alignment unavailable;check(!publication(s,unavailable,1,100).valid,"driver no alignment");
    s.packet.head.valid=false;check(!publication(s,ready,1,100).valid,"driver invalid Frame HMD");s.packet.head.valid=true;
    s.packet.trackers[0].pose.valid=false;check(!publication(s,ready,1,100).valid,"driver invalid source tracker");s.packet.trackers[0].pose.valid=true;
    check(s.fresh(100.49)&&!s.fresh(100.501),"500 ms outage");check(near(s.age(100.03),0.035),"clock epoch offset and baseline latency");
    check(!s.ingest(b.data(),b.size(),100.1)&&near(s.received,100),"duplicate does not refresh");
    p.sequence=2;p.timeNs+=10000000;b=encode(p);check(s.ingest(b.data(),b.size(),100.01),"next packet");
    p.sequence=1;p.timeNs+=10000000;b=encode(p);check(!s.ingest(b.data(),b.size(),100.02),"reorder");
    p.sequence=3;p.timeNs=1;b=encode(p);check(!s.ingest(b.data(),b.size(),100.03),"timestamp regression");
    p.session=124;b=encode(p);check(!s.ingest(b.data(),b.size(),100.04),"foreign active session");
    check(s.ingest(b.data(),b.size(),101)&&s.fresh(101),"sender restart after outage");
    p.sequence++;p.timeNs+=10000000;b=encode(p);check(s.ingest(b.data(),b.size(),101.7)&&!s.fresh(101.7),"delayed queued packet cannot revive stale pose");
    std::mt19937 random(123);for(unsigned i=0;i<10000;++i){auto fuzz=encode(fixture());for(int k=0;k<3;++k)fuzz[random()%fuzz.size()]=static_cast<uint8_t>(random());decode(fuzz.data(),fuzz.size(),out);++checks;}
    std::cout<<"PASS "<<checks<<" checks (synthetic only)\n";return 0;
}catch(const std::exception& e){std::cerr<<"FAIL "<<checks<<": "<<e.what()<<"\n";return 1;}
