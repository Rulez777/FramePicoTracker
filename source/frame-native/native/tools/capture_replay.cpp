#include "../device_config.hpp"
// Build (aarch64, from native/; frames_bench link set plus pose_publication.cpp):
//   g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -fno-fast-math -ffp-contract=off -pthread -I. -o build/capture_replay tools/capture_replay.cpp pose_publication.cpp replay_calibration.cpp openocd_process.cpp roy_owner.cpp tcl_transport.cpp owner_lifecycle.cpp image_admission.cpp radio_loop.cpp peer_engine.cpp station_control.cpp query_peer.cpp native25.cpp native_identity.cpp data_plane.cpp -ldl
// Usage: build/capture_replay <capture_dir> <private-led-json> <factory-camera-json> <out-wire.bin> [--max-pairs N] [--mono-minus-raw NS]
// Prior: --prior-calibration <replay-calibration.txt> --prior-stage <0|1|2|3> (default 3).
// Stages: 0 pose-only baseline, 1 gyro/four-point, 2 gyro/three-point, 3 + rotation gate.
// Offline replay of a captured session through the live upper-stereo pose path: short-exposure upper pairs from
// <capture_dir>/frames, head pose from <capture_dir>/attempt/driver-proof.jsonl (HeadHistory, causal, live rules),
// identity_stereo_candidate + OpticalWorker fit_valid, OpticalWorker velocity/quality, driver_pose, PublicationExpiry
// sequence, PTP2 encode. out-wire.bin = wire.bin records ([u64 receive ns][u32 len][bytes]) in the run's span with every
// old PTP2 replaced by the new ones at receive = exposure stamp + 12 ms.
// Not reproduced: lower-camera fallback, identity/phase gating (one tracker, identified), expiry (invalid) frames,
// compute-thread scheduling (pairs are never dropped for being late).
//
// Clock domains (from source, 2026-10-30):
//  - frames/videoN_<ts>.pgm: V4L2 timestamp = CLOCK_MONOTONIC_RAW, end of readout (xrtap.h, xrtap_save.c).
//  - Live exposure stamp: min(raw0,raw1)+offset_ns (-10.137 ms, xrtap_source.hpp), then OpticalCameraClock "xrtap-direct"
//    adds MONOTONIC-RAW sampled at conversion (optical_timing.cpp). PTP2 sourceTime is that CLOCK_MONOTONIC stamp.
//  - driver-proof.jsonl t_ns: tracking_watch std::chrono::steady_clock = CLOCK_MONOTONIC (frame_app/tracking_watch.cpp),
//    checked against CLOCK_MONOTONIC now in phase5/tracking_guard.cpp.
//  - wire.bin receive ns: the driver's RunFrame clock and PTI1 receiveTime are CLOCK_MONOTONIC (driver_pico_tracker.cpp,
//    data_plane.cpp). The recorder is not in this tree, so wire-PTI1.receiveTime is measured; a value that is not a small
//    latency (outside -1..100 ms) refuses the run.
//  - Nothing records MONOTONIC-RAW. It is derived from the old PTP2 stamps vs the frame stamps: the 33 ms frame ambiguity
//    is resolved by pose content (identity-head stereo fits vs old PTP2 positions), then tracked across the run (NTP slews
//    MONOTONIC by ppm). The value absorbs any difference between the old run's offset_ns and -10.137 ms.
//    --mono-minus-raw NS overrides it (required when wire.bin holds no old PTP2 poses).
// Exit 2 when inputs/libraries/calibration/clocks cannot be established; otherwise 0.
#include <iterator>
#include "calibration_discovery.hpp"
#include "driver_origin.hpp"
#include "head_history.hpp"
#include "optical_compute.hpp"
#include "optical_prior.hpp"
#include "replay_gyro.hpp"
#include "few_led.hpp"
#include "prior_consensus.hpp"
#include "pose_publication.hpp"
#include "replay_calibration.hpp"
#include <algorithm>
#include <cctype>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace {
using namespace pico;
namespace fs = std::filesystem;
constexpr const char* serials[2] = {"CAMUPPERLEFT", "CAMUPPERRIGHT"}; // video0, video3 (xrtap_source.hpp)
constexpr int cams[2] = {0, 3};
constexpr double exposure_us = 782.267;     // XrtapConfig::exposure_us
constexpr int64_t offset_ns = -10137000;    // XrtapConfig::offset_ns
constexpr int64_t delay_ns = 12000000;      // measured exposure -> driver receive
constexpr uint32_t chip = 0xA1B2C3D4;       // OpticalWorker first target

struct Pgm { int width=0,height=0,maxval=0; size_t header=0; uintmax_t file_size=0; std::vector<uint8_t> pixels; std::string error; };
Pgm read_pgm(const fs::path& path){
    Pgm out;std::ifstream f(path,std::ios::binary);
    if(!f){out.error="unreadable";return out;}
    out.file_size=fs::file_size(path);
    auto token=[&]{
        int c;
        while((c=f.peek())!=EOF){if(c=='#'){std::string skip;std::getline(f,skip);}else if(std::isspace(c))f.get();else break;}
        long v=-1;f>>v;return v;
    };
    std::string magic;f>>magic;
    if(magic!="P5"){out.error="magic '"+magic+"'";return out;}
    out.width=int(token());out.height=int(token());out.maxval=int(token());
    if(!f||out.width<=0||out.height<=0||out.maxval<=0||out.maxval>255){out.error="bad header";return out;}
    f.get();
    out.header=size_t(f.tellg());
    const size_t bytes=size_t(out.width)*size_t(out.height);
    if(out.file_size!=out.header+bytes){out.error="size mismatch";return out;}
    out.pixels.resize(bytes);f.read(reinterpret_cast<char*>(out.pixels.data()),std::streamsize(bytes));
    if(!f)out.error="short read";
    return out;
}
struct Frame { int64_t stamp; fs::path path; };
struct Pair { std::array<Frame,2> f; int64_t exposure; }; // exposure: RAW, offset applied
struct WireRec { uint64_t t; std::string bytes; };
struct OldPose { int64_t src; uint64_t receive; std::array<double,3> p; };
template<class T> T le(const std::string& b,size_t at){T v;std::memcpy(&v,b.data()+at,sizeof v);return v;} // aarch64 is LE
bool is(const WireRec& r,const char* magic,size_t len){return r.bytes.size()==len&&!r.bytes.compare(0,4,magic);}

