#include "native_test_runtime.hpp"
#include "mono_solver.hpp"
#include <iostream>
#include <stdexcept>
namespace pico { void log_optical_drop(const char*) {} }
using namespace pico;
int projection(const OpticalCamera *c,const double *p,size_t n,double *uv) {
 for(size_t i=0;i<n;++i) {if(p[3*i+2]<=0) return 1;
 uv[2*i]=c->cx+c->fx*p[3*i]/p[3*i+2];uv[2*i+1]=c->cy+c->fy*p[3*i+1]/p[3*i+2];}return 0;
}
int main() { try {
 LedModel model={-.041,-.02,.003,.032,-.025,-.006,.048,.014,.009,-.026,.036,-.012,.006,.044,.018,-.046,.008,.022,.018,-.008,.031,.003,.012,-.026};
 for(int i=0;i<8;++i) model[3*i+2]*=4;
 OpticalView view{};view.camera={700,710,528,512,{0,0,0,0},1056,1024};
 view.head_from_camera[0]=view.head_from_camera[5]=view.head_from_camera[10]=view.head_from_camera[15]=1;view.head_from_camera[3]=.2;
 double xyz[24],uv[16];for(int i=0;i<8;++i) {xyz[3*i]=model[3*i]+.04;xyz[3*i+1]=model[3*i+1]-.02;xyz[3*i+2]=model[3*i+2]+.7;}
 projection(&view.camera,xyz,8,uv);view.count=8;view.pixels=uv;NativeOpticalApi api{};api.project=projection;OpticalFit fit{};
 if(!solve_monocular(api,model,view,fit) || std::abs(fit.pose[3]-.24)>.002 || std::abs(fit.pose[7]+.02)>.002 || std::abs(fit.pose[11]-.7)>.003 || fit.residual>.1) throw std::runtime_error("synthetic monocular pose");
 // Seeded (prior = WORLD<-tracker): exact prior, prior rotation with a stale position, and a far rotation.
 auto prior_at=[](double deg,double x,double y,double z){const double a=deg*std::acos(-1.)/180;
  std::array<double,16> p{std::cos(a),-std::sin(a),0,x,std::sin(a),std::cos(a),0,y,0,0,1,z,0,0,0,1};return p;};
 auto truth=prior_at(0,.24,-.02,.7),stale=prior_at(3,.40,.10,.9),far_prior=prior_at(30,.24,-.02,.7);
 if(!solve_monocular(api,model,view,fit,truth.data()) || !fit.continuation || std::abs(fit.pose[3]-.24)>.002 || std::abs(fit.pose[11]-.7)>.003) throw std::runtime_error("seeded exact prior");
 if(!solve_monocular(api,model,view,fit,stale.data()) || !fit.continuation || std::abs(fit.pose[3]-.24)>.002 || std::abs(fit.pose[11]-.7)>.003) throw std::runtime_error("seeded stale-position prior");
 if(solve_monocular(api,model,view,fit,far_prior.data()) && fit.continuation) throw std::runtime_error("seeded far-rotation prior admitted");
 // Four of eight LEDs (front half): a blind solve is ambiguous, the prior selects the true branch.
 double four[8];for(int i=0;i<8;++i) four[i]=uv[i];
 OpticalView part=view;part.count=4;part.pixels=four;
 if(!solve_monocular(api,model,part,fit,truth.data()) || std::abs(fit.pose[3]-.24)>.005 || std::abs(fit.pose[11]-.7)>.02) throw std::runtime_error("seeded four-LED view");
 for(int i=0;i<8;++i) uv[2*i]=1056-uv[2*i];
 if(solve_monocular(api,model,view,fit)) throw std::runtime_error("mirrored constellation admitted");
 if(solve_monocular(api,model,view,fit,truth.data())) throw std::runtime_error("mirrored constellation admitted with a prior");
 std::cout<<"mono solver tests passed\n";
} catch(const std::exception &e) { std::cerr<<e.what()<<"\n"; return 1; }
}
