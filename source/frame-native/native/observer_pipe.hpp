#pragma once
#include "live_policy.hpp"
#ifndef _WIN32
#include <cerrno>
#include <fcntl.h>
#include <poll.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <memory>
#include <stdexcept>
#include <iostream>
namespace pico {
// Adopt duplicates of an already nonblocking pipe and launch-bound pidfds.
// No PID polling, background reader, shell, process termination or spawn here.
// The launcher must retain its observer/parent attribution and driver proof.
class ObserverPipe {
    int pipe_=-1,child_=-1,parent_=-1;
    static int copy(int fd){int out=fcntl(fd,F_DUPFD_CLOEXEC,3);if(out<0)throw std::runtime_error("Observer fd duplicate failed");
    return out;}
    static bool alive(int fd){
        pollfd p{fd,POLLIN,0};
        int n=poll(&p,1,0);
        if(n<0){std::cerr<<"Observer pidfd poll failed; waiting for observer\n";return false;}
    return n==0;
    }
    ObserverRead read(size_t limit){
        if(limit==0||limit>4096)throw std::runtime_error("Observer read bound invalid");
        std::string bytes(limit,'\0');auto n=::read(pipe_,&bytes[0],limit);
        if(n<0){if(errno==EAGAIN||errno==EWOULDBLOCK||errno==EINTR)return {};
        std::cerr<<"Observer pipe read failed; waiting for observer\n";return {};}
        bytes.resize(size_t(n));return {std::move(bytes),n==0};
    }
public:
    ObserverPipe(int pipe,int child_pidfd,int parent_pidfd){
        struct stat st{};auto flags=fcntl(pipe,F_GETFL);
        if(flags<0||!(flags&O_NONBLOCK)||(flags&O_ACCMODE)!=O_RDONLY||fstat(pipe,&st)||!S_ISFIFO(st.st_mode))
            throw std::runtime_error("Observer requires nonblocking read pipe");
        // Validate actual live pidfds, rejecting ordinary descriptors.
        // Signal 0 probes existence only. EPERM means the process exists but belongs to another user (the observer
        // runs as root via runuser, exactly as the Python launches it); only ESRCH/EBADF/EINVAL mean "not a live pidfd".
        auto live=[](int pidfd){
            if(!syscall(SYS_pidfd_send_signal,pidfd,0,nullptr,0))return true;
            return errno==EPERM;
        };
        if(!live(child_pidfd)||!live(parent_pidfd))
            throw std::runtime_error("Observer live child/parent pidfds required: errno="+std::to_string(errno));
        try{
            pipe_=copy(pipe);
            // dup shares O_NONBLOCK flags with the launcher's open description.
            // Reopen the pinned pipe to own flags independently, then verify
            // the same pipe inode before accepting any bytes.
            auto path=std::string("/proc/self/fd/")+std::to_string(pipe_);
            int isolated=::open(path.c_str(),O_RDONLY|O_NONBLOCK|O_CLOEXEC);
            if(isolated<0)throw std::runtime_error("Observer isolated pipe open failed");
            struct stat bound{};
            if(fstat(isolated,&bound)||bound.st_dev!=st.st_dev||bound.st_ino!=st.st_ino){
                ::close(isolated);throw std::runtime_error("Observer pipe identity changed");
            }
            ::close(pipe_);pipe_=isolated;
            child_=copy(child_pidfd);parent_=copy(parent_pidfd);
        }
        catch(...){close();throw;}
    }
    ObserverPipe(const ObserverPipe&)=delete;
    ObserverPipe& operator=(const ObserverPipe&)=delete;
    ~ObserverPipe(){close();}
    void close(){for(auto fd:{pipe_,child_,parent_})if(fd>=0)::close(fd);
    pipe_=child_=parent_=-1;}
    static ObserverHooks hooks(int pipe,int child,int parent,std::function<int64_t()> now,std::function<bool()> cancel){
        auto state=std::make_shared<ObserverPipe>(pipe,child,parent);
        return {[state](size_t n){return state->read(n);},[state]{return alive(state->child_);},
            [state]{return alive(state->parent_);},std::move(cancel),std::move(now)};
    }
};
}
#endif