// Verbatim from optical_worker.cpp.
std::array<double,4> quaternion(const Rigid &m) {
    std::array<double,4> q{};
    double trace=m[0]+m[5]+m[10];
    if(trace>0) {
        double s=2*std::sqrt(trace+1);
        q={(m[9]-m[6])/s,(m[2]-m[8])/s,(m[4]-m[1])/s,s/4};
    } else {
        int i=m[5]>m[0]?1:0; if(m[10]>m[5*i]) i=2;
        int j=(i+1)%3,k=(i+2)%3;
        double s=2*std::sqrt(1+m[5*i]-m[5*j]-m[5*k]);
        q[i]=s/4; q[j]=(m[4*i+j]+m[4*j+i])/s;
        q[k]=(m[4*i+k]+m[4*k+i])/s; q[3]=(m[4*k+j]-m[4*j+k])/s;
    }
    return q;
}
bool fit_valid(const IdentityCandidate &c) {
    return std::isfinite(c.residual) && c.residual>=0 && c.residual<=2.5 &&
        std::isfinite(c.condition) && c.condition>=0 && c.condition<=1e7 &&
        std::isfinite(c.position_std) && c.position_std>=0 && c.position_std<=.03 &&
        std::isfinite(c.rotation_std_deg) && c.rotation_std_deg>=0 && c.rotation_std_deg<=5;
}
Rigid multiply(const Rigid& a,const Rigid& b){
    Rigid o{};
    for(int i=0;i<4;++i)for(int j=0;j<4;++j)for(int k=0;k<4;++k)o[size_t(4*i+j)]+=a[size_t(4*i+k)]*b[size_t(4*k+j)];
    return o;
}
std::string stats(std::vector<double> v,int precision){
    if(v.empty())return "n=0";
    std::sort(v.begin(),v.end());
    auto at=[&](double p){return v[std::min(v.size()-1,size_t(std::max(1.,std::ceil(p*double(v.size()))))-1)];};
    std::ostringstream s;s.setf(std::ios::fixed);s.precision(precision);
    s<<"p50="<<at(.5)<<" p90="<<at(.9)<<" max="<<v.back();return s.str();
}
struct NullBuffer : std::streambuf { int overflow(int c) override { return c; } };

