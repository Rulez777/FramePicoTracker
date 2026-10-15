/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
 */
/**
 * @file driver_pico_tracker.cpp
 * @brief Native SteamVR OpenVR Driver for Pico Motion Trackers on Steam Frame.
 *
 * Implements vr::IServerTrackedDeviceProvider and vr::ITrackedDeviceServerDriver.
 * Exposes Pico Trackers as TrackedDeviceClass_GenericTracker devices that coexist
 * with Valve Frame Controllers (Roy) without touching the onboard nRF52 radio.
 *
 * Receives 6DoF poses and telemetry from the pico_tracker_daemon via IPC socket.
 */

#include <openvr_driver.h>
#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include "pose_wire.h"

#if defined(_WIN32)
#define HMD_DLL_EXPORT extern "C" __declspec(dllexport)
#else
#define HMD_DLL_EXPORT extern "C" __attribute__((visibility("default")))
#endif

namespace pico_driver {

class CPicoTrackerDeviceDriver : public vr::ITrackedDeviceServerDriver {
public:
    CPicoTrackerDeviceDriver(const std::string &serial, const std::string &role)
        : m_sSerialNumber(serial), m_sRole(role), m_unObjectId(vr::k_unTrackedDeviceIndexInvalid)
    {
        memset(&m_Pose, 0, sizeof(m_Pose));
        m_Pose.qWorldFromDriverRotation.w = 1.0;
        m_Pose.qDriverFromHeadRotation.w = 1.0;
        m_Pose.qRotation.w = 1.0;
        m_Pose.result = vr::TrackingResult_Uninitialized;
        m_Pose.poseIsValid = false;
        m_Pose.deviceIsConnected = false;
    }

    vr::EVRInitError Activate(uint32_t unObjectId) override {
        m_unObjectId = unObjectId;
        vr::PropertyContainerHandle_t container = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_unObjectId);

        vr::VRProperties()->SetStringProperty(container, vr::Prop_ModelNumber_String, "Pico Motion Tracker (Swift 2.0)");
        vr::VRProperties()->SetStringProperty(container, vr::Prop_SerialNumber_String, m_sSerialNumber.c_str());
        vr::VRProperties()->SetStringProperty(container, vr::Prop_RenderModelName_String, "{pico_tracker}pico_half");
        vr::VRProperties()->SetStringProperty(container, vr::Prop_ControllerType_String, "pico_motion_tracker");
        vr::VRProperties()->SetInt32Property(container, vr::Prop_DeviceClass_Int32, vr::TrackedDeviceClass_GenericTracker);
        vr::VRProperties()->SetInt32Property(container, vr::Prop_ControllerRoleHint_Int32, vr::TrackedControllerRole_OptOut);

        // Role hint property
        vr::VRProperties()->SetStringProperty(container, vr::Prop_NamedIconPathDeviceReady_String, "{pico_tracker}/icons/tracker_ready.png");

        return vr::VRInitError_None;
    }

    void Deactivate() override {
        m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
    }

    void EnterStandby() override {}

    void *GetComponent(const char *pchComponentNameAndVersion) override {
        return nullptr;
    }

    void DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize) override {
        if (unResponseBufferSize > 0) pchResponseBuffer[0] = '\0';
    }

    vr::DriverPose_t GetPose() override {
        return m_Pose;
    }

    void UpdatePose(double x, double y, double z, double qx, double qy, double qz, double qw) {
        m_HasMeasuredPose = true;
        m_Pose.poseIsValid = true;
        m_Pose.deviceIsConnected = true;
        m_Pose.result = vr::TrackingResult_Running_OK;
        m_Pose.vecPosition[0] = x;
        m_Pose.vecPosition[1] = y;
        m_Pose.vecPosition[2] = z;
        m_Pose.qRotation.x = qx;
        m_Pose.qRotation.y = qy;
        m_Pose.qRotation.z = qz;
        m_Pose.qRotation.w = qw;

        if (m_unObjectId != vr::k_unTrackedDeviceIndexInvalid) {
            vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, m_Pose, sizeof(vr::DriverPose_t));
        }
    }

    void Apply(const PoseFrameWire &p, uint64_t now) {
        m_Pose.poseTimeOffset = -double(now-p.sourceTime)/1e9;
        for (int i=0;i<3;++i) {
            m_Pose.vecVelocity[i]=p.linearVelocity[i];
            m_Pose.vecAngularVelocity[i]=p.angularVelocity[i];
        }
        if (m_unObjectId!=vr::k_unTrackedDeviceIndexInvalid) {
            const auto container=vr::VRProperties()->TrackedDeviceToPropertyContainer(m_unObjectId);
            vr::VRProperties()->SetBoolProperty(container,vr::Prop_DeviceProvidesBatteryStatus_Bool,(p.flags&4u)!=0);
            if(p.flags&4u)
                vr::VRProperties()->SetFloatProperty(container,vr::Prop_DeviceBatteryPercentage_Float,p.batteryPercent/100.f);
        }
        if (!(p.flags&2u)) { Invalidate((p.flags&1u)!=0); return; }
        double norm=0;
        for(float q:p.quaternion) norm+=q*q;
        norm=std::sqrt(norm);
        UpdatePose(p.position[0],p.position[1],p.position[2],p.quaternion[0]/norm,
                   p.quaternion[1]/norm,p.quaternion[2]/norm,p.quaternion[3]/norm);
    }

    const std::string &GetSerialNumber() const { return m_sSerialNumber; }

    void Invalidate(bool connected=false) {
        // User-requested visual continuity: retain the last measured transform
        // while RF is alive, explicitly OutOfRange rather than Running_OK.
        // Never create a visual pose before the first real optical measurement.
        m_Pose.poseIsValid = connected && m_HasMeasuredPose;
        m_Pose.deviceIsConnected = connected;
        m_Pose.result = vr::TrackingResult_Running_OutOfRange;
        m_Pose.poseTimeOffset = 0;
        for (int i=0;i<3;++i) {
            m_Pose.vecVelocity[i]=0;
            m_Pose.vecAngularVelocity[i]=0;
        }
        if (m_unObjectId != vr::k_unTrackedDeviceIndexInvalid)
            vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, m_Pose, sizeof(m_Pose));
    }

