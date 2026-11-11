#pragma once
#include "optical_prior.hpp"
#include <stdexcept>
namespace pico {
inline int64_t replay_imu_available(int64_t recorded,int64_t sensor,std::optional<int64_t> latency) {
    if(!latency) return recorded;
    if(sensor<0 || *latency<0 || sensor>INT64_MAX-*latency) throw std::invalid_argument("IMU availability overflow");
    return sensor+*latency;
}
inline Rigid replay_rotation(const std::array<double,7>& pose) {
    auto r=identity_rigid();
    double x=pose[3],y=pose[4],z=pose[5],w=pose[6];
    const double n=std::sqrt(x*x+y*y+z*z+w*w);
    if(!std::isfinite(n) || n<1e-12) throw std::invalid_argument("Invalid metric quaternion");
    x/=n;y/=n;z/=n;w/=n;
    r[0]=1-2*(y*y+z*z);r[1]=2*(x*y-z*w);r[2]=2*(x*z+y*w);
    r[4]=2*(x*y+z*w);r[5]=1-2*(x*x+z*z);r[6]=2*(y*z-x*w);
    r[8]=2*(x*z-y*w);r[9]=2*(y*z+x*w);r[10]=1-2*(x*x+y*y);
    return r;
}
inline double replay_gyro_disagreement(const Rigid& previous,const Rigid& current,const Rigid& integral) {
    auto predicted=identity_rigid();
    for(int i=0;i<3;++i) for(int j=0;j<3;++j) {
        predicted[4*i+j]=0;
        for(int k=0;k<3;++k) predicted[4*i+j]+=previous[4*i+k]*integral[4*k+j];
    }
    return OpticalPrior::angle(predicted,current); // angle(prev^-1 current, integral)
}
}