struct Solver {
    NativeOpticalApi api; LedModel model; std::array<IdentityView,2> views;
    std::map<std::string,uint64_t> skipped;
    std::array<std::map<std::string,uint64_t>,4> reasons;
    OpticalPrior gyro; int stage=0;
    std::optional<Rigid> prior; int64_t prior_ns=0;
    FewLedHistory few_history;
    bool few=false,evaluate=false,recovery=false;
    size_t blob_class=0; std::array<uint64_t,4> failed{};
    std::array<std::map<size_t,uint64_t>,2> histogram;
    std::vector<double> few_full_mm,few_full_deg,drop2_mm,drop2_deg,drop3_mm,drop3_deg;
    uint64_t drop_attempts[2]{},few_poses=0,recovery_seeds=0,recovery_attempts=0,recovery_fits=0,recovery_rotation_drops=0;
    void accepted(const IdentityCandidate& c) {
        prior=c.world; prior_ns=c.stamp; gyro.accepted(c.world,c.stamp); few_history.accepted(c);
    }
    void reset() { prior.reset(); prior_ns=0; gyro.missing(); few_history=FewLedHistory{}; }
    // Same IdentityPair the tap consumer builds; stamps/heads already converted. Empty candidate = no fit.
    IdentityCandidate operator()(const Pair& source,const std::array<int64_t,2>& stamps,const std::array<Rigid,2>& heads,int64_t now){
        auto pair=std::make_shared<IdentityPair>();
        pair->timestamp_matched=true;pair->direct_capture=true;pair->clock_error=0;
        for(size_t i=0;i<2;++i){
            auto g=read_pgm(source.f[i].path);
            if(!g.error.empty()){++skipped["video"+std::to_string(cams[i])+" "+g.error];return {};}
            auto& f=pair->frames[i];
            f.serial=serials[i];f.width=g.width;f.height=g.height;f.timestamp_matched=true;
            f.stamp=stamps[i];f.exposure_us=exposure_us;f.view=views[i];f.world_from_head=heads[i];f.pixels=std::move(g.pixels);
            f.metadata="pgm:"+f.serial+":"+source.f[i].path.filename().string();
        }
        try{
            OpticalComputeBudget budget;
            const auto diagnostic_start=recovery_diagnostics;
            const auto prediction=stage?gyro.predict(std::min(stamps[0],stamps[1]),recovery):std::optional<Rigid>{};
            if(recovery && prediction) ++recovery_seeds;
            const double* seed=prediction?prediction->data():
                (!recovery && !few && prior&&now-prior_ns<=300000000?prior->data():nullptr);
            IdentityCandidate c=identity_stereo_candidate(api,model,pair,now,seed,stage>=2);
            if(recovery && prediction && c.pair && !gyro.consistent(c.world,c.stamp,prediction)) {
                ++recovery_rotation_drops; recovery_reject("full/gyro-orientation"); c={};
            }
            if(recovery && prediction && !c.pair) {
                ++recovery_attempts;
                c=identity_few_led_candidate(api,model,pair,now,*prediction,prior_ns,prior_ns,true);
                if(c.pair) ++recovery_fits;
            }
            if(recovery && prediction && c.pair && !gyro.consistent(c.world,c.stamp,prediction)) {
                ++recovery_rotation_drops; c={};
            }
            const auto sparse_prior=few_history.predict(std::min(stamps[0],stamps[1]),prediction,recovery);
            if(recovery && !c.pair && few && sparse_prior)
                c=identity_few_led_candidate(api,model,pair,now,*sparse_prior,few_history.anchor(),few_history.last_stamp());
            if(c.pair && !gyro.supported_candidate(c,prediction,recovery || few)) c={};
            // Evaluation has its own budget and must not overwrite production failure attribution.
            const auto diagnostic_end=recovery_diagnostics;
            const auto production_failure=optical_last_drop();
            if(evaluate) {
                OpticalComputeBudget evaluation_budget;
                const auto reference=identity_stereo_candidate(api,model,pair,now,seed,stage>=2);
                OpticalView optical[2]{};
                std::array<size_t,2> raw_counts{};
                std::array<std::vector<double>,2> pixels;
                for(int v=0;v<2;++v) {
                    std::vector<double> blobs;
                    identity_detect_blobs(api,pair->frames[size_t(v)],blobs);
                    ++histogram[size_t(v)][blobs.size()/5];
                    pixels[size_t(v)]=compact_led_cluster(blobs);
                    auto& view=optical[v]; view.camera=pair->frames[size_t(v)].view.camera;
                    const auto transform=multiply(pair->frames[size_t(v)].world_from_head,pair->frames[size_t(v)].view.head_from_camera);
                    std::copy(transform.begin(),transform.end(),view.head_from_camera);
                    view.count=pixels[size_t(v)].size()/2; view.pixels=pixels[size_t(v)].data();
                    raw_counts[size_t(v)]=blobs.size()/5;
                }
                // PICO_PAIR_TRACE (2026-11-02): per pair raw/clustered blob counts and the full-solver result, to see
                // what the cameras saw inside tracking gaps.
                static std::FILE* trace=[]{const char* p=std::getenv("PICO_PAIR_TRACE");return p?std::fopen(p,"w"):nullptr;}();
                blob_class=raw_counts[0]>=4 && raw_counts[1]>=4?0:std::max(raw_counts[0],raw_counts[1])>=4?1:raw_counts[0]+raw_counts[1]?2:3;
                for(const auto& entry:diagnostic_end) {
                    const auto it=diagnostic_start.find(entry.first);
                    const auto count=entry.second-(it==diagnostic_start.end()?0:it->second);
                    if(count) reasons[blob_class][entry.first]+=count;
                }
                if(!c.pair) ++reasons[blob_class]["pair/"+production_failure];
                if(trace && evaluate) std::fprintf(trace,"%lld %zu %zu %zu %zu %d\n",(long long)std::min(stamps[0],stamps[1]),
                    raw_counts[0],raw_counts[1],optical[0].count,optical[1].count,reference.pair?1:0);
                if(reference.pair && sparse_prior) {
                    auto difference=[&](const OpticalFit& fit,std::vector<double>& mm,std::vector<double>& deg) {
                        Rigid pose; std::copy(std::begin(fit.pose),std::end(fit.pose),pose.begin());
                        double error=0;
                        for(int k=0;k<3;++k) error+=std::pow(pose[4*k+3]-reference.world[4*k+3],2);
                        mm.push_back(1000*std::sqrt(error)); deg.push_back(OpticalPrior::angle(pose,reference.world));
                    };
                    OpticalFit fit{}; int observations=0; bool rotation=false;
                    const double dt=double(reference.stamp-few_history.last_stamp())*1e-9;
                    if((recovery && solve_prior_consensus(api,model,optical,*sparse_prior,dt,fit,observations,rotation)) || solve_few_led(api,model,optical,*sparse_prior,dt,fit,observations,rotation))
                        difference(fit,few_full_mm,few_full_deg);
                    // Mask genuine detector centroids labelled by the FULL fit; do not seed from that fit.
                    // Several LED choices, both mono 2/3 and 1+1/2+1 stereo; discarded centroids are never fabricated.
                    std::array<std::vector<std::pair<int,std::array<double,2>>>,2> labelled;
                    for(int v=0;v<2;++v) for(int led=0;led<8;++led) {
                        double uv[2];
                        if(!few_led_detail::project(api,model,optical[v],reference.world,led,uv,true)) continue;
                        int best=-1; double distance=2.5;
                        for(size_t b=0;b<optical[v].count;++b) {
                            const double d=std::hypot(uv[0]-optical[v].pixels[2*b],uv[1]-optical[v].pixels[2*b+1]);
                            if(d<distance) { best=int(b); distance=d; }
                        }
                        if(best>=0) labelled[size_t(v)].push_back({led,{optical[v].pixels[2*best],optical[v].pixels[2*best+1]}});
                    }
                    for(int count=2;count<=3;++count) for(int mode=0;mode<3;++mode) {
                        const int left=mode==0?count:mode==1?0:count-1,right=count-left;
                        if(labelled[0].size()<size_t(left) || labelled[1].size()<size_t(right)) continue;
                        for(size_t start=0;start<std::max(labelled[0].size(),labelled[1].size());++start) {
                            OpticalView masked[2]{optical[0],optical[1]}; double chosen[2][6]{};
                            const int take[2]{left,right};
                            for(int v=0;v<2;++v) {
                                masked[v].count=size_t(take[v]); masked[v].pixels=chosen[v];
                                for(int k=0;k<take[v];++k) {
                                    const auto& centroid=labelled[size_t(v)][(start+size_t(k))%labelled[size_t(v)].size()].second;
                                    chosen[v][2*k]=centroid[0]; chosen[v][2*k+1]=centroid[1];
                                }
                            }
                            ++drop_attempts[count-2];
                            if((recovery && solve_prior_consensus(api,model,masked,*sparse_prior,dt,fit,observations,rotation)) || solve_few_led(api,model,masked,*sparse_prior,dt,fit,observations,rotation))
                                difference(fit,count==2?drop2_mm:drop3_mm,count==2?drop2_deg:drop3_deg);
                        }
                    }
                }
            }
            if(!recovery && !c.pair && few && sparse_prior) c=identity_few_led_candidate(api,model,pair,now,*sparse_prior,few_history.anchor(),few_history.last_stamp());
            if(c.pair && !gyro.supported_candidate(c,prediction,recovery || few)) c={};
            if(c.few_led) ++few_poses;
            if(!c.pair){gyro.missing();++skipped[production_failure+(prediction?" [gyro prior]":seed?" [last-pose prior]":" [no prior]")];}
            return c;
        }
        catch(const std::exception& e){++skipped[std::string("exception ")+e.what()];return {};}
    }
};
void print_motion(const char* name,const std::vector<std::pair<int64_t,std::array<double,7>>>& poses,const OpticalPrior& hindsight){
    std::vector<double> gaps,steps,turns;uint64_t flips=0;
    uint64_t covered=0,insufficient=0,over10=0,over20=0;
    std::vector<double> disagreement,coverage;
    for(size_t i=1;i<poses.size();++i){
        const auto &a=poses[i-1].second,&b=poses[i].second;
        gaps.push_back(double(poses[i].first-poses[i-1].first)/1e6);
        steps.push_back(1000*std::sqrt(std::pow(a[0]-b[0],2)+std::pow(a[1]-b[1],2)+std::pow(a[2]-b[2],2)));
        double dot=0,na=0,nb=0;for(int k=3;k<7;++k){dot+=a[k]*b[k];na+=a[k]*a[k];nb+=b[k]*b[k];}
        const double deg=2*std::acos(std::min(1.,std::abs(dot)/std::sqrt(na*nb)))*180/std::acos(-1.);
        turns.push_back(deg);if(deg>20)++flips;
        const auto from=poses[i-1].first,to=poses[i].first;
        const auto integrated=hindsight.integral(from,to);
        const double fraction=to>from?double(integrated.covered_ns)/double(to-from):0;
        coverage.push_back(fraction*100);
        if(fraction>=.8) {
            ++covered;
            const double error=replay_gyro_disagreement(replay_rotation(a),replay_rotation(b),integrated.rotation);
            disagreement.push_back(error); if(error>10) ++over10; if(error>20) ++over20;
        } else ++insufficient;
    }
    std::cout<<name<<" publications="<<poses.size()<<"\n  gap_ms "<<stats(gaps,1)<<"\n  position_step_mm "<<stats(steps,1)
             <<"\n  rotation_step_deg "<<stats(turns,2)<<"\n  flips_over_20deg="<<flips<<'\n';
    std::cout<<"  gyro_hindsight covered_steps="<<covered<<" insufficient_steps="<<insufficient
        <<" disagreement_over_10deg="<<over10<<" disagreement_over_20deg="<<over20
        <<"\n  gyro_disagreement_deg "<<stats(disagreement,2)<<"\n  gyro_coverage_pct "<<stats(coverage,1)<<'\n';
}
}

