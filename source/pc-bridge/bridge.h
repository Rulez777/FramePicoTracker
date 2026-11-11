#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace bridge {
constexpr unsigned MaxTrackers=4;
enum class Role : uint8_t { Unknown, Waist, LeftFoot, RightFoot, Chest };
inline const char* controller(Role r) {
    switch(r) { case Role::Waist:return "vive_tracker_waist"; case Role::LeftFoot:return "vive_tracker_left_foot";
    case Role::RightFoot:return "vive_tracker_right_foot"; case Role::Chest:return "vive_tracker_chest";
    default:return "vive_tracker"; }
}
inline std::string profile(Role r){return std::string("{frametrackers}/input/")+controller(r)+"_profile.json";}
inline Role role(const std::string& s) {
    if(s=="pelvis") return Role::Waist;
    if(s=="left_foot") return Role::LeftFoot;
    if(s=="right_foot") return Role::RightFoot;
    if(s=="chest") return Role::Chest;
    return Role::Unknown;
}
inline bool chipId(const std::string& s,uint32_t& chip) {
    if(s.size()!=8 || s.find_first_not_of("0123456789abcdefABCDEF")!=std::string::npos) return false;
    chip=static_cast<uint32_t>(std::stoul(s,nullptr,16)); return chip!=0;
}
inline std::map<uint32_t,Role> roles(const std::string& text) {
    std::map<uint32_t,Role> out; std::istringstream in(text); std::string s;
    while(in>>s) { auto eq=s.find('='); uint32_t chip=0;
        if(eq==8 && chipId(s.substr(0,8),chip)) out[chip]=role(s.substr(9)); }
    return out;
}
struct Vec { double x=0,y=0,z=0; };
inline Vec operator+(Vec a,Vec b){return {a.x+b.x,a.y+b.y,a.z+b.z};}
inline Vec operator-(Vec a,Vec b){return {a.x-b.x,a.y-b.y,a.z-b.z};}
inline Vec operator*(Vec a,double s){return {a.x*s,a.y*s,a.z*s};}
struct Quat { double w=1,x=0,y=0,z=0; };
inline Quat conjugate(Quat q){return {q.w,-q.x,-q.y,-q.z};}
inline Quat operator*(Quat a,Quat b) {return {a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z,
    a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w};}
