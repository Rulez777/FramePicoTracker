#pragma once
#include "tcl_transport.hpp"
#include <memory>
#include <optional>

namespace pico {
std::vector<std::string> openocd_argv();
// Hooks isolate process policy from syscalls; test hooks never touch hardware.
struct OpenOcdHooks {
    std::function<int64_t()> now;
    std::function<void(int64_t)> sleep;
    std::function<bool()> listener_present;
    std::function<void(const std::vector<std::string>&)> spawn;
    std::function<std::optional<int>()> poll;
    std::function<bool()> connect;
    std::function<std::string(const std::string&)> command;
    std::function<void()> disconnect,terminate,kill;
    // Must prove the listener belongs to the owned child. Immutable transitive
    // dependency confinement is extra hardening, not Python-parity admission.
    // Missing or failed attribution is logged; it does not gate a session.
    std::function<void()> verify_attribution;
    std::function<int64_t()> child_pid;
};
// Requires exclusive Roy ownership BEFORE start. Never adopts a preexisting
// listener. No destructor kills a possibly active SRAM owner.
class OpenOcdProcess {
    OpenOcdHooks h_;
    bool spawned_=false,connected_=false,sram_unresolved_=false,closed_=false;
    bool wait(int64_t);
    void reap_failed_start();
public:
    explicit OpenOcdProcess(OpenOcdHooks);
    void start();
    std::string command(const std::string&);
    void before_sram_mutation() { sram_unresolved_=true; }
    void restored(bool session,bool ram,bool mpu,bool trace);
    void close();
    bool owned() const {return spawned_&&!closed_;}
};
OpenOcdHooks linux_openocd_hooks(const std::string& exclusive_log,
                               const std::string& executable="/usr/bin/openocd");
}
