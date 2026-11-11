#include "device_config.hpp"
#include <cstdlib>
#include "guard_evidence.hpp"
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <utility>
#include "native_identity.hpp"
#include <fstream>
#include <limits>
#include "mono_solver.hpp"
#include "few_led.hpp"
#include "prior_consensus.hpp"
#include "optical_compute.hpp"
#include "query_peer.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace pico {
namespace {
void *open_library(const std::string &path) {
#ifdef _WIN32
    return reinterpret_cast<void *>(LoadLibraryA(path.c_str()));
#else
    return dlopen(path.c_str(),RTLD_NOW|RTLD_LOCAL);
#endif
}
void close_library(void *handle) {
    if(!handle) return;
#ifdef _WIN32
    FreeLibrary(static_cast<HMODULE>(handle));
#else
    dlclose(handle);
#endif
}
template<class T> T library_symbol(void *handle,const char *name) {
#ifdef _WIN32
    return reinterpret_cast<T>(GetProcAddress(static_cast<HMODULE>(handle),name));
#else
    return reinterpret_cast<T>(dlsym(handle,name));
#endif
}
const std::string serials[] = {pico::devices::upper(0), pico::devices::upper(1)};
bool time_valid(int64_t t) { return t > 0 && t <= std::numeric_limits<int64_t>::max()/4; }
bool fresh(int64_t stamp, int64_t now) {
    return time_valid(stamp) && time_valid(now) && now >= stamp && now-stamp <= 250000000;
}
void require(bool value, const char *reason) { if (!value) throw std::invalid_argument(reason); }
Rigid multiply(const Rigid &a, const Rigid &b) {
    Rigid c{};
    for (int r=0;r<4;++r) for (int col=0;col<4;++col)
        for (int k=0;k<4;++k) c[r*4+col]+=a[r*4+k]*b[k*4+col];
    return c;
}
std::array<double,3> point(const Rigid &a, const double *p) {
    std::array<double,3> out{};
    for (int r=0;r<3;++r) { out[r]=a[r*4+3]; for(int c=0;c<3;++c) out[r]+=a[r*4+c]*p[c]; }
    return out;
}
std::optional<Rigid> inverse(const Rigid &input) {
    // Python uses a full inverse, including the admitted rigid tolerances.
    // A transpose shortcut can shift a ceil/floor ROI boundary by one pixel.
    auto a=input, b=identity_rigid();
    for(int c=0;c<4;++c) {
        int pivot=c;
        for(int r=c+1;r<4;++r) if(std::abs(a[r*4+c])>std::abs(a[pivot*4+c])) pivot=r;
        const double divisor=a[pivot*4+c];
        if(!std::isfinite(divisor) || divisor==0) return {};
        for(int k=0;k<4;++k) { std::swap(a[c*4+k],a[pivot*4+k]); std::swap(b[c*4+k],b[pivot*4+k]); }
        for(int k=0;k<4;++k) { a[c*4+k]/=divisor; b[c*4+k]/=divisor; }
        for(int r=0;r<4;++r) if(r!=c) {
            const double scale=a[r*4+c];
            for(int k=0;k<4;++k) { a[r*4+k]-=scale*a[c*4+k]; b[r*4+k]-=scale*b[c*4+k]; }
        }
    }
    for(double x:b) if(!std::isfinite(x)) return {};
    return b;
}
bool camera_valid(const IdentityView &v,int width=1056,int height=1024) {
    const auto &c=v.camera;
    if(c.width!=width || c.height!=height || !std::isfinite(c.fx) || !std::isfinite(c.fy) ||
       c.fx<=0 || c.fy<=0 || !std::isfinite(c.cx) || !std::isfinite(c.cy) ||
       !identity_rigid_valid(v.head_from_camera)) return false;
    for(double k:c.k) if(!std::isfinite(k)) return false;
    const bool zero=std::abs(v.pixel_offset[0])<=1e-4 && std::abs(v.pixel_offset[1])<=1e-4;
    const bool half=std::abs(v.pixel_offset[0]-.5)<=1e-4 && std::abs(v.pixel_offset[1]-.5)<=1e-4;
    return zero || half;
}
bool pair_valid(const IdentityPair &p) {
    if(!p.timestamp_matched || p.clock_error<0 || p.clock_error>500000) return false;
    for(size_t i=0;i<2;++i) {
        const auto &f=p.frames[i];
        if(f.serial!=serials[i] || !f.timestamp_matched || f.width!=1056 || f.height!=1024 ||
           f.pixels.size()!=1056u*1024u || !time_valid(f.stamp) ||
           !std::isfinite(f.exposure_us) || (p.direct_capture ? (f.exposure_us<=0 || f.exposure_us>=1000) : (f.exposure_us<750 || f.exposure_us>850)) ||
           !identity_rigid_valid(f.world_from_head) || !camera_valid(f.view)) return false;
    }
    for(const auto &f:p.lower) if(!identity_lower_valid(p,f)) return false;
    return std::abs(p.frames[0].stamp-p.frames[1].stamp)<=(p.direct_capture?1000000:2000);
}
constexpr int lower_width=640, lower_height=480;
constexpr int64_t lower_skew_ns=5000000;
uint8_t box_mean11(const std::vector<uint8_t> &pixels,int width,int height,int x,int y) {
    unsigned sum=0;
    for(int dy=-5;dy<=5;++dy) for(int dx=-5;dx<=5;++dx)
        sum+=pixels[size_t(std::clamp(y+dy,0,height-1))*size_t(width)+size_t(std::clamp(x+dx,0,width-1))];
    return uint8_t((sum+60)/121);
}
// Native detector into one calibrated view; false (logged) on detector failure.
bool detect_view(const NativeOpticalApi &api,const IdentityExposure &f,OpticalView &view,std::vector<double> &pixels) {
    std::vector<double> accepted;
    const auto *mask=IdentityBlobMask::current;
    const int cached=!mask?-1:f.pixels.data()==mask->source[0]?0:f.pixels.data()==mask->source[1]?1:-1;
    if(cached>=0 && mask->detector==api.detect) {
        // Multi-tracker optics: same detector result, without the blobs another tracker claims.
        const auto &b=mask->blobs[size_t(cached)];
        for(size_t i=0;i<b.size()/5;++i) if(!mask->excluded[size_t(cached)][i]) accepted.insert(accepted.end(),b.begin()+5*i,b.begin()+5*i+5);
    } else if(!identity_detect_blobs(api,f,accepted)) return false;
    else if(cached>=0) {
        // Another detector ran here: leave out its blobs at the place of an excluded one.
        const auto &b=mask->blobs[size_t(cached)];
        std::vector<double> kept;
        for(size_t i=0;i<accepted.size()/5;++i) {
            bool excluded=false;
            for(size_t j=0;j<b.size()/5 && !excluded;++j)
                excluded=mask->excluded[size_t(cached)][j] && std::hypot(accepted[5*i]-b[5*j],accepted[5*i+1]-b[5*j+1])<=1.;
            if(!excluded) kept.insert(kept.end(),accepted.begin()+5*i,accepted.begin()+5*i+5);
        }
        accepted.swap(kept);
    }
    pixels=compact_led_cluster(accepted);
    const int count=int(pixels.size()/2);
    view.camera=f.view.camera;
    const auto transform=multiply(f.world_from_head,f.view.head_from_camera);
    std::copy(transform.begin(),transform.end(),view.head_from_camera);
    view.count=size_t(count); view.pixels=pixels.data();
    return true;
}
}
bool identity_detect_blobs(const NativeOpticalApi &api,const IdentityExposure &f,std::vector<double> &accepted) {
    const int limit=1024; // bound repeated detector allocation/work on pathological clutter
    int capacity=256, count=-1;
    std::vector<double> blobs;
    while(true) {
        blobs.resize(size_t(capacity)*5);
        count=api.detect(f.pixels.data(),f.height,f.width,180,2,250,blobs.data(),capacity);
        if(count>=0) break;
        if(capacity>=limit) { log_optical_drop("Native detector capacity dropped view"); return false; }
        capacity=std::min(capacity*2,limit);
    }
    if(count>capacity) { log_optical_drop("Native detector failed"); return false; }
    accepted.clear();
    for(int i=0;i<count;++i) {
        const double x=blobs[size_t(i)*5]+f.view.pixel_offset[0], y=blobs[size_t(i)*5+1]+f.view.pixel_offset[1];
        if(!std::isfinite(x)||!std::isfinite(y)) { log_optical_drop("Nonfinite native blob"); return false; }
        // One lens-rim glint outside the KB domain would otherwise fail the ray inversion of the whole view.
        if(!identity_pixel_in_ray_domain(f.view.camera,x,y)) { log_optical_drop("Blob outside camera ray domain"); continue; }
        accepted.insert(accepted.end(),{x,y,blobs[size_t(i)*5+2],blobs[size_t(i)*5+3],blobs[size_t(i)*5+4]});
    }
    return true;
}
bool identity_pixel_in_ray_domain(const OpticalCamera &c,double px,double py) {
    const double x=(px-c.cx)/c.fx, y=(py-c.cy)/c.fy, radius=std::sqrt(x*x+y*y);
    // First theta where the KB derivative stops being positive bounds the invertible radius.
    double limit=std::numeric_limits<double>::infinity();
    for(double theta=0.01;theta<3.14159265358979;theta+=0.01) {
        const double t=theta*theta, d=1+t*(3*c.k[0]+t*(5*c.k[1]+t*(7*c.k[2]+t*9*c.k[3])));
        if(!(d>0)) { limit=(theta-0.01)*(1+(theta-0.01)*(theta-0.01)*(c.k[0]+(theta-0.01)*(theta-0.01)*(c.k[1]+(theta-0.01)*(theta-0.01)*(c.k[2]+(theta-0.01)*(theta-0.01)*c.k[3])))); break; }
    }
    return radius<limit;
}
bool identity_lower_valid(const IdentityPair &p,const IdentityExposure &f) {
    return p.direct_capture && !f.serial.empty() && f.serial!=serials[0] && f.serial!=serials[1] &&
        f.timestamp_matched && f.width==lower_width && f.height==lower_height &&
        f.pixels.size()==size_t(lower_width)*lower_height && time_valid(f.stamp) &&
        std::abs(f.stamp-p.frames[0].stamp)<=lower_skew_ns &&
        std::isfinite(f.exposure_us) && f.exposure_us>0 && f.exposure_us<1000 &&
        identity_rigid_valid(f.world_from_head) && camera_valid(f.view,lower_width,lower_height);
}
int identity_candidate_rank(const IdentityCandidate &c) {
    if(!c.pair) return 3;
    if(c.stereo_verified) return 0;
    if(c.mono && c.lower_view<0) return 1;
    return c.mono ? 2 : 3;
}
bool identity_candidate_better(const IdentityCandidate &a,const IdentityCandidate &b) {
    const int ra=identity_candidate_rank(a), rb=identity_candidate_rank(b);
    return ra!=rb ? ra<rb : a.residual<b.residual;
}
NativeOpticalLibraries::NativeOpticalLibraries(const std::string &optics_path,const std::string &detector_path) {
    optics_=open_library(optics_path); detector_=open_library(detector_path);
    if(optics_ && detector_) {
        api_.project=library_symbol<NativeOpticalApi::Project>(optics_,"pico_project");
        api_.solve=library_symbol<NativeOpticalApi::Solve>(optics_,"pico_solve");
        api_.detect=library_symbol<NativeOpticalApi::Detect>(detector_,"pico_detect");
    }
    if(!api_.project || !api_.solve || !api_.detect) {
        close_library(detector_); close_library(optics_);
        throw std::runtime_error("Native optics/detector libraries or exact ABI symbols unavailable");
    }
}
NativeOpticalLibraries::~NativeOpticalLibraries() { close_library(detector_); close_library(optics_); }
Rigid identity_rigid() { Rigid a{}; a[0]=a[5]=a[10]=a[15]=1; return a; }
bool identity_rigid_valid(const Rigid &a) {
    for(double x:a) if(!std::isfinite(x)) return false;
    for(int c=0;c<4;++c) if(std::abs(a[12+c]-(c==3?1.:0.))>1e-6+1e-5*(c==3?1.:0.)) return false;
    for(int r=0;r<3;++r) for(int c=0;c<3;++c) {
        double sum=0; for(int k=0;k<3;++k) sum+=a[k*4+r]*a[k*4+c];
        if(std::abs(sum-(r==c?1.:0.))>1e-4+1e-5*(r==c?1.:0.)) return false;
    }
    const double det=a[0]*(a[5]*a[10]-a[6]*a[9])-a[1]*(a[4]*a[10]-a[6]*a[8])+a[2]*(a[4]*a[9]-a[5]*a[8]);
    return det>=.999;
}
namespace {
std::atomic<uint64_t> stale_drops{0};
std::mutex drop_mutex;
std::map<std::string,uint64_t> drop_counts;
uint64_t drop_total=0;
std::string last_drop;
}
uint64_t identity_stale_drops(){return stale_drops.load();}
void log_optical_drop(const char *reason) {
    std::lock_guard<std::mutex> lock(drop_mutex);
    const auto count=++drop_counts[reason]; ++drop_total; last_drop=reason;
    if(count%30==1) std::cerr<<"optical_drop reason=\""<<reason<<"\" count="<<count<<" total="<<drop_total<<'\n';
}
uint64_t optical_drops() { std::lock_guard<std::mutex> lock(drop_mutex); return drop_total; }
uint64_t optical_drops(const std::string &reason) {
    std::lock_guard<std::mutex> lock(drop_mutex);
    const auto i=drop_counts.find(reason); return i==drop_counts.end()?0:i->second;
}
std::string optical_last_drop() { std::lock_guard<std::mutex> lock(drop_mutex); return last_drop; }
IdentityObservation identity_observation(const std::shared_ptr<const IdentityPair> &p, int64_t index,
    const std::optional<IdentityCandidate> &candidate, int64_t now) {
    require(p && index>=0, "Identity observation pair and frame index required");
    // Measured-data checks drop this pair (stamp=0) and log; they never throw.
    auto drop=[](const char *reason) {
        log_optical_drop(reason);
        pico::guard_evidence::emit(16,"image-age-provenance",false);
        return IdentityObservation{};
    };
    if(!pair_valid(*p)) return drop("Actual full decoded matched stereo required");
    for(const auto &f:p->frames)
        if(!fresh(f.stamp,now)){++stale_drops;return drop("Stale/future identity exposure");}
    IdentityObservation o;
    o.index=index; o.stamp=std::min(p->frames[0].stamp,p->frames[1].stamp); o.error=p->clock_error; o.pair=*p;
    if(candidate) {
        const auto &c=*candidate;
        const bool upper_mono=c.mono && c.lower_view<0 && c.mono_view>=0 && c.mono_view<2;
        const bool lower_mono=c.mono && c.mono_view<0 && c.lower_view>=0 && size_t(c.lower_view)<p->lower.size();
        const bool few=c.few_led && c.prior_continuation && c.lower_view<0 &&
            c.inliers>=1 && c.matched_observations>=2 && c.matched_observations<=16;
        if(!(c.pair.get()==p.get() && ((c.stereo_verified && c.lower_view<0) || upper_mono || lower_mono) && (few || c.inliers>=4 || (c.inliers==3 && c.stereo_verified && c.prior_continuation)) && c.inliers<=8 &&
            identity_rigid_valid(c.world))) return drop("Same-exposure measured lit bracket required");
        if(!(c.stamp>=o.stamp && c.stamp<=std::max(p->frames[0].stamp,p->frames[1].stamp)))
            return drop("Candidate timestamp mismatch");
        for(size_t i=0;i<2;++i) if(!(c.metadata[i]==p->frames[i].metadata && c.stamps[i]==p->frames[i].stamp &&
            c.heads[i]==p->frames[i].world_from_head)) return drop("Candidate exposure/head provenance mismatch");
        o.world=c.world;
    }
    pico::guard_evidence::emit(16,"image-age-provenance",true);
    return o;
}
IdentityCandidate identity_stereo_candidate(const NativeOpticalApi &api, const LedModel &model,
    const std::shared_ptr<const IdentityPair> &p, int64_t now,const double *prior,bool allow_three) {
    require(api.detect && api.solve, "Native detector/solver ABI required");
    if(!identity_observation(p,0,{},now).stamp)return {};
    for(double x:model) require(std::isfinite(x), "Finite factory model required");
    // A pair without a measured fit returns an unverified candidate (logged); the
    // caller keeps it as unlit identity evidence and never publishes it as a pose.
    auto no_fit=[](const char *reason) {
        ++recovery_diagnostics[std::string("full/")+reason];
        log_optical_drop(reason); return IdentityCandidate{};
    };
    OpticalView views[2]{};
    std::array<std::vector<double>,2> pixels;
    for(size_t v=0;v<2;++v) if(!detect_view(api,p->frames[v],views[v],pixels[v])) return {};
    OpticalFit fit{};
    bool solver_seeded=false;
    const bool stereo=(views[0].count>=4 && views[1].count>=4) ||
        (allow_three && prior && views[0].count>=3 && views[1].count>=3);
    const int mono_view=views[0].count>=4?0:1;
    if(!stereo && views[mono_view].count<4) return no_fit("Four measured blobs per stereo view required");
    if(!OpticalComputeBudget::start_solve()) return no_fit("Pair solver budget exhausted");
    if(stereo) {
        // Blind acquisition is retained. A seeded continuation recovers rejected
        // constellations (11/12) and resolves symmetric alternatives by the
        // smallest orientation innovation. Three-point pairs never go blind.
        const bool full=views[0].count>=4 && views[1].count>=4;
        int solve_rc=full?api.solve(model.data(),views,nullptr,&fit):1;
        if(prior) {
            OpticalFit seeded{};
            const int seeded_rc=api.solve(model.data(),views,prior,&seeded);
            auto innovation=[&](const OpticalFit& f){
                double trace=0;for(int i=0;i<3;++i)for(int k=0;k<3;++k)trace+=prior[4*k+i]*f.pose[4*k+i];
                return std::acos(std::clamp((trace-1)/2,-1.,1.));
            };
            if(seeded_rc==0 && (solve_rc!=0 || innovation(seeded)<=innovation(fit))) {fit=seeded;solve_rc=0;solver_seeded=true;}
        }
        if(solve_rc!=0) {
            ++recovery_diagnostics["full/stereo-rc-"+std::to_string(solve_rc)];
            static uint64_t rejected=0;
            if(++rejected%60==1) { std::ofstream d("/tmp/pico-diag.log",std::ios::app); d<<"stereo_reject rc="<<solve_rc;
                for(size_t v=0;v<2;++v) { d<<" v"<<v<<" n="<<views[v].count<<":"; for(size_t i=0;i<views[v].count&&i<24;++i) d<<" "<<int(pixels[v][i*2])<<","<<int(pixels[v][i*2+1]); }
                d<<std::endl; }
            return no_fit("Native stereo solve rejected");
        }
    } else if(!solve_monocular(api,model,views[mono_view],fit,tracking_recovery_enabled()?prior:nullptr)) return no_fit("Native mono solve residual/ambiguity rejected");
    IdentityCandidate out;
    out.solver_kind=stereo?(solver_seeded?"seeded full":"blind full"):(fit.continuation?"seeded mono":"blind mono");
    out.prior_continuation=prior && fit.continuation!=0;
    out.seeded=solver_seeded || out.prior_continuation;
    out.residual=fit.residual; out.condition=fit.condition;
    out.position_std=fit.position_std; out.rotation_std_deg=fit.rotation_std_deg;
    out.pair=p; out.inliers=fit.count; out.stereo_verified=stereo; out.mono=!stereo; out.mono_view=stereo?-1:mono_view;
    out.stamp=std::min(p->frames[0].stamp,p->frames[1].stamp);
    std::copy(std::begin(fit.pose),std::end(fit.pose),out.world.begin());
    for(size_t i=0;i<2;++i) { out.metadata[i]=p->frames[i].metadata; out.stamps[i]=p->frames[i].stamp; out.heads[i]=p->frames[i].world_from_head; }
    if(!identity_observation(p,0,out,now).stamp) return {}; // logged by identity_observation
    return out;
}
IdentityCandidate identity_few_led_candidate(const NativeOpticalApi& api,const LedModel& model,
    const std::shared_ptr<const IdentityPair>& p,int64_t now,const Rigid& prior,int64_t anchor,int64_t prior_stamp,bool recovery) {
    if(!few_led_enabled() || !p || !api.project || !identity_observation(p,0,{},now).stamp) return {};
    const int64_t stamp=std::min(p->frames[0].stamp,p->frames[1].stamp);
    recovery=recovery && tracking_recovery_enabled();
    if(anchor<=0 || stamp<=anchor || stamp-anchor>=(recovery?prior_hold_ns():300000000)) return {};
    OpticalView views[2]{};
    std::array<std::vector<double>,2> pixels;
    for(size_t v=0;v<2;++v) if(!detect_view(api,p->frames[v],views[v],pixels[v])) return {};
    OpticalFit fit{}; int observations=0; bool rotation=false;
    const double dt=double(stamp-(prior_stamp?prior_stamp:anchor))*1e-9;
    const bool consensus=recovery && solve_prior_consensus(api,model,views,prior,dt,fit,observations,rotation);
    if(!consensus &&
       !solve_few_led(api,model,views,prior,dt,fit,observations,rotation,recovery)) {
        log_optical_drop("Few LED prior fit rejected (matching/rank/residual/speed/covariance)"); return {};
    }
    IdentityCandidate out;
    out.solver_kind=consensus?"consensus":"few-LED";
    out.pair=p; out.stamp=stamp; out.few_led=true; out.prior_continuation=true; out.seeded=true;
    out.few_led_rotation=rotation; out.matched_observations=observations;
    out.inliers=fit.count; out.residual=fit.residual; out.condition=fit.condition;
    out.position_std=fit.position_std; out.rotation_std_deg=fit.rotation_std_deg;
    bool left=false,right=false;
    for(int i=0;i<fit.count;++i) { left=left || fit.matches[i].i>=0; right=right || fit.matches[i].j>=0; }
    out.stereo_verified=left && right; out.mono=!out.stereo_verified;
    out.mono_view=out.mono?(left?0:1):-1;
    std::copy(std::begin(fit.pose),std::end(fit.pose),out.world.begin());
    for(size_t i=0;i<2;++i) {
        out.metadata[i]=p->frames[i].metadata; out.stamps[i]=p->frames[i].stamp;
        out.heads[i]=p->frames[i].world_from_head;
    }
    if(!identity_observation(p,0,out,now).stamp) return {};
    return out;
}
IdentityCandidate identity_lower_candidate(const NativeOpticalApi &api, const LedModel &model,
    const std::shared_ptr<const IdentityPair> &p, int64_t now) {
    require(api.detect && api.project, "Native detector/projection ABI required");
    if(!p || p->lower.empty() || !identity_observation(p,0,{},now).stamp) return {};
    for(double x:model) require(std::isfinite(x), "Finite factory model required");
    IdentityCandidate best;
    for(size_t v=0;v<p->lower.size() && v<2;++v) {
        if(OpticalComputeBudget::exhausted()){log_optical_drop("Pair solver budget exhausted");break;}
        OpticalView view{};
        std::vector<double> pixels;
        if(!detect_view(api,p->lower[v],view,pixels)) continue;
        if(view.count<4) { log_optical_drop("Four measured blobs per lower view required"); continue; }
        OpticalFit fit{};
        if(!OpticalComputeBudget::start_solve())break;
        if(!solve_monocular(api,model,view,fit)) { log_optical_drop("Native lower mono solve residual/ambiguity rejected"); continue; }
        IdentityCandidate c;
        c.residual=fit.residual; c.condition=fit.condition;
        c.position_std=fit.position_std; c.rotation_std_deg=fit.rotation_std_deg;
        c.pair=p; c.inliers=fit.count; c.mono=true; c.lower_view=int(v);
        // The identity observation is the upper exposure; the lower frame is the same LED flash.
        c.stamp=std::min(p->frames[0].stamp,p->frames[1].stamp);
        std::copy(std::begin(fit.pose),std::end(fit.pose),c.world.begin());
        for(size_t i=0;i<2;++i) { c.metadata[i]=p->frames[i].metadata; c.stamps[i]=p->frames[i].stamp; c.heads[i]=p->frames[i].world_from_head; }
        if(!best.pair || identity_candidate_better(c,best)) best=std::move(c);
    }
    if(best.pair && !identity_observation(p,0,best,now).stamp) return {};
    return best;
}
uint8_t identity_box_mean11(const std::vector<uint8_t> &pixels, int x, int y) {
    require(pixels.size()==1056u*1024u && x>=0 && x<1056 && y>=0 && y<1024,"Full-image box11 coordinate required");
    return box_mean11(pixels,1056,1024,x,y);
}
NativeLedModeIdentity::NativeLedModeIdentity(uint32_t chip, LedModel model, NativeOpticalApi api)
    : chip_(chip), model_(model), api_(api) {
    require(chip && api.project,"Exact chip and native projection ABI required");
    for(double x:model) require(std::isfinite(x),"Finite factory eight-LED model required");
}
void NativeLedModeIdentity::configure(const std::vector<uint32_t> &chips, const std::string &link) {
    require(!link.empty(),"Fresh link token required");
    const bool enabled=chips==std::vector<uint32_t>{chip_};
    if(link!=link_ || !enabled || !configured_) { observations.clear(); confirmations.clear(); }
    link_=link; configured_=enabled;
}
bool NativeLedModeIdentity::observe(IdentityObservation o) {
    if(!configured_ || !time_valid(o.stamp) || o.index<0 || !pair_valid(o.pair) ||
       o.stamp!=std::min(o.pair.frames[0].stamp,o.pair.frames[1].stamp) || o.error!=o.pair.clock_error ||
       (o.world && !identity_rigid_valid(*o.world))) return false;
    if(!observations.empty() && o.stamp<=observations.back().stamp) {
        // Hardware 2026-10-29: one future-stamped observation blocked every later frame for the whole session
        // (identity stuck). A jump back of more than 200 ms restarts the history instead of rejecting forever.
        if(observations.back().stamp-o.stamp<=200000000LL) return false;
        log_optical_drop("Identity observation stamp went back >200 ms; history restarted");
        observations.clear(); confirmations.clear();
    }
    const auto stamp=o.stamp;
    observations.push_back(std::move(o));
    observations.erase(observations.begin(),std::find_if(observations.begin(),observations.end(),
        [stamp](const IdentityObservation &v){return stamp-v.stamp<=3000000000LL;}));
    return true;
}
bool NativeLedModeIdentity::dark_pixels(const IdentityObservation &o,const IdentityObservation &left,
    const IdentityObservation &right) const {
    bool visible=false;
    // Upper views, then same-exposure lower views (a tracker on the feet may be seen only there).
    std::vector<const IdentityExposure *> frames{&o.pair.frames[0],&o.pair.frames[1]};
    for(const auto &f:o.pair.lower) frames.push_back(&f);
    for(const auto *frame:frames) {
        const auto &f=*frame;
        const auto transform=inverse(multiply(f.world_from_head,f.view.head_from_camera));
        if(!transform) return false;
        double low[2]={1e30,1e30},high[2]={-1e30,-1e30};
        bool projected=true;
        for(const auto *endpoint:{&left,&right}) {
            double local[24],pixels[16];
            for(size_t i=0;i<8;++i) {
                auto w=point(*endpoint->world,model_.data()+3*i); auto p=point(*transform,w.data());
                std::copy(p.begin(),p.end(),local+3*i);
            }
            if(api_.project(&f.view.camera,local,8,pixels)!=0) { projected=false; break; }
            for(size_t i=0;i<8;++i) for(size_t c=0;c<2;++c) {
                const double x=pixels[2*i+c]-f.view.pixel_offset[c];
                if(!std::isfinite(x)) return false;
                low[c]=std::min(low[c],x); high[c]=std::max(high[c],x);
            }
        }
        if(!projected) continue;
        low[0]=std::floor(low[0]-25); low[1]=std::floor(low[1]-25);
        high[0]=std::ceil(high[0]+25); high[1]=std::ceil(high[1]+25);
        if(low[0]<0 || low[1]<0 || high[0]>=f.width || high[1]>=f.height) continue;
        visible=true;
        for(int y=int(low[1]);y<=int(high[1]);++y) for(int x=int(low[0]);x<=int(high[0]);++x)
            if(f.pixels[size_t(y)*size_t(f.width)+size_t(x)]>std::min(255,int(box_mean11(f.pixels,f.width,f.height,x,y))+30)) return false;
    }
    return visible;
}
bool NativeLedModeIdentity::evaluate(const LedEpisode &e) {
    if(!configured_ || confirmations.count(e.token)) return false;
    std::vector<IdentityFrameTime> times;
    for(const auto &o:observations) times.push_back({o.stamp,o.index,o.error,bool(o.world)});
    const auto window=identity_window(e,chip_,times);
    if(!window) return false;
    std::vector<size_t> lit=window->before;
    lit.insert(lit.end(),window->after.begin(),window->after.end());
    double mt=0,mp[3]{};
    for(auto i:lit) { mt+=double(observations[i].stamp-observations[lit[0]].stamp)/1e9; for(int c=0;c<3;++c) mp[c]+=(*observations[i].world)[c*4+3]; }
    mt/=6; for(double &x:mp) x/=6;
    double denom=0,velocity[3]{};
    for(auto i:lit) {
        const double dt=double(observations[i].stamp-observations[lit[0]].stamp)/1e9-mt;
        denom+=dt*dt; for(int c=0;c<3;++c) velocity[c]+=dt*((*observations[i].world)[c*4+3]-mp[c]);
    }
    if(!(denom>0)) return false;
    double speed2=0; for(double &x:velocity) { x/=denom; speed2+=x*x; }
    if(speed2>1.5*1.5) return false;
    for(auto i:lit) {
        double residual2=0;
        const double dt=double(observations[i].stamp-observations[lit[0]].stamp)/1e9-mt;
        for(int c=0;c<3;++c) { double r=(*observations[i].world)[c*4+3]-mp[c]-dt*velocity[c]; residual2+=r*r; }
        if(residual2>.02*.02) return false;
    }
    for(auto i:window->dark) if(!dark_pixels(observations[i],observations[window->before.back()],observations[window->after.front()])) return false;
    IdentityConfirmation proof; proof.episode=e;
    for(auto i:lit) proof.lit.push_back(observations[i].index);
    for(auto i:window->dark) proof.dark.push_back(observations[i].index);
    confirmations.emplace(e.token,std::move(proof));
    return true;
}
bool NativeLedModeIdentity::verified(uint32_t chip,int64_t index,const std::string &link,int64_t now) const {
    return bound() && chip==chip_ && link==link_ && !observations.empty() &&
        index==observations.back().index && fresh(observations.back().stamp,now);
}
void IdentityPhaseWindow::observe(int64_t index,bool lit,bool verified,bool acked) {
    if(state==State::Locked || !acked || index<900) return;
    state=State::Verifying;
    if(!start_) start_=index;
    seen_.insert(index); if(lit) lit_.insert(index);
    if(index-*start_<60) return;
    if(seen_.size()>=48 && lit_.size()>=48 && verified) { state=State::Locked; return; }
    // Not proven in this window: log it and keep verifying in a fresh window.
    std::cerr<<"identity_phase_window_retry attempt="<<++retries_<<" start="<<*start_<<" end="<<index
             <<" seen="<<seen_.size()<<" lit="<<lit_.size()<<" verified="<<verified<<'\n';
    start_.reset(); seen_.clear(); lit_.clear();
}
NativeIdentitySession::NativeIdentitySession(uint32_t chip,LedModel model,std::string link,NativeOpticalApi api)
    : chip_(chip),link_(std::move(link)),matcher_(chip,model,api),owner(chip,link_) { matcher_.configure({chip},link_); }
