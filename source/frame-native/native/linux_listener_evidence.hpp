#pragma once
#include "roy_process_evidence.hpp"
namespace pico {
struct TcpListenerEvidence {
    unsigned count=0;
    uint64_t inode=0;
};
inline void collect_tcp_listener_rows(const std::string& bytes,unsigned port,bool ipv6,TcpListenerEvidence& out) {
    std::istringstream rows(bytes);
    std::string line;
    if(!std::getline(rows,line)||line.find("local_address")==std::string::npos)
        throw std::runtime_error("Invalid proc TCP header");
    while(std::getline(rows,line)) {
        if(line.empty())
            continue;
        std::istringstream fields(line);
        std::string slot,local,remote,state,queue,timer,retransmit,uid,timeout,inode;
        if(!(fields>>slot>>local>>remote>>state>>queue>>timer>>retransmit>>uid>>timeout>>inode))
            throw std::runtime_error("Truncated proc TCP row");
        const auto colon=local.find(':');
        if(colon==std::string::npos||colon!=(ipv6?32u:8u)||local.size()-colon!=5
           ||local.substr(0,colon).find_first_not_of("0123456789ABCDEFabcdef")!=std::string::npos
           ||local.substr(colon+1).find_first_not_of("0123456789ABCDEFabcdef")!=std::string::npos
           ||state.size()!=2||state.find_first_not_of("0123456789ABCDEFabcdef")!=std::string::npos)
            throw std::runtime_error("Malformed proc TCP address");
        if(std::stoul(local.substr(colon+1),nullptr,16)!=port||state!="0A")
            continue;
        // Count/refuse wildcard, foreign-address and IPv6 competing listeners.
        ++out.count;
        if(ipv6||local.substr(0,colon)!="0100007F"||inode.empty()
           ||inode.find_first_not_of("0123456789")!=std::string::npos)
            throw std::runtime_error("Foreign/wildcard OpenOCD listener");
        out.inode=std::stoull(inode);
        if(!out.inode)
            throw std::runtime_error("Missing OpenOCD listener inode");
    }
}
}
#ifndef _WIN32
namespace pico {
// Proves the unreaped owned child holds the sole loopback listening inode.
// Does not substitute for executable/config closure or establish exclusivity
// against a descriptor inherited by another process. Launch adapter must supply
// that boundary before installing verify_attribution in OpenOcdHooks.
inline uint64_t linux_child_listener(int64_t child,uint64_t expected_start,unsigned port=6666) {
    if(child<=0||!expected_start||port==0||port>65535)
        throw std::runtime_error("Invalid owned listener child");
    const auto process=std::filesystem::path("/proc")/std::to_string(child);
    if(roy_process_start(roy_proc_read(process/"stat"))!=expected_start)
        throw std::runtime_error("Listener child identity changed");
    TcpListenerEvidence evidence;
    collect_tcp_listener_rows(roy_proc_read("/proc/net/tcp"),port,false,evidence);
    collect_tcp_listener_rows(roy_proc_read("/proc/net/tcp6"),port,true,evidence);
    if(evidence.count!=1)
        throw std::runtime_error("OpenOCD listener absent or ambiguous");
    const auto target="socket:["+std::to_string(evidence.inode)+"]";
    bool found=false;
    size_t count=0;
    const auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(2);
    for(const auto& entry:std::filesystem::directory_iterator(process/"fd")) {
        if(++count>65536||std::chrono::steady_clock::now()>deadline)
            throw std::runtime_error("Listener fd scan deadline/cap");
        if(std::filesystem::read_symlink(entry.path()).string()==target)
            found=true;
    }
    TcpListenerEvidence fresh;
    collect_tcp_listener_rows(roy_proc_read("/proc/net/tcp"),port,false,fresh);
    collect_tcp_listener_rows(roy_proc_read("/proc/net/tcp6"),port,true,fresh);
    if(!found||fresh.count!=1||fresh.inode!=evidence.inode
       ||roy_process_start(roy_proc_read(process/"stat"))!=expected_start)
        throw std::runtime_error("OpenOCD listener not held by stable child");
    return evidence.inode;
}
}
#endif
