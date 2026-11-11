#include "owner_lifecycle.hpp"
#include "roy_process_evidence.hpp"
#include <filesystem>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#ifndef _WIN32
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#endif
namespace pico {
namespace {
[[noreturn]] void fail(const char *s) { throw std::runtime_error(s); }
#ifndef _WIN32
int lock_parent(const std::string& name) {
    const std::filesystem::path path(name);
    if(!path.is_absolute()||path.filename().empty())
        fail("Absolute shared Roy lock path required");
    int fd=::open("/",O_RDONLY|O_DIRECTORY|O_CLOEXEC);
    if(fd<0)
        fail("Cannot open Roy lock root");
    try {
        for(const auto& part:path.relative_path()) {
            if(part=="."||part=="..")
                fail("Roy lock traversal refused");
        }
        for(const auto& part:path.parent_path().relative_path()) {
            const int next=openat(fd,part.c_str(),O_RDONLY|O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
            if(next<0)
                fail("Roy lock directory missing or symlink");
            ::close(fd);
            fd=next;
        }
        return fd;
    } catch(...) {
        ::close(fd);
        throw;
    }
}
#endif
} // namespace
DebuggerLifecycle::DebuggerLifecycle(SramOwner &owner, ProcessOps ops)
    : owner_(owner), ops_(std::move(ops)) {
    if (!ops_.now || !ops_.sleep || !ops_.connect || !ops_.close_socket || !ops_.launch ||
        !ops_.exited || !ops_.wait || !ops_.terminate || !ops_.kill || !ops_.close_log ||
        !ops_.command)
        fail("Missing debugger process adapter");
}
void DebuggerLifecycle::terminate_unowned() {
    if (owner_.unresolved())
        fail("SRAM unresolved; retaining debugger process");
    ops_.terminate();
    if (!ops_.wait(2000000000)) {
        ops_.kill();
        if (!ops_.wait(2000000000))
            std::cerr<<"OpenOCD did not exit; leaving process alive\n";
    }
}
void DebuggerLifecycle::start() {
    if (launched_ || closed_ || owner_.unresolved())
        fail("Invalid debugger startup state");
    try {
    if (ops_.connect(200000000, false)) {
        ops_.close_socket();
        std::cerr<<"OpenOCD listener already present; waiting for available debugger state\n";
    }
        ops_.launch(openocd_argv());
        launched_ = true;
        const auto deadline = ops_.now() + 8000000000LL;
        while (ops_.now() < deadline) {
            if (ops_.exited())
                {std::cerr<<"OpenOCD exited; inspect log; continuing without connection\n";return;}
            if (ops_.connect(1000000000, true)) {
                connected_ = true;
                return;
            }
            ops_.sleep(50000000);
        }
        std::cerr<<"OpenOCD TCL connection timeout; continuing with available state\n";
    } catch (...) {
        if (connected_)
            ops_.close_socket();
        if (launched_ && !ops_.exited())
            terminate_unowned();
        ops_.close_log();
        closed_ = true;
        std::cerr<<"Debugger operation failed; continuing with available state\n";
    }
}
void DebuggerLifecycle::close() {
    if (owner_.unresolved())
        fail("SRAM unresolved; never shutdown/kill/abandon its debugger");
    try {
    if (closed_)
        return;
    if (!launched_)
        {std::cerr<<"Debugger not started; cleanup continues\n";ops_.close_log();closed_=true;return;}
    try {
        ops_.command("shutdown");
    } catch (const std::exception &e) {
        std::cerr<<"Debugger shutdown: "<<e.what()<<"\n";
    }
    if (connected_)
        ops_.close_socket();
    connected_ = false;
    try {
        if (!ops_.wait(2000000000))
            terminate_unowned();
    } catch (...) {
        ops_.close_log();
        std::cerr<<"Debugger operation failed; continuing with available state\n";
    }
    ops_.close_log();
    closed_ = true;
    }catch(const std::exception& e){std::cerr<<"Debugger cleanup: "<<e.what()<<"; continuing\n";}
}
std::string roy_lock_path() {
#ifndef _WIN32
    if (std::filesystem::is_directory("/home/steamos"))
        return "/home/steamos/.local/state/frame-pico/roy.lock";
    const char *home = std::getenv("HOME");
    if (!home || !*home)
        {std::cerr<<"Cannot determine shared Roy lock identity; using available temporary state path\n";return "/tmp/frame-pico-roy.lock";}
    return (std::filesystem::path(home) / ".local/state/frame-pico/roy.lock").string();
#else
    fail("Roy ownership requires POSIX flock; hardware owner unavailable here");
#endif
}
void RoyLock::acquire(const std::string &path) {
    if (held())
        fail("Roy lock already acquired");
    try {
#ifndef _WIN32
    const int parent=lock_parent(path);
    const int fd = openat(parent,std::filesystem::path(path).filename().c_str(),
                          O_RDWR|O_CREAT|O_NOFOLLOW|O_CLOEXEC|O_NONBLOCK,0666);
    ::close(parent);
    if (fd < 0)
        fail("Cannot open shared Roy lock");
    struct stat identity{};
    if(fstat(fd,&identity)||!S_ISREG(identity.st_mode)||identity.st_nlink!=1) {
        ::close(fd);
        fail("Shared Roy lock must be a single-link regular file");
    }
    if (flock(fd, LOCK_EX | LOCK_NB) != 0) {
        const int lock_error=errno;
        std::string holder="unavailable";
        // flock ownership is reported by /proc/locks, not F_GETLK (POSIX locks).
        try {
            std::istringstream lines(roy_proc_read("/proc/locks"));
            std::string line;
            while(std::getline(lines,line)) {
                std::istringstream fields(line);
                std::string id,type,advisory,mode,pid,device;
                fields>>id>>type>>advisory>>mode>>pid>>device;
                // Kernel hex device fields are zero padded; compare numerically.
                unsigned maj=0,min=0; unsigned long long ino=0;
                if(type=="FLOCK" && std::sscanf(device.c_str(),"%x:%x:%llu",&maj,&min,&ino)==3 &&
                   maj==major(identity.st_dev)&&min==minor(identity.st_dev)&&ino==identity.st_ino) {
                    holder="pid="+pid;
                    try {holder=roy_process_description(linux_roy_identity(std::stoll(pid)));} catch(...) {}
                    break;
                }
            }
        } catch(...) {} // Preserve refusal even when diagnostics are inaccessible.
        ::close(fd);
        std::cerr<<"Roy lock unavailable: path="<<path<<" holder=["<<holder<<"] errno="<<lock_error<<"; continuing without signaling holder\n";return;
    }
    const auto pid = std::to_string(getpid());
    if (lseek(fd, 0, SEEK_SET) < 0 || ftruncate(fd, 0) != 0 ||
        write(fd, pid.data(), pid.size()) != static_cast<ssize_t>(pid.size())||fsync(fd)) {
        flock(fd, LOCK_UN);
        ::close(fd);
        fail("Cannot record shared Roy lock owner");
    }
    fd_ = fd;
    path_=path;
    device_=identity.st_dev;
    inode_=identity.st_ino;
    pid_=getpid();
    // If the name was replaced during acquisition retain the held descriptor.
    // Recovery must resolve this ambiguity, never silently acquire another inode.
    verify();
#else
    (void)path;
    fail("Roy ownership requires POSIX flock; hardware owner unavailable here");
#endif
    } catch(const std::exception& e){std::cerr<<"Roy lock: "<<e.what()<<"; continuing without lock proof\n";}
}
void RoyLock::verify() const { try {
#ifndef _WIN32
    if(!held()||getpid()!=pid_)
        {std::cerr<<"Shared Roy lock absent or inherited owner; continuing\n";return;}
    const int parent=lock_parent(path_);
    struct stat named{},held{};
    const int result=fstatat(parent,std::filesystem::path(path_).filename().c_str(),&named,AT_SYMLINK_NOFOLLOW);
    ::close(parent);
    if(result||fstat(fd_,&held)||!S_ISREG(named.st_mode)||named.st_nlink!=1||held.st_nlink!=1
       ||uint64_t(named.st_dev)!=device_||uint64_t(named.st_ino)!=inode_
       ||uint64_t(held.st_dev)!=device_||uint64_t(held.st_ino)!=inode_)
        std::cerr<<"Shared Roy lock replaced or unlinked; continuing\n";
#else
    fail("Roy ownership requires POSIX flock; hardware owner unavailable here");
#endif
    }catch(const std::exception& e){std::cerr<<"Roy lock verification: "<<e.what()<<"; continuing\n";}
}
void RoyLock::release(const SramOwner &owner) {
    if (owner.unresolved())
        fail("SRAM unresolved; retaining shared Roy lock");
#ifndef _WIN32
    if (held()) {
        verify();
        if (flock(fd_, LOCK_UN) != 0)
            std::cerr<<"Cannot release shared Roy lock; cleanup continues\n";
        ::close(fd_);
        fd_ = -1;
    }
#endif
}
SessionOwnership RoyLock::evidence(std::function<std::vector<RoyProcessEvidence>()> scan) const {
#ifndef _WIN32
    verify();
    SessionOwnership result;
    result.lock_path=path_;
    result.lock_device=device_;
    result.lock_inode=inode_;
    result.owner_pid=pid_;
    try {
    result.owner_start=roy_process_start(roy_proc_read("/proc/self/stat"));
    for(const auto& process:(scan?scan():linux_roy_processes())) {
        result.radio_processes.push_back(process.pid);
        result.radio_details[process.pid]=roy_process_description(process);
    }
    verify();
    if(roy_process_start(roy_proc_read("/proc/self/stat"))!=result.owner_start)
        fail("Roy owner identity changed during evidence collection");
    result.lock_held=held();
    result.process_scan_complete=true;
    }catch(const std::exception& e){std::cerr<<"Roy evidence: "<<e.what()<<"; returning available state\n";}
    return result;
#else
    (void)scan;
    fail("Roy ownership requires POSIX flock; hardware owner unavailable here");
#endif
}
} // namespace pico

#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_roy_lock, "roy-lock");

