/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
 */
#include "openvr.h"
#include <cstdio>
#include <cstring>
int main(int argc,char** argv) {
    if(argc!=2 || (std::strcmp(argv[1],"on") && std::strcmp(argv[1],"off"))) return 2;
    vr::EVRInitError error=vr::VRInitError_None;
    vr::VR_Init(&error,vr::VRApplication_Background);
    if(error!=vr::VRInitError_None) return 3;
    vr::EVRSettingsError se=vr::VRSettingsError_None;
    auto settings=vr::VRSettings();
    bool old=settings->GetBool("steamvr","allowTrackingCameraRecording",&se);
    if(se!=vr::VRSettingsError_None) { vr::VR_Shutdown(); return 4; }
    settings->SetBool("steamvr","allowTrackingCameraRecording",!std::strcmp(argv[1],"on"),&se);
    bool actual=settings->GetBool("steamvr","allowTrackingCameraRecording",&se);
    std::printf("{\"old\":%s,\"actual\":%s,\"error\":%d}\n",old?"true":"false",actual?"true":"false",se);
    vr::VR_Shutdown();return se==vr::VRSettingsError_None?0:5;
}
