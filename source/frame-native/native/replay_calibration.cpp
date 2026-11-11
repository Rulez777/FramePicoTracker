#include "device_config.hpp"
#include "guard_evidence.hpp"
#include "replay_calibration.hpp"
#include "calibration_discovery.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
namespace pico {
namespace {
using phase5::Value; using phase5::field;
void need(bool b,const char* s){if(!b)throw std::runtime_error(s);}
double num(const Value& v){need(v.type=='i',"Calibration number required");double n=std::stod(v.text);need(std::isfinite(n),"Nonfinite calibration");return n;}
std::string str(const Value& v){need(v.type=='s',"Calibration string required");return v.text;}
const std::vector<Value>& arr(const Value& v){need(v.type=='[',"Calibration array required");return v.array;}
Rigid mul(const Rigid&a,const Rigid&b){Rigid c{};for(int i=0;i<4;++i)for(int j=0;j<4;++j)for(int k=0;k<4;++k)c[4*i+j]+=a[4*i+k]*b[4*k+j];
return c;}
Rigid transform(const Value& v,double scale){
    const auto& x=arr(field(v,"plus_x"));const auto& z=arr(field(v,"plus_z"));const auto& p=arr(field(v,"position"));
    need(x.size()==3&&z.size()==3&&p.size()==3,"Calibrated vector length");
    double a[3],b[3];for(int i=0;i<3;++i){a[i]=num(x[i]);b[i]=num(z[i]);}
    auto m=identity_rigid();for(int i=0;i<3;++i){m[4*i]=a[i];m[4*i+2]=b[i];m[4*i+1]=b[(i+1)%3]*a[(i+2)%3]-b[(i+2)%3]*a[(i+1)%3];m[4*i+3]=num(p[i])*scale;}
    need(identity_rigid_valid(m),"Invalid calibrated axis basis");return m;
}
Rigid inv(const Rigid&m){
    auto a=m,b=identity_rigid();
    for(int c=0;c<4;++c){int pivot=c;for(int r=c+1;r<4;++r)if(std::abs(a[4*r+c])>std::abs(a[4*pivot+c]))pivot=r;
        double d=a[4*pivot+c];need(std::isfinite(d)&&d!=0,"Singular calibrated transform");
        for(int k=0;k<4;++k){std::swap(a[4*c+k],a[4*pivot+k]);std::swap(b[4*c+k],b[4*pivot+k]);}
        for(int k=0;k<4;++k){a[4*c+k]/=d;b[4*c+k]/=d;}
        for(int r=0;r<4;++r)if(r!=c){double s=a[4*r+c];for(int k=0;k<4;++k){a[4*r+k]-=s*a[4*c+k];b[4*r+k]-=s*b[4*c+k];}}
    }return b;
}
const Value& unique(const Value& list,const char* key,const std::string& name){const Value* out=nullptr;for(const auto& v:arr(list))if(str(field(v,key))==name){need(!out,"Ambiguous calibration binding");out=&v;}
need(out,"Missing calibration binding");return *out;}
std::vector<uint8_t> base64(const std::string&s){
    need(s.size()==880,"Factory base64 length");std::vector<uint8_t> out;
    const std::string alphabet="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    for(size_t i=0;i<s.size();i+=4){uint32_t w=0;for(int j=0;j<4;++j){auto n=alphabet.find(s[i+size_t(j)]);need(n!=std::string::npos,"Invalid factory base64");w=(w<<6)|uint32_t(n);}out.push_back(uint8_t(w>>16));out.push_back(uint8_t(w>>8));out.push_back(uint8_t(w));}return out;
}
bool ray_valid(const OpticalCamera& c,double x,double y){
    double r=std::hypot((x-c.cx)/c.fx,(y-c.cy)/c.fy),t=r;
    for(int i=0;i<30;++i){double t2=t*t;double d=1+t2*(3*c.k[0]+t2*(5*c.k[1]+t2*(7*c.k[2]+t2*9*c.k[3])));if(!std::isfinite(d)||d<=0)return false;
    t-=(t*(1+t2*(c.k[0]+t2*(c.k[1]+t2*(c.k[2]+t2*c.k[3]))))-r)/d;}
    double t2=t*t,res=t*(1+t2*(c.k[0]+t2*(c.k[1]+t2*(c.k[2]+t2*c.k[3]))))-r;
    return std::isfinite(t)&&std::abs(res)<=1e-8&&t>=0&&t<3.141592653589793;
}
}
phase5::Value calibration_json(const std::filesystem::path&p){
    need(!std::filesystem::is_symlink(p)&&std::filesystem::is_regular_file(p)&&std::filesystem::file_size(p)<=8*1024*1024,"Calibration file missing, symlink or too large");
    std::ifstream f(p,std::ios::binary);need(bool(f),"Calibration read failed");std::string s{std::istreambuf_iterator<char>(f),{}};return phase5::Json(s).parse();
}
ReplayRig load_replay_rig(const std::filesystem::path& dataset,const std::filesystem::path& factory){
    const auto device=unique_active_device(dataset);
    try{
        // The factory file is optional: without it cameras are bound by their XRService name.
        const bool have_factory=!factory.empty()&&std::filesystem::exists(factory);
        ReplayRig rig{have_factory?calibration_json(factory):phase5::Json(std::string(R"({"cameras":[]})")).parse(),
            calibration_json(dataset/"sensorRigCalibration.json"),calibration_json(device)};
        std::set<std::string> serials;for(const auto& c:arr(field(rig.factory,"cameras")))need(serials.insert(str(field(c,"serial"))).second,"Ambiguous factory serial");
        return rig;
    }catch(const std::exception& error){
        throw std::runtime_error(std::string(error.what())+"; "+active_calibration_diagnostic(dataset));
    }
}
IdentityView ReplayRig::bind(const std::string& serial,int width,int height)const{
    pico::guard_evidence::Scope guard_evidence_scope(22,"rig");
    // A factory serial maps to an XRService camera name; otherwise the argument is that name.
    const phase5::Value* f=nullptr;
    for(const auto& cam:arr(field(factory,"cameras")))if(str(field(cam,"serial"))==serial){need(!f,"Ambiguous factory serial");f=&cam;}
    const std::string name=f?str(field(*f,"sourceCamera")):serial;
    const auto& c=unique(field(active,"cameras"),"sourceCamera",name);
    need(str(field(c,"cameraModel"))=="kb","Unsupported camera model");
    auto rot=c.object.find("preRotateClockwiseByDeg");need(rot==c.object.end()||num(rot->second)==0,"Unsupported camera rotation");
    need(num(field(c,"width"))==width&&num(field(c,"height"))==height,"Decoded dimensions do not match calibration");
    IdentityView out;auto& k=out.camera;const auto& i=field(c,"intrinsics");
    k.fx=num(field(i,"fx"));k.fy=num(field(i,"fy"));k.cx=num(field(i,"cx"));k.cy=num(field(i,"cy"));k.width=width;k.height=height;
    need(k.fx>0&&k.fy>0,"Invalid focal length");for(int j=0;j<4;++j)k.k[j]=num(field(i,("k"+std::to_string(j+1)).c_str()));
    // Runtime principal points sit 0.5 px from the factory ones (pixel-centre convention of every Frame).
    double dx=.5,dy=.5;
    if(f){const auto origin=unique(field(*f,"intrinsics"),"cameraModel","kb");
        dx=k.cx-num(field(origin,"cx"));dy=k.cy-num(field(origin,"cy"));}
    need((std::abs(dx)<=1e-4&&std::abs(dy)<=1e-4)||(std::abs(dx-.5)<=1e-4&&std::abs(dy-.5)<=1e-4),"Factory/runtime pixel origin mismatch");out.pixel_offset={dx,dx};
    const auto& e=field(c,"extrinsics");need(str(field(e,"__units"))=="mm","Unknown camera extrinsic units");
    out.head_from_camera=mul(mul(inv(transform(field(device,"head"),1)),transform(field(field(device,"cv"),"cad_from_cal"),1)),transform(e,.001));return out;
}
LedModel load_replay_leds(const Value& root,const std::string& serial){
    pico::guard_evidence::Scope guard_evidence_scope(22,"factory-model");
    auto bytes=base64(str(field(unique(field(root,"calibration_data"),"sn",serial),"calibration")));
    auto word=[&](size_t p){return uint32_t(bytes[p])|(uint32_t(bytes[p+1])<<8)|(uint32_t(bytes[p+2])<<16)|(uint32_t(bytes[p+3])<<24);};
    need(bytes.size()==660&&word(204)==1&&word(208)==60,"Unrecognized factory blob layout");
    LedModel m{};for(size_t i=0;i<24;++i){auto bits=word(212+4*i);float f;std::memcpy(&f,&bits,4);need(std::isfinite(f)&&std::abs(f)<=.1,"Invalid LED positions in metres");m[i]=f;}return m;
}
void ReplayOptical::frame(const camera::GrayFrame& f){
    need(api_.detect&&api_.solve,"Replay requires real native detector and solver");
    need(f.packet_timestamp_matched&&!f.source_time_verified&&f.metadata.sensor&&f.metadata.timestamp,"Replay PTS/provenance violation");
    need(std::isfinite(*f.metadata.timestamp)&&*f.metadata.timestamp>0,"Invalid replay timestamp");
    int side=*f.metadata.sensor==pico::devices::upper(0)?0:*f.metadata.sensor==pico::devices::upper(1)?1:-1;need(side>=0,"Unexpected replay sensor");
    rig_.bind(*f.metadata.sensor,1056,1024);need(f.pixels.size()==1056u*1024u,"Replay raster size");
    auto& q=pending_[side];if(q.size()==256){q.pop_front();++counts.unpaired;}q.push_back(f);
    while(!pending_[0].empty()&&!pending_[1].empty()){
        double delta=*pending_[0].front().metadata.timestamp-*pending_[1].front().metadata.timestamp;
        if(std::abs(delta)>.000002){pending_[delta<0?0:1].pop_front();++counts.unpaired;continue;}
        std::array<camera::GrayFrame,2> pair{std::move(pending_[0].front()),std::move(pending_[1].front())};pending_[0].pop_front();pending_[1].pop_front();++counts.pairs;
        ExposureHeads heads;
        try{heads=head_lookup_(pair);}catch(const std::exception&){++counts.rejected["head_history_or_clock"];continue;}
        bool valid_heads=true;for(const auto& head:heads.world_from_head)valid_heads=valid_heads&&identity_rigid_valid(head);
        if(!valid_heads){++counts.rejected["head_history_or_clock"];continue;}
        OpticalView views[2]{};std::array<std::vector<double>,2> pixels;bool enough=true;
        for(size_t v=0;v<2;++v){const auto& a=pair[v];
            if(!a.metadata.exposure||!std::isfinite(*a.metadata.exposure)||*a.metadata.exposure<.000750||*a.metadata.exposure>.000850){enough=false;++counts.rejected["exposure"];break;}
            auto b=rig_.bind(*a.metadata.sensor,1056,1024);int capacity=256,n;std::vector<double> blobs;
            for(;;){blobs.resize(size_t(capacity)*5);n=api_.detect(a.pixels.data(),1024,1056,180,2,250,blobs.data(),capacity);if(n>=0)break;
            need(capacity<1056*1024,"Detector failed");capacity=std::min(capacity*2,1056*1024);}
            need(n<=capacity,"Detector capacity violation");
            for(int j=0;j<n;++j){double x=blobs[size_t(j)*5]+b.pixel_offset[0],y=blobs[size_t(j)*5+1]+b.pixel_offset[1];if(!ray_valid(b.camera,x,y)){++counts.rejected["kb_ray"];continue;}pixels[v].push_back(x);pixels[v].push_back(y);}
            auto world_from_camera=mul(heads.world_from_head[v],b.head_from_camera);
            views[v].camera=b.camera;std::copy(world_from_camera.begin(),world_from_camera.end(),views[v].head_from_camera);views[v].count=pixels[v].size()/2;views[v].pixels=pixels[v].data();enough=enough&&views[v].count>=4;
        }
        if(!enough){++counts.rejected["insufficient_points_or_exposure"];continue;}++counts.candidates;
        OpticalFit fit{};if(api_.solve(model_.data(),views,nullptr,&fit)!=0){++counts.rejected["solve"];continue;}
        if(fit.count<4||fit.count>8){++counts.rejected["inliers"];continue;}++counts.stereo_verified;
        Rigid pose;std::copy(std::begin(fit.pose),std::end(fit.pose),pose.begin());
        if(!identity_rigid_valid(pose)||!std::isfinite(fit.residual)||fit.residual<0||fit.residual>2.5||!std::isfinite(fit.condition)||fit.condition<0||fit.condition>1e7||!std::isfinite(fit.position_std)||fit.position_std<0||fit.position_std>.03||!std::isfinite(fit.rotation_std_deg)||fit.rotation_std_deg<0||fit.rotation_std_deg>5){++counts.rejected["fit_quality"];continue;}
        last_pose=pose;source_times=heads.stamps;++counts.world_poses;
        if(pose_sink)pose_sink(pose,pair,counts.pairs);
    }
}
void ReplayOptical::finish(){for(auto& q:pending_){counts.unpaired+=q.size();q.clear();}}
}