private:
    bool m_HasMeasuredPose = false;
    vr::DriverPose_t m_Pose;
    std::string m_sSerialNumber;
    std::string m_sRole;
    uint32_t m_unObjectId;
};


class CServerDriver_PicoTracker : public vr::IServerTrackedDeviceProvider {
public:
    vr::EVRInitError Init(vr::IVRDriverContext *pDriverContext) override {
        VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);
        m_socket = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        address.sin_port = htons(39571);
        if (m_socket < 0 || bind(m_socket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0) {
            if (m_socket >= 0) close(m_socket);
            m_socket = -1;
            VR_CLEANUP_SERVER_DRIVER_CONTEXT();
            return vr::VRInitError_Driver_Failed;
        }

        return vr::VRInitError_None;
    }

    void Cleanup() override {
        if (m_socket >= 0) close(m_socket);
        m_socket = -1;
        m_devices.clear();
        VR_CLEANUP_SERVER_DRIVER_CONTEXT();
    }

    const char *const *GetInterfaceVersions() override {
        return vr::k_InterfaceVersions;
    }

    void RunFrame() override {
        timespec ts{};
        clock_gettime(CLOCK_MONOTONIC,&ts);
        const uint64_t now=uint64_t(ts.tv_sec)*1000000000ull+ts.tv_nsec;
        for (int packet=0;packet<64;++packet) {
            char buffer[512];
            const auto count=recv(m_socket,buffer,sizeof(buffer),MSG_TRUNC);
            if(count<0)break;
            PoseFrameWire pose{};
            if(!DecodePose(buffer,count,now,pose))continue;
            size_t i=0;
            while(i<m_devices.size() && m_devices[i].chip!=pose.chip)++i;
            if(i==m_devices.size()) {
                if(!(pose.flags&1u) || m_devices.size()>=32)continue;
                char serial[32];
                std::snprintf(serial,sizeof(serial),"PicoSwift%08X",pose.chip);
                auto tracker=std::make_shared<CPicoTrackerDeviceDriver>(serial,"Tracker");
                if(!vr::VRServerDriverHost()->TrackedDeviceAdded(serial,vr::TrackedDeviceClass_GenericTracker,tracker.get()))continue;
                m_devices.push_back({pose.chip,tracker,0,0});
            }
            auto &device=m_devices[i];
            // Source-time monotonicity survives a producer restart (sequence may reset).
            if(pose.sourceTime<=device.sourceTime)continue;
            device.sourceTime=pose.sourceTime;
            device.seen=now;
            device.tracker->Apply(pose,now);
        }
        for(auto &device:m_devices)
            if(device.seen && now-device.sourceTime>250000000ull) {
                device.tracker->Invalidate();
                device.seen=0;
            }
    }

    bool ShouldBlockStandbyMode() override { return false; }
    void EnterStandby() override {}
    void LeaveStandby() override {}

private:
    int m_socket = -1;
    struct Device {
        uint32_t chip;
        std::shared_ptr<CPicoTrackerDeviceDriver> tracker;
        uint64_t sourceTime, seen;
    };
    std::vector<Device> m_devices;
};

static CServerDriver_PicoTracker g_ServerDriverPicoTracker;

} // namespace pico_driver

HMD_DLL_EXPORT void *HmdDriverFactory(const char *pInterfaceName, int *pReturnCode) {
    if (0 == strcmp(vr::IServerTrackedDeviceProvider_Version, pInterfaceName)) {
        return &pico_driver::g_ServerDriverPicoTracker;
    }
    if (pReturnCode) *pReturnCode = vr::VRInitError_Init_InterfaceNotFound;
    return nullptr;
}
