#include "/home/steamos/pico-ram-work/native-port/native/mono_solver.hpp"
#include <iostream>

using namespace pico;

const OpticalCamera lower_camera={400,400,320,240,{0,0,0,0},640,480};
int project(const OpticalCamera *c,const double *p,size_t n,double *out) {
    for(size_t i=0;i<n;++i) { if(p[3*i+2]<=0) return 1;
        out[2*i]=c->cx+c->fx*p[3*i]/p[3*i+2]; out[2*i+1]=c->cy+c->fy*p[3*i+1]/p[3*i+2]; }
    return 0;
}
LedModel model() {
    LedModel m={-.041,-.02,.003,.032,-.025,-.006,.048,.014,.009,-.026,.036,-.012,.006,.044,.018,-.046,.008,.022,.018,-.008,.031,.003,.012,-.026};
    for(int i=0;i<8;++i) m[3*i+2]*=4;
    return m;
}

int main() {
    auto m = model();
    double xyz[24], lower_blobs[16];
    for(int i=0;i<8;++i) { xyz[3*i]=m[3*i]; xyz[3*i+1]=m[3*i+1]; xyz[3*i+2]=m[3*i+2]+.5; }
    project(&lower_camera,xyz,8,lower_blobs);
    
    OpticalView view{};
    view.camera = lower_camera;
    view.count = 8;
    view.pixels = lower_blobs;
    view.head_from_camera[0]=view.head_from_camera[5]=view.head_from_camera[10]=view.head_from_camera[15]=1; view.head_from_camera[3]=.25;
    
    NativeOpticalApi api{};
    api.project = project;
    
    OpticalFit fit{};
    bool ok = solve_monocular(api, m, view, fit);
    std::cout << "solve_monocular ok=" << ok << " count=" << fit.count << " residual=" << fit.residual 
              << " z=" << fit.pose[11] << "\n";
    return 0;
}
