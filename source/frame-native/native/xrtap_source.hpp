#pragma once
#include "device_config.hpp"
#include "live_camera.hpp"
#include <array>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#ifndef _WIN32
#include "../xrtap/xrtap.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <time.h>
namespace pico {
struct XrtapConfig {
    std::string path="/dev/shm/xrtap";
    // One mapping field; swap serial values here if hardware establishes the reverse mapping.
    std::array<std::pair<std::string,std::string>,2> cameras{{
        {"/dev/video0",pico::devices::upper(0)},{"/dev/video3",pico::devices::upper(1)}}};
    // Measured 2026-10-29 by image content against an XRService recording: the V4L2 timestamp of a
    // short frame is end of readout, 8.637 ms after the XRService exposure stamp; tracking profile
    // short exposure is 0.782267 ms.
    double ir_mean_threshold=8, exposure_us=782.267;
    int64_t ir_gap_ns=15000000;  // a frame stamped less than this after the previous frame of its camera is the short IR one
    int64_t offset_ns=-10137000;  // -8.637 ms readout match, then -1.5 ms found by the 2026-10-29 LED phase sweep
    // Lower cameras (og0ve10, 640x480 GREY, same normal/short cadence). Node->serial mapping verified by image content
    // 2026-10-31 (docs/LOWER_CAMERAS_2026-10-31.md: side frame vs the view synthesized from the upper cameras, r 0.86
    // swapped vs ~0 before): video9 = CAMLOWERRIGHT (right), video13 = CAMLOWERLEFT (left); the earlier default was mirrored.
    // OFF by default since 2026-10-31: they never see a tracker held in front or on the body at head pitch 0..-60 deg,
    // the blind lower mono search found 0 poses and spent >=841 solves on window glare (hand1). PICO_XRTAP_LOWER=1
    // or optics "xrtap_lower":true turns them on. Their offset is the upper value until measured for these sensors.
    bool lower_enabled=false, lower_mapping_verified=true;
    std::array<std::pair<std::string,std::string>,2> lower_cameras{{
        {"/dev/video9",pico::devices::lower(1)},{"/dev/video13",pico::devices::lower(0)}}};
    int64_t lower_offset_ns=-10137000;
    int64_t lower_pair_ns=1000000;   // stereo skew bound between the two lower nodes (as upper)
    int64_t lower_upper_ns=4000000;  // lower pair <-> upper pair of the same exposure (next IR exposure is >=8 ms away)
    int64_t lower_wait_ns=3000000;   // consumer waits this long for a late lower pair while the lower stream is live
};
class XrtapSource {
public:
    struct Pair { std::array<camera::GrayFrame,2> frames; };
    struct Counts { uint64_t pairs=0,dropped=0,torn=0,non_ir=0,unpaired=0,
        lower_pairs=0,lower_non_ir=0,lower_unpaired=0,lower_matched=0,lower_missed=0; };
    static int64_t now(clockid_t id=CLOCK_MONOTONIC) {
        timespec t{}; clock_gettime(id,&t); return int64_t(t.tv_sec)*1000000000+t.tv_nsec;
    }
private:
    XrtapConfig config_;
    // The trailing ioctl event ring (after the nodes) is unused by capture.
    static constexpr size_t map_bytes_=offsetof(xrtap_shm,event_seq);
    int fd_=-1;
    xrtap_shm* shm_=nullptr;
    std::atomic<bool> stop_{false};
    std::thread thread_;
    mutable std::mutex mutex_;
    std::condition_variable pairs_ready_;
    bool consumer_stopped_=false;
    Counts counts_;
    std::deque<Pair> ready_;
    // Lower pairs are retained (not handed over in bulk): the consumer takes the one matching each upper pair.
    std::deque<Pair> lower_ready_;
    std::condition_variable lower_ready_cv_;
    int64_t lower_seen_ns_=0;  // monotonic time of the last lower pair
    struct Pending { camera::GrayFrame frame; int64_t raw_ns; };
    std::deque<Pending> pending_[4];  // 0,1 upper by serial; 2,3 lower by configured node order
    std::array<int64_t,4> last_ts_{{0,0,0,0}};
    static constexpr size_t lower_capacity_=16;  // ~0.5 s of lower IR pairs
    // Diagnostic phase sweep: PICO_XRTAP_SWEEP="-4000000:4000000:500000" (start:end:step ns) and
    // PICO_XRTAP_SWEEP_S=8 add an extra offset to exposure stamps that steps every S seconds (ascending only,
    // so stamps never go backwards). Each change is logged to /tmp/pico-diag.log with the monotonic time.
    int64_t sweep_start_=0,sweep_end_=0,sweep_step_=0,sweep_period_=0,sweep_t0_=0,sweep_last_=INT64_MIN;
    int64_t sweep_offset(){
        if(!sweep_step_)return 0;
        const auto now_ns=now();
        if(!sweep_t0_)sweep_t0_=now_ns;
        const int64_t k=(now_ns-sweep_t0_)/sweep_period_;
        const int64_t value=std::min(sweep_end_,sweep_start_+k*sweep_step_);
        if(value!=sweep_last_){sweep_last_=value;std::ofstream("/tmp/pico-diag.log",std::ios::app)<<"xrtap_sweep t_ns="<<now_ns<<" offset_ns="<<value<<std::endl;}
        return value;
    }
    uint64_t seen_[XRTAP_NODES]{};
    void poll() {
        __atomic_store_n(&shm_->reader_heartbeat_ns,now(),__ATOMIC_RELEASE);
        for(unsigned n=0;n<std::min(__atomic_load_n(&shm_->nodes_used,__ATOMIC_ACQUIRE),XRTAP_NODES);++n) {
            auto& node=shm_->nodes[n];
            const auto ws=__atomic_load_n(&node.write_seq,__ATOMIC_ACQUIRE);
            if(ws<seen_[n]) {seen_[n]=ws;for(auto& q:pending_)q.clear();}
            if(ws-seen_[n]>XRTAP_SLOTS) {counts_.dropped+=ws-seen_[n]-XRTAP_SLOTS;seen_[n]=ws-XRTAP_SLOTS;}
            while(seen_[n]<ws) {
                const auto index=seen_[n]++;
                auto& s=node.slots[index%XRTAP_SLOTS];
                const auto a=__atomic_load_n(&s.seq,__ATOMIC_ACQUIRE);
                if(a&1) {++counts_.torn;continue;}
                const auto ts=s.v4l2_timestamp_ns;
                const auto number=s.frame_number;
                const uint32_t w=s.width,h=s.height,stride=s.stride,bytes=s.bytes;
                if(number!=index) {++counts_.torn;continue;}  // slot already reused by a newer frame
                if(!w||!h||stride<w||uint64_t(stride)*h>bytes||bytes>XRTAP_MAX_BYTES)continue;
                const std::string device(node.device,strnlen(node.device,sizeof node.device));
                int cam=-1;  // 0,1 upper node; 2,3 lower node
                for(int i=0;i<2;++i){
                    if(device==config_.cameras[i].first)cam=i;
                    if(config_.lower_enabled&&device==config_.lower_cameras[i].first)cam=2+i;
                }
                if(cam<0)continue;
                const bool lower=cam>=2;
                // Classify before copying: bright SLAM frames are never copied.
                if(lower?(w!=640||h!=480):(w!=1056||h!=1024))continue;
                // Short (IR) and normal exposures alternate per camera: the short frame's V4L2 stamp is
                // ~8.4 ms after the normal one, the next normal ~25 ms later (measured 2026-10-29 in the
                // tracking profile, where brightness is 31 vs 43 and cannot separate them). The lower
                // og0ve10 nodes interleave with the same cadence.
                const int64_t previous=last_ts_[cam];
                last_ts_[cam]=ts;
                if(!(previous>0&&ts-previous>0&&ts-previous<config_.ir_gap_ns)){++(lower?counts_.lower_non_ir:counts_.non_ir);continue;}
                camera::GrayFrame f;
                f.pixels.resize(size_t(w)*h);
                for(unsigned y=0;y<h;++y)std::memcpy(f.pixels.data()+size_t(y)*w,s.pixels+size_t(y)*stride,w);
                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                if(a!=__atomic_load_n(&s.seq,__ATOMIC_ACQUIRE)||number!=index) {++counts_.torn;continue;}
                f.decode_index=number; f.decoded_monotonic_ns=now();
                f.source_time_verified=true;
                f.metadata.timestamp=double(ts+(lower?config_.lower_offset_ns:config_.offset_ns)+sweep_offset())*1e-9;
                f.metadata.exposure=config_.exposure_us*1e-6;
                f.metadata.key.kind=38;
                int side=cam;
                if(lower)f.metadata.sensor=config_.lower_cameras[cam-2].second;
                else {
                    f.metadata.sensor=config_.cameras[cam].second;
                    side=config_.cameras[cam].second==pico::devices::upper(0)?0:1;
                }
                auto& q=pending_[side];
                // Retain one ring's worth while the other node is copied this poll.
                // The delivered stereo queue below always holds at most two pairs.
                if(q.size()==XRTAP_SLOTS){q.pop_front();++(lower?counts_.lower_unpaired:counts_.unpaired);}
                q.push_back({std::move(f),ts});
            }
        }
        while(!pending_[0].empty()&&!pending_[1].empty()) {
            const auto dt=pending_[0].front().raw_ns-pending_[1].front().raw_ns;
            if(std::abs(dt)>1000000){pending_[dt<0?0:1].pop_front();++counts_.unpaired;continue;}
            Pair pair;
            for(int i=0;i<2;++i){pair.frames[i]=std::move(pending_[i].front().frame);pending_[i].pop_front();}
            if(ready_.size()==2){ready_.pop_front();++counts_.dropped;}
            ready_.push_back(std::move(pair));++counts_.pairs;
            pairs_ready_.notify_one();
        }
        while(!pending_[2].empty()&&!pending_[3].empty()) {
            const auto dt=pending_[2].front().raw_ns-pending_[3].front().raw_ns;
            if(std::abs(dt)>config_.lower_pair_ns){pending_[dt<0?2:3].pop_front();++counts_.lower_unpaired;continue;}
            Pair pair;
            for(int i=0;i<2;++i){pair.frames[i]=std::move(pending_[2+i].front().frame);pending_[2+i].pop_front();}
            if(lower_ready_.size()==lower_capacity_)lower_ready_.pop_front();
            lower_ready_.push_back(std::move(pair));++counts_.lower_pairs;
            lower_seen_ns_=now();
            lower_ready_cv_.notify_all();
        }
    }
    // Nearest retained lower pair within lower_upper_ns of the upper exposure stamp; it and every
    // older lower pair leave the queue. Caller holds mutex_.
    std::optional<Pair> take_lower_locked(double upper) {
        std::optional<size_t> best;
        double best_dt=double(config_.lower_upper_ns)*1e-9;
        for(size_t i=0;i<lower_ready_.size();++i) {
            const double dt=std::abs(*lower_ready_[i].frames[0].metadata.timestamp-upper);
            if(dt<=best_dt){best=i;best_dt=dt;}
        }
        if(!best)return {};
        std::optional<Pair> out=std::move(lower_ready_[*best]);
        lower_ready_.erase(lower_ready_.begin(),lower_ready_.begin()+std::ptrdiff_t(*best)+1);
        return out;
    }
public:
    // The XRService tap is dormant unless XRTAP_ENABLE_FLAG exists. A tracker session creates it and waits
    // (up to 5 s, the tap checks every 2 s) for the ring; the session removes it at its end (disable_tap).
    static void enable_tap(const std::string& path) {
        if(path!="/dev/shm/xrtap")return;  // synthetic fixtures use their own file
        const int flag=::open(XRTAP_ENABLE_FLAG,O_CREAT|O_WRONLY|O_CLOEXEC,0644);
        if(flag>=0)::close(flag);
        for(int i=0;i<50;++i){
            struct stat st{};
            if(::stat(path.c_str(),&st)==0&&st.st_size>=int64_t(map_bytes_))return;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    static void disable_tap(){::unlink(XRTAP_ENABLE_FLAG);}
    explicit XrtapSource(XrtapConfig config={}):config_(std::move(config)) {
        if(const char* s=std::getenv("PICO_XRTAP_SWEEP")){
            long long a=0,b=0,c=0;
            if(std::sscanf(s,"%lld:%lld:%lld",&a,&b,&c)==3&&c>0&&b>=a){sweep_start_=a;sweep_end_=b;sweep_step_=c;}
            const char* p=std::getenv("PICO_XRTAP_SWEEP_S");
            sweep_period_=int64_t((p?std::atof(p):8.0)*1e9);
            if(sweep_period_<=0)sweep_step_=0;
        }
        enable_tap(config_.path);
        fd_=open(config_.path.c_str(),O_RDWR|O_CLOEXEC);
        struct stat st{};
        if(fd_<0||fstat(fd_,&st)||st.st_size<int64_t(map_bytes_)) {
            if(fd_>=0)close(fd_);
            throw std::runtime_error("Cannot map xrtap file");
        }
        void* map=mmap(nullptr,map_bytes_,PROT_READ|PROT_WRITE,MAP_SHARED,fd_,0);
        if(map==MAP_FAILED){close(fd_);throw std::runtime_error("xrtap mmap failed");}
        shm_=static_cast<xrtap_shm*>(map);
        if(shm_->magic!=XRTAP_MAGIC||(shm_->version!=XRTAP_VERSION)){munmap(shm_,map_bytes_);close(fd_);throw std::runtime_error("xrtap magic/version mismatch");}
        for(unsigned i=0;i<XRTAP_NODES;++i)seen_[i]=__atomic_load_n(&shm_->nodes[i].write_seq,__ATOMIC_ACQUIRE);
        try {thread_=std::thread([this]{while(!stop_){ {std::lock_guard<std::mutex> lock(mutex_);poll();} std::this_thread::sleep_for(std::chrono::microseconds(500));}});}
        catch(...){munmap(shm_,map_bytes_);close(fd_);throw;}
    }
    ~XrtapSource(){stop_consumer();stop_=true;if(thread_.joinable())thread_.join();
    munmap(shm_,map_bytes_);close(fd_);}
    XrtapSource(const XrtapSource&)=delete;
    Counts counts()const{std::lock_guard<std::mutex> lock(mutex_);return counts_;}
    std::deque<Pair> take_pairs(){std::lock_guard<std::mutex> lock(mutex_);std::deque<Pair> out;out.swap(ready_);return out;}
    // Stop wins over queued work; the mapping remains alive until the consumer joins.
    void stop_consumer(){
        {std::lock_guard<std::mutex> lock(mutex_);consumer_stopped_=true;}
        pairs_ready_.notify_all();lower_ready_cv_.notify_all();
    }
    std::deque<Pair> wait_pairs(){
        std::unique_lock<std::mutex> lock(mutex_);
        pairs_ready_.wait(lock,[this]{return consumer_stopped_||!ready_.empty();});
        std::deque<Pair> out;
        if(!consumer_stopped_)out.swap(ready_);
        return out;
    }
    // Lower pair of the same exposure as an upper pair stamped `upper` (seconds, offsets applied). While the
    // lower stream is live (a pair within the last second) and nothing newer has arrived, a late pair is
    // awaited up to lower_wait_ns. A miss is counted, never an error; stop wins.
    std::optional<Pair> lower_for(double upper){
        std::unique_lock<std::mutex> lock(mutex_);
        if(!config_.lower_enabled)return {};
        auto found=take_lower_locked(upper);
        const auto later=[&]{
            const double bound=upper+double(config_.lower_upper_ns)*1e-9;
            return !lower_ready_.empty()&&*lower_ready_.back().frames[0].metadata.timestamp>bound;
        };
        if(!found&&config_.lower_wait_ns>0&&lower_seen_ns_&&now()-lower_seen_ns_<1000000000&&!later()){
            const auto deadline=std::chrono::steady_clock::now()+std::chrono::nanoseconds(config_.lower_wait_ns);
            while(!found&&!consumer_stopped_&&!later()&&
                  lower_ready_cv_.wait_until(lock,deadline)!=std::cv_status::timeout)
                found=take_lower_locked(upper);
            if(!found&&!consumer_stopped_)found=take_lower_locked(upper);
        }
        ++(found?counts_.lower_matched:counts_.lower_missed);
        return found;
    }
    const XrtapConfig& config()const{return config_;}
};
}
#endif
