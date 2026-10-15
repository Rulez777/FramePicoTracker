/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
 */
#include <openvr.h>
#include <chrono>
#include <cstdio>
#include <thread>
#include <cstdlib>
#include <string>
#include <cctype>

// Read-only HMD/world transform history and controller coexistence watchdog.
// No device registration or pose injection. stdout is one bounded JSON per poll.
int main(int argc,char **argv) {
    vr::EVRInitError error=vr::VRInitError_None;
    auto *system=vr::VR_Init(&error,vr::VRApplication_Background);
    if(error!=vr::VRInitError_None){std::fprintf(stderr,"%s\n",vr::VR_GetVRInitErrorAsEnglishDescription(error));return 1;}
    const int count=argc>1?std::atoi(argv[1]):0;
    for(int n=0;!count || n<count;++n) {
        vr::TrackedDevicePose_t poses[vr::k_unMaxTrackedDeviceCount]{};
        const auto before=std::chrono::steady_clock::now();
        system->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding,0,poses,vr::k_unMaxTrackedDeviceCount);
        const auto after=std::chrono::steady_clock::now();
        const auto ns=std::chrono::duration_cast<std::chrono::nanoseconds>(before.time_since_epoch()+(after-before)/2).count();
        std::printf("{\"t_ns\":%lld,\"devices\":[",(long long)ns);
        bool first=true;
        for(unsigned i=0;i<vr::k_unMaxTrackedDeviceCount;++i) {
            const auto kind=system->GetTrackedDeviceClass(i);
            if(kind!=vr::TrackedDeviceClass_HMD && kind!=vr::TrackedDeviceClass_Controller && kind!=vr::TrackedDeviceClass_GenericTracker)continue;
            unsigned chip=0;
            if(kind==vr::TrackedDeviceClass_GenericTracker){
                char serial[128]{};
                system->GetStringTrackedDeviceProperty(i,vr::Prop_SerialNumber_String,serial,sizeof(serial));
                if(std::string(serial).size()==17 && std::string(serial).compare(0,9,"PicoSwift")==0){
                    bool hex=true;for(int j=9;j<17;++j)if(!std::isxdigit((unsigned char)serial[j]))hex=false;
                    if(hex)chip=(unsigned)std::strtoul(serial+9,nullptr,16);
                }
            }
            if(!first)std::printf(",");first=false;
            const auto &p=poses[i];
            std::printf("{\"index\":%u,\"kind\":%d,\"chip_id\":%u,\"role\":%d,\"connected\":%s,\"valid\":%s,\"result\":%d,\"matrix\":[",
                        i,(int)kind,chip,(int)system->GetControllerRoleForTrackedDeviceIndex(i),
                        p.bDeviceIsConnected?"true":"false",p.bPoseIsValid?"true":"false",(int)p.eTrackingResult);
            for(int row=0;row<3;++row)for(int col=0;col<4;++col)
                std::printf("%s%.9g",row||col?",":"",p.mDeviceToAbsoluteTracking.m[row][col]);
            std::printf("]}");
        }
        std::printf("]}\n");std::fflush(stdout);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    vr::VR_Shutdown();
}
