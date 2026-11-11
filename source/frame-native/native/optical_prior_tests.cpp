#include "optical_prior.hpp"
#include <iostream>
#include <stdexcept>
using namespace pico;
// Synthetic checks only: no camera/hardware claims.
static void check(bool b){if(!b)throw std::runtime_error("optical prior check failed");}
static Rigid yaw(double degrees){auto r=identity_rigid();double a=degrees*std::acos(-1.)/180;r[0]=r[5]=std::cos(a);r[1]=-std::sin(a);r[4]=std::sin(a);return r;}
int main(){
    OpticalPrior p;
    {std::ofstream f("build/prior-test-calibration.txt");f<<"A1B2C3D4 1 0 0 0 1 0 0 0 1 0.001 0 0 10 -3000000 0\n";}
    check(p.load("build/prior-test-calibration.txt",0xA1B2C3D4));
    const int64_t base=2000000000;
    p.accepted(identity_rigid(),base);
    check(!p.predict(base+10000000));
    for(int i=0;i<=250;++i){
        std::array<uint8_t,32> b{};std::memcpy(b.data(),"PTI1",4);
        auto put=[&](int at,uint64_t x,int n){for(int j=0;j<n;++j)b[size_t(at+j)]=uint8_t(x>>(8*j));};
        put(4,0xA1B2C3D4,4);put(12,uint64_t(base+3000000+4000000*i),8);put(24,1010,2);
        p.wire(b.data(),b.size());
    }
    auto predicted=p.predict(base+1000000000);
    check(bool(predicted));check(OpticalPrior::angle(*predicted,yaw(180/std::acos(-1.)))<1e-5);
    check(!p.predict(base+1000000001,false)); // Explicit legacy horizon; extended coverage is in recovery_tests.
    const auto identity=identity_rigid();
    for(int i=1;i<5;++i)check(!p.admit(yaw(90),base+i*10000000,identity));
    check(p.admit(yaw(90),base+50000000,identity));check(p.rejected==4&&p.reanchors==1);
    check(!p.admit(yaw(90),base+60000000,identity));p.missing();
    check(!p.admit(yaw(90),base+70000000,identity));check(p.reanchors==1);
    check(p.admit(yaw(24),base+80000000,identity));
    // Conflicting candidates do not form a re-anchor, nor do separated fits.
    check(!p.admit(yaw(90),base+90000000,identity));
    check(!p.admit(yaw(140),base+100000000,identity));
    check(!p.admit(yaw(140),base+400000000,identity));
    check(p.reanchors==1);
    std::cout<<"optical_prior synthetic propagation, bias/time conversion, expiry, gate/reanchor PASS\n";
}