bool NativeIdentitySession::optical(const std::shared_ptr<const IdentityPair> &pair,int64_t index,
    const std::optional<IdentityCandidate> &candidate,int64_t now) {
    if(invalid_) return false;
    IdentityObservation o;
    try { o=identity_observation(pair,index,candidate,now); }
    catch(const std::invalid_argument &) { ready_stamp_=0; return false; }
    std::lock_guard<std::mutex> lock(optical_);
    if(invalid_ || !matcher_.observe(std::move(o))) { ready_stamp_=0; return false; }
    const auto &all=matcher_.observations;
    bool ready=all.size()>=3;
    if(ready) for(size_t i=all.size()-3;i<all.size();++i) {
        ready=ready && bool(all[i].world);
        if(i>all.size()-3) { auto dt=all[i].stamp-all[i-1].stamp;
            ready=ready && all[i].index-all[i-1].index==1 && dt*5>4*33333333LL && dt*5<6*33333333LL; }
    }
    ready_stamp_=ready?all.back().stamp:0;
    auto episode=owner.snapshot();
    if(episode && matcher_.evaluate(*episode) && !invalid_) restored_token_=episode->token;
    binding_=matcher_.bound() && !invalid_;
    return true;
}
bool NativeIdentitySession::begin_if_ready(PeerEngine &e,const std::string &link,int64_t now,bool controllers) {
    if(!std::getenv("PICO_IDENTITY_EPISODES")) return false; // LED-off episodes blinded the only tracker (48 of 50 failed, 2026-10-30)
    const auto stamp=ready_stamp_.load();
    if(invalid_ || recovery_only_ || owner.abort_requested || link!=link_ || !fresh(stamp,now)) return false;
    return owner.begin(e,link,now,controllers,true);
}
void NativeIdentitySession::radio_tick(PeerEngine &e,const std::string &link,int64_t now) {
    if(owner.abort_requested || link!=link_ || e.chip!=le32(chip_) || e.slot!=0) invalidate();
    if(invalid_) { owner.abort_requested=true; owner.failure="Identity link/owner invalidated; restoration retained"; }
    // An unverified or non-frame mode is a transient peer state: log, keep the
    // session, and let drive_radio wait for a verified mode before a new episode.
    else if(e.mode_profile!="frame" || !e.mode_verified()) log_optical_drop("Identity radio tick without verified frame mode");
    const auto episode=owner.snapshot();
    const auto token=restored_token_.exchange(0);
    if(!invalid_ && episode && token==episode->token) owner.optical_return(e,link,true);
    owner.tick(e,link,now);
}
bool NativeIdentitySession::verified(uint32_t chip,int64_t index,const std::string &link,int64_t now) const {
    if(invalid_) return false;
    std::lock_guard<std::mutex> lock(optical_);
    const bool accepted=!invalid_ && matcher_.verified(chip,index,link,now);
    pico::guard_evidence::emit(15,"two-episodes",matcher_.bound());
    return accepted;
}
void NativeIdentitySession::begin_recovery() {
    recovery_only_ = true;
    // Keep an already completed, immutable episode token. configure/link loss
    // still invalidates it through invalidate(); cleanup alone does not.
    invalid_ = true;
    binding_ = false;
    ready_stamp_ = 0;
}
bool NativeIdentitySession::recovery_optical(const std::shared_ptr<const IdentityPair>& pair,
    int64_t index, const std::optional<IdentityCandidate>& candidate, int64_t now) {
    if (!recovery_only_ || !owner.restore_required()) return false;
    IdentityObservation observation;
    try { observation = identity_observation(pair, index, candidate, now); }
    catch (const std::invalid_argument&) { return false; }
    std::lock_guard<std::mutex> lock(optical_);
    if (!matcher_.observe(std::move(observation))) return false;
    const auto episode = owner.snapshot();
    if (episode && matcher_.evaluate(*episode)) restored_token_ = episode->token;
    return true;
}
bool NativeIdentitySession::recovery_tick(PeerEngine& engine,const std::string& link,int64_t now) {
    if (!recovery_only_) throw std::runtime_error("LED recovery not entered");
    const auto episode = owner.snapshot();
    const auto token = restored_token_.load();
    if (episode && token == episode->token) {
        // Radio alone retires the ledger. ON ACK, exact engine/link and fresh
        // mode plus the full Python measured episode remain mandatory.
        owner.optical_return(engine, link, true);
        restored_token_ = 0;
    }
    owner.tick(engine, link, now);
    engine.native25_exclusive = owner.restore_required();
    return engine.native25_exclusive;
}
void NativeIdentitySession::invalidate() { invalid_=true; binding_=false; ready_stamp_=0; restored_token_=0; }
// Hardware 2026-10-29: StationControl reset_optics used to invalidate the session for good, so identity stopped
// after ~3 s and never came back. A reset now only clears the optical history; earned confirmations and the
// native25 LED ledger stay, and LED recovery (cleanup) keeps its own invalidation.
void NativeIdentitySession::restart() {
    if(recovery_only_) return;
    std::lock_guard<std::mutex> lock(optical_);
    matcher_.observations.clear();
    invalid_=false; ready_stamp_=0; restored_token_=0;
    log_optical_drop("Optics reset: identity optical history restarted");
}
SingleTrackerNativeIdentityHooks::SingleTrackerNativeIdentityHooks(QueryPeer &engine,RoyClock &clock,
    LedModel model,std::string link,NativeOpticalApi api,std::function<int64_t()> now)
    : engine_(engine),session_(pico::devices::chip(),model,link,api),
      bridge_(session_.owner,engine,clock,link,std::move(now)),link_(std::move(link)) {}
