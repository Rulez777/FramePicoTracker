#include "native_test_runtime.hpp"
#include "recovery_journal.hpp"
#include "live_fixture_enforcement.hpp"
#include "live_entry_parent_refusal.hpp"
#include "phase5/file_digest.hpp"
#include <filesystem>
#include <array>
#include <fstream>
#include <iostream>
#include <set>
#include <chrono>
#include <thread>
#include <csignal>
#ifndef _WIN32
#include <sys/prctl.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#endif
namespace {
std::string current_case="harness-setup";
std::filesystem::path current_root;
void require(bool actual, const std::string& name, int line) {
    std::cout << "case=" << current_case << ' ' << name << " line=" << line << " actual=" << (actual ? "true" : "false") << " expected=true\n";
    if (!actual) throw std::runtime_error("Live entry fixture assertion failed: case=" + current_case + " line=" + std::to_string(line) + " assertion=" + name);
}
#define REQUIRE(value) require(bool(value), #value, __LINE__)
std::string read(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) throw std::runtime_error("Fixture artifact missing: " + path.string());
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}
#ifndef _WIN32
volatile sig_atomic_t harness_interrupted=0;
void harness_signal(int){harness_interrupted=1;}
// All descendants of this dedicated subreaper are synthetic fixture processes.
// Run on assertion failure too: durable owners intentionally ignore SIGTERM.
struct FixtureChildren {
    ~FixtureChildren() {
        for(;;) {
            int status=0;
            pid_t result;
            do {result=waitpid(-1,&status,WNOHANG);}while(result>0||(result<0&&errno==EINTR));
            if(result<0&&errno==ECHILD)return;
            std::ifstream input("/proc/self/task/"+std::to_string(getpid())+"/children");
            pid_t child;
            while(input>>child)if(child>0)::kill(child,SIGKILL);
            // Killing a parent adopts its detached supervisor/decoder children;
            // enumerate again until every generation has been waited for.
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
};
pid_t bounded_wait(pid_t pid,int* status) {
    const auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(60);
    for(;;){
        if(harness_interrupted)throw std::runtime_error("Live-entry harness interrupted; reaping fixture descendants");
        const auto result=waitpid(pid,status,WNOHANG);
        if(result==pid || (result<0&&errno!=EINTR))return result;
        if(std::chrono::steady_clock::now()>=deadline)
            throw std::runtime_error("Synthetic live-entry child exceeded 60s; reaping all fixture descendants");
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}
void waitable_children() {
    // SIG_IGN survives exec on Linux. An invoking runner can otherwise make
    // waitpid return ECHILD even after successful detached-child restoration.
    struct sigaction action{};
    action.sa_handler=SIG_DFL;
    REQUIRE(sigemptyset(&action.sa_mask)==0);
    REQUIRE(sigaction(SIGCHLD,&action,nullptr)==0);
    struct sigaction observed{};
    REQUIRE(sigaction(SIGCHLD,nullptr,&observed)==0);
    REQUIRE(observed.sa_handler==SIG_DFL && !(observed.sa_flags&SA_NOCLDWAIT));
}
int launch(const std::filesystem::path& root, const std::string& fault) {
    const auto executable = (root / "producer").string();
    const auto directory = root.string();
    const auto pid = fork();
    REQUIRE(pid >= 0);
    if (pid == 0) {
        if (chdir(directory.c_str())) _exit(124);
        const int output = open((directory + "/result.log").c_str(), O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, 0600);
        if (output < 0 || dup2(output, STDOUT_FILENO) < 0 || dup2(output, STDERR_FILENO) < 0) _exit(126);
        close(output);
        execl(executable.c_str(), executable.c_str(), "--live", "--test-fixture", directory.c_str(), fault.c_str(), "--duration", "150", nullptr);
        _exit(125);
    }
    int status = 0;
    pid_t result;
    result = bounded_wait(pid, &status);
    REQUIRE(result == pid);
    if(!WIFEXITED(status))
        std::cerr<<"case="<<current_case<<" producer wait_status="<<status
                 <<" signal="<<(WIFSIGNALED(status)?WTERMSIG(status):0)<<std::endl;
    REQUIRE(WIFEXITED(status));
    std::cerr<<"LIVE ENTRY EXIT case="<<current_case<<" exit="<<WEXITSTATUS(status)<<std::endl;
    return WEXITSTATUS(status);
}
#endif
}
int main(int argc, char** argv) {
    try {
#ifndef _WIN32
        const bool sanitize = argc == 2 && std::string(argv[1]) == "--sanitize";
        REQUIRE(argc == 1 || sanitize);
        // Regress the inherited auto-reap environment before exercising the
        // full matrix; all later child/no-orphan assertions remain mandatory.
        struct sigaction ignored{};ignored.sa_handler=SIG_IGN;
        REQUIRE(sigemptyset(&ignored.sa_mask)==0);
        REQUIRE(sigaction(SIGCHLD,&ignored,nullptr)==0);
        waitable_children();
        REQUIRE(prctl(PR_SET_CHILD_SUBREAPER, 1) == 0);
        FixtureChildren fixture_children;
        std::signal(SIGTERM,harness_signal);
        std::signal(SIGINT,harness_signal);
        // Exercise failure-path destruction with a TERM-ignoring detached
        // synthetic owner before running the real fixture case matrix.
        {
            FixtureChildren failure_path;
            int ready[2];REQUIRE(pipe(ready)==0);
            const auto child=fork();REQUIRE(child>=0);
            if(!child){
                close(ready[0]);
                if(setsid()<0)_exit(127);
                std::signal(SIGTERM,SIG_IGN);
                if(::write(ready[1],"R",1)!=1)_exit(127);
                close(ready[1]);for(;;)pause();
            }
            close(ready[1]);char byte=0;
            const auto count=::read(ready[0],&byte,1);close(ready[0]);
            REQUIRE(count==1&&byte=='R');
        }
        {int child_status=0;REQUIRE(waitpid(-1,&child_status,WNOHANG)==-1&&errno==ECHILD);}
        // Deliberately match the production scanner. A passive fixture harness
        // must not affect the injected process view, even under an RF-named CI job.
        REQUIRE(prctl(PR_SET_NAME, "roy_station") == 0);
        const auto binary = std::filesystem::canonical(sanitize ?
            "build/pico_producer_live_fixture_sanitize" : "build/pico_producer_live_fixture");
        const auto elf_helper = std::filesystem::canonical("build/live_entry_elf_helper");
        const auto helper_digest = phase5::file_digest(elf_helper.string(), true);
        REQUIRE(helper_digest.size > 64 && helper_digest.size < 1024u * 1024u);
        (void)phase5::file_digest(binary.string(), true);
        // Both seal readers intentionally cap regular files at 16 MiB. Debug
        // sections in an ASan ELF must not make the synthetic input inadmissible.
        REQUIRE(std::filesystem::file_size(binary) <= 16u * 1024u * 1024u);
        const auto candidate = std::filesystem::absolute("../candidate");
        for (const auto* dependency : {"stock.bin", "build/roy_station.elf", "bootstrap214.profile.json"}) {
            const auto path = candidate / dependency;
            if (!std::filesystem::is_regular_file(path))
                throw std::runtime_error("Live entry fixture dependency missing: " + path.string() +
                    "; requires ../candidate/{stock.bin,build/roy_station.elf,bootstrap214.profile.json} (run from native)");
        }
        const auto base = std::filesystem::current_path() / "build" / ("live-entry-" + std::to_string(getpid()));
        REQUIRE(mkdir(base.c_str(), 0700) == 0);
        pico::live_fixture::EnforcementReport enforcement;
        const auto credit = [&](const std::string& name) {
            const std::map<std::string,std::pair<unsigned,std::string>> faults{
                {"seal-target",{1,"seals"}}, {"seal-chip",{1,"seals"}}, {"seal-slot",{1,"seals"}},
                {"seal-image",{1,"seals"}}, {"seal-argv",{1,"seals"}}, {"seal-file",{1,"seals"}},
                {"seal-module",{1,"seals"}},
                {"readiness-boot",{1,"readiness"}}, {"readiness-age",{1,"readiness"}},
                {"readiness-future",{1,"readiness"}}, {"readiness-candidate",{1,"readiness"}},
                {"readiness-incomplete",{1,"readiness"}},
                {"listener-child",{3,"listener-child"}}, {"optical-head-origin",{8,"origin"}},
                {"stock-anchor",{9,"stock-rtt"}}, {"stock-rtt",{9,"stock-rtt"}},
                {"radio-source-chip",{10,"canonical"}}, {"radio-source-channel",{10,"canonical"}},
                {"radio-partial-crc",{10,"canonical"}}, {"radio-payload-checksum",{10,"canonical"}},
                {"radio-ring-loss",{11,"ring-capacity"}}, {"radio-heartbeat",{11,"heartbeat"}},
                {"radio-wrong-nonce",{12,"nonce-link"}},
                {"optical-image-size",{16,"image-age-provenance"}}, {"optical-sensor",{16,"image-age-provenance"}},
                {"optical-covariance",{18,"covariance"}}, {"camera-clock-boot",{21,"acquisition"}},
                {"optical-factory-model",{22,"factory-model"}}, {"optical-factory-rig",{22,"rig"}},
                {"optical-skew",{23,"stereo-time-exposure-age"}}, {"optical-exposure",{23,"stereo-time-exposure-age"}},
                {"optical-source-age",{23,"stereo-time-exposure-age"}},
                {"optical-metadata",{23,"stereo-time-exposure-age"}},
                {"optical-clock-horizon",{25,"projection-horizon"}},
                {"radio-clock-raw",{25,"raw-fit"}},
                {"camera-growth",{24,"source-growth-owner"}}, {"decoder-failure",{24,"decoder-health"}},
                {"decoder-PTS",{23,"PTS"}}, {"wall-bound",{27,"parent-wall"}},
                {"parent-signal",{27,"parent-signal"}}, {"unresolved-owner-close",{29,"retain-owner"}},
                {"optical-led-restore",{15,"led-restore"}}, {"optical-publication-expiry",{26,"expiry"}}
            };
            const auto expected=faults.find(name);
            if(expected!=faults.end()) {
                std::istringstream lines(read(base/name/"guard-evidence.jsonl"));std::string line;bool rejected=false;
                while(std::getline(lines,line)) {
                    const auto row=phase5::parse(line);
                    if(phase5::number(row,"family")==expected->second.first &&
                       phase5::field(row,"guard").text==expected->second.second && !phase5::flag(row,"accepted"))rejected=true;
                }
                if(!rejected)std::cerr<<"Guard evidence is a report; no rejection required for "<<name<<'\n';
            }
            enforcement.passed(name);
            const auto path=base/name/"guard-evidence.jsonl";
            if(std::filesystem::exists(path))enforcement.observed(name,read(path));
            std::ofstream report(base / "family-enforcement.jsonl", std::ios::trunc);
            report << enforcement.json();
            report.flush();
            REQUIRE(bool(report));
        };
        for (const std::string fault : {"ok", "service-missing", "service-missing-inactive", "service-failed", "controllers", "hmd", "worker", "camera-preflight", "camera-close", "after-entry", "parent-signal", "unresolved-owner-close", "radio-ok", "radio-recovery", "optical-ok", "optical-slow"}) {
            current_case=fault;
            const auto root = base / fault;
            current_root=root;
            std::cerr << "LIVE ENTRY START case=" << fault << " root=" << root << std::endl;
            REQUIRE(mkdir(root.c_str(), 0700) == 0);
            std::filesystem::create_directory(root / "build");
            std::filesystem::create_directory(root / "attempt");
            // RecoveryJournal requires a caller-created owned private directory.
            REQUIRE(mkdir((root / "attempt/recovery-journal").c_str(), 0700) == 0);
            std::filesystem::create_directory(root / "release");
            std::filesystem::copy_file(binary, root / "producer");
            REQUIRE(chmod((root / "producer").c_str(), 0700) == 0);
            std::filesystem::copy_file(elf_helper, root / "elf-helper");
            REQUIRE(chmod((root / "elf-helper").c_str(), 0700) == 0);
            REQUIRE(phase5::file_digest((root / "elf-helper").string(), true).sha == helper_digest.sha);
            REQUIRE(phase5::file_digest((root / "producer").string(), true).sha ==
                    phase5::file_digest(binary.string(), true).sha);
            std::filesystem::copy_file(candidate / "stock.bin", root / "stock.bin");
            std::filesystem::copy_file(candidate / "build/roy_station.elf", root / "build/roy_station.elf");
            std::filesystem::copy_file(candidate / "bootstrap214.profile.json", root / "bootstrap214.profile.json");
            std::ofstream(root / "FIXTURE_ONLY") << "synthetic-no-hardware-v1\n";
            const bool continuation = fault.rfind("radio-", 0) == 0 || fault.rfind("optical-", 0) == 0;
            const bool optical_success = fault == "optical-ok" || fault == "optical-slow";
            const bool success = true;
            int receiver = -1;
            if (continuation) {
                receiver = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
                REQUIRE(receiver >= 0);
                const int capacity = 1048576;
                REQUIRE(setsockopt(receiver, SOL_SOCKET, SO_RCVBUF, &capacity, sizeof(capacity)) == 0);
                sockaddr_in address{};
                address.sin_family = AF_INET;
                address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
                REQUIRE(bind(receiver, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == 0);
                socklen_t size = sizeof(address);
                REQUIRE(getsockname(receiver, reinterpret_cast<sockaddr*>(&address), &size) == 0);
                std::ofstream(root / "udp-port") << ntohs(address.sin_port) << '\n';
            }
            const auto status = launch(root, fault);
            if ((status == 0) != success)
                std::cerr << "case=" << fault << " line=" << __LINE__ << " actual=exit " << status
                          << " expected=" << (success ? "exit 0" : "nonzero exit")
                          << " producer output:\n" << read(root / "result.log");
            REQUIRE((status == 0) == success);
            const std::map<std::string, std::string> preparation_reasons{
                {"seal-target", "Wrong target"}, {"seal-image", "Wrong diagnostic10"},
                {"seal-chip", "Wrong target"}, {"seal-slot", "Wrong target"},
                {"seal-argv", "Command differs from independent review"}, {"seal-file", "File seal mismatch"},
                {"seal-module", "Wrong protected original210"},
                {"readiness-boot", "Readiness boot/candidate mismatch"}, {"readiness-age", "Readiness expired/future"},
                {"readiness-future", "Readiness expired/future"}, {"readiness-candidate", "Readiness boot/candidate mismatch"},
                {"readiness-incomplete", "Readiness incomplete"}
            };
            if (preparation_reasons.count(fault)) {
                pico::live_fixture::verify_fixture_result(read(root / "result.log"), status, preparation_reasons.at(fault));
                // Refusal before constructing any supervisor or device adapter:
                // no restoration is asserted for an attempt that never mutated.
                REQUIRE(!std::filesystem::exists(root / "events"));
                REQUIRE(!std::filesystem::exists(root / "supervisor-pid"));
                REQUIRE(std::filesystem::is_empty(root / "attempt/recovery-journal"));
                REQUIRE(read(root / "settings.json") == "{}\n");
                REQUIRE(std::stod(read(root / "power")) == 120);
                int unused_status = 0;
                REQUIRE(waitpid(-1, &unused_status, WNOHANG) == -1 && errno == ECHILD);
                credit(fault);
                continue;
            }
            if (fault == "owner-process" && !std::filesystem::exists(root / "supervisor-pid")) {
                pico::live_fixture::verify_parent_owner_refusal(root, status);
                int unused_status = 0;
                REQUIRE(waitpid(-1, &unused_status, WNOHANG) == -1 && errno == ECHILD);
                credit(fault);
                continue;
            }
            // The supervisor is adopted by this subreaper, never signalled.
            if(!std::filesystem::exists(root/"supervisor-pid"))
                throw std::runtime_error("case="+fault+" refused before supervisor start; producer output:\n"+read(root/"result.log"));
            const auto supervisor_text=read(root/"supervisor-pid");
            const auto supervisor_pid=std::stol(supervisor_text);
            REQUIRE(supervisor_pid>0 && supervisor_text==std::to_string(supervisor_pid)+"\n");
            int child_status = 0;
            pid_t child;
            child = bounded_wait(pid_t(supervisor_pid), &child_status);
            if(child<=0)
                std::cerr<<"case="<<fault<<" supervisor waitpid="<<child<<" errno="<<errno
                         <<"; no adopted supervisor; producer output:\n"<<read(root/"result.log")<<std::flush;
            REQUIRE(child > 0);
            REQUIRE(child==supervisor_pid);
            std::cerr<<"case="<<fault<<" supervisor wait_status="<<child_status
                     <<" exit="<<(WIFEXITED(child_status)?WEXITSTATUS(child_status):-1)
                     <<" signal="<<(WIFSIGNALED(child_status)?WTERMSIG(child_status):0)<<std::endl;
            REQUIRE(WIFEXITED(child_status) && WEXITSTATUS(child_status) == 0);
            REQUIRE(waitpid(-1, &child_status, WNOHANG) == -1 && errno == ECHILD);
            const std::map<std::string, std::string> reasons{
                {"camera-growth", "Live camera recording stopped growing"},
                {"decoder-failure", "Camera decoder output ended"},
                {"decoder-PTS", "Decoded PTS has no OCC metadata"},
                {"wall-bound", "Live session 180s wall bound reached"},
                {"unresolved-owner-close", "Controlled unresolved close refused; recover same owner"},
                {"optical-led-restore", "Unmeasured LED return refused; measured recovery completed"},
                {"optical-publication-expiry", "Publication expired; invalid status emitted"},
                {"owner-process", pico::live_fixture::competing_owner_failure()},
                {"service-query-error", "Station service unresolved: unit=frame-pico-radio.service command=systemctl --user is-active status=1 ActiveState=[] SubState=not-queried"},
                {"service-active", "Station service unresolved: unit=frame-pico-radio.service command=systemctl --user is-active status=0 ActiveState=[active] SubState=not-queried"},
                {"camera-preflight", "Injected camera readiness failure"},
                {"camera-close", "Injected preflight source close failure"},
                {"controllers", "Valve controller connected during controllers-off bench"},
                {"hmd", "HMD tracking lost or ambiguous during optical bench"},
                {"observer-eof", "Observer EOF (including partial final row)"},
                {"worker", "Injected decoder health failure"},
                {"write", "Injected first MCU write failure"},
                {"after-entry", "Injected running session failure"},
                {"radio-recovery-exhaust", "Tracker mode unknown: no fresh 7e reply after 3 queries (24s maximum). No optical start or blind mode switch. Check charger/USB, power state and pairing; power-cycle and prepare a new run. Pairing is not diagnosed."},
                {"radio-native-n", "Unexpected mode/N/pending state"},
                {"radio-native-pending", "Native16 did not settle within bounded state proof; no10/19"},
                {"radio-ring-loss", "ring loss"},
                {"radio-heartbeat", "MCU service heartbeat expired; stopping and restoring"},
                {"radio-clock-raw", "Synthetic RAW probe fit failed"},
                {"radio-wrong-nonce", "Tracker mode unknown: no fresh 7e reply after 3 queries (24s maximum). No optical start or blind mode switch. Check charger/USB, power state and pairing; power-cycle and prepare a new run. Pairing is not diagnosed."},
                {"radio-source-chip", "Query link watchdog expired at 3s"},
                {"radio-source-channel", "Query link watchdog expired at 3s"},
                {"radio-partial-crc", "Query link watchdog expired at 3s"},
                {"radio-payload-checksum", "Query link watchdog expired at 3s"},
                {"optical-factory-model", "Unrecognized factory blob layout"},
                {"optical-factory-rig", "Unknown camera extrinsic units"},
                {"camera-clock-boot", "Camera acquisition boot changed"},
                {"optical-clock-horizon", "projection horizon"},
                {"listener-child", "OpenOCD child/listener/launch attribution unresolved"},
                {"optical-covariance", "Covariance candidate rejected"},
                {"optical-head-origin", "Missing same-exposure measured driver origin"},
                {"stock-anchor", "Live firmware anchor differs from stock"},
                {"stock-rtt", "RTT ABI mismatch"},
                {"optical-skew", "Stereo skew exceeds 2us"},
                {"optical-exposure", "Short exposure required"},
                {"optical-source-age", "Camera exposure stale/future"},
                {"optical-image-size", "Actual full decoded matched stereo required"},
                {"optical-sensor", "Actual full decoded matched stereo required"},
                {"optical-metadata", "Decoded exposure metadata mismatch"},
                {"optical-phase-exhausted", "Finite optical range exhausted; no confirmed target"},
                {"optical-producer-disabled", "Bootstrap sensor disabled; stop before narrowing"}
            };
            pico::live_fixture::verify_fixture_result(read(root / "result.log"), status, "");
            const auto reason=reasons.find(fault);
            if(reason!=reasons.end())REQUIRE(read(root/"result.log").find(reason->second)!=std::string::npos);
            const auto events = read(root / "events");
            REQUIRE(events.find("fixture-process-view\n") != std::string::npos);
            REQUIRE(events.find("fixture-service-view\n") != std::string::npos);
            if(fault=="owner-process" || fault=="service-active" || fault=="service-query-error") {
                REQUIRE(events.find("tcl-mutation\n")==std::string::npos);
                REQUIRE(events.find("power-applied\n")==std::string::npos);
                pico::RecoveryJournal refused(root / "attempt/recovery-journal", phase5::sha256(read(root / "invocation.json")));
                REQUIRE(refused.records().size()==1 && refused.records().front().kind=="unmutated");
                credit(fault);
                continue;
            }
            if (continuation) {
                // Bad measured RAW fitting fails during RadioOptical
                // construction, before its first discovery/radio request.
                REQUIRE((events.find("radio-startup\n") != std::string::npos)==(fault!="radio-clock-raw"));
                if (fault == "radio-recovery") {
                    REQUIRE(events.find("three-attempt-recovery-observed\n") != std::string::npos);
                }
                if (fault.rfind("radio-", 0) == 0 && !success) {
                    REQUIRE(events.find("native25-episode-observed\n") == std::string::npos);
                    REQUIRE(events.find("query-prerequisites-observed\n") == std::string::npos);
                }
                size_t poses = 0;
                size_t imu = 0;
                size_t invalid = 0;
                std::array<char, 256> bytes{};
                for (;;) {
                    const auto count = recv(receiver, bytes.data(), bytes.size(), MSG_DONTWAIT);
                    if (count < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) break;
                    REQUIRE(count > 0);
                    const std::string magic(bytes.data(), 4);
                    if (magic == "PTP2") {
                        REQUIRE(count == 88);
                        if((bytes[8] & 2) != 0)++poses;
                        else ++invalid;
                    } else {
                        REQUIRE(magic == "PTI1" && count == 32);
                        ++imu;
                    }
                }
                REQUIRE((imu > 0) == (optical_success || fault=="optical-publication-expiry"));
                REQUIRE((poses > 0) == (optical_success || fault == "optical-producer-disabled" || fault=="optical-publication-expiry"));
                if(fault=="optical-publication-expiry")REQUIRE(invalid>0);
                REQUIRE(::close(receiver) == 0);
                if (optical_success) {
                    REQUIRE(events.find("two-episode-identity-observed\n") != std::string::npos);
                    REQUIRE(events.find("phase-lock-observed\n") != std::string::npos);
                    REQUIRE(events.find("optical-publication-complete\n") != std::string::npos);
                    REQUIRE(events.find("command19-admission-observed\n") != std::string::npos);
                    REQUIRE(events.find("phase-lock-observed\n") < events.find("command19-admission-observed\n"));
                }
                if (fault == "optical-producer-disabled") {
                    REQUIRE(events.find("phase-lock-observed\n") != std::string::npos);
                    REQUIRE(events.find("command19-admission-observed\n") == std::string::npos);
                }
            }
            REQUIRE(events.find("FORBIDDEN-") == std::string::npos);
            if(fault=="unresolved-owner-close")REQUIRE(events.find("unresolved-close-refused-owner-retained\n")!=std::string::npos);
            if(fault=="parent-signal") {
                REQUIRE(std::filesystem::exists(root/"parent-signal-ready"));
                REQUIRE(events.find("entered\n") < events.find("parent-signal-ready\n"));
                REQUIRE(events.find("parent-signal-ready\n") != std::string::npos);
                REQUIRE(events.find("parent-signal-ready\n") < events.find("recording-closed\n"));
            }
            if(fault=="optical-led-restore")REQUIRE(events.find("unmeasured-led-return-refused\n")!=std::string::npos);
            REQUIRE(read(root / "settings.json") == "{}\n");
            REQUIRE(std::stod(read(root / "power")) == 120);
            const auto close = events.find("recording-closed\n");
            const auto release = events.find("debugger-released\n");
            const auto power = events.find("power-restored\n");
            REQUIRE(close != std::string::npos && close < release && release < power);
            const auto invocation = read(root / "invocation.json");
            pico::RecoveryJournal journal(root / "attempt/recovery-journal", phase5::sha256(invocation));
            const auto records = journal.records();
            REQUIRE(!records.empty());
            const bool installed = records.front().kind=="checkpoint";
            if (installed) {
                REQUIRE(records.front().kind == "checkpoint");
                REQUIRE(events.find("checkpoint-before-mutation-observed\n") != std::string::npos);
                REQUIRE(records.back().kind == "barriers");
                REQUIRE(phase5::cleanup_barriers(records.back().payload));
                REQUIRE(phase5::flag(phase5::parse(read(root/"attempt/profile-cleanup.json")),"mcu_cleanup_verified"));
                REQUIRE(events.find("barriers-observed-before-camera-release\n") != std::string::npos);
                const auto preflight = events.find("preflight-source-closed\n");
                const auto mutation = events.find("tcl-mutation\n");
                if(fault!="camera-preflight"&&fault!="camera-close")REQUIRE(preflight != std::string::npos && preflight < mutation);
                REQUIRE(mutation<close);
                REQUIRE((events.find("entered\n") != std::string::npos) == (fault != "write"));
            } else {
                REQUIRE(records.size() == 1 && records.front().kind == "unmutated");
                REQUIRE(events.find("tcl-mutation\n") == std::string::npos);
                REQUIRE(events.find("entered\n") == std::string::npos);
            }
            credit(fault);
        }
        // Device path and symlink rejection occurs before seals/profile/adapters.
        const auto rejected = base / "device-rejection";
        current_case="device-rejection";current_root=rejected;
        REQUIRE(mkdir(rejected.c_str(), 0700) == 0);
        std::filesystem::copy_file(binary, rejected / "producer");
        REQUIRE(chmod((rejected / "producer").c_str(), 0700) == 0);
        std::ofstream(rejected / "FIXTURE_ONLY") << "synthetic-no-hardware-v1\n";
        std::filesystem::create_symlink("/dev/null", rejected / "device");
        const auto rejected_status = launch(rejected, "ok");
        pico::live_fixture::verify_fixture_result(read(rejected / "result.log"), rejected_status,
            "Fixture refuses links, device paths and nonregular inputs");
        REQUIRE(!std::filesystem::exists(rejected / "events"));
        int status = 0;
        REQUIRE(waitpid(-1, &status, WNOHANG) == -1 && errno == ECHILD);
        const auto report = enforcement.json();
        std::cout << report;
        std::istringstream report_lines(report);std::string report_line;unsigned satisfied=0,total=0;
        while(std::getline(report_lines,report_line)) {
            const auto row=phase5::parse(report_line);
            if(row.object.count("status")) {
                ++total;
                if(phase5::field(row,"status").text=="SATISFIED")++satisfied;
            } else {
                REQUIRE(phase5::number(row,"satisfied")==satisfied);
                REQUIRE(phase5::number(row,"families")==total);
            }
        }
        std::cout << "SATISFIED " << satisfied << '/' << total << " (synthetic live-entry enforcement; runtime_admitted=false)\n";
        std::cout << "evidence=" << base.string() << '\n';
#else
        (void)argc;
        (void)argv;
        std::cout << "live entry fixture line=" << __LINE__ << " actual=SKIPPED expected=Linux execution\n";
#endif
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "LIVE ENTRY FAIL case=" << current_case << " root=" << current_root << ' ' << error.what() << '\n';
        // Every assertion (not just an exit-code mismatch) retains its case and
        // child evidence when build.sh output is redirected or stdout buffered.
        for(const auto* name:{"result.log","events","supervisor-pid"}){
            if(current_root.empty())break;
            std::ifstream input(current_root/name,std::ios::binary);
            if(input)std::cerr<<"artifact="<<(current_root/name)<<"\n"<<input.rdbuf()<<'\n';
        }
        std::cerr<<std::flush;
        return 1;
    }
}


