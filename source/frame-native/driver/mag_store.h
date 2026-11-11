#pragma once
#include "pico_fusion.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
namespace pico_fusion {
inline bool ValidMagFit(const MagFit &f) {
    if(!f.ready||!std::isfinite(f.radius)||f.radius<30||f.radius>65||!std::isfinite(f.rms)||f.rms<0||f.rms>=3||!std::isfinite(f.coverage)||f.coverage<62.5||f.coverage>100||f.samples<240||!std::isfinite(Norm(f.calibration.offset))||Norm(f.calibration.offset)>100)return false;
    if(f.model<0||f.model>2||!std::isfinite(f.minEigen)||f.minEigen<0||!std::isfinite(f.halfDifference)||f.halfDifference<0||f.halfDifference>=6||!std::isfinite(f.envelopeDifference)||f.envelopeDifference<0)return false;
    const auto &s=f.calibration.soft;
    for(int r=0;r<3;++r)for(int c=0;c<3;++c)if(!std::isfinite(s[r*3+c])||std::fabs(s[r*3+c]-s[c*3+r])>1e-6)return false;
    double det=s[0]*(s[4]*s[8]-s[5]*s[7])-s[1]*(s[3]*s[8]-s[5]*s[6])+s[2]*(s[3]*s[7]-s[4]*s[6]);
    return s[0]>0&&s[0]*s[4]-s[1]*s[3]>0&&det>.1&&det<10;
}
inline std::string MagFile(const std::string &base,uint32_t chip){char b[32];std::snprintf(b,sizeof(b),".v1.%08X",chip);return base+b;}
inline bool SaveMagFit(const std::string &base,uint32_t chip,const MagFit &f) {
    if(!ValidMagFit(f)||f.minEigen<.08)return false;
    std::string path=MagFile(base,chip),tmp=path+".tmp";
    {std::ofstream o(tmp,std::ios::trunc);o.precision(17);o<<"PICO_MAG 2 "<<chip<<' '<<f.radius<<' '<<f.rms<<' '<<f.coverage<<' '<<f.samples<<' '<<f.calibration.offset.x<<' '<<f.calibration.offset.y<<' '<<f.calibration.offset.z;
    for(double v:f.calibration.soft)o<<' '<<v;
    o<<' '<<f.model<<' '<<f.minEigen<<' '<<f.halfDifference<<' '<<f.envelopeDifference<<'\n';o.flush();if(!o)return false;}
#ifdef _WIN32
    return MoveFileExA(tmp.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
#else
    return std::rename(tmp.c_str(),path.c_str())==0;
#endif
}
inline bool LoadMagFit(const std::string &base,uint32_t chip,MagFit &out){
    std::ifstream in(MagFile(base,chip));std::string magic,extra;unsigned version,id;MagFit f;
    if(!(in>>magic>>version>>id>>f.radius>>f.rms>>f.coverage>>f.samples>>f.calibration.offset.x>>f.calibration.offset.y>>f.calibration.offset.z)||magic!="PICO_MAG"||(version!=1&&version!=2)||id!=chip)return false;
    for(double &v:f.calibration.soft)if(!(in>>v))return false;
    if(version==2){if(!(in>>f.model>>f.minEigen>>f.halfDifference>>f.envelopeDifference)||f.minEigen<.08)return false;}
    else {if(f.coverage!=100||f.rms>=1.5)return false;
    f.model=2;} // legacy has no covariance field
    if(in>>extra)return false;
    f.ready=true;f.calibration.radius=f.radius;if(!ValidMagFit(f))return false;
    out=f;return true;
}
}
