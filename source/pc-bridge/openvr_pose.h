#pragma once
#include "bridge.h"
// Include after either openvr.h or openvr_driver.h (which cannot coexist).
namespace bridge {
inline Pose fromVr(const vr::TrackedDevicePose_t& in) {
    Pose p;p.valid=in.bPoseIsValid;p.connected=in.bDeviceIsConnected;
    const auto& m=in.mDeviceToAbsoluteTracking.m;
    p.p={m[0][3],m[1][3],m[2][3]};
    // Stable matrix -> quaternion, including rotations near 180 degrees.
    double trace=m[0][0]+m[1][1]+m[2][2];
    if(trace>0) {double s=std::sqrt(trace+1)*2;p.q={s/4,(m[2][1]-m[1][2])/s,(m[0][2]-m[2][0])/s,(m[1][0]-m[0][1])/s};}
    else {int i=0;if(m[1][1]>m[0][0])i=1;if(m[2][2]>m[i][i])i=2;
        int j=(i+1)%3,k=(i+2)%3;double s=std::sqrt(std::max(0.0,1.0+m[i][i]-m[j][j]-m[k][k]))*2;
        if(s<1e-9){p.valid=false;return p;}
        double q[3]{};q[i]=s/4;q[j]=(m[j][i]+m[i][j])/s;q[k]=(m[k][i]+m[i][k])/s;
        p.q={(m[k][j]-m[j][k])/s,q[0],q[1],q[2]};
    }
    p.v={in.vVelocity.v[0],in.vVelocity.v[1],in.vVelocity.v[2]};
    p.w={in.vAngularVelocity.v[0],in.vAngularVelocity.v[1],in.vAngularVelocity.v[2]};
    if(!validPose(p)){p=Pose{};p.connected=in.bDeviceIsConnected;}else p.q=normalize(p.q);
    return p;
}
}
