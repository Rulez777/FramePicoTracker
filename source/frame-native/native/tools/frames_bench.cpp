#include "../device_config.hpp"
// Build (aarch64, from native/; pico_producer flags, replay_calibration_tests link set with libpico_control/data_plane expanded):
//   g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -fno-fast-math -ffp-contract=off -pthread -I. -o build/frames_bench tools/frames_bench.cpp replay_calibration.cpp openocd_process.cpp roy_owner.cpp tcl_transport.cpp owner_lifecycle.cpp image_admission.cpp radio_loop.cpp peer_engine.cpp station_control.cpp query_peer.cpp native25.cpp native_identity.cpp data_plane.cpp -ldl
// Usage: build/frames_bench <frames_dir> <private-led-json> <factory-camera-json> [max_pairs]
// Offline benchmark of the producer's native detector + identity_stereo_candidate over PGM dumps
// (video0_<ts_ns>.pgm / video3_<ts_ns>.pgm), head pose = identity, one summary at the end.
// Libraries are found like pico_producer: <exe>/../../../candidate/release/optical/libpico_{optics,detector}.so.
// Active calibration: $PICO_CALIBRATION_DATASET, else frames_dir, else its parent (direct_calibration_dataset rules).
// Exit 2 only when inputs/libraries/calibration cannot be loaded; otherwise 0.
#include "calibration_discovery.hpp"
#include "optical_compute.hpp"
#include "replay_calibration.hpp"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace {
using namespace pico;
namespace fs = std::filesystem;
constexpr const char* serials[2] = {"CAMUPPERLEFT", "CAMUPPERRIGHT"}; // video0, video3 (xrtap_source.hpp)
constexpr int cams[2] = {0, 3};
// PGM carries no exposure; xrtap default short exposure (accepted by pair_valid for direct capture).
constexpr double exposure_us = 782.267;

struct Pgm { int width=0,height=0,maxval=0; size_t header=0; uintmax_t file_size=0; std::vector<uint8_t> pixels; std::string error; };
// Geometry comes from the P5 header; the payload must be exactly width*height samples (no row padding).
Pgm read_pgm(const fs::path& path,bool load){
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
    if(!f||out.width<=0||out.height<=0||out.maxval<=0||out.maxval>65535){out.error="bad header";return out;}
    f.get(); // single whitespace byte ends the header
    out.header=size_t(f.tellg());
    const size_t bytes=size_t(out.width)*size_t(out.height)*(out.maxval>255?2:1);
    if(out.file_size!=out.header+bytes){
        out.error="file "+std::to_string(out.file_size)+"B != header "+std::to_string(out.header)+"B + payload "+std::to_string(bytes)+"B";
        return out;
    }
    if(out.maxval>255){out.error="16-bit PGM";return out;}
    if(load){out.pixels.resize(bytes);f.read(reinterpret_cast<char*>(out.pixels.data()),std::streamsize(bytes));if(!f)out.error="short read";}
    return out;
}
struct Frame { int64_t stamp; fs::path path; };
// Same detector call and ray-domain filter as native_identity.cpp detect_view; -1 on detector failure.
int blob_count(const NativeOpticalApi& api,const IdentityExposure& f){
    const int limit=f.width*f.height;int capacity=256,count=-1;std::vector<double> blobs;
    for(;;){
        blobs.resize(size_t(capacity)*5);
        count=api.detect(f.pixels.data(),f.height,f.width,180,2,250,blobs.data(),capacity);
        if(count>=0)break;
        if(capacity>=limit)return -1;
        capacity=std::min(capacity*2,limit);
    }
    if(count>capacity)return -1;
    int kept=0;
    for(int i=0;i<count;++i)
        if(identity_pixel_in_ray_domain(f.view.camera,blobs[size_t(i)*5]+f.view.pixel_offset[0],blobs[size_t(i)*5+1]+f.view.pixel_offset[1]))++kept;
    return kept;
}
std::string stats(std::vector<double> v,int precision){
    if(v.empty())return "n=0";
    std::sort(v.begin(),v.end());
    auto at=[&](double p){return v[std::min(v.size()-1,size_t(std::max(1.,std::ceil(p*double(v.size()))))-1)];};
    std::ostringstream s;s.setf(std::ios::fixed);s.precision(precision);
    s<<"p50="<<at(.5)<<" p90="<<at(.9)<<" max="<<v.back();return s.str();
}
struct NullBuffer : std::streambuf { int overflow(int c) override { return c; } };
}

