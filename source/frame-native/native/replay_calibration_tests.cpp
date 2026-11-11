#include "native_test_runtime.hpp"
#include "replay_calibration.hpp"
#include <cmath>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include "head_history_tests.hpp"
using namespace pico;
namespace {
int checks=0;void check(bool b){++checks;if(!b)throw std::runtime_error("calibration check "+std::to_string(checks));}
template<class F>void rejects(F f){bool bad=false;try{f();}catch(const std::exception&){bad=true;}check(bad);}
phase5::Value json(const std::string&s){return phase5::Json(s).parse();}
std::string axes(double x,const std::string& units=""){
    return "{\"plus_x\":[1,0,0],\"plus_z\":[0,0,1],\"position\":["+std::to_string(x)+",0,0]"+units+"}";
}
ReplayRig rig(){
    std::string factory="{\"cameras\":[",active="{\"cameras\":[";
    for(int i=0;i<2;++i){if(i){factory+=",";active+=",";}auto name=i?"right":"left";auto serial=i?"CAMUPPERRIGHT":"CAMUPPERLEFT";
        factory+="{\"serial\":\""+std::string(serial)+"\",\"sourceCamera\":\""+name+"\",\"intrinsics\":[{\"cameraModel\":\"kb\",\"cx\":527.5,\"cy\":511.5}]}";
        active+="{\"sourceCamera\":\""+std::string(name)+"\",\"cameraModel\":\"kb\",\"width\":1056,\"height\":1024,\"intrinsics\":{\"fx\":400,\"fy\":400,\"cx\":528,\"cy\":512,\"k1\":0,\"k2\":0,\"k3\":0,\"k4\":0},\"extrinsics\":"+axes(i?60:-60,",\"__units\":\"mm\"")+"}";
    }
    return {json(factory+"]}"),json(active+"]}"),json("{\"head\":"+axes(.2)+",\"cv\":{\"cad_from_cal\":"+axes(.3)+"}}")};
}
LedModel model(){return {-.04,-.03,0, .01,-.035,.008, .042,-.01,-.005, .035,.027,.012, -.015,.04,-.01, -.04,.01,.015, .006,.008,.025, -.02,-.012,-.018};}
std::array<std::vector<double>,2> expected;
int detect(const uint8_t* p,int h,int w,int threshold,int min,int max,double*out,int cap){
    check(h==1024&&w==1056&&threshold==180&&min==2&&max==250&&cap>=8);if(!p[0])return 0;
    const auto& xy=expected[p[0]-1];for(size_t i=0;i<8;++i){out[5*i]=xy[2*i]-.5;out[5*i+1]=xy[2*i+1]-.5;out[5*i+2]=1;out[5*i+3]=255;out[5*i+4]=9;}return 8;
}
int mutation=0;
std::array<Rigid,2> expected_heads{identity_rigid(),identity_rigid()};
ExposureHeads identity_heads(const std::array<camera::GrayFrame,2>& pair){return {{nearest_even(*pair[0].metadata.timestamp*1e9),nearest_even(*pair[1].metadata.timestamp*1e9)},expected_heads};}
int solve(const double*m,const OpticalView*v,const double* prior,OpticalFit*f){
    check(!prior);for(int i=0;i<24;++i)check(m[i]==model()[size_t(i)]);
    for(int s=0;s<2;++s){check(v[s].count==8);for(int row=0;row<3;++row){for(int col=0;col<3;++col)check(std::abs(v[s].head_from_camera[4*row+col]-expected_heads[size_t(s)][size_t(4*row+col)])<1e-12);
    check(std::abs(v[s].head_from_camera[4*row+3]-(expected_heads[size_t(s)][size_t(4*row)]*(s?.16:.04)+expected_heads[size_t(s)][size_t(4*row+3)]))<1e-12);}for(int i=0;i<16;++i)check(std::abs(v[s].pixels[i]-expected[size_t(s)][size_t(i)])<1e-10);}
    auto pose=identity_rigid();pose[3]=.1;pose[11]=1;std::copy(pose.begin(),pose.end(),f->pose);f->count=8;f->residual=mutation?2.51:0;return 0;
}
camera::GrayFrame frame(int side,double stamp=10){camera::GrayFrame f;f.pixels.resize(1056u*1024u);f.pixels[0]=uint8_t(side+1);f.metadata.sensor=side?"CAMUPPERRIGHT":"CAMUPPERLEFT";f.metadata.timestamp=stamp;f.metadata.exposure=.0008;return f;}
void projections(){auto r=rig();auto m=model();for(int s=0;s<2;++s){auto b=r.bind(s?"CAMUPPERRIGHT":"CAMUPPERLEFT",1056,1024);expected[size_t(s)].clear();for(int i=0;i<8;++i){double x=m[size_t(3*i)]+.1-b.head_from_camera[3],y=m[size_t(3*i+1)],z=m[size_t(3*i+2)]+1;double radius=std::hypot(x,y),scale=std::atan2(radius,z)/radius;expected[size_t(s)].push_back(528+400*x*scale);expected[size_t(s)].push_back(512+400*y*scale);}}}
std::string encode(const std::vector<uint8_t>& b){std::string out,abc="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";for(size_t i=0;i<b.size();i+=3){uint32_t w=(uint32_t(b[i])<<16)|(uint32_t(b[i+1])<<8)|b[i+2];for(int j=18;j>=0;j-=6)out+=abc[(w>>j)&63];}return out;}
void leds(){std::vector<uint8_t>b(660);b[204]=1;b[208]=60;
    // One little-endian float 0.0625, represented independently of the loader.
    b[214]=0x80;b[215]=0x3d;
    auto make=[&](const std::string& payload){return json("{\"calibration_data\":[{\"sn\":\"synthetic\",\"calibration\":\""+payload+"\"}]}");};
    auto root=make(encode(b));check(load_replay_leds(root,"synthetic")[0]==.0625);rejects([&]{load_replay_leds(root,"absent");});
    b[204]=2;rejects([&]{load_replay_leds(make(encode(b)),"synthetic");});b[204]=1;b[215]=0x7f;b[214]=0x80;rejects([&]{load_replay_leds(make(encode(b)),"synthetic");});
    auto payload=encode(b);payload[0]='!';rejects([&]{load_replay_leds(make(payload),"synthetic");});
}
}
int main(){try{
    // Exercise the interrupted loader diagnostic, including parse/type failures
    // after unique discovery. Retain fixtures as reviewable failure evidence.
    const auto dataset=std::filesystem::absolute("build/rig-load-"+
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directories(dataset);
    auto put=[&](const char* name,const std::string& bytes){
        std::ofstream out(dataset/name);out<<bytes;check(bool(out));};
    auto diagnostic=[&](const std::string& expected_text){
        try{(void)load_replay_rig(dataset,dataset/"factory.json");}
        catch(const std::exception& error){const std::string message=error.what();
            check(message.find(expected_text)!=std::string::npos);
            check(message.find(dataset.string())!=std::string::npos);return;}
        check(false);
    };
    diagnostic("candidates=0");
    put("live_device_config.json","{}");
    put("factory.json","{\"cameras\":[]}");
    put("sensorRigCalibration.json","invalid json");
    diagnostic((dataset/"live_device_config.json").string());
    put("sensorRigCalibration.json","{}");
    (void)load_replay_rig(dataset,dataset/"factory.json");
    put("second_device_config.json","{}");
    diagnostic("candidates=2");
    head_history_tests::run();
    auto r=rig();auto b=r.bind("CAMUPPERLEFT",1056,1024);check(std::abs(b.head_from_camera[3]-.04)<1e-12);check(b.pixel_offset[0]==.5&&b.pixel_offset[1]==.5);
    rejects([&]{r.bind("unknown",1056,1024);});rejects([&]{r.bind("CAMUPPERLEFT",1024,1056);});
    auto bad=r;bad.active.object["cameras"].array[0].object["extrinsics"].object["__units"].text="m";rejects([&]{bad.bind("CAMUPPERLEFT",1056,1024);});
    bad=r;bad.factory.object["cameras"].array.push_back(bad.factory.object["cameras"].array[0]);rejects([&]{bad.bind("CAMUPPERLEFT",1056,1024);});
    bad=r;bad.active.object["cameras"].array[0].object["intrinsics"].object["cx"].text="528.25";rejects([&]{bad.bind("CAMUPPERLEFT",1056,1024);});
    bad=r;bad.device.object["head"].object["plus_x"].array[0].text="2";rejects([&]{bad.bind("CAMUPPERLEFT",1056,1024);});
    auto rotated=r;rotated.device.object["head"].object["plus_x"]=json("[0,1,0]");
    auto rotated_view=rotated.bind("CAMUPPERLEFT",1056,1024);
    check(std::abs(rotated_view.head_from_camera[3])<1e-12&&std::abs(rotated_view.head_from_camera[7]+.04)<1e-12);
    check(rotated_view.head_from_camera[1]==1&&rotated_view.head_from_camera[4]==-1);
    leds();projections();ReplayOptical replay(r,model(),{nullptr,solve,detect},identity_heads);
    int exported=0;
    replay.pose_sink=[&](const Rigid& pose,const std::array<camera::GrayFrame,2>& pair,uint64_t id){++exported;check(id==1&&pose[3]==.1&&pose[11]==1);check(*pair[0].metadata.sensor=="CAMUPPERLEFT"&&*pair[1].metadata.sensor=="CAMUPPERRIGHT");check(*pair[0].metadata.timestamp==10);};
    replay.frame(frame(1));replay.frame(frame(0));check(replay.counts.pairs==1&&replay.counts.candidates==1&&replay.counts.stereo_verified==1&&replay.counts.world_poses==1);
    mutation=1;replay.frame(frame(0,11));replay.frame(frame(1,11));check(replay.counts.world_poses==1&&replay.counts.rejected["fit_quality"]==1);mutation=0;
    replay.frame(frame(0,12));replay.frame(frame(1,12.001));replay.finish();check(replay.counts.unpaired==2);
    auto dark=frame(0,13);dark.pixels[0]=0;replay.frame(dark);replay.frame(frame(1,13));check(replay.counts.world_poses==1);
    auto unverified=frame(0,14);unverified.source_time_verified=true;rejects([&]{replay.frame(unverified);});
    check(exported==1);
    ReplayOptical missing(r,model(),{nullptr,solve,detect},[](const std::array<camera::GrayFrame,2>&)->ExposureHeads{throw std::runtime_error("missing");});
    missing.frame(frame(0));missing.frame(frame(1));check(missing.counts.rejected["head_history_or_clock"]==1&&missing.counts.candidates==0&&missing.counts.world_poses==0);
    expected_heads[0]=head_history_tests::rotation(.4,1);expected_heads[1]=head_history_tests::rotation(.40001,1.00002);
    ReplayOptical moving(r,model(),{nullptr,solve,detect},identity_heads);moving.frame(frame(0));moving.frame(frame(1,10.000001));check(moving.counts.world_poses==1);
    expected_heads={identity_rigid(),identity_rigid()};
#ifndef _WIN32
    NativeOpticalLibraries libraries("../candidate/release/optical/libpico_optics.so","../candidate/release/optical/libpico_detector.so");
    ReplayOptical real(r,model(),libraries.api(),identity_heads);
    for(int s=0;s<2;++s){auto f=frame(s);f.pixels[0]=0;for(int i=0;i<8;++i){int x=int(std::round(expected[size_t(s)][size_t(2*i)]-.5)),y=int(std::round(expected[size_t(s)][size_t(2*i+1)]-.5));for(int dy=-1;dy<=1;++dy)for(int dx=-1;dx<=1;++dx)f.pixels[size_t(y+dy)*1056+size_t(x+dx)]=255;}real.frame(f);}
    check(real.counts.world_poses==1&&real.last_pose);
    check(std::abs((*real.last_pose)[3]-.1)<.01&&std::abs((*real.last_pose)[7])<.01&&std::abs((*real.last_pose)[11]-1)<.02);
#endif
    std::cout<<checks<<" replay calibration checks passed\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