#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_calibration, "calibration");
namespace pico {
// Mean factory LED model of five PICO Swift trackers (each within 0.42 mm of it), metres, 8 LEDs x xyz.
// Used when the PICO factory calibration of a tracker is not available (it lives on a PICO 4 headset).
LedModel default_tracker_leds(){
    return {0.007296,0.016003,-0.009386, -0.004805,0.013566,-0.004663, -0.009507,-0.001489,-0.009746,
            -0.003695,-0.014474,-0.009562, 0.010074,-0.018259,-0.009589, 0.022022,-0.013409,-0.005245,
            0.024698,-0.001131,-0.009281, 0.019216,0.013443,-0.004419};
}
LedModel tracker_leds(const phase5::Value& root,const std::string& serial){
    try{return load_replay_leds(root,serial);}
    catch(const std::exception& e){std::cerr<<"LED model for "<<serial<<": "<<e.what()<<"; using the mean PICO Swift model\n";}
    return default_tracker_leds();
}
LedModel load_tracker_leds(const std::filesystem::path& file,const std::string& serial){
    if(file.empty()||!std::filesystem::exists(file)){
        std::cerr<<"No PICO factory LED calibration file; using the mean PICO Swift model\n";return default_tracker_leds();}
    return tracker_leds(calibration_json(file),serial);
}
}
