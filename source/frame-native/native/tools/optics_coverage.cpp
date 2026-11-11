#include "../device_config.hpp"
// Build (aarch64, from native/; frames_bench link set):
//   g++ -std=c++17 -O2 -fno-fast-math -ffp-contract=off -pthread -I. -o build/optics_coverage tools/optics_coverage.cpp replay_calibration.cpp openocd_process.cpp roy_owner.cpp tcl_transport.cpp owner_lifecycle.cpp image_admission.cpp radio_loop.cpp peer_engine.cpp station_control.cpp query_peer.cpp native25.cpp native_identity.cpp data_plane.cpp -ldl
// Usage: build/optics_coverage <capture_dir> <private-led-json> <factory-camera-json> [swap]
// Offline: where does the tracker go when the upper stereo pose is lost, and do the lower cameras see its LEDs?
// For every upper short-exposure pair (head = identity, so poses are HEAD<-tracker): blob counts, stereo candidate.
// For every pose: the 8 LEDs projected into both upper and both lower cameras (lower serial map as in
// xrtap_source.hpp, "swap" = reversed), pixel max in a 7x7 box at each projection in the lower short (dark) frame and
// in the lower long frame before it. Upper projections check the transform (dark upper frame must be bright there).
// Per pair CSV on stdout; summary on stderr.
#include "calibration_discovery.hpp"
#include "optical_compute.hpp"
#include "replay_calibration.hpp"
#include "mono_solver.hpp"
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
// Short (dark, LED) frame = <15 ms after the previous frame; index into the full list.
std::vector<size_t> shorts(const std::vector<Frame>&v){std::vector<size_t> s;for(size_t j=1;j<v.size();++j)if(v[j].stamp-v[j-1].stamp<15000000)s.push_back(j);return s;}
const Frame* nearest(const std::vector<Frame>&v,const std::vector<size_t>&s,int64_t t,int64_t tol,size_t*idx=nullptr){
    const Frame*best=nullptr;int64_t bd=tol+1;for(size_t j:s){auto d=std::llabs(v[j].stamp-t);if(d<bd){bd=d;best=&v[j];if(idx)*idx=j;}}return best;}
int detect_count(const NativeOpticalApi&api,const Pgm&g){std::vector<double> b(1024*5);return api.detect(g.px.data(),g.h,g.w,180,2,250,b.data(),1024);}
// camera <- tracker point: inv(head_from_camera) * pose * p (row-major rigid, translation at 3/7/11)
void to_camera(const Rigid&hc,const double*pose,const double*p,double*o){
    double h[3];for(int r=0;r<3;++r)h[r]=pose[4*r]*p[0]+pose[4*r+1]*p[1]+pose[4*r+2]*p[2]+pose[4*r+3];
    double d[3]={h[0]-hc[3],h[1]-hc[7],h[2]-hc[11]};
    for(int r=0;r<3;++r)o[r]=hc[r]*d[0]+hc[4+r]*d[1]+hc[8+r]*d[2];}
int boxmax(const Pgm&g,double x,double y){int m=-1;int xi=int(std::lround(x)),yi=int(std::lround(y));
    for(int dy=-3;dy<=3;++dy)for(int dx=-3;dx<=3;++dx){int xx=xi+dx,yy=yi+dy;if(xx<0||yy<0||xx>=g.w||yy>=g.h)continue;m=std::max(m,int(g.px[size_t(yy)*g.w+xx]));}return m;}
// Projected LED pixels of a pose in one view; returns how many land inside w x h, box max over them in img (or -1).
int project_box(const NativeOpticalApi&api,const IdentityView&v,const double*pose,const LedModel&model,int w,int h,const Pgm*img,int&box){
    int inside=0;box=-1;
    for(int e=0;e<8;++e){double cp[3],px[2];to_camera(v.head_from_camera,pose,&model[3*e],cp);
        if(cp[2]<=0.02||api.project(&v.camera,cp,1,px)!=0)continue;
        px[0]-=v.pixel_offset[0];px[1]-=v.pixel_offset[1];
        if(px[0]<0||px[1]<0||px[0]>=w||px[1]>=h)continue;++inside;
        if(img)box=std::max(box,boxmax(*img,px[0],px[1]));}
    return inside;}
