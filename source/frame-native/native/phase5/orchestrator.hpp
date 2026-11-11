#pragma once
#include "json.hpp"
#include <functional>
#include <optional>

namespace phase5 {
std::string sha256(const std::string&);
std::string sha256_stream(const std::function<size_t(char*,size_t)>&);
struct FileDigest { std::string sha; uint64_t size=0; };
std::string encode(const Value&);
Value parse(const std::string&);
// Absent is nullopt; inaccessible/invalid/short IO MUST throw, never mean absent.
// resolve must resolve symlinks, including existing parents of absent leaves.
// Mutations require exclusive ownership; compare operations must refuse races.
// Linux adapter candidate: see os_adapters.hpp and NOTES_PHASE5.md admission limits.
struct Files {
    std::function<std::string(const std::string&)> resolve;
    std::function<std::optional<std::string>(const std::string&)> read;
    std::function<void(const std::string&,const std::string&)> exclusive;
    std::function<void(const std::string&,const std::optional<std::string>&,
                       const std::optional<std::string>&)> compare_replace;
    // Mandatory for profile mutation/recovery. Revalidate the held common
    // writer lock, including pathname inode and process identity.
    std::function<void()> writer_exclusion;
    std::function<FileDigest(const std::string&,bool)> digest={};
};
inline FileDigest digest_file(Files& files,const std::string& path,bool executable) {
    if(files.digest)return files.digest(path,executable);
    auto bytes=files.read(path);
    if(!bytes)throw std::runtime_error("Digest input absent");
    return {sha256(*bytes),uint64_t(bytes->size())};
}
struct Clock {
    std::function<int64_t()> now;
    std::function<void(int64_t)> sleep;
};
struct BoundInvocation {
    std::string root, sha, native, native_sha, power_tool, settings;
    std::vector<std::string> command;
    // Exact bytes which passed the invocation anchor and file seals. Adapter
    // construction must not reparse a mutable invocation/config pathname.
    // Executables verified through Files::digest have empty byte entries;
    // launch adapters capture and seal their bytes independently.
    std::string invocation_bytes;
    std::map<std::string,std::string> sealed_files;
};
// reviewed_command is an independently approved native argv, NOT data copied
// from invocation.json. This is not a live admission entry point.
BoundInvocation verify_seals(Files&,const std::string& root,
    const std::string& invocation_path,const std::string& anchor_path,
    const std::vector<std::string>& reviewed_command);
void validate_single_peer(const Value& config,const Value& registry);
struct HumanAnswer {
    std::string exact_answer;
    bool hmd_tracked=false,controllers_off=false,tracker_supported=false;
    // Independent physical confirmation; tracked never implies worn.
    bool hmd_worn=false;
};
void create_readiness(Files&,const BoundInvocation&,const HumanAnswer&,
                      const std::string& boot,int64_t now);
void verify_readiness(Files&,const BoundInvocation&,const std::string& boot,int64_t now);
bool cleanup_barriers(const std::string& report);

struct ManagedProcess { int64_t pid; double age_seconds; };
struct Runtime {
    Clock clock;
    std::function<std::string()> boot_id;
    // Return ONLY XRService argv with BOTH --wasStartedByDriverCV and
    // --connectToSteamVR. OS adapter must bind stable process identity.
    std::function<std::vector<ManagedProcess>()> managed;
    std::function<void(int64_t)> terminate_native;
    std::function<void(int64_t)> kill_native;
    // Exactly argv [power_tool] or [power_tool, value], 8-second command cap.
    // Nonzero exit, timeout, truncated output and parse errors MUST throw.
    std::function<std::string(const std::string&,std::optional<double>,int64_t)> power;
};
void restart_native(Runtime&);
struct Child {
    int64_t pid=0;
    // nullopt means alive; errors throw and cannot prove termination.
    std::function<std::optional<int>()> poll;
    std::function<void()> interrupt;
};
struct ChildState {
    bool terminated=false;
    std::optional<int> exit_code;
    std::string cause;
};
ChildState bounded_wait(Child&,Clock&,const std::function<bool()>& interrupted);

struct Cleanup {
    bool owned_camera_closed=false,native_settings_restored=false;
    bool power_restored=false,mcu_cleanup_verified=false,deferred=false;
    std::vector<std::string> errors;
    std::string json() const;
};
class ProfileOwner {
    Files& fs_; Runtime& rt_; BoundInvocation inv_;
    std::optional<std::string> original_;
    std::string expected_;
    std::optional<double> old_power_;
    bool started_=false,profile_ready_=false,settings_attempted_=false,launch_entered_=false,cleaned_=false;
    bool supervisor_verified_=false;
    // Copying the handle retains the OS adapter's shared process state even
    // after the caller's Child wrapper leaves scope (like Python's reference).
    std::optional<Child> child_;
    std::function<void()> camera_close_;
    void writer();
public:
    ProfileOwner(Files&,Runtime&,BoundInvocation);
    void apply();
    // Call BEFORE starting acquisition; a failed start still needs close.
    void own_camera(std::function<void()> close);
    void launch_begin();
    void attach_child(Child&);
    void verify_active(); // same settings/power writer throughout SRAM session
    void supervisor_restored(const std::string& barriers);
    // Always attempts independent cleanup resources; never signals an SRAM owner.
    // Explicit cleanup; errors are logged and returned as diagnostics.
    Cleanup cleanup();
};
} // namespace phase5
