#include "native_test_runtime.hpp"
#include "few_led.hpp"
#include "optical_prior.hpp"
#include <iostream>
#include <stdexcept>
#include <limits>
using namespace pico;
namespace {
int checks=0;
void check(bool value,const char* why) { ++checks; if(!value) throw std::runtime_error(why); }
int project(const OpticalCamera* c,const double* p,size_t n,double* uv) {
    for(size_t i=0;i<n;++i) {
        if(p[3*i+2]<=0) return 1;
        uv[2*i]=c->cx+c->fx*p[3*i]/p[3*i+2];
        uv[2*i+1]=c->cy+c->fy*p[3*i+1]/p[3*i+2];
    }
    return 0;
}
const LedModel model{-.045,-.035,-.01, .04,-.034,-.015, .046,.035,-.008,
    -.043,.038,-.018, -.016,-.012,-.025, .017,-.013,-.005, .018,.014,-.02, -.018,.016,-.007};
struct Fixture {
    NativeOpticalApi api{};
    OpticalView views[2]{};
    Rigid prior=identity_rigid(),truth=identity_rigid();
    double pixels[2][16]{};
    Fixture() {
        api.project=project; prior[11]=.6; truth=prior; truth[3]=.0015; truth[7]=-.001;
        for(int v=0;v<2;++v) {
            views[v].camera={700,710,528,512,{0,0,0,0},1056,1024};
            auto camera=identity_rigid(); camera[3]=v==0?-.06:.06;
            std::copy(camera.begin(),camera.end(),views[v].head_from_camera);
            views[v].pixels=pixels[v];
        }
    }
    void blobs(int v,std::initializer_list<int> leds) {
        views[v].count=0;
        for(int led:leds) {
            check(few_led_detail::project(api,model,views[v],truth,led,pixels[v]+2*views[v].count,false),"projection");
            ++views[v].count;
        }
    }
};
Fixture* detector_source=nullptr;
int detect(const uint8_t* image,int,int,int,int,int,double* blobs,int capacity) {
    const int v=image[0]; const auto& view=detector_source->views[v];
    if(view.count>size_t(capacity)) return -1;
    for(size_t i=0;i<view.count;++i) {
        blobs[5*i]=view.pixels[2*i]; blobs[5*i+1]=view.pixels[2*i+1];
        blobs[5*i+2]=1; blobs[5*i+3]=255; blobs[5*i+4]=3;
    }
    return int(view.count);
}
int no_solve(const double*,const OpticalView*,const double*,OpticalFit*) { return 1; }
}
int main() { try {
    for(int scenario=0;scenario<5;++scenario) {
        Fixture f;
        if(scenario==0) { f.blobs(0,{0}); f.blobs(1,{0}); }
        if(scenario==1) f.blobs(0,{0,2});
        if(scenario==2) { f.blobs(0,{0,2}); f.blobs(1,{0}); }
        if(scenario==3) { f.blobs(0,{0,1,2}); f.blobs(1,{0,1,2}); }
        if(scenario==4) { f.blobs(0,{0,1,2}); f.blobs(1,{0,1}); }
        OpticalFit fit{}; int n=0; bool rotation=false;
        check(solve_few_led(f.api,model,f.views,f.prior,.033,fit,n,rotation),"few LED solve");
        check(n>=2,"measured count");
        double error=0;
        for(int k=0;k<3;++k) error+=std::pow(fit.pose[4*k+3]-f.truth[4*k+3],2);
        check(std::sqrt(error)<.0001,"translation truth");
        check(fit.residual<.001,"residual");
        if(scenario<3) check(!rotation,"underconstrained rotation must remain prior");
        else check(rotation,"constrained rotation refinement");
        std::cout<<"synthetic scenario="<<scenario<<" blobs="<<n<<" error_mm="<<1000*std::sqrt(error)<<" residual_px="<<fit.residual<<'\n';
    }
    Fixture f; f.blobs(0,{0});
    OpticalFit fit{}; int n=0; bool rotation=false;
    check(!solve_few_led(f.api,model,f.views,f.prior,.033,fit,n,rotation),"one monocular ray rejected");
    f.blobs(1,{0});
    for(double dt:{0.,-.01,.3,1.}) check(!solve_few_led(f.api,model,f.views,f.prior,dt,fit,n,rotation),"stale/noncausal prior rejected");
    auto bad=f.prior; bad[0]=std::numeric_limits<double>::quiet_NaN();
    check(!solve_few_led(f.api,model,f.views,bad,.03,fit,n,rotation),"nonfinite rejected");
    // Zero stereo baseline is rank deficient even with two observations.
    std::copy(f.views[0].head_from_camera,f.views[0].head_from_camera+16,f.views[1].head_from_camera);
    f.blobs(1,{0});
    check(!solve_few_led(f.api,model,f.views,f.prior,.03,fit,n,rotation),"zero baseline rejected");
    Fixture ambiguous; ambiguous.blobs(0,{0}); ambiguous.blobs(1,{0});
    auto duplicate=model; for(int k=0;k<3;++k) duplicate[3+k]=duplicate[k];
    check(!solve_few_led(ambiguous.api,duplicate,ambiguous.views,ambiguous.prior,.03,fit,n,rotation),"two LED claims rejected");
    ambiguous.views[0].count=2; ambiguous.pixels[0][2]=ambiguous.pixels[0][0]; ambiguous.pixels[0][3]=ambiguous.pixels[0][1];
    check(!solve_few_led(ambiguous.api,model,ambiguous.views,ambiguous.prior,.03,fit,n,rotation),"two blob claims rejected");
    Fixture back; back.truth[10]=-1; back.truth[0]=-1; back.prior=back.truth;
    back.blobs(0,{0}); back.blobs(1,{0});
    check(!solve_few_led(back.api,model,back.views,back.prior,.03,fit,n,rotation),"back facing LEDs rejected");
    FewLedHistory history;
    IdentityCandidate full; full.pair=std::make_shared<IdentityPair>(); full.world=f.prior; full.stamp=1000000000;
    check(!history.predict(full.stamp+1000000,{}),"no blind prior");
    history.accepted(full);
    check(bool(history.predict(full.stamp+33000000,{})),"fresh full lock");
    full.few_led=true; full.stamp+=250000000; history.accepted(full);
    check(bool(history.predict(1300000000,{})),"measured few fit renews lock");
    check(!history.predict(1550000000,{}),"prediction cannot renew lock without measurements");
    Fixture integration; integration.blobs(0,{0}); integration.blobs(1,{0}); detector_source=&integration;
    integration.api.detect=detect; integration.api.solve=no_solve;
    auto pair=std::make_shared<IdentityPair>(); pair->timestamp_matched=pair->direct_capture=true;
    for(int v=0;v<2;++v) {
        auto& frame=pair->frames[size_t(v)]; frame.serial=v?"CAMUPPERRIGHT":"CAMUPPERLEFT";
        frame.timestamp_matched=true; frame.stamp=2033000000; frame.exposure_us=800;
        frame.view.camera=integration.views[v].camera;
        std::copy(integration.views[v].head_from_camera,integration.views[v].head_from_camera+16,frame.view.head_from_camera.begin());
        frame.pixels.resize(1056u*1024u); frame.pixels[0]=uint8_t(v); frame.metadata="synthetic"+std::to_string(v);
    }
    check(!identity_stereo_candidate(integration.api,model,pair,2040000000,integration.prior.data()).pair,"regression: old path rejects 1+1");
    const auto sparse=identity_few_led_candidate(integration.api,model,pair,2040000000,integration.prior,2000000000);
    check(sparse.pair && sparse.few_led && sparse.inliers==1 && sparse.matched_observations==2,"new path admits measured 1+1");
    check(identity_observation(pair,0,sparse,2040000000).world.has_value(),"provenance admits few fit");
    auto wrong=sparse; wrong.metadata[0]+="wrong";
    check(!identity_observation(pair,0,wrong,2040000000).stamp,"wrong provenance still rejected");
    check(!identity_few_led_candidate(integration.api,model,pair,2040000000,integration.prior,1700000000).pair,"expired lock still rejected");
    integration.pixels[0][1]+=3; integration.pixels[1][1]-=3;
    check(!solve_few_led(integration.api,model,integration.views,integration.prior,.033,fit,n,rotation),"inconsistent stereo residual rejected");
    Fixture rotated;
    const double delta[6]{0,0,0,.01,-.015,.02};
    rotated.truth=few_led_detail::step(rotated.truth,delta,6);
    rotated.blobs(0,{0,1,2}); rotated.blobs(1,{0,1,2});
    check(solve_few_led(rotated.api,model,rotated.views,rotated.prior,.033,fit,n,rotation),"rotating three LEDs");
    Rigid recovered; std::copy(std::begin(fit.pose),std::end(fit.pose),recovered.begin());
    check(rotation && OpticalPrior::angle(recovered,rotated.truth)<.001,"three LED rotation truth");
    std::cout<<"synthetic rotation error_deg="<<OpticalPrior::angle(recovered,rotated.truth)<<'\n';
    // The fallback propagates optical angular velocity; it does not silently hold orientation.
    FewLedHistory fallback;
    IdentityCandidate a; a.pair=pair; a.world=rotated.prior; a.stamp=3000000000;
    fallback.accepted(a); a.world=rotated.truth; a.stamp+=33000000; fallback.accepted(a);
    const auto extrapolated=fallback.predict(a.stamp+33000000,{});
    const auto expected=few_led_detail::step(a.world,delta,6);
    check(extrapolated && OpticalPrior::angle(*extrapolated,expected)<.001,"optical angular velocity fallback");
    Fixture jump; jump.truth[11]+=.045; jump.blobs(0,{4}); jump.blobs(1,{4});
    check(!solve_few_led(jump.api,model,jump.views,jump.prior,.001,fit,n,rotation),"speed-bound jump rejected");
    for(int scenario=0;scenario<3;++scenario) {
        double max_error=0; int accepted=0;
        for(int sample=0;sample<20;++sample) {
            Fixture noisy;
            if(scenario==0) { noisy.blobs(0,{0}); noisy.blobs(1,{0}); }
            if(scenario==1) noisy.blobs(0,{0,2});
            if(scenario==2) { noisy.blobs(0,{0,1,2}); noisy.blobs(1,{0,1,2}); }
            for(int v=0;v<2;++v) for(size_t i=0;i<2*noisy.views[v].count;++i)
                noisy.pixels[v][i]+=.2*std::sin(double(sample*17+v*11+int(i)*5));
            if(solve_few_led(noisy.api,model,noisy.views,noisy.prior,.033,fit,n,rotation)) {
                ++accepted; double error=0;
                for(int k=0;k<3;++k) error+=std::pow(fit.pose[4*k+3]-noisy.truth[4*k+3],2);
                max_error=std::max(max_error,1000*std::sqrt(error));
            }
        }
        check(accepted==20 && max_error<10,"0.2-pixel noise stability");
        std::cout<<"synthetic noise scenario="<<scenario<<" accepted="<<accepted<<"/20 max_error_mm="<<max_error<<'\n';
    }
    // Small, distant constellation: a fixed 8 px gate marks even exact projections contested.
    // Use synthetic 36 mm geometry, never private factory coordinates.
    Fixture small; auto small_model=model;
    for(double& value:small_model) value*=.4;
    small.truth=small.prior; small.truth[11]=small.prior[11]=1.8;
    for(int v=0;v<2;++v) {
        small.views[v].count=2;
        for(int i=0;i<2;++i) check(few_led_detail::project(small.api,small_model,small.views[v],small.truth,i*2,small.pixels[v]+2*i,false),"small projection");
    }
    check(solve_few_led(small.api,small_model,small.views,small.prior,.033,fit,n,rotation),"36 mm constellation at 1.8 m must continue at exact prior");
    std::cout<<"few LED tests PASS "<<checks<<'\n';
} catch(const std::exception& e) { std::cerr<<"few LED check "<<checks<<": "<<e.what()<<'\n'; return 1; }
}
