#include "native_test_runtime.hpp"
#include "replay_gyro.hpp"
#include <iostream>
#include <stdexcept>
using namespace pico;
static int checks=0;
static void check(bool ok,const char* why) { ++checks; if(!ok) throw std::runtime_error(why); }
static void sample(OpticalPrior& p,int64_t t,int rate=1000,int accel=1024) {
    std::array<uint8_t,32> b{}; std::memcpy(b.data(),"PTI1",4);
    auto put=[&](int at,uint64_t v,int n) { for(int j=0;j<n;++j) b[size_t(at+j)]=uint8_t(v>>(8*j)); };
    put(4,0xA1B2C3D4,4);put(12,uint64_t(t),8);put(24,uint16_t(rate),2);put(28,uint16_t(accel),2);
    p.wire(b.data(),b.size());
}
static Rigid turn(double a) { auto r=identity_rigid();r[0]=r[5]=std::cos(a);r[1]=-std::sin(a);r[4]=std::sin(a);return r; }
int main() { try {
    const int64_t t=2000000000;
    {std::ofstream f("build/recovery3-calibration.txt");f<<"A1B2C3D4 1 0 0 0 1 0 0 0 1 0.001 0 0 0 0\n";}
    OpticalPrior p(true);check(p.load("build/recovery3-calibration.txt",0xA1B2C3D4),"load");
    IdentityCandidate c;c.stamp=t;c.seeded=true;
    check(!p.supported_candidate(c,{},true),"seeded fix without gyro must drop (round-2 regression)");
    check(p.supported_candidate(c,{},false),"OFF keeps old admission");
    check(p.supported_candidate(c,identity_rigid(),true),"supported seeded fit");
    c.seeded=false;c.few_led=true;
    check(!p.supported_candidate(c,{},true),"optical-rate few-LED fix must drop");
    c.few_led=false;c.prior_continuation=true;
    check(!p.supported_candidate(c,{},true),"continuation must drop");
    c.prior_continuation=false;
    sample(p,t,0);
    check(p.supported_candidate(c,{},true),"upright blind acquisition");
    c.world=turn(std::acos(-1.));
    check(!p.supported_candidate(c,{},true),"upside-down blind acquisition must drop (round-2 regression)");
    c.world=turn(16*std::acos(-1.)/180);
    check(!p.supported_candidate(c,{},true),"16 degree tilt reject");
    c.world=turn(14*std::acos(-1.)/180);
    check(p.supported_candidate(c,{},true),"14 degree tilt accept");
    c.world=turn(std::acos(-1.));c.stamp=t+61000000;
    check(!p.tilt_deg(c.world,c.stamp),"stale accel not used");
    sample(p,t+65000000,1000);
    check(!p.tilt_deg(c.world,t+65000000),"fast rotation not a gravity check");
    sample(p,t+70000000,0,3000);
    check(!p.tilt_deg(c.world,t+70000000),"dynamic acceleration not gravity");
    sample(p,t+75000000,0,2048);
    check(!p.tilt_deg(c.world,t+75000000),"2g is not reinterpreted as another sensor scale");
    OpticalPrior full(true);check(full.load("build/recovery3-calibration.txt",0xA1B2C3D4),"load hindsight");
    // A real 57-degree turn in a 1s optical gap is NOT a flip.
    for(int i=0;i<=1000;++i) sample(full,t+i*5000000LL);
    auto integrated=full.integral(t,t+1000000000);
    check(integrated.covered_ns==1000000000,"full bracketed coverage after >3s retention");
    check(replay_gyro_disagreement(identity_rigid(),turn(1),integrated.rotation)<1e-5,"true turn not a flip");
    check(replay_gyro_disagreement(identity_rigid(),turn(1.5),integrated.rotation)>20,"extra optical turn is flip");
    check(replay_gyro_disagreement(turn(.7),turn(1.7),integrated.rotation)<1e-5,"relative rotation cancels world orientation");
    auto previous=identity_rigid();previous[5]=previous[10]=0;previous[6]=-1;previous[9]=1;
    auto current=previous;
    for(int i=0;i<3;++i) for(int j=0;j<3;++j) {
        current[4*i+j]=0;
        for(int k=0;k<3;++k) current[4*i+j]+=previous[4*i+k]*integrated.rotation[4*k+j];
    }
    check(replay_gyro_disagreement(previous,current,integrated.rotation)<1e-5,"noncommuting world/body convention");
    const auto q=replay_rotation({0,0,0,0,0,std::sin(.5),std::cos(.5)});
    check(OpticalPrior::angle(q,turn(1))<1e-5,"xyzw metric convention");
    OpticalPrior holes(true);check(holes.load("build/recovery3-calibration.txt",0xA1B2C3D4),"load sparse");
    sample(holes,t);sample(holes,t+5000000);sample(holes,t+95000000);sample(holes,t+100000000);
    integrated=holes.integral(t,t+100000000);
    check(integrated.covered_ns==10000000,"90ms hole not counted or interpolated");
    check(holes.integral(t-10000000,t+110000000).covered_ns==10000000,"unbracketed tails not counted");
    sample(holes,t+5000000);check(holes.imu==4,"duplicate sample no extra coverage");
    check(replay_imu_available(t+78000000,t,{})==t+78000000,"recorded receive default");
    check(replay_imu_available(t+78000000,t,25000000)==t+25000000,"25ms availability");
    check(replay_imu_available(t+78000000,t,25000000)>t+12000000,"25ms sample unavailable at +12ms");
    full.accepted(identity_rigid(),t+5000000000);
    check(!full.predict(t+5000000000+60000001),"causal tail bound still rejects");
    std::cout<<"recovery3 tests PASS "<<checks<<'\n';
} catch(const std::exception& e) { std::cerr<<"recovery3 check "<<checks<<": "<<e.what()<<'\n';return 1; } }
