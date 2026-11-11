#pragma once
#include "device_config.hpp"
#include "calibration_discovery.hpp"
#include "live_recording_session.hpp"
#include "live_camera_worker_linux.hpp"
#include "live_query_evidence.hpp"
#include "live_event_sink_linux.hpp"
#ifndef _WIN32
namespace pico {
// Concrete camera/decoder/rig/library lifetime binding. Evidence policy is a
// mandatory measured adapter; this class supplies no permissive QueryHooks.
// The production entry must still install phase routing and evidence policy.
class LinuxLiveRecordingSession final : public LiveRecordingSession {
    phase5::BoundInvocation bound_;
    NativeOpticalLibraries libraries_;
    LedModel model_;
    int ticks_;
    OpticalPhaseVerification phase_;
    QueryHooks hooks_;
    LiveSessionOwner* owner_=nullptr;
    LiveSessionSafety* safety_=nullptr;
    std::function<void(const std::string&,const std::string&)> event_;
    std::shared_ptr<LiveCameraWorker> camera_ = std::make_shared<LiveCameraWorker>();
    std::shared_ptr<camera::LiveCameraSource> preflight_source_;
    bool preflight_complete_ = false;
    std::filesystem::path root_, archive_;
    std::string ffmpeg_, factory_camera_;
    bool windows_=true;
    bool tap_selected_=true;
    XrtapConfig tap_config_;
    bool mode_recording_=false;  // XRService recording started only to put the cameras in tracking mode
    std::filesystem::path calibration_;
    std::string leds_;  // PICO factory calibration (calibration_data by serial): every enrolled tracker's LED model
    bool attempted_ = false, stopped_ = false, failed_ = false;

