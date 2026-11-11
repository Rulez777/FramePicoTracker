#include "../guard_evidence.hpp"
#include "tracking_guard.hpp"
#include "orchestrator.hpp"
#include <set>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <algorithm>
namespace phase5 {
namespace {
const Value& tracking_field(const Value& v,const char* name){auto it=v.object.find(name);if(v.type!='{'||it==v.object.end())throw std::runtime_error("Missing tracking field");
    return it->second;}
bool boolean(const Value& v){if(v.type!='b')throw std::runtime_error("Tracking boolean required");
    return v.text=="true";}
int64_t tracking_integer(const Value& v){if(v.type!='i'||v.text.find_first_of(".eE")!=std::string::npos)throw std::runtime_error("Tracking integer required");
size_t n=0;auto x=std::stoll(v.text,&n);if(n!=v.text.size())throw std::runtime_error("Tracking integer syntax");
    return x;}
}
namespace {
Value validated_sample(const std::string& line,std::optional<int64_t> last_stamp){
    if(line.size()>65536)throw std::runtime_error("Tracking line exceeds limit");
    auto v=parse(line);auto stamp=tracking_integer(tracking_field(v,"t_ns"));
    if(stamp<0||(last_stamp&&stamp<=*last_stamp))throw std::runtime_error("Nonforward tracking clock");
    auto& devices=tracking_field(v,"devices");if(devices.type!='[')throw std::runtime_error("Tracking device array required");
    std::set<int64_t> indices;
    for(auto& d:devices.array){
        auto index=tracking_integer(tracking_field(d,"index"));if(index<0||index>=64||!indices.insert(index).second)throw std::runtime_error("Tracking device index");
        (void)tracking_integer(tracking_field(d,"kind"));(void)tracking_integer(tracking_field(d,"result"));
        (void)boolean(tracking_field(d,"connected"));(void)boolean(tracking_field(d,"valid"));
        auto& matrix=tracking_field(d,"matrix");if(matrix.type!='['||matrix.array.size()!=12)throw std::runtime_error("Tracking matrix shape");
        for(auto& x:matrix.array){if(x.type!='i'||!std::isfinite(std::stod(x.text)))throw std::runtime_error("Tracking matrix value");}
    }
    return v;
}
}
void BenchTrackingGuard::ingest(const std::string& line) try {
    auto v=validated_sample(line,last_stamp_);
    unsafe_row_.clear();
    size_t hmds=0;bool healthy_hmd=false;
    for(const auto& d:tracking_field(v,"devices").array){
        const auto kind=tracking_integer(tracking_field(d,"kind"));
        const auto index=tracking_integer(tracking_field(d,"index"));
        const bool connected=boolean(tracking_field(d,"connected"));
        if(kind==2&&connected)unsafe_row_="Valve controller connected during controllers-off bench";
        if(connected&&(kind<1||kind>5))unsafe_row_="Unknown connected OpenVR device class";
        if(index==0&&kind!=1)unsafe_row_="OpenVR HMD index/class mismatch";
        if(kind==1){++hmds;healthy_hmd=index==0&&connected&&boolean(tracking_field(d,"valid"))&&tracking_integer(tracking_field(d,"result"))==200;}
    }
    if(hmds!=1||!healthy_hmd)unsafe_row_="HMD tracking lost or ambiguous during optical bench";
    last_stamp_=tracking_integer(tracking_field(v,"t_ns"));sample_=std::move(v);
    if(observe_)observe_(*sample_);
}
catch(const std::exception& e){std::cerr<<e.what()<<"; dropping observer sample\n";}
bool BenchTrackingGuard::check(int64_t now,bool alive,bool require_now) try {
    if(!alive)throw std::runtime_error("OpenVR observer terminated or failed");
    if(!unsafe_row_.empty())throw std::runtime_error(unsafe_row_);
    if(last_stamp_&&now<*last_stamp_)throw std::runtime_error("Future OpenVR sample clock");
    bool fresh=last_stamp_&&now>=*last_stamp_&&now-*last_stamp_<250000000;
    if(fresh){
        bool hmd=false;
        for(auto& d:tracking_field(*sample_,"devices").array){
            auto kind=tracking_integer(tracking_field(d,"kind"));bool connected=boolean(tracking_field(d,"connected"));
            if(kind==2&&connected)throw std::runtime_error("Valve controller connected during controllers-off bench");
            hmd|=kind==1&&connected&&boolean(tracking_field(d,"valid"))&&tracking_integer(tracking_field(d,"result"))==200;
        }
        if(!hmd)throw std::runtime_error("HMD tracking lost during optical bench");
        stale_since_.reset();return true;
    }
    if(require_now)throw std::runtime_error("Fresh OpenVR sample required before RF");
    if(!stale_since_)stale_since_=now;
    if(now<*stale_since_||now-*stale_since_>500000000)throw std::runtime_error("OpenVR monitor stale; controllers-off cannot be confirmed");
    std::cerr<<"Tracking sample unavailable or stale; continuing\n";return false;
}
catch(const std::exception& e){std::cerr<<e.what()<<"; continuing without fresh tracking\n";return false;}
void CoexistenceTrackingGuard::ingest(const std::string& line) try {
    auto v=validated_sample(line,stamp_);
    const auto stamp=tracking_integer(tracking_field(v,"t_ns"));
    for(const auto& d:tracking_field(v,"devices").array){
        if(tracking_integer(tracking_field(d,"kind"))!=2)continue;
        auto index=tracking_integer(tracking_field(d,"index"));
        std::array<double,12> matrix{};size_t i=0;
        for(const auto& x:tracking_field(d,"matrix").array)matrix[i++]=std::stod(x.text);
        auto previous=changed_.find(index);
        if(previous==changed_.end()||previous->second.second!=matrix)changed_[index]={stamp,matrix};
    }
    stamp_=stamp;sample_=std::move(v);

}
catch(const std::exception& e){std::cerr<<e.what()<<"; dropping observer sample\n";}
bool CoexistenceTrackingGuard::check(int64_t now,bool alive,bool require_now) try {

    if(!alive)throw std::runtime_error("OpenVR observer terminated or failed");
    if(now<0||(stamp_&&now<*stamp_))throw std::runtime_error("Future OpenVR sample clock");
    bool fresh=stamp_&&now-*stamp_<250000000;
    bool hmd=false;std::set<int64_t> indices;
    if(fresh)for(const auto& d:tracking_field(*sample_,"devices").array){
        bool healthy=boolean(tracking_field(d,"connected"))&&boolean(tracking_field(d,"valid"))&&tracking_integer(tracking_field(d,"result"))==200;
        auto kind=tracking_integer(tracking_field(d,"kind"));
        if(healthy&&kind==1)hmd=true;
        if(healthy&&kind==2)indices.insert(tracking_integer(tracking_field(d,"index")));
    }
    if(hmd&&indices.size()>=2){
        if(!admitted_)admitted_=indices;
        if(std::includes(indices.begin(),indices.end(),admitted_->begin(),admitted_->end())){
            for(auto index:*admitted_)if(*stamp_-changed_.at(index).first>150000000)
                {pico::guard_evidence::emit(7,"coexistence-150ms",false);throw std::runtime_error("Controller pose frozen beyond 150ms; stopping custom RF");}
            pico::guard_evidence::emit(7,"coexistence-150ms",true);
            invalid_since_.reset();return true;
        }
    }
    if(require_now)throw std::runtime_error("Fresh valid HMD and two original controllers required before RF");
    if(!invalid_since_)invalid_since_=now;
    if(now<*invalid_since_||now-*invalid_since_>500000000)throw std::runtime_error("HMD/controller tracking lost beyond 500ms");
    std::cerr<<"Tracking sample unavailable or stale; continuing\n";return false;
}
catch(const std::exception& e){std::cerr<<e.what()<<"; continuing without fresh tracking\n";return false;}
}
