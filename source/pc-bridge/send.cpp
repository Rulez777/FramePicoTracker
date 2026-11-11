#include "net.h"
#include "openvr.h"
#include "openvr_pose.h"
#include <csignal>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <thread>
#ifndef _WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif
static volatile std::sig_atomic_t running=1;
static void stop(int){running=0;}
// Resolve only the public C entry points. No SDK library/binary needs to be copied.
class OpenVR {
#ifdef _WIN32
    HMODULE lib_=nullptr;
    void* symbol(const char* s){return reinterpret_cast<void*>(GetProcAddress(lib_,s));}
#else
    void* lib_=nullptr;
    void* symbol(const char* s){return dlsym(lib_,s);}
#endif
    using Init=uint32_t(*)(vr::EVRInitError*,vr::EVRApplicationType,const char*);
    using Get=void*(*)(const char*,vr::EVRInitError*);
    using Shutdown=void(*)();Shutdown shutdown_=nullptr;
public:
    vr::IVRSystem* system=nullptr;
    explicit OpenVR(const std::string& path){
#ifdef _WIN32
        lib_=LoadLibraryA(path.c_str());
#else
        lib_=dlopen(path.c_str(),RTLD_NOW|RTLD_LOCAL);
#endif
        if(!lib_)throw std::runtime_error("load OpenVR API: "+path);
        auto init=reinterpret_cast<Init>(symbol("VR_InitInternal2"));
        auto get=reinterpret_cast<Get>(symbol("VR_GetGenericInterface"));
        shutdown_=reinterpret_cast<Shutdown>(symbol("VR_ShutdownInternal"));
        if(!init||!get||!shutdown_)throw std::runtime_error("OpenVR exports missing");
        vr::EVRInitError err=vr::VRInitError_None;init(&err,vr::VRApplication_Background,nullptr);
        if(err!=vr::VRInitError_None)throw std::runtime_error("OpenVR init error "+std::to_string(err));
        system=static_cast<vr::IVRSystem*>(get(vr::IVRSystem_Version,&err));
        if(!system||err!=vr::VRInitError_None){shutdown_();shutdown_=nullptr;throw std::runtime_error("OpenVR IVRSystem version unavailable");}
    }
    ~OpenVR(){if(shutdown_)shutdown_();
#ifdef _WIN32
        if(lib_)FreeLibrary(lib_);
#else
        if(lib_)dlclose(lib_);
#endif
    }
};
int main(int argc,char** argv) try {
    std::string target=getenv("PICO_PC_TARGET")?getenv("PICO_PC_TARGET"):"",rolesPath="/tmp/pico-body-status.txt";
#ifdef _WIN32
    std::string library="openvr_api.dll";
#else
    std::string library="/opt/steamvr/bin/linuxarm64/libopenvr_api.so";
#endif
    bool test=false;int frames=0,hz=100;
    for(int i=1;i<argc;++i) {std::string a=argv[i];
        if(a=="--test")test=true;
        else if(i+1<argc&&a=="--target")target=argv[++i];
        else if(i+1<argc&&a=="--roles")rolesPath=argv[++i];
        else if(i+1<argc&&a=="--openvr")library=argv[++i];
        else if(i+1<argc&&a=="--frames")frames=std::stoi(argv[++i]);
        else if(i+1<argc&&a=="--hz")hz=std::stoi(argv[++i]);
        else throw std::runtime_error("usage: pico_pc_bridge_send [--target HOST:PORT] [--roles FILE] [--openvr LIB] [--hz 90..120] [--frames N] [--test]");
    }
    if(hz<90||hz>120||frames<0)throw std::runtime_error("bad rate/frame count");
    if(target.empty())throw std::runtime_error("pass --target <PC IP>:39590 (or set PICO_PC_TARGET): the Windows PC running SteamVR");
    bridge::Network network;bridge::Udp udp;auto dest=bridge::endpoint(target);
    std::unique_ptr<OpenVR> api;if(!test)api=std::make_unique<OpenVR>(library);
    std::random_device random;bridge::Packet packet;packet.session=(static_cast<uint64_t>(random())<<32)^random()^bridge::nowNs();if(!packet.session)packet.session=1;
    std::map<uint32_t,bridge::Role> mapping;std::signal(SIGINT,stop);std::signal(SIGTERM,stop);
    uint64_t errors=0;auto next=std::chrono::steady_clock::now();
    std::cerr<<"FTB1 target="<<target<<" hz="<<hz<<" synthetic="<<test<<" raw coordinates\n";
    while(running&&(!frames||packet.sequence<static_cast<uint64_t>(frames))) {
        packet.timeNs=bridge::nowNs();packet.trackers.clear();
        if(test) {
            packet.head={};packet.head.valid=packet.head.connected=true;packet.head.p={0,1.7,0};
            for(unsigned i=0;i<3;++i) {bridge::Tracker t;t.chip=0x7E570001+i;t.role=static_cast<bridge::Role>(i+1);
                t.pose=packet.head;t.pose.p={static_cast<double>(i)*0.2,0.1,-0.2};packet.trackers.push_back(t);}
        } else {
            if(packet.sequence%100==0) {std::ifstream f(rolesPath);std::string text{std::istreambuf_iterator<char>(f),{}};mapping=bridge::roles(text);}
            vr::TrackedDevicePose_t poses[vr::k_unMaxTrackedDeviceCount]{};
            api->system->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseRawAndUncalibrated,0,poses,vr::k_unMaxTrackedDeviceCount);
            packet.head=bridge::fromVr(poses[0]);
            for(uint32_t i=1;i<vr::k_unMaxTrackedDeviceCount&&packet.trackers.size()<bridge::MaxTrackers;++i) {
                if(api->system->GetTrackedDeviceClass(i)!=vr::TrackedDeviceClass_GenericTracker)continue;
                char serial[128]{};vr::ETrackedPropertyError err;
                api->system->GetStringTrackedDeviceProperty(i,vr::Prop_SerialNumber_String,serial,sizeof(serial),&err);
                if(err!=vr::TrackedProp_Success)continue;
                std::string s(serial);uint32_t chip=0;if(s.size()!=17||s.substr(0,9)!="PicoSwift"||!bridge::chipId(s.substr(9),chip))continue;
                bridge::Tracker t;t.chip=chip;t.pose=bridge::fromVr(poses[i]);auto it=mapping.find(chip);if(it!=mapping.end())t.role=it->second;
                packet.trackers.push_back(t);
            }
        }
        ++packet.sequence;if(!udp.send(dest,bridge::encode(packet)))++errors;
        if(packet.sequence%500==0)std::cerr<<"sent="<<packet.sequence<<" errors="<<errors<<" trackers="<<packet.trackers.size()<<" head_valid="<<packet.head.valid<<"\n";
        next+=std::chrono::nanoseconds(1000000000/hz);std::this_thread::sleep_until(next);
        if(std::chrono::steady_clock::now()-next>std::chrono::milliseconds(100))next=std::chrono::steady_clock::now();
    }
    std::cerr<<"sent="<<packet.sequence<<" errors="<<errors<<"\n";return errors?1:0;
}catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 1;}