bool SingleTrackerNativeIdentityHooks::prepare(bool pipeline_has_target) {
    if(invalid_) { log_optical_drop("Invalidated native identity hooks; continuing without identity"); return false; }
    if(prepared_) {
        if(engine_.mode_profile!="frame" || !engine_.mode_verified() || !pipeline_has_target) {
            log_optical_drop("Identity mode/pipeline changed; waiting");
            return false;
        }
        return true;
    }
    if(!engine_.diagnostics_ready()) return false;
    require(engine_.slot==0 && engine_.chip==le32(pico::devices::chip()),"Unexpected identity target");
    if(engine_.mode_profile!="frame" || !engine_.mode_verified() || !pipeline_has_target) return false;
    prepared_=true; return true;
}
void SingleTrackerNativeIdentityHooks::configure(const std::vector<uint32_t> &chips) {
    if(chips!=std::vector<uint32_t>{pico::devices::chip()}) log_optical_drop("Active identity membership changed; identity kept for first target");
}
void SingleTrackerNativeIdentityHooks::invalidate() { invalid_=true; session_.invalidate(); }
void SingleTrackerNativeIdentityHooks::disconnect() { engine_.disconnect(); invalidate(); }
bool SingleTrackerNativeIdentityHooks::optical(uint32_t chip,const std::shared_ptr<const IdentityPair> &pair,
    int64_t index,const std::optional<IdentityCandidate> &candidate,int64_t now) {
    if(chip!=pico::devices::chip()) { log_optical_drop("Unexpected optical target; pair dropped"); return false; }
    return prepared_ && !invalid_ && session_.optical(pair,index,candidate,now);
}
bool SingleTrackerNativeIdentityHooks::radio_tick(int64_t now,bool guard,bool healthy) {
    if(invalid_ || !guard || !healthy) {
        // Retain restoration/deadline processing on the original engine/link,
        // but begin no new episode while invalid or the guard/worker is unhealthy.
        log_optical_drop(invalid_ ? "Invalid identity session; restoration retained"
                                  : "Fresh controller/HMD guard or optical worker failed; no new episode");
        if(!prepared_ && !invalid_) return false;
        session_.radio_tick(engine_,link_,now);
        {const auto s=session_.owner.snapshot(); engine_.native25_exclusive=s && !s->on_published;}  // exclusive only while LEDs are OFF
        return engine_.native25_exclusive;
    }
    if(!prepared_) return false;
    return session_.drive_radio(engine_,link_,now);
}
bool NativeIdentitySession::drive_radio(PeerEngine &engine,const std::string &link,int64_t now) {
    radio_tick(engine,link,now);
    if(!owner.snapshot() && !engine.pending && engine.queued.empty() &&
       engine.mode_profile=="frame" && engine.mode_verified())
        begin_if_ready(engine,link,now,true);
    // Hardware 2026-10-29: exclusivity during the whole episode blocked the light-sync commands that make the
    // LEDs flash, so the lit return the episode waits for never came (deadlock). Exclusive only until ON goes out.
    {const auto s=owner.snapshot(); engine.native25_exclusive=s && !s->on_published;}
    return engine.native25_exclusive;
}
bool SingleTrackerNativeIdentityHooks::verified(uint32_t chip,int64_t index,int64_t now) const {
    return prepared_ && !invalid_ && session_.verified(chip,index,link_,now);
}
bool SingleTrackerNativeIdentityHooks::bindings_snapshot() const { return prepared_ && !invalid_ && session_.bindings_snapshot(); }
void SingleTrackerNativeIdentityHooks::record(const Record &r,const std::function<void()> &previous) { bridge_.record(r,previous); }
bool SingleTrackerNativeIdentityHooks::publish(MailboxWriter &writer,int slot,const Mailbox &m,
    std::optional<PhaseReference> phase,std::optional<CycleWindow> window) { return bridge_.publish(writer,slot,m,phase,window); }
} // namespace pico


#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_identity, "identity");
