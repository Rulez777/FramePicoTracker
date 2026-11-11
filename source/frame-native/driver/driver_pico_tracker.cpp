#include "mag_store.h"
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
#include "pico_fusion.h"
#include "pico_imu_time.h"
#include "pico_burst_time.h"
#include "pico_sample_clock.h"
#include "pico_body.h"
#include "sensor_wire.h"
#include "sensor_config.h"
#include <mutex>
#include <dlfcn.h>
#include <sys/stat.h>
#include <map>
#include <algorithm>

// Raw IMU sample forwarded by the producer (PTI1, little-endian): the producer does no alignment; counter unwrapping,
// sample time and fusion happen here in native code.
#pragma pack(push, 1)
struct ImuFrameWire {
    char magic[4];
    uint32_t chip;
    uint8_t counter, pad[3];
    uint64_t receiveTime;  // host CLOCK_MONOTONIC ns of the radio receive (Roy cycle clock converted)
    int16_t gyro[3], accel[3];
};
#pragma pack(pop)
static_assert(sizeof(ImuFrameWire) == 32, "PTI1 ABI");

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
        std::lock_guard<std::recursive_mutex> lock(m_poseMutex);
        if (m_fusion && !m_bodyDriven) {  // with the body model the provider publishes every frame (RunFrame)
            timespec ts{}; clock_gettime(CLOCK_MONOTONIC,&ts);
            ApplyFused(m_fusion->Pose(uint64_t(ts.tv_sec)*1000000000ull+ts.tv_nsec),false);
        }
        return m_Pose;
    }

    void BindFusion(pico_fusion::Tracker *f) { m_fusion=f; }
    void SetBodyDriven(bool b) { m_bodyDriven=b; }

    void UpdatePose(double x, double y, double z, double qx, double qy, double qz, double qw) {
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

    void UpdateBattery(const PoseFrameWire &p) {
        if (m_unObjectId!=vr::k_unTrackedDeviceIndexInvalid) {
            const auto container=vr::VRProperties()->TrackedDeviceToPropertyContainer(m_unObjectId);
            vr::VRProperties()->SetBoolProperty(container,vr::Prop_DeviceProvidesBatteryStatus_Bool,(p.flags&4u)!=0);
            if(p.flags&4u)
                vr::VRProperties()->SetFloatProperty(container,vr::Prop_DeviceBatteryPercentage_Float,p.batteryPercent/100.f);
        }
    }

    // Fused pose for "now": IMU-propagated orientation + optical position, published every driver frame.
    void ApplyFused(const pico_fusion::Tracker::Output &o, bool publish=true) {
        std::lock_guard<std::recursive_mutex> lock(m_poseMutex);
        m_Pose.poseTimeOffset = 0;
        m_Pose.deviceIsConnected = true;
        m_Pose.poseIsValid = o.valid;
        m_Pose.result = !o.valid ? vr::TrackingResult_Running_OutOfRange
                      : o.positionTracked ? vr::TrackingResult_Running_OK : vr::TrackingResult_Fallback_RotationOnly;
        if (o.valid) {
            m_Pose.vecPosition[0]=o.position.x; m_Pose.vecPosition[1]=o.position.y; m_Pose.vecPosition[2]=o.position.z;
            m_Pose.qRotation.w=o.orientation.w; m_Pose.qRotation.x=o.orientation.x;
            m_Pose.qRotation.y=o.orientation.y; m_Pose.qRotation.z=o.orientation.z;
            m_Pose.vecVelocity[0]=o.linearVelocity.x; m_Pose.vecVelocity[1]=o.linearVelocity.y; m_Pose.vecVelocity[2]=o.linearVelocity.z;
#ifndef PICO_NO_ANGVEL
            m_Pose.vecAngularVelocity[0]=o.angularVelocityWorld.x; m_Pose.vecAngularVelocity[1]=o.angularVelocityWorld.y;
            m_Pose.vecAngularVelocity[2]=o.angularVelocityWorld.z;
#else
            for (int i=0;i<3;++i) m_Pose.vecAngularVelocity[i]=0;
#endif
        } else {
            for (int i=0;i<3;++i) { m_Pose.vecVelocity[i]=0; m_Pose.vecAngularVelocity[i]=0; }
        }
        if (publish && m_unObjectId != vr::k_unTrackedDeviceIndexInvalid)
            vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, m_Pose, sizeof(m_Pose));
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
        m_Pose.poseIsValid = false;
        m_Pose.deviceIsConnected = connected;
        m_Pose.result = vr::TrackingResult_Running_OutOfRange;
        if (m_unObjectId != vr::k_unTrackedDeviceIndexInvalid)
            vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, m_Pose, sizeof(m_Pose));
    }

