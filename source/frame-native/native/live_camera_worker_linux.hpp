#pragma once
#include "device_config.hpp"
#include <fstream>
#include <atomic>
#include <thread>
#include "calibration_discovery.hpp"
#include "live_session_owner.hpp"
#include "live_preflight_linux.hpp"
#include "live_pixel_pairs.hpp"
#include "live_startup.hpp"
#include "xrtap_config.hpp"
#ifndef _WIN32
namespace pico {
// Direct tap (default) or explicit legacy recording -> supervised optical worker.
// Query evidence and phase/writer ACK routing stay with the radio policy.
class LiveCameraWorker : public std::enable_shared_from_this<LiveCameraWorker> {
    LiveSessionOwner* owner_=nullptr;
    std::optional<ReplayRig> rig_;
    std::unique_ptr<OpticalCameraClock> clock_;
    std::unique_ptr<OpticalExposureSchedule> schedule_;
    std::unique_ptr<LivePixelPairs> pixels_;
    std::unique_ptr<XrtapSource> tap_;
    std::map<std::string,IdentityView> lower_views_;
    std::thread optics_;
    std::atomic<bool> optics_stop_{false};
    bool started_=false,stopped_=false;
    std::atomic<bool> failed_{false};
    std::atomic<uint64_t> processed_{0};
    uint64_t seen_=0,dropped_=0;
public:
    void start_tap(LiveSessionOwner& owner,const std::filesystem::path& calibration,
        const std::string& factory,XrtapConfig config,LedModel model,NativeOpticalApi api,
        OpticalPhaseVerification& phase,int ticks) {
        if(started_||stopped_)throw std::runtime_error("Live camera worker start consumed");
        started_=true;owner_=&owner;
        rig_=load_replay_rig(direct_calibration_dataset(calibration),factory);
        (void)rig_->bind(pico::devices::upper(0),1056,1024);
        (void)rig_->bind(pico::devices::upper(1),1056,1024);
        // Lower views for full-body trackers. A serial the rig cannot bind is logged and its frames are skipped.
        for(const auto& entry:config.lower_cameras) {
            if(!config.lower_enabled)break;
            try{lower_views_.emplace(entry.second,rig_->bind(entry.second,640,480));}
            catch(const std::exception& e){std::cerr<<"xrtap_lower bind serial="<<entry.second<<" failed: "<<e.what()<<'\n';}
        }
        std::ofstream("/tmp/pico-diag.log",std::ios::app)<<"xrtap_lower enabled="<<config.lower_enabled
            <<" "<<config.lower_cameras[0].first<<"="<<config.lower_cameras[0].second
            <<" "<<config.lower_cameras[1].first<<"="<<config.lower_cameras[1].second
            <<" mapping_verified="<<config.lower_mapping_verified<<" offset_ns="<<config.lower_offset_ns
            <<" bound="<<lower_views_.size()<<std::endl;
        tap_=std::make_unique<XrtapSource>(std::move(config));
        // Source clock is RAW. Conversion at the existing MONOTONIC consumer boundary
        // needs no recording-derived clock proof or bridge uncertainty admission.
        clock_=std::make_unique<OpticalCameraClock>(CameraClockEvidence{1,"xrtap","CLOCK_MONOTONIC_RAW","xrtap-direct",true,0,0},"xrtap");
        schedule_=std::make_unique<OpticalExposureSchedule>(*clock_,owner.radio_clock(),ticks);
        owner.direct_camera(true);
        owner.start_worker(model,api,*clock_,*schedule_,phase,
            []{return preflight_detail::clock_ns();},owner.udp_pose_sink(),shared_from_this());
        owner.optics_stop([weak=weak_from_this()]{if(auto self=weak.lock())self->stop_optics();});
        optics_=std::thread([this]{run_tap();});
    }
private:
    // Only this consumer calls the measured optical worker on the tap path.
    void run_tap() noexcept {
        while(!optics_stop_){
            try {
            for(auto& source:tap_->wait_pairs()) {
                if(optics_stop_)break;
                {   // Diagnostic: keep every 50th IR pair's first view as PGM for visual inspection.
                    static uint64_t taken=0;
                    if(++taken%50==1&&taken<=1000){
                        std::ofstream pgm("/tmp/pico-ir-"+std::to_string(taken)+".pgm",std::ios::binary);
                        pgm<<"P5\n1056 1024\n255\n";
                        pgm.write(reinterpret_cast<const char*>(source.frames[0].pixels.data()),std::streamsize(source.frames[0].pixels.size()));
                    }
                }
                IdentityPair pair;pair.timestamp_matched=true;pair.direct_capture=true;
                std::array<CameraMetadata,2> metadata;
                for(size_t i=0;i<2;++i){
                    auto& f=source.frames[i];auto& target=pair.frames[i];
                    target.serial=*f.metadata.sensor;target.timestamp_matched=true;
                    target.exposure_us=*f.metadata.exposure*1e6;
                    target.view=rig_->bind(target.serial,1056,1024);
                    target.pixels=std::move(f.pixels);
                    metadata[i]={*f.metadata.timestamp,*f.metadata.exposure};
                    target.metadata="xrtap:"+target.serial+":"+std::to_string(f.decode_index)+":"+
                        std::to_string(int64_t(metadata[i].raw*1e9));
                }
                if(!lower_views_.empty())if(auto lower=tap_->lower_for(metadata[0].raw))for(auto& f:lower->frames){
                    const auto view=lower_views_.find(*f.metadata.sensor);
                    if(view==lower_views_.end())continue;
                    IdentityExposure target;
                    target.serial=*f.metadata.sensor;target.width=640;target.height=480;target.timestamp_matched=true;
                    target.exposure_us=*f.metadata.exposure*1e6;target.source_raw=*f.metadata.timestamp;
                    target.view=view->second;target.pixels=std::move(f.pixels);
                    target.metadata="xrtap:"+target.serial+":"+std::to_string(f.decode_index)+":"+
                        std::to_string(int64_t(target.source_raw*1e9));
                    pair.lower.push_back(std::move(target));
                }
                (void)owner_->measured_exposure(std::move(pair),metadata);
                ++processed_;
            }

            } catch(const std::exception& e) {
                std::cerr<<"xrtap optics: "<<e.what()<<"; dropping pair, continuing\n";
            } catch(...) {
                failed_=true;
                std::cerr<<"xrtap optics: unknown failure; continuing\n";
            }
        }
    }
    void stop_optics(){
        optics_stop_=true;
        if(tap_)tap_->stop_consumer();
        if(optics_.joinable())optics_.join();
    }
public:
    ~LiveCameraWorker(){stop_optics();}
    void start(LiveSessionOwner& owner,LiveSessionSafety& safety,
        const std::filesystem::path& root,const std::filesystem::path& archive,
        const std::string& ffmpeg,const std::string& factory_camera,
        LedModel model,NativeOpticalApi api,OpticalPhaseVerification& phase,int phase_ticks){
        if(started_||stopped_)throw std::runtime_error("Live camera worker start consumed");
        started_=true;
        owner_=&owner;
        try{
            auto boot=[]{
                auto value=preflight_detail::required("/proc/sys/kernel/random/boot_id");
                while(!value.empty()&&std::isspace(static_cast<unsigned char>(value.back())))value.pop_back();
                return value;
            };
            auto read=[]{return CameraReadClocks{preflight_detail::clock_ns(CLOCK_MONOTONIC_RAW),
                preflight_detail::clock_ns(),preflight_detail::clock_ns(CLOCK_BOOTTIME)};};
            auto camera_hooks=camera::linux_camera_hooks(ffmpeg);
            camera_hooks.continuous=true;  // drain independently of the RF setup phase (Python reader thread)
            owner.start_camera(root,archive,std::move(camera_hooks),boot,read,
                "active calibration pending",geteuid()==0,2ULL*1024*1024*1024,
                [this,factory_camera,&safety](const std::filesystem::path& dataset){
                    // XRService writes *_device_config.json just after the dataset directory appears.
                    await_active_calibration(dataset,[]{return preflight_detail::clock_ns();},
                        [](int64_t ns){std::this_thread::sleep_for(std::chrono::nanoseconds(ns));},
                        [&safety]{safety.fresh_tracking();});
                    rig_=load_replay_rig(dataset,factory_camera);
                    (void)rig_->bind(pico::devices::upper(0),1056,1024);
                    (void)rig_->bind(pico::devices::upper(1),1056,1024);
                    return phase5::sha256(phase5::encode(rig_->factory)+phase5::encode(rig_->active)+phase5::encode(rig_->device));
                });
            // Do not feed acquisition backlog as current optical evidence.
            await_camera_ready([] { return preflight_detail::clock_ns(); },
                [&] { safety.fresh_tracking(); }, [&] {
                const bool ready=owner.camera_poll();
                (void)owner.take_camera_frames();
                return ready;
            }, [] {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            });
            clock_=std::make_unique<OpticalCameraClock>(owner.camera_proof().clock,boot());
            schedule_=std::make_unique<OpticalExposureSchedule>(*clock_,owner.radio_clock(),phase_ticks);
            owner.start_worker(model,api,*clock_,*schedule_,phase,
                []{return preflight_detail::clock_ns();},owner.udp_pose_sink(),shared_from_this());
            pixels_=std::make_unique<LivePixelPairs>(std::array<IdentityView,2>{
                rig_->bind(pico::devices::upper(0),1056,1024),rig_->bind(pico::devices::upper(1),1056,1024)},
                [this](IdentityPair pair,const std::array<CameraMetadata,2>& metadata){
                    return owner_->measured_exposure(std::move(pair),metadata);
                });
        }catch(...){
            failed_=true;
            const auto failure=std::current_exception();
            // The supervisor retains recording/decoder ownership until MCU
            // barriers, including when startup fails after SRAM entry.
            if(owner_)owner_->stop_worker();
            std::rethrow_exception(failure);
        }
    }
    void health()const{
        if(failed_)throw std::runtime_error("Live camera worker failed");
        if(owner_)owner_->camera_health();
        if(pixels_)pixels_->health();
    }
    void poll(){
        if(tap_&&!stopped_){
            health();
            {static uint64_t polls=0; if(++polls%100==1){auto c=tap_->counts(); std::ofstream("/tmp/pico-diag.log",std::ios::app)<<"tap_poll polls="<<polls<<" pairs="<<c.pairs<<" processed="<<processed_.load()<<" non_ir="<<c.non_ir<<" torn="<<c.torn<<" unpaired="<<c.unpaired<<" dropped="<<c.dropped<<" lower_pairs="<<c.lower_pairs<<" lower_non_ir="<<c.lower_non_ir<<" lower_unpaired="<<c.lower_unpaired<<" lower_matched="<<c.lower_matched<<" lower_missed="<<c.lower_missed<<std::endl;}}
            return;
        }
        if(stopped_)return;  // late poll during cleanup: nothing left to read
        if(!pixels_)throw std::runtime_error("Live camera worker unavailable");
        health();
        try{
            (void)owner_->camera_poll();
            auto frames=owner_->take_camera_frames();
            // Drop frames already near the 250 ms freshness bound before they enter pairing (Python live
            // pipeline rejects stale pairs; one stale pair must never fail the session).
            const auto now=preflight_detail::clock_ns();
            const auto offset=now-preflight_detail::clock_ns(CLOCK_MONOTONIC_RAW);
            for(auto& frame:frames){
                if(frame.metadata.timestamp){
                    const double age_ms=double(now-(int64_t(*frame.metadata.timestamp*1e9)+offset))/1e6;
                    if(++seen_%60==1)std::cerr<<"live_frame_age_ms="<<age_ms<<" dropped="<<dropped_<<'\n';
                    if(age_ms>200){++dropped_;continue;}
                }
                pixels_->frame(std::move(frame));
            }
        }catch(...){failed_=true;throw;}
    }
    OpticalExposureSchedule& schedule(){
        if(!schedule_||stopped_||failed_)throw std::runtime_error("Live schedule unavailable");
        return *schedule_;
    }
    void stop_join(){
        if(stopped_)return;
        stop_optics();
        if(owner_)stop_camera_worker([this] { owner_->stop_worker(); },
            [this] { owner_->stop_camera(); });
        tap_.reset();
        stopped_=true;
    }
};
}
#endif

#include "live_implementation_registry.hpp"
#ifndef _WIN32
PICO_LIVE_IMPLEMENTATION(pico_impl_camera_worker, "camera-worker");
#endif
