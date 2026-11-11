#pragma once
#include "head_history.hpp"
#include "live_camera.hpp"
#include "optical_timing.hpp"
#include "recording_camera_clock.hpp"
#include <filesystem>
#include <map>

namespace pico {
struct ExposureHeads {
    std::array<int64_t,2> stamps;
    std::array<Rigid,2> world_from_head;
};
using ExposureHeadLookup=std::function<ExposureHeads(const std::array<camera::GrayFrame,2>&)>;

// Recording-local interchange adapter. These files must contain ORIGINAL run
// evidence; this class never infers a clock offset or samples today's headset.
// The candidate does not define an OCSV reader. Do not guess its schema here.
class RecordingHeadHistory {
    HeadHistory heads_;
    struct Clock { int64_t stamp,now; };
    std::map<int64_t,Clock> clocks_;
    std::optional<CameraClockEvidence> evidence_;
    struct MappedPair { std::array<int64_t,2> raw,stamp,decoded; };
    std::multimap<int64_t,MappedPair> exposure_map_;
    bool mapped_=false;
    size_t skipped_mapping_rows_=0;
    template<class F> static void lines(const std::filesystem::path& path,F consume) {
        if(std::filesystem::is_symlink(path)||!std::filesystem::is_regular_file(path))
            throw std::runtime_error("Missing original recording HMD/clock evidence: "+path.string());
        std::ifstream input(path);std::string line;size_t rows=0;
        if(!input)throw std::runtime_error("Cannot read recording HMD/clock evidence");
        while(std::getline(input,line)){
            if(line.empty())continue;
            if(line.size()>65536||++rows>2000000)throw std::runtime_error("Recording HMD/clock evidence exceeds bounds");
            consume(phase5::Json(line).parse());
        }
        if(!input.eof()||!rows)throw std::runtime_error("Empty/unreadable recording HMD/clock evidence");
    }
public:
    explicit RecordingHeadHistory(const std::filesystem::path& dataset,
                                  const std::filesystem::path& driver_proof={},
                                  const std::filesystem::path& camera_clock={},
                                  const std::filesystem::path& exposure_map={}) {
        if(driver_proof.empty()!=camera_clock.empty())
            throw std::runtime_error("--driver-proof and --camera-clock must be supplied together");
        if(!camera_clock.empty())evidence_=read_recording_camera_clock(camera_clock);
        // head_pose uses matrix AS IS, already Standing WORLD<-HEAD.
        // raw_to_standing belongs to driver_pose, not head_pose.
        lines(driver_proof.empty()?dataset/"tracking_watch.jsonl":driver_proof,
              [&](const phase5::Value& s){heads_.observe(s);});
        if(!exposure_map.empty()) {
            mapped_=true;
            lines(exposure_map,[&](const phase5::Value& v){
                auto timing=v.object.find("frame_timing");
                if(timing==v.object.end()||timing->second.type!='b'||timing->second.text!="true")return;
                auto array=[&](const char* key)->const std::vector<phase5::Value>& {
                    const auto& a=phase5::field(v,key);
                    if(a.type!='['||a.array.size()!=2)throw std::runtime_error("Exposure map requires stereo arrays");
    return a.array;
                };
                // No candidate may have been evaluated on this timing row.
                // Missing evidence is skippable; present malformed evidence is not.
                for(const auto* key:{"selected_source_raw","exposure_stamps_ns","decoded_monotonic_ns"}) {
                    if(v.object.find(key)==v.object.end())continue;
                    const auto& values=array(key);
                    for(const auto& value:values) {
                        if(std::string(key)=="selected_source_raw") {
                            if(value.type!='i')throw std::runtime_error("Invalid exposure map RAW time");
                            const double t=std::stod(value.text);
                            if(!std::isfinite(t)||t<=0||t>=double(INT64_MAX/8)/1e9)
                                throw std::runtime_error("Invalid exposure map RAW time");
                        } else {
                            const auto t=phase5::integer(value);
                            if(t<=0||t>=INT64_MAX/8)throw std::runtime_error("Invalid exposure map time");
                        }
                    }
                }
                if(v.object.find("selected_source_raw")==v.object.end()||
                   v.object.find("exposure_stamps_ns")==v.object.end()) {
                    ++skipped_mapping_rows_;
                    return;
                }
                const auto& raw=array("selected_source_raw");
                const auto& stamps=array("exposure_stamps_ns");
                const auto& decoded=array("decoded_monotonic_ns");
                MappedPair p{};
                for(size_t i=0;i<2;++i){
                    if(raw[i].type!='i')throw std::runtime_error("Invalid exposure map RAW time");
                    double t=std::stod(raw[i].text);
                    if(!std::isfinite(t)||t<=0||t>=double(INT64_MAX/8)/1e9)throw std::runtime_error("Invalid exposure map RAW time");
                    p.raw[i]=nearest_even(t*1e9);
                    p.stamp[i]=phase5::integer(stamps[i]);p.decoded[i]=phase5::integer(decoded[i]);
                    if(p.stamp[i]<=0||p.decoded[i]>=INT64_MAX/8||p.decoded[i]<p.stamp[i]||p.decoded[i]-p.stamp[i]>250000000)
                        throw std::runtime_error("Invalid/stale exposure map time");
                }
                if(std::abs(p.raw[0]-p.raw[1])>2000)throw std::runtime_error("Exposure map stereo skew exceeds 2us");
                exposure_map_.emplace(p.raw[0],p);
            });
            if(exposure_map_.empty())throw std::runtime_error("Exposure map contains no usable frame_timing mappings; skipped_missing_mapping="+std::to_string(skipped_mapping_rows_));
            return;
        }
        if(!std::filesystem::exists(dataset/"camera_exposure_clock.jsonl")) {
            if(evidence_&&evidence_->domain=="CLOCK_MONOTONIC_RAW")
                throw std::runtime_error("Original camera-clock.json declares CLOCK_MONOTONIC_RAW but contains no per-conversion RAW-to-MONOTONIC bridge; retain original camera_exposure_clock.jsonl. Startup offset, clock_probe interpolation and replay-host clocks cannot reproduce Python CameraClock.convert");
            throw std::runtime_error("Missing original camera_exposure_clock.jsonl: conversion_now_ns required to verify original exposure freshness");
        }
        lines(dataset/"camera_exposure_clock.jsonl",[&](const phase5::Value& v){
            auto raw=phase5::number(v,"camera_raw_ns"),stamp=phase5::number(v,"source_time_ns"),now=phase5::number(v,"conversion_now_ns");
            auto uncertainty=phase5::number(v,"uncertainty_ns");
            const auto& domain=phase5::field(v,"time_domain");
            if(!phase5::flag(v,"verified")||domain.type!='s'||domain.text!="CLOCK_MONOTONIC"||
                raw<=0||raw>=INT64_MAX/8||stamp<=0||stamp>=INT64_MAX/8||now<stamp||now>=INT64_MAX/8||now-stamp>250000000||
                uncertainty<0||uncertainty>500000||!clocks_.emplace(raw,Clock{stamp,now}).second)
                throw std::runtime_error("Invalid/duplicate original exposure clock evidence");
            if(evidence_&&(uncertainty>evidence_->uncertainty||
               (evidence_->domain=="CLOCK_MONOTONIC"&&stamp!=raw+evidence_->offset)))
                throw std::runtime_error("Original exposure conversion contradicts camera-clock.json");
        });
    }
    size_t skipped_mapping_rows() const { return skipped_mapping_rows_; }
    size_t mapped_rows() const { return exposure_map_.size(); }
    ExposureHeads lookup(const std::array<camera::GrayFrame,2>& pair) const {
        ExposureHeads out{};std::array<int64_t,2> raw{};
        for(size_t i=0;i<2;++i){
            auto t=pair[i].metadata.timestamp;
            if(!t||!std::isfinite(*t)||*t<=0||*t>=double(INT64_MAX/8)/1e9)
                throw std::runtime_error("Invalid recorded camera time");
            raw[i]=nearest_even(*t*1e9);
        }
        // Python convert_pair converts the first exposure once, then preserves
        // integer RAW differences for the other view (one bridge per pair).
        if(mapped_){
            // Match BOTH decoded source times within 1us, never frame ordinal.
            // Multiple eligible rows are ambiguous even if their stamps agree.
            const MappedPair* match=nullptr;
            for(auto it=exposure_map_.lower_bound(raw[0]-1000);it!=exposure_map_.end()&&it->first<=raw[0]+1000;++it){
                if(std::abs(it->second.raw[1]-raw[1])>1000)continue;
                if(match)throw std::runtime_error("Ambiguous exposure map pair");
                match=&it->second;
            }
            if(!match)throw std::runtime_error("Unmapped exposure pair");
            if(std::abs(raw[0]-raw[1])>2000)throw std::runtime_error("Stereo exposure skew exceeds 2us");
            for(size_t i=0;i<2;++i){
                auto exposure=pair[i].metadata.exposure;
                if(!exposure||!std::isfinite(*exposure)||*exposure<=0||*exposure>=.001)throw std::runtime_error("Original exposure not short");
                out.stamps[i]=match->stamp[i];
                out.world_from_head[i]=heads_.head_pose(out.stamps[i]);
            }
            return out;
        }
        auto clock=clocks_.find(raw[0]);
        if(clock==clocks_.end())throw std::runtime_error("Missing original exposure clock conversion");
        if(std::abs(*pair[0].metadata.timestamp-*pair[1].metadata.timestamp)*1e9>2000)
            throw std::runtime_error("Stereo exposure skew exceeds 2us");
        for(size_t i=0;i<2;++i){
            out.stamps[i]=clock->second.stamp+raw[i]-raw[0];
            auto exposure=pair[i].metadata.exposure;
            if(!exposure||!std::isfinite(*exposure)||*exposure<=0||*exposure>=.001||
                out.stamps[i]<=0||out.stamps[i]>clock->second.now||clock->second.now-out.stamps[i]>250000000)
                throw std::runtime_error("Original stereo exposure stale/future or not short");
            out.world_from_head[i]=heads_.head_pose(out.stamps[i]);
        }
        return out;
    }
};
}
