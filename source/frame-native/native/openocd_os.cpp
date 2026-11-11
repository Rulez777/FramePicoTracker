#include "openocd_process.hpp"
#include "linux_listener_evidence.hpp"
#include <stdexcept>
#ifndef _WIN32
#include <cerrno>
#include <chrono>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <spawn.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <thread>
#include <unistd.h>
extern char** environ;
namespace pico {
namespace {
struct State {
    int log=-1;
    pid_t pid=-1;
    uint64_t start=0;
    std::string executable;
    std::optional<int> status;
    std::unique_ptr<TclSocket> socket;
    ~State(){if(log>=0)::close(log);} // no implicit owner termination
    std::optional<int> poll_exit(){
        if(status)return status;
        if(pid<0)throw std::runtime_error("OpenOCD not spawned");
        int s=0;auto r=waitpid(pid,&s,WNOHANG);
        if(r<0&&errno!=EINTR)throw std::runtime_error("OpenOCD waitpid failed");
        if(r==pid)status=WIFEXITED(s)?WEXITSTATUS(s):128+WTERMSIG(s);
        return status;
    }
    void signal(int sig){if(!poll_exit()&&::kill(pid,sig)&&errno!=ESRCH)throw std::runtime_error("OpenOCD signal failed");}
};
bool listener(){
    int s=socket(AF_INET,SOCK_STREAM|SOCK_CLOEXEC|SOCK_NONBLOCK,0);
    if(s<0)throw std::runtime_error("OpenOCD probe socket failed");
    sockaddr_in a{};a.sin_family=AF_INET;a.sin_port=htons(6666);a.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
    auto r=::connect(s,reinterpret_cast<sockaddr*>(&a),sizeof(a));int error=errno;
    if(r&&error==EINPROGRESS){pollfd p{s,POLLOUT,0};r=::poll(&p,1,200);
        if(r<=0){::close(s);throw std::runtime_error("OpenOCD listener probe unresolved");}
        socklen_t n=sizeof(error);if(getsockopt(s,SOL_SOCKET,SO_ERROR,&error,&n)){::close(s);throw std::runtime_error("OpenOCD listener probe failed");}
    }else if(!r)error=0;
    ::close(s);
    if(!error)return true;
    if(error==ECONNREFUSED)return false;
    throw std::runtime_error("OpenOCD listener state unknown");
}
}
OpenOcdHooks linux_openocd_hooks(const std::string& log,const std::string& executable){
    if(executable.empty()||executable.front()!='/'||executable.find('\0')!=std::string::npos)
        throw std::runtime_error("Reviewed absolute OpenOCD executable required");
    auto s=std::make_shared<State>();OpenOcdHooks h;
    s->executable=std::filesystem::canonical(executable).string();
    h.now=[]{return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();};
    h.sleep=[](int64_t ns){std::this_thread::sleep_for(std::chrono::nanoseconds(ns));};
    h.listener_present=listener;
    h.spawn=[s,log](const std::vector<std::string>& args){
        s->log=::open(log.c_str(),O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC|O_NOFOLLOW,0600);
        if(s->log<0)throw std::runtime_error("Cannot exclusively create OpenOCD log");
        if(s->log<3){int fd=fcntl(s->log,F_DUPFD_CLOEXEC,3);if(fd<0)throw std::runtime_error("OpenOCD log descriptor failed");
        ::close(s->log);s->log=fd;}
        auto command=args;command[0]=s->executable;
        std::vector<char*> av;for(auto& a:command)av.push_back(a.data());
        av.push_back(nullptr);
        posix_spawn_file_actions_t actions;
        if(posix_spawn_file_actions_init(&actions))throw std::runtime_error("OpenOCD spawn actions failed");
        int error=posix_spawn_file_actions_adddup2(&actions,s->log,STDOUT_FILENO);
        if(!error)error=posix_spawn_file_actions_adddup2(&actions,s->log,STDERR_FILENO);
        if(!error)error=posix_spawn_file_actions_addclose(&actions,s->log);
        if(!error)error=posix_spawn(&s->pid,av[0],&actions,nullptr,av.data(),environ);
        posix_spawn_file_actions_destroy(&actions);
        if(error)throw std::runtime_error("OpenOCD spawn failed");
    };
    h.poll=[s]{return s->poll_exit();};
    h.connect=[s]{if(!listener())return false;
    s->socket=std::make_unique<TclSocket>(6666);return true;};
    h.command=[s](const std::string& c){return s->socket->command(c);};
    h.disconnect=[s]{s->socket.reset();};
    h.child_pid=[s]{return int64_t(s->pid);};
    h.verify_attribution=[s]{
        if(s->poll_exit())throw std::runtime_error("OpenOCD direct child exited");
        // The direct child remains unreaped, so this PID cannot have been reused.
        if(!s->start)s->start=roy_process_start(roy_proc_read("/proc/"+std::to_string(s->pid)+"/stat"));
        if(!s->start||
           std::filesystem::read_symlink("/proc/"+std::to_string(s->pid)+"/exe").string()!=s->executable)
            throw std::runtime_error("OpenOCD direct child identity changed");
        (void)linux_child_listener(s->pid,s->start);
    };
    h.terminate=[s]{s->signal(SIGTERM);};h.kill=[s]{s->signal(SIGKILL);};
    return h;
}
}
#else
namespace pico {OpenOcdHooks linux_openocd_hooks(const std::string&,const std::string&){throw std::runtime_error("Linux OpenOCD process required");}}
#endif

#include "live_implementation_registry.hpp"
#ifndef _WIN32
PICO_LIVE_IMPLEMENTATION(pico_impl_openocd, "openocd");
#endif
