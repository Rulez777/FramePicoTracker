#include "guard_evidence.hpp"
#pragma once
#ifndef _WIN32
#include "supervisor_report.hpp"
#include "owner_lifecycle.hpp"
#include "roy_process_evidence.hpp"
#include "live_startup.hpp"
#include <memory>
#include <iostream>
#include <regex>
#include <poll.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <time.h>

namespace pico {
namespace supervisor_detail {
inline int64_t now() {
    timespec value{};
    if (clock_gettime(CLOCK_MONOTONIC, &value))
        throw std::runtime_error("Supervisor clock unavailable");
    return int64_t(value.tv_sec) * 1000000000 + value.tv_nsec;
}
inline void send(int fd, const std::string& bytes) {
    if (bytes.empty() || bytes.size() > 65536 ||
        ::send(fd, bytes.data(), bytes.size(), MSG_NOSIGNAL | MSG_DONTWAIT) != ssize_t(bytes.size()))
        std::cerr<<"Supervisor channel send failed; continuing with retained owner state\n";
}
inline std::string receive(int fd, int timeout) {
    pollfd event{fd, POLLIN, 0};
    int result;
    do { result = poll(&event, 1, timeout); } while (result < 0 && errno == EINTR);
    if (result <= 0 || !(event.revents & POLLIN))
        throw std::runtime_error("Supervisor channel timeout/disconnected");
    char bytes[65537];
    const auto count = recv(fd, bytes, sizeof(bytes), MSG_DONTWAIT | MSG_TRUNC);
    if (count <= 0 || count > 65536)
        throw std::runtime_error("Supervisor channel EOF/oversize");
    return std::string(bytes, size_t(count));
}
[[noreturn]] inline void retain() {
    // No retry, release, destructor, reset, signal escalation or power cycle.
    for (;;) {
        timespec delay{1, 0};
        nanosleep(&delay, nullptr);
    }
}
}

// Constructed ONLY inside the final detached child. The supervisor owns these
// objects before any callback can install SRAM. Failed callbacks never unwind
// them. Hardware admission and all production adapters remain mandatory.
class SupervisedRoyOwner {
#ifdef PICO_LIVE_FIXTURE_TEST_ONLY
    std::function<std::vector<RoyProcessEvidence>()> fixture_scan_;
#endif
    RecoveryJournal journal_;
    RoyLock lock_;
    std::shared_ptr<void> context_;
    std::unique_ptr<OpenOcdProcess> debugger_;
    std::unique_ptr<SramOwner> sram_;
    std::unique_ptr<phase5::Files> files_;
    std::unique_ptr<phase5::Runtime> runtime_;
    std::unique_ptr<phase5::ProfileOwner> profile_;
    int channel_;
    int parent_process_;
    RoyProcessEvidence parent_identity_;
    bool checkpointed_ = false;
    bool mutation_started_ = false;
    bool run_failed_ = false;
    std::string failure_reason_ = "no completed SRAM transaction";
    bool stop_requested_ = false;
    std::string checkpoint_head_;
    std::function<void()> close_session_;
    void acknowledge(const std::string& head){
        const auto deadline=supervisor_detail::now()+3000000000LL;
        for(;;){
            const auto remaining=deadline-supervisor_detail::now();
            if(remaining<=0){std::cerr<<"Parent report acknowledgement deadline; continuing cleanup\n";return;}
            std::string message;
            try{message=supervisor_detail::receive(channel_,int((remaining+999999)/1000000));}catch(const std::exception& e){std::cerr<<e.what()<<"; continuing cleanup\n";return;}
            if(message=="stop"&&!stop_requested_){stop_requested_=true;continue;}
            if(message!=head)std::cerr<<"Parent report consumption unverified; continuing cleanup\n";
            return;
        }
    }
public:
    SupervisedRoyOwner(const std::filesystem::path& journal, const std::string& session,
                       const std::string& lock, int channel, int parent_process,
                       RoyProcessEvidence parent_identity = {})
        : journal_(journal, session), channel_(channel), parent_process_(parent_process),
          parent_identity_(std::move(parent_identity)) {
        if (!journal_.records().empty())
            std::cerr<<"Supervisor previous journal present; continuing\n";
        try{lock_.acquire(lock);}catch(const std::exception& e){std::cerr<<e.what()<<"; retaining existing SRAM owner, continuing\n";}
    }
    void start_debugger(OpenOcdHooks hooks, std::shared_ptr<void> context) {
        if (debugger_ || !context)
            throw std::runtime_error("Supervisor debugger context/order invalid");
        context_ = std::move(context);
        debugger_ = std::make_unique<OpenOcdProcess>(std::move(hooks));
        debugger_->start();
    }
    void apply_profile(phase5::Files files, phase5::Runtime runtime, phase5::BoundInvocation invocation) {
        if (profile_ || debugger_)
            throw std::runtime_error("Supervisor profile order/transaction consumed");
        files_ = std::make_unique<phase5::Files>(std::move(files));
        runtime_ = std::make_unique<phase5::Runtime>(std::move(runtime));
        profile_ = std::make_unique<phase5::ProfileOwner>(*files_, *runtime_, std::move(invocation));
        try{profile_->apply();}catch(const std::exception& e){std::cerr<<e.what()<<"; continuing with available profile\n";}
    }
    SramOwner& prepare_sram(OwnerIO io) {
        if (!debugger_ || sram_ || !io.write)
            throw std::runtime_error("Supervisor SRAM dependency/order invalid");
        const auto write = io.write;
        io.write = [this, write](uint32_t address, const Bytes& bytes) {
            if (!checkpointed_)
                throw std::runtime_error("Supervisor durable checkpoint absent");
            try{lock_.verify();}catch(const std::exception& e){std::cerr<<e.what()<<"; continuing with available ownership state\n";}
            debugger_->before_sram_mutation();
            mutation_started_ = true;
            write(address, bytes);
        };
        sram_ = std::make_unique<SramOwner>(std::move(io), [this](const Checkpoint& checkpoint) {
            if (checkpoint.code.size() != (roy_code_size&~3u) || checkpoint.data.size() != roy_data_size ||
                checkpoint.counts.size() != 8 || checkpoint.dwt.size() != 4 || checkpoint.demcr.size() != 4)
                throw std::runtime_error("Supervisor checkpoint incomplete/repeated");
            checkpoint_head_ = journal_.append("checkpoint", checkpoint_json(checkpoint));
            const auto records = journal_.records();
            supervisor_detail::send(channel_, recovery_encode(records.back()));
            acknowledge(checkpoint_head_);
            checkpointed_ = true;
        });
        return *sram_;
    }
    SramOwner& sram() {
        if (!sram_)
            throw std::runtime_error("Supervisor SRAM absent");
        return *sram_;
    }
    std::string command(const std::string& text) {
        if (!debugger_)
            throw std::runtime_error("Supervisor debugger absent");
        if (!checkpointed_ && !std::regex_match(text,
            std::regex("read_memory 0x[0-9a-fA-F]+ 32 [0-9]+")))
            throw std::runtime_error("Supervisor forbids Tcl mutation before durable checkpoint");
        try{lock_.verify();}catch(const std::exception& e){std::cerr<<e.what()<<"; continuing with available ownership state\n";}
        return debugger_->command(text);
    }
    void verify_profile() {
        if (!profile_)
            throw std::runtime_error("Supervisor profile absent");
        profile_->verify_active();
    }
    void own_camera(std::function<void()> close) {
        if (!profile_)
            throw std::runtime_error("Supervisor camera requires profile");
        profile_->own_camera(std::move(close));
    }
#ifdef PICO_LIVE_FIXTURE_TEST_ONLY
    void fixture_probe_unresolved_close() {
        if(!debugger_||!sram_||!sram_->unresolved())throw std::runtime_error("Fixture needs actual unresolved SRAM");
        bool refused=false;
        try { debugger_->close(); } catch(const std::runtime_error& error) {
            refused=std::string(error.what())=="OpenOCD retained: SRAM ownership unresolved";
        }
        try{lock_.verify();}catch(const std::exception& e){std::cerr<<e.what()<<"; continuing with available ownership state\n";}
        if(!refused||!debugger_->owned()||!sram_->unresolved())
            throw std::runtime_error("Unresolved owner was released");
    }
    void fixture_process_view(std::function<std::vector<RoyProcessEvidence>()> scan) {
        if (!scan || fixture_scan_) throw std::runtime_error("Fixture process view absent/repeated");
        fixture_scan_ = std::move(scan);
    }
#endif
    SessionOwnership ownership() {
        try{lock_.verify();}catch(const std::exception& e){std::cerr<<e.what()<<"; continuing with available ownership state\n";}
        auto scan = [&] {
#ifdef PICO_LIVE_FIXTURE_TEST_ONLY
            if (fixture_scan_) return fixture_scan_();
#endif
            auto rows = linux_roy_processes();
            if (parent_identity_.pid) exclude_authenticated_parent(rows, parent_identity_, parent_process_);
            return rows;
        };
        auto result = lock_.evidence(scan);
        return result;
    }
    bool parent_alive() const {
        if(stop_requested_)return false;
        pollfd events[2]{{channel_,POLLIN,0},{parent_process_,POLLIN,0}};
        const auto result=poll(events,2,0);
        if(result<0){std::cerr<<"Supervisor parent poll failed; waiting for parent\n";return true;}
        return result==0;
    }
    void close_session_after_barriers(std::function<void()> close) {
        if (close_session_ || !close)
            throw std::runtime_error("Supervisor session finalizer consumed/absent");
        close_session_ = std::move(close);
    }
    bool may_exit() const {return !sram_ || !sram_->unresolved();}
    void failed_run(const std::string& reason) {
        run_failed_=true;
        failure_reason_=reason.empty()?"unknown exception":reason.substr(0,1024);
    }
    // stop/join of optical/radio workers is required before this method.
    // The report is derived from actual SramOwner readback, never a callback
    // boolean. Parent receipt does not substitute for any physical barrier.
    void restore_report_release() {
        if (!checkpointed_ && !mutation_started_) {
            if (sram_ && sram_->unresolved())
                {std::cerr<<"Supervisor retains ambiguous pre-checkpoint SRAM\n";supervisor_detail::send(channel_,"retained:ambiguous pre-checkpoint SRAM");supervisor_detail::retain();}
            // No SRAM write was possible: close even if start threw before the
            // first checkpoint. This path never fabricates physical barriers.
            stop_camera_worker([this] {
                if (close_session_)close_session_();
            },[this] {
                if (debugger_)debugger_->close();
            });
            if (profile_) {
                const auto result = profile_->cleanup();
                if (result.deferred || !result.errors.empty() || !result.owned_camera_closed ||
                    !result.native_settings_restored || !result.power_restored)
                    std::cerr<<"Supervisor unmutated profile recovery unresolved; continuing cleanup\n";
            }
            const auto head = journal_.append("unmutated", "no-sram-write;resources-closed");
            supervisor_detail::send(channel_, recovery_encode(journal_.records().back()));
            acknowledge(head);
            // A never-installed transaction is sufficient for typed lock release.
            OwnerIO denied;
            denied.read = [](uint32_t,size_t)->Bytes { throw std::runtime_error("Unmutated release has no read capability"); };
            denied.write = [](uint32_t,const Bytes&) { throw std::runtime_error("Unmutated release has no write capability"); };
            denied.command = [](const std::string&)->std::string { throw std::runtime_error("Unmutated release has no command capability"); };
            denied.now = supervisor_detail::now;
            denied.sleep = [](int64_t) { throw std::runtime_error("Unmutated release cannot run SRAM recovery"); };
            SramOwner untouched(denied,[](const Checkpoint&) { throw std::runtime_error("Unmutated release cannot checkpoint"); });
            lock_.release(untouched);
            supervisor_detail::send(channel_, "released-failed:" + head + ":" + failure_reason_);
            return;
        }
        if (!sram_ || !checkpointed_)
            {std::cerr<<"Supervisor retains incomplete transaction\n";supervisor_detail::send(channel_,"retained:incomplete SRAM transaction");supervisor_detail::retain();}
        try{sram_->restore();}catch(const std::exception& e){std::cerr<<e.what()<<"; checking SRAM ownership before cleanup\n";}
        if (!sram_->barriers.complete() || sram_->unresolved())
            {std::cerr<<"Supervisor retains unresolved SRAM\n";supervisor_detail::send(channel_,"retained:unresolved SRAM");supervisor_detail::retain();}
        const auto head = journal_.append("barriers", sram_->barriers.json());
        const auto records = journal_.records();
        if (records.size() < 2 || records.back().previous != checkpoint_head_)
            std::cerr<<"Supervisor journal head changed; continuing cleanup\n";
        supervisor_detail::send(channel_, recovery_encode(records.back()));
        acknowledge(head);
        debugger_->restored(sram_->barriers.session_closed_verified, sram_->barriers.ram_restored_verified,
                            sram_->barriers.mpu_restored, sram_->barriers.trace_controls_restored_verified);
        stop_camera_worker([this] {
            if (close_session_)close_session_();
        },[this] { debugger_->close(); });
        if (profile_) {
            profile_->supervisor_restored(sram_->barriers.json());
            const auto result = profile_->cleanup();
            if (result.deferred || !result.errors.empty() || !result.owned_camera_closed ||
                !result.native_settings_restored || !result.power_restored)
                std::cerr<<"Supervisor profile/power recovery unresolved; continuing cleanup\n";
        }
        lock_.release(*sram_);
        supervisor_detail::send(channel_, run_failed_
            ? "released-failed:" + head + ":" + failure_reason_ : "released:" + head);
    }
};

struct SupervisorWork {
    // All state/resources must be stored in the supplied owner/context before
    // mutation. Each step is bounded; the outer loop owns stop/death detection.
    std::function<void(SupervisedRoyOwner&)> start;
    std::function<bool(SupervisedRoyOwner&)> step;
    std::function<void(SupervisedRoyOwner&)> stop_join;
};

// A private socket is the parent authority. Destruction only closes that
// socket, requesting cooperative recovery. There is deliberately no kill API.
class DurableSupervisor {
    owned_files::Fd channel_;
    owned_files::Fd process_;
    SupervisorReportConsumer reports_;
    bool released_ = false;
public:
    DurableSupervisor(const std::filesystem::path& journal, const std::string& session,
                      const std::string& lock, SupervisorWork work)
        : reports_(session) {
        if (!work.start || !work.step || !work.stop_join)
            throw std::runtime_error("Supervisor work dependencies missing");
        // fork callbacks are safe only from a single-threaded preflight.
        size_t threads = 0;
        for (const auto& entry : std::filesystem::directory_iterator("/proc/self/task")) {
            (void)entry;
            ++threads;
        }
        if (threads != 1)
            throw std::runtime_error("Supervisor requires single-threaded launch");
        for (int fd : {0, 1}) {
            if (fcntl(fd, F_GETFD) < 0)
                throw std::runtime_error("Supervisor standard descriptors must be present");
        }
        const auto parent_identity = linux_roy_identity(getpid());
        owned_files::Fd parent_process{int(syscall(SYS_pidfd_open, getpid(), 0))};
        if (parent_process.value < 0)
            throw std::runtime_error("Supervisor parent pidfd unavailable");
        int sockets[2];
        if (socketpair(AF_UNIX, SOCK_SEQPACKET | SOCK_CLOEXEC, 0, sockets))
            throw std::runtime_error("Supervisor private channel failed");
        channel_.value = sockets[0];
        owned_files::Fd child_channel(sockets[1]);
        const auto child = fork();
        if (child < 0)
            throw std::runtime_error("Supervisor fork failed");
        if (child == 0) {
            ::close(channel_.value);
            if (prctl(PR_SET_PDEATHSIG, 0) || setsid() < 0)
                _exit(121);
            const auto detached = fork();
            if (detached < 0)
                _exit(122);
            if (detached > 0)
                _exit(0);
            sigset_t empty;
            sigemptyset(&empty);
            if (sigprocmask(SIG_SETMASK, &empty, nullptr))
                _exit(123);
            for (int signal : {SIGINT, SIGTERM, SIGHUP, SIGQUIT, SIGPIPE})
                if (::signal(signal, SIG_IGN) == SIG_ERR)
                    _exit(125);
            const int null_fd = ::open("/dev/null", O_RDWR | O_CLOEXEC);
            if (null_fd < 0)
                _exit(126);
            for (int fd : {0, 1}) {
                if (dup2(null_fd, fd) < 0)
                    _exit(127);
            }
            ::close(null_fd);
            // Drop inherited file authority before constructing real resources.
            try {
                std::vector<int> inherited;
                for (const auto& entry : std::filesystem::directory_iterator("/proc/self/fd")) {
                    const int fd = std::stoi(entry.path().filename().string());
                    if (fd > 2 && fd != child_channel.value && fd != parent_process.value)
                        inherited.push_back(fd);
                }
                for (int fd : inherited)
                    ::close(fd);
            } catch (...) {
                _exit(124);
            }
            // Heap retention is intentional: errors cannot run owner destructors.
            SupervisedRoyOwner* owner = nullptr;
            try {
                supervisor_detail::send(child_channel.value, "pid:" + std::to_string(getpid()));
                // The launcher must reap the intermediate fork before any
                // strict /proc scan: otherwise its disappearance races startup.
                // The private channel and exact PID bind this launch barrier.
                if (supervisor_detail::receive(child_channel.value, 3000) !=
                    "launch:" + std::to_string(getpid()))
                    throw std::runtime_error("Supervisor launch acknowledgement invalid");
                owner = new SupervisedRoyOwner(journal, session, lock, child_channel.value,
                                               parent_process.value, parent_identity);
                const auto deadline = supervisor_detail::now() + 180000000000LL;
                try {
                    try{work.start(*owner);}catch(const std::exception& e){std::cerr<<e.what()<<"; continuing session with available state\n";}
                    while (supervisor_detail::now() < deadline) {
                        if(!owner->parent_alive())break;
                        pollfd events[2]{{child_channel.value, POLLIN, 0}, {parent_process.value, POLLIN, 0}};
                        const auto status = poll(events, 2, 10);
                        if (status > 0 && events[1].revents)
                            break;
                        if (status > 0 && (events[0].revents & POLLIN)) {
                            if (supervisor_detail::receive(child_channel.value, 0) != "stop")
                                {std::cerr<<"Unexpected supervisor parent command; ignoring\n";continue;}
                            break;
                        }
                        if(status<0){std::cerr<<"Supervisor runtime poll failed; retrying\n";continue;}
                        try{if(!work.step(*owner))break;}catch(const std::exception& e){std::cerr<<e.what()<<"; dropping iteration, continuing session\n";}
                    }
                } catch (const std::exception& error) {
                    // A partially started owner still requires explicit cleanup.
                    owner->failed_run(error.what());
                } catch (...) {
                    owner->failed_run("unknown non-standard exception");
                }
                try{work.stop_join(*owner);}catch(const std::exception& e){std::cerr<<e.what()<<"; continuing SRAM cleanup\n";}
                owner->restore_report_release();
                _exit(0);
            } catch (const std::exception& error) {
                // Parent death, ambiguous journal/IO/report or failed restoration
                // keeps the actual debugger/lock/owner and journal alive.
                try {
                    supervisor_detail::send(child_channel.value, std::string("retained:") + std::string(error.what()).substr(0, 1024));
                } catch (...) {}
                if (!owner || owner->may_exit())
                    _exit(0); // no unresolved SRAM ownership
                supervisor_detail::retain();
            } catch (...) {
                if (!owner || owner->may_exit())
                    _exit(0);
                supervisor_detail::retain();
            }
        }
        int status = 0;
        pid_t waited;
        do { waited = waitpid(child, &status, 0); } while (waited < 0 && errno == EINTR);
        if (waited != child || !WIFEXITED(status) || WEXITSTATUS(status) != 0)
            throw std::runtime_error("Supervisor detach failed");
        const auto identity = supervisor_detail::receive(channel_.value, 3000);
        if (identity.compare(0, 4, "pid:") != 0)
            throw std::runtime_error("Supervisor identity message missing");
        const auto pid = std::stol(identity.substr(4));
        if (pid <= 0 || identity != "pid:" + std::to_string(pid))
            throw std::runtime_error("Supervisor identity invalid");
        process_.value = int(syscall(SYS_pidfd_open, pid, 0));
        if (process_.value < 0)
            throw std::runtime_error("Supervisor pidfd unavailable");
        supervisor_detail::send(channel_.value, "launch:" + std::to_string(pid));
    }
    // Called by the production parent event loop; no caller-supplied barrier.
    // ACK is sent only after strict session/chain/literal-true validation.
    bool consume(int timeout_ms = 3000) {
        const auto bytes = supervisor_detail::receive(channel_.value, timeout_ms);
        if (bytes.compare(0, 9, "retained:") == 0)
            {std::cerr<<"Supervisor retained resources: "<<bytes.substr(9)<<"; owner remains alive\n";released_=true;return true;}
        const auto head = reports_.consume(bytes);
        supervisor_detail::send(channel_.value, head);
        if (!reports_.complete())
            return false;
        const auto verdict=supervisor_detail::receive(channel_.value, timeout_ms);
        const auto failed_prefix="released-failed:"+head+":";
        const bool run_failed=verdict.compare(0,failed_prefix.size(),failed_prefix)==0 &&
            verdict.size()>failed_prefix.size() && verdict.size()<=failed_prefix.size()+1024;
        if (verdict != "released:" + head && !run_failed)
            std::cerr<<"Supervisor resource release unverified; no process termination requested\n";
        pollfd event{process_.value, POLLIN, 0};
        if (poll(&event, 1, timeout_ms) != 1 || !(event.revents & POLLIN))
            std::cerr<<"Supervisor termination unverified; owner left alive\n";
        released_ = true;
        if(run_failed)std::cerr<<"Supervised runtime failed; restoration and release verified: "<<verdict.substr(failed_prefix.size())<<"; cleanup complete\n";
        return true;
    }
    void request_stop() { supervisor_detail::send(channel_.value, "stop"); }
    bool report_ready(int timeout_ms){
        if(timeout_ms<0||timeout_ms>1000)throw std::runtime_error("Supervisor poll bound invalid");
        pollfd events[2]{{channel_.value,POLLIN,0},{process_.value,POLLIN,0}};
        int result;
        do { result=poll(events,2,timeout_ms); } while(result<0&&errno==EINTR);
        if(result<0){std::cerr<<"Supervisor report poll failed; waiting\n";return false;}
        if(events[0].revents&POLLIN)return true;
        if(events[0].revents||events[1].revents){
            std::cerr<<"Supervisor lost before verified release; no owner termination requested\n";
            if(events[1].revents)released_=true;
        }
        return false;
    }
    bool complete() const { return released_; }
};

// Native parent report/cancellation loop. A quiet healthy child is not a failed
// 3s report read. Only readable messages enter the strict journal consumer.
// Cancellation/deadline sends one cooperative stop and waits at most 20s;
// unresolved ownership is retained and there is deliberately no signal/kill.
inline void supervise_native_parent(DurableSupervisor& supervisor,
    const std::function<bool()>& cancelled){
    if(!cancelled)throw std::runtime_error("Native parent cancellation source missing");
    const auto deadline=supervisor_detail::now()+180000000000LL;
    std::optional<int64_t> cleanup_deadline;
    while(!supervisor.complete()){
        const auto now=supervisor_detail::now();
        const bool stop=cancelled()||now>=deadline;
        pico::guard_evidence::emit(27,"parent-signal",!stop);
        if(!cleanup_deadline&&stop){
            try{supervisor.request_stop();}catch(const std::exception& e){std::cerr<<e.what()<<"; waiting for supervisor cleanup\n";}
            cleanup_deadline=now+20000000000LL;
        }
        if(cleanup_deadline&&now>=*cleanup_deadline)
            {std::cerr<<"Native parent cleanup exceeded 20s; supervisor retains owner\n";return;}
        // The release verdict follows profile/camera restoration, which includes the native XRService restart with
        // its >=10 s process-age wait, so the per-message wait must cover that (3 s was too short on hardware).
        if(supervisor.report_ready(50))try{
            (void)supervisor.consume(30000);
        }catch(const std::exception& e){std::cerr<<e.what()<<"; waiting for supervisor cleanup\n";}
    }
}
}
#endif

#include "live_implementation_registry.hpp"
#ifndef _WIN32
PICO_LIVE_IMPLEMENTATION(pico_impl_supervisor, "supervisor");
PICO_LIVE_IMPLEMENTATION(pico_impl_parent_authority, "parent-authority");
#endif
