#pragma once
#include "camera_transport.hpp"
#include <deque>
#include <functional>
#include <memory>
#include <stdexcept>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <exception>

namespace pico::camera {
// Nonblocking byte IO: nullopt = EOF, zero = would block. close must reap.
struct DecoderPipe {
    virtual ~DecoderPipe() = default;
    virtual size_t write(const uint8_t*, size_t) = 0;
    virtual std::optional<size_t> read(bool stderr_stream, uint8_t*, size_t) = 0;
    virtual void close() = 0;
    // Concurrent writer and separate output readers; wait is bounded for joining.
    virtual bool threaded_io() const { return true; }
    virtual void wait_ready(int stream);
    virtual void finish_input() { throw std::runtime_error("Decoder does not support EOF drain"); }
    virtual std::optional<int> exit_status() { return {}; }
};
std::vector<std::string> ffmpeg_argv(const std::string& executable);
std::unique_ptr<DecoderPipe> linux_decoder(const std::string& executable);
// Full uint8 raster: 1024 rows, 1056 columns; packed row stride 1056 bytes.
struct GrayFrame {
    Bytes pixels;
    AccessUnit metadata;
    uint64_t decode_index=0;
    int64_t decoded_monotonic_ns=0;
    bool packet_timestamp_matched=true;
    bool source_time_verified=false; // PTS pairing is NOT camera-clock calibration.
};
// Serialized consumer, with independent bounded I/O workers.
class GrayDecoder {
    std::unique_ptr<DecoderPipe> pipe_;
    TimestampMux mux_;
    StreamKey key_;
    std::deque<Bytes> chunks_;
    size_t written_=0;
    mutable std::mutex mutex_;
    std::condition_variable changed_;
    std::thread writer_, output_reader_, log_reader_, pairing_worker_;
    // Includes the raster currently being filled by stdout. Never more than two.
    std::deque<Bytes> rasters_;
    size_t raster_slots_=0;
    bool pairing_done_=false;
    bool threaded_=false, stopping_=false;
    std::exception_ptr worker_failure_;
    std::deque<GrayFrame> ready_frames_;
    uint64_t worker_dropped_=0;
    void write_loop();
    void read_loop(bool);
    void pair_loop();
    void worker_failed();
    void start_workers();
    std::map<uint64_t,AccessUnit> metadata_;
    std::deque<uint64_t> metadata_order_;
    std::deque<std::pair<uint64_t,uint64_t>> times_;
    Bytes image_;
    std::string log_;
    uint64_t index_=0;
    int64_t waiting_since_=0;
    bool closed_=false, failed_=false;
    bool finishing_=false,input_ended_=false,eof_[2]={false,false},finished_=false;
    size_t frame_capacity_=2;
    [[noreturn]] void fail(const char*);
    void parse_frames(int64_t now);
public:
    std::deque<GrayFrame> frames;
    uint64_t dropped_frames=0;
    GrayDecoder(StreamKey,std::unique_ptr<DecoderPipe>,size_t frame_capacity=2);
    ~GrayDecoder();
    void feed(const AccessUnit&);
    void poll(int64_t monotonic_ns);
    void close();
    // Offline only: drain queued input, half-close stdin, consume both outputs.
    void finish();
    bool finished() const;
    size_t queued_input() const;
    size_t unmatched_metadata() const;
    uint64_t decoded_count() const;
};
struct CameraHooks {
    bool continuous=false; // Linux production: drain independently of RF/optics.
    std::function<int64_t()> now;
    std::function<void(int64_t)> sleep;
    // Must execute recording_argv, deadline 8s, exit0 AND 'response: ok'.
    std::function<void(const std::vector<std::string>&)> command;
    std::function<std::unique_ptr<DecoderPipe>()> decoder;
    // Supervisor release barrier, checked before decoder or recording closure,
    // including the failed-start cleanup path. Empty for pre-MCU/offline sources.
    std::function<void()> before_close;
};
CameraHooks linux_camera_hooks(const std::string& absolute_ffmpeg);
std::vector<std::string> recording_argv(const std::string& vrcmd,bool root,bool start);
// Explicit close may throw; failed stop/archive retains ownership and evidence.
// Caller registers close with ProfileOwner BEFORE start. Consumers take bounded
// queue snapshots; continuous mode keeps draining while RF setup blocks optics.
class LiveCameraSource {
    std::thread follower_;
    std::mutex queue_mutex_, stop_mutex_;
    std::condition_variable wake_;
    bool stop_=false;
    std::exception_ptr failure_;
    void pump();
    void stop_follow();
    std::filesystem::path root_,archive_,path_;
    CameraHooks hooks_;
    std::string vrcmd_;
    bool as_root_,owned_=false,created_=false,started_=false,closed_=false;
    uint64_t budget_,start_size_=0;
    int64_t last_growth_=0;
    std::unique_ptr<GrowingOcc> reader_;
    std::map<StreamKey,Bytes> parameters_;
    std::map<StreamKey,std::unique_ptr<GrayDecoder>> decoders_;
    std::deque<AccessUnit> metadata;
    std::deque<GrayFrame> frames;
    uint64_t dropped=0;
public:
    LiveCameraSource(std::filesystem::path root,std::filesystem::path archive_target,
        CameraHooks,bool as_root=false,uint64_t budget=2ULL*1024*1024*1024,
        std::string vrcmd="/opt/steamvr/bin/linuxarm64/vrcmd");
    std::filesystem::path start();
    ~LiveCameraSource();
    std::deque<AccessUnit> take_metadata();
    std::deque<GrayFrame> take_frames();
    void poll();
    void close();
    bool owns_recording() const {return owned_;}
};
}
