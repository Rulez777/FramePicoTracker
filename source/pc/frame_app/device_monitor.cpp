/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
 */
#include <openvr.h>
#include <chrono>
#include <cstdio>
#include <thread>
#include <cstdlib>

// Read-only concurrent tracking evidence; this does not inject poses.
int main(int argc,char** argv){
    vr::EVRInitError error=vr::VRInitError_None;
    auto* system=vr::VR_Init(&error,vr::VRApplication_Background);
    if(error!=vr::VRInitError_None){std::fprintf(stderr,"%s\n",vr::VR_GetVRInitErrorAsEnglishDescription(error));return 1;}
    int iterations=argc>1?std::atoi(argv[1]):1;
    for(int n=0;n<iterations;n++){
        vr::TrackedDevicePose_t poses[vr::k_unMaxTrackedDeviceCount]{};
        system->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding,0,poses,vr::k_unMaxTrackedDeviceCount);
        auto ms=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
        for(unsigned i=0;i<vr::k_unMaxTrackedDeviceCount;i++){
            auto kind=system->GetTrackedDeviceClass(i);
            if(kind!=vr::TrackedDeviceClass_HMD&&kind!=vr::TrackedDeviceClass_Controller)continue;
            std::printf("%lld,%u,%d,%d,%d,%d,%d\n",(long long)ms,i,(int)kind,(int)system->GetControllerRoleForTrackedDeviceIndex(i),poses[i].bDeviceIsConnected,poses[i].bPoseIsValid,(int)poses[i].eTrackingResult);
        }
        std::fflush(stdout);std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    vr::VR_Shutdown();
}
