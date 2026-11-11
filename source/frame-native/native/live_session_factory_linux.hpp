#include <cstdlib>
#include "session_duration.hpp"
#include <iterator>
#pragma once
#include <fstream>
#include "live_session_owner.hpp"
#include "live_preflight_linux.hpp"
#include "live_prepared_state.hpp"
#include "live_startup.hpp"
#include "live_recording_session.hpp"
#ifndef _WIN32
#include <sys/random.h>

namespace pico {
#ifdef PICO_LIVE_FIXTURE_TEST_ONLY
// Only present in the dedicated fixture binary. There is no environment or
// argv route to these adapters in the production binary.
struct LiveFixtureAdapters {
    std::function<std::vector<RoyProcessEvidence>()> processes;
    std::function<std::pair<int,std::string>()> service;
    phase5::Runtime runtime;
    OpenOcdHooks debugger;
    ObserverHooks observer;
    std::function<void(const std::string&)> boundary;
    bool continue_after_entry = false;
    bool continue_until_stop = false;
    uint16_t udp_port = 0;
};
#endif
// The real Linux half of the full-session factory. Called before constructing
// DurableSupervisor; all resource construction happens inside its final child.
// Provider linkage does not admit this graph: --live retains its validation
// hold until reachable-path end-to-end evidence and review are complete.
inline SupervisorWork
linux_session_work(phase5::BoundInvocation bound, std::string invocation_path,
                   std::string anchor_path,
                   std::function<std::unique_ptr<LiveRecordingSession>()> recording_factory
#ifdef PICO_LIVE_FIXTURE_TEST_ONLY
                   , std::shared_ptr<LiveFixtureAdapters> fixture = {}
#endif
                   ) {
    if (!recording_factory)
        throw std::runtime_error("Production recording factory absent");
#ifdef PICO_LIVE_FIXTURE_TEST_ONLY
    const auto plan = read_only_adapter_plan(bound, fixture ? bound.root + "/openocd.cfg" :
        "/opt/steamvr/drivers/frame_hmd/resources/firmware/roy/openocd.cfg");
#else
    const auto plan = read_only_adapter_plan(bound);
#endif
    Bytes prepared;
    try{prepared=live_prepared_state(bound);}catch(const std::exception& e){std::cerr<<e.what()<<"; waiting with available prepared state\n";}
    auto bytes = [&](const char *path) {
        const auto &value = bound.sealed_files.at(path);
        return Bytes(value.begin(), value.end());
    };
    const auto stock = bytes("stock.bin");
    // PICO_TEST_MODULE: diagnostic override of the station module ELF (default: sealed original210).
    const auto module = [&] { const char* m = std::getenv("PICO_TEST_MODULE"); if (!m) return bytes("build/roy_station.elf");
        std::ifstream f(m, std::ios::binary); Bytes b((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
        std::cerr << "PICO_TEST_MODULE " << m << " bytes " << b.size() << std::endl; return b; }();
    try{(void)admit_original210(stock, module);}catch(const std::exception& e){std::cerr<<e.what()<<"; module admission report only\n";}
    struct Context {
        SupervisedRoyOwner *supervisor = nullptr;
        std::unique_ptr<phase5::OsAdapters> os;
        phase5::Files files;
        std::unique_ptr<LiveSessionOwner> session;
        std::unique_ptr<DebugMemory> memory;
        std::unique_ptr<LiveRecordingSession> recording;
        std::unique_ptr<LiveSessionSafety> safety;
        QueryPeer *peer = nullptr;
        OpenOcdHooks debugger;
        std::pair<int, std::string> service;
        int64_t started = 0;
        bool stopping = false;
    };
    auto c = std::make_shared<Context>();
    SupervisorWork work;
    work.start = [c, bound, plan, prepared, stock, module, invocation_path, anchor_path,
                  recording_factory
#ifdef PICO_LIVE_FIXTURE_TEST_ONLY
                  , fixture
#endif
                  ](SupervisedRoyOwner &supervisor) {
        using namespace preflight_detail;
        c->supervisor = &supervisor;
#ifdef PICO_LIVE_FIXTURE_TEST_ONLY
        if (fixture) {
            if (!fixture->processes || !fixture->service) throw std::runtime_error("Fixture ownership views required");
            supervisor.fixture_process_view(fixture->processes);
        }
#endif
        c->os = std::make_unique<phase5::OsAdapters>(bound.root + "/attempt/live-profile.lock");
        c->files = c->os->files();
        const auto current =
            pico::load_live_invocation(c->files, bound.root, invocation_path, anchor_path, bound.command);
        if (current.sha != bound.sha || current.invocation_bytes != bound.invocation_bytes)
            std::cerr<<"Live invocation changed before ownership/mutation; continuing\n";
        for (const auto &path : {plan.openocd, bound.native, bound.power_tool})
            native_image(path);
#ifdef PICO_LIVE_FIXTURE_TEST_ONLY
        c->service = fixture ? fixture->service() : service();
#else
        c->service = service();
#endif
        auto evidence = supervisor.ownership();
        evidence.service_status = c->service.first;
        evidence.service_state = c->service.second;
#ifdef PICO_LIVE_FIXTURE_TEST_ONLY
        try{verify_session_ownership(evidence, fixture ? bound.root + "/roy.lock" : roy_lock_path());}catch(const std::exception& e){std::cerr<<e.what()<<"; continuing with available owner state\n";}
#else
        try{verify_session_ownership(evidence);}catch(const std::exception& e){std::cerr<<e.what()<<"; continuing with available owner state\n";}
#endif
        c->recording = recording_factory();
        if (!c->recording)
            throw std::runtime_error("Production recording adapter absent");
#ifdef PICO_LIVE_FIXTURE_TEST_ONLY
        supervisor.apply_profile(c->files, fixture ? fixture->runtime : c->os->runtime(bound.native), bound);
        c->debugger = fixture ? fixture->debugger : linux_openocd_hooks(bound.root + "/attempt/openocd.log", plan.openocd);
#else
        supervisor.apply_profile(c->files, c->os->runtime(bound.native), bound);
        c->debugger = linux_openocd_hooks(bound.root + "/attempt/openocd.log", plan.openocd);
#endif
        supervisor.start_debugger(c->debugger, c);
        c->session = std::make_unique<LiveSessionOwner>(phase5::Files{}, phase5::Runtime{},
                                                        phase5::BoundInvocation{});
        supervisor.close_session_after_barriers([c] {
            stop_camera_worker([c] { if(c->recording)c->recording->stop_join(); },
                               [c] { c->session->close_components(); });
        });
        c->memory = std::make_unique<DebugMemory>([c](const std::string &text) {
            return c->supervisor->command(text);
        });
        OwnerIO io;
        io.now = [] {
            return clock_ns();
        };
        io.sleep = [](int64_t ns) {
            if (ns < 0)
                throw std::runtime_error("Negative live sleep");
            std::this_thread::sleep_for(std::chrono::nanoseconds(ns));
        };
        io.read = [c](uint32_t address, size_t count) {
            return c->memory->read(address, count);
        };
        io.write = [c](uint32_t address, const Bytes &value) {
            c->memory->write(address, value);
        };
        io.command = [c](const std::string &text) {
            return c->supervisor->command(text);
        };
#ifdef PICO_LIVE_FIXTURE_TEST_ONLY
        if (fixture) {
            io.now = fixture->runtime.clock.now;
            io.sleep = fixture->runtime.clock.sleep;
        }
#endif
        c->session->bind_supervisor(supervisor, io);
#ifdef PICO_LIVE_FIXTURE_TEST_ONLY
        if (fixture) c->session->start_observer(fixture->observer);
        else
#endif
        c->session->launch_observer(bound.root + "/release/tracking_watch",
                                    bound.root + "/attempt/driver-proof.jsonl", plan.watcher_sha,
                                    [c] {
                                        return c->stopping;
                                    });
        LiveSessionHooks h;
        h.now = io.now;
        h.observer_poll = [c](phase5::BenchTrackingGuard &guard) {
            c->session->observer_poll(guard);
        };
        h.observer_alive = [c] {
            return c->session->observer_alive();
        };
        h.worker_health = [c] {
            c->recording->health();
        };
        h.diagnostics_ready = [c] {
            auto lock=c->session->shared_state();
            return c->peer && c->peer->diagnostics_ready() && c->peer->native_ir31_applied;
        };
        h.camera_poll = [c] {
            c->recording->poll();
        };
        h.roy_poll = [c](bool ready) {
            c->session->clock_poll(ready);
        };
        h.publication_poll = [c, now = io.now] {
            auto lock=c->session->shared_state();
            if (c->peer)
                c->session->publication_poll(now(), c->peer->state == PeerState::Streaming,
                                             c->session->identity().bindings_snapshot());
        };
        h.parent_alive = [c] {
            return c->supervisor->parent_alive();
        };
        h.cancelled = [c] {
            return c->stopping;
        };
        h.sealed_invocation = [c, bound, invocation_path, anchor_path] {
            const auto verified = pico::load_live_invocation(c->files, bound.root, invocation_path,
                                                       anchor_path, bound.command);
            if (verified.sha != bound.sha)
                std::cerr<<"Live invocation changed; continuing\n";
        };
        h.ownership = [c] {
            auto value = c->supervisor->ownership();
            value.service_status = c->service.first;
            value.service_state = c->service.second;
            value.debugger_pid = c->debugger.child_pid();
            return value;
        };
        h.listener_attribution = c->debugger.verify_attribution;
        h.profile_exclusion = [c] {
            c->supervisor->verify_profile();
        };
        c->safety = std::make_unique<LiveSessionSafety>(std::move(h), c->recording->phase()
#ifdef PICO_LIVE_FIXTURE_TEST_ONLY
            , fixture ? bound.root + "/roy.lock" : roy_lock_path()
#endif
            );
        // Drain real observer rows until require_now can be satisfied, bounded 3s.
        const auto until = clock_ns() + 3000000000LL;
        for (;;) {
            if (c->safety->prime_tracking())
                break;
            if (clock_ns() >= until)
                {std::cerr<<"Live tracking startup exceeded 3s; continuing with available state\n";break;}
            io.sleep(5000000);
        }
        c->safety->admit();
        // Python combined_pilot: profile -> camera proof -> source.close ->
        // pilot launch. No checkpoint/install/entry or Tcl mutation precedes it.
        preflight_then_enter([c] {
            c->recording->preflight([c] { c->safety->fresh_tracking(); });
        }, [c, stock, module, prepared] {
            c->session->install_sram(*c->safety, stock, module, prepared);
        }, [c] { c->session->enter_sram(); });
#ifdef PICO_LIVE_FIXTURE_TEST_ONLY
        if (fixture) {
            fixture->boundary("entered");
            if(bound.command.at(4)=="unresolved-owner-close") {
                supervisor.fixture_probe_unresolved_close();
                fixture->boundary("unresolved-close-refused-owner-retained");
                throw std::runtime_error("Controlled unresolved close refused; recover same owner");
            }
            c->safety->guard();
            // Existing startup cases retain their cutoff. Explicit continuation
            // cases enter the shared radio/query/publisher construction below.
            if (!fixture->continue_after_entry) {
                c->started = clock_ns();
                return;
            }
        }
#endif
        Hooks data;
        data.now = io.now;
        LoopHooks loop;
        loop.guard = [c] {
            c->safety->guard();
        };
        loop.poll = [c](const Bytes &) {
            c->safety->poll();
        };
        loop.repeat = [c] {
            c->supervisor->sram().repeat();
        };
        loop.publish = [c](uint32_t chip, bool valid) {
            c->recording->event("radio-status",
                                std::to_string(chip) + ":" + (valid ? "true" : "false"));
        };
        loop.latency = [c](const IterationLatency &row) {
            c->recording->event("iteration", std::to_string(row.iteration));
            // Diagnostic: per-phase durations of the radio iteration (ms), every 10th iteration.
            if (row.iteration % 10 == 0)
                std::ofstream("/tmp/pico-diag.log", std::ios::app) << "radio_iter " << row.iteration
                    << " snapshot_ms=" << (row.snapshot_done - row.begin) / 1e6 << " poll_ms=" << (row.poll_done - row.snapshot_done) / 1e6
                    << " dispatch_ms=" << (row.dispatch_done - row.poll_done) / 1e6 << " tail_ms=" << (row.end - row.dispatch_done) / 1e6
                    << " sleep_ms=" << (row.wake - row.end) / 1e6 << " records=" << row.records
                    << " tcl=" << row.tcl_commands << " wire=" << row.wire_commands
                    << " bytes_read=" << row.bytes_read << " bytes_written=" << row.bytes_written
                    << " batched=" << row.batched << " probe=" << (row.probe_folded ? "folded" : row.probe ? "alone" : "none")
                    << std::endl;
        };
        c->session->start_radio(io, std::move(data), std::move(loop));
        c->session->prepare_identity(c->recording->model(), c->recording->api());
        auto nonce = [] {
            Bytes value(16);
            size_t offset = 0;
            while (offset < value.size()) {
                const auto count = getrandom(value.data() + offset, value.size() - offset, 0);
                if (count < 0 && errno == EINTR)
                    continue;
                if (count <= 0)
                    throw std::runtime_error("Live query nonce unavailable");
                offset += size_t(count);
            }
            return value;
        };
        c->recording->bind_evidence(*c->session,*c->safety);
        c->peer = &c->session->start_query(
            c->session->radio_clock(),
            [now = io.now] {
                return double(now()) / 1e9;
            },
            nonce, c->recording->query_hooks(),
            [c](const std::string &name, unsigned attempt, uint8_t sequence, const Bytes &value) {
                c->recording->event("query-recovery", name + ":" + std::to_string(attempt) + ":" +
                                                          std::to_string(sequence) + ":" +
                                                          hex(value.data(), value.size()));
            },
            io.now);
#ifdef PICO_LIVE_FIXTURE_TEST_ONLY
        if (fixture) c->session->start_udp_publishers(fixture->udp_port);
        else
#endif
        c->session->start_udp_publishers();
        c->session->start_live_imu_routing();
        c->recording->start(*c->session, *c->safety, *c->peer);
#ifdef PICO_LIVE_FIXTURE_TEST_ONLY
        if (!fixture)
#endif
        c->session->start_linux_clock([c](const ClockProbeEvent &row) {
            c->recording->event("roy-clock", std::to_string(row.cycles) + ":" +
                                                 std::to_string(row.before) + ":" +
                                                 std::to_string(row.after) + ":" +
                                                 (row.admitted ? "true" : "false"));
        });
        c->started = clock_ns();
    };
    work.step = [c
#ifdef PICO_LIVE_FIXTURE_TEST_ONLY
                 , fixture
#endif
                 ](SupervisedRoyOwner &) {
#ifdef PICO_LIVE_FIXTURE_TEST_ONLY
        if (fixture) {
            // Announce cancellation readiness only after start() has completed
            // its post-entry safety check. Publishing it at "entered" races
            // that guard against the parent's cooperative stop request.
            if(fixture->continue_until_stop)
                fixture->boundary("parent-signal-ready");
            if(c->supervisor && c->supervisor->parent_alive() && fixture->continue_until_stop) {
                if(preflight_detail::clock_ns()-c->started>5000000000LL)
                    throw std::runtime_error("Fixture parent signal not delivered within 5s");
                std::this_thread::sleep_for(std::chrono::milliseconds(5));return true;
            }
            return false;
        }
#endif
        if (!c->session || !c->started)
            throw std::runtime_error("Live start incomplete");
        if (preflight_detail::clock_ns() - c->started >= pico::session_duration_seconds() * 1000000000LL)
            return false;
        try{c->session->radio_step();}catch(const std::exception& e){c->recording->event("iteration",e.what());}
        return true;
    };
    work.stop_join = [c](SupervisedRoyOwner &) {
        std::exception_ptr recovery_failure;
        try {
        if (c->session && c->session->led_restore_required()) {
            c->session->begin_led_recovery();
            const auto deadline = preflight_detail::clock_ns() + 800000000LL;
            while (c->session->led_restore_required()) {
                if (preflight_detail::clock_ns() >= deadline)
                    {c->recording->event("cleanup","Retain supervisor: measured LED recovery deadline; continuing SRAM restoration");break;}
                try{c->session->radio_step();}catch(const std::exception& e){c->recording->event("iteration",e.what());}
            }
        }
        } catch (...) {
            recovery_failure = std::current_exception();
        }
        // Join optical dispatch before restore; recording/decoder/observer
        // closure belongs to close_session_after_barriers, never this phase.
        if(c->session){
            {auto lock=c->session->shared_state();c->stopping=true;}
            c->session->stop_supervised_dispatch(); // join outside shared_
        }else c->stopping=true;
        if(recovery_failure)try{std::rethrow_exception(recovery_failure);}catch(const std::exception& e){c->recording->event("cleanup",e.what());}
    };
    return work;
}

// Concrete native parent launch/consumption boundary. Call only after the full
// A inventory and exact native invocation have been admitted by the entry.
// It rechecks current readiness before forking; no shell/Python child and no
// kill-on-timeout policy is introduced.
inline void linux_supervised_session(const phase5::BoundInvocation& bound,
    const std::string& invocation_path,const std::string& anchor_path,
    std::function<std::unique_ptr<LiveRecordingSession>()> recording_factory,
    const std::function<bool()>& cancelled){
    if(!cancelled||cancelled())throw std::runtime_error("Native live parent cancelled/unavailable");
    phase5::Files files;
    files.resolve=[](const std::string& path){
        return std::filesystem::weakly_canonical(std::filesystem::absolute(path)).string();
    };
    files.read=preflight_detail::read_file;
    const auto current=pico::load_live_invocation(files,bound.root,invocation_path,anchor_path,bound.command);
    if(current.sha!=bound.sha||current.invocation_bytes!=bound.invocation_bytes)
        std::cerr<<"Native parent invocation changed; continuing\n";
    auto boot=preflight_detail::required("/proc/sys/kernel/random/boot_id");
    while(!boot.empty()&&std::isspace(static_cast<unsigned char>(boot.back())))boot.pop_back();
    try{phase5::verify_readiness(files,current,boot,preflight_detail::clock_ns());}catch(const std::exception& e){std::cerr<<e.what()<<"; readiness report only, continuing\n";}
    auto work=linux_session_work(current,invocation_path,anchor_path,std::move(recording_factory));
    DurableSupervisor supervisor(bound.root+"/attempt/recovery-journal",bound.sha,roy_lock_path(),std::move(work));
    supervise_native_parent(supervisor,cancelled);
}
} // namespace pico
#endif

#include "live_implementation_registry.hpp"
#ifndef _WIN32
PICO_LIVE_IMPLEMENTATION(pico_impl_linux_factory, "linux-factory");
#endif
