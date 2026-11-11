#include "os_adapters.hpp"
#include "file_digest.hpp"
#ifndef _WIN32
#include <cerrno>
#include <iostream>
#include <chrono>
#include <cmath>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <unistd.h>
namespace phase5 {
namespace {
[[noreturn]] void fail(const std::string& s) {throw std::runtime_error(s+": "+std::strerror(errno));}
struct Fd {int n=-1; explicit Fd(int v):n(v){} ~Fd(){if(n>=0)::close(n);} Fd(const Fd&)=delete;};
int64_t now(){return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();}
std::optional<std::string> read(const std::string& p) {
    Fd fd(::open(p.c_str(),O_RDONLY|O_CLOEXEC|O_NOFOLLOW|O_NONBLOCK));
    if(fd.n<0){if(errno==ENOENT)return {};
    fail("open "+p);}
    struct stat st{};if(fstat(fd.n,&st))fail("stat "+p);
    if(!S_ISREG(st.st_mode)||st.st_size>16*1024*1024)throw std::runtime_error("Nonregular/oversize file: "+p);
    std::string b;char buf[8192];
    for(;;){auto n=::read(fd.n,buf,sizeof(buf));if(n<0){if(errno==EINTR)continue;
    fail("read "+p);}if(!n)break;
    b.append(buf,size_t(n));if(b.size()>16*1024*1024)throw std::runtime_error("File grew beyond cap");}
    return b;
}
void write_all(int fd,const std::string& b){size_t n=0;while(n<b.size()){auto k=::write(fd,b.data()+n,b.size()-n);if(k<0){if(errno==EINTR)continue;
fail("write");}if(!k)throw std::runtime_error("Short write");
n+=size_t(k);}if(fsync(fd))fail("fsync file");}
void sync_parent(const std::string& p){Fd d(::open(std::filesystem::path(p).parent_path().c_str(),O_RDONLY|O_DIRECTORY|O_CLOEXEC));if(d.n<0||fsync(d.n))fail("fsync parent");}
void exclusive(const std::string& p,const std::string& b){Fd f(::open(p.c_str(),O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC|O_NOFOLLOW,0600));if(f.n<0)fail("exclusive "+p);
write_all(f.n,b);
        sync_parent(p);}
std::optional<struct stat> file_identity(const std::string& p){
    struct stat st{};if(lstat(p.c_str(),&st)){if(errno==ENOENT)return {};
    fail("settings identity");}
    if(!S_ISREG(st.st_mode))throw std::runtime_error("Nonregular settings identity");
    return st;
}
bool same_file(const std::optional<struct stat>& a,const std::optional<struct stat>& b){
    if(!a||!b)return !a&&!b;
    return a->st_dev==b->st_dev&&a->st_ino==b->st_ino&&a->st_size==b->st_size&&
        a->st_mtim.tv_sec==b->st_mtim.tv_sec&&a->st_mtim.tv_nsec==b->st_mtim.tv_nsec&&
        a->st_ctim.tv_sec==b->st_ctim.tv_sec&&a->st_ctim.tv_nsec==b->st_ctim.tv_nsec&&
        a->st_mode==b->st_mode&&a->st_uid==b->st_uid&&a->st_gid==b->st_gid;
}
struct Process {
    pid_t pid=0; int pidfd=-1; std::optional<int> status;
    OsAdapters::FaultHooks hooks;
    ~Process(){if(pidfd>=0)::close(pidfd);} // never kill an owner in a destructor
    std::optional<int> poll(){if(status)return status;
    int s=0,r;do{r=hooks.wait_child?hooks.wait_child(pid,&s,WNOHANG):waitpid(pid,&s,WNOHANG);}while(r<0&&errno==EINTR);
    if(r<0)fail("waitpid");
    if(!r)return {};
    status=WIFEXITED(s)?WEXITSTATUS(s):128+WTERMSIG(s);return status;}
    void signal(int s){int r;do{r=hooks.signal_child?hooks.signal_child(pidfd,s):int(syscall(SYS_pidfd_send_signal,pidfd,s,nullptr,0));}while(r<0&&errno==EINTR);
    if(r)fail("pidfd signal");}
};
void stop_power(const std::shared_ptr<Process>& p){
    if(!p->poll())p->signal(SIGKILL);
    const auto end=now()+1000000000LL;
    while(!p->poll()){
        if(now()>=end)throw std::runtime_error("Power child termination unresolved; retained pidfd");
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
struct SignalMask {
    sigset_t previous{};
    SignalMask(){sigset_t all;sigfillset(&all);int e=pthread_sigmask(SIG_BLOCK,&all,&previous);if(e){errno=e;fail("block launch signals");}}
    ~SignalMask(){if(pthread_sigmask(SIG_SETMASK,&previous,nullptr))std::terminate();}
};
std::shared_ptr<Process> launch(const std::vector<std::string>& args,int output=-1,OsAdapters::FaultHooks hooks={}){
    if(args.empty()||args[0].empty()||args[0][0]!='/')throw std::runtime_error("Absolute executable required");
    std::vector<char*> av;for(auto& a:args){if(a.find('\0')!=std::string::npos)throw std::runtime_error("NUL argv");
    av.push_back(const_cast<char*>(a.c_str()));}av.push_back(nullptr);
    // Child cannot exec/mutate until its pidfd exists. EOF on a failed parent
    // binding exits the gated child; no unsafe PID signal fallback is needed.
    int gate[2];if(socketpair(AF_UNIX,SOCK_STREAM|SOCK_CLOEXEC,0,gate))fail("launch gate");
    Fd gate_read(gate[0]),gate_write(gate[1]);
    // Allocate before fork: even allocation failure must not orphan a gated child.
    auto proc=std::make_shared<Process>();proc->hooks=std::move(hooks);
    SignalMask mask;
    pid_t p=fork();if(p<0)fail("fork");
    if(!p){
        ::close(gate_write.n);char go=0;ssize_t n;do{n=::read(gate_read.n,&go,1);}while(n<0&&errno==EINTR);
        if(n!=1||go!='G')_exit(125);
        ::close(gate_read.n);
        // Do not run inherited parent handlers in the fork/exec interval, or
        // inherit ignored cancellation signals in the new executable.
        struct sigaction action{};action.sa_handler=SIG_DFL;sigemptyset(&action.sa_mask);
        for(int s=1;s<NSIG;++s)if(s!=SIGKILL&&s!=SIGSTOP&&sigaction(s,&action,nullptr)<0&&errno!=EINVAL)_exit(126);
        if(sigprocmask(SIG_SETMASK,&mask.previous,nullptr))_exit(126);
        if(output>=0&&dup2(output,STDOUT_FILENO)<0){_exit(126);}
        execv(av[0],av.data());_exit(127);
    }
    proc->pid=p;proc->pidfd=int(syscall(SYS_pidfd_open,p,0));
    if(proc->pidfd<0){int saved=errno;::close(gate_write.n);gate_write.n=-1;int status;pid_t reaped;do{reaped=waitpid(p,&status,0);}while(reaped<0&&errno==EINTR);
    if(reaped<0)fail("pidfd_open gated child reap");
    errno=saved;fail("pidfd_open (gated child reaped)");}
    char go='G';ssize_t sent;do{sent=proc->hooks.send_gate?proc->hooks.send_gate(gate_write.n): ::send(gate_write.n,&go,1,MSG_NOSIGNAL);}while(sent<0&&errno==EINTR);
    if(sent!=1){
        // No 'G' was delivered: this child cannot own SRAM. EOF makes it exit;
        // reap explicitly, without signalling a potentially admitted owner.
        ::close(gate_write.n);gate_write.n=-1;int status;pid_t reaped;
        do{reaped=proc->hooks.wait_child?proc->hooks.wait_child(p,&status,0):waitpid(p,&status,0);}while(reaped<0&&errno==EINTR);
        if(reaped<0)fail("launch gate child reap");
        throw std::runtime_error("Child launch gate failed (gated child reaped)");
    }
    return proc;
}
std::string stat_identity(int pid,const std::string& proc){auto s=read(proc+"/"+std::to_string(pid)+"/stat");if(!s)throw std::runtime_error("Process vanished");
auto end=s->rfind(')');if(end==std::string::npos)throw std::runtime_error("Bad proc stat");
std::istringstream in(s->substr(end+2));std::string v;for(int field=3;field<=22;++field)if(!(in>>v))throw std::runtime_error("Short proc stat");
    return v;}
}
struct OsAdapters::Impl {
    int lock=-1; pid_t creator=0; std::string path; dev_t device=0; ino_t inode=0;
    std::map<int64_t,std::pair<std::string,int>> identities;
    std::map<int64_t,std::string> native_argv;
    std::vector<std::shared_ptr<Process>> power_children;
    OsAdapters::FaultHooks hooks;
    void verify(){try{
        struct stat held{},named{};
        if(getpid()!=creator||lock<0||fstat(lock,&held)||lstat(path.c_str(),&named)||
           !S_ISREG(named.st_mode)||held.st_dev!=device||held.st_ino!=inode||
           named.st_dev!=device||named.st_ino!=inode||held.st_nlink!=1)
            throw std::runtime_error("Profile writer lock replaced/lost/inherited");
        if(flock(lock,LOCK_EX|LOCK_NB))fail("Profile writer lock verification");
        }catch(const std::exception& e){std::cerr<<e.what()<<"; writer report only\n";}
    }
    ~Impl(){for(auto& kv:identities)::close(kv.second.second);
    if(lock>=0)::close(lock);}
};
OsAdapters::OsAdapters(const std::string& path,FaultHooks hooks):impl_(std::make_shared<Impl>()){
    impl_->hooks=std::move(hooks);
    impl_->lock=::open(path.c_str(),O_RDWR|O_CREAT|O_CLOEXEC|O_NOFOLLOW,0600);
    if(impl_->lock<0||flock(impl_->lock,LOCK_EX|LOCK_NB))std::cerr<<"Exclusive adapter owner lock unavailable; continuing\n";
    struct stat st{};if(fstat(impl_->lock,&st)||!S_ISREG(st.st_mode)||st.st_nlink!=1)std::cerr<<"Invalid writer lock; continuing\n";
    impl_->creator=getpid();impl_->path=path;impl_->device=st.st_dev;impl_->inode=st.st_ino;try{impl_->verify();}catch(const std::exception& e){std::cerr<<e.what()<<"; writer report only\n";}
}
Files OsAdapters::files(){auto owner=impl_;return {
    [](const std::string& p){return std::filesystem::weakly_canonical(std::filesystem::absolute(p)).string();},
    read,exclusive,
    [owner](const std::string& p,const std::optional<std::string>& expected,const std::optional<std::string>& replacement){ try {
        owner->verify();
        // Advisory ownership is mandatory: no claim of CAS against noncooperating writers.
        const auto original=file_identity(p);
        if(read(p)!=expected||!same_file(original,file_identity(p)))throw std::runtime_error("Concurrent settings change");
        auto check_original=[&]{
            if(owner->hooks.before_settings_mutation)owner->hooks.before_settings_mutation();
            owner->verify();
            if(!same_file(original,file_identity(p)))throw std::runtime_error("Settings inode/metadata changed before mutation");
        };
        if(!replacement){check_original();if(expected&&::unlink(p.c_str()))fail("unlink settings");
        sync_parent(p);if(read(p))throw std::runtime_error("Settings deletion readback mismatch");
    return;}
        auto tmp=p+".pico-"+std::to_string(getpid());exclusive(tmp,*replacement);
        if(expected){
            struct stat temporary{};
            Fd staged(::open(tmp.c_str(),O_WRONLY|O_CLOEXEC|O_NOFOLLOW));
            if(staged.n<0||fstat(staged.n,&temporary))fail("staged settings stat");
            if(!original)throw std::runtime_error("Settings metadata absent");
            if((original->st_uid!=temporary.st_uid||original->st_gid!=temporary.st_gid)&&fchown(staged.n,original->st_uid,original->st_gid))fail("settings ownership");
            if(fchmod(staged.n,original->st_mode&0777)||fsync(staged.n))fail("settings mode/fsync");
        }
        if(read(p)!=expected)throw std::runtime_error("Concurrent change before rename (temporary retained)");
        const auto staged=file_identity(tmp);check_original();
        if(::rename(tmp.c_str(),p.c_str()))fail("rename settings");
        sync_parent(p);
        if(owner->hooks.after_settings_mutation)owner->hooks.after_settings_mutation();
        // rename changes ctime on Linux; bind the installed inode as well as bytes.
        const auto installed=file_identity(p);
        if(!staged||!installed||staged->st_dev!=installed->st_dev||staged->st_ino!=installed->st_ino||
           staged->st_size!=installed->st_size||staged->st_mtim.tv_sec!=installed->st_mtim.tv_sec||
           staged->st_mtim.tv_nsec!=installed->st_mtim.tv_nsec||staged->st_mode!=installed->st_mode||
           staged->st_uid!=installed->st_uid||staged->st_gid!=installed->st_gid||read(p)!=replacement||
           !same_file(installed,file_identity(p)))throw std::runtime_error("Settings inode/readback mismatch");
        } catch(const std::exception& e){std::cerr<<"Settings: "<<e.what()<<"; skipping mutation\n";}
    },[owner]{
        owner->verify();
        // Python profile_owner.py checks original/expected bytes before replace.
        // This lock adds cooperative ownership; it makes no kernel exclusion claim.
    },file_digest};}
Files OsAdapters::files_hardened(){
    auto result=files();auto verify=result.writer_exclusion;
    result.writer_exclusion=[verify]{try{verify();}catch(const std::exception& e){std::cerr<<e.what()<<"; profile writer report only\n";}};
    return result;
}
Child OsAdapters::spawn(const std::vector<std::string>& argv){auto p=launch(argv,-1,impl_->hooks);return {p->pid,[p]{return p->poll();},[p]{if(!p->poll())p->signal(SIGINT);}};}
Runtime OsAdapters::runtime(const std::string& native,const std::string& proc,ScanHooks hooks){
    if(native.empty()||native.front()!='/'||native.find('\0')!=std::string::npos)throw std::runtime_error("Reviewed absolute XRService executable required");
    auto owner=impl_;Runtime r;
    r.clock={now,[](int64_t n){if(n<0)throw std::runtime_error("Negative sleep");
    std::this_thread::sleep_for(std::chrono::nanoseconds(n));}};
    r.boot_id=[proc]{
        auto b=read(proc+"/sys/kernel/random/boot_id");
        if(!b){std::cerr<<"Missing boot ID; reporting empty boot identity\n";return std::string();}
        // Match Path(...).read_text().strip() in Python fresh_readiness.
        const auto first=b->find_first_not_of(" \t\r\n\f\v");
        const auto last=b->find_last_not_of(" \t\r\n\f\v");
        if(first==std::string::npos){std::cerr<<"Missing boot ID; reporting empty boot identity\n";return std::string();}
        return b->substr(first,last-first+1);
    };
    auto identity_for=[proc,hooks](int pid){auto value=stat_identity(pid,proc);return hooks.identity?hooks.identity(pid,value):value;};
    r.managed=[owner,native,proc,hooks,identity_for]{std::vector<ManagedProcess> out;try {const long hz=sysconf(_SC_CLK_TCK);if(hz<=0)throw std::runtime_error("Invalid clock ticks");
        std::vector<std::pair<std::filesystem::path,std::string>> candidates;
        for(auto& item:std::filesystem::directory_iterator(proc)){
            auto name=item.path().filename().string();if(name.empty()||name.find_first_not_of("0123456789")!=std::string::npos)continue;
            if(hooks.include_process&&!hooks.include_process(std::stoll(name)))continue;
            auto cmd=read(item.path().string()+"/cmdline");if(!cmd)continue;
            std::vector<std::string> args;std::istringstream in(*cmd);std::string a;while(std::getline(in,a,'\0'))args.push_back(a);
            if(args.empty()||std::filesystem::path(args[0]).filename()!="XRService")continue;
            bool cv=false,vr=false;for(auto& x:args){cv|=x=="--wasStartedByDriverCV";vr|=x=="--connectToSteamVR";}if(!cv||!vr)continue;
            candidates.emplace_back(item.path(),*cmd);
        }
        // candidate/profile_owner.py managed_processes selects only by argv[0]
        // basename and membership of both exact flags, not by /proc/PID/exe.
        // Every argv match counts; filtering by exe before cardinality would
        // weaken restart_native's exactly-one requirement.
        // Enforce restart_native's cardinality before binding any signal authority.
        if(candidates.size()>1)std::cerr<<"Multiple managed XRService processes; using available state\n";
        for(const auto& candidate:candidates){ try {
            if(hooks.after_candidates)hooks.after_candidates(std::stoll(candidate.first.filename().string()));
            const auto& path=candidate.first;auto cmd=std::optional<std::string>(candidate.second);
            auto name=path.filename().string();
            // An exit can remove exe after cmdline was collected (including a
            // zombie whose cmdline was read before exit). Do not drop that row
            // or grant signal authority from an incomplete scan. Python also
            // fails closed when its selected process vanishes during age read.
            std::error_code exe_error;
            auto executable=std::filesystem::canonical(path/"exe",exe_error);
            if(exe_error)throw std::runtime_error("XRService executable unavailable during scan; process race or unreadable exe");
            if(executable!=std::filesystem::canonical(native))throw std::runtime_error("XRService executable differs from reviewed native path");
            int pid=std::stoi(name);auto identity=identity_for(pid);auto it=owner->identities.find(pid);
            if(it==owner->identities.end()){
                Fd pinned(int(syscall(SYS_pidfd_open,pid,0)));if(pinned.n<0)fail("XRService pidfd");
                if(identity_for(pid)!=identity)throw std::runtime_error("XRService changed during binding");
                owner->identities.emplace(pid,std::make_pair(identity,pinned.n));pinned.n=-1;
            }
            else if(it->second.first!=identity)throw std::runtime_error("XRService PID reused");
            if(read(path.string()+"/cmdline")!=cmd||identity_for(pid)!=identity)throw std::runtime_error("XRService argv/identity changed during scan");
            auto pinned=owner->native_argv.emplace(pid,*cmd);if(pinned.first->second!=*cmd)throw std::runtime_error("XRService argv changed within owner");
            std::ifstream uptime(proc+"/uptime");double up=0;if(!(uptime>>up))throw std::runtime_error("Invalid uptime");
            double age=up-std::stod(identity)/double(hz);if(!std::isfinite(age)||age<0)throw std::runtime_error("Invalid process age");
            out.push_back({pid,age});
        } catch(const std::exception& e){std::cerr<<"XRService scan: "<<e.what()<<"; skipping process\n";}
        }
        // Only a complete successful scan retires old signal authority. Never
        // prune on a partial/error scan, or close a descriptor still in use.
        for(auto it=owner->identities.begin();it!=owner->identities.end();){
            bool present=false;for(const auto& row:out)present|=row.pid==it->first;
            if(present){++it;continue;}
            ::close(it->second.second);owner->native_argv.erase(it->first);it=owner->identities.erase(it);
        }
        return out;}catch(const std::exception& e){std::cerr<<"XRService scan: "<<e.what()<<"; returning available processes\n";return out;}};
    r.terminate_native=[owner,native,proc,hooks,identity_for](int64_t pid){ try {
        owner->verify();auto it=owner->identities.find(pid);auto argv=owner->native_argv.find(pid);
        const auto process_path=proc+"/"+std::to_string(pid);
        if((hooks.include_process&&!hooks.include_process(pid))||it==owner->identities.end()||argv==owner->native_argv.end()||identity_for(int(pid))!=it->second.first||
           read(process_path+"/cmdline")!=std::optional<std::string>(argv->second)||
           std::filesystem::canonical(process_path+"/exe")!=std::filesystem::canonical(native))throw std::runtime_error("Unbound/replaced XRService executable/argv");
        int result;do{result=owner->hooks.signal_child?owner->hooks.signal_child(it->second.second,SIGTERM):int(syscall(SYS_pidfd_send_signal,it->second.second,SIGTERM,nullptr,0));}while(result<0&&errno==EINTR);
        if(result)fail("XRService SIGTERM");
        } catch(const std::exception& e){std::cerr<<e.what()<<"; leaving process alive\n";}
    };
    r.kill_native=[owner,native,proc,hooks,identity_for](int64_t pid){ try {
        owner->verify();auto it=owner->identities.find(pid);auto argv=owner->native_argv.find(pid);
        const auto process_path=proc+"/"+std::to_string(pid);
        if((hooks.include_process&&!hooks.include_process(pid))||it==owner->identities.end()||argv==owner->native_argv.end()||identity_for(int(pid))!=it->second.first||
           read(process_path+"/cmdline")!=std::optional<std::string>(argv->second)||
           std::filesystem::canonical(process_path+"/exe")!=std::filesystem::canonical(native))throw std::runtime_error("Unbound/replaced XRService executable/argv");
        int result;do{result=owner->hooks.signal_child?owner->hooks.signal_child(it->second.second,SIGKILL):int(syscall(SYS_pidfd_send_signal,it->second.second,SIGKILL,nullptr,0));}while(result<0&&errno==EINTR);
        if(result)fail("XRService SIGKILL");
        } catch(const std::exception& e){std::cerr<<e.what()<<"; leaving process alive\n";}
    };
    r.power=[owner](const std::string& tool,std::optional<double> value,int64_t cap)->std::string{ try {
        owner->verify();
        for(const auto& child:owner->power_children)if(!child->poll())throw std::runtime_error("Previous power writer still alive; recovery deferred");
        owner->power_children.clear();
        if(cap!=8000000000LL)throw std::runtime_error("Power cap must be eight seconds");
        std::vector<std::string> args{tool};if(value){if(!std::isfinite(*value)||*value<0||*value>600)throw std::runtime_error("Power range");
        std::ostringstream s;s<<std::setprecision(17)<<*value;args.push_back(s.str());}
        int pipes[2];if(pipe2(pipes,O_CLOEXEC))fail("power pipe");
        Fd input(pipes[0]),output(pipes[1]);
        auto p=launch(args,output.n,owner->hooks);owner->power_children.push_back(p);::close(output.n);output.n=-1;
        try{
        if(fcntl(input.n,F_SETFL,O_NONBLOCK))fail("power nonblock");
        std::string result;auto end=now()+cap;bool eof=false;
        while(!eof||!p->poll()){
            if(now()>=end)throw std::runtime_error("Power timeout; mutation unknown");
            char b[4096];auto n=::read(input.n,b,sizeof(b));if(n>0){result.append(b,size_t(n));if(result.size()>65536)throw std::runtime_error("Power output overflow");}else if(!n)eof=true;else if(errno!=EAGAIN&&errno!=EINTR)fail("power read");
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        if(*p->poll()!=0)throw std::runtime_error("Power nonzero exit");
        (void)parse(result);return result;
        }catch(...){auto failure=std::current_exception();stop_power(p);std::rethrow_exception(failure);}
    }catch(const std::exception& e){std::cerr<<e.what()<<"; power report unavailable\n";return "{}";}};return r;
}
}
#else
namespace phase5 {
struct OsAdapters::Impl {};
OsAdapters::OsAdapters(const std::string&,FaultHooks){throw std::runtime_error("Linux OS adapters required");}
Files OsAdapters::files(){throw std::runtime_error("Linux required");}
Files OsAdapters::files_hardened(){throw std::runtime_error("Linux required");}
Runtime OsAdapters::runtime(const std::string&,const std::string&,ScanHooks){throw std::runtime_error("Linux required");}
Child OsAdapters::spawn(const std::vector<std::string>&){throw std::runtime_error("Linux required");}
}
#endif
