/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
 */
#include "openvr.h"
#include <cstdio>
#include <vector>
#include <thread>
#include <chrono>
int main() {
    vr::EVRInitError error=vr::VRInitError_None;
    auto system=vr::VR_Init(&error,vr::VRApplication_Background);
    if(error!=vr::VRInitError_None) { std::printf("{\"init_error\":%d}\n",error); return 1; }
    auto camera=vr::VRTrackedCamera();
    if(!camera) { std::puts("{\"interface_available\":false}"); vr::VR_Shutdown(); return 0; }
    bool has=false; auto e=camera->HasCamera(0,&has);
    vr::ETrackedPropertyError pe=vr::TrackedProp_Success;
    auto count=system->GetInt32TrackedDeviceProperty(0,vr::Prop_NumCameras_Int32,&pe);
    vr::EVRSettingsError settings_error=vr::VRSettingsError_None;
    bool enabled=vr::VRSettings()->GetBool("camera","enableCamera",&settings_error);
    std::printf("{\"camera_count\":%d,\"count_error\":%d,\"enableCamera\":%s,\"setting_error\":%d}\n",count,pe,enabled?"true":"false",settings_error);
    for(const char* key:{"allowTrackingCameraRecording","allowTrackingScreenRecording","allowTrackingAudioRecording"}) {
        bool value=vr::VRSettings()->GetBool("steamvr",key,&settings_error);
        std::printf("{\"setting\":\"%s\",\"value\":%s,\"error\":%d}\n",key,value?"true":"false",settings_error);
    }
    uint32_t width=0,height=0,size=0;
    auto se=camera->GetCameraFrameSize(0,vr::VRTrackedCameraFrameType_Distorted,&width,&height,&size);
    std::printf("{\"has_camera\":%s,\"has_error\":%d,\"size_error\":%d,\"width\":%u,\"height\":%u,\"bytes\":%u}",has?"true":"false",e,se,width,height,size);
    if(has && se==vr::VRTrackedCameraError_None && size>0 && size<64*1024*1024) {
        vr::TrackedCameraHandle_t handle=0;
        auto ae=camera->AcquireVideoStreamingService(0,&handle);
        std::printf("\n{\"acquire_error\":%d}",ae);
        if(ae==vr::VRTrackedCameraError_None) {
            std::vector<unsigned char> buffer(size); vr::CameraVideoStreamFrameHeader_t header{};
            for(int n=0;n<20;n++) {
                auto fe=camera->GetVideoStreamFrameBuffer(handle,vr::VRTrackedCameraFrameType_Distorted,buffer.data(),size,&header,sizeof(header));
                if(fe==vr::VRTrackedCameraError_None) { std::printf("\n{\"frame_received\":true,\"sequence\":%u}",header.nFrameSequence); break; }
                if(n==19) std::printf("\n{\"frame_received\":false,\"frame_error\":%d}",fe);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            camera->ReleaseVideoStreamingService(handle);
        }
    }
    std::puts(""); vr::VR_Shutdown();
}
