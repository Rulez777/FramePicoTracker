#include "device_config.hpp"
#include "recording_replay.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <thread>

using namespace pico::camera;
namespace {
int64_t now(){return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();}
struct Stats {
    uint64_t packets=0,decoded=0,exposure_missing=0,exposure_invalid=0,latencies=0;
    double first=0,last=0,latency_sum=0,latency_max=0;
    uint64_t nonforward=0,pts_exposure_paired=0,decoded_exposure_missing=0,decoded_exposure_invalid=0;
    std::string sensor;
};
struct Pairing {
    std::deque<double> pending[2];
    std::optional<double> previous[2];
    uint64_t pairs=0,unpaired[2]={0,0};
    double max_skew_us=0;
    void frame(const AccessUnit& a){
        if(!a.upper()||!a.exposure||!std::isfinite(*a.exposure)||*a.exposure<=0||*a.exposure>=.001)return;
        int side=*a.sensor==pico::devices::upper(0)?0:1;
        if(previous[side]&&*a.timestamp<=*previous[side])throw std::runtime_error("Nonforward decoded stereo timestamp");
        previous[side]=*a.timestamp;
        auto& q=pending[side];
        if(q.size()==256){q.pop_front();++unpaired[side];}
        q.push_back(*a.timestamp);
        while(!pending[0].empty()&&!pending[1].empty()){
            double delta=pending[0].front()-pending[1].front();
            if(std::abs(delta)<=.000002){++pairs;max_skew_us=std::max(max_skew_us,std::abs(delta)*1e6);pending[0].pop_front();pending[1].pop_front();}
            else {int old=delta<0?0:1;pending[old].pop_front();++unpaired[old];}
        }
    }
};
void replay(const std::filesystem::path& path,const std::string& ffmpeg,int64_t deadline,const RecordingReplayHooks& hooks){
    const auto size=std::filesystem::file_size(path);
    const auto modified=std::filesystem::last_write_time(path);
    GrowingOcc reader(path);
    std::map<StreamKey,Stats> stats;
    std::map<StreamKey,std::unique_ptr<GrayDecoder>> decoders;
    std::map<StreamKey,Bytes> parameters;
    std::map<StreamKey,std::map<double,int64_t>> submitted;
    Pairing pairing;
    auto pump=[&]{
        if(now()>deadline)throw std::runtime_error("Replay deadline exceeded");
        if(hooks.tick)hooks.tick();
        for(auto& item:decoders){
            auto& d=*item.second;d.poll(now());
            while(!d.frames.empty()){
                auto f=std::move(d.frames.front());d.frames.pop_front();
                auto& s=stats[item.first];++s.decoded;
                if(!f.metadata.exposure)++s.decoded_exposure_missing;
                else {
                    ++s.pts_exposure_paired;
                    if(!std::isfinite(*f.metadata.exposure)||*f.metadata.exposure<=0||*f.metadata.exposure>=.001)
                        ++s.decoded_exposure_invalid;
                }
                if(!f.packet_timestamp_matched||f.source_time_verified)throw std::runtime_error("Replay PTS/provenance violation");
                auto& times=submitted[item.first];auto t=times.find(*f.metadata.timestamp);
                if(t==times.end())throw std::runtime_error("Missing decode submission time");
                double ms=double(f.decoded_monotonic_ns-t->second)/1e6;
                ++s.latencies;s.latency_sum+=ms;s.latency_max=std::max(s.latency_max,ms);times.erase(t);
                pairing.frame(f.metadata);
                if(hooks.frame)hooks.frame(f);
            }
        }
    };
    auto next_progress=now()+1000000000LL;
    for(;;){
        auto records=reader.poll(1);
        if(records.empty())break;
        for(auto& r:records)for(auto a:camera_packets(r.kind,r.payload)){
            if(a.key.kind!=38)continue;
            auto& s=stats[a.key];
            if(s.packets&&s.sensor!=a.sensor.value_or("unknown"))throw std::runtime_error("Camera sensor identity changed");
            s.sensor=a.sensor.value_or("unknown");
            if(!std::isfinite(*a.timestamp)||*a.timestamp<=0)throw std::runtime_error("Invalid camera timestamp");
            if(!s.packets)s.first=*a.timestamp;
            else if(*a.timestamp<=s.last)++s.nonforward;
            s.last=*a.timestamp;++s.packets;
            if(!a.exposure)++s.exposure_missing;
            else if(!std::isfinite(*a.exposure)||*a.exposure<=0||*a.exposure>=.001)++s.exposure_invalid;
            if(!a.upper())continue; // same two upper sensors as live_source
            auto p=parameter_sets(a.annex_b);if(!p.empty())parameters[a.key]=std::move(p);
            if(!decoders.count(a.key)){
                if(decoders.size()==2)throw std::runtime_error("Upper camera stream identity changed");
                decoders[a.key]=std::make_unique<GrayDecoder>(a.key,linux_decoder(ffmpeg),8);
            }
            auto& d=*decoders.at(a.key);
            // Bounded offline backpressure; do not overflow the live queue.
            while(d.queued_input()>=8||d.unmatched_metadata()>=128){pump();std::this_thread::sleep_for(std::chrono::milliseconds(1));}
            auto params=parameters[a.key];params.insert(params.end(),a.annex_b.begin(),a.annex_b.end());a.annex_b=std::move(params);
            auto& times=submitted[a.key];
            if(times.size()>=256)times.erase(times.begin());
            if(!times.emplace(*a.timestamp,now()).second)throw std::runtime_error("Duplicate input timestamp");
            d.feed(a);pump();
        }
        if(now()>=next_progress){std::cerr<<"replay "<<path.filename().string()<<" bytes="<<reader.offset()<<"/"<<size<<'\n';next_progress=now()+1000000000LL;}
    }
    if(reader.offset()!=size)throw std::runtime_error("Incomplete final OCC record (recording must be finalized)");
    for(auto& d:decoders)d.second->finish();
    auto drain_deadline=std::min<int64_t>(deadline,now()+8000000000LL);
    for(;;){
        pump();bool done=true;for(auto& d:decoders)done&=d.second->finished();
        if(done)break;
        if(now()>=drain_deadline)throw std::runtime_error("Decoder EOF drain exceeded eight seconds");
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    if(std::filesystem::file_size(path)!=size||std::filesystem::last_write_time(path)!=modified)throw std::runtime_error("Recording changed during offline replay");
    std::cout<<"file="<<std::quoted(path.string())<<" records="<<reader.records_emitted()<<" bytes="<<reader.offset()<<'\n';
    for(auto& item:stats){auto& k=item.first;auto& s=item.second;
        std::cout<<"camera="<<s.sensor<<" stream="<<k.kind<<':'<<unsigned(k.selector)<<':'<<k.ordinal
            <<" packets="<<s.packets<<" decoded="<<s.decoded<<" first_pts_seconds="<<s.first<<" last_pts_seconds="<<s.last
            <<" nonforward_pts="<<s.nonforward<<" exposure_missing="<<s.exposure_missing<<" exposure_not_short="<<s.exposure_invalid;
        auto d=decoders.find(k);
        if(d!=decoders.end()){
            std::cout<<" pts_matched="<<s.decoded<<" pts_exposure_paired="<<s.pts_exposure_paired
                <<" decoded_exposure_missing="<<s.decoded_exposure_missing<<" decoded_exposure_not_short="<<s.decoded_exposure_invalid
                <<" short_exposure_stereo_paired="<<pairing.pairs
                <<" short_exposure_unpaired="<<(pairing.unpaired[s.sensor==pico::devices::upper(0)?0:1]+pairing.pending[s.sensor==pico::devices::upper(0)?0:1].size())
                <<" unmatched_input="<<d->second->unmatched_metadata()<<" decoded_queue_drops="<<d->second->dropped_frames
                <<" decode_latency_samples="<<s.latencies<<" decode_feed_to_output_mean_ms="<<(s.latencies?s.latency_sum/double(s.latencies):0)<<" decode_feed_to_output_max_ms="<<s.latency_max;
            d->second->close();
        }else std::cout<<" decode=metadata_only_lower_camera";
        std::cout<<'\n';
    }
    std::cout<<"short_exposure_stereo_pairs="<<pairing.pairs<<" max_pair_skew_us="<<pairing.max_skew_us
        <<" unpaired_left="<<pairing.unpaired[0]+pairing.pending[0].size()<<" unpaired_right="<<pairing.unpaired[1]+pairing.pending[1].size()
        <<" source_time_verified=false offline_only=true\n"<<std::flush;
    for(auto& d:decoders)if(d.second->dropped_frames)throw std::runtime_error("Offline decoded queue lost frames");
    if(decoders.size()!=2||!pairing.pairs)throw std::runtime_error("No decoded short-exposure upper stereo pairs");
}
}
void pico::camera::replay_recording(const std::filesystem::path& directory,const std::string& ffmpeg,int64_t deadline,const RecordingReplayHooks& hooks){

    auto root=std::filesystem::canonical(directory);
    if(!std::filesystem::is_directory(root))throw std::runtime_error("Recording directory required");
    std::vector<std::filesystem::path> files;
    auto collect=[&](const std::filesystem::path& dir){for(auto& f:std::filesystem::directory_iterator(dir))
        if(!f.is_symlink()&&f.is_regular_file()&&f.path().extension()==".occ")files.push_back(f.path());};
    collect(root);
    for(auto& d:std::filesystem::directory_iterator(root))if(!d.is_symlink()&&d.is_directory()&&d.path().extension()==".dataset")collect(d.path());
    std::sort(files.begin(),files.end());if(files.empty())throw std::runtime_error("No OCC files in recording or immediate .dataset directories");
    std::cout<<std::setprecision(15);
    if(deadline<=now())throw std::runtime_error("Replay deadline already expired");
    for(auto& f:files){
        if(hooks.begin_file)hooks.begin_file(f.parent_path());
        replay(f,ffmpeg,deadline,hooks);
        if(hooks.end_file)hooks.end_file();
    }

}
