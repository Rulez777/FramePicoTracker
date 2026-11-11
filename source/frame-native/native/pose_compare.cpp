#include "native_test_runtime.hpp"
#include "pose_compare.hpp"
#include <iostream>
#include <iomanip>
using namespace posecmp;
static void distribution(const char* name,const std::vector<int64_t>& times,int64_t origin,int64_t width){
 std::cout<<name<<"_count="<<times.size()<<'\n';if(times.empty())return;
 std::cout<<name<<"_first_ns="<<times.front()<<" last_ns="<<times.back()<<'\n';
 std::map<int64_t,size_t> bins;for(auto t:times)++bins[(t-origin)/width];
 for(auto b:bins)std::cout<<name<<"_bin_start_seconds="<<double(b.first*width)/1e9<<" count="<<b.second<<'\n';
}
int main(int argc,char** argv){try{
 if(argc<3)throw std::runtime_error("Usage: pose_compare native.jsonl optical-live.jsonl [--tolerance-ms 2] [--inlier-m 0.03] [--bin-seconds 10] [--native-time-offset-ns 0] [--rotation-diagnostics] [--native-head-poses world_from_head.jsonl]");
 double tolerance=2,threshold=.03,bin=10;
 int64_t offset=0;
 bool diagnostics=false;std::string heads_path;
 for(int i=3;i<argc;){std::string key=argv[i++];if(key=="--rotation-diagnostics"){diagnostics=true;continue;}if(i==argc)throw std::runtime_error("missing option value");
 std::string s=argv[i++];size_t end=0;
 if(key=="--native-head-poses"){heads_path=s;continue;}
 if(key=="--native-time-offset-ns"){offset=std::stoll(s,&end);if(end!=s.size())throw std::runtime_error("invalid offset");
 continue;}
 double v=std::stod(s,&end);if(end!=s.size()||!std::isfinite(v)||v<0)throw std::runtime_error("invalid option value");
 if(key=="--tolerance-ms")tolerance=v;else if(key=="--inlier-m")threshold=v;else if(key=="--bin-seconds")bin=v;else throw std::runtime_error("unknown option "+key);}
 if(tolerance>60000||bin<.000001||bin>1e6||threshold<=0)throw std::runtime_error("option out of range");
 auto a=read(argv[1],false),b=read(argv[2],true);
 for(auto& p:a){if((offset>0&&p.time>INT64_MAX-offset)||(offset<0&&offset < -p.time))throw std::runtime_error("offset timestamp out of range");
 p.time+=offset;}
 if(!heads_path.empty()){apply_heads(a,read(heads_path,false));std::cout<<"native_head_composition=exact_exposure_world_from_head\n";}
 else if(std::any_of(a.begin(),a.end(),[](const Pose& p){return p.fixed_head;}))std::cout<<"frame_warning=fixed_replay_HEAD_vs_published_world_constant_alignment_cannot_remove_head_motion\n";
 auto m=match(a,b,int64_t(std::llround(tolerance*1e6)));
 std::cout<<std::setprecision(12)<<"native_count="<<a.size()<<" python_count="<<b.size()<<" matched_count="<<m.a.size()<<" tolerance_ms="<<tolerance<<'\n';
 int64_t origin=a.empty()?(b.empty()?0:b.front().time):(b.empty()?a.front().time:std::min(a.front().time,b.front().time));
 std::cout<<"native_time_offset_ns="<<offset<<" distribution_origin_ns="<<origin<<" bin_seconds="<<bin<<'\n';distribution("native_only",m.only_a,origin,int64_t(std::llround(bin*1e9)));distribution("python_only",m.only_b,origin,int64_t(std::llround(bin*1e9)));
 size_t inliers=0;auto t=robust(m,threshold,inliers);std::cout<<"alignment_inliers="<<inliers<<" alignment_threshold_m="<<threshold<<" transform_native_to_python_rotation=";for(double x:t.r)std::cout<<x<<' ';
 std::cout<<" translation=";for(double x:t.t)std::cout<<x<<' ';
 std::cout<<'\n';
 std::vector<double> pos,rot,dt;for(size_t i=0;i<m.a.size();++i){pos.push_back(residual(t,m.a[i],m.b[i]));rot.push_back(angle(t,m.a[i],m.b[i]));dt.push_back(double(std::abs(m.a[i].time-m.b[i].time))/1e6);}
 auto report=[](const char* name,const std::vector<double>& values){std::cout<<name<<" p50="<<percentile(values,.5)<<" p90="<<percentile(values,.9)<<" p99="<<percentile(values,.99)<<'\n';};
 report("position_m",pos);report("rotation_deg",rot);report("time_delta_ms",dt);
 if(diagnostics){
  size_t samples=0;auto c=body_offset(m,t,threshold,samples);
  std::cout<<"body_offset_fit_samples="<<samples<<" body_offset_model=R_python=A*R_native*C body_offset_rotation=";for(double x:c)std::cout<<x<<' ';
  std::cout<<" angle_deg="<<rotation_angle(c,eye())<<'\n';
  std::vector<double> right,corrected,left_in,right_in,corrected_in;
  for(size_t i=0;i<m.a.size();++i){right.push_back(right_angle(t,m.a[i],m.b[i]));corrected.push_back(rotation_angle(product(product(t.r,m.a[i].r),c),m.b[i].r));
   if(pos[i]<=threshold){left_in.push_back(rot[i]);right_in.push_back(right.back());corrected_in.push_back(corrected.back());}}
  report("rotation_left_deg",rot);report("rotation_right_deg",right);report("rotation_left_body_offset_deg",corrected);
  report("rotation_left_position_inliers_deg",left_in);report("rotation_right_position_inliers_deg",right_in);report("rotation_left_body_offset_position_inliers_deg",corrected_in);
  std::cout<<"rotation_diagnostics_scope=descriptive_fit_same_samples_not_validation_or_calibration\n";
 }
 return 0;
 }catch(const std::exception& e){std::cerr<<"pose_compare: "<<e.what()<<'\n';return 1;}}
