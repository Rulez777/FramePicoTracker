#pragma once
#include "native_identity.hpp"
#include "phase5/json.hpp"
#include "live_camera.hpp"
#include "recording_head_history.hpp"
namespace pico {
// Read-only calibration loaders shared by replay and live preflight. Loading a
// rig never authorizes a live clock or pose. Active extrinsics are flattened;
// never chain factory extrinsics.
struct ReplayRig {
    phase5::Value factory, active, device;
    IdentityView bind(const std::string& serial,int width,int height) const;
};
phase5::Value calibration_json(const std::filesystem::path&);
ReplayRig load_replay_rig(const std::filesystem::path& dataset,const std::filesystem::path& factory);
LedModel load_replay_leds(const phase5::Value&,const std::string& serial);
LedModel default_tracker_leds();
LedModel tracker_leds(const phase5::Value&,const std::string& serial);
LedModel load_tracker_leds(const std::filesystem::path&,const std::string& serial);
struct ReplayOpticalCounts {
    uint64_t pairs=0,candidates=0,stereo_verified=0,world_poses=0,unpaired=0;
    std::map<std::string,uint64_t> rejected;
};
// Recorded measured Standing world; nothing enters identity or UDP publication.
class ReplayOptical {
    ReplayRig rig_; LedModel model_; NativeOpticalApi api_;
    ExposureHeadLookup head_lookup_;
    std::deque<camera::GrayFrame> pending_[2];
public:
    ReplayOptical(ReplayRig rig,LedModel model,NativeOpticalApi api,ExposureHeadLookup heads):rig_(std::move(rig)),model_(model),api_(api),head_lookup_(std::move(heads)){if(!head_lookup_)throw std::runtime_error("Measured exposure HMD history required");}
    ReplayOpticalCounts counts;
    std::optional<Rigid> last_pose; // WORLD<-tracker; never a live publication
    std::array<int64_t,2> source_times{}; // original MONOTONIC conversion
    std::function<void(const Rigid&,const std::array<camera::GrayFrame,2>&,uint64_t)> pose_sink;
    void frame(const camera::GrayFrame&);
    void finish();
};
}
