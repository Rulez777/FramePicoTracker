#include "../device_config.hpp"
// Build (aarch64, from native/; frames_bench link set):
//   g++ -std=c++17 -O2 -fno-fast-math -ffp-contract=off -pthread -I. -o build/flip_audit tools/flip_audit.cpp replay_calibration.cpp openocd_process.cpp roy_owner.cpp tcl_transport.cpp owner_lifecycle.cpp image_admission.cpp radio_loop.cpp peer_engine.cpp station_control.cpp query_peer.cpp native25.cpp native_identity.cpp data_plane.cpp -ldl
// Usage: build/flip_audit <capture_dir> <private-led-json> <factory-camera-json>
// Offline: upper short-exposure pairs through identity_stereo_candidate with the live prior rule (last accepted
// pose <=300 ms), head = identity (poses HEAD<-tracker). For consecutive stereo poses <=100 ms apart, a rotation
// step >20 deg is a flip. Each flip prints both poses' blob counts, inliers, residual, condition, rotation std,
// prior continuation, the step axis in the tracker frame, and whether the next pose returns (outlier vs switch).
// One line per stereo pose on stdout (CSV), the flip report and summary on stderr.
#include "calibration_discovery.hpp"
#include "optical_compute.hpp"
#include "replay_calibration.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
namespace {
using namespace pico;
namespace fs=std::filesystem;
struct Pgm{int w=0,h=0;std::vector<uint8_t> px;};
bool read_pgm(const fs::path&p,Pgm&o){std::ifstream f(p,std::ios::binary);std::string m;int mx;f>>m>>o.w>>o.h>>mx;f.get();
    if(m!="P5")return false;o.px.resize(size_t(o.w)*o.h);f.read((char*)o.px.data(),o.px.size());return bool(f);}
struct Frame{int64_t stamp;fs::path path;};
std::vector<Frame> list(const fs::path&dir,const std::string&cam){
    std::vector<Frame> v;const auto pre=cam+"_";
    for(auto&e:fs::directory_iterator(dir)){auto n=e.path().filename().string();if(n.rfind(pre,0)==0)v.push_back({std::stoll(n.substr(pre.size())),e.path()});}
    std::sort(v.begin(),v.end(),[](auto&a,auto&b){return a.stamp<b.stamp;});return v;}
std::vector<size_t> shorts(const std::vector<Frame>&v){std::vector<size_t> s;for(size_t j=1;j<v.size();++j)if(v[j].stamp-v[j-1].stamp<15000000)s.push_back(j);return s;}
int count_blobs(const NativeOpticalApi&api,const Pgm&g){std::vector<double> b(1024*5);return api.detect(g.px.data(),g.h,g.w,180,2,250,b.data(),1024);}
// Rotation angle (deg) and axis in the tracker frame of Ra^T*Rb (row-major rigid).
double step(const Rigid&a,const Rigid&b,double axis[3]){
    double m[9];for(int i=0;i<3;++i)for(int j=0;j<3;++j){m[3*i+j]=0;for(int k=0;k<3;++k)m[3*i+j]+=a[4*k+i]*b[4*k+j];}
    const double c=std::clamp((m[0]+m[4]+m[8]-1)/2,-1.,1.),ang=std::acos(c);
    double x=m[7]-m[5],y=m[2]-m[6],z=m[3]-m[1],n=std::sqrt(x*x+y*y+z*z);
    if(n<1e-9){ // near 180: axis from the largest diagonal of (m+I)/2
        int k=m[0]>m[4]?(m[0]>m[8]?0:2):(m[4]>m[8]?1:2);double v[3];for(int i=0;i<3;++i)v[i]=(m[3*i+k]+(i==k?1:0))/2;
        n=std::sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);x=v[0];y=v[1];z=v[2];}
    axis[0]=x/n;axis[1]=y/n;axis[2]=z/n;return ang*180/M_PI;}
