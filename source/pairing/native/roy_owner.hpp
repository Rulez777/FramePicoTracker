#pragma once
#include "tcl_transport.hpp"
#include <stdexcept>

namespace pico {
inline const uint32_t roy_mpsl = roy_env("ROY_MPSL", 0x2001056c), roy_hook = roy_env("ROY_HOOK", 0x2000d8cc),
                       roy_static = roy_env("ROY_STATIC", 0x20010a24);
inline const uint32_t roy_data_size = roy_env("ROY_DATA_SIZE", 2048, 10), roy_code_size = roy_env("ROY_CODE_SIZE", 2045, 10);
uint32_t word(const Bytes &, size_t offset = 0);
Bytes le32(uint32_t);
struct OwnerIO {
    std::function<Bytes(uint32_t, size_t)> read;
    std::function<void(uint32_t, const Bytes &)> write;
    std::function<std::string(const std::string &)> command;
    std::function<int64_t()> now;
    std::function<void(int64_t)> sleep;
};
struct Checkpoint {
    Bytes code, data, counts, static_word, dwt, demcr;
    bool execute_permission = false, permissions_saved = false;
};
// JSON fields follow loader checkpoint order; byte fields remain exact hex.
std::string checkpoint_json(const Checkpoint &);
// Creates/replaces a checkpoint with flush + OS durability, then byte readback.
// The explicit path is owned by the caller; there is no default output location.
void persist_checkpoint(const std::string &path, const Checkpoint &);
struct CleanupBarriers {
    bool session_closed_verified = false, ram_restored_verified = false, mpu_restored = false,
         trace_controls_restored_verified = false;
    bool complete() const {
        return session_closed_verified && ram_restored_verified && mpu_restored &&
               trace_controls_restored_verified;
    }
    std::string json() const;
};
// The SRAM transaction only. No process launcher, RF entry point or permissive
// replacement for the sealed hardware admission/lock/watchdog is provided here.
// A supervising owner MUST retain its process/lock/debugger whenever unresolved().
class SramOwner {
    OwnerIO io_;
    std::function<void(const Checkpoint &)> persist_;
    Checkpoint saved_;
    uint32_t entry_ = 0;
    bool saved_code_ = false, saved_static_ = false, saved_trace_ = false, initialized_ = false,
         installed_ = false, invocation_ = false, running_ = false, restoring_ = false,
         opened_ = false, mutated_ = false;
    bool reserve_, disable_;
    void equal(uint32_t, const Bytes &, const char *);
    void invoke(unsigned);
    void enable_trace();

  public:
    CleanupBarriers barriers;
    // multi4 station module: the host enables DEMCR.TRCENA and DWT_CTRL.CYCCNTENA over SWD before operation 0.
    // Default: PICO_PEERS (peer_table.hpp) or PICO_HOST_DWT set; the checkpoint values are restored as before.
    bool host_dwt = false, trace_enabled_verified = false;
    SramOwner(OwnerIO, std::function<void(const Checkpoint &)>, bool reserve = true,
              bool disable = false);
    SramOwner(const SramOwner &) = delete;
    SramOwner &operator=(const SramOwner &) = delete;
    // stock/module hashes, ELF and prepared state admission belong to the caller;
    // this layer verifies the five live anchors and checkpoint/install sequence.
    void install(const Bytes &stock, const Bytes &module, uint32_t entry, const Bytes &state);
    void enter(const std::function<void()> &fresh_guard);
    void repeat();
    void restore(); // throws on first MCU failure; retains all checkpoint state
    bool unresolved() const { return mutated_ && !barriers.complete(); }
    bool may_release_owner() const { return !unresolved(); }
};
struct OwnerResources {
    std::function<void()> publish_stopped, restore_signals, close_debugger, close_optics,
        close_guard, release_lock;
    std::function<void(const CleanupBarriers &, const std::vector<std::string> &, bool)> report;
};
// Explicit finalization, including partial installation. Independent resource
// failures cannot suppress reporting. Unresolved SRAM retains debugger AND lock;
// the supervising process must remain alive for recovery, never kill/abandon it.
std::vector<std::string> finalize_owner(SramOwner &, const OwnerResources &);
} // namespace pico
