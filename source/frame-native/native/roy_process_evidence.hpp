#pragma once
#include <algorithm>
#include <iostream>
#include <cctype>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>
#include <utility>

namespace pico {
inline uint64_t roy_process_start_ticks(const std::string& stat) {
    const auto end=stat.rfind(')');
    if(end==std::string::npos)
        throw std::runtime_error("Malformed Roy process stat");
    std::istringstream in(stat.substr(end+1));
    std::string field;
    for(unsigned i=3;i<=22;++i) {
        if(!(in>>field))
            throw std::runtime_error("Truncated Roy process stat");
    }
    if(field.empty()||field.find_first_not_of("0123456789")!=std::string::npos)
        throw std::runtime_error("Invalid Roy process start");
    return std::stoull(field);
}
inline uint64_t roy_process_start(const std::string& stat) {
    const auto start=roy_process_start_ticks(stat);
    if(!start)
        throw std::runtime_error("Missing Roy process start");
    return start;
}
// Early kernel threads may have been created in boot tick zero. They are
// exclusion evidence only, never owner/parent/debugger identity authority.
inline uint64_t roy_scan_process_start(const std::string& stat,
    const std::string& command, bool executable_missing) {
    std::istringstream fields(stat.substr(stat.rfind(')')+1));
    std::string state,field;
    unsigned long long flags=0;
    fields>>state;
    for(unsigned i=4;i<=8;++i) fields>>field;
    const bool verified_kernel=(fields>>flags)&&executable_missing&&command.empty()
        &&(flags&0x200000ULL); // PF_KTHREAD; zombie status alone is insufficient.
    const auto start=roy_process_start_ticks(stat);
    if(!start&&!verified_kernel)
        throw std::runtime_error("Missing Roy process start");
    return start;
}
// Broad exclusion includes legacy loaders, native producers, recovery tools,
// OpenOCD and station owners. Classification grants no signaling authority.
inline bool roy_radio_command(const std::string& executable) {
    auto name=executable.substr(executable.find_last_of('/')+1);
    const std::string deleted=" (deleted)";
    if(name.size()>deleted.size()&&name.compare(name.size()-deleted.size(),deleted.size(),deleted)==0)
        name.resize(name.size()-deleted.size());
    for(const auto* binary:{"openocd","pico_producer","pico_producer_live_fixture","roy_ram","roy_station","frame-pico-radio","roy_recover"})
        if(name==binary)return true;
    return false;
}
struct RoyProcessEvidence {
    int64_t pid=0;
    uint64_t start=0;
    std::string executable,command,comm;
    RoyProcessEvidence() = default;
    RoyProcessEvidence(int64_t p,uint64_t s,std::string exe,std::string argv,std::string name={})
        :pid(p),start(s),executable(std::move(exe)),command(std::move(argv)),comm(std::move(name)) {}
};
}
#ifndef _WIN32
#include <cerrno>
#include <cstring>
#include <chrono>
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
namespace pico {
inline std::string roy_proc_read(const std::filesystem::path& path) {
    const int fd=::open(path.c_str(),O_RDONLY|O_CLOEXEC|O_NOFOLLOW|O_NONBLOCK);
    if(fd<0)
        throw std::system_error(errno,std::generic_category(),"Incomplete Roy process scan: open " + path.string());
    std::string bytes;
    char buffer[4096];
    for(;;) {
        const auto n=::read(fd,buffer,sizeof(buffer));
        if(n<0&&errno==EINTR)
            continue;
        if(n<0||bytes.size()+size_t(n)>65536) {
            const int saved_errno=errno;
            const std::string reason=n<0?std::strerror(errno):"65536-byte cap exceeded";
            ::close(fd);
            if(n<0)
                throw std::system_error(saved_errno,std::generic_category(),"Incomplete Roy process scan: read " + path.string());
            throw std::runtime_error("Incomplete Roy process scan: read " + path.string() + ": " + reason);
        }
        if(n==0)
            break;
        bytes.append(buffer,size_t(n));
    }
    ::close(fd);
    return bytes;
}
inline RoyProcessEvidence linux_roy_identity(int64_t pid) {
    const auto root = std::filesystem::path("/proc") / std::to_string(pid);
    const auto start = roy_process_start(roy_proc_read(root / "stat"));
    const auto command = roy_proc_read(root / "cmdline");
    const auto executable = std::filesystem::read_symlink(root / "exe").string();
    if (roy_process_start(roy_proc_read(root / "stat")) != start ||
        roy_proc_read(root / "cmdline") != command ||
        std::filesystem::read_symlink(root / "exe").string() != executable)
        throw std::runtime_error("Parent identity changed during capture");
    return {pid, start, executable, command};
}
inline void exclude_authenticated_parent(std::vector<RoyProcessEvidence>& scan,
    const RoyProcessEvidence& parent, int pidfd) { try {
    pollfd event{pidfd, POLLIN, 0};
    if (pidfd < 0 || parent.pid <= 0 || !parent.start || poll(&event, 1, 0) != 0)
        throw std::runtime_error("Parent scan authority lost");
    const auto info = roy_proc_read("/proc/self/fdinfo/" + std::to_string(pidfd));
    std::istringstream fields(info);
    std::string key;
    int64_t actual_pid = 0;
    while (fields >> key) {
        if (key == "Pid:") {
            fields >> actual_pid;
            break;
        }
        std::string rest;
        std::getline(fields, rest);
    }
    const auto current = linux_roy_identity(parent.pid);
    if (actual_pid != parent.pid || current.start != parent.start ||
        current.command != parent.command || current.executable != parent.executable)
        throw std::runtime_error("Parent scan identity/pidfd mismatch");
    scan.erase(std::remove_if(scan.begin(), scan.end(), [&](const RoyProcessEvidence& row) {
        if (row.pid != parent.pid) return false;
        if (row.start != parent.start || row.command != parent.command ||
            row.executable != parent.executable)
            throw std::runtime_error("Scanned parent identity changed");
        return true;
    }), scan.end());
    }catch(const std::exception& e){std::cerr<<"Roy parent scan: "<<e.what()<<"; retaining available scan\n";}
}
// Only /proc/PID/exe selects an actual radio binary. argv/comm never select owners.
inline std::vector<RoyProcessEvidence> linux_roy_processes() {
    std::vector<RoyProcessEvidence> result;
    std::error_code error;
    std::filesystem::directory_iterator entries("/proc",error);
    if(error){std::cerr<<"Roy process scan: "<<error.message()<<"\n";return result;}
    for(const auto& entry:entries) {
        const auto name=entry.path().filename().string();
        if(name.empty()||name.find_first_not_of("0123456789")!=std::string::npos)continue;
        std::error_code exe_error;
        const auto executable=std::filesystem::read_symlink(entry.path()/"exe",exe_error).string();
        if(exe_error||!roy_radio_command(executable))continue;
        try {
            auto identity=linux_roy_identity(std::stoll(name));
            if(roy_radio_command(identity.executable))result.push_back(std::move(identity));
        } catch(const std::exception& failure) {
            std::cerr<<"Roy process scan: "<<failure.what()<<"; skip changed/unavailable process\n";
        }
    }
    return result;
}
}
#endif

#include "live_implementation_registry.hpp"
#ifndef _WIN32
PICO_LIVE_IMPLEMENTATION(pico_impl_process_scan, "process-scan");
#endif
