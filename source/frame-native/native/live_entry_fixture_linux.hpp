#pragma once
#if defined(PICO_LIVE_FIXTURE_TEST_ONLY) && !defined(_WIN32)
#include "live_session_factory_linux.hpp"
#include "synthetic_tcl_server.hpp"
#include "camera_decode_fixture.hpp"
#include "live_query_evidence.hpp"
#include "fixture_tracking_stream.hpp"
#include "listener_attribution.hpp"
#include "live_guard_inventory.hpp"
#include "replay_calibration.hpp"
#include "phase5/file_digest.hpp"
#include <cstring>
#include <fstream>
#include <csignal>

namespace pico::live_fixture {
inline volatile sig_atomic_t parent_interrupted=0;
inline void fixture_parent_signal(int){parent_interrupted=1;}
inline void need(bool value, const char* message) {
    if (!value) throw std::runtime_error(message);
}
inline void put(const std::string& path, const std::string& bytes) {
    owned_files::Fd fd(::open(path.c_str(), O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC | O_NOFOLLOW, 0600));
    need(fd.value >= 0, "Fixture output exists/unavailable");
    size_t offset = 0;
    while (offset < bytes.size()) {
        const auto n = ::write(fd.value, bytes.data() + offset, bytes.size() - offset);
        if (n < 0 && errno == EINTR) continue;
        need(n > 0, "Fixture output write failed");
        offset += size_t(n);
    }
    need(fsync(fd.value) == 0, "Fixture output flush failed");
}
inline phase5::Value string_value(const std::string& bytes) {
    phase5::Value result;
    result.type = 's';
    result.text = bytes;
    return result;
}
// All supplied objects must be private regular files. In particular no device,
// FIFO, socket, symlink, hard link, absolute adapter override or shell is accepted.
inline std::string validate_root(const std::string& input) {
    const auto root = std::filesystem::canonical(input).string();
    need(input == root && root != "/" && root.rfind("/dev/", 0) != 0 &&
         root.rfind("/sys/", 0) != 0 && root.rfind("/proc/", 0) != 0,
         "Fixture requires a canonical private root");
    struct stat info{};
    need(lstat(root.c_str(), &info) == 0 && S_ISDIR(info.st_mode) &&
         info.st_uid == geteuid() && (info.st_mode & 0777) == 0700,
         "Fixture root must be owned mode 0700");
    for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
        need(lstat(entry.path().c_str(), &info) == 0 && info.st_uid == geteuid() &&
             (S_ISDIR(info.st_mode) || (S_ISREG(info.st_mode) && info.st_nlink == 1)),
             "Fixture refuses links, device paths and nonregular inputs");
    }
    need(preflight_detail::required(root + "/FIXTURE_ONLY") == "synthetic-no-hardware-v1\n",
         "Fixture marker missing");
    return root;
}
struct State {
    std::string root, fault;
    std::unique_ptr<SyntheticTclServer> server;
    std::unique_ptr<TclSocket> socket;
    bool exited = false, preflight_closed = false, recording_closed = false;
    bool entered = false, write_failed = false;
    bool mutation_attempted = false, attribution_failed = false;
    bool parent_signal_ready = false;
    int64_t native_pid = 100;
    unsigned debugger_commands = 0;
    int64_t now = preflight_detail::clock_ns();
    uint16_t udp_port = 0;
    TrackingStream tracking_stream;
    void event(const std::string& value) {
        std::ofstream out(root + "/events", std::ios::app);
        out << value << '\n';
        out.flush();
        need(bool(out), "Fixture event write failed");
    }
};
#include "live_entry_radio_optical_linux.hpp"
class Recording final : public LiveRecordingSession {
    std::shared_ptr<State> state_;
    OpticalPhaseVerification phase_{261700, {0xA1B2C3D4}, true};
    LiveSessionOwner* owner_ = nullptr;
    LiveSessionSafety* safety_ = nullptr;
    std::shared_ptr<RadioOptical> radio_optical_;
public:
    explicit Recording(std::shared_ptr<State> state) : state_(std::move(state)) {}
    const OpticalPhaseVerification& phase() const override { return phase_; }
    LedModel model() const override { return fixture_model(state_->fault=="optical-factory-model"); }
    NativeOpticalApi api() const override { return fixture_api(); }
    QueryHooks query_hooks() override {
        need(owner_ && safety_, "Fixture evidence not bound");
        return measured_query_hooks([this] { return owner_->measured_bench(phase_, state_->now); },
            [this] { safety_->fresh_tracking(); },
            [this](const std::string& name, const std::string& detail) { event(name, detail); });
    }
    void bind_evidence(LiveSessionOwner& owner, LiveSessionSafety& safety) override {
        owner_ = &owner;
        safety_ = &safety;
    }
    void start(LiveSessionOwner& owner, LiveSessionSafety& safety, QueryPeer& peer) override {
        radio_optical_ = std::make_shared<RadioOptical>(state_, owner, safety, peer, phase_);
        radio_optical_->run(radio_optical_);
    }
    void preflight(const std::function<void()>& guard) override {
        state_->event("camera-preflight");
        guard();
        if (state_->fault == "camera-preflight") throw std::runtime_error("Injected camera readiness failure");
        const auto camera_root=std::filesystem::path(state_->root)/"camera-source";
        std::filesystem::create_directory(camera_root);
        camera::CameraHooks source_hooks;
        source_hooks.now=[state=state_]{return state->now;};
        source_hooks.sleep=[state=state_](int64_t n){state->now+=n;};
        source_hooks.decoder=[]{return std::make_unique<camera::FixturePipe>();};
        source_hooks.command=[camera_root](const std::vector<std::string>& command){
            if(command.back()=="start_tracking_recording 1 1 0") {
                std::filesystem::create_directory(camera_root/"current.dataset");
                std::ofstream(camera_root/"current.dataset/frames.occ",std::ios::binary);
            }
        };
        camera::LiveCameraSource source(camera_root,camera_root/"archive",source_hooks,false,1024);
        source.start();
        try {
            source.poll();
            if(state_->fault=="camera-growth") {state_->now+=2000000001LL;source.poll();}
            source.close();
        }catch(...){source.close();throw;}
        if(state_->fault=="decoder-failure" || state_->fault=="decoder-PTS") {
            auto pipe=std::make_unique<camera::FixturePipe>();auto* raw=pipe.get();
            camera::GrayDecoder decoder({38,0,0},std::move(pipe));
            camera::AccessUnit unit;unit.key={38,0,0};unit.timestamp=1.;unit.annex_b={0,0,0,1,0x65,1};
            decoder.feed(unit);
            if(state_->fault=="decoder-failure")raw->ended=true;
            else raw->image(0,90001);
            decoder.poll(state_->now);
            throw std::runtime_error("Decoder fault unexpectedly admitted");
        }
        need(camera::synthetic_camera_decode() == 2, "Fake recording/decoder failed");
        state_->event("camera-decoded");
        const auto start=state_->now;
        CameraAcquisition acquisition(start,"fixture-boot",state_->root+"/camera","synthetic current recording");
        for(int i=0;i<=120;++i) {
            const auto t=start+int64_t(i)*33333334;
            state_->now=t;guard();
            for(int side=0;side<2;++side) {
                camera::AccessUnit row;row.key={38,0,unsigned(side)};
                row.timestamp=double(t-1000)/1e9;row.exposure=.0008;
                row.sensor=side?"CAMUPPERRIGHT":"CAMUPPERLEFT";
                acquisition.observe(row,t,{t,t,t});
            }
        }
        need(acquisition.poll(state_->now,state_->fault=="camera-clock-boot"?"wrong-boot":"fixture-boot"),"Synthetic camera acquisition incomplete");
        (void)acquisition.proof();
        if (state_->fault == "camera-close") throw std::runtime_error("Injected preflight source close failure");
        state_->preflight_closed = true;
        state_->event("preflight-source-closed");
    }
    void poll() override {}
    void health() override {
        if (state_->fault == "worker") throw std::runtime_error("Injected decoder health failure");
    }
    void stop_join() override {
        if (state_->mutation_attempted) {
            std::optional<RecoveryRecord> last;
            for (const auto& file : std::filesystem::directory_iterator(state_->root + "/attempt/recovery-journal")) {
                if (file.path().extension() != ".record") continue;
                const auto record = recovery_decode(preflight_detail::required(file.path().string()));
                if (!last || record.sequence > last->sequence) last = record;
            }
            need(last && last->kind == "barriers" && phase5::cleanup_barriers(last->payload),
                 "Camera release preceded four journaled literal-true barriers");
            state_->event("barriers-observed-before-camera-release");
        }
        state_->recording_closed = true;
        state_->event("recording-closed");
    }
    void event(const std::string& name, const std::string& detail) override {
        state_->event(name);
        if (!detail.empty()) state_->event("detail:" + detail);
    }
};
inline int entry(int argc, char** argv) {
    need(argc == 7 && std::string(argv[1]) == "--live" &&
         std::string(argv[2]) == "--test-fixture" && std::string(argv[5]) == "--duration" &&
         std::string(argv[6]) == "150", "Fixture-only binary requires --live --test-fixture ROOT CASE --duration 150");
    const auto root = validate_root(argv[3]);
    need(std::filesystem::canonical(argv[0]).string() == root + "/producer", "Fixture executable must be private");
    const std::string fault = argv[4];
    const std::set<std::string> cases{"owner-process", "service-active", "service-missing", "service-missing-inactive", "service-query-error", "service-failed", "ok", "camera-preflight", "camera-close", "controllers", "hmd", "observer-eof", "worker", "write", "after-entry",
        "radio-ok", "radio-recovery", "radio-recovery-exhaust", "radio-native-n", "radio-native-pending", "radio-ring-loss", "radio-heartbeat", "radio-clock-raw", "radio-wrong-nonce", "radio-source-chip", "radio-source-channel", "radio-partial-crc", "radio-payload-checksum", "optical-ok", "optical-slow", "optical-phase-exhausted", "optical-producer-disabled",
        "seal-target", "seal-chip", "seal-slot", "seal-image", "seal-argv", "seal-file", "seal-module", "readiness-boot", "readiness-age", "readiness-future", "readiness-candidate", "readiness-incomplete", "stock-anchor", "stock-rtt",
        "optical-factory-model", "optical-factory-rig", "camera-clock-boot", "optical-clock-horizon", "listener-child", "optical-covariance", "optical-head-origin", "optical-skew", "optical-exposure", "optical-source-age", "optical-image-size", "optical-sensor", "optical-metadata",
        "camera-growth", "decoder-failure", "decoder-PTS", "optical-led-restore", "optical-publication-expiry", "wall-bound", "parent-signal", "unresolved-owner-close"};
    need(cases.count(fault) != 0, "Unknown fixture case");
    guard_evidence::sink=[root,fault,seen=std::set<std::string>{}](unsigned family,const std::string& guard,bool accepted) mutable {
        const auto key=std::to_string(family)+":"+guard+":"+(accepted?"accept":"reject");
        if(!seen.insert(key).second)return;
        size_t count=0;const auto rows=live_guard_inventory(count);
        need(family>0&&family<=count,"Guard family outside inventory");
        phase5::Value row;row.type='{';
        row.object["family"]=phase5::parse(std::to_string(family));
        row.object["id"]=string_value(rows[family-1].id);
        row.object["case"]=string_value(fault);row.object["guard"]=string_value(guard);
        row.object["accepted"]=phase5::parse(accepted?"true":"false");
        const auto line=phase5::encode(row)+"\n";
        owned_files::Fd fd(::open((root+"/guard-evidence.jsonl").c_str(),O_WRONLY|O_APPEND|O_CREAT|O_CLOEXEC|O_NOFOLLOW,0600));
        need(fd.value>=0,"Guard evidence open failed");
        need(::write(fd.value,line.data(),line.size())==ssize_t(line.size())&&fdatasync(fd.value)==0,"Guard evidence write failed");
    };
    auto config = phase5::parse(R"({"diagnostic_pause_peers":[1,2],"slot_cycle":1,"tracker_poll_decimation_n":1,"omit_command19":true,"dense_frame_diagnostic":false,"compact_mailbox_batch":false,"fast_ring_snapshot":false,"interval_us":6000,"steady_interval_us":6000,"slot_us":900,"seed_le":"12345678","optics":{"camera_source":"recording","phase_at_exposure_ticks":261700,"timed_windows":false,"optical_method":"calibrated_stereo_v1","imu_calibration_method":"sparse_observations_v1"},"coexistence":{"controllers_off":true,"min_controllers":0}})");
    auto& optics = config.object["optics"].object;
    optics["dataset_root"] = string_value(root + "/camera");
    optics["factory_camera_file"] = string_value(root + "/factory-camera.json");
    optics["factory_led_file"] = string_value(root + "/factory-led.json");
    put(root + "/attempt/station-config.json", phase5::encode(config));
    put(root + "/attempt/registry.json", R"({"peers":[{"slot":0,"chip_id_le":"d4c3b2a1","product_sn":"PCTESTTRACKER0001"}]})");
    for (const auto& file : {"openocd.cfg", "factory-camera.json", "factory-led.json"}) put(root + "/" + file, "{}");
    put(root + "/settings.json", "{}\n");
    put(root + "/power", "120\n");
    // Every executable identity must pass the production streaming ELF digest.
    // Copy a compiled host helper, including its execute permission, rather
    // than writing magic bytes. None of these roles accesses real devices.
    for (const auto* file : {"native", "debugger", "release/tracking_watch", "ffmpeg", "power-tool"}) {
        const auto path = root + "/" + file;
        std::filesystem::copy_file(root + "/elf-helper", path);
        need(chmod(path.c_str(), 0700) == 0, "Fixture executable chmod failed");
        (void)phase5::file_digest(path, true);
    }
    (void)phase5::file_digest(root + "/producer", true);
    auto invocation = phase5::parse(R"({"launch_admitted":true,"preparation_only":false,"target":{"serial":"PCTESTTRACKER0001","chip_id_le":"d4c3b2a1","slot":0},"required_tracker_image_sha256":"040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d","files":{},"external_files":{},"external_paths":{},"command":[]})");
    invocation.object["working_directory"] = string_value(root);
    std::vector<std::string> actual;
    actual.push_back(root + "/producer");
    for (int i = 1; i < argc; ++i) actual.emplace_back(argv[i]);
    for (const auto& arg : actual) invocation.object["command"].array.push_back(string_value(arg));
    for (const auto* file : {"stock.bin", "build/roy_station.elf", "bootstrap214.profile.json", "attempt/station-config.json", "attempt/registry.json", "release/tracking_watch", "producer"}) {
        const auto bytes = preflight_detail::required(root + "/" + file);
        invocation.object["files"].object[file] = phase5::parse("{\"bytes\":" + std::to_string(bytes.size()) + ",\"sha256\":\"" + phase5::sha256(bytes) + "\"}");
    }
    for (const auto* file : {"native", "debugger", "ffmpeg", "power-tool", "openocd.cfg", "factory-camera.json", "factory-led.json"}) {
        const auto path = root + "/" + file;
        invocation.object["external_files"].object[path] = string_value(phase5::sha256(preflight_detail::required(path)));
    }
    // Power state is mutable; its tool has a separately sealed ELF identity.
    for (const auto& pair : std::map<std::string, std::string>{{"native", "native"}, {"power_tool", "power-tool"}, {"ffmpeg", "ffmpeg"}, {"settings", "settings.json"}, {"openocd", "debugger"}, {"producer", "producer"}})
        invocation.object["external_paths"].object[pair.first] = string_value(root + "/" + pair.second);
    invocation.object["native_xrservice_sha256"] = invocation.object["external_files"].object[root + "/native"];
    if (fault == "seal-target") invocation.object["target"].object["serial"] = string_value("WRONG-TARGET");
    if (fault == "seal-chip") invocation.object["target"].object["chip_id_le"] = string_value("00000000");
    if (fault == "seal-slot") invocation.object["target"].object["slot"] = phase5::parse("1");
    if (fault == "seal-image") invocation.object["required_tracker_image_sha256"] = string_value(std::string(64, '0'));
    if (fault == "seal-argv") invocation.object["command"].array.back() = string_value("149");
    // A self-consistent caller seal must still fail the independent module anchor.
    if (fault == "seal-module") {
        std::ofstream(root + "/build/roy_station.elf", std::ios::binary | std::ios::trunc) << "wrong-module";
        invocation.object["files"].object["build/roy_station.elf"] =
            phase5::parse("{\"bytes\":12,\"sha256\":\"" + phase5::sha256("wrong-module") + "\"}");
    }
    const auto bytes = phase5::encode(invocation);
    put(root + "/invocation.json", bytes);
    put(root + "/invocation.sha256", phase5::sha256(bytes) + "\n");
    phase5::OsAdapters os(root + "/prepare.lock");
    auto files = os.files();
    if (fault == "seal-file") std::ofstream(root + "/attempt/registry.json", std::ios::app) << ' ';
    const auto bound = load_live_invocation(files, root, root + "/invocation.json", root + "/invocation.sha256", actual);
    auto boot = preflight_detail::required("/proc/sys/kernel/random/boot_id");
    while (!boot.empty() && std::isspace(static_cast<unsigned char>(boot.back()))) boot.pop_back();
    // Synthetic readiness is confined to this private attempt and binary.
    phase5::create_readiness(files, bound, {"SYNTHETIC FIXTURE ONLY; no physical readiness asserted", true, true, true, true}, boot, preflight_detail::clock_ns());
    if (fault == "readiness-candidate" || fault == "readiness-incomplete") {
        const auto path = root + "/attempt/human-readiness.json";
        auto readiness = phase5::parse(preflight_detail::required(path));
        if (fault == "readiness-candidate") readiness.object["invocation_sha256"] = string_value(std::string(64, '0'));
        else readiness.object["controllers_off"] = phase5::parse("false");
        std::ofstream(path, std::ios::trunc) << phase5::encode(readiness);
    }
    try{phase5::verify_readiness(files, bound, fault == "readiness-boot" ? "wrong-boot" : boot,
        preflight_detail::clock_ns() + (fault == "readiness-age" ? 301000000000LL : fault == "readiness-future" ? -1000000000LL : 0));}catch(const std::exception& e){std::cerr<<e.what()<<"; fixture readiness report only\n";}
    auto state = std::make_shared<State>();
    state->root = root;
    state->fault = fault;
    auto adapters = std::make_shared<LiveFixtureAdapters>();
    adapters->continue_until_stop=fault=="parent-signal";
    adapters->processes = [state] {
        state->event("fixture-process-view");
        if(state->fault=="owner-process") return std::vector<RoyProcessEvidence>{{424242,1,"/fixture/openocd","openocd --fixture-owner"}};
        return std::vector<RoyProcessEvidence>{};
    };
    adapters->service = [state] {
        state->event("fixture-service-view");
        if(state->fault=="service-missing-inactive") return std::make_pair(4,std::string("inactive"));
        if(state->fault=="service-query-error") return std::make_pair(1,std::string());
        if(state->fault=="service-active") return std::make_pair(0,std::string("active"));
        if(state->fault=="service-missing") return std::make_pair(4,std::string("unknown"));
        if(state->fault=="service-failed") return std::make_pair(3,std::string("failed"));
        return std::make_pair(3,std::string("inactive"));
    };
    adapters->runtime.boot_id = [boot] { return boot; };
    adapters->runtime.clock.now = [state] { return state->now; };
    adapters->runtime.clock.sleep = [](int64_t) {};
    adapters->continue_after_entry = fault.rfind("radio-", 0) == 0 || fault.rfind("optical-", 0) == 0;
    if (adapters->continue_after_entry) {
        const auto port = std::stoul(preflight_detail::required(root + "/udp-port"));
        need(port > 0 && port <= 65535, "Private fixture UDP port missing");
        state->udp_port = static_cast<uint16_t>(port);
        adapters->udp_port = state->udp_port;
    }
    adapters->runtime.managed = [state] { return std::vector<phase5::ManagedProcess>{{state->native_pid, 20}}; };
    adapters->runtime.terminate_native = [state](int64_t pid) {
        need(pid == state->native_pid, "Fake native identity mismatch");
        ++state->native_pid;
        state->event("native-restarted");
    };
    adapters->runtime.power = [state, root](const std::string& tool, std::optional<double> value, int64_t timeout) {
        need(tool == root + "/power-tool" && timeout == 8000000000LL, "Fake power tool identity mismatch");
        const auto before = std::stod(preflight_detail::required(root + "/power"));
        if (value) {
            std::ofstream out(root + "/power");
            out << *value << '\n';
            out.close();
            need(bool(out), "Fake power write failed");
            state->event(*value == 300 ? "power-applied" : "power-restored");
        }
        return "{\"read_error\":0,\"set_error\":0,\"before\":" + std::to_string(before) +
            (value ? ",\"set\":" + std::to_string(*value) : "") + "}";
    };
    auto& debugger = adapters->debugger;
    debugger.now = [] { return preflight_detail::clock_ns(); };
    debugger.sleep = [](int64_t ns) { std::this_thread::sleep_for(std::chrono::nanoseconds(ns)); };
    debugger.listener_present = [] { return false; };
    debugger.spawn = [state, bound](const std::vector<std::string>&) {
        state->server = std::make_unique<SyntheticTclServer>();
        const auto& stock = bound.sealed_files.at("stock.bin");
        state->server->backend.stock(Bytes(stock.begin(), stock.end()));
        if (state->fault == "stock-anchor") state->server->backend.memory[0] ^= 1;
        if (state->fault == "stock-rtt") state->server->backend.memory[roy_rtt] ^= 1;
        state->server->start();
        state->event("fake-tcl-started");
    };
    debugger.poll = [state]() -> std::optional<int> { return state->exited ? std::optional<int>(0) : std::nullopt; };
    debugger.connect = [state] {
        state->socket = std::make_unique<TclSocket>(state->server->port);
        return true;
    };
    debugger.command = [state](const std::string& command) {
        // Deliberately slow only the synthetic transport. The session clock
        // and every production freshness/deadline predicate remain unchanged.
        if (state->fault == "optical-slow" && ++state->debugger_commands % 128 == 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        if (command == "shutdown") {
            need(state->recording_closed, "Debugger release preceded recording closure");
            state->exited = true;
            state->event("debugger-released");
            return std::string("ok");
        }
        if (command.rfind("read_memory ", 0) != 0) {
            need(state->preflight_closed, "MCU mutation preceded camera preflight/source close");
            if (!state->mutation_attempted) {
                bool checkpoint = false;
                for (const auto& file : std::filesystem::directory_iterator(state->root + "/attempt/recovery-journal")) {
                    if (file.path().extension() == ".record")
                        checkpoint = checkpoint || recovery_decode(preflight_detail::required(file.path().string())).kind == "checkpoint";
                }
                need(checkpoint, "MCU mutation preceded durable checkpoint");
                state->event("checkpoint-before-mutation-observed");
            }
            state->mutation_attempted = true;
            state->event("tcl-mutation");
            if (state->fault == "write" && !state->write_failed) {
                state->write_failed = true;
                throw std::runtime_error("Injected first MCU write failure");
            }
        }
        return state->socket->command(command);
    };
    debugger.disconnect = [state] {
        state->socket.reset();
        state->server->finish();
    };
    debugger.terminate = [state] { state->event("FORBIDDEN-terminate"); throw std::runtime_error("Never terminate fixture SRAM owner"); };
    debugger.kill = [state] { state->event("FORBIDDEN-kill"); throw std::runtime_error("Never kill fixture SRAM owner"); };
    debugger.verify_attribution = [state] {
        ListenerIdentity launch;launch.child=11;launch.child_start=3;
        launch.argv={"synthetic-openocd","-f","interface","-f","target"};
        launch.executable_seal="synthetic executable";launch.dependency_seals={"interface","target"};
        auto observed=launch;observed.owner=11;observed.observed_start=3;
        observed.socket_inode=4;observed.address="127.0.0.1";observed.port=6666;observed.matching_listeners=1;
        if(state->entered && state->fault=="listener-child" && !state->attribution_failed){state->attribution_failed=true;observed.owner=12;}
        verify_listener_identity(observed,launch);
    };
    debugger.child_pid = [] { return int64_t(0); };
    adapters->observer = {
        [state](size_t) -> ObserverRead {
            if (state->fault == "observer-eof") return {"", true};
            const auto stamp = state->tracking_stream.read(state->now);
            if (!stamp) return {};
            const auto controller = state->fault == "controllers" ? ",{\"index\":1,\"kind\":2,\"connected\":true,\"valid\":false,\"result\":0,\"matrix\":[1,0,0,0,0,1,0,0,0,0,1,0]}" : "";
            auto row=phase5::parse("{\"t_ns\":" + std::to_string(*stamp) + ",\"devices\":[{\"index\":0,\"kind\":1,\"connected\":true,\"valid\":" +
                (state->fault == "hmd" ? "false" : "true") + ",\"result\":200,\"matrix\":[1,0,0,0,0,1,0,0,0,0,1,0]}" + controller + "]}\n");
            if(state->fault!="optical-head-origin")
                row.object["raw_to_standing"]=phase5::parse("[1,0,0,0,0,1,0,0,0,0,1,0]");
            return {phase5::encode(row)+"\n",false};
        }, [] { return true; }, [] { return true; }, [] { return false; }, [state] { return state->now; }
    };
    adapters->boundary = [state](const std::string& name) {
        state->entered = true;
        state->event(name);
        if (state->fault == "after-entry") throw std::runtime_error("Injected running session failure");
        if(name=="entered" && state->fault=="wall-bound")state->now+=180000000000LL;
        if(name=="parent-signal-ready" && state->fault=="parent-signal" && !state->parent_signal_ready) {
            put(state->root+"/parent-signal-ready","ready\n");
            state->parent_signal_ready=true;
        }
    };
    auto work = linux_session_work(bound, root + "/invocation.json", root + "/invocation.sha256",
        [state] { return std::make_unique<Recording>(state); }, adapters);
    const auto start=work.start;
    work.start=[root,start](SupervisedRoyOwner& owner){
        // Written by the actual detached child, never inferred from a passing
        // producer exit or a case name. Allows the subreaper to wait for exactly
        // this supervisor rather than accidentally accepting another child.
        put(root+"/supervisor-pid",std::to_string(getpid())+"\n");
        start(owner);
    };
    DurableSupervisor supervisor(root + "/attempt/recovery-journal", bound.sha, root + "/roy.lock", std::move(work));
    parent_interrupted=0;
    const auto previous=std::signal(SIGTERM,fixture_parent_signal);
    supervise_native_parent(supervisor, [root,fault] {
        if(fault=="parent-signal" && !parent_interrupted && std::filesystem::exists(root+"/parent-signal-ready"))
            std::raise(SIGTERM);
        return parent_interrupted!=0;
    });
    std::signal(SIGTERM,previous);
    return 0;
}
} // namespace pico::live_fixture
#endif


