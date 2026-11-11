#include "native_test_runtime.hpp"
#include <algorithm>
#include "fake_openocd.hpp"
#include "owner_lifecycle.hpp"
#include "roy_process_evidence.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#ifndef _WIN32
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
using namespace pico;
namespace {
size_t checks = 0;
void check(bool b, int line = __builtin_LINE()) {
    ++checks;
    if (!b)
        throw std::runtime_error("lifecycle check " + std::to_string(checks) + " line " +
                                 std::to_string(line));
}
template <class F> void rejects(F f, int line = __builtin_LINE()) {
    bool b = false;
    try {
        f();
    } catch (const std::exception &) {
        b = true;
    }
    check(b, line);
}
} // namespace
int main() {
    try {
        // Exclusion classifies the exact executable basename (a deleted binary still counts).
        check(roy_radio_command("/usr/bin/openocd"));
        check(roy_radio_command("/checkout/pico_producer"));
        check(roy_radio_command("/checkout/pico_producer_live_fixture (deleted)"));
        check(roy_radio_command("/opt/pico/roy_recover"));
        check(roy_radio_command("frame-pico-radio"));
        check(!roy_radio_command("/usr/bin/ordinary-process"));
        check(!roy_radio_command("/checkout/pico_producer_tests"));
        std::string stat="77 (name with ) parentheses) S";
        for(unsigned field=4;field<=21;++field)
            stat+=" 0";
        check(roy_process_start(stat+" 1234")==1234);
        rejects([&]{roy_process_start(stat);});
        rejects([&]{roy_process_start(stat+" -1");});
        SessionOwnership evidence;
        evidence.lock_path="/home/steamos/.local/state/frame-pico/roy.lock";
        evidence.lock_device=1;
        evidence.lock_inode=2;
        evidence.owner_start=3;
        evidence.owner_pid=4;
        evidence.lock_held=true;
        evidence.process_scan_complete=true;
        evidence.service_status=3;
        evidence.service_state="inactive";
        verify_session_ownership(evidence);
        evidence.service_state="failed";
        verify_session_ownership(evidence);
        evidence.service_status=4;
        evidence.service_state="unknown";
        verify_session_ownership(evidence); // Python accepts known absent unit/status4.
        evidence.service_status=3;
        verify_session_ownership(evidence);
        evidence.service_status=1;
        rejects([&]{verify_session_ownership(evidence);});
        evidence.service_status=3;
        evidence.service_state="active";
        rejects([&]{verify_session_ownership(evidence);});
        for (unsigned scenario = 0; scenario < 6; ++scenario) {
            FakeOpenOcd f;
            DebugMemory d([&](const std::string &c) { return f.command(c); });
            SramOwner owner(f.io(d), [](const Checkpoint &) {});
            int64_t ns = 0;
            unsigned connects = 0, waits = 0;
            std::vector<std::string> events;
            DebuggerLifecycle process(
                owner,
                {[&] { return ns; },
                 [&](int64_t n) {
                     check(n == 50000000);
                     ns += n;
                 },
                 [&](int64_t timeout, bool configure) {
                     ++connects;
                     events.push_back("connect");
                     if (connects == 1) {
                         check(timeout == 200000000 && !configure);
                         return scenario == 0;
                     }
                     check(timeout == 1000000000 && configure);
                     return scenario != 1 && scenario != 2;
                 },
                 [&] { events.push_back("socket_close"); },
                 [&](const std::vector<std::string> &argv) {
                     events.push_back("launch");
                     check(argv ==
                           std::vector<std::string>(
                               {"openocd", "-f",
                                "/opt/steamvr/drivers/frame_hmd/resources/firmware/roy/openocd.cfg",
                                "-c",
                                "bindto 127.0.0.1; gdb_port disabled; telnet_port disabled; "
                                "tcl_port 6666; init; poll; poll off"}));
                 },
                 [&] { return scenario == 2; },
                 [&](int64_t timeout) {
                     events.push_back("wait");
                     check(timeout == 2000000000);
                     return ++waits > (scenario == 4 ? 2u : 0u);
                 },
                 [&] { events.push_back("terminate"); }, [&] { events.push_back("kill"); },
                 [&] { events.push_back("log_close"); },
                 [&](const std::string &c) {
                     check(c == "shutdown");
                     events.push_back(c);
                     if (scenario == 5)
                         throw std::runtime_error("transport");
                 }});
            // Existing listener (0), TCL timeout (1) and early exit (2) are logged; the
            // debugger stays owned and is still closed through the normal path.
            process.start();
            check(process.retained());
            check(std::find(events.begin(), events.end(), "shutdown") == events.end());
            if (scenario == 0)
                check(events == std::vector<std::string>(
                                    {"connect", "socket_close", "launch", "connect"}));
            if (scenario == 1)
                check(ns == 8000000000LL && connects == 161);
            if (scenario == 2)
                check(connects == 1 && events == std::vector<std::string>({"connect", "launch"}));
            process.close();
            check(!process.retained());
            check((std::find(events.begin(), events.end(), "kill") != events.end()) ==
                  (scenario == 4));
        }
        // A real synthetic SRAM transaction bars every debugger teardown operation.
        FakeOpenOcd f;
        Bytes stock(0x19000, 0x91);
        f.stock(stock);
        DebugMemory d([&](const std::string &c) { return f.command(c); });
        SramOwner owner(f.io(d), [](const Checkpoint &) {});
        unsigned calls = 0, connects = 0;
        DebuggerLifecycle process(owner,
                                  {[] { return int64_t(0); }, [](int64_t) {},
                                   [&](int64_t, bool) { return ++connects > 1; }, [&] { ++calls; },
                                   [](const std::vector<std::string> &) {}, [] { return false; },
                                   [](int64_t) { return true; }, [&] { ++calls; }, [&] { ++calls; },
                                   [&] { ++calls; }, [&](const std::string &) { ++calls; }});
        process.start();
        Bytes state(2048);
        auto magic = le32(0x314d5250);
        std::copy(magic.begin(), magic.end(), state.begin());
        state[84] = 255;
        owner.install(stock, {1, 2, 3, 4}, roy_code | 1, state);
        rejects([&] { process.close(); });
        check(calls == 0 && process.retained());
        owner.restore();
        process.close();
        check(!process.retained() && calls == 3);
#ifndef _WIN32
        // Every Linux assertion includes name/line/actual/expected.
        auto linux_check=[&](const char* name,int line,bool actual,bool expected) {
            ++checks;
            if(actual!=expected)
                throw std::runtime_error(std::string(name)+" line="+std::to_string(line)
                    +" actual="+std::to_string(actual)+" expected="+std::to_string(expected));
        };
        auto linux_reject=[&](const char* name,int line,const std::function<void()>& fn) {
            bool threw=false;
            try {
                fn();
            } catch(const std::exception&) {
                threw=true;
            }
            linux_check(name,line,threw,true);
        };
        const auto path =
            std::filesystem::absolute(std::filesystem::path(__FILE__).parent_path() / "build"
                / ("roy-test-"+std::to_string(getpid())+".lock")).string();
        linux_check("proc-self-identity",__LINE__,roy_process_start(roy_proc_read("/proc/self/stat"))>0,true);
        linux_reject("proc-read-missing",__LINE__,[]{roy_proc_read("/proc/self/no-such-roy-evidence");});
        RoyLock a, b;
        a.acquire(path);
        linux_check("lock-held",__LINE__,a.held(),true);
        a.verify();
        // A competing holder is logged (path/holder on stderr); the lock is not taken.
        b.acquire(path);
        linux_check("competing-lock-not-held",__LINE__,b.held(),false);
        linux_check("first-lock-kept",__LINE__,a.held(),true);
        const auto child=fork();
        linux_check("fork-created",__LINE__,child>=0,true);
        if(child==0) {
            // An inherited descriptor is logged by verify(), never thrown.
            bool threw=false;
            try {
                a.verify();
            } catch(const std::exception&) {
                threw=true;
            }
            _exit(threw?1:0);
        }
        int status=0;
        linux_check("fork-reaped",__LINE__,waitpid(child,&status,0)==child,true);
        linux_check("inherited-lock-logged",__LINE__,WIFEXITED(status)&&WEXITSTATUS(status)==0,true);
        a.release(owner);
        b.acquire(path);
        b.release(owner);
        linux_check("locks-released",__LINE__,!a.held()&&!b.held(),true);
        const auto alias=path+".alias";
        // Unsafe lock files are logged and never acquired (no lock is followed or shared).
        std::filesystem::create_symlink(path,alias);
        a.acquire(alias);
        linux_check("symlink-lock-not-held",__LINE__,a.held(),false);
        std::filesystem::remove(alias);
        std::filesystem::create_directory_symlink(std::filesystem::path(path).parent_path(),alias);
        a.acquire(alias+"/nested.lock");
        linux_check("parent-symlink-lock-not-held",__LINE__,a.held(),false);
        std::filesystem::remove(alias);
        linux_check("fifo-created",__LINE__,mkfifo(alias.c_str(),0600)==0,true);
        a.acquire(alias);
        linux_check("fifo-lock-not-held",__LINE__,a.held(),false);
        std::filesystem::remove(alias);
        std::filesystem::create_hard_link(path,alias);
        a.acquire(path);
        linux_check("hardlinked-lock-not-held",__LINE__,a.held(),false);
        std::filesystem::remove(alias);
        a.acquire(path);
        linux_check("lock-reacquired",__LINE__,a.held(),true);
        std::filesystem::rename(path,alias);
        {
            std::ofstream replacement(path);
            replacement<<"replacement";
        }
        // A replaced name is logged; our own descriptor is still released (SRAM resolved).
        a.verify();
        a.release(owner);
        linux_check("replaced-lock-released",__LINE__,a.held(),false);
        std::filesystem::remove(path);
        std::filesystem::remove(alias);
#else
        rejects([] { roy_lock_path(); });
#endif
        std::cout << checks
                  << " debugger lifecycle checks PASS (fake process; no OpenOCD launch)\n";
        return 0;
    } catch (const std::exception &e) {
        std::cerr << "lifecycle_tests name=setup-or-check line=" << __LINE__
                  << " actual=" << e.what() << " expected=success\n";
        return 1;
    }
}
