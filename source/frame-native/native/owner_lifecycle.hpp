#pragma once
#include "roy_owner.hpp"
#include "openocd_process.hpp"
#include "session_ownership.hpp"
#include "roy_process_description.hpp"
namespace pico {

struct ProcessOps {
    std::function<int64_t()> now;
    std::function<void(int64_t)> sleep;
    // connect(timeout_ns, configure_tcp_nodelay_and_10s_timeout); false=OSError
    std::function<bool(int64_t, bool)> connect;
    std::function<void()> close_socket;
    std::function<void(const std::vector<std::string> &)> launch;
    std::function<bool()> exited;
    std::function<bool(int64_t)> wait;
    std::function<void()> terminate, kill, close_log;
    std::function<void(const std::string &)> command;
};
// Process policy with injected OS adapter. Tests never launch OpenOCD. The
// native POSIX spawn/connect adapter and parent watchdog are still required
// before this policy can form a runnable hardware owner.
class DebuggerLifecycle {
    SramOwner &owner_;
    ProcessOps ops_;
    bool launched_ = false, connected_ = false, closed_ = false;
    void terminate_unowned();

  public:
    DebuggerLifecycle(SramOwner &, ProcessOps);
    void start();
    void close(); // refuses before ANY shutdown/terminate/kill if SRAM unresolved
    bool retained() const { return launched_ && !closed_; }
};
// Exact shared lock identity, including root/steamos agreement. No lock is
// acquired implicitly by constructing another Phase 2 component.
std::string roy_lock_path();
class RoyLock {
    int fd_ = -1;
    std::string path_;
    uint64_t device_=0,inode_=0;
    int64_t pid_=0;

  public:
    RoyLock() = default;
    RoyLock(const RoyLock &) = delete;
    RoyLock &operator=(const RoyLock &) = delete;
    // Explicit release only; unresolved ownership MUST stay in its supervisor.
    // Destructor deliberately does not unlock/close a retained descriptor.
    void acquire(const std::string &path);
    // Re-open every directory without following symlinks and verify the named
    // inode still is the held lock. A replaced lock never authorizes release.
    void verify() const;
    // Collect from the held inode and a complete scan (real procfs by default).
    // The supervisor supplies its authenticated-parent-filtered scan; the
    // dedicated fixture supplies a synthetic view. Service evidence is separate.
    SessionOwnership evidence(std::function<std::vector<RoyProcessEvidence>()> scan = {}) const;
    void release(const SramOwner &);
    bool held() const { return fd_ != -1; }
};
} // namespace pico

