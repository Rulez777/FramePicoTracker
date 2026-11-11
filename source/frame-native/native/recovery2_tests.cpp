#include "native_test_runtime.hpp"
#include "optical_prior.hpp"
#include "prior_consensus.hpp"
#include <iostream>
#include <stdexcept>
using namespace pico;
static int checks=0;
static void check(bool ok,const char* why) { ++checks; if(!ok) throw std::runtime_error(why); }
static double measured[16]{};
static int projection(const OpticalCamera* c,const double* p,size_t n,double* uv) {
    for(size_t i=0;i<n;++i) {
        if(p[3*i+2]<=0) return 1;
        uv[2*i]=c->cx+c->fx*p[3*i]/p[3*i+2]; uv[2*i+1]=c->cy+c->fy*p[3*i+1]/p[3*i+2];
    }
    return 0;
}
static int detect(const uint8_t* image,int,int,int,int,int,double* out,int) {
    if(!image[0]) return 0;
    for(int i=0;i<4;++i) { out[5*i]=measured[2*i]; out[5*i+1]=measured[2*i+1]; out[5*i+2]=1; out[5*i+3]=255; out[5*i+4]=3; }
    return 4;
}
static int no_stereo(const double*,const OpticalView*,const double*,OpticalFit*) { return 1; }
int main() { try {
    check(OpticalPrior::consistency_bound_deg(0.)==10,"initial bound");
    check(OpticalPrior::consistency_bound_deg(3.)==16,"age-growing bound");
    check(OpticalPrior::consistency_bound_deg(50.)==30,"capped bound");
    OpticalPrior gyro; auto truth=identity_rigid(); truth[3]=.24; truth[7]=-.02; truth[11]=.7;
    gyro.accepted(truth,1000000000);
    double delta[6]{0,0,0,0,0,11*std::acos(-1.)/180};
    auto wrong=few_led_detail::step(truth,delta,6);
    check(!gyro.consistent(wrong,1033333333,truth),"reject 11-degree early fit");
    check(gyro.consistent(wrong,2000000000,truth),"allow calibrated age growth");
    delta[5]=std::acos(-1.); wrong=few_led_detail::step(truth,delta,6);
    for(int i=0;i<8;++i) check(!gyro.consistent(wrong,1033333333+i*33333333LL,truth),"no repeated flip reanchor");
    check(gyro.consistent(wrong,1033333333,{}),"missing gyro is not fabricated");
    // Same asymmetric four measured LEDs as the mono solver fixture, through
    // the actual IdentityPair dispatcher (the old caller silently discarded prior).
    LedModel model{-.041,-.02,.012,.032,-.025,-.024,.048,.014,.036,-.026,.036,-.048,
        .006,.044,.072,-.046,.008,.088,.018,-.008,.124,.003,.012,-.104};
    auto pair=std::make_shared<IdentityPair>(); pair->timestamp_matched=true;
    for(int v=0;v<2;++v) {
        auto& f=pair->frames[size_t(v)]; f.serial=v?"CAMUPPERRIGHT":"CAMUPPERLEFT";
        f.width=1056; f.height=1024; f.pixels.resize(1056u*1024u); f.pixels[0]=v?0:255;
        f.stamp=1100000000; f.timestamp_matched=true; f.exposure_us=800; f.metadata="synthetic-mono"+std::to_string(v);
        f.view.camera={700,710,528,512,{0,0,0,0},1056,1024}; f.view.head_from_camera[3]=.2;
    }
    NativeOpticalApi api{projection,no_stereo,detect};
    OpticalView view{}; view.camera=pair->frames[0].view.camera;
    std::copy(pair->frames[0].view.head_from_camera.begin(),pair->frames[0].view.head_from_camera.end(),view.head_from_camera);
    for(int led=0;led<4;++led) check(few_led_detail::project(api,model,view,truth,led,measured+2*led,false),"project mono fixture");
    const auto c=identity_stereo_candidate(api,model,pair,1112000000,truth.data());
    check(c.pair && c.mono && c.prior_continuation,"mono dispatcher must pass prior to resolve four-LED ambiguity");
    check(std::string(c.solver_kind)=="seeded mono","mono diagnostic kind");
    check(OpticalPrior::angle(c.world,truth)<.1,"mono stays in gyro branch");
    // Weak rotation observability must still permit a measured translation at known gyro orientation.
    std::vector<few_led_detail::Match> matches;
    double sparse[6]{};
    auto distant=truth; distant[11]=1.0; auto weak_model=model;
    for(int led=0;led<3;++led) weak_model[3*led+2]*=.05;
    for(int led=0;led<3;++led) {
        check(few_led_detail::project(api,weak_model,view,distant,led,sparse+2*led,false),"project distant fixture");
        matches.push_back({0,led,led});
    }
    view.count=3; view.pixels=sparse; OpticalView views[2]{view,{}};
    OpticalFit fit{}; int n=0; bool rotation=false;
    check(!refine_led_matches(api,weak_model,views,distant,.033,matches,fit,n,rotation,false,true),"free distant rotation fails covariance");
    check(refine_led_matches(api,weak_model,views,distant,.033,matches,fit,n,rotation,true,true),"gyro-fixed translation remains observable");
    check(solve_prior_consensus(api,weak_model,views,distant,.033,fit,n,rotation),"consensus must keep observed translation when rotation covariance fails");
    check(!rotation,"weak rotation is held at gyro orientation");
    std::cout<<"recovery2 tests PASS "<<checks<<'\n';
} catch(const std::exception& e) { std::cerr<<"recovery2 check "<<checks<<": "<<e.what()<<'\n'; return 1; } }