int main(int argc,char** argv){
    if(argc<4||argc>5){std::cerr<<"Usage: frames_bench <frames_dir> <private-led-json> <factory-camera-json> [max_pairs]\n";return 2;}
    size_t max_pairs=SIZE_MAX;
    if(argc==5){try{max_pairs=size_t(std::stoull(argv[4]));}catch(...){std::cerr<<"Invalid max_pairs\n";return 2;}}
    std::map<int,std::vector<Frame>> frames;
    LedModel model{};
    std::array<IdentityView,2> views;
    std::unique_ptr<NativeOpticalLibraries> libraries;
    try{
        const fs::path dir=argv[1];
        if(!fs::is_directory(dir))throw std::runtime_error("frames_dir missing: "+dir.string());
        for(const auto& entry:fs::directory_iterator(dir)){
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
        for(int cam:cams)if(frames[cam].empty())throw std::runtime_error("no video"+std::to_string(cam)+"_<ts_ns>.pgm in "+dir.string());
        model=load_tracker_leds(argv[2],pico::devices::serial());
        fs::path dataset;
        if(const char* env=std::getenv("PICO_CALIBRATION_DATASET"))dataset=direct_calibration_dataset(env);
        else try{dataset=direct_calibration_dataset(dir);}catch(const std::exception&){dataset=direct_calibration_dataset(fs::absolute(dir).parent_path());}
        const auto rig=load_replay_rig(dataset,argv[3]);
        for(int i=0;i<2;++i)views[size_t(i)]=rig.bind(serials[i],1056,1024);
        std::cout<<"calibration_dataset="<<dataset.string()<<'\n';
        const auto library_root=fs::canonical(argv[0]).parent_path().parent_path().parent_path()/"candidate/release/optical";
        libraries=std::make_unique<NativeOpticalLibraries>((library_root/"libpico_optics.so").string(),(library_root/"libpico_detector.so").string());
    }catch(const std::exception& e){std::cerr<<"frames_bench: "<<e.what()<<'\n';return 2;}
    const auto api=libraries->api();

    // Header facts per camera from its first file (lower cameras reported only; this bench is upper stereo).
    for(auto& entry:frames){
        auto& list=entry.second;
        std::sort(list.begin(),list.end(),[](const Frame& a,const Frame& b){return a.stamp<b.stamp;});
        const auto g=read_pgm(list.front().path,false);
        std::cout<<"video"<<entry.first<<": files="<<list.size()<<" first="<<list.front().path.filename().string();
        if(g.error.empty())std::cout<<" P5 "<<g.width<<"x"<<g.height<<" maxval="<<g.maxval<<" header="<<g.header
                                   <<"B payload="<<(g.file_size-g.header)<<"B stride="<<g.width<<'\n';
        else std::cout<<" error="<<g.error<<'\n';
    }
    // Short exposure: <15 ms after the previous frame of the same camera.
    std::array<std::vector<Frame>,2> shorts;
    for(int i=0;i<2;++i){
        const auto& list=frames[cams[i]];
        for(size_t j=1;j<list.size();++j)if(list[j].stamp-list[j-1].stamp<15000000)shorts[size_t(i)].push_back(list[j]);
    }
    std::vector<std::array<Frame,2>> pairs;
    for(const auto& a:shorts[0]){
        const auto& b=shorts[1];
        auto it=std::lower_bound(b.begin(),b.end(),a.stamp,[](const Frame& f,int64_t t){return f.stamp<t;});
        const Frame* best=nullptr;
        if(it!=b.end())best=&*it;
        if(it!=b.begin()&&(!best||std::llabs(std::prev(it)->stamp-a.stamp)<std::llabs(best->stamp-a.stamp)))best=&*std::prev(it);
        if(best&&std::llabs(best->stamp-a.stamp)<=2000000)pairs.push_back({a,*best});
    }
    const size_t matched=pairs.size();
    if(pairs.size()>max_pairs)pairs.resize(max_pairs);

    // Reasons the stereo path can log; any other reason seen as the last drop of a call is added.
    std::set<std::string> reasons{"Native detector failed","Nonfinite native blob","Blob outside camera ray domain",
        "Four measured blobs per stereo view required","Native stereo solve rejected","Native mono solve residual/ambiguity rejected",
        "Actual full decoded matched stereo required","Stale/future identity exposure","Same-exposure measured lit bracket required",
        "Candidate timestamp mismatch","Candidate exposure/head provenance mismatch"};
    std::array<std::vector<double>,2> blobs;
    std::vector<double> wall_ms;
    std::map<std::string,uint64_t> skipped,errors;
    uint64_t stereo=0,mono=0,empty=0;
    NullBuffer null;
    auto* saved=std::cerr.rdbuf(&null); // log_optical_drop stderr lines are summarized below instead
    for(const auto& source:pairs){
        auto pair=std::make_shared<IdentityPair>();
        pair->timestamp_matched=true;pair->direct_capture=true;pair->clock_error=0;
        bool loaded=true;
        for(size_t i=0;i<2&&loaded;++i){
            auto g=read_pgm(source[i].path,true);
            if(!g.error.empty()){++skipped["video"+std::to_string(cams[i])+" "+g.error];loaded=false;break;}
            auto& f=pair->frames[i];
            f.serial=serials[i];f.width=g.width;f.height=g.height;f.timestamp_matched=true;
            f.stamp=source[i].stamp;f.exposure_us=exposure_us;f.view=views[i];f.pixels=std::move(g.pixels);
            f.metadata="pgm:"+f.serial+":"+source[i].path.filename().string();
            const int n=blob_count(api,f);
            if(n>=0)blobs[i].push_back(n);
        }
        if(!loaded)continue;
        const int64_t now=std::max(pair->frames[0].stamp,pair->frames[1].stamp);
        IdentityCandidate candidate;
        const auto start=std::chrono::steady_clock::now();
        try{OpticalComputeBudget budget;static std::optional<Rigid> prior;candidate=identity_stereo_candidate(api,model,pair,now,prior?prior->data():nullptr);if(candidate.stereo_verified)prior=candidate.world;else if(!candidate.mono)prior.reset();}
        catch(const std::exception& e){++errors[e.what()];}
        wall_ms.push_back(std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count());
        if(!optical_last_drop().empty())reasons.insert(optical_last_drop());
        if(candidate.pair&&candidate.stereo_verified)++stereo;
        else if(candidate.pair&&candidate.mono)++mono;
        else ++empty;
    }
    std::cerr.rdbuf(saved);

    std::cout<<"short_frames video0="<<shorts[0].size()<<" video3="<<shorts[1].size()
             <<" pairs_matched="<<matched<<" pairs_run="<<wall_ms.size()<<'\n';
    std::cout<<"blobs video0 "<<stats(blobs[0],0)<<"\nblobs video3 "<<stats(blobs[1],0)<<'\n';
    std::cout<<"wall_ms identity_stereo_candidate "<<stats(wall_ms,2)<<'\n';
    std::cout<<"candidates stereo_verified="<<stereo<<" mono="<<mono<<" none="<<empty<<'\n';
    uint64_t attributed=0;
    std::cout<<"optical_drop histogram (total="<<optical_drops()<<"):\n";
    for(const auto& r:reasons){const auto n=optical_drops(r);attributed+=n;if(n)std::cout<<"  "<<n<<"  "<<r<<'\n';}
    if(optical_drops()>attributed)std::cout<<"  "<<(optical_drops()-attributed)<<"  (unattributed reasons)\n";
    for(const auto& s:skipped)std::cout<<"skipped "<<s.second<<"  "<<s.first<<'\n';
    for(const auto& e:errors)std::cout<<"exception "<<e.second<<"  "<<e.first<<'\n';
    return 0;
}
