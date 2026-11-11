#pragma once
#include "observer_pipe.hpp"
#include "observer_launch_files.hpp"
#ifndef _WIN32
#include <spawn.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <filesystem>
extern char **environ;
namespace pico {
// Matches DriverObservation's direct argv. Root ControllerGuard's runuser
// boundary requires separate attribution and refuses here. No shell, Python,
// extra watcher arguments or pre-existing process.
class ObserverProcess {
    pid_t pid_=-1;
    int child_=-1,parent_=-1,pipe_=-1,proof_=-1;
    std::unique_ptr<ObserverLaunchFiles> files_;
    std::vector<std::string> argv_;
    static int64_t now(){timespec t{};if(clock_gettime(CLOCK_MONOTONIC,&t))throw std::runtime_error("Observer clock failed");
        return int64_t(t.tv_sec)*1000000000+t.tv_nsec;}
    void record(const std::string& bytes){
        size_t at=0;
        while(at<bytes.size()){
            auto n=::write(proof_,bytes.data()+at,bytes.size()-at);
            if(n<0&&errno==EINTR)continue;
            if(n<=0)throw std::runtime_error("Driver proof write failed");
            at+=size_t(n);
        }
        if(fsync(proof_))throw std::runtime_error("Driver proof flush failed");
    }
    bool reap_until(int64_t deadline){
        while(now()<deadline){
            auto r=waitpid(pid_,nullptr,WNOHANG);
            if(r==pid_){pid_=-1;return true;}
            if(r<0&&errno!=EINTR)throw std::runtime_error("Observer reap failed");
            timespec pause{0,10000000};nanosleep(&pause,nullptr);
        }
        return false;
    }
public:
    ObserverProcess(const std::filesystem::path& binary,const std::filesystem::path& proof,
                    const std::string& reviewed_sha,bool controller_guard=false){
        // runuser is a separate executable/privilege boundary; do not silently
        // treat its image as the reviewed tracking_watch. Direct DriverObservation
        // is the production adapter here. Wrapper admission remains blocked.
        if(controller_guard&&geteuid()==0)
            throw std::runtime_error("Sealed ControllerGuard runuser attribution unavailable");
        files_=std::make_unique<ObserverLaunchFiles>(binary,proof,reviewed_sha);
        argv_={binary.string()};
        proof_=files_->proof();
        int fds[2]{-1,-1};int nullfd=-1;
        posix_spawn_file_actions_t actions;bool initialized=false;
        try{
            if(pipe2(fds,O_CLOEXEC))throw std::runtime_error("Observer pipe creation failed");
            nullfd=::open("/dev/null",O_RDWR|O_CLOEXEC);
            if(nullfd<0||posix_spawn_file_actions_init(&actions))throw std::runtime_error("Observer launch actions failed");
            initialized=true;
            if(posix_spawn_file_actions_adddup2(&actions,fds[1],STDOUT_FILENO)||
               posix_spawn_file_actions_adddup2(&actions,nullfd,STDERR_FILENO)||
               posix_spawn_file_actions_adddup2(&actions,nullfd,STDIN_FILENO))throw std::runtime_error("Observer launch redirection failed");
            std::vector<char*> args;for(auto& a:argv_)args.push_back(a.data());
            args.push_back(nullptr);
            const auto image=files_->exec_path();
            const int error=posix_spawn(&pid_,image.c_str(),&actions,nullptr,args.data(),environ);
            if(error){pid_=-1;throw std::runtime_error("Observer exec failed");}
            files_->verify_child(pid_);
            child_=int(syscall(SYS_pidfd_open,pid_,0));parent_=int(syscall(SYS_pidfd_open,getppid(),0));
            if(child_<0||parent_<0)throw std::runtime_error("Observer launch pidfd binding failed");
            if(fcntl(fds[0],F_SETFL,O_NONBLOCK))throw std::runtime_error("Observer nonblocking pipe failed");
            pipe_=fds[0];fds[0]=-1;
            posix_spawn_file_actions_destroy(&actions);initialized=false;
            ::close(fds[1]);fds[1]=-1;::close(nullfd);nullfd=-1;
        }catch(...){
            if(initialized)posix_spawn_file_actions_destroy(&actions);
            for(auto fd:{fds[0],fds[1],nullfd})if(fd>=0)::close(fd);
            // Only this newly launched observer, which never owns hardware.
            if(pid_>0){::kill(pid_,SIGKILL);while(waitpid(pid_,nullptr,0)<0&&errno==EINTR){}pid_=-1;}
            for(auto fd:{pipe_,child_,parent_})if(fd>=0)::close(fd);
            throw;
        }
    }
    ObserverProcess(const ObserverProcess&)=delete;
    ObserverProcess& operator=(const ObserverProcess&)=delete;
    ~ObserverProcess(){for(auto fd:{pipe_,child_,parent_})if(fd>=0)::close(fd);}
    ObserverHooks hooks(std::function<bool()> cancelled){
        auto h=ObserverPipe::hooks(pipe_,child_,parent_,now,std::move(cancelled));
        auto read=h.read;
        h.read=[this,read](size_t n){
            files_->verify_child(pid_);
            auto r=read(n);
            if(!r.bytes.empty())
                record(r.bytes);
            return r;
        };
        return h;
    }
    void close(){
        if(pid_<=0)return;
        if(syscall(SYS_pidfd_send_signal,child_,SIGTERM,nullptr,0)&&errno!=ESRCH)
            throw std::runtime_error("Observer terminate failed");
        if(reap_until(now()+2000000000LL))return;
        if(syscall(SYS_pidfd_send_signal,child_,SIGKILL,nullptr,0)&&errno!=ESRCH)
            throw std::runtime_error("Observer kill failed");
        if(!reap_until(now()+2000000000LL))throw std::runtime_error("Observer reap deadline; retain owner");
    }
    const std::vector<std::string>& argv()const{return argv_;}
};
}
#endif

#include "live_implementation_registry.hpp"
#ifndef _WIN32
PICO_LIVE_IMPLEMENTATION(pico_impl_observer, "observer");
#endif