    static std::string sealed(const phase5::BoundInvocation& bound,const std::string& path) {
        if (path.empty() || path.find('\0')!=std::string::npos)
            throw std::runtime_error("Recording adapter path required");
        // A missing/mismatched seal or a relative path is logged; the adapter is still used.
        if (path.front()!='/')
            std::cerr<<"recording_adapter_unsealed path="<<path<<" reason=\"not absolute\"\n";
        try {
            const auto invocation = phase5::parse(bound.invocation_bytes);
            preflight_detail::seal(invocation,path);
        } catch (const std::exception& e) {
            std::cerr<<"recording_adapter_unsealed path="<<path<<" reason=\""<<e.what()<<"\"\n";
        }
        return path;
    }
    static int phase_ticks(const phase5::BoundInvocation& bound) {
        const auto config=phase5::parse(bound.sealed_files.at("attempt/station-config.json"));
        const auto value=phase5::number(phase5::field(config,"optics"),"phase_at_exposure_ticks");
        if (value<0 || value>=533333)
            throw std::runtime_error("Recording phase outside native period");
        return int(value);
    }
  public:
    LinuxLiveRecordingSession(phase5::BoundInvocation bound,const std::string& optics,
        const std::string& detector,const std::string& ffmpeg,
        std::function<void(const std::string&,const std::string&)> events)
        : bound_(std::move(bound)),
          libraries_(sealed(bound_,optics),sealed(bound_,detector)),
          model_{}, ticks_(phase_ticks(bound_)),
          phase_(ticks_,{pico::devices::chip()},true), event_(std::move(events)) {
        if (!event_)
            throw std::runtime_error("Measured recording evidence adapters required");
        const auto config = phase5::parse(bound_.sealed_files.at("attempt/station-config.json"));
        const auto& optical = phase5::field(config,"optics");
        root_ = preflight_detail::string_field(optical,"dataset_root");
        archive_ = std::filesystem::path(bound_.root) / "attempt" / "camera-archive";
        factory_camera_ = sealed(bound_,preflight_detail::string_field(optical,"factory_camera_file"));
        const auto leds = sealed(bound_,preflight_detail::string_field(optical,"factory_led_file"));
        model_ = load_tracker_leds(leds,pico::devices::serial());
        leds_ = leds;
        tap_selected_=use_xrtap(optical);
        tap_config_=xrtap_config(optical);
        calibration_=root_;
        auto snapshot=optical.object.find("calibration_dataset");
        if(snapshot!=optical.object.end())calibration_=snapshot->second.text;
        if(tap_selected_)calibration_=xrtap_calibration(optical);
        if(!tap_selected_)ffmpeg_ = sealed(bound_,ffmpeg);
        if(!tap_selected_)preflight_detail::native_image(ffmpeg_);
    }
    const OpticalPhaseVerification& phase() const override { return phase_; }
    LedModel model() const override { return model_; }
    NativeOpticalApi api() const override { return libraries_.api(); }
    void preflight(const std::function<void()>& guard) override {
        if (preflight_source_ || preflight_complete_ || attempted_ || stopped_ || !guard)
            throw std::runtime_error("Recording preflight consumed/invalid");
        guard();
        if(tap_selected_){
            // XRService applies the tracking profile's short IR exposure only while a tracking recording runs
            // (measured 2026-10-29: short frames mean ~31 without, ~5 with). Start one for the camera mode only;
            // frames still come straight from the tap, the recording is never read or decoded.
            camera::linux_camera_hooks("/usr/bin/ffmpeg").command(camera::recording_argv("/opt/steamvr/bin/linuxarm64/vrcmd",geteuid()==0,true));
            mode_recording_=true;
            // The tap mapping can appear only after XRService restarts: log and keep retrying.
            std::unique_ptr<XrtapSource> source;
            for(uint64_t attempt=1;!source;++attempt){
                guard();
                try{source=std::make_unique<XrtapSource>(tap_config_);}
                catch(const std::runtime_error& e){
                    if(attempt%50==1)event_("camera-preflight-wait","xrtap:attempt="+std::to_string(attempt)+":"+e.what());
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
            xrtap_preflight(*source,guard,[this](const std::string& detail){event_("camera-preflight-wait",detail);});
            event_("camera-preflight","xrtap:matched_ir_pairs="+std::to_string(source->counts().pairs)+":rf_started=false");
            preflight_complete_=true;return;
        }
        std::filesystem::create_directory(root_);
        // Live production drains the recording on its own follower thread, independent of the RF setup phase
        // (mode queries, native10, 7e recovery), exactly like the Python live_source reader thread. Polling only
        // from the ~5 Hz radio loop let the compressed backlog exceed 32 units on hardware.
        auto camera_hooks = camera::linux_camera_hooks(ffmpeg_);
        camera_hooks.continuous = true;
        preflight_source_ = std::make_shared<camera::LiveCameraSource>(root_,
            std::filesystem::path(bound_.root)/"attempt"/"preflight-camera-recordings",
            std::move(camera_hooks), geteuid()==0);
        // Retained by this recording, whose supervisor finalizer was registered
        // before this call. Failed start/close must remain recoverable.
        const auto dataset = preflight_source_->start();
        // XRService writes the new dataset's *_device_config.json shortly after the recording directory appears.
        // Same bounded wait as the preflight and the Python calibration snapshot: 10 s, 100 ms polls, 200 ms settle.
        await_active_calibration(dataset, [] { return preflight_detail::clock_ns(); },
            [](int64_t ns) { std::this_thread::sleep_for(std::chrono::nanoseconds(ns)); }, guard);
        auto boot=[] {
            auto value=preflight_detail::required("/proc/sys/kernel/random/boot_id");
            while(!value.empty()&&std::isspace(static_cast<unsigned char>(value.back())))value.pop_back();
            return value;
        };
        CameraAcquisition proof(preflight_detail::clock_ns(),boot(),dataset.string(),
                                "pre-mutation exposure/cadence and RAW clock proof");
        await_camera_ready([] { return preflight_detail::clock_ns(); }, guard, [&] {
            preflight_source_->poll();
            for(const auto& row:preflight_source_->take_metadata()) {
                proof.observe(row,preflight_detail::clock_ns(),
                    {preflight_detail::clock_ns(CLOCK_MONOTONIC_RAW),
                     preflight_detail::clock_ns(),preflight_detail::clock_ns(CLOCK_BOOTTIME)});
            }
            (void)preflight_source_->take_frames();
            return proof.poll(preflight_detail::clock_ns(),boot());
        }, [] { std::this_thread::sleep_for(std::chrono::milliseconds(5)); });
        const auto& accepted=proof.proof();
        event_("camera-preflight",std::to_string(accepted.exposures[0])+":"+
               std::to_string(accepted.exposures[1])+":rf_started=false");
        preflight_source_->close();
        preflight_complete_ = true;
    }
    QueryHooks query_hooks() override {
        if(!owner_ || !safety_)throw std::runtime_error("Recording evidence not bound");
        return hooks_;
    }
    void bind_evidence(LiveSessionOwner& owner,LiveSessionSafety& safety) override {
        if(owner_ || attempted_ || stopped_)
            throw std::runtime_error("Recording evidence binding consumed");
        owner_=&owner;
        safety_=&safety;
        hooks_=measured_query_hooks([this]{
            return owner_->measured_bench(phase_,preflight_detail::clock_ns());
        },[this]{safety_->fresh_tracking();},event_);
    }
    void start(LiveSessionOwner& owner,LiveSessionSafety& safety,QueryPeer&) override {
        if (!preflight_complete_ || attempted_ || stopped_)
            throw std::runtime_error("Recording start consumed");
        attempted_ = true;
        try {
            if(tap_selected_)camera_->start_tap(owner,calibration_,factory_camera_,tap_config_,model_,libraries_.api(),phase_,ticks_);
            else camera_->start(owner,safety,root_,archive_,ffmpeg_,factory_camera_,
                           model_,libraries_.api(),phase_,ticks_);
            const auto config=phase5::parse(bound_.sealed_files.at("attempt/station-config.json"));
            const auto& optics=phase5::field(config,"optics");
            const auto windows=optics.object.find("timed_windows");
            if(windows==optics.object.end()||windows->second.type!='b')
                event_("config-default","timed_windows missing or not Boolean; using true");
            else windows_=windows->second.text=="true";
            owner.start_phase_routing(camera_->schedule(),phase_,ticks_,windows_);
            // 2026-10-31 multi-tracker optics: the LED model of every secondary by its serial (PICO_PEERS); a chip
            // without serial or factory record stays IMU-only (logged), never a run abort.
            const auto& table=PeerTable::configured();
            if(table.multi) try {
                std::vector<MultiChipModel> models;
                const auto root=std::filesystem::exists(leds_)?calibration_json(leds_):phase5::Json(std::string(R"({"calibration_data":[]})")).parse();
                for(size_t i=1;i<table.peers.size();++i) {
                    const auto& p=table.peers[i];
                    if(!p.chip) continue;
                    try {
                        if(p.serial.empty()) throw std::runtime_error("no serial in PICO_PEERS");
                        models.push_back({p.chip,p.serial,tracker_leds(root,p.serial)});
                    } catch(const std::exception& e) {
                        event_("multi-optics-model",p.serial+":"+e.what()+":imu_only=true");
                    }
                }
                owner.start_multi_optics(std::move(models),model_,libraries_.api(),&camera_->schedule(),
                                         []{return preflight_detail::clock_ns();});
            } catch(const std::exception& e) {  // owner rule: logged, the first target keeps running
                event_("multi-optics-start",std::string(e.what())+":secondaries_imu_only=true");
            }
        } catch (...) {
            failed_ = true;
            throw;
        }
    }
    void health() override {
        if (failed_)
            throw std::runtime_error("Recording session latched");
        camera_->health();
    }
    void poll() override {
        health();
        try { camera_->poll(); }
        catch (...) {
            failed_ = true;
            throw;
        }
    }
    void stop_join() override {
        if (stopped_) return;
        XrtapSource::disable_tap();  // the XRService tap goes dormant again after the tracker session
        if(mode_recording_){
            try{camera::linux_camera_hooks("/usr/bin/ffmpeg").command(camera::recording_argv("/opt/steamvr/bin/linuxarm64/vrcmd",geteuid()==0,false));}
            catch(const std::exception& e){event_("camera-mode-recording-stop-failed",e.what());}  // logged, not fatal
            mode_recording_=false;
        }
        stop_camera_worker([this] { if(preflight_source_)preflight_source_->close(); },
                           [this] { camera_->stop_join(); });
        stopped_ = true;
    }
    void event(const std::string& name,const std::string& detail) override {
        event_(name,detail);
    }
};
}
#endif

#include "live_implementation_registry.hpp"
#ifndef _WIN32
PICO_LIVE_IMPLEMENTATION(pico_impl_linux_recording, "linux-recording");
#endif


