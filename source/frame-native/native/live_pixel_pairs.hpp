#pragma once
#include "device_config.hpp"
#include "live_camera.hpp"
#include "native_identity.hpp"
#include "optical_timing.hpp"
#include <cmath>
#include <iomanip>
#include <sstream>

namespace pico {
// Original decoder rasters only. This adapter does not infer a clock, head,
// darkness, phase ACK or identity; the supervised worker supplies those guards.
class LivePixelPairs {
    std::array<IdentityView,2> views_;
    std::deque<camera::GrayFrame> pending_[2];
    std::optional<double> previous_[2];
    std::optional<camera::StreamKey> streams_[2];
    bool failed_=false;
    std::function<bool(IdentityPair,const std::array<CameraMetadata,2>&)> dispatch_;
    void require(bool value,const char* message){
        if(!value)throw std::runtime_error(message);
    }
    // Frame data checks log and drop that frame; they never latch the pipeline.
    bool admit(bool value,const char* message){
        if(!value){++rejected;log_optical_drop(message);}
        return value;
    }
    void rebaseline(int side,const char* message){
        ++rejected;log_optical_drop(message);
        pending_[side].clear();previous_[side].reset();streams_[side].reset();
    }
public:
    uint64_t pairs=0,unpaired=0,publications=0,rejected=0;
    LivePixelPairs(std::array<IdentityView,2> views,
        std::function<bool(IdentityPair,const std::array<CameraMetadata,2>&)> dispatch)
        :views_(std::move(views)),dispatch_(std::move(dispatch)){
        require(bool(dispatch_),"Live pixel dispatch missing");
    }
    void health()const{
        if(failed_)throw std::runtime_error("Live pixel pipeline latched");
    }
    void frame(camera::GrayFrame frame){
        health();
        try{
            const auto& m=frame.metadata;
            if(!admit(frame.packet_timestamp_matched&&!frame.source_time_verified&&m.key.kind==38&&m.upper()&&
                m.sensor&&m.timestamp&&m.exposure,"Live original decoder provenance missing"))return;
            if(!admit(std::isfinite(*m.timestamp)&&*m.timestamp>0&&std::isfinite(*m.exposure)&&
                *m.exposure>=.000750&&*m.exposure<=.000850,"Live pixel timestamp/exposure invalid"))return;
            const int side=*m.sensor==pico::devices::upper(0)?0:*m.sensor==pico::devices::upper(1)?1:-1;
            if(!admit(side>=0,"Live pixel sensor changed"))return;
            if(!admit(frame.pixels.size()==1056u*1024u,"Live original raster dimensions invalid"))return;
            // A regressed time or a changed stream drops this frame and restarts that
            // camera's sequence, so a restarted decoder is followed instead of refused.
            if(previous_[side]&&!(*m.timestamp>*previous_[side])){
                rebaseline(side,"Live pixel source time regressed/repeated");return;
            }
            if(streams_[side]&&((m.key<*streams_[side])||(*streams_[side]<m.key))){
                rebaseline(side,"Live pixel stream changed");return;
            }
            streams_[side]=m.key;
            previous_[side]=*m.timestamp;
            auto& queue=pending_[side];
            if(queue.size()==8){queue.pop_front();++unpaired;}
            queue.push_back(std::move(frame));
            while(!pending_[0].empty()&&!pending_[1].empty()){
                const double delta=*pending_[0].front().metadata.timestamp-*pending_[1].front().metadata.timestamp;
                if(std::abs(delta)>.000002){pending_[delta<0?0:1].pop_front();++unpaired;continue;}
                IdentityPair pair;
                pair.timestamp_matched=true;
                // The measured worker supplies clock_error and stamps from its
                // one current-boot conversion, before head lookup/detection.
                std::array<CameraMetadata,2> metadata;
                for(size_t i=0;i<2;++i){
                    auto source=std::move(pending_[i].front());
                    pending_[i].pop_front();
                    const auto& original=source.metadata;
                    metadata[i]={*original.timestamp,*original.exposure};
                    auto& target=pair.frames[i];
                    target.serial=*original.sensor;
                    target.timestamp_matched=true;
                    target.exposure_us=*original.exposure*1e6;
                    target.view=views_[i];
                    target.pixels=std::move(source.pixels);
                    // Lossless canonical serialization of original packet
                    // fields; decode time never substitutes for source time.
                    std::ostringstream proof;
                    proof<<std::setprecision(17)<<original.key.kind<<':'<<unsigned(original.key.selector)
                         <<':'<<original.key.ordinal<<':'<<target.serial<<':'<<*original.timestamp
                         <<':'<<*original.exposure<<':';
                    if(original.camera_id)proof<<*original.camera_id;
                    proof<<':'<<source.decode_index;
                    target.metadata=proof.str();
                }
                ++pairs;
                if(dispatch_(std::move(pair),metadata))++publications;
            }
        }catch(...){failed_=true;throw;}
    }
};
}

#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_pixels, "pixels");
