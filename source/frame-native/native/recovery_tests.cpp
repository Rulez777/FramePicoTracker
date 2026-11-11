#include "native_test_runtime.hpp"
#include "few_led.hpp"
#include "optical_prior.hpp"
#include "prior_consensus.hpp"
#include <iostream>
#include <limits>
#include <stdexcept>
using namespace pico;
static int checks=0;
static void check(bool ok,const char* why) { ++checks; if(!ok) throw std::runtime_error(why); }
static int project(const OpticalCamera* c,const double* p,size_t n,double* uv) {
    for(size_t i=0;i<n;++i) { if(p[3*i+2]<=0) return 1;
        uv[2*i]=c->cx+c->fx*p[3*i]/p[3*i+2]; uv[2*i+1]=c->cy+c->fy*p[3*i+1]/p[3*i+2]; }
    return 0;
}
static const LedModel model{-.045,-.035,-.01,.04,-.034,-.015,.046,.035,-.008,
    -.043,.038,-.018,-.016,-.012,-.025,.017,-.013,-.005,.018,.014,-.02,-.018,.016,-.007};
int main() { try {
    FewLedHistory history;
    IdentityCandidate full; full.pair=std::make_shared<IdentityPair>();
    full.world=identity_rigid(); full.world[11]=.6; full.stamp=1000000000;
    history.accepted(full);
    check(bool(history.predict(1800000000,full.world)),"regression: calibrated gyro prior must survive 800 ms optical outage");
    check(!history.predict(1800000000,{}),"no extended optical-rate extrapolation without gyro");
    check(!history.predict(4000000000,full.world),"prediction must expire without measurements");
    { std::ofstream file("build/recovery-calibration.txt"); file<<"A1B2C3D4 1 0 0 0 1 0 0 0 1 0.001 0 0 10 -3000000\n"; }
    OpticalPrior gyro;
    check(gyro.load("build/recovery-calibration.txt",0xA1B2C3D4),"calibration");
    gyro.accepted(full.world,full.stamp);
    for(int i=0;i<=750;++i) {
        std::array<uint8_t,32> wire{}; std::memcpy(wire.data(),"PTI1",4);
        auto put=[&](int at,uint64_t value,int size){ for(int j=0;j<size;++j) wire[size_t(at+j)]=uint8_t(value>>(8*j)); };
        put(4,0xA1B2C3D4,4); put(12,uint64_t(full.stamp+3000000+4000000LL*i),8); put(24,1010,2);
        gyro.wire(wire.data(),wire.size());
    }
    const auto predicted=gyro.predict(full.stamp+2800000000);
    double yaw[6]{0,0,0,0,0,2.8}; const auto expected=few_led_detail::step(full.world,yaw,6);
    std::cout<<"gyro 2.8s supported="<<bool(predicted)<<" error_deg="<<(predicted?OpticalPrior::angle(*predicted,expected):-1)<<'\n';
    check(predicted && OpticalPrior::angle(*predicted,expected)<1e-4,"2.8 seconds calibrated gyro integration");
    check(predicted && (*predicted)[11]==full.world[11],"gyro prediction holds translation");
    check(!gyro.predict(full.stamp+2800000000,false),"OFF keeps one-second legacy horizon");
    check(!gyro.predict(full.stamp+3100000000),"hold expiry");
    OpticalPrior missing; check(missing.load("build/recovery-calibration.txt",0xA1B2C3D4),"missing calibration");
    missing.accepted(full.world,full.stamp); check(!missing.predict(full.stamp+500000000),"missing IMU is not invented");
    NativeOpticalApi api{}; api.project=project;
    for(int scenario=0;scenario<5;++scenario) {
        OpticalView views[2]{}; std::array<std::vector<double>,2> pixels;
        Rigid prior=full.world,truth=prior;
        truth[3]=.08; truth[7]=-.025; if(scenario==2) truth[11]+=.12;
        double turn[6]{0,0,0,.02,-.01,.03}; truth=few_led_detail::step(truth,turn,6);
        for(int v=0;v<2;++v) {
            views[v].camera={700,710,528,512,{0,0,0,0},1056,1024};
            auto camera=identity_rigid(); camera[3]=v==0?-.06:.06;
            std::copy(camera.begin(),camera.end(),views[v].head_from_camera);
            if(scenario==1 && v==1) continue;
            for(int led=0;led<(scenario==3?3:6);++led) {
                double uv[2]; check(few_led_detail::project(api,model,views[v],truth,led,uv,true),"fixture projection");
                pixels[size_t(v)].insert(pixels[size_t(v)].end(),uv,uv+2);
            }
            if(scenario==4) for(size_t b=0;b<pixels[size_t(v)].size();++b)
                pixels[size_t(v)][b]+=.2*std::sin(double(b*7+size_t(v)*11));
            // Unrelated bright centroids mixed into both views.
            pixels[size_t(v)].insert(pixels[size_t(v)].end(),{250.,220.,600.,260.,810.,610.});
            views[v].count=pixels[size_t(v)].size()/2; views[v].pixels=pixels[size_t(v)].data();
        }
        OpticalFit fit{}; int n=0; bool rotation=false;
        check(!solve_few_led(api,model,views,prior,.033,fit,n,rotation),"regression: old local matcher loses translated target");
        check(solve_prior_consensus(api,model,views,prior,.8,fit,n,rotation),"consensus must recover translated LEDs among glints");
        Rigid recovered; std::copy(std::begin(fit.pose),std::end(fit.pose),recovered.begin());
        double error=0; for(int k=0;k<3;++k) error+=std::pow(recovered[4*k+3]-truth[4*k+3],2);
        check(std::sqrt(error)<(scenario==4?.005:.001),"recovered position accuracy");
        check(OpticalPrior::angle(recovered,truth)<(scenario==4?2.:.1),"recovered rotation accuracy");
        std::cout<<"synthetic recovery="<<scenario<<" inliers="<<n<<" error_mm="<<1000*std::sqrt(error)<<'\n';
        check(!solve_prior_consensus(api,model,views,prior,3.1,fit,n,rotation),"stale seed rejected");
        check(!solve_prior_consensus(api,model,views,prior,.001,fit,n,rotation),"impossible speed rejected");
        check(solve_few_led(api,model,views,recovered,.8,fit,n,rotation,true),"local few LED fit uses persistent gyro seed");
        auto invalid=prior; invalid[0]=std::numeric_limits<double>::quiet_NaN();
        check(!solve_prior_consensus(api,model,views,invalid,.8,fit,n,rotation),"nonfinite prior rejected");
        auto wrong=prior; double flip[6]{0,0,0,0,0,3.141592653589793}; wrong=few_led_detail::step(wrong,flip,6);
        if(solve_prior_consensus(api,model,views,wrong,.8,fit,n,rotation)) {
            std::copy(std::begin(fit.pose),std::end(fit.pose),recovered.begin());
            check(OpticalPrior::angle(recovered,wrong)<11.46,"even symmetric alternatives cannot flip away from gyro seed");
        }
        // Only unrelated glints: no model consensus can be fabricated.
        for(int v=0;v<2;++v) {
            pixels[size_t(v)]={250.,220.,600.,260.,810.,610.};
            views[v].count=3; views[v].pixels=pixels[size_t(v)].data();
        }
        check(!solve_prior_consensus(api,model,views,prior,.8,fit,n,rotation),"unrelated glints rejected");
    }
    std::cout<<"recovery tests PASS "<<checks<<'\n';
} catch(const std::exception& e) { std::cerr<<"recovery check "<<checks<<": "<<e.what()<<'\n'; return 1; } }