int main(int argc,char** argv){
    if(argc<5){std::cerr<<"Usage: capture_replay <capture_dir> <private-led-json> <factory-camera-json> <out-wire.bin> [--max-pairs N] [--mono-minus-raw NS] [--prior-calibration FILE] [--prior-stage 0..3]\n";return 2;}
    size_t max_pairs=SIZE_MAX;std::optional<int64_t> mono_minus_raw;
    std::optional<int64_t> imu_latency;
    std::string prior_calibration; int prior_stage=3; bool few=few_led_enabled();
    try{
        for(int i=5;i<argc;i+=2){
            const std::string o=argv[i];
            if(i+1>=argc)throw std::runtime_error("missing value for "+o);
            if(o=="--max-pairs")max_pairs=size_t(std::stoull(argv[i+1]));
            else if(o=="--mono-minus-raw")mono_minus_raw=std::stoll(argv[i+1]);
            else if(o=="--prior-calibration")prior_calibration=argv[i+1];
            else if(o=="--imu-latency-ms") {
                size_t used=0; const double ms=std::stod(argv[i+1],&used);
                if(used!=std::strlen(argv[i+1]) || !std::isfinite(ms) || ms<0 || ms>60000)
                    throw std::runtime_error("imu-latency-ms must be finite, 0..60000");
                imu_latency=int64_t(std::llround(ms*1e6));
            }
            else if(o=="--prior-stage") { prior_stage=std::stoi(argv[i+1]); if(prior_stage<0||prior_stage>3)throw std::runtime_error("prior-stage must be 0..3"); }
            else if(o=="--few-led") { if(std::string(argv[i+1])!="0" && std::string(argv[i+1])!="1") throw std::runtime_error("few-led must be 0/1"); few=std::string(argv[i+1])=="1"; }
            else throw std::runtime_error("unknown option "+o);
        }
    }catch(const std::exception& e){std::cerr<<"capture_replay: "<<e.what()<<'\n';return 2;}
    const fs::path dir=argv[1];
    Solver solve;
    std::unique_ptr<NativeOpticalLibraries> libraries;
    std::vector<Pair> pairs;
    std::vector<WireRec> wire;
    std::vector<std::string> proof_lines;std::vector<int64_t> proof_t;
    HeadHistory all_heads; // whole session, alignment only
    try{
        // Frames, short exposures and upper pairs as XrtapSource: <15 ms after the previous frame, stereo skew <=1 ms.
        std::map<int,std::vector<Frame>> frames;
        for(const auto& entry:fs::directory_iterator(dir/"frames")){
            const auto name=entry.path().filename().string();
            const auto underscore=name.find('_');
            if(name.rfind("video",0)!=0||underscore==std::string::npos||entry.path().extension()!=".pgm")continue;
            try{
                const int cam=std::stoi(name.substr(5,underscore-5));
                const auto ts=name.substr(underscore+1,name.size()-underscore-5);
                size_t used=0;const int64_t stamp=std::stoll(ts,&used);
                if(used==ts.size()&&stamp>0)frames[cam].push_back({stamp,entry.path()});
            }catch(const std::exception&){}
        }
        std::array<std::vector<Frame>,2> shorts;
        for(int i=0;i<2;++i){
            auto& list=frames[cams[i]];
            if(list.empty())throw std::runtime_error("no frames/video"+std::to_string(cams[i])+"_<ts_ns>.pgm");
            std::sort(list.begin(),list.end(),[](const Frame& a,const Frame& b){return a.stamp<b.stamp;});
            for(size_t j=1;j<list.size();++j)if(list[j].stamp-list[j-1].stamp>0&&list[j].stamp-list[j-1].stamp<15000000)shorts[size_t(i)].push_back(list[j]);
        }
        for(const auto& a:shorts[0]){
            const auto& b=shorts[1];
            auto it=std::lower_bound(b.begin(),b.end(),a.stamp,[](const Frame& f,int64_t t){return f.stamp<t;});
            const Frame* best=nullptr;
            if(it!=b.end())best=&*it;
            if(it!=b.begin()&&(!best||std::llabs(std::prev(it)->stamp-a.stamp)<std::llabs(best->stamp-a.stamp)))best=&*std::prev(it);
            if(best&&std::llabs(best->stamp-a.stamp)<=1000000)pairs.push_back({{a,*best},std::min(a.stamp,best->stamp)+offset_ns});
        }
        std::cout<<"frames video0="<<frames[0].size()<<" video3="<<frames[3].size()<<" short video0="<<shorts[0].size()
                 <<" video3="<<shorts[1].size()<<" pairs="<<pairs.size()<<'\n';
        if(pairs.empty())throw std::runtime_error("no short upper stereo pairs");
        {   std::ifstream f(dir/"wire.bin",std::ios::binary);
            if(!f)throw std::runtime_error("wire.bin unreadable");
            // The capture file can hold a torn record where the flag was toggled: resynchronise on a plausible header.
            std::string all((std::istreambuf_iterator<char>(f)),std::istreambuf_iterator<char>());
            size_t at=0,skipped=0;
            while(at+12<=all.size()){
                WireRec r;uint32_t len;std::memcpy(&r.t,all.data()+at,8);std::memcpy(&len,all.data()+at+8,4);
                const bool ok=len>0&&len<=512&&at+12+len<=all.size()&&uint64_t(r.t)>1000000000ULL&&uint64_t(r.t)<4000000000000000ULL;
                if(!ok){++at;++skipped;continue;}
                r.bytes.assign(all.begin()+long(at)+12,all.begin()+long(at)+12+long(len));
                wire.push_back(std::move(r));at+=12+len;
            }
            if(skipped)std::cout<<"wire resync skipped "<<skipped<<" bytes\n";
        }
        {   std::ifstream f(dir/"attempt"/"driver-proof.jsonl");
            if(!f)throw std::runtime_error("attempt/driver-proof.jsonl unreadable");
            uint64_t bad=0;
            for(std::string line;std::getline(f,line);){
                if(line.empty())continue;
                int64_t t=-1;
                try{auto v=phase5::Json(line).parse();t=phase5::number(v,"t_ns");all_heads.observe(v);}catch(const std::exception&){++bad;}
                proof_lines.push_back(std::move(line));proof_t.push_back(t);
            }
            std::cout<<"driver_proof rows="<<proof_lines.size()<<" dropped="<<bad<<" (CLOCK_MONOTONIC)\n";
        }
        solve.model=load_tracker_leds(argv[2],pico::devices::serial());
        fs::path dataset;
        if(const char* env=std::getenv("PICO_CALIBRATION_DATASET"))dataset=direct_calibration_dataset(env);
        else try{dataset=direct_calibration_dataset(dir);}catch(const std::exception&){dataset=direct_calibration_dataset(dir/"frames");}
        const auto rig=load_replay_rig(dataset,argv[3]);
        for(int i=0;i<2;++i)solve.views[size_t(i)]=rig.bind(serials[i],1056,1024);
        std::cout<<"calibration_dataset="<<dataset.string()<<'\n';
        auto library_root=fs::canonical(argv[0]).parent_path().parent_path().parent_path()/"candidate/release/optical";
        if(const char* root=std::getenv("PICO_OPTICAL_LIBRARY_ROOT")) library_root=root;
        libraries=std::make_unique<NativeOpticalLibraries>((library_root/"libpico_optics.so").string(),(library_root/"libpico_detector.so").string());
        solve.api=libraries->api();
    }catch(const std::exception& e){std::cerr<<"capture_replay: "<<e.what()<<'\n';return 2;}

    // Wire clock check against PTI1 receiveTime (CLOCK_MONOTONIC), old PTP2 delay for reference.
    std::vector<OldPose> old;
    {   std::vector<double> imu_lag,pose_lag;uint64_t imu=0,ptp2=0,other=0;
        for(const auto& r:wire){
            if(is(r,"PTI1",32)){++imu;imu_lag.push_back((double(r.t)-double(le<uint64_t>(r.bytes,12)))/1e6);}
            else if(is(r,"PTP2",88)){
                ++ptp2;
                if(le<uint32_t>(r.bytes,8)&2u){
                    OldPose o{int64_t(le<uint64_t>(r.bytes,20)),r.t,{}};
                    for(size_t i=0;i<3;++i)o.p[i]=le<float>(r.bytes,28+4*i);
                    old.push_back(o);pose_lag.push_back((double(r.t)-double(o.src))/1e6);
                }
            }else ++other;
        }
        std::sort(old.begin(),old.end(),[](const OldPose& a,const OldPose& b){return a.src<b.src;});
        old.erase(std::unique(old.begin(),old.end(),[](const OldPose& a,const OldPose& b){return a.src==b.src;}),old.end());
        std::cout<<"wire records="<<wire.size()<<" PTI1="<<imu<<" PTP2="<<ptp2<<" (valid "<<old.size()<<") other="<<other<<'\n'
                 <<"wire_minus_PTI1_receiveTime_ms "<<stats(imu_lag,3)<<'\n'
                 <<"wire_minus_old_PTP2_sourceTime_ms "<<stats(pose_lag,1)<<'\n';
        if(!imu_lag.empty()){
            std::sort(imu_lag.begin(),imu_lag.end());
            const double p50=imu_lag[imu_lag.size()/2];
            if(p50<-1||p50>100){std::cerr<<"capture_replay: wire receive clock is not CLOCK_MONOTONIC (PTI1 lag p50 "<<p50<<" ms)\n";return 2;}
            std::cout<<"wire receive clock = CLOCK_MONOTONIC (PTI1 lag p50 "<<p50<<" ms)\n";
        }else std::cout<<"wire receive clock unchecked (no PTI1)\n";
    }

    NullBuffer null;
    auto* saved=std::cerr.rdbuf(&null); // log_optical_drop lines; counts are printed below
    // MONOTONIC-RAW per pair: tracked samples (raw exposure, D), interpolated.
    std::vector<std::pair<int64_t,int64_t>> track;
    if(mono_minus_raw)track.push_back({0,*mono_minus_raw});
    else if(!old.empty()){
        std::vector<int64_t> exposures;for(const auto& p:pairs)exposures.push_back(p.exposure);
        auto nearest=[&](int64_t t)->int64_t{
            auto it=std::lower_bound(exposures.begin(),exposures.end(),t);
            int64_t best=INT64_MAX/4;
            if(it!=exposures.end())best=*it;
            if(it!=exposures.begin()&&std::llabs(*std::prev(it)-t)<std::llabs(best-t))best=*std::prev(it);
            return best;
        };
        auto old_at=[&](int64_t t)->const OldPose*{
            auto it=std::lower_bound(old.begin(),old.end(),t-1000000,[](const OldPose& o,int64_t x){return o.src<x;});
            return it!=old.end()&&it->src<=t+1000000?&*it:nullptr;
        };
        // Identity-head stereo fits (head-frame poses; the stereo solve is world-frame equivariant) in 300-pair
        // windows, middle forward then backward, until one window has 20 fits; the window with most fits is used.
        std::vector<std::pair<int64_t,Rigid>> local;
        std::vector<size_t> starts;
        for(size_t s=pairs.size()/2;s<pairs.size();s+=300)starts.push_back(s);
        for(size_t s=pairs.size()/2;s>=300;){s-=300;starts.push_back(s);}
        for(size_t s:starts){
            std::vector<std::pair<int64_t,Rigid>> window;
            for(size_t j=s;j<std::min(pairs.size(),s+300);++j){
                const auto& p=pairs[j];
                const std::array<int64_t,2> stamps{p.f[0].stamp+offset_ns,p.f[1].stamp+offset_ns};
                const auto c=solve(p,stamps,{identity_rigid(),identity_rigid()},std::max(stamps[0],stamps[1])+delay_ns);
                if(c.pair&&c.stereo_verified&&fit_valid(c))window.push_back({c.stamp,c.world});
            }
            if(window.size()>local.size())local=std::move(window);
            if(local.size()>=20)break;
        }
        // Candidates: each of the first 10 local fits aligned to each old pose (an exposure the old run did not
        // publish gives only wrong phases). Count timing matches, then score positions (raw_from_standing * head *
        // fit) against the old PTP2 positions; the lowest median error wins.
        struct Score { int64_t d; size_t n; double median; };
        std::vector<int64_t> ds;
        for(size_t a=0;a<std::min<size_t>(10,local.size());++a)for(const auto& o:old)ds.push_back(o.src-local[a].first);
        std::sort(ds.begin(),ds.end());
        ds.erase(std::unique(ds.begin(),ds.end(),[](int64_t a,int64_t b){return b-a<=500000;}),ds.end());
        std::vector<Score> scores;size_t best_count=0;
        for(const int64_t d:ds){
            size_t n=0;
            for(const auto& l:local)if(old_at(l.first+d))++n;
            if(n>=8)scores.push_back({d,n,0});
            best_count=std::max(best_count,n);
        }
        std::vector<Score> scored;
        for(auto s:scores){
            if(s.n*2<best_count)continue;
            std::vector<double> errors;
            for(const auto& l:local){
                const int64_t t=l.first+s.d;const auto* o=old_at(t);
                if(!o)continue;
                try{
                    const auto raw=multiply(all_heads.raw_from_standing(t),multiply(all_heads.head_pose(t),l.second));
                    errors.push_back(std::sqrt(std::pow(raw[3]-o->p[0],2)+std::pow(raw[7]-o->p[1],2)+std::pow(raw[11]-o->p[2],2)));
                }catch(const std::exception&){}
            }
            if(errors.size()<8)continue;
            std::nth_element(errors.begin(),errors.begin()+std::ptrdiff_t(errors.size()/2),errors.end());
            s.n=errors.size();s.median=errors[errors.size()/2];scored.push_back(s);
        }
        std::sort(scored.begin(),scored.end(),[](const Score& a,const Score& b){return a.median<b.median;});
        if(scored.empty()){std::cerr.rdbuf(saved);std::cerr<<"capture_replay: cannot align frames to old PTP2 poses (identity-head fits="<<local.size()<<"); pass --mono-minus-raw\n";return 2;}
        std::cout<<"alignment fits="<<local.size()<<" candidates="<<scored.size()<<" best mono_minus_raw_ns="<<scored[0].d
                 <<" median_err_mm="<<scored[0].median*1000<<" n="<<scored[0].n;
        for(const auto& s:scored)if(std::llabs(s.d-scored[0].d)>16000000){std::cout<<" | next phase "<<s.d<<" median_err_mm="<<s.median*1000<<" n="<<s.n;break;}
        std::cout<<'\n';
        // Track across the run: each old pose matches the pair nearest src-D (within 1 ms), D follows the drift.
        const auto anchor=std::min<std::ptrdiff_t>(std::ptrdiff_t(old.size())-1,std::distance(old.begin(),
            std::lower_bound(old.begin(),old.end(),local.front().first+scored[0].d,[](const OldPose& o,int64_t x){return o.src<x;})));
        for(int step=1;step>=-1;step-=2){
            int64_t d=scored[0].d;
            for(auto i=anchor;i>=0&&i<std::ptrdiff_t(old.size());i+=step){
                const auto e=nearest(old[size_t(i)].src-d);
                if(std::llabs(old[size_t(i)].src-e-d)<=1000000){d=old[size_t(i)].src-e;track.push_back({e,d});}
            }
        }
        std::sort(track.begin(),track.end());
        track.erase(std::unique(track.begin(),track.end(),[](auto& a,auto& b){return a.first==b.first;}),track.end());
    }
    if(track.empty()){std::cerr.rdbuf(saved);std::cerr<<"capture_replay: no old PTP2 poses to derive MONOTONIC-RAW; pass --mono-minus-raw\n";return 2;}
    std::cout<<"mono_minus_raw_ns first="<<track.front().second<<" last="<<track.back().second<<" samples="<<track.size();
    if(track.size()>1)std::cout<<" drift_ppm="<<double(track.back().second-track.front().second)/double(track.back().first-track.front().first)*1e6;
    std::cout<<(mono_minus_raw?" (--mono-minus-raw)":" (derived)")<<'\n';
    auto mono=[&](int64_t raw){
        auto it=std::lower_bound(track.begin(),track.end(),std::make_pair(raw,INT64_MIN));
        if(it==track.begin())return raw+it->second;
        if(it==track.end())return raw+track.back().second;
        const auto& a=*std::prev(it);const auto& b=*it;
        return raw+a.second+int64_t(std::llround(double(b.second-a.second)*double(raw-a.first)/double(b.first-a.first)));
    };

    std::stable_sort(wire.begin(),wire.end(),[](const WireRec& a,const WireRec& b){return a.t<b.t;});
    // Live pass: causal HeadHistory (rows visible by publication time), OpticalWorker + PublicationExpiry rules.
    if(pairs.size()>max_pairs)pairs.resize(max_pairs);
    solve.reset(); solve.stage=prior_stage; solve.few=few; solve.evaluate=true; solve.recovery=few && tracking_recovery_enabled();
    Solver baseline; baseline.api=solve.api; baseline.model=solve.model; baseline.views=solve.views; baseline.stage=prior_stage;
    if(!prior_calibration.empty())solve.gyro.load(prior_calibration,chip);
    if(!prior_calibration.empty())baseline.gyro.load(prior_calibration,chip);
    OpticalPrior hindsight(true);
    if(!prior_calibration.empty()) hindsight.load(prior_calibration,chip);
    std::vector<WireRec> causal_imu;
    for(const auto& r:wire) if(is(r,"PTI1",32)) {
        hindsight.wire(reinterpret_cast<const uint8_t*>(r.bytes.data()),r.bytes.size());
        const auto available=replay_imu_available(int64_t(r.t),int64_t(le<uint64_t>(r.bytes,12)),imu_latency);
        causal_imu.push_back({uint64_t(available),r.bytes});
    }
    std::stable_sort(causal_imu.begin(),causal_imu.end(),[](const WireRec& a,const WireRec& b){return a.t<b.t;});
    std::cout<<"imu_availability="<<(imu_latency?"sensor-plus-latency":"recorded-receive")
        <<" latency_ms="<<(imu_latency?double(*imu_latency)*1e-6:-1.)<<" hindsight_samples="<<hindsight.imu<<'\n';
    uint64_t baseline_count=0; int64_t baseline_last=0,baseline_gap=0,new_last=0,new_gap=0;
    std::vector<double> overlap_mm,overlap_deg;
    size_t imu_fed=0;
    HeadHistory heads(4096);size_t fed=0;uint64_t proof_dropped=0;
    uint64_t head_drops=0,origin_drops=0,nofit=0,stereo=0,mono_fits=0,fit_drops=0,order_drops=0,invalid=0,speed=0,sequence=0;
    std::optional<OpticalPoseFrame> last;
    std::vector<WireRec> out;
    std::vector<std::pair<int64_t,std::array<double,7>>> published,baseline_published;
    std::array<uint64_t,4> recovered_old_failure{},lost_old_pose{};
    const int64_t span0=mono(pairs.front().exposure),span1=mono(pairs.back().exposure)+delay_ns;
    for(const auto& p:pairs){
        const int64_t d=mono(p.exposure)-p.exposure;
        const std::array<int64_t,2> stamps{p.f[0].stamp+offset_ns+d,p.f[1].stamp+offset_ns+d};
        const int64_t sent=std::min(stamps[0],stamps[1])+delay_ns;
        while(imu_fed<causal_imu.size() && causal_imu[imu_fed].t<=uint64_t(sent)) {
            const auto& r=causal_imu[imu_fed++]; solve.gyro.wire(reinterpret_cast<const uint8_t*>(r.bytes.data()),r.bytes.size());
            baseline.gyro.wire(reinterpret_cast<const uint8_t*>(r.bytes.data()),r.bytes.size());
        }
        for(;fed<proof_lines.size()&&proof_t[fed]<=sent;++fed)
            try{heads.observe(phase5::Json(proof_lines[fed]).parse());}catch(const std::exception&){++proof_dropped;}
        std::array<Rigid,2> pose;bool ok=true;
        for(size_t i=0;i<2&&ok;++i)
            try{(void)heads.raw_from_standing(stamps[i]);pose[i]=heads.head_pose(stamps[i]);}catch(const std::runtime_error&){ok=false;}
        if(!ok){++head_drops;solve.gyro.missing();continue;}
        const auto c=solve(p,stamps,pose,sent);
        const auto old_fit=baseline(p,stamps,pose,sent);
        const bool baseline_good=old_fit.pair && fit_valid(old_fit) && (prior_stage<3 || baseline.gyro.admit(old_fit.world,old_fit.stamp,baseline.gyro.predict(old_fit.stamp,false)));
        if(baseline_good) {
            const auto raw=multiply(heads.raw_from_standing(old_fit.stamp),old_fit.world);
            const auto q=quaternion(raw); baseline_published.push_back({old_fit.stamp,{raw[3],raw[7],raw[11],q[0],q[1],q[2],q[3]}});
            ++baseline_count; baseline.accepted(old_fit);
            baseline_gap=std::max(baseline_gap,old_fit.stamp-(baseline_last?baseline_last:span0)); baseline_last=old_fit.stamp;
        }
        if(!baseline_good) ++baseline.failed[solve.blob_class];
        struct Outcome {
            bool old,accepted; size_t category;
            std::array<uint64_t,4>& failed;
            std::array<uint64_t,4>& recovered;
            std::array<uint64_t,4>& lost;
            ~Outcome() {
                if(!accepted) ++failed[category];
                if(!old && accepted) ++recovered[category];
                if(old && !accepted) ++lost[category];
            }
        } outcome{baseline_good,false,solve.blob_class,solve.failed,recovered_old_failure,lost_old_pose};
        if(!c.pair||(!c.stereo_verified&&!c.mono)){++nofit; continue;}
        ++(c.stereo_verified?stereo:mono_fits);
        if(!fit_valid(c)){++fit_drops; solve.gyro.missing();continue;}
        const auto final_prediction=solve.gyro.predict(c.stamp,solve.recovery);
        if(solve.recovery && final_prediction && !solve.gyro.consistent(c.world,c.stamp,final_prediction)) {
            ++solve.recovery_rotation_drops; ++solve.reasons[solve.blob_class]["final/gyro-orientation"]; continue;
        }
        if(prior_stage>=3 && !solve.gyro.admit(c.world,c.stamp,final_prediction)) continue;
        if(old_fit.pair && fit_valid(old_fit)) {
            double error=0;
            for(int k=0;k<3;++k) error+=std::pow(c.world[4*k+3]-old_fit.world[4*k+3],2);
            overlap_mm.push_back(1000*std::sqrt(error)); overlap_deg.push_back(OpticalPrior::angle(c.world,old_fit.world));
        }
        OpticalPoseFrame frame;
        frame.chip=chip;frame.stamp=c.stamp;frame.connected=frame.valid=true;
        frame.quality=std::min(1.,c.inliers/8.)*std::exp(-c.residual/2);
        if(c.few_led) frame.quality=std::min(.24,c.matched_observations/32.)*std::exp(-c.residual/2);
        frame.quaternion=quaternion(c.world);
        for(size_t i=0;i<3;++i)frame.position[i]=c.world[4*i+3];
        if(last){
            if(frame.stamp<=last->stamp){++order_drops;continue;}
            const double dt=double(frame.stamp-last->stamp)/1e9;
            double distance=0;
            for(size_t i=0;i<3;++i)distance+=std::pow(frame.position[i]-last->position[i],2);
            distance=std::sqrt(distance);
            if(!(dt>=.25||distance<=std::max(.05,8*dt)))++speed;
            if(dt>.005&&dt<.25&&distance/dt<=8){
                const auto alpha=1-std::exp(-dt/.05);
                for(size_t i=0;i<3;++i)frame.velocity[i]=(1-alpha)*last->velocity[i]+alpha*(frame.position[i]-last->position[i])/dt;
            }
        }
        try{(void)frame.encode(sent);}catch(const std::invalid_argument&){++invalid;continue;}
        OpticalPoseFrame output;
        try{output=driver_pose(frame,heads.raw_from_standing(frame.stamp));}catch(const std::runtime_error&){++origin_drops;continue;}
        output.sequence=++sequence;
        std::array<uint8_t,88> bytes;
        try{bytes=output.encode(sent);}catch(const std::invalid_argument&){--sequence;++invalid;continue;}
        out.push_back({uint64_t(sent),std::string(bytes.begin(),bytes.end())});
        published.push_back({output.stamp,{output.position[0],output.position[1],output.position[2],
            output.quaternion[0],output.quaternion[1],output.quaternion[2],output.quaternion[3]}});
        if(last) {
            const auto& a=published[published.size()-2].second;
            const auto& b=published.back().second;
            double dot=0,na=0,nb=0;
            for(int k=3;k<7;++k) { dot+=a[k]*b[k]; na+=a[k]*a[k]; nb+=b[k]*b[k]; }
            const double step=2*std::acos(std::min(1.,std::abs(dot)/std::sqrt(na*nb)))*180/std::acos(-1.);
            if(step>20) std::cout<<"flip time_ns="<<c.stamp<<" gap_ms="<<double(c.stamp-last->stamp)*1e-6
                <<" kind="<<c.solver_kind<<" points="<<c.inliers<<" residual="<<c.residual
                <<" gyro_angle_deg="<<(final_prediction?OpticalPrior::angle(*final_prediction,c.world):-1.)
                <<" gyro_bound_deg="<<(final_prediction?solve.gyro.consistency_bound_deg(c.stamp):-1.)
                <<" previous_angle_deg="<<step
                <<" standing_step_deg="<<(solve.prior?OpticalPrior::angle(*solve.prior,c.world):-1.)<<'\n';
        }
        last=frame;
        solve.accepted(c);
        outcome.accepted=true;
        new_gap=std::max(new_gap,c.stamp-(new_last?new_last:span0)); new_last=c.stamp;
    }
    std::cerr.rdbuf(saved);

    uint64_t kept=0;
    for(const auto& r:wire)if(!is(r,"PTP2",88)&&r.t>=uint64_t(span0)&&r.t<=uint64_t(span1)){out.push_back(r);++kept;}
    std::stable_sort(out.begin(),out.end(),[](const WireRec& a,const WireRec& b){return a.t<b.t;});
    {   std::ofstream f(argv[4],std::ios::binary|std::ios::trunc);
        for(const auto& r:out){
            const uint32_t len=uint32_t(r.bytes.size());
            f.write(reinterpret_cast<const char*>(&r.t),8);f.write(reinterpret_cast<const char*>(&len),4);f.write(r.bytes.data(),len);
        }
        if(!f){std::cerr<<"capture_replay: cannot write "<<argv[4]<<'\n';return 2;}
    }
    std::cout<<"span_mono_ns="<<span0<<".."<<span1<<" out records="<<out.size()<<" (new PTP2 "<<published.size()<<", kept "<<kept<<")\n"
             <<"pairs_run="<<pairs.size()<<" head_drops="<<head_drops<<" proof_rows_dropped="<<proof_dropped<<" no_fit="<<nofit
             <<"\nstereo_candidates="<<stereo<<" mono_candidates="<<mono_fits<<" fit_valid_drops="<<fit_drops
             <<" order_drops="<<order_drops<<" origin_drops="<<origin_drops<<" invalid_ptp2="<<invalid<<" speed_flagged="<<speed<<'\n';
    std::cout<<"prior_stage="<<prior_stage<<" prior_imu="<<solve.gyro.imu<<" rotation_rejected="<<solve.gyro.rejected<<" reanchors="<<solve.gyro.reanchors<<'\n';
    std::cout<<"recovery_enabled="<<solve.recovery<<" prior_hold_ms="<<prior_hold_ns()/1000000
        <<" recovery_seed_pairs="<<solve.recovery_seeds<<" recovery_fits="<<solve.recovery_fits
        <<" recovery_attempts="<<solve.recovery_attempts
        <<" recovery_rotation_drops="<<solve.recovery_rotation_drops<<'\n';
    print_motion("replay",published,hindsight);
    print_motion("old_solver_replay",baseline_published,hindsight);
    const char* classes[]{"both_ge4","one_ge4","max_le3_nonzero","both_zero"};
    for(size_t i=0;i<4;++i) for(const auto& reason:solve.reasons[i])
        std::cout<<"rejection_class "<<classes[i]<<" count="<<reason.second<<" reason="<<reason.first<<'\n';
    for(int i=0;i<4;++i) std::cout<<"failed_pairs "<<classes[i]<<" old="<<baseline.failed[size_t(i)]<<" new="<<solve.failed[size_t(i)]
        <<" recovered_old_failure="<<recovered_old_failure[size_t(i)]<<" lost_old_pose="<<lost_old_pose[size_t(i)]<<'\n';
    std::vector<std::pair<int64_t,std::array<double,7>>> before;
    for(const auto& r:wire)if(is(r,"PTP2",88)&&(le<uint32_t>(r.bytes,8)&2u)&&r.t>=uint64_t(span0)&&r.t<=uint64_t(span1)){
        std::array<double,7> v{};for(size_t i=0;i<7;++i)v[i]=le<float>(r.bytes,28+4*i);
        before.push_back({int64_t(le<uint64_t>(r.bytes,20)),v});
    }
    print_motion("old_ptp2_same_span",before,hindsight);
    baseline_gap=std::max(baseline_gap,span1-(baseline_last?baseline_last:span0));
    new_gap=std::max(new_gap,span1-(new_last?new_last:span0));
    std::cout<<"few_led="<<few<<" old_solver_poses="<<baseline_count<<" new_poses="<<published.size()
        <<" few_candidates="<<solve.few_poses<<" longest_gap_old_ms="<<double(baseline_gap)*1e-6
        <<" longest_gap_new_ms="<<double(new_gap)*1e-6<<" (includes endpoints)\n";
    for(int v=0;v<2;++v) { std::cout<<"upper_blob_histogram view="<<v; for(const auto& h:solve.histogram[size_t(v)]) std::cout<<' '<<h.first<<':'<<h.second; std::cout<<'\n'; }
    std::cout<<"old_new_overlap_mm "<<stats(overlap_mm,4)<<" rotation_deg "<<stats(overlap_deg,4)<<'\n'
        <<"few_vs_full_mm "<<stats(solve.few_full_mm,4)<<" rotation_deg "<<stats(solve.few_full_deg,4)<<'\n'
        <<"drop_to_2 attempts="<<solve.drop_attempts[0]<<" accepted="<<solve.drop2_mm.size()<<" error_mm "<<stats(solve.drop2_mm,4)<<" rotation_deg "<<stats(solve.drop2_deg,4)<<'\n'
        <<"drop_to_3 attempts="<<solve.drop_attempts[1]<<" accepted="<<solve.drop3_mm.size()<<" error_mm "<<stats(solve.drop3_mm,4)<<" rotation_deg "<<stats(solve.drop3_deg,4)<<'\n';
    for(const auto& s:solve.skipped)std::cout<<"skipped "<<s.second<<"  "<<s.first<<'\n';
    return 0;
}
