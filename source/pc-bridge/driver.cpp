#include "net.h"
#pragma warning(push)
#pragma warning(disable:4100) // Valve header has unused parameters in default virtual methods.
#include "openvr_driver.h"
#pragma warning(pop)
#include "openvr_pose.h"
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <mutex>

namespace {
using namespace bridge;
class Device final:public vr::ITrackedDeviceServerDriver {
    uint32_t index_=vr::k_unTrackedDeviceIndexInvalid;
    Role role_=Role::Unknown;vr::DriverPose_t pose_{};mutable std::mutex mutex_;
    void properties() {
        if(index_==vr::k_unTrackedDeviceIndexInvalid)return;
        auto c=vr::VRProperties()->TrackedDeviceToPropertyContainer(index_);
        vr::VRProperties()->SetStringProperty(c,vr::Prop_ControllerType_String,controller(role_));
        vr::VRProperties()->SetStringProperty(c,vr::Prop_InputProfilePath_String,profile(role_).c_str());
    }
public:
    std::string serial;
    Role currentRole() const {return role_;}
    Device(uint32_t chip,Role r):role_(r){char s[64];std::snprintf(s,sizeof(s),"FramePicoSwift%08X",chip);serial=s;pose_.qRotation.w=1;pose_.qWorldFromDriverRotation.w=1;pose_.qDriverFromHeadRotation.w=1;}
    vr::EVRInitError Activate(uint32_t id) override {
        index_=id;auto p=vr::VRProperties();auto c=p->TrackedDeviceToPropertyContainer(id);
        p->SetStringProperty(c,vr::Prop_SerialNumber_String,serial.c_str());
        p->SetStringProperty(c,vr::Prop_ModelNumber_String,"Frame PICO Tracker Bridge");
        p->SetStringProperty(c,vr::Prop_ManufacturerName_String,"Frame Trackers");
        p->SetStringProperty(c,vr::Prop_TrackingSystemName_String,"frametrackers");
        p->SetStringProperty(c,vr::Prop_RenderModelName_String,"{frametrackers}frame_tracker");
        p->SetInt32Property(c,vr::Prop_DeviceClass_Int32,vr::TrackedDeviceClass_GenericTracker);
        p->SetInt32Property(c,vr::Prop_ControllerRoleHint_Int32,vr::TrackedControllerRole_OptOut);
        // HMD raw space alignment is already applied to every submitted pose.
        p->SetBoolProperty(c,vr::Prop_WillDriftInYaw_Bool,true);
        properties();vr::VRDriverLog()->Log(("frametrackers: added "+serial+"\n").c_str());return vr::VRInitError_None;
    }
    void Deactivate() override {index_=vr::k_unTrackedDeviceIndexInvalid;}
    void EnterStandby() override {}
    void* GetComponent(const char*) override {return nullptr;}
    void DebugRequest(const char*,char* response,uint32_t size) override {if(size)response[0]=0;}
    vr::DriverPose_t GetPose() override {std::lock_guard<std::mutex> lock(mutex_);return pose_;}
    void publish(const Pose& p,Role r,double age) {
        if(role_!=r){role_=r;properties();}
        vr::DriverPose_t out{};out.qWorldFromDriverRotation.w=out.qDriverFromHeadRotation.w=1;
        out.qRotation={p.q.w,p.q.x,p.q.y,p.q.z};
        out.vecPosition[0]=p.p.x;out.vecPosition[1]=p.p.y;out.vecPosition[2]=p.p.z;
        out.vecVelocity[0]=p.v.x;out.vecVelocity[1]=p.v.y;out.vecVelocity[2]=p.v.z;
        out.vecAngularVelocity[0]=p.w.x;out.vecAngularVelocity[1]=p.w.y;out.vecAngularVelocity[2]=p.w.z;
        out.deviceIsConnected=p.connected;out.poseIsValid=p.valid;
        out.result=out.poseIsValid?vr::TrackingResult_Running_OK:vr::TrackingResult_Running_OutOfRange;
        out.poseTimeOffset=-std::min(age,0.5);
        {std::lock_guard<std::mutex> lock(mutex_);pose_=out;}
        if(index_!=vr::k_unTrackedDeviceIndexInvalid)vr::VRServerDriverHost()->TrackedDevicePoseUpdated(index_,out,sizeof(out));
    }
};
class Provider final:public vr::IServerTrackedDeviceProvider {
    std::unique_ptr<Network> network_;std::unique_ptr<Receiver> receiver_;
    std::map<uint32_t,std::unique_ptr<Device>> devices_;Alignment alignment_;uint64_t session_=0;
    double lastLog_=0;bool lastFresh_=false;
public:
    vr::EVRInitError Init(vr::IVRDriverContext* context) override {
        VR_INIT_SERVER_DRIVER_CONTEXT(context);
        try {
            network_=std::make_unique<Network>();receiver_=std::make_unique<Receiver>();
            const char* address=std::getenv("FRAME_TRACKERS_BIND");receiver_->bindTo(address?address:"0.0.0.0:39590");
            vr::VRDriverLog()->Log("frametrackers: listening UDP 39590 FTB1\n");
        }catch(const std::exception& e){vr::VRDriverLog()->Log(e.what());receiver_.reset();network_.reset();VR_CLEANUP_SERVER_DRIVER_CONTEXT();return vr::VRInitError_Driver_Failed;}
        return vr::VRInitError_None;
    }
    void Cleanup() override {devices_.clear();receiver_.reset();network_.reset();alignment_={};session_=0;VR_CLEANUP_SERVER_DRIVER_CONTEXT();}
    const char* const* GetInterfaceVersions() override {return vr::k_InterfaceVersions;}
    void RunFrame() override {
        if(!receiver_)return;
        unsigned received=receiver_->poll();const double t=now();auto& s=receiver_->stream;
        bool fresh=s.fresh(t);
        if(received && fresh) {
            if(session_!=s.packet.session){alignment_={};session_=s.packet.session;}
            vr::TrackedDevicePose_t pc[vr::k_unMaxTrackedDeviceCount]{};
            // Ask for the PC HMD at the packet sample's estimated time, not at now.
            vr::VRServerDriverHost()->GetRawTrackedDevicePoses(-static_cast<float>(s.age(t)),pc,vr::k_unMaxTrackedDeviceCount);
            Pose head=fromVr(pc[0]);
            if(!head.valid||!s.packet.head.valid){alignment_.have=false;}
            else alignment_.update(head,s.packet.head,t);
            for(const auto& tr:s.packet.trackers) {
                if(!devices_.count(tr.chip)&&devices_.size()<MaxTrackers) {
                    auto d=std::make_unique<Device>(tr.chip,tr.role);
                    if(vr::VRServerDriverHost()->TrackedDeviceAdded(d->serial.c_str(),vr::TrackedDeviceClass_GenericTracker,d.get()))devices_.emplace(tr.chip,std::move(d));
                }
            }
        }
        for(auto& kv:devices_) {
            Role r=kv.second->currentRole();
            if(s.have)for(const auto& tr:s.packet.trackers)if(tr.chip==kv.first){r=tr.role;break;}
            kv.second->publish(publication(s,alignment_,kv.first,t),r,s.age(t));
        }
        if(t-lastLog_>5||fresh!=lastFresh_) {
            char msg[256];std::snprintf(msg,sizeof(msg),"frametrackers: stream=%d aligned=%d devices=%zu seq=%llu age_ms=%.1f rejected=%llu\n",
                fresh?1:0,alignment_.have?1:0,devices_.size(),static_cast<unsigned long long>(s.packet.sequence),s.age(t)*1000,static_cast<unsigned long long>(s.rejected));
            vr::VRDriverLog()->Log(msg);lastLog_=t;lastFresh_=fresh;
        }
    }
    bool ShouldBlockStandbyMode() override {return false;}
    void EnterStandby() override {}
    void LeaveStandby() override {}
} provider;
}
extern "C" __declspec(dllexport) void* HmdDriverFactory(const char* name,int* error) {
    if(name&&std::strcmp(name,vr::IServerTrackedDeviceProvider_Version)==0)return &provider;
    if(error)*error=vr::VRInitError_Init_InterfaceNotFound;
    return nullptr;
}