struct Pose{int64_t t;int n0,n3;IdentityCandidate c;};
struct NullBuffer:std::streambuf{int overflow(int c)override{return c;}};
}
int main(int argc,char**argv){
    if(argc<4){std::cerr<<"usage: flip_audit <capture_dir> <led-json> <factory-camera-json>\n";return 2;}
    const fs::path cap=argv[1],dir=cap/"frames";
    const char* serial[2]={"CAMUPPERLEFT","CAMUPPERRIGHT"};const char* cam[2]={"video0","video3"};
    LedModel model=load_tracker_leds(argv[2],pico::devices::serial());
    const auto rig=load_replay_rig(direct_calibration_dataset(cap/"calibration"),argv[3]);
    IdentityView uv[2];for(int i=0;i<2;++i)uv[i]=rig.bind(serial[i],1056,1024);
    const auto root=fs::canonical(argv[0]).parent_path().parent_path().parent_path()/"candidate/release/optical";
    NativeOpticalLibraries libs((root/"libpico_optics.so").string(),(root/"libpico_detector.so").string());
    const auto api=libs.api();
    std::vector<Frame> U[2];std::vector<size_t> US[2];for(int i=0;i<2;++i){U[i]=list(dir,cam[i]);US[i]=shorts(U[i]);}
    std::vector<Pose> poses;std::optional<Rigid> prior;int64_t prior_ns=0;
    NullBuffer null;auto*saved=std::cerr.rdbuf(&null);
    for(size_t j:US[0]){
        const Frame&a=U[0][j];const Frame*b=nullptr;int64_t bd=2000001;
        for(size_t k:US[1]){auto d=std::llabs(U[1][k].stamp-a.stamp);if(d<bd){bd=d;b=&U[1][k];}}
        if(!b)continue;
        auto pair=std::make_shared<IdentityPair>();pair->timestamp_matched=pair->direct_capture=true;pair->clock_error=0;
        const Frame*src[2]={&a,b};int n[2]={0,0};bool ok=true;
        for(int i=0;i<2;++i){Pgm g;if(!read_pgm(src[i]->path,g)){ok=false;break;}auto&f=pair->frames[i];
            f.serial=serial[i];f.width=g.w;f.height=g.h;f.timestamp_matched=true;f.stamp=src[i]->stamp;f.exposure_us=782.267;
            f.view=uv[i];n[i]=count_blobs(api,g);f.pixels=std::move(g.px);f.metadata="pgm:"+src[i]->path.filename().string();}
        if(!ok)continue;
        const bool use_prior=prior&&a.stamp-prior_ns<=300000000;
        IdentityCandidate c;
        try{OpticalComputeBudget budget;c=identity_stereo_candidate(api,model,pair,std::max(a.stamp,b->stamp),use_prior?prior->data():nullptr);}catch(...){}
        if(!c.pair||!c.stereo_verified)continue;
        prior=c.world;prior_ns=a.stamp;poses.push_back({a.stamp,n[0],n[1],c});
    }
    std::cerr.rdbuf(saved);
    std::printf("t_ms,n0,n3,inliers,residual,condition,rot_std,pos_std,cont,step_deg\n");
    std::map<std::string,long> sum;std::vector<double> res_ok,res_flip;
    for(size_t i=0;i<poses.size();++i){
        const auto&p=poses[i];double ax[3]={0,0,0},st=-1;
        if(i&&p.t-poses[i-1].t<=100000000)st=step(poses[i-1].c.world,p.c.world,ax);
        std::printf("%.1f,%d,%d,%d,%.3f,%.1f,%.2f,%.4f,%d,%.2f\n",(p.t-poses[0].t)/1e6,p.n0,p.n3,p.c.inliers,p.c.residual,p.c.condition,
            p.c.rotation_std_deg,p.c.position_std,int(p.c.prior_continuation),st);
        if(st<0)continue;
        if(st<=20){res_ok.push_back(p.c.residual);++sum["steps <=20"];continue;}
        ++sum["flips >20"];res_flip.push_back(p.c.residual);
        // Return test: the next pose within 100 ms back near the previous one -> single outlier; else a switch.
        std::string kind="switch/end";
        if(i+1<poses.size()&&poses[i+1].t-p.t<=100000000){double a2[3];double back=step(poses[i-1].c.world,poses[i+1].c.world,a2);
            kind=back<=20?"outlier (next returns)":"switch (next stays)";}
        ++sum[kind];
        if(p.c.prior_continuation)++sum["flip pose seeded by prior"];else ++sum["flip pose blind"];
        if(st>150)++sum["flip angle >150 (near symmetric)"];
        std::fprintf(stderr,"flip t=%.1fms step=%.1f axis(%.2f %.2f %.2f) %s | prev n=%d/%d inl=%d res=%.3f | this n=%d/%d inl=%d res=%.3f cond=%.1f rstd=%.2f cont=%d\n",
            (p.t-poses[0].t)/1e6,st,ax[0],ax[1],ax[2],kind.c_str(),poses[i-1].n0,poses[i-1].n3,poses[i-1].c.inliers,poses[i-1].c.residual,
            p.n0,p.n3,p.c.inliers,p.c.residual,p.c.condition,p.c.rotation_std_deg,int(p.c.prior_continuation));
    }
    auto med=[](std::vector<double> v){if(v.empty())return -1.;std::sort(v.begin(),v.end());return v[v.size()/2];};
    std::fprintf(stderr,"stereo poses %zu\n",poses.size());
    for(auto&[k,v]:sum)std::fprintf(stderr,"%s: %ld\n",k.c_str(),v);
    std::fprintf(stderr,"residual median: normal steps %.3f, flip poses %.3f\n",med(res_ok),med(res_flip));
}
