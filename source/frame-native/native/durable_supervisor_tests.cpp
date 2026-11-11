#include "native_test_runtime.hpp"
#include "supervisor_report.hpp"
#include "durable_supervisor.hpp"
#include "fake_openocd.hpp"
#include "live_event_sink_linux.hpp"
#include "roy_process_evidence.hpp"
#include <iostream>
#include <algorithm>

namespace {
int checks = 0;
void check(bool actual, const char* name, int line) {
    ++checks;
    std::cout << name << " line=" << line << " actual=" << (actual ? "true" : "false") << " expected=true\n";
    if (!actual)
        throw std::runtime_error("Supervisor test failed");
}
#define REQUIRE(expr) check(bool(expr), #expr, __LINE__)
template<class F> bool rejects(F operation) {
    try { operation(); } catch (const std::exception& error) {
        std::cout << "expected rejection reason=" << error.what() << '\n';
        return true;
    }
    return false;
}
#ifndef _WIN32
void process_scan_regressions() {
    REQUIRE(!pico::roy_radio_command("/usr/bin/ssh openocd pico_producer"));
    REQUIRE(!pico::roy_radio_command("/usr/bin/grep"));
    REQUIRE(pico::roy_radio_command("/usr/bin/openocd"));
    REQUIRE(pico::roy_radio_command("/tmp/run/pico_producer"));
    // PID 1 belongs to another user on the Frame. Its unreadable exe must not
    // obstruct enumeration when argv/comm do not select an RF owner.
    std::error_code foreign_error;
    (void)std::filesystem::read_symlink("/proc/1/exe",foreign_error);
    const auto foreign_command=pico::roy_proc_read("/proc/1/cmdline");
    const auto foreign_comm=pico::roy_proc_read("/proc/1/comm");
    REQUIRE(!pico::roy_radio_command(foreign_command)&&!pico::roy_radio_command(foreign_comm));
    if(foreign_error==std::errc::permission_denied)
        std::cout << "PID 1 unreadable foreign exe regression active\n";
    else
        std::cout << "PID 1 EACCES condition SKIPPED: host permits exe read or uses different proc policy\n";

    // A real child supplies both readable and nondumpable candidate cases.
    // Keep it alive until after scanning, and reap on every assertion failure.
    for(bool unreadable:{false,true}) {
        int ready[2],stop[2];
        REQUIRE(pipe(ready)==0);
        if(pipe(stop)!=0) { close(ready[0]);close(ready[1]);throw std::runtime_error("stop pipe failed"); }
        const auto child=fork();
        if(child==0) {
            close(ready[0]);close(stop[1]);
            const bool ok=prctl(PR_SET_NAME,"openocd") == 0 &&
                (!unreadable||prctl(PR_SET_DUMPABLE,0)==0);
            const char status=ok?'1':'0';
            (void)!write(ready[1],&status,1);
            char done;
            (void)!read(stop[0],&done,1);
            _exit(ok?0:1);
        }
        close(ready[1]);close(stop[0]);
        if(child<0) { close(ready[0]);close(stop[1]);throw std::runtime_error("candidate fork failed"); }
        try {
            char status=0;
            REQUIRE(read(ready[0],&status,1)==1&&status=='1');
            std::error_code error;
            const auto exe=std::filesystem::read_symlink("/proc/"+std::to_string(child)+"/exe",error).string();
            if(!unreadable) REQUIRE(!error&&!exe.empty());
            const auto scan=pico::linux_roy_processes();
            const auto found=std::find_if(scan.begin(),scan.end(),[&](const pico::RoyProcessEvidence& row){return row.pid==child;});
            REQUIRE(found==scan.end());


            REQUIRE(std::none_of(scan.begin(),scan.end(),[](const pico::RoyProcessEvidence& row){return row.pid==1;}));
            if(unreadable&&!error)
                std::cout << "Nondumpable candidate exe is readable with host privilege; EACCES assertion SKIPPED\n";
        } catch(...) {
            close(ready[0]);close(stop[1]);
            while(waitpid(child,nullptr,0)<0&&errno==EINTR) {}
            throw;
        }
        close(ready[0]);close(stop[1]);
        int status=0;
        REQUIRE(waitpid(child,&status,0)==child);
        REQUIRE(WIFEXITED(status)&&WEXITSTATUS(status)==0);
    }
}
// Real SramOwner/codecs/journal/process isolation; synthetic MCU and debugger.
// No injected failure is allowed to leave a detached unresolved test owner.
// GCC 15 at -O1 with ASan/UBSan diagnoses libstdc++ std::function's
// temporary manager/invoker during callback assignment. Limit the workaround
// to these synthetic callback factories; production and assertions keep -Werror.
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ == 15 && defined(__SANITIZE_ADDRESS__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
struct Fixture {
    pico::FakeOpenOcd mcu;
    pico::DebugMemory memory;
    pico::Bytes image = pico::Bytes(0x19000, 0x91);
    std::optional<int> exit;
    unsigned steps = 0;
    std::function<void()> before_shutdown;
    Fixture() : memory([this](const std::string& command) { return mcu.command(command); }) { mcu.stock(image); }
    pico::OpenOcdHooks hooks() {
        pico::OpenOcdHooks result;
        result.now = [this] { return mcu.ns; };
        result.sleep = [this](int64_t ns) { mcu.ns += ns; };
        result.listener_present = [] { return false; };
        result.spawn = [](const std::vector<std::string>&) {};
        result.poll = [this] { return exit; };
        result.connect = [] { return true; };
        result.command = [this](const std::string& command) {
            if (command == "shutdown") {
                if(before_shutdown)before_shutdown();
                exit = 0;
            }
            return std::string("ok");
        };
        result.disconnect = [] {};
        result.terminate = [] { throw std::runtime_error("Unexpected terminate"); };
        result.kill = [] { throw std::runtime_error("Unexpected kill"); };
        result.verify_attribution = [] {};
        return result;
    }
};
pico::SupervisorWork work() {
    // Created in final child, not in the launching parent.
    pico::SupervisorWork result;
    struct Lifetime {
        bool camera_open=true,worker_joined=false,finalized=false;
        std::shared_ptr<Fixture> fixture;
    };
    auto lifetime=std::make_shared<Lifetime>();
    const auto launcher = getpid();
    result.start = [launcher,lifetime](pico::SupervisedRoyOwner& owner) {
        if (prctl(PR_SET_NAME, "roy ) owner )") != 0)
            throw std::runtime_error("Cannot set regression comm");
        // This test is a subreaper. The intermediate child must have been
        // reaped before work starts, so it cannot disappear during strict scan.
        const auto children = pico::roy_proc_read("/proc/" + std::to_string(launcher) +
            "/task/" + std::to_string(launcher) + "/children");
        std::istringstream child_ids(children);
        int64_t child = 0, extra = 0;
        if (!(child_ids >> child) || child != getpid() || (child_ids >> extra))
            throw std::runtime_error("Supervisor started before intermediate fork was reaped");
        const auto ownership = owner.ownership();
        if (std::find(ownership.radio_processes.begin(), ownership.radio_processes.end(),
                      int64_t(getppid())) != ownership.radio_processes.end())
            throw std::runtime_error("Authenticated parent was not excluded");
        auto fixture = std::make_shared<Fixture>();
        lifetime->fixture=fixture;
        fixture->before_shutdown=[lifetime] {
            if(lifetime->camera_open||!lifetime->finalized)
                throw std::runtime_error("Debugger closed before camera/session finalizer");
        };
        owner.start_debugger(fixture->hooks(), fixture);
        owner.close_session_after_barriers([lifetime,&owner] {
            if(!lifetime->worker_joined||!lifetime->camera_open||
               !owner.sram().barriers.complete()||owner.sram().unresolved())
                throw std::runtime_error("Camera closed before worker join/four MCU barriers");
            lifetime->camera_open=false;
            lifetime->finalized=true;
        });
        auto& sram = owner.prepare_sram(fixture->mcu.io(fixture->memory));
        pico::Bytes state(2048);
        const auto magic = pico::le32(0x314d5250);
        std::copy(magic.begin(), magic.end(), state.begin());
        state[84] = 255;
        sram.install(fixture->image, pico::Bytes{1, 2, 3, 4, 5}, pico::roy_code | 1, state);
        sram.enter([] {});
    };
    result.step = [](pico::SupervisedRoyOwner&) { return false; };
    result.stop_join = [lifetime](pico::SupervisedRoyOwner& owner) {
        if(lifetime->fixture) {
            if(!lifetime->camera_open||owner.sram().barriers.complete())
                throw std::runtime_error("Dispatch stop must precede barriers and retain camera");
            lifetime->worker_joined=true;
        }
    };
    return result;
}
void configure_fault(pico::SupervisorWork& task, int run, const std::filesystem::path& directory) {
    using namespace pico;
    if (run >= 4) {
        task.start = [run,directory](SupervisedRoyOwner& owner) {
            auto fixture = std::make_shared<Fixture>();
            if (run != 7)
                owner.start_debugger(fixture->hooks(),fixture);
            const bool refused = run == 7 || rejects([&] { owner.command("mww 0x20000000 1"); });
            owner.close_session_after_barriers([fixture,directory,refused] {
                if (fixture->exit)
                    throw std::runtime_error("Debugger closed before observer");
                const auto path = directory.parent_path() / (directory.filename().string() + "-observer.closed");
                owned_files::Fd proof(::open(path.c_str(),O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC,0600));
                if (proof.value < 0)
                    throw std::runtime_error("Observer closure proof failed");
                owned_files::write(proof.value,refused ? "closed;mutation-refused" : "closed;mutation-admitted");
            });
            if (run >= 5 && run != 7)
                (void)owner.prepare_sram(fixture->mcu.io(fixture->memory));
            if (run != 6)
                throw std::runtime_error("Injected before first checkpoint");
        };
    }
    if (run == 1 || run == 3)
        task.step = [](SupervisedRoyOwner&) { return true; };
    if (run == 2)
        task.step = [remaining=1](SupervisedRoyOwner&) mutable ->bool {if(remaining-- > 0)throw std::runtime_error("Injected runtime failure");
        return false;};
}
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ == 15 && defined(__SANITIZE_ADDRESS__)
#pragma GCC diagnostic pop
#endif
#endif
}
int main() {
    try {
        using namespace pico;
        {
            int64_t now=0;int kills=0;std::optional<int> exited;
            OpenOcdHooks hooks;
            hooks.now=[&]{return now;};hooks.sleep=[&](int64_t ns){now+=ns;};
            hooks.listener_present=[]{return true;};hooks.spawn=[](const std::vector<std::string>&){};
            hooks.poll=[&]{return exited;};hooks.connect=[]{return true;};
            hooks.command=[&](const std::string& command){if(command=="shutdown")exited=0;
            return std::string("ok");};
            hooks.disconnect=[]{};
            hooks.terminate=[&]{++kills;exited=0;};hooks.kill=[&]{++kills;exited=0;};
            hooks.verify_attribution=[]{throw std::runtime_error("attribution mismatch");};
            OpenOcdProcess debugger(hooks);debugger.start();
            REQUIRE(debugger.command("read")=="ok");
            debugger.before_sram_mutation();
            debugger.restored(false,false,false,false);
            REQUIRE(rejects([&]{debugger.close();}));
            REQUIRE(kills==0);
            debugger.restored(true,true,true,true);  // the kept rule: close only after full SRAM restoration
            debugger.close();REQUIRE(kills<=1);
            hooks.listener_present=[]{return false;};hooks.connect=[]{return false;};
            exited.reset();now=0;
            OpenOcdProcess timeout(hooks);timeout.start();REQUIRE(now>=8000000000LL);
            REQUIRE(timeout.command("read").empty());timeout.close();
        }
        // /proc field 22 is not necessarily positive for boot-time kernel
        // threads. Reproduce the scan failure without depending on host uptime.
        auto stat = [](const std::string& state, uint64_t flags, const std::string& start) {
            std::string row="2 (kernel ) with )) spaces) "+state;
            for(unsigned field=4;field<=21;++field)
                row+=" "+std::to_string(field==9?flags:0);
            return row+" "+start+" 999 888";
        };
        const auto kernel_zero=stat("S",0x200000ULL,"0");
        REQUIRE(rejects([&] { roy_process_start(kernel_zero); }));
        REQUIRE(roy_scan_process_start(kernel_zero,"",true)==0);
        REQUIRE(roy_scan_process_start(stat("S",0,"12345"),"owner",false)==12345);
        REQUIRE(rejects([&] { roy_scan_process_start(stat("Z",0,"0"),"",true); }));
        REQUIRE(rejects([&] { roy_scan_process_start(stat("S",0,"0"),"",true); }));
        REQUIRE(rejects([&] { roy_scan_process_start(kernel_zero,"owner",true); }));
        REQUIRE(rejects([&] { roy_scan_process_start(kernel_zero,"",false); }));
        REQUIRE(rejects([&] { roy_scan_process_start(stat("S",0x200000ULL,"-1"),"",true); }));
        REQUIRE(rejects([&] { roy_scan_process_start("2 (truncated) S","",true); }));
        REQUIRE(rejects([&] { roy_scan_process_start(stat("S",0x200000ULL,"18446744073709551616"),"",true); }));
        REQUIRE(rejects([&] { roy_scan_process_start("malformed","",true); }));
#ifndef _WIN32
        process_scan_regressions();
        // Only the captured live parent may be excluded; identical names and
        // arbitrary inherited descriptors never confer scan authority.
        char old_comm[16]{};
        REQUIRE(prctl(PR_GET_NAME, old_comm)==0);
        const auto original_start=roy_process_start(roy_proc_read("/proc/self/stat"));
        REQUIRE(prctl(PR_SET_NAME,"roy ) test ))")==0);
        const auto identity = linux_roy_identity(getpid());
        REQUIRE(identity.pid==getpid() && identity.start==original_start);
        REQUIRE(roy_proc_read("/proc/self/stat").find("(roy ) test )))")!=std::string::npos);
        REQUIRE(prctl(PR_SET_NAME,old_comm)==0);
        owned_files::Fd authority{int(syscall(SYS_pidfd_open, getpid(), 0))};
        REQUIRE(authority.value >= 0);
        auto other = identity;
        other.pid += 1000000;
        std::vector<RoyProcessEvidence> scan{identity, other};
        exclude_authenticated_parent(scan, identity, authority.value);
        REQUIRE(scan.size() == 1 && scan[0].pid == other.pid);
        auto changed = identity;
        changed.start=0;
        REQUIRE(!rejects([&] { exclude_authenticated_parent(scan, changed, authority.value); }));
        changed = identity;
        ++changed.start;
        REQUIRE(!rejects([&] { exclude_authenticated_parent(scan, changed, authority.value); }));
        changed = identity;
        changed.command += " forged";
        REQUIRE(!rejects([&] { exclude_authenticated_parent(scan, changed, authority.value); }));
        REQUIRE(!rejects([&] { exclude_authenticated_parent(scan, identity, -1); }));
        owned_files::Fd ordinary(::open("/proc/self/stat", O_RDONLY | O_CLOEXEC));
        REQUIRE(!rejects([&] { exclude_authenticated_parent(scan, identity, ordinary.value); }));
#endif
        const std::string session(64, 'a');
        RecoveryRecord checkpoint{1, session, "checkpoint", std::string(64, '0'), "checkpoint fixture"};
        const auto first = recovery_encode(checkpoint);
        SupervisorReportConsumer parent(session);
        REQUIRE(parent.consume(first) == phase5::sha256(first));
        REQUIRE(!parent.complete());
        REQUIRE(rejects([&] { parent.consume(first); }));
        RecoveryRecord second{2, session, "checkpoint", phase5::sha256(first), "second checkpoint fixture"};
        const auto next = recovery_encode(second);
        REQUIRE(parent.consume(next) == phase5::sha256(next));
        RecoveryRecord report{3, session, "barriers", phase5::sha256(next), CleanupBarriers{true,true,true,true}.json()};
        for (int fault = 0; fault < 9; ++fault) {
            auto bad = report;
            switch (fault) {
            case 0: bad.session = std::string(64, 'b'); break;
            case 1: bad.previous = phase5::sha256(first); break;
            case 2: bad.sequence = 4; break;
            case 3: bad.kind = "readback"; break;
            case 4: bad.payload.replace(bad.payload.find("true"), 4, "1"); break;
            case 5: bad.payload.replace(bad.payload.find("true"), 4, "\"true\""); break;
            case 6: bad.payload.replace(bad.payload.find("true"), 4, "false"); break;
            case 7: bad.payload = "{}"; break;
            case 8: bad.payload += "garbage"; break;
            }
            REQUIRE(rejects([&] { parent.consume(recovery_encode(bad)); }));
            REQUIRE(!parent.complete());
        }
        const auto final = recovery_encode(report);
        REQUIRE(parent.consume(final) == phase5::sha256(final));
        REQUIRE(parent.complete());
        REQUIRE(rejects([&] { parent.consume(final); }));
        SupervisorReportConsumer absent(session);
        REQUIRE(rejects([&] { absent.consume(final); }));
        REQUIRE(rejects([&] { SupervisorReportConsumer invalid("bad"); }));
        const RecoveryRecord unmutated{1,session,"unmutated",std::string(64,'0'),"no-sram-write;resources-closed"};
        SupervisorReportConsumer early(session);
        REQUIRE(early.consume(recovery_encode(unmutated)) == phase5::sha256(recovery_encode(unmutated)));
        REQUIRE(early.complete());
        REQUIRE(rejects([&] { early.consume(recovery_encode(unmutated)); }));
        REQUIRE(rejects([&] { parent.consume(recovery_encode(unmutated)); }));
        auto bad_unmutated = unmutated;
        bad_unmutated.payload = "no-sram-write";
        SupervisorReportConsumer malformed(session);
        REQUIRE(rejects([&] { malformed.consume(recovery_encode(bad_unmutated)); }));
        SupervisorReportConsumer checkpoint_first(session);
        REQUIRE(checkpoint_first.consume(first) == phase5::sha256(first));
        auto late_unmutated = unmutated;
        late_unmutated.sequence = 2;
        late_unmutated.previous = phase5::sha256(first);
        REQUIRE(rejects([&] { checkpoint_first.consume(recovery_encode(late_unmutated)); }));
        REQUIRE(!checkpoint_first.complete());
#ifndef _WIN32
        REQUIRE(prctl(PR_SET_CHILD_SUBREAPER, 1) == 0);
        const auto base = std::filesystem::current_path() / "build" / ("supervisor-" + std::to_string(getpid()));
        REQUIRE(::mkdir(base.c_str(), 0700) == 0);
        {
            const auto path=base/"live-events.jsonl";
            LiveEventSink sink(path);
            sink.append("proof","original\nquoted\"body");
            REQUIRE(rejects([&]{LiveEventSink duplicate(path);}));
            owned_files::Fd input(::open(path.c_str(),O_RDONLY|O_CLOEXEC));
            const auto bytes=owned_files::read(input.value,4096);
            const auto row=phase5::parse(bytes);
            REQUIRE(phase5::field(row,"detail").text=="original\nquoted\"body");
            REQUIRE(phase5::integer(phase5::field(row,"sequence"))==1);
            REQUIRE(rejects([&]{sink.append("","");}));
            REQUIRE(rejects([&]{sink.append("proof","cannot resurrect");}));
        }
        {
            const auto directory=base/"finished-journal";
            REQUIRE(::mkdir(directory.c_str(),0700)==0);
            {RecoveryJournal old(directory,session);REQUIRE(!old.append("checkpoint","old completed run").empty());}
            std::ostringstream log;auto* previous=std::cerr.rdbuf(log.rdbuf());
            {RecoveryJournal next(directory,std::string(64,'b'));REQUIRE(next.records().empty());REQUIRE(next.append("checkpoint","new run").empty());}
            std::cerr.rdbuf(previous);REQUIRE(!log.str().empty());
        }
        for (int run = 0; run < 8; ++run) {
            std::cout << "supervised runtime run=" << run << std::endl;
            const auto directory = base / std::to_string(run);
            REQUIRE(::mkdir(directory.c_str(), 0700) == 0);
            auto task = work();
            configure_fault(task, run, directory);
            DurableSupervisor supervisor(directory, session, (base / "roy.lock").string(), std::move(task));
            REQUIRE(!supervisor.complete());
            if(run>=4){
                supervise_native_parent(supervisor,[] { return false; });
            }else if(run==3){
                supervise_native_parent(supervisor,[]{return true;});
            }else{
                REQUIRE(!supervisor.consume());
                REQUIRE(!supervisor.consume());
                REQUIRE(!supervisor.consume());
            }
            if (run == 1)
                supervisor.request_stop();
            if(run==2)REQUIRE(supervisor.consume());
            else if(run!=3 && run<4)REQUIRE(supervisor.consume());
            REQUIRE(supervisor.complete());
            int exit_status = 0;
            const auto reaped = waitpid(-1, &exit_status, WNOHANG);
            REQUIRE(reaped > 0);
            REQUIRE(WIFEXITED(exit_status) && WEXITSTATUS(exit_status) == 0);
            RecoveryJournal journal(directory, session);
            const auto records = journal.records();
            REQUIRE(records.size() == (run >= 4 ? 1u : 4u));
            if (run >= 4) {
                const auto proof_path = base / (std::to_string(run) + "-observer.closed");
                owned_files::Fd proof(::open(proof_path.c_str(),O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
                REQUIRE(proof.value >= 0);
                REQUIRE(owned_files::read(proof.value,64) == "closed;mutation-refused");
                REQUIRE(records.back().kind == "unmutated");
                REQUIRE(records.back().payload == "no-sram-write;resources-closed");
                REQUIRE(journal.append("checkpoint","forbidden after terminal result").empty());
            } else {
                REQUIRE((records.back().payload == CleanupBarriers{true,true,true,true}.json()));
            }
            owned_files::Fd lock(::open((base / "roy.lock").c_str(), O_RDWR | O_CLOEXEC | O_NOFOLLOW));
            REQUIRE(lock.value >= 0);
            REQUIRE(flock(lock.value, LOCK_EX | LOCK_NB) == 0);
        }
        int unexpected = 0;
        REQUIRE(waitpid(-1, &unexpected, WNOHANG) == -1 && errno == ECHILD);
#else
        std::cout << "Linux detached supervisor line=" << __LINE__ << " actual=SKIPPED expected=Linux target execution\n";
#endif
        std::cout << "durable_supervisor_tests checks=" << checks << " PASS\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "durable_supervisor_tests line=" << __LINE__ << " actual=" << error.what()
                  << " expected=successful setup and all checks\n";
        return 1;
    }
}