inline Quat normalize(Quat q) {double n=std::sqrt(q.w*q.w+q.x*q.x+q.y*q.y+q.z*q.z);return {q.w/n,q.x/n,q.y/n,q.z/n};}
inline Vec rotate(Quat q,Vec p) {auto r=q*Quat{0,p.x,p.y,p.z}*conjugate(q);return {r.x,r.y,r.z};}
inline Quat blend(Quat a,Quat b,double t) {
    if(a.w*b.w+a.x*b.x+a.y*b.y+a.z*b.z<0) b={-b.w,-b.x,-b.y,-b.z};
    return normalize({a.w+(b.w-a.w)*t,a.x+(b.x-a.x)*t,a.y+(b.y-a.y)*t,a.z+(b.z-a.z)*t});
}
struct Pose { Vec p; Quat q; Vec v,w; bool valid=false,connected=false; };
struct Tracker { uint32_t chip=0; Role role=Role::Unknown; Pose pose; };
struct Packet { uint64_t session=0,sequence=0,timeNs=0; Pose head; std::vector<Tracker> trackers; };
inline bool validPose(const Pose& p) {
    const double values[]={p.p.x,p.p.y,p.p.z,p.q.w,p.q.x,p.q.y,p.q.z,p.v.x,p.v.y,p.v.z,p.w.x,p.w.y,p.w.z};
    for(double d:values) if(!std::isfinite(d)||std::abs(d)>1e6) return false;
    double n=p.q.w*p.q.w+p.q.x*p.q.x+p.q.y*p.q.y+p.q.z*p.q.z;
    return n>0.99 && n<1.01 && (!p.valid||p.connected);
}
// FTB1, LE integers + IEEE754 binary32. No compiler structs on the wire.
// Header 36 B; head 56 B; each tracker 64 B. Max datagram 348 B, no fragmentation.
inline void u32(std::vector<uint8_t>& b,uint32_t x){for(int i=0;i<4;++i)b.push_back(static_cast<uint8_t>(x>>(8*i)));}
inline void u64(std::vector<uint8_t>& b,uint64_t x){u32(b,static_cast<uint32_t>(x));u32(b,static_cast<uint32_t>(x>>32));}
inline void scalar(std::vector<uint8_t>& b,double d){float f=static_cast<float>(d);uint32_t x;std::memcpy(&x,&f,4);u32(b,x);}
inline void putPose(std::vector<uint8_t>& b,const Pose& p) {
    u32(b,(p.valid?1u:0u)|(p.connected?2u:0u));
    for(double d:{p.p.x,p.p.y,p.p.z,p.q.w,p.q.x,p.q.y,p.q.z,p.v.x,p.v.y,p.v.z,p.w.x,p.w.y,p.w.z})scalar(b,d);
}
inline std::vector<uint8_t> encode(const Packet& p) {
    if(p.trackers.size()>MaxTrackers) return {};
    std::vector<uint8_t> b;u32(b,0x31425446);u32(b,1);u32(b,static_cast<uint32_t>(p.trackers.size()));
    u64(b,p.session);u64(b,p.sequence);u64(b,p.timeNs);putPose(b,p.head);
    for(auto& t:p.trackers){u32(b,t.chip);u32(b,static_cast<uint32_t>(t.role));putPose(b,t.pose);}return b;
}
struct Reader {
    const uint8_t* p;
    uint32_t u32(){uint32_t x=0;for(int i=0;i<4;++i)x|=static_cast<uint32_t>(*p++)<<(8*i);return x;}
    uint64_t u64(){uint64_t x=u32();return x|(static_cast<uint64_t>(u32())<<32);}
    double f(){uint32_t x=u32();float v;std::memcpy(&v,&x,4);return v;}
    bool pose(Pose& o){auto flags=u32();o.valid=(flags&1)!=0;o.connected=(flags&2)!=0;
        o.p={f(),f(),f()};o.q={f(),f(),f(),f()};o.v={f(),f(),f()};o.w={f(),f(),f()};
        if(flags>3||!validPose(o))return false;
        o.q=normalize(o.q);return true;}
};
inline bool decode(const uint8_t* data,size_t n,Packet& result) {
    if(n<92) return false;
    Reader r{data};if(r.u32()!=0x31425446||r.u32()!=1)return false;
    auto count=r.u32();if(count>MaxTrackers||n!=92+64*count)return false;
    Packet p;p.session=r.u64();p.sequence=r.u64();p.timeNs=r.u64();
    if(!p.session||!p.timeNs||!r.pose(p.head))return false;
    for(unsigned i=0;i<count;++i) {Tracker t;t.chip=r.u32();auto ro=r.u32();if(!t.chip||ro>4)return false;
        for(auto& other:p.trackers)if(other.chip==t.chip)return false;
        t.role=static_cast<Role>(ro);if(!r.pose(t.pose))return false;p.trackers.push_back(t);}
    result=std::move(p);return true;
}
struct Transform { Quat q; Vec p; };
inline Transform alignment(const Pose& pc,const Pose& frame) {Quat q=pc.q*conjugate(frame.q);return {q,pc.p-rotate(q,frame.p)};}
inline Pose transform(const Transform& t,Pose p) {p.p=rotate(t.q,p.p)+t.p;p.q=t.q*p.q;p.v=rotate(t.q,p.v);p.w=rotate(t.q,p.w);return p;}
struct Alignment {
    bool have=false;Transform value;double time=0;
    void update(const Pose& pc,const Pose& frame,double now) {
        if(!pc.valid||!frame.valid)return;
        auto t=alignment(pc,frame);
        double a=have?1-std::exp(-std::max(0.0,now-time)/0.25):1;
        value.q=blend(value.q,t.q,a);value.p=value.p+(t.p-value.p)*a;time=now;have=true;
    }
};
// Shared by DLL and standalone receiver. Different monotonic clock epochs never subtract directly.
struct Stream {
    Packet packet;bool have=false;double received=0,minOffset=0;uint64_t rejected=0;
    bool ingest(const uint8_t* b,size_t n,double now) {
        Packet p;if(!decode(b,n,p)){++rejected;return false;}
        if(have && p.session==packet.session && (p.sequence<=packet.sequence||p.timeNs<=packet.timeNs)){++rejected;return false;}
        if(have && p.session!=packet.session && now-received<=0.5){++rejected;return false;}
        double offset=now-static_cast<double>(p.timeNs)*1e-9;
        if(!have||p.session!=packet.session)minOffset=offset;else minOffset=std::min(minOffset,offset);
        packet=std::move(p);received=now;have=true;return true;
    }
    double age(double now,double baseLatency=0.005) const {
        return have?std::max(0.0,now-(static_cast<double>(packet.timeNs)*1e-9+minOffset))+baseLatency:1e9;
    }
    bool fresh(double now) const {return have&&now-received<=0.5&&age(now)<=0.5;}
};
inline Pose publication(const Stream& s,const Alignment& a,uint32_t chip,double now) {
    Pose p;bool found=false;
    if(s.have)for(const auto& t:s.packet.trackers)if(t.chip==chip){p=t.pose;found=true;break;}
    if(a.have)p=transform(a.value,p);
    if(!found||!s.fresh(now)||!s.packet.head.valid||!a.have)p.valid=p.connected=false;
    return p;
}
} // namespace bridge
