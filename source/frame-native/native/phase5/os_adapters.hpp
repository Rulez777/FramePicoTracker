#pragma once
#include "orchestrator.hpp"
#include <memory>
namespace phase5 {
// Lifetime owns an exclusive advisory lock. All settings writers must share it.
// files() implements Python parity: held advisory ownership and compare/replace.
// files_hardened() additionally requires kernel exclusion and currently refuses.
// Linux-only; construction never restarts a service or invokes the power tool.
class OsAdapters {
    struct Impl;
    std::shared_ptr<Impl> impl_;
public:
    // Empty in production. Deterministic faults around real Linux syscalls.
    struct FaultHooks {
        std::function<int(int,int*,int)> wait_child;
        std::function<int(int,int)> signal_child;
        std::function<int(int)> send_gate;
        std::function<void()> before_settings_mutation;
        std::function<void()> after_settings_mutation;
    };
    explicit OsAdapters(const std::string& owner_lock, FaultHooks = {});
    Files files();
    Files files_hardened();
    // Test hooks retain real procfs and pidfds; empty hooks preserve production behavior.
    struct ScanHooks {
        std::function<bool(int64_t)> include_process;
        std::function<void(int64_t)> after_candidates;
        // Deterministic fault injection at the PID reuse boundary.
        std::function<std::string(int64_t, const std::string&)> identity;
    };
    Runtime runtime(const std::string& reviewed_native_executable, const std::string& proc_root = "/proc", ScanHooks hooks = {});
    Child spawn(const std::vector<std::string>& argv);
};
}
