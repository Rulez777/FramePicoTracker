#include "guard_evidence.hpp"
#include "pose_publication.hpp"
#include <cmath>
#include <cstring>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#ifndef _WIN32
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#endif
namespace pico {
OpticalPosePublisher::OpticalPosePublisher(uint16_t port) : port_(port) {
    if (!port) throw std::invalid_argument("Nonzero loopback destination required");
}
std::array<uint8_t,88> OpticalPoseFrame::encode(int64_t now) const {
    pico::guard_evidence::Scope guard_evidence_scope(26,"PTP2");
    auto check=[](bool ok) { if(!ok) throw std::invalid_argument("Invalid PTP2 pose"); };
    // Owner rule 2026-10-29: no 250 ms / future cut; only a missing or >1 s old stamp is invalid.
    check(chip && stamp>0 && (now<stamp || now-stamp<=1000000000));
    check(std::isfinite(quality) && quality>=0 && quality<=1 && (!valid || (connected && quality>0)));
    check(!battery || (std::isfinite(*battery) && *battery>=0 && *battery<=100));
    double norm=0; for(auto x:quaternion) norm+=x*x;
    check(norm>=.81 && norm<=1.21);
    std::array<double,15> values{};
    size_t n=0;
    for(auto x:position) values[n++]=x;
    for(auto x:quaternion) values[n++]=x;
    for(auto x:velocity) values[n++]=x;
    for(auto x:omega) values[n++]=x;
    for(size_t i=0;i<n;++i) check(std::isfinite(values[i]) && std::abs(values[i])<=1e6);
    values[n++]=battery.value_or(-1); values[n++]=quality;
    std::array<uint8_t,88> out{}; size_t at=0;
    auto le=[&](uint64_t x,size_t bytes) { for(size_t i=0;i<bytes;++i) { out[at++]=uint8_t(x&255); x>>=8; } };
    for(char c:std::string("PTP2")) out[at++]=uint8_t(c);
    le(chip,4); le(unsigned(connected)|(unsigned(valid)<<1)|(unsigned(bool(battery))<<2),4);
    le(sequence,8); le(uint64_t(stamp),8);
    static_assert(sizeof(float)==4 && std::numeric_limits<float>::is_iec559,"IEEE float32 wire required");
    for(double d:values) { float f=float(d); uint32_t bits; std::memcpy(&bits,&f,4); le(bits,4); }
    return out;
}
OpticalPosePublisher::~OpticalPosePublisher() {
#ifndef _WIN32
    if(fd_>=0) close(fd_);
#endif
}
bool OpticalPosePublisher::send(const OpticalPoseFrame &frame,int64_t now) {
    std::array<uint8_t,88> bytes{};
    // Independent final freshness barrier: an invalid/stale pose is logged and dropped.
    try { bytes=frame.encode(now); }
    catch(const std::invalid_argument &) {
        if(++dropped_%30==1) std::cerr<<"pose_publication_drop reason=\"Invalid PTP2 pose\" count="<<dropped_<<'\n';
        return false;
    }
#ifdef _WIN32
    (void)bytes; return false;
#else
    if(fd_<0) fd_=socket(AF_INET,SOCK_DGRAM|SOCK_CLOEXEC,0);
    if(fd_<0) return false;
    sockaddr_in a{}; a.sin_family=AF_INET; a.sin_port=htons(port_); a.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
    return sendto(fd_,bytes.data(),bytes.size(),MSG_DONTWAIT,reinterpret_cast<sockaddr *>(&a),sizeof(a))==88;
#endif
}
} // namespace pico


#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_publication, "publication");
