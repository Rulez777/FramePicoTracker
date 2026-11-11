#pragma once
#include "fake_openocd.hpp"
#include <thread>
#include <exception>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif
namespace pico {
// Private loopback endpoint: never accepts a caller-specified debugger port.
class SyntheticTclServer {
#ifdef _WIN32
    using Socket=SOCKET;
    static void close(Socket s){closesocket(s);}
#else
    using Socket=int;
    static void close(Socket s){::close(s);}
#endif
    Socket listener_=Socket(-1);
    std::thread thread_;
    std::exception_ptr error_;
public:
    FakeOpenOcd backend;
    uint16_t port=0;
    SyntheticTclServer(){
#ifdef _WIN32
        WSADATA w{};if(WSAStartup(MAKEWORD(2,2),&w))throw std::runtime_error("WSA startup");
#endif
        listener_=socket(AF_INET,SOCK_STREAM,0);if(listener_==Socket(-1))throw std::runtime_error("fake socket");
        sockaddr_in a{};a.sin_family=AF_INET;a.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
        if(bind(listener_,reinterpret_cast<sockaddr*>(&a),sizeof(a))||listen(listener_,1))throw std::runtime_error("fake bind/listen");
#ifdef _WIN32
        int size=sizeof(a);
#else
        socklen_t size=sizeof(a);
#endif
        if(getsockname(listener_,reinterpret_cast<sockaddr*>(&a),&size))throw std::runtime_error("fake port");
        port=ntohs(a.sin_port);
    }
    void start(){thread_=std::thread([this]{Socket s=accept(listener_,nullptr,nullptr);if(s==Socket(-1)){error_=std::make_exception_ptr(std::runtime_error("fake accept"));return;}
        try{std::string request;char c;for(;;){auto n=recv(s,&c,1,0);if(!n)break;
        if(n<0)throw std::runtime_error("fake recv");
        if(c!='\x1a'){request+=c;if(request.size()>1024*1024)throw std::runtime_error("fake request cap");
        continue;}
            auto reply=backend.command(request)+'\x1a';request.clear();size_t offset=0;while(offset<reply.size()){auto sent=::send(s,reply.data()+offset,int(reply.size()-offset),
#ifdef _WIN32
0
#else
MSG_NOSIGNAL
#endif
);if(sent<=0)throw std::runtime_error("fake send");
offset+=size_t(sent);}}
        }catch(...){error_=std::current_exception();}close(s);});}
    void finish(){if(thread_.joinable())thread_.join();
    if(error_)std::rethrow_exception(error_);}
    ~SyntheticTclServer(){close(listener_);if(thread_.joinable())thread_.join();}
};
}
