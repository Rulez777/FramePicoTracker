// Linux-only, read-only Tcl readiness probe; no radio or memory commands.
#include "tcl_transport.hpp"
#include <chrono>
#include <iostream>
#include <thread>
#ifndef _WIN32
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif
int main(int argc,char **argv) {
    try {
        if(argc!=3)throw std::runtime_error("usage: ota_prep_probe PORT OPENOCD_PID");
        const auto port=std::stoul(argv[1]);
        if(!port||port>65535)throw std::runtime_error("Invalid port");
#ifndef _WIN32
        if(std::string(argv[2])=="--free") {
            const int fd=socket(AF_INET,SOCK_STREAM,0);
            if(fd<0)throw std::runtime_error("Socket failed");
            sockaddr_in address{};address.sin_family=AF_INET;
            address.sin_addr.s_addr=htonl(INADDR_LOOPBACK);address.sin_port=htons(uint16_t(port));
            const int result=bind(fd,reinterpret_cast<sockaddr*>(&address),sizeof(address));
            close(fd);
            if(result)throw std::runtime_error("Tcl port is already occupied");
            return 0;
        }
#endif
        for(unsigned i=0;i<80;++i) {
#ifndef _WIN32
            if(kill(std::stoi(argv[2]),0))throw std::runtime_error("OpenOCD child exited");
#endif
            try {
                pico::TclSocket socket{uint16_t(port)};
                if(socket.command("expr {20+18}")=="38")return 0;
            }catch(const std::exception&){}
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        throw std::runtime_error("OpenOCD Tcl readiness timeout");
    }catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}
}
