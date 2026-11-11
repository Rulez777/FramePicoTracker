#pragma once
#include "device_config.hpp"
#include "xrtap_source.hpp"
#include "phase5/json.hpp"
#include "calibration_discovery.hpp"
#include <cstdlib>
#include <functional>
#include <iostream>
#ifndef _WIN32
namespace pico {
inline std::filesystem::path xrtap_calibration(const phase5::Value& optics){
    for(const char* field:{"calibration_dataset","documentsRoot","dataset_root"}){
        const auto it=optics.object.find(field);
        if(it!=optics.object.end())return direct_calibration_dataset(it->second.text);
    }
    throw std::runtime_error("xrtap calibration location missing");
}
inline bool use_xrtap(const phase5::Value& optics) {
    auto i=optics.object.find("camera_source");
    if(i==optics.object.end())return true;
    if(i->second.text!="xrtap"&&i->second.text!="recording")throw std::runtime_error("Unknown camera_source");
    return i->second.text=="xrtap";
}
inline XrtapConfig xrtap_config(const phase5::Value& optics) {
    XrtapConfig c;
    auto s=[&](const char* k,std::string& v){auto i=optics.object.find(k);if(i!=optics.object.end())v=i->second.text;};
    s("xrtap_path",c.path);
    auto n=[&](const char* k,double& v){auto i=optics.object.find(k);if(i!=optics.object.end())v=std::stod(i->second.text);};
    n("xrtap_ir_mean_threshold",c.ir_mean_threshold);n("xrtap_exposure_us",c.exposure_us);
    auto offset=optics.object.find("xrtap_offset_ns");if(offset!=optics.object.end())c.offset_ns=std::stoll(offset->second.text);
    auto mapping=optics.object.find("xrtap_cameras");
    if(mapping!=optics.object.end())for(auto& entry:c.cameras)entry.second=phase5::field(mapping->second,entry.first.c_str()).text;
    auto flag=[&](const char* k,bool& v){auto i=optics.object.find(k);if(i!=optics.object.end())v=i->second.text=="true";};
    auto integer=[&](const char* k,int64_t& v){auto i=optics.object.find(k);if(i!=optics.object.end())v=std::stoll(i->second.text);};
    if(const char* e=std::getenv("PICO_XRTAP_LOWER"))c.lower_enabled=std::string(e)=="1";
    flag("xrtap_lower",c.lower_enabled);flag("xrtap_lower_mapping_verified",c.lower_mapping_verified);
    integer("xrtap_lower_offset_ns",c.lower_offset_ns);integer("xrtap_lower_pair_ns",c.lower_pair_ns);
    integer("xrtap_lower_upper_ns",c.lower_upper_ns);integer("xrtap_lower_wait_ns",c.lower_wait_ns);
    // {"/dev/video9":"SPSE...","/dev/video13":"SPSE..."}; node names may be replaced as a whole.
    auto lower=optics.object.find("xrtap_lower_cameras");
    if(lower!=optics.object.end()){
        if(lower->second.object.size()!=2)throw std::runtime_error("Invalid xrtap configuration");
        size_t i=0;for(const auto& entry:lower->second.object)c.lower_cameras[i++]={entry.first,entry.second.text};
    }
    auto lower_serial=[](const std::string& s){return s==pico::devices::lower(1)||s==pico::devices::lower(0);};
    if(c.lower_enabled&&(c.lower_cameras[0].second==c.lower_cameras[1].second||c.lower_cameras[0].first==c.lower_cameras[1].first||
        !lower_serial(c.lower_cameras[0].second)||!lower_serial(c.lower_cameras[1].second)||
        c.lower_pair_ns<0||c.lower_upper_ns<0||c.lower_upper_ns>=8000000||c.lower_wait_ns<0||c.lower_wait_ns>20000000))
        throw std::runtime_error("Invalid xrtap configuration");
    if(c.cameras[0].second==c.cameras[1].second ||
       (c.cameras[0].second!=pico::devices::upper(0)&&c.cameras[0].second!=pico::devices::upper(1)) ||
       (c.cameras[1].second!=pico::devices::upper(0)&&c.cameras[1].second!=pico::devices::upper(1)) ||
       !std::isfinite(c.ir_mean_threshold)||!std::isfinite(c.exposure_us)||c.exposure_us<=0||c.exposure_us>=1000)
        throw std::runtime_error("Invalid xrtap configuration");
    return c;
}
// The tracking-profile apply restarts XRService; its cameras need several seconds before the first
// frame. Returns once six matched IR pairs arrived. A slow source is never refused: after 30 s without
// a first pair, or 3 s after it with fewer than six, one line is logged and the wait continues (logged
// again every 30 s). Only guard() (owner stop/signal) leaves early.
inline void xrtap_preflight(XrtapSource& source,const std::function<void()>& guard,
    const std::function<void(const std::string&)>& log={},
    int64_t first_wait_ns=30000000000,int64_t stream_wait_ns=3000000000) {
    const auto start=source.counts().pairs;
    auto until=XrtapSource::now()+first_wait_ns;
    bool streaming=false;
    for(;;) {
        guard();(void)source.take_pairs();
        const auto got=source.counts().pairs-start;
        if(!streaming&&got>0){streaming=true;until=XrtapSource::now()+stream_wait_ns;}
        if(got>=6)return;
        if(XrtapSource::now()>=until) {
            const auto c=source.counts();
            const auto line=std::string(streaming?"xrtap preflight: fewer than six matched IR stereo pairs in 3 seconds"
                                                  :"xrtap preflight: no matched IR stereo pair within 30 seconds")+
                "; still waiting pairs="+std::to_string(c.pairs-start)+" non_ir="+std::to_string(c.non_ir)+" torn="+
                std::to_string(c.torn)+" unpaired="+std::to_string(c.unpaired)+" dropped="+std::to_string(c.dropped);
            if(log)log(line);else std::cerr<<line<<'\n';
            until=XrtapSource::now()+first_wait_ns;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
}
#endif
