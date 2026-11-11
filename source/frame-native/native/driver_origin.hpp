#pragma once
#include "head_history.hpp"
#include "pose_publication.hpp"
namespace pico {
// Worker estimates remain Standing; only the driver publication is converted.
inline OpticalPoseFrame driver_pose(OpticalPoseFrame frame,const Rigid& raw) {
    auto rotate=[&](std::array<double,3> v){
        std::array<double,3> out{};
        for(int i=0;i<3;++i)for(int j=0;j<3;++j)out[i]+=raw[4*i+j]*v[j];
        return out;
    };
    frame.position=rotate(frame.position);
    for(int i=0;i<3;++i)frame.position[i]+=raw[4*i+3];
    frame.velocity=rotate(frame.velocity);frame.omega=rotate(frame.omega);
    // Stable largest-diagonal matrix -> quaternion (xyzw).
    std::array<double,4> q{};
    const double trace=raw[0]+raw[5]+raw[10];
    if(trace>0){
        const double s=2*std::sqrt(trace+1);q={(raw[9]-raw[6])/s,(raw[2]-raw[8])/s,(raw[4]-raw[1])/s,s/4};
    }else{
        int i=0;if(raw[5]>raw[0])i=1;
        if(raw[10]>raw[4*i+i])i=2;
        const int j=(i+1)%3,k=(i+2)%3;
        const double s=2*std::sqrt(1+raw[4*i+i]-raw[4*j+j]-raw[4*k+k]);
        q[i]=s/4;q[j]=(raw[4*i+j]+raw[4*j+i])/s;q[k]=(raw[4*i+k]+raw[4*k+i])/s;
        q[3]=(raw[4*k+j]-raw[4*j+k])/s;
    }
    const auto b=frame.quaternion;
    frame.quaternion={q[3]*b[0]+q[0]*b[3]+q[1]*b[2]-q[2]*b[1],
        q[3]*b[1]-q[0]*b[2]+q[1]*b[3]+q[2]*b[0],
        q[3]*b[2]+q[0]*b[1]-q[1]*b[0]+q[2]*b[3],
        q[3]*b[3]-q[0]*b[0]-q[1]*b[1]-q[2]*b[2]};
    return frame;
}
}
