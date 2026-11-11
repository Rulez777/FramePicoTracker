#include "guard_evidence.hpp"
#pragma once
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
namespace pico {
// OS adapter must collect these from the unreaped direct child, never a PID
// supplied by a readiness file. Immutable launch handles remain an adapter duty.
struct ListenerIdentity {
    int64_t child=0, owner=0;
    uint64_t child_start=0, observed_start=0, socket_inode=0;
    std::string address, executable_seal;
    unsigned port=0, matching_listeners=0;
    std::vector<std::string> argv, dependency_seals;
};
inline void verify_listener_identity(const ListenerIdentity& e,const ListenerIdentity& launch) {
    pico::guard_evidence::Scope guard_evidence_scope(3,"listener-child");
    if(launch.child<=0 || !launch.child_start || launch.executable_seal.empty() || launch.dependency_seals.empty()
       || e.child!=launch.child || e.owner!=launch.child || e.observed_start!=launch.child_start
       || e.child_start!=launch.child_start || !e.socket_inode || e.matching_listeners!=1
       || launch.argv.size()!=5 || launch.argv[0].empty() || e.address!="127.0.0.1" || e.port!=6666 || e.argv!=launch.argv
       || e.executable_seal!=launch.executable_seal || e.dependency_seals!=launch.dependency_seals)
        throw std::runtime_error("OpenOCD child/listener/launch attribution unresolved");
    for(const auto& seal:launch.dependency_seals)if(seal.empty())throw std::runtime_error("Empty OpenOCD dependency seal");
}
}
