#pragma once
#include "phase5/json.hpp"
#include <array>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <random>
#include <numeric>
namespace posecmp {
using V=std::array<double,3>;using M=std::array<double,9>;
inline M eye(){return {1,0,0,0,1,0,0,0,1};}
inline M transpose(M a){M b{};for(int i=0;i<3;++i)for(int j=0;j<3;++j)b[3*i+j]=a[3*j+i];
return b;}
inline M product(M a,M b){M c{};for(int i=0;i<3;++i)for(int j=0;j<3;++j)for(int k=0;k<3;++k)c[3*i+j]+=a[3*i+k]*b[3*k+j];
return c;}
inline double rotation_angle(M a,M b){double trace=0;for(int i=0;i<9;++i)trace+=a[i]*b[i];
return std::acos(std::clamp((trace-1)/2,-1.,1.))*180/3.14159265358979323846;}
inline V add(V a,V b,double s=1){for(int i=0;i<3;++i)a[i]+=s*b[i];
return a;}
inline V mul(M r,V a){V b{};for(int i=0;i<3;++i)for(int j=0;j<3;++j)b[i]+=r[3*i+j]*a[j];
return b;}
inline double norm(V a){return std::sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);}
struct Pose{int64_t time=0;V p{};M r=eye();bool fixed_head=false;};
inline double scalar(const phase5::Value& v){if(v.type!='i')throw std::runtime_error("expected number");
double x=std::stod(v.text);if(!std::isfinite(x))throw std::runtime_error("nonfinite number");
    return x;}
