#pragma once
#include "live_camera.hpp"
namespace pico::camera {
struct RecordingReplayHooks {
    std::function<void(const std::filesystem::path&)> begin_file;
    std::function<void()> end_file;
    // Called throughout decode/backpressure/drain, not just between files.
    std::function<void()> tick;
    std::function<void(const GrayFrame&)> frame;
};
// Finalized, immutable OCC only. One shared absolute steady-clock deadline.
// Does not calibrate timestamps or synthesize head/rig/identity provenance.
void replay_recording(const std::filesystem::path&,const std::string& ffmpeg,
                      int64_t deadline,const RecordingReplayHooks& = {});
}
