#pragma once
#include <cstdint>
#include <map>
#include <string>
namespace pico {
// Thread-local counters: replay snapshots exclude its independent evaluation fits.
inline thread_local std::map<std::string,uint64_t> recovery_diagnostics;
inline bool recovery_reject(const char* reason) {
    ++recovery_diagnostics[reason]; return false;
}
}
