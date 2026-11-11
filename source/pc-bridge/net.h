#pragma once
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
using Socket=SOCKET;
inline void closeSocket(Socket s){closesocket(s);}
#else
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
using Socket=int;
inline void closeSocket(Socket s){close(s);}
#endif
#include "bridge.h"
#include <chrono>
#include <stdexcept>

namespace bridge {
inline double now(){return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();}
inline uint64_t nowNs(){return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());}
struct Network {
    Network(){
#ifdef _WIN32
        WSADATA d{};if(WSAStartup(MAKEWORD(2,2),&d))throw std::runtime_error("WSAStartup");
#endif
    }
    ~Network(){
#ifdef _WIN32
        WSACleanup();
#endif
    }
};
inline sockaddr_in endpoint(const std::string& target) {
    auto c=target.rfind(':');if(c==std::string::npos)throw std::runtime_error("expected IPv4 HOST:PORT");
    const auto port=target.substr(c+1);
    if(port.empty()||port.find_first_not_of("0123456789")!=std::string::npos||std::stoul(port)==0||std::stoul(port)>65535)throw std::runtime_error("invalid port");
    addrinfo hints{},*r=nullptr;hints.ai_family=AF_INET;hints.ai_socktype=SOCK_DGRAM;
    if(getaddrinfo(target.substr(0,c).c_str(),port.c_str(),&hints,&r))throw std::runtime_error("cannot resolve target");
    sockaddr_in out{};std::memcpy(&out,r->ai_addr,sizeof(out));freeaddrinfo(r);return out;
}
class Udp {
public:
    Socket socket_;
    Udp():socket_(::socket(AF_INET,SOCK_DGRAM,0)) {
        if(socket_==static_cast<Socket>(-1))throw std::runtime_error("UDP socket");
    }
    ~Udp(){closeSocket(socket_);}
    Udp(const Udp&)=delete;Udp& operator=(const Udp&)=delete;
    void bindTo(const std::string& address) {
        auto a=endpoint(address);if(::bind(socket_,reinterpret_cast<sockaddr*>(&a),sizeof(a)))throw std::runtime_error("UDP bind failed (port in use?)");
#ifdef _WIN32
        u_long one=1;if(ioctlsocket(socket_,FIONBIO,&one))throw std::runtime_error("nonblocking socket");
#else
        if(fcntl(socket_,F_SETFL,O_NONBLOCK)<0)throw std::runtime_error("nonblocking socket");
#endif
    }
    bool send(const sockaddr_in& target,const std::vector<uint8_t>& b) {
        return sendto(socket_,reinterpret_cast<const char*>(b.data()),static_cast<int>(b.size()),0,
            reinterpret_cast<const sockaddr*>(&target),sizeof(target))==static_cast<int>(b.size());
    }
};
// Bounded draining, one peer pinned until a 0.5 s outage. Invalid packets never refresh it.
// DLL and loopback executable call exactly this socket -> decoder -> Stream path.
class Receiver {
    Udp udp_;sockaddr_in peer_{};bool pinned_=false;
public:
    Stream stream;
    void bindTo(const std::string& address){udp_.bindTo(address);}
    unsigned poll() {
        unsigned accepted=0;
        for(unsigned i=0;i<256;++i) {
            uint8_t data[2048];sockaddr_in from{};
#ifdef _WIN32
            int size=sizeof(from);
#else
            socklen_t size=sizeof(from);
#endif
            int n=static_cast<int>(recvfrom(udp_.socket_,reinterpret_cast<char*>(data),sizeof(data),0,reinterpret_cast<sockaddr*>(&from),&size));
            if(n<0)break;
            const double t=now();
            if(pinned_&&stream.fresh(t)&&(peer_.sin_addr.s_addr!=from.sin_addr.s_addr||peer_.sin_port!=from.sin_port)){++stream.rejected;continue;}
            if(stream.ingest(data,static_cast<size_t>(n),t)){peer_=from;pinned_=true;++accepted;}
        }
        return accepted;
    }
};
}