inline Pose parse(const phase5::Value& v){
 Pose p;p.time=phase5::number(v,"source_time_ns");if(p.time<0)throw std::runtime_error("negative time");
 auto frame=v.object.find("world_reference");p.fixed_head=frame!=v.object.end()&&frame->second.text=="fixed_replay_HEAD";
 auto& a=phase5::field(v,v.object.count("position")?"position":"wire_position");
 if(a.type!='['||a.array.size()!=3)throw std::runtime_error("position shape");
 for(int i=0;i<3;++i)p.p[i]=scalar(a.array[i]);
 auto& r=phase5::field(v,"rotation");if(r.type!='['||r.array.size()!=3)throw std::runtime_error("rotation shape");
 for(int i=0;i<3;++i){if(r.array[i].type!='['||r.array[i].array.size()!=3)throw std::runtime_error("rotation row");
 for(int j=0;j<3;++j)p.r[3*i+j]=scalar(r.array[i].array[j]);}
 for(int i=0;i<3;++i)for(int j=0;j<3;++j){double dot=0;for(int k=0;k<3;++k)dot+=p.r[3*k+i]*p.r[3*k+j];
 if(std::abs(dot-(i==j?1:0))>1e-3)throw std::runtime_error("nonrigid rotation");}
 double det=p.r[0]*(p.r[4]*p.r[8]-p.r[5]*p.r[7])-p.r[1]*(p.r[3]*p.r[8]-p.r[5]*p.r[6])+p.r[2]*(p.r[3]*p.r[7]-p.r[4]*p.r[6]);if(det<.999)throw std::runtime_error("reflected rotation");
    return p;
}
inline std::vector<Pose> read(const std::string& path,bool python){
 std::ifstream f(path);if(!f)throw std::runtime_error("cannot open "+path);
 std::string line;std::vector<Pose> out;size_t n=0;
 while(std::getline(f,line)){++n;if(line.empty())continue;
 if(python&&line.find("published_optical_pose")==std::string::npos)continue;
 try{auto v=phase5::Json(line).parse();if(python&&!phase5::flag(v,"published_optical_pose"))continue;
 out.push_back(parse(v));}catch(const std::exception& e){throw std::runtime_error(path+":"+std::to_string(n)+": "+e.what());}}
 if(!f.eof())throw std::runtime_error("read failure "+path);
 std::sort(out.begin(),out.end(),[](auto& a,auto& b){return a.time<b.time;});for(size_t i=1;i<out.size();++i)if(out[i].time==out[i-1].time)throw std::runtime_error("duplicate source time in "+path);
    return out;
}
struct Matches{std::vector<Pose>a,b;std::vector<int64_t>only_a,only_b;};
// Nearest timestamp edges first, deterministic and one-to-one.
inline Matches match(const std::vector<Pose>& a,const std::vector<Pose>& b,int64_t tolerance){
 if(tolerance<0)throw std::runtime_error("negative tolerance");
 struct Edge{int64_t d;size_t i,j;};std::vector<Edge> edges;
 for(size_t i=0,j=0;i<a.size();++i){while(j<b.size()&&b[j].time<a[i].time&&a[i].time-b[j].time>tolerance)++j;
 for(size_t k=j;k<b.size();++k){auto d=b[k].time>=a[i].time?b[k].time-a[i].time:a[i].time-b[k].time;if(b[k].time>a[i].time&&d>tolerance)break;
 if(d<=tolerance)edges.push_back({d,i,k});
 if(edges.size()>10000000)throw std::runtime_error("too many matching edges");}}
 std::sort(edges.begin(),edges.end(),[](auto& x,auto& y){if(x.d!=y.d)return x.d<y.d;
 if(x.i!=y.i)return x.i<y.i;
 return x.j<y.j;});std::vector<bool>ua(a.size()),ub(b.size());Matches m;
 for(auto e:edges)if(!ua[e.i]&&!ub[e.j]){ua[e.i]=ub[e.j]=true;m.a.push_back(a[e.i]);m.b.push_back(b[e.j]);}
 for(size_t i=0;i<a.size();++i)if(!ua[i])m.only_a.push_back(a[i].time);
 for(size_t i=0;i<b.size();++i)if(!ub[i])m.only_b.push_back(b[i].time);
 return m;
}
struct Transform{M r=eye();V t{};};
// Kabsch proper-rotation least squares via its equivalent Horn quaternion
// eigenproblem. Jacobi finds the largest algebraic eigenvalue, not magnitude.
inline Transform fit(const Matches& m,const std::vector<size_t>& ids){
 if(ids.size()<3)throw std::runtime_error("alignment needs three points");
 V a{},b{};for(auto i:ids){a=add(a,m.a[i].p);b=add(b,m.b[i].p);}for(int k=0;k<3;++k){a[k]/=double(ids.size());b[k]/=double(ids.size());}
 double s[3][3]{};double spread=0,crossmax=0;V first{};
 for(auto i:ids){auto x=add(m.a[i].p,a,-1),y=add(m.b[i].p,b,-1);if(norm(x)>norm(first))first=x;
 spread+=norm(x)*norm(x);for(int j=0;j<3;++j)for(int k=0;k<3;++k)s[j][k]+=x[j]*y[k];}
 for(auto i:ids){auto x=add(m.a[i].p,a,-1);crossmax=std::max(crossmax,norm({first[1]*x[2]-first[2]*x[1],first[2]*x[0]-first[0]*x[2],first[0]*x[1]-first[1]*x[0]}));}
 if(spread<1e-12||crossmax<1e-8*spread/double(ids.size()))throw std::runtime_error("degenerate stationary/collinear alignment");
 double n[4][4]={{s[0][0]+s[1][1]+s[2][2],s[1][2]-s[2][1],s[2][0]-s[0][2],s[0][1]-s[1][0]},
 {s[1][2]-s[2][1],s[0][0]-s[1][1]-s[2][2],s[0][1]+s[1][0],s[0][2]+s[2][0]},
 {s[2][0]-s[0][2],s[0][1]+s[1][0],-s[0][0]+s[1][1]-s[2][2],s[1][2]+s[2][1]},
 {s[0][1]-s[1][0],s[0][2]+s[2][0],s[1][2]+s[2][1],-s[0][0]-s[1][1]+s[2][2]}};
 double v[4][4]{};for(int i=0;i<4;++i)v[i][i]=1;
 for(int it=0;it<100;++it){int p=0,q=1;for(int i=0;i<4;++i)for(int j=i+1;j<4;++j)if(std::abs(n[i][j])>std::abs(n[p][q])){p=i;q=j;}
 if(std::abs(n[p][q])<1e-15)break;
 double theta=.5*std::atan2(2*n[p][q],n[q][q]-n[p][p]),c=std::cos(theta),sn=std::sin(theta);
 double pp=n[p][p],qq=n[q][q],pq=n[p][q];n[p][p]=c*c*pp-2*c*sn*pq+sn*sn*qq;n[q][q]=sn*sn*pp+2*c*sn*pq+c*c*qq;n[p][q]=n[q][p]=0;
 for(int k=0;k<4;++k){if(k!=p&&k!=q){double x=n[k][p],y=n[k][q];n[k][p]=n[p][k]=c*x-sn*y;n[k][q]=n[q][k]=sn*x+c*y;}double x=v[k][p],y=v[k][q];v[k][p]=c*x-sn*y;v[k][q]=sn*x+c*y;}}
 int best=0;for(int i=1;i<4;++i)if(n[i][i]>n[best][best])best=i;
 double w=v[0][best],x=v[1][best],y=v[2][best],z=v[3][best];Transform t;
 t.r={1-2*(y*y+z*z),2*(x*y-z*w),2*(x*z+y*w),2*(x*y+z*w),1-2*(x*x+z*z),2*(y*z-x*w),2*(x*z-y*w),2*(y*z+x*w),1-2*(x*x+y*y)};t.t=add(b,mul(t.r,a),-1);return t;
}
inline double residual(const Transform& t,const Pose& a,const Pose& b){return norm(add(add(mul(t.r,a.p),t.t),b.p,-1));}
inline Transform robust(const Matches& m,double threshold,size_t& inliers){
 if(m.a.size()<3)throw std::runtime_error("too few matches");
 std::mt19937 rng(20261028);std::vector<size_t> best;double best_error=1e300;
 for(int trial=0;trial<512;++trial){std::vector<size_t> ids;for(int k=0;k<3;++k)ids.push_back(rng()%m.a.size());
 try{auto t=fit(m,ids);std::vector<size_t> good;double err=0;for(size_t i=0;i<m.a.size();++i){double d=residual(t,m.a[i],m.b[i]);if(d<=threshold){good.push_back(i);err+=d*d;}}if(good.size()>best.size()||(good.size()==best.size()&&err<best_error)){best=good;best_error=err;}}catch(const std::runtime_error&){} }
 if(best.size()<3||best.size()*2<m.a.size())throw std::runtime_error("no majority rigid alignment consensus");
 auto t=fit(m,best);for(int round=0;round<5;++round){std::vector<size_t> good;for(size_t i=0;i<m.a.size();++i)if(residual(t,m.a[i],m.b[i])<=threshold)good.push_back(i);
 if(good==best)break;
 best=good;t=fit(m,best);}inliers=best.size();return t;
}
// Column vectors: p_py=A*p_native+t, R_py=A*R_native for world<-body.
inline double angle(const Transform& t,const Pose& a,const Pose& b){return rotation_angle(product(t.r,a.r),b.r);}
// Alternative hypothesis for body<-world matrices: R_py=R_native*A^T.
inline double right_angle(const Transform& t,const Pose& a,const Pose& b){return rotation_angle(product(a.r,transpose(t.r)),b.r);}
// Compose the retained world<-head pose with a HEAD<-tracker replay pose.
// Require exact timestamps AFTER the explicit native clock offset. No invented
// head history, nearest-neighbour substitution, or current live HMD query.
inline void apply_heads(std::vector<Pose>& native,const std::vector<Pose>& heads){
 for(auto& p:native){auto h=std::lower_bound(heads.begin(),heads.end(),p.time,[](const Pose& a,int64_t t){return a.time<t;});
  if(h==heads.end()||h->time!=p.time)throw std::runtime_error("missing exact exposure head pose at "+std::to_string(p.time));
  if(!p.fixed_head)throw std::runtime_error("head composition requires fixed_replay_HEAD input");
  p.p=add(mul(h->r,p.p),h->p);p.r=product(h->r,p.r);p.fixed_head=false;
 }
}
// Chordal least-squares proper mean of C_i=(A*R_native)^T*R_py.
// Fit only position inliers, independent of their angular errors. Symmetric
// basis observations give zero centroid and reuse the proper SO(3) Horn fit.
inline M body_offset(const Matches& m,const Transform& t,double threshold,size_t& samples){
 Matches axes;samples=0;
 for(size_t i=0;i<m.a.size();++i)if(residual(t,m.a[i],m.b[i])<=threshold){
  auto c=product(transpose(product(t.r,m.a[i].r)),m.b[i].r);++samples;
  for(int k=0;k<3;++k)for(double sign:{-1.,1.}){Pose a,b;a.p[k]=sign;b.p=mul(c,a.p);axes.a.push_back(a);axes.b.push_back(b);}
 }
 if(!samples)throw std::runtime_error("body offset needs position inliers");
 std::vector<size_t> ids(axes.a.size());std::iota(ids.begin(),ids.end(),size_t(0));return fit(axes,ids).r;
}
inline double percentile(std::vector<double> v,double q){std::sort(v.begin(),v.end());if(v.empty())throw std::runtime_error("empty percentile");
double index=q*double(v.size()-1);size_t i=size_t(index);return v[i]+(v[std::min(i+1,v.size()-1)]-v[i])*(index-double(i));}
}
