#pragma once
#include "pico_fusion.h"
#include <fstream>
#include <sstream>
#include <string>
namespace pico_fusion {
// chip + PICO magCoeff's 9 soft / 3 offset / flag / 2 opaque scalars,
// followed by an independently measured 3x3 sensor-to-body rotation.
inline bool LoadMagCalibration(const std::string &path,uint32_t chip,MagCalibration &out) {
    std::ifstream f(path);std::string line;
    while(std::getline(f,line)) {
        std::istringstream in(line); uint32_t id; MagCalibration c;
        if(!(in>>std::hex>>id>>std::dec)||id!=chip) continue;
        bool ok=true; for(double &v:c.soft) ok=ok && bool(in>>v) && std::isfinite(v) && std::fabs(v)<10;
        ok=ok && bool(in>>c.offset.x>>c.offset.y>>c.offset.z>>c.preCoeff>>c.scalar[0]>>c.scalar[1]);
        for(double &v:c.mount) ok=ok && bool(in>>v) && std::isfinite(v);
        if(!ok||!std::isfinite(Norm(c.offset))||Norm(c.offset)>1000||!std::isfinite(c.scalar[0])||!std::isfinite(c.scalar[1])) return false;
        const auto &m=c.mount;
        for(int r=0;r<3;++r) for(int s=0;s<3;++s) {
            double d=0;for(int k=0;k<3;++k)d+=m[r*3+k]*m[s*3+k];
            if(std::fabs(d-(r==s?1:0))>.01) return false;
        }
        double det=m[0]*(m[4]*m[8]-m[5]*m[7])-m[1]*(m[3]*m[8]-m[5]*m[6])+m[2]*(m[3]*m[7]-m[4]*m[6]);
        const auto &a=c.soft;
        double sd=a[0]*(a[4]*a[8]-a[5]*a[7])-a[1]*(a[3]*a[8]-a[5]*a[6])+a[2]*(a[3]*a[7]-a[4]*a[6]);
        if(det<.99||sd<.01||sd>100) return false;
        c.valid=true;out=c;return true;
    }
    return false;
}
}