// Same steps as native_identity.cpp detect_view: detector, ray-domain filter, compact cluster, WORLD<-camera.
bool view_of(const NativeOpticalApi&api,const IdentityExposure&f,OpticalView&view,std::vector<double>&pixels){
    std::vector<double> b(1024*5);const int count=api.detect(f.pixels.data(),f.height,f.width,180,2,250,b.data(),1024);
    if(count<0||count>1024)return false;
    std::vector<double> acc;
    for(int i=0;i<count;++i){const double x=b[5*i]+f.view.pixel_offset[0],y=b[5*i+1]+f.view.pixel_offset[1];
        if(identity_pixel_in_ray_domain(f.view.camera,x,y))acc.insert(acc.end(),{x,y,b[5*i+2],b[5*i+3],b[5*i+4]});}
    pixels=compact_led_cluster(acc);view.camera=f.view.camera;
    std::copy(f.view.head_from_camera.begin(),f.view.head_from_camera.end(),view.head_from_camera); // world = head here
    view.count=pixels.size()/2;view.pixels=pixels.data();return true;}
struct NullBuffer:std::streambuf{int overflow(int c)override{return c;}};
}
int main(int argc,char**argv){
    if(argc<4){std::cerr<<"usage: optics_coverage <capture_dir> <led-json> <factory-camera-json> [swap]\n";return 2;}
    const fs::path cap=argv[1],dir=cap/"frames";const bool swap=argc>4&&std::string(argv[4])=="swap";
    const char* upper_serial[2]={"CAMUPPERLEFT","CAMUPPERRIGHT"};const char* upper_cam[2]={"video0","video3"};
    const char* lower_cam[2]={"video9","video13"};
    std::string lower_serial[2]={"CAMLOWERLEFT","CAMLOWERRIGHT"};if(swap)std::swap(lower_serial[0],lower_serial[1]);
    LedModel model=load_tracker_leds(argv[2],pico::devices::serial());
    const auto rig=load_replay_rig(direct_calibration_dataset(cap/"calibration"),argv[3]);
    IdentityView uv[2],lv[2];
    for(int i=0;i<2;++i){uv[i]=rig.bind(upper_serial[i],1056,1024);lv[i]=rig.bind(lower_serial[i],640,480);}
    const auto root=fs::canonical(argv[0]).parent_path().parent_path().parent_path()/"candidate/release/optical";
    NativeOpticalLibraries libs((root/"libpico_optics.so").string(),(root/"libpico_detector.so").string());
    const auto api=libs.api();
    std::vector<Frame> U[2],L[2];std::vector<size_t> US[2],LS[2];
    for(int i=0;i<2;++i){U[i]=list(dir,upper_cam[i]);US[i]=shorts(U[i]);L[i]=list(dir,lower_cam[i]);LS[i]=shorts(L[i]);}
    std::map<std::string,long> sum;std::vector<int> lower_dark[2],lower_long[2],upper_box;
    NullBuffer null;auto*saved=std::cerr.rdbuf(&null);
    std::optional<Rigid> prior;int64_t prior_ns=0;std::vector<std::pair<int64_t,Rigid>> history;
    auto envd=[](const char*k,double d){const char*v=std::getenv(k);return v?std::atof(v):d;};
    const int64_t seed_age_ns=int64_t(envd("SEED_AGE_MS",0)*1e6);const bool blind=envd("BLIND",0)!=0;
    seeded_turn_deg=envd("SEED_TURN",seeded_turn_deg);seeded_shift_m=envd("SEED_SHIFT",seeded_shift_m);
    struct MonoCheck{long tried=0,ok=0,via_seed=0,bad=0;std::vector<double> pos,rot;} mono_check[2];
    std::printf("stamp,n0,n3,kind,tx,ty,tz,in9,dark9,long9,in13,dark13,long13,up_box_min\n");
    for(size_t j:US[0]){
        const Frame&a=U[0][j];const Frame*b=nearest(U[1],US[1],a.stamp,2000000);if(!b)continue;
        auto pair=std::make_shared<IdentityPair>();pair->timestamp_matched=pair->direct_capture=true;pair->clock_error=0;
        const Frame*src[2]={&a,b};int n[2]={0,0};Pgm g[2];bool ok=true;
        for(int i=0;i<2;++i){if(!read_pgm(src[i]->path,g[i])){ok=false;break;}auto&f=pair->frames[i];
            f.serial=upper_serial[i];f.width=g[i].w;f.height=g[i].h;f.timestamp_matched=true;f.stamp=src[i]->stamp;f.exposure_us=782.267;
            f.view=uv[i];f.pixels=g[i].px;f.metadata="pgm:"+src[i]->path.filename().string();n[i]=detect_count(api,g[i]);}
        if(!ok)continue;
        IdentityCandidate c;
        // Live rule (optical_worker.cpp): the last accepted pose is the prior while it is <=300 ms old.
        const bool use_prior=prior&&a.stamp-prior_ns<=300000000;
        try{OpticalComputeBudget budget;c=identity_stereo_candidate(api,model,pair,std::max(a.stamp,b->stamp),use_prior?prior->data():nullptr);}catch(...){}
        // Mono check against the stereo pose of the same pair: each view alone, seeded by the prior and blind.
        const Rigid* seed=nullptr;for(auto h=history.rbegin();h!=history.rend();++h)if(a.stamp-h->first>=seed_age_ns){if(a.stamp-h->first<=seed_age_ns+100000000)seed=&h->second;break;}
        if(c.pair&&c.stereo_verified&&seed)for(int i=0;i<2;++i){
            OpticalView v{};std::vector<double> px;if(!view_of(api,pair->frames[i],v,px)||v.count<4)continue;
            for(int seeded=0;seeded<2;++seeded){OpticalFit f{};bool okm=false;
                if(!seeded&&!blind)continue;
                {OpticalComputeBudget budget;okm=solve_monocular(api,model,v,f,seeded?seed->data():nullptr);}
                auto&st=mono_check[seeded];++st.tried;if(!okm)continue;++st.ok;if(f.continuation)++st.via_seed;
                st.pos.push_back(1000*std::sqrt(std::pow(f.pose[3]-c.world[3],2)+std::pow(f.pose[7]-c.world[7],2)+std::pow(f.pose[11]-c.world[11],2)));
                double tr=0;for(int r=0;r<3;++r)for(int k=0;k<3;++k)tr+=f.pose[4*r+k]*c.world[4*r+k];
                st.rot.push_back(std::acos(std::clamp((tr-1)/2,-1.,1.))*180/M_PI);if(st.rot.back()>10||st.pos.back()>30)++st.bad;}
        }
        if(c.pair){prior=c.world;prior_ns=a.stamp;history.push_back({a.stamp,c.world});}
        if(!c.pair)++sum["none_reason "+std::string(n[0]>=4||n[1]>=4?"[one>=4] ":"")+optical_last_drop()+(use_prior?" [prior]":" [no prior]")];
        const char*kind=c.pair?(c.stereo_verified?"stereo":"mono"):"none";++sum[std::string("kind_")+kind];
        const char*bucket=n[0]>=4&&n[1]>=4?"both>=4":(n[0]>=4||n[1]>=4?"one>=4":(std::max(n[0],n[1])>0?"1..3":"0"));++sum[std::string("raw_blobs_")+bucket];
        std::printf("%lld,%d,%d,%s",(long long)a.stamp,n[0],n[1],kind);
        if(!c.pair){std::printf(",,,,,,,,,,\n");continue;}
        const double*pose=c.world.data();std::printf(",%.4f,%.4f,%.4f",pose[3],pose[7],pose[11]);
        for(int l=0;l<2;++l){
            size_t k=0;const Frame*d=nearest(L[l],LS[l],a.stamp,4000000,&k);
            Pgm gd,gl;const bool hd=d&&read_pgm(d->path,gd),hl=d&&k>0&&read_pgm(L[l][k-1].path,gl);
            int dark=-1,lng=-1;const int inside=project_box(api,lv[l],pose,model,640,480,hd?&gd:nullptr,dark);
            if(hl)project_box(api,lv[l],pose,model,640,480,&gl,lng);
            std::printf(",%d,%d,%d",inside,dark,lng);
            if(inside>=4){++sum[std::string("led4_in_")+lower_cam[l]];lower_dark[l].push_back(dark);lower_long[l].push_back(lng);}
        }
        int upmin=999;for(int i=0;i<2;++i){int m;project_box(api,uv[i],pose,model,1056,1024,&g[i],m);upmin=std::min(upmin,m);}
        upper_box.push_back(upmin);std::printf(",%d\n",upmin);
    }
    std::cerr.rdbuf(saved);
    auto pct=[](std::vector<int> v,double p){if(v.empty())return -1;std::sort(v.begin(),v.end());return v[std::min(v.size()-1,size_t(p*v.size()))];};
    std::cerr<<"lower map video9="<<lower_serial[0]<<" video13="<<lower_serial[1]<<"\n";
    // Camera optical axis (+z) and position in the head frame, intrinsics.
    const IdentityView* all[4]={&uv[0],&uv[1],&lv[0],&lv[1]};const char* names[4]={"video0","video3","video9","video13"};
    for(int i=0;i<4;++i){const auto&h=all[i]->head_from_camera;const auto&c=all[i]->camera;
        std::fprintf(stderr,"%s axis(%.3f %.3f %.3f) pos(%.3f %.3f %.3f) f=%.1f c=(%.1f %.1f) offset=(%.1f %.1f)\n",names[i],
            h[2],h[6],h[10],h[3],h[7],h[11],c.fx,c.cx,c.cy,all[i]->pixel_offset[0],all[i]->pixel_offset[1]);}
    for(auto&[k,v]:sum)std::cerr<<k<<"="<<v<<"\n";
    auto pd=[](std::vector<double> v,double p){if(v.empty())return -1.;std::sort(v.begin(),v.end());return v[std::min(v.size()-1,size_t(p*v.size()))];};
    for(int s=0;s<2;++s){auto&m=mono_check[s];std::fprintf(stderr,"mono vs stereo (%s): views=%ld solved=%ld via_seed=%ld bad(>10deg/>30mm)=%ld pos_mm p50=%.1f p90=%.1f max=%.1f rot_deg p50=%.2f p90=%.2f max=%.2f\n",
        s?"seeded":"blind",m.tried,m.ok,m.via_seed,m.bad,pd(m.pos,.5),pd(m.pos,.9),pd(m.pos,1),pd(m.rot,.5),pd(m.rot,.9),pd(m.rot,1));}
    std::cerr<<"upper dark frame, box max at projected LEDs (min of two views) over poses: p10="<<pct(upper_box,.1)<<" p50="<<pct(upper_box,.5)<<"\n";
    for(int l=0;l<2;++l)std::cerr<<lower_cam[l]<<" poses with >=4 LEDs inside image: "<<lower_dark[l].size()
        <<" dark box max p50="<<pct(lower_dark[l],.5)<<" p90="<<pct(lower_dark[l],.9)<<" max="<<pct(lower_dark[l],1)
        <<" | long box max p50="<<pct(lower_long[l],.5)<<" p90="<<pct(lower_long[l],.9)<<"\n";
}
