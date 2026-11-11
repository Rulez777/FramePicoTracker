#pragma once
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <stdexcept>
namespace pico {
// Evidence must come from the held shared lock and a complete OS scan, never
// from readiness. Scan errors throw; an empty list is not an error fallback.
struct SessionOwnership {
    std::string lock_path, service_state;
    uint64_t lock_device=0,lock_inode=0,owner_start=0;
    int64_t owner_pid=0,debugger_pid=0;
    int service_status=-1;
    bool lock_held=false,process_scan_complete=false;
    std::vector<int64_t> radio_processes;
    std::map<int64_t,std::string> radio_details{};
};
inline void verify_session_ownership(const SessionOwnership& e,
    const std::string& required_lock="/home/steamos/.local/state/frame-pico/roy.lock") {
    if(!e.lock_held||!e.lock_device||!e.lock_inode||!e.owner_start||e.owner_pid<=0||e.lock_path!=required_lock)
        throw std::runtime_error("Shared Roy lock unresolved: path="+e.lock_path+" holder="+std::to_string(e.owner_pid)+" held="+std::to_string(e.lock_held));
    if(!e.process_scan_complete)
        throw std::runtime_error("Roy process scan incomplete: lock="+e.lock_path);
    if(!((e.service_status==3||e.service_status==4)&&
         (e.service_state=="inactive"||e.service_state=="failed"||e.service_state=="unknown")))
        throw std::runtime_error("Station service unresolved: unit=frame-pico-radio.service command=systemctl --user is-active status="+
            std::to_string(e.service_status)+" ActiveState=["+e.service_state+"] SubState=not-queried");
    for(auto pid:e.radio_processes)
        if(pid<=0||(pid!=e.owner_pid&&(e.debugger_pid<=0||pid!=e.debugger_pid)))
            throw std::runtime_error("Existing or unknown radio owner: "+(e.radio_details.count(pid)?e.radio_details.at(pid):"pid="+std::to_string(pid)+" identity=unavailable"));
}
inline bool same_session_owner(const SessionOwnership& a,const SessionOwnership& b) {
    return a.lock_path==b.lock_path&&a.lock_device==b.lock_device&&a.lock_inode==b.lock_inode
        &&a.owner_pid==b.owner_pid&&a.owner_start==b.owner_start&&a.debugger_pid==b.debugger_pid;
}
}

