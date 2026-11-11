#include "native_test_runtime.hpp"
#include "pose_compare.hpp"
#include "replay_pose_text.hpp"
#include <iostream>
#include <sstream>
using namespace posecmp;
static int checks=0;
static void check(bool ok){++checks;if(!ok)throw std::runtime_error("check "+std::to_string(checks));}
template<class F> void rejects(F f){bool bad=false;try{f();}catch(const std::exception&){bad=true;}check(bad);}
int main(){try{
 auto p=parse(phase5::Json(R"({"source_time_ns":9007199254740993,"position":[1,2,3],"rotation":[[1,0,0],[0,1,0],[0,0,1]]})").parse());check(p.time==9007199254740993LL);check(p.p[1]==2);
 check(parse(phase5::Json(R"({"source_time_ns":1,"wire_position":[3,2,1],"rotation":[[1,0,0],[0,1,0],[0,0,1]]})").parse()).p[0]==3);
 rejects([]{parse(phase5::Json(R"({"source_time_ns":1.1,"position":[1,2,3],"rotation":[]})").parse());});
 rejects([]{phase5::Json(R"({"source_time_ns":1,"source_time_ns":2})").parse();});
 rejects([]{parse(phase5::Json(R"({"source_time_ns":1,"position":[1,2,3],"rotation":[[-1,0,0],[0,1,0],[0,0,1]]})").parse());});
 rejects([]{parse(phase5::Json(R"({"source_time_ns":1,"position":[1e999,2,3],"rotation":[[1,0,0],[0,1,0],[0,0,1]]})").parse());});
 std::vector<Pose>a,b;Transform truth;truth.r={0,-1,0,1,0,0,0,0,1};truth.t={2,-3,4};
 for(int i=0;i<100;++i){Pose x;x.time=10000000000LL+int64_t(i)*10000000;x.p={.2*std::sin(i*.1),.3*std::cos(i*.17),.1*std::sin(i*.31)};a.push_back(x);x.time+=1000000;x.p=add(mul(truth.r,x.p),truth.t);x.r=truth.r;b.push_back(x);}
 auto m=match(a,b,2000000);check(m.a.size()==100);check(m.only_a.empty()&&m.only_b.empty());size_t inliers=0;auto t=robust(m,.03,inliers);check(inliers==100);
 for(size_t i=0;i<m.a.size();++i){check(residual(t,m.a[i],m.b[i])<1e-10);check(angle(t,m.a[i],m.b[i])<1e-5);}
 for(size_t i=0;i<20;++i)m.b[i].p=add(m.b[i].p,{1,2,3});
 t=robust(m,.03,inliers);check(inliers==80);check(norm(add(t.t,truth.t,-1))<1e-10);check(residual(t,m.a[0],m.b[0])>3);
 auto dropped=b;dropped.erase(dropped.begin()+5);Pose extra=b.back();extra.time+=20000000;dropped.push_back(extra);auto gaps=match(a,dropped,2000000);check(gaps.a.size()==99);check(gaps.only_a.size()==1&&gaps.only_b.size()==1);check(gaps.only_a[0]==a[5].time);
 check(match(a,b,999999).a.empty());check(match(a,b,1000000).a.size()==100);rejects([&]{match(a,b,-1);});
 auto ambiguous=match({a[0],a[1]},{b[0]},20000000);check(ambiguous.a.size()==1&&ambiguous.only_a.size()==1);
 Matches deg;for(int i=0;i<10;++i){Pose x;x.p={double(i),0,0};deg.a.push_back(x);deg.b.push_back(x);}rejects([&]{robust(deg,.03,inliers);});rejects([&]{robust(Matches{},.03,inliers);});
 check(percentile({0,1,2,3},.5)==1.5);check(std::abs(percentile({0,1,2,3},.9)-2.7)<1e-12);
 // Arbitrary 3-axis rotation and deterministic small noise.
 double w=.7,x=.2,y=-.3,z=.6,l=std::sqrt(w*w+x*x+y*y+z*z);w/=l;x/=l;y/=l;z/=l;
 truth.r={1-2*(y*y+z*z),2*(x*y-z*w),2*(x*z+y*w),2*(x*y+z*w),1-2*(x*x+z*z),2*(y*z-x*w),2*(x*z-y*w),2*(y*z+x*w),1-2*(x*x+y*y)};
 m=match(a,b,2000000);for(size_t i=0;i<m.b.size();++i){m.b[i].p=add(mul(truth.r,m.a[i].p),truth.t);m.b[i].r=truth.r;m.b[i].p[0]+=.0001*std::sin(double(i));}
 t=robust(m,.03,inliers);check(inliers==100);for(size_t i=0;i<m.a.size();++i){check(residual(t,m.a[i],m.b[i])<.0002);check(angle(t,m.a[i],m.b[i])<.02);}
 // Known rotational error remains visible after position-only alignment.
 m.b[0].r=eye();check(angle(t,m.a[0],m.b[0])>30);
 // Noncommuting rotations catch left/right, inverse and storage mistakes that
 // the old identity-native-orientation fixtures could not distinguish.
 M rx={1,0,0,0,0,-1,0,1,0},ry={0,0,1,0,1,0,-1,0,0};
 Transform alignment;alignment.r=truth.r;alignment.t=truth.t;
 Matches convention;for(size_t i=0;i<a.size();++i){Pose n=a[i],q;n.r=i%2?rx:ry;q.p=add(mul(alignment.r,n.p),alignment.t);q.r=product(alignment.r,n.r);convention.a.push_back(n);convention.b.push_back(q);}
 check(angle(alignment,convention.a[0],convention.b[0])<1e-5);
 check(right_angle(alignment,convention.a[0],convention.b[0])>10);
 Pose inverse_a=convention.a[0],inverse_b=convention.b[0];inverse_a.r=transpose(inverse_a.r);inverse_b.r=transpose(inverse_b.r);
 check(right_angle(alignment,inverse_a,inverse_b)<1e-5);check(angle(alignment,inverse_a,inverse_b)>10);
 for(auto& q:convention.b)q.r=product(q.r,rx);
 // A position outlier must not bias the body-frame offset estimate.
 convention.b[0].p=add(convention.b[0].p,{1,2,3});convention.b[0].r=eye();
 size_t offset_samples=0;auto offset_r=body_offset(convention,alignment,.03,offset_samples);
 check(offset_samples==99);check(rotation_angle(offset_r,rx)<1e-5);
 for(size_t i=1;i<convention.a.size();++i)check(rotation_angle(product(product(alignment.r,convention.a[i].r),offset_r),convention.b[i].r)<1e-5);
 // Reproduce the actual FRAME mismatch: head rotates around the tracker
 // origin, so positions match exactly while rotations disagree systematically.
 std::vector<Pose> head_relative,head_history,standing;
 for(int i=0;i<100;++i){Pose n=a[size_t(i)],h,q;n.fixed_head=true;n.r=i%2?rx:ry;h.time=n.time;
  double theta=.1+.002*i,c=std::cos(theta),s=std::sin(theta);h.r={c,-s,0,s,c,0,0,0,1};h.p=add(n.p,mul(h.r,n.p),-1);
  q=n;q.fixed_head=false;q.r=product(h.r,n.r);head_relative.push_back(n);head_history.push_back(h);standing.push_back(q);}
 auto frame_matches=match(head_relative,standing,0);auto frame_alignment=robust(frame_matches,.03,inliers);
 check(residual(frame_alignment,frame_matches.a[0],frame_matches.b[0])<1e-10);
 check(angle(frame_alignment,frame_matches.a[0],frame_matches.b[0])>5);
 // CLI integration inputs: all values are invented, no device data.
 auto write_poses=[](const char* path,const std::vector<Pose>& rows,bool published){std::ofstream out(path);out<<std::setprecision(17);check(bool(out));
  for(const auto& row:rows){out<<"{\"source_time_ns\":"<<row.time;if(published)out<<",\"published_optical_pose\":true";
  if(row.fixed_head)out<<",\"world_reference\":\"fixed_replay_HEAD\"";
   out<<",\"position\":["<<row.p[0]<<','<<row.p[1]<<','<<row.p[2]<<"],\"rotation\":[";
   for(int i=0;i<3;++i){if(i)out<<',';
   out<<'['<<row.r[3*i]<<','<<row.r[3*i+1]<<','<<row.r[3*i+2]<<']';}out<<"]}\n";}
  out.close();check(bool(out));};
 write_poses("build/pose_compare_moving_head_native.synthetic.jsonl",head_relative,false);
 write_poses("build/pose_compare_moving_head_history.synthetic.jsonl",head_history,false);
 write_poses("build/pose_compare_moving_head_world.synthetic.jsonl",standing,true);
 auto missing_head=head_history;missing_head.erase(missing_head.begin());auto uncorrected=head_relative;
 rejects([&]{apply_heads(uncorrected,missing_head);});
 apply_heads(head_relative,head_history);frame_matches=match(head_relative,standing,0);frame_alignment=robust(frame_matches,.03,inliers);
 for(size_t i=0;i<frame_matches.a.size();++i){check(residual(frame_alignment,frame_matches.a[i],frame_matches.b[i])<1e-10);check(angle(frame_alignment,frame_matches.a[i],frame_matches.b[i])<1e-5);}
 rejects([&]{apply_heads(head_relative,head_history);});
 // Exercise the exact producer text writer and file readers, including irrelevant
 // optical log lines and published=false. All files contain invented data only.
 std::ofstream native("build/pose_compare_native.synthetic.jsonl"),python("build/pose_compare_python.synthetic.jsonl");
 check(bool(native)&&bool(python));python<<"{\"unrelated\":true}\n{\"published_optical_pose\":false}\n";
 for(size_t i=0;i<a.size();++i){pico::Rigid rigid{};rigid[15]=1;for(int k=0;k<3;++k){rigid[4*k+3]=a[i].p[k];for(int j=0;j<3;++j)rigid[4*k+j]=a[i].r[3*k+j];}
 std::array<pico::camera::GrayFrame,2> pair;pair[0].metadata.timestamp=double(a[i].time)/1e9;pair[1].metadata.timestamp=*pair[0].metadata.timestamp+.000001;pair[0].metadata.camera_id=4;
 std::ostringstream line;pico::write_replay_pose(line,rigid,pair,1,i+1,i+1);auto v=phase5::Json(line.str()).parse();check(phase5::number(v,"stereo_id")==int64_t(i+1));check(parse(v).time==a[i].time);native<<line.str();
 std::string logged=line.str();logged.insert(1,"\"published_optical_pose\":true,");python<<logged;}
 native.close();python.close();check(bool(native)&&bool(python));
 auto read_a=read("build/pose_compare_native.synthetic.jsonl",false),read_b=read("build/pose_compare_python.synthetic.jsonl",true);check(read_a.size()==100&&read_b.size()==100);check(match(read_a,read_b,0).a.size()==100);check(read_a[0].fixed_head);
 {std::ofstream bad("build/pose_compare_bad.synthetic.jsonl");bad<<"{\"published_optical_pose\":true,\"source_time_ns\":1}\n";}
 rejects([]{read("build/pose_compare_bad.synthetic.jsonl",true);});
 {std::ofstream duplicate("build/pose_compare_duplicate.synthetic.jsonl");std::ifstream source("build/pose_compare_native.synthetic.jsonl");std::string line;std::getline(source,line);duplicate<<line<<'\n'<<line<<'\n';}
 rejects([]{read("build/pose_compare_duplicate.synthetic.jsonl",false);});
 std::cout<<checks<<" pose comparison checks passed\n";return 0;
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