private:
    std::recursive_mutex m_poseMutex;
    pico_fusion::Tracker *m_fusion=nullptr; // owned by provider until device cleanup
    bool m_bodyDriven=false;  // body model active: GetPose returns the last published pose
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

        LoadCalibration();
        return vr::VRInitError_None;
    }

    // resources/imu_calibration.txt next to this driver: one line per chip
    //   chip_hex r00 r01 r02 r10 r11 r12 r20 r21 r22 gyro_rad_per_lsb bias_x bias_y bias_z imu_offset_ns period_ns
    // Absent file or chip: that chip keeps the optical-only behaviour.
    void LoadCalibration() {
        Dl_info info{};
        if (!dladdr(reinterpret_cast<void*>(&HmdDriverFactoryAnchor), &info) || !info.dli_fname) return;
        std::string path = info.dli_fname;
        for (int up = 0; up < 3; ++up) { auto cut = path.find_last_of('/'); if (cut == std::string::npos) return; path.resize(cut); }
        path += "/resources/imu_calibration.txt";
        m_magPath=path.substr(0,path.size()-std::strlen("imu_calibration.txt"))+"mag_calibration.txt";
        FILE *f = std::fopen(path.c_str(), "r");
        if (!f) return;
        char line[512];
        while (std::fgets(line, sizeof(line), f)) {
            if (line[0] == '#') continue;
            unsigned chip; pico_fusion::Calibration c; double period = 0; double r[9];
            if (std::sscanf(line, "%x %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", &chip, &r[0], &r[1], &r[2],
                            &r[3], &r[4], &r[5], &r[6], &r[7], &r[8], &c.gyroRadPerLsb, &c.gyroBiasLsb.x, &c.gyroBiasLsb.y,
                            &c.gyroBiasLsb.z, &c.imuTimeOffsetNs, &period) != 16) continue;
            for (int i = 0; i < 9; ++i) c.r[i / 3][i % 3] = r[i];
            if (!(c.gyroRadPerLsb > 0) || !(period > 3e6 && period < 12e6)) continue;  // sensor sample period per counter, ~4.94 ms
            c.valid = true;
            pico_fusion::Params prm; prm.biasGain = 0;  // online bias learning diverged in replay; fixed bias for now
            Fusion fz{std::make_unique<pico_fusion::Tracker>(c, prm), std::make_unique<pico_fusion::ImuSampleClock>(period), period};
            pico_fusion::MagCalibration mc;
            if(pico_fusion::LoadMagCalibration(m_magPath,chip,mc)) fz.tracker->SetMagCalibration(mc);
            m_fusion.emplace(chip, std::move(fz));
            if (!m_haveDefault) { m_defaultCal = c; m_defaultPeriod = period; m_defaultChip = chip; m_haveDefault = true; }
        }
        std::fclose(f);
        LoadBodyRoles(path.substr(0, path.size() - std::strlen("imu_calibration.txt")) + "body_roles.txt");
    }
    // resources/body_roles.txt (optional, 2026-10-31): "<chip_hex> pelvis|chest|left_foot|right_foot" lines or "auto <n>",
    // optional "eye_height <m>" (2026-11-02: IMU-only body proportions when no foot is seen at calibration).
    // Absent: the body model is off and every tracker is published exactly as before.
    // The FramePicoTracker app edits the file at runtime: RunFrame reloads it when its mtime changes (checked every 2 s),
    // /tmp/pico-body-calibrate requests a new two-step calibration, /tmp/pico-body-status.txt reports the step and the
    // per-tracker state (every 0.5 s).
    void LoadBodyRoles(const std::string &path) {
        m_bodyRolesPath = path;
        struct stat st{}; m_bodyRolesMtime = stat(path.c_str(), &st) == 0 ? st.st_mtime : 0;
        m_body.Reset();
        FILE *f = std::fopen(path.c_str(), "r");
        if (!f) return;
        char line[256];
        while (std::fgets(line, sizeof(line), f)) {
            if (line[0] == '#') continue;
            unsigned chip; char role[32]; int n; double eye;
            if (std::sscanf(line, "auto %d", &n) == 1) { m_body.SetAuto(n); continue; }
            if (std::sscanf(line, "eye_height %lf", &eye) == 1) { m_body.SetEyeHeight(eye); continue; }
            if (std::sscanf(line, "%x %31s", &chip, role) != 2) continue;
            static const char *nm[pico_body::RoleCount] = {"pelvis", "chest", "left_foot", "right_foot"};
            for (int r = 0; r < pico_body::RoleCount; ++r) if (!std::strcmp(role, nm[r])) m_body.SetRole(chip, r);
        }
        std::fclose(f);
        if (m_body.Active()) vr::VRDriverLog()->Log("pico_body: body model active (resources/body_roles.txt)\n");
    }
    static void HmdDriverFactoryAnchor() {}

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
        std::vector<ImuFrameWire> pendingImu;
        for (int packet=0;packet<256;++packet) {
            char buffer[512];
            const auto count=recv(m_socket,buffer,sizeof(buffer),MSG_TRUNC);
            if(count<0)break;
            // Diagnostic capture for IMU axis calibration: while /tmp/pico-wire-capture exists, append every
            // datagram as [u64 receive ns][u32 length][bytes] to /tmp/pico-wire.bin (checked every 2 s).
            if(now-m_wireCheck>2000000000ull){
                m_wireCheck=now;
                const bool want=access("/tmp/pico-wire-capture",F_OK)==0;
                if(want&&!m_wire)m_wire=std::fopen("/tmp/pico-wire.bin","ab");
                if(!want&&m_wire){std::fclose(m_wire);m_wire=nullptr;}
            }
            if(m_wire&&count>0&&size_t(count)<=sizeof(buffer)){
                const uint32_t len=uint32_t(count);
                std::fwrite(&now,sizeof(now),1,m_wire);std::fwrite(&len,sizeof(len),1,m_wire);std::fwrite(buffer,1,len,m_wire);
            }
            pico_sensors::Wire sensor;
            if(pico_sensors::Decode(buffer,size_t(count),sensor)) {
                if(sensor.hostNs>now || now-sensor.hostNs>1000000000ull ||
                   ((sensor.flags&pico_sensors::RoyTime) && (!sensor.receiveNs || sensor.receiveNs>now+10000000ull ||
                    (sensor.receiveNs<=now && now-sensor.receiveNs>500000000ull)))) continue;
                auto f=EnsureFusion(sensor.chip); if(f==m_fusion.end()) continue;
                auto &ss=m_sensors[sensor.chip];
                if(sensor.flags&pico_sensors::Imu) {
                    ss.imu.push_back(sensor);while(ss.imu.size()>128)ss.imu.pop_front();
                }
                if(sensor.flags&(pico_sensors::Imu|pico_sensors::TemperatureSnapshot)) {
                    ss.temperature=pico_sensors::Temperature(sensor.temperature); ss.temperatureNs=now;
                }
                if(sensor.flags&pico_sensors::Mag) {
                    if(sensor.flags&pico_sensors::Fresh) {
                        ++ss.fresh;
#if PICO_MAG_LEARN
                        const uint64_t mt=(sensor.flags&pico_sensors::RoyTime)?sensor.receiveNs:sensor.hostNs;
                        pico_fusion::Vec3 mb{pico_sensors::Microtesla(sensor.mag[0]),pico_sensors::Microtesla(sensor.mag[1]),pico_sensors::Microtesla(sensor.mag[2])};
                        if(!ss.magLoaded){ss.magLoaded=true;if(pico_fusion::LoadMagFit(m_magPath,sensor.chip,ss.learner.fit)){ss.seed=true;f->second.tracker->SetMagFieldCalibration(ss.learner.fit.calibration);}}
                        const auto oldOffset=ss.learner.fit.calibration.offset;
                        if(ss.learner.Observe(double(mt),mb)){
                            ss.seed=false;
                            f->second.tracker->SetMagFieldCalibration(ss.learner.fit.calibration);
                            if(pico_fusion::Norm(pico_fusion::Sub(oldOffset,ss.learner.fit.calibration.offset))>.5)ss.mapping={};
                            if(mt>ss.magSaved+30'000'000'000ULL && pico_fusion::SaveMagFit(m_magPath,sensor.chip,ss.learner.fit))ss.magSaved=mt;
                        }
                        // Query historical orientation without touching Pose()'s mutable render residual.
                        pico_fusion::Quat mq;
                        if(ss.learner.fit.ready && f->second.tracker->MagOrientation(mt,mq))
                            ss.mapping.Observe(pico_fusion::MagCalibration::Matrix(ss.learner.fit.calibration.soft,pico_fusion::Sub(mb,ss.learner.fit.calibration.offset)),mq);
#endif
                        // Stamped after the IMU of the same Roy record (below): MAG is ~10 ms older than its newest sample.
                        if(sensor.flags&pico_sensors::RoyTime) {
                            if(ss.pendingMag.size()>=64) { ss.pendingMag.pop_front(); ++ss.magUnstamped; }
                            ss.pendingMag.push_back({sensor.receiveNs,now,{pico_sensors::Microtesla(sensor.mag[0]),pico_sensors::Microtesla(sensor.mag[1]),pico_sensors::Microtesla(sensor.mag[2])}});
                        } else ++ss.magUnstamped;
                    } else ++ss.repeat;
                }
                if(sensor.flags&pico_sensors::Power) {ss.battery=sensor.batteryLevel;ss.charger=sensor.chargerRaw;ss.powerNs=now;}
                continue;
            }
            if(count==sizeof(ImuFrameWire) && !std::memcmp(buffer,"PTI1",4)) {
                ImuFrameWire imu; std::memcpy(&imu,buffer,sizeof(imu));
                auto f=EnsureFusion(imu.chip);
                if(f==m_fusion.end()) { m_diag.imuUnknownChip++; continue; }
                auto device=std::find_if(m_devices.begin(),m_devices.end(),[&](const Device &d){return d.chip==imu.chip;});
                if(device==m_devices.end() && m_devices.size()<32) {
                    char serial[32]; std::snprintf(serial,sizeof(serial),"PicoSwift%08X",imu.chip);
                    auto tracker=std::make_shared<CPicoTrackerDeviceDriver>(serial,"Tracker");
                    tracker->BindFusion(f->second.tracker.get());
                    if(vr::VRServerDriverHost()->TrackedDeviceAdded(serial,vr::TrackedDeviceClass_GenericTracker,tracker.get())) {
                        m_devices.push_back({imu.chip,tracker,0,0});
                        vr::VRDriverLog()->Log("pico_fusion: IMU visible before first optical; position is unanchored at driver origin, result 300 until optical\n");
                    }
                }
                ++m_diag.imuPackets;
                if(imu.receiveTime>now || now-imu.receiveTime>=500000000ull) {
                    ++m_diag.imuStale; // BurstTime reconciles Roy/host offset below.
                }
                m_diag.receiveLagNs=double(now-imu.receiveTime);
                pendingImu.push_back(imu);
                continue;
            }
            PoseFrameWire pose{};
            if(!DecodePose(buffer,count,now,pose))continue;
            size_t i=0;
            while(i<m_devices.size() && m_devices[i].chip!=pose.chip)++i;
            if(i==m_devices.size()) {
                if(!(pose.flags&1u) || m_devices.size()>=32)continue;
                char serial[32];
                std::snprintf(serial,sizeof(serial),"PicoSwift%08X",pose.chip);
                auto tracker=std::make_shared<CPicoTrackerDeviceDriver>(serial,"Tracker");
                auto fusion=EnsureFusion(pose.chip);
                if(fusion!=m_fusion.end())tracker->BindFusion(fusion->second.tracker.get());
                if(!vr::VRServerDriverHost()->TrackedDeviceAdded(serial,vr::TrackedDeviceClass_GenericTracker,tracker.get()))continue;
                m_devices.push_back({pose.chip,tracker,0,0});
            }
            auto &device=m_devices[i];
            // Source-time monotonicity survives a producer restart (sequence may reset).
            if(pose.sourceTime<=device.sourceTime)continue;
            device.sourceTime=pose.sourceTime;
            device.seen=now;
            auto f=m_fusion.find(pose.chip);
            if(f==m_fusion.end()) { device.tracker->Apply(pose,now); continue; }
            device.tracker->UpdateBattery(pose);
            if(pose.flags&2u) {
                pico_fusion::Quat q{pose.quaternion[3],pose.quaternion[0],pose.quaternion[1],pose.quaternion[2]};
                f->second.tracker->Optical(pose.sourceTime,{pose.position[0],pose.position[1],pose.position[2]},q);
            }
        }
        // A record shares its Roy receive stamp. Anchor its newest sample there and
        // back-space older samples by their counter distance (one counter = one sensor
        // sample, FW189 ring), never by their position in the packet.
        for(size_t i=0;i<pendingImu.size();++i) {
            const auto &imu=pendingImu[i]; size_t later=0;
            for(size_t j=i+1;j<pendingImu.size();++j)
                if(pendingImu[j].chip==imu.chip && pendingImu[j].receiveTime==imu.receiveTime) {
                    const size_t d=uint8_t(pendingImu[j].counter-imu.counter);
                    if(d<32) later=std::max(later,d);
                }
            auto &f=m_fusion.at(imu.chip);
            double stamp=f.burstTime.Stamp(now,imu.receiveTime,later,f.periodNs);
            // 2026-10-30: on the dense stream (one sample per reply) the receive stamp moves in 6 ms reply steps; the
            // sensor cadence line (pico_sample_clock.h) gives each sample its own 4.94 ms slot once fitted.
            {double t;if(f.sensorClock.Observe(imu.counter,stamp+double(later)*f.periodNs,t))stamp=t;}
            m_diag.stampLagNs=double(now)-stamp;
            f.tracker->ClearSensorImu();
            auto si=m_sensors.find(imu.chip);
            if(si!=m_sensors.end()) {
                auto &stamps=si->second.stamps;
                auto r=std::find_if(stamps.begin(),stamps.end(),[&](const std::pair<uint64_t,double> &x){return x.first==imu.receiveTime;});
                if(r!=stamps.end()) r->second=std::max(r->second,stamp);
                else {
                    stamps.push_back({imu.receiveTime,stamp});
                    while(stamps.size()>64) stamps.pop_front();
                }
            }
            if(si!=m_sensors.end()) for(const auto &v:si->second.imu)
                if(v.counter==imu.counter && v.receiveNs==imu.receiveTime && (v.flags&pico_sensors::RoyTime)) {
                    f.tracker->SensorImu(v.temperature,(v.flags&pico_sensors::Scale)?v.imuId:0);break;
                }
            f.tracker->Imu(uint64_t(stamp),imu.gyro,imu.accel);
        }
        // MAG time = newest IMU sample stamp of the same Roy record minus PICO_MAG_LATENCY_NS
        // (outputs/mag-map-claude2/REPORT.md). A record whose IMU never arrives is dropped after 100 ms.
        for(auto &kv:m_sensors) {
            auto &ss=kv.second;
            auto f=m_fusion.find(kv.first);
            for(auto it=ss.pendingMag.begin();it!=ss.pendingMag.end();) {
                auto r=std::find_if(ss.stamps.begin(),ss.stamps.end(),[&](const std::pair<uint64_t,double> &x){return x.first==it->receiveNs;});
                if(r!=ss.stamps.end() && f!=m_fusion.end() && r->second>PICO_MAG_LATENCY_NS) {
                    f->second.tracker->Magnetometer(uint64_t(r->second-PICO_MAG_LATENCY_NS),it->field);
                    it=ss.pendingMag.erase(it);
                } else if(now-it->queuedNs>100000000ull) {
                    ++ss.magUnstamped;
                    it=ss.pendingMag.erase(it);
                } else {
                    ++it;
                }
            }
        }
        if(!m_bodyRolesPath.empty() && now-m_bodyCheck>2000000000ull) {  // app interface of the body model (see LoadBodyRoles)
            m_bodyCheck=now;
            struct stat st{}; const long mtime=stat(m_bodyRolesPath.c_str(),&st)==0?long(st.st_mtime):0;
            if(mtime!=m_bodyRolesMtime) { LoadBodyRoles(m_bodyRolesPath); if(!m_body.Active()) vr::VRDriverLog()->Log("pico_body: body model off\n"); }
            if(access("/tmp/pico-body-calibrate",F_OK)==0) { unlink("/tmp/pico-body-calibrate"); m_body.Recalibrate(); vr::VRDriverLog()->Log("pico_body: calibration requested\n"); }
        }
        if(!m_bodyRolesPath.empty() && now-m_bodyStatusAt>500000000ull) {  // calibration prompts in the app follow the step
            m_bodyStatusAt=now;
            if(FILE *s=std::fopen("/tmp/pico-body-status.txt.tmp","w")) { std::fputs(m_body.Status().c_str(),s); std::fclose(s); rename("/tmp/pico-body-status.txt.tmp","/tmp/pico-body-status.txt"); }
        }
        if(now-m_diag.lastLog>2000000000ull) {  // fusion diagnostics into vrserver.txt every 2 s
            m_diag.lastLog=now;
            for(auto &kv:m_fusion) {
                const auto &d=kv.second.tracker->Diagnostics();
                char line[512];
                std::snprintf(line,sizeof(line),"pico_fusion %08X: imu pkts %llu stale %llu clockRej %llu | fusion imu %llu late %llu reinit %llu optical %llu opticsOnly %llu rejected %llu orientDrop %llu orientAnchor %llu | lag ms: receive %.1f stamp %.1f opt-imu %.1f imuAge %.1f\n",
                    kv.first,(unsigned long long)m_diag.imuPackets,(unsigned long long)m_diag.imuStale,(unsigned long long)m_diag.imuClockRejected,
                    (unsigned long long)d.imu,(unsigned long long)d.imuLate,(unsigned long long)d.reinitGaps,(unsigned long long)d.optical,
                    (unsigned long long)d.opticsOnly,(unsigned long long)kv.second.tracker->Rejected(),
                    (unsigned long long)kv.second.tracker->OrientationRejected(),(unsigned long long)kv.second.tracker->OrientationReanchors(),m_diag.receiveLagNs/1e6,m_diag.stampLagNs/1e6,
                    d.lastOpticalMinusImuNs/1e6,(double(now)-kv.second.tracker->LastImuNs())/1e6);
                vr::VRDriverLog()->Log(line);
                const auto ss=m_sensors.find(kv.first);
                if(ss!=m_sensors.end()) {
                    const auto &v=ss->second;
                    const auto mag=kv.second.tracker->MagneticDiagnostics();
                    const auto &fit=v.learner.fit;const auto &map=v.mapping;
                    std::snprintf(line,sizeof(line),"pico_mag_cal %08X state=%s offset=%.3f,%.3f,%.3f radius=%.3f rms=%.3f coverage_pct=%.1f samples=%u seen=%llu rejected=%llu mapping_pair=%d agreement=%.5f margin=%.5f mapping_samples=%u mount_verified=0\n",kv.first,fit.ready?(v.seed?"seed":"calibrated"):"learning",fit.calibration.offset.x,fit.calibration.offset.y,fit.calibration.offset.z,fit.radius,fit.rms,fit.coverage,fit.samples,(unsigned long long)v.learner.seen,(unsigned long long)v.learner.rejected,map.best,map.agreement,map.margin,map.count);
                    vr::VRDriverLog()->Log(line);
                    const auto &attempt=v.learner.attempted;
                    std::snprintf(line,sizeof(line),"pico_mag_fit %08X received=%llu rate_limited=%llu retained=%zu attempts=%llu status=%s last_solved_attempt=%llu candidate_ready=%d candidate_offset=%.3f,%.3f,%.3f radius=%.3f rms=%.3f coverage_pct=%.1f inliers=%u quality_failures=%u model=%d min_eigen=%.4f half_difference=%.3f envelope_difference=%.3f\n",kv.first,(unsigned long long)v.learner.received,(unsigned long long)v.learner.rateLimited,v.learner.points.size(),(unsigned long long)v.learner.attempts,v.learner.fitStatus,(unsigned long long)v.learner.lastSolvedAttempt,int(attempt.ready),attempt.calibration.offset.x,attempt.calibration.offset.y,attempt.calibration.offset.z,attempt.radius,attempt.rms,attempt.coverage,attempt.samples,attempt.qualityFailures,attempt.model,attempt.minEigen,attempt.halfDifference,attempt.envelopeDifference);
                    vr::VRDriverLog()->Log(line);
                    std::snprintf(line,sizeof(line),"pico_sensors %08X temperature=%.3f temp_age_s=%.1f mag_fresh=%llu mag_repeat=%llu battery_level=%d charger_raw=%d power_age_s=%.1f mag_accepted=%llu mag_rejected=%llu mag_corrections=%llu field_uT=%.3f\n",
                        kv.first,v.temperature,v.temperatureNs?(now-v.temperatureNs)/1e9:-1.,(unsigned long long)v.fresh,(unsigned long long)v.repeat,v.battery,v.charger,v.powerNs?(now-v.powerNs)/1e9:-1.,(unsigned long long)mag.accepted,(unsigned long long)mag.rejected,(unsigned long long)mag.corrections,mag.localNorm);
                    vr::VRDriverLog()->Log(line);
                    std::snprintf(line,sizeof(line),"pico_mag_gate %08X accepted=%llu reject_norm=%llu reject_history=%llu reject_gyro=%llu reject_horizontal=%llu reject_dip=%llu corrections=%llu unstamped=%llu pending=%zu dip_deg=%.2f\n",
                        kv.first,(unsigned long long)mag.accepted,(unsigned long long)mag.rejectNorm,(unsigned long long)mag.rejectHistory,(unsigned long long)mag.rejectGyro,
                        (unsigned long long)mag.rejectHorizontal,(unsigned long long)mag.rejectDip,(unsigned long long)mag.corrections,(unsigned long long)v.magUnstamped,v.pendingMag.size(),mag.dip*180/3.141592653589793);
                    vr::VRDriverLog()->Log(line);
                }
            }
        }
        if(m_body.Active()) {
            // Body model (pico_body.h): all fused outputs of this frame together with the HMD pose.
            std::vector<pico_fusion::Tracker::Output> outs(m_devices.size());
            std::vector<pico_body::BodyRig::Slot> slots;
            for(size_t i=0;i<m_devices.size();++i) {
                auto f=m_fusion.find(m_devices[i].chip);
                if(f==m_fusion.end()) continue;
                outs[i]=f->second.tracker->Pose(now); slots.push_back({m_devices[i].chip,&outs[i]});
            }
            vr::TrackedDevicePose_t hmd{}; vr::VRServerDriverHost()->GetRawTrackedDevicePoses(0,&hmd,1);
            const auto &m=hmd.mDeviceToAbsoluteTracking.m;
            const pico_fusion::Vec3 head{m[0][3],m[1][3],m[2][3]};
            const pico_fusion::Quat headQ=QuatFromMatrix(m);
            const std::string log=m_body.Step(double(now)/1e9,hmd.bPoseIsValid,head,headQ,slots.data(),int(slots.size()));
            if(!log.empty()) vr::VRDriverLog()->Log(log.c_str());
            for(size_t i=0;i<m_devices.size();++i) {
                auto &device=m_devices[i];
                if(m_fusion.find(device.chip)==m_fusion.end()) continue;
                device.tracker->SetBodyDriven(true); device.tracker->ApplyFused(outs[i]);
            }
        }
        for(auto &device:m_devices) {
            auto f=m_fusion.find(device.chip);
            if(f!=m_fusion.end()) { if(!m_body.Active()) { device.tracker->SetBodyDriven(false); device.tracker->ApplyFused(f->second.tracker->Pose(now)); } continue; }
            if(device.seen && now-device.sourceTime>250000000ull) {
                device.tracker->Invalidate();
                device.seen=0;
            }
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
    struct Fusion {
        std::unique_ptr<pico_fusion::Tracker> tracker;
        std::unique_ptr<pico_fusion::ImuSampleClock> clock;
        double periodNs;
        pico_fusion::BurstTime burstTime;
        pico_fusion::SensorSampleClock sensorClock{periodNs};
    };
    std::map<uint32_t, Fusion> m_fusion;
    std::string m_magPath;
    struct PendingMag { uint64_t receiveNs,queuedNs; pico_fusion::Vec3 field; };
    struct Sensors { pico_fusion::MagLearner learner; pico_fusion::MagMapping mapping; bool magLoaded=false,seed=false; uint64_t magSaved=0; std::deque<pico_sensors::Wire> imu;
        std::deque<PendingMag> pendingMag; std::deque<std::pair<uint64_t,double>> stamps; uint64_t magUnstamped=0; double temperature=NAN; uint64_t fresh=0,repeat=0,temperatureNs=0,powerNs=0; int battery=-1,charger=-1; };
    std::map<uint32_t,Sensors> m_sensors;
    // 2026-10-31 (full body, more trackers): a chip without its own imu_calibration.txt line gets the first line's
    // calibration. Mount rotation, gyro scale and sample period are properties of the PICO tracker model; the gyro bias
    // is per unit (a few LSB, ~1-2 deg/s), corrected by the optics while the tracker is visible. Logged once per chip.
    pico_fusion::Calibration m_defaultCal; double m_defaultPeriod = 0; uint32_t m_defaultChip = 0; bool m_haveDefault = false;
    std::map<uint32_t, Fusion>::iterator EnsureFusion(uint32_t chip) {
        auto it = m_fusion.find(chip);
        if (it != m_fusion.end() || !m_haveDefault || m_fusion.size() >= 16) return it;
        pico_fusion::Params prm; prm.biasGain = 0;
        Fusion fz{std::make_unique<pico_fusion::Tracker>(m_defaultCal, prm), std::make_unique<pico_fusion::ImuSampleClock>(m_defaultPeriod), m_defaultPeriod};
        pico_fusion::MagCalibration mc;
        if(pico_fusion::LoadMagCalibration(m_magPath,chip,mc)) fz.tracker->SetMagCalibration(mc);
        it = m_fusion.emplace(chip, std::move(fz)).first;
        char line[160];
        std::snprintf(line, sizeof(line), "pico_fusion %08X: no imu_calibration.txt line, using the %08X calibration (gyro bias may differ)\n", chip, m_defaultChip);
        vr::VRDriverLog()->Log(line);
        return it;
    }
    pico_body::BodyRig m_body;
    std::string m_bodyRolesPath; long m_bodyRolesMtime = 0; uint64_t m_bodyCheck = 0, m_bodyStatusAt = 0;
    static pico_fusion::Quat QuatFromMatrix(const float (&m)[3][4]) {
        const double t=m[0][0]+m[1][1]+m[2][2]; pico_fusion::Quat q;
        if(t>0){const double s=std::sqrt(t+1)*2; q={s/4,(m[2][1]-m[1][2])/s,(m[0][2]-m[2][0])/s,(m[1][0]-m[0][1])/s};}
        else if(m[0][0]>m[1][1]&&m[0][0]>m[2][2]){const double s=std::sqrt(1+m[0][0]-m[1][1]-m[2][2])*2; q={(m[2][1]-m[1][2])/s,s/4,(m[0][1]+m[1][0])/s,(m[0][2]+m[2][0])/s};}
        else if(m[1][1]>m[2][2]){const double s=std::sqrt(1+m[1][1]-m[0][0]-m[2][2])*2; q={(m[0][2]-m[2][0])/s,(m[0][1]+m[1][0])/s,s/4,(m[1][2]+m[2][1])/s};}
        else {const double s=std::sqrt(1+m[2][2]-m[0][0]-m[1][1])*2; q={(m[1][0]-m[0][1])/s,(m[0][2]+m[2][0])/s,(m[1][2]+m[2][1])/s,s/4};}
        return pico_fusion::Normalize(q);
    }
    FILE *m_wire = nullptr; uint64_t m_wireCheck = 0;  // diagnostic wire capture (see RunFrame)

    struct Diag { uint64_t imuPackets = 0, imuUnknownChip = 0, imuStale = 0, imuClockRejected = 0, lastLog = 0; double receiveLagNs = 0, stampLagNs = 0; } m_diag;
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
