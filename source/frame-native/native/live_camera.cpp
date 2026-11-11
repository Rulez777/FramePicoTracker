#include "guard_evidence.hpp"
#include "live_camera.hpp"
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <exception>
#include <regex>
#include <stdexcept>

namespace pico::camera {
std::vector<std::string> ffmpeg_argv(const std::string& executable) {
    return {executable,"-v","info","-threads","1","-fflags","nobuffer","-probesize","32768",
        "-analyzeduration","0","-f","mpegts","-i","pipe:0","-an","-copyts","-vf","showinfo",
        "-fps_mode","passthrough","-f","image2pipe","-vcodec","pgm","pipe:1"};
}
std::vector<std::string> recording_argv(const std::string& exe,bool root,bool start) {
    std::vector<std::string> a;
    if(root)a={"runuser","-u","steamos","--"};
    a.insert(a.end(),{exe,"--cmd","0",start?"start_tracking_recording 1 1 0":"stop_tracking_recording"});return a;
}
GrayDecoder::GrayDecoder(StreamKey key,std::unique_ptr<DecoderPipe> p,size_t capacity):pipe_(std::move(p)),key_(key),frame_capacity_(capacity) {
    if(!pipe_)throw std::runtime_error("Missing decoder pipe");
    if(!capacity||capacity>64)throw std::runtime_error("Invalid decoded frame capacity");
    threaded_=pipe_->threaded_io();
}
GrayDecoder::~GrayDecoder(){try{close();}catch(...) {}}
[[noreturn]] void GrayDecoder::fail(const char* s){failed_=true;throw std::runtime_error(s);}
void GrayDecoder::close(){
    {std::lock_guard<std::mutex> lock(mutex_);stopping_=true;}
    changed_.notify_all();
    for(auto* worker:{&writer_,&output_reader_,&log_reader_,&pairing_worker_})
        if(worker->joinable())worker->join();
    if(!closed_){pipe_->close();closed_=true;}chunks_.clear();
}
void GrayDecoder::finish(){std::lock_guard<std::mutex> lock(mutex_);finishing_=true;changed_.notify_all();}
bool GrayDecoder::finished() const {std::lock_guard<std::mutex> lock(mutex_);return finished_;}
size_t GrayDecoder::queued_input() const {std::lock_guard<std::mutex> lock(mutex_);return chunks_.size();}
size_t GrayDecoder::unmatched_metadata() const {std::lock_guard<std::mutex> lock(mutex_);return metadata_.size();}
uint64_t GrayDecoder::decoded_count() const {std::lock_guard<std::mutex> lock(mutex_);return index_;}
void GrayDecoder::feed(const AccessUnit& au){
    std::unique_lock<std::mutex> lock(mutex_);
    if(worker_failure_){auto error=worker_failure_;lock.unlock();close();std::rethrow_exception(error);}
    if(closed_||failed_||finishing_)throw std::runtime_error("Decoder stopped/failed; new IDR required");
    if(au.key<key_||key_<au.key||!au.timestamp||au.annex_b.size()>2*1024*1024)
        throw std::runtime_error("Invalid compressed camera input");
    if(chunks_.size()==32){failed_=true;lock.unlock();close();throw std::runtime_error("Compressed backlog exceeded; decoder reset and new IDR required");}
    auto encoded=mux_.encode(au.annex_b,*au.timestamp);
    // A wrapped/colliding outstanding PTS is ambiguous, never substitute metadata.
    if(metadata_.count(encoded.pts)){failed_=true;lock.unlock();close();throw std::runtime_error("Duplicate outstanding camera PTS");}
    auto metadata=au;metadata.annex_b.clear();
    metadata_.emplace(encoded.pts,std::move(metadata));metadata_order_.push_back(encoded.pts);
    while(metadata_order_.size()>256){metadata_.erase(metadata_order_.front());metadata_order_.pop_front();}
    chunks_.push_back(std::move(encoded.transport)); if(threaded_)try{start_workers();}catch(...){lock.unlock();close();throw;} changed_.notify_all();
}
void GrayDecoder::parse_frames(int64_t now){

    for(;;){
        const std::string header="P5\n1056 1024\n255\n";
        if(image_.size()<header.size() && std::count(image_.begin(),image_.end(),uint8_t('\n'))<3)return;
        if(image_.size()<header.size() || !std::equal(header.begin(),header.end(),image_.begin())){
            // Bound diagnostics and escape bytes; never print the image payload.
            std::string actual;size_t lines=0;
            for(size_t i=0;i<std::min<size_t>(image_.size(),256);++i){
                auto c=image_[i];
                if(c=='\n'){actual+="\\n";if(++lines==3)break;}
                else if(c=='\r')actual+="\\r";
                else if(c>=32&&c<=126)actual+=char(c);
                else {const char* hex="0123456789abcdef";actual+="\\x";actual+=hex[c>>4];actual+=hex[c&15];}
            }
            if(lines<3&&image_.size()<256)return;
            auto message="Expected 1056x1024 uint8 PGM (width x height); actual header=\""+actual+"\"";
            fail(message.c_str());
        }
        constexpr size_t pixels=1024*1056;
        if(image_.size()<header.size()+pixels)return;
        if(times_.empty()){
            if(!waiting_since_)waiting_since_=now;
            if(now-waiting_since_>2000000000LL)fail("Decoded timestamp timeout");
            return;
        }
        waiting_since_=0;auto t=times_.front();times_.pop_front();
        if(t.first!=index_)fail("Decoded timestamp/frame-index mismatch");
        auto m=metadata_.find(t.second);if(m==metadata_.end()){pico::guard_evidence::emit(23,"PTS",false);fail("Decoded PTS has no OCC metadata");}
        pico::guard_evidence::emit(23,"PTS",true);
        metadata_order_.erase(std::remove(metadata_order_.begin(),metadata_order_.end(),t.second),metadata_order_.end());
        GrayFrame f;f.pixels.assign(image_.begin()+header.size(),image_.begin()+header.size()+pixels);
        f.metadata=std::move(m->second);metadata_.erase(m);f.decode_index=index_++;f.decoded_monotonic_ns=now;
        auto& target=threaded_?ready_frames_:frames;
        if(target.size()==frame_capacity_){target.pop_front();if(threaded_)++worker_dropped_;else ++dropped_frames;}
        target.push_back(std::move(f));changed_.notify_all();
        image_.erase(image_.begin(),image_.begin()+header.size()+pixels);
    }
}
void GrayDecoder::poll(int64_t now){
    pico::guard_evidence::Scope guard_evidence_scope(24,"decoder-health");
    if(threaded_){
        std::unique_lock<std::mutex> lock(mutex_);
        if(worker_failure_){auto error=worker_failure_;lock.unlock();close();std::rethrow_exception(error);}
        if(closed_||failed_)throw std::runtime_error("Decoder stopped/failed");
        while(!ready_frames_.empty()){
            if(frames.size()==frame_capacity_){frames.pop_front();++dropped_frames;}
            frames.push_back(std::move(ready_frames_.front()));ready_frames_.pop_front();
        }
        dropped_frames+=worker_dropped_;worker_dropped_=0;
        if(eof_[0]&&eof_[1]&&(!threaded_||pairing_done_)){
            if(!image_.empty()||!times_.empty()){lock.unlock();close();throw std::runtime_error("Incomplete final decoded frame/timestamp");}
            lock.unlock();auto status=pipe_->exit_status();lock.lock();
            if(status){if(*status!=0){lock.unlock();close();throw std::runtime_error("Decoder exited unsuccessfully");}finished_=true;}
        }
        return;
    }

    if(closed_||failed_)throw std::runtime_error("Decoder stopped/failed");
    if(finished_)return;
    try {
        // Bounded work per invocation, with stderr drained before stdout.
        for(int stream=1;stream>=0;--stream)for(int pass=0;pass<64;++pass){
            if(eof_[stream])break;
            uint8_t b[65536];auto n=pipe_->read(stream!=0,b,sizeof(b));
            if(!n){if(!input_ended_)fail("Camera decoder output ended");
            eof_[stream]=true;break;}
            if(!*n)break;
            if(stream){
                log_.append(reinterpret_cast<char*>(b),*n);
                size_t end;
                while((end=log_.find('\n'))!=std::string::npos){
                    auto line=log_.substr(0,end);log_.erase(0,end+1);
                    static const std::regex pattern(R"(\bn:\s*(\d+)\s+pts:\s*(-?\d+)\s+pts_time:)");
                    std::smatch match;if(std::regex_search(line,match,pattern)){
                        if(times_.size()==32)fail("Decoded timestamp queue overflow");
                        times_.emplace_back(std::stoull(match[1]),uint64_t(std::stoll(match[2]))&((1ULL<<33)-1));
                    }
                }
                if(log_.size()>65536)fail("Decoder log line overflow");
            }else{
                image_.insert(image_.end(),b,b+*n);parse_frames(now);
                if(image_.size()>2*1024*1024)fail("Decoded image backlog exceeded");
            }
        }
        parse_frames(now);
        for(int pass=0;pass<32&&!chunks_.empty();++pass){
            auto& b=chunks_.front();auto n=pipe_->write(b.data()+written_,b.size()-written_);
            if(n>b.size()-written_)fail("Invalid pipe write count");
            written_+=n;if(written_==b.size()){chunks_.pop_front();written_=0;}if(!n)break;
        }
        if(finishing_&&chunks_.empty()&&!input_ended_){pipe_->finish_input();input_ended_=true;}
        if(eof_[0]&&eof_[1]&&(!threaded_||pairing_done_)){
            if(!image_.empty()||!times_.empty())fail("Incomplete final decoded frame/timestamp");
            auto status=pipe_->exit_status();
            if(status){if(*status!=0)fail("Decoder exited unsuccessfully");
            finished_=true;}
        }
    }catch(...){failed_=true;close();throw;}
}
void DecoderPipe::wait_ready(int){std::this_thread::sleep_for(std::chrono::milliseconds(2));}
namespace {
int64_t decoder_now(){
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}
}
void GrayDecoder::start_workers(){
    if(writer_.joinable())return;
    writer_=std::thread([this]{try{write_loop();}catch(...){worker_failed();}});
    output_reader_=std::thread([this]{try{read_loop(false);}catch(...){worker_failed();}});
    log_reader_=std::thread([this]{try{read_loop(true);}catch(...){worker_failed();}});
    pairing_worker_=std::thread([this]{try{pair_loop();}catch(...){worker_failed();}});
}
void GrayDecoder::worker_failed(){
    {std::lock_guard<std::mutex> lock(mutex_);
        if(!stopping_){worker_failure_=std::current_exception();failed_=true;stopping_=true;}}
    changed_.notify_all();
}
void GrayDecoder::write_loop(){
    for(;;){
        Bytes chunk;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            changed_.wait(lock,[this]{return stopping_||finishing_||!chunks_.empty();});
            if(stopping_)return;
            if(chunks_.empty()){
                input_ended_=true;lock.unlock();pipe_->finish_input();return;
            }
            chunk=std::move(chunks_.front());chunks_.pop_front();
        }
        size_t offset=0;
        while(offset<chunk.size()){
            {std::lock_guard<std::mutex> lock(mutex_);if(stopping_)return;}
            auto n=pipe_->write(chunk.data()+offset,chunk.size()-offset);
            if(n>chunk.size()-offset)throw std::runtime_error("Invalid pipe write count");
            offset+=n;if(!n)pipe_->wait_ready(0);
        }
    }
}
void GrayDecoder::pair_loop(){
    std::unique_lock<std::mutex> lock(mutex_);
    for(;;){
        changed_.wait(lock,[this]{return stopping_||!rasters_.empty()||eof_[0];});
        if(stopping_)return;
        if(rasters_.empty()){
            pairing_done_=true;changed_.notify_all();return;
        }
        if(!changed_.wait_for(lock,std::chrono::seconds(2),[this]{return stopping_||!times_.empty();}))
            throw std::runtime_error("Decoded timestamp timeout");
        if(stopping_)return;
        auto t=times_.front();times_.pop_front();
        if(t.first!=index_)fail("Decoded timestamp/frame-index mismatch");
        auto m=metadata_.find(t.second);
        if(m==metadata_.end()){pico::guard_evidence::emit(23,"PTS",false);fail("Decoded PTS has no OCC metadata");}
        pico::guard_evidence::emit(23,"PTS",true);
        metadata_order_.erase(std::remove(metadata_order_.begin(),metadata_order_.end(),t.second),metadata_order_.end());
        GrayFrame f;f.pixels=std::move(rasters_.front());rasters_.pop_front();--raster_slots_;
        f.metadata=std::move(m->second);metadata_.erase(m);
        f.decode_index=index_++;f.decoded_monotonic_ns=decoder_now();
        if(ready_frames_.size()==frame_capacity_){ready_frames_.pop_front();++worker_dropped_;}
        ready_frames_.push_back(std::move(f));changed_.notify_all();
    }
}
void GrayDecoder::read_loop(bool err){
    const int stream=err?1:0;
    static const std::regex pattern(R"(\bn:\s*(\d+)\s+pts:\s*(-?\d+)\s+pts_time:)");
    const std::string header="P5\n1056 1024\n255\n";
    constexpr size_t pixels=1056*1024;
    // Reader-owned storage: neither regex nor raster reads/copies hold mutex_.
    std::string log;
    Bytes raster;
    size_t cursor=0;
    bool reserved=false;
    for(;;){
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if(stopping_)return;
            if(!err&&!reserved){
                changed_.wait(lock,[this]{return stopping_||raster_slots_<2;});
                if(stopping_)return;
                ++raster_slots_;reserved=true;
            }
        }
        uint8_t bytes[65536];
        const size_t remaining=header.size()+pixels-cursor;
        auto n=pipe_->read(err,bytes,err?sizeof(bytes):std::min(sizeof(bytes),remaining));
        if(!n){
            std::lock_guard<std::mutex> lock(mutex_);
            if(!input_ended_)throw std::runtime_error("Camera decoder output ended");
            if(!err){
                if(cursor)throw std::runtime_error("Incomplete final decoded frame/timestamp");
                if(reserved)--raster_slots_;
            }
            eof_[stream]=true;changed_.notify_all();return;
        }
        if(!*n){pipe_->wait_ready(err?2:1);continue;}
        if(err){
            log.append(reinterpret_cast<char*>(bytes),*n);
            size_t end;
            while((end=log.find('\n'))!=std::string::npos){
                auto line=log.substr(0,end);log.erase(0,end+1);
                std::smatch match;
                if(std::regex_search(line,match,pattern)){
                    auto index=std::stoull(match[1]);
                    auto pts=uint64_t(std::stoll(match[2]))&((1ULL<<33)-1);
                    std::unique_lock<std::mutex> lock(mutex_);
                    if(!changed_.wait_for(lock,std::chrono::seconds(1),[this]{return stopping_||times_.size()<32;}))
                        throw std::runtime_error("Decoded timestamp queue overflow");
                    if(stopping_)return;
                    times_.emplace_back(index,pts);changed_.notify_all();
                }
            }
            if(log.size()>65536)throw std::runtime_error("Decoder log line overflow");
        }else{
            // A fixed raster plus a read cursor avoids vector-front memmoves.
            if(raster.empty())raster.reserve(pixels);
            for(size_t i=0;i<*n;++i,++cursor){
                if(cursor<header.size()){
                    if(bytes[i]!=uint8_t(header[cursor]))
                        throw std::runtime_error("Expected 1056x1024 uint8 PGM (width x height)");
                }else{
                    auto count=*n-i;
                    raster.insert(raster.end(),bytes+i,bytes+*n);
                    cursor+=count;break;
                }
            }
            if(cursor==header.size()+pixels){
                std::lock_guard<std::mutex> lock(mutex_);
                if(stopping_)return;
                rasters_.push_back(std::move(raster));cursor=0;reserved=false;
                changed_.notify_all();
            }
        }
    }
}
namespace {
using Paths=std::map<std::filesystem::path,uint64_t>;
Paths recordings(const std::filesystem::path& root){
    Paths out;for(auto& d:std::filesystem::directory_iterator(root))if(!d.is_symlink()&&d.is_directory()&&d.path().extension()==".dataset")
        for(auto& f:std::filesystem::directory_iterator(d.path()))if(!f.is_symlink()&&f.is_regular_file()&&f.path().extension()==".occ")out[f.path()]=f.file_size();
    return out;
}
template<class T> void latest(std::deque<T>& q,T value,size_t cap,uint64_t& dropped){
    if(q.size()==cap){q.pop_front();++dropped;}q.push_back(std::move(value));
}
}
LiveCameraSource::LiveCameraSource(std::filesystem::path root,std::filesystem::path archive,
    CameraHooks h,bool as_root,uint64_t budget,std::string vrcmd):root_(std::filesystem::canonical(root)),
    archive_(std::move(archive)),hooks_(std::move(h)),vrcmd_(std::move(vrcmd)),as_root_(as_root),budget_(budget){
    if(!hooks_.now||!hooks_.sleep||!hooks_.command||!hooks_.decoder||!budget_)throw std::runtime_error("Invalid camera hooks");
}
std::filesystem::path LiveCameraSource::start(){
    if(started_||closed_)throw std::runtime_error("Camera source already started/closed");
    started_=true;
    auto sizes=recordings(root_);hooks_.sleep(200000000);
    auto choose=[&](const Paths& current){
        for(auto& p:current){auto old=sizes.find(p.first);if(old==sizes.end()||p.second>old->second)
            if(path_.empty()||std::filesystem::last_write_time(p.first)>std::filesystem::last_write_time(path_))path_=p.first;}
    };
    // Reuse only recordings observed growing, not newly appeared paths.
    auto current=recordings(root_);Paths growing;
    for(auto& p:current)if(sizes.count(p.first)&&p.second>sizes.at(p.first))growing.insert(p);
    choose(growing);
    try{
        if(path_.empty()){
            if(std::filesystem::space(root_).available<budget_+512ULL*1024*1024)throw std::runtime_error("Insufficient camera disk budget");
            // An ambiguous command result must still require stop recovery.
            owned_=true;hooks_.command(recording_argv(vrcmd_,as_root_,true));
            auto until=hooks_.now()+8000000000LL;
            while(path_.empty()&&hooks_.now()<until){choose(recordings(root_));if(path_.empty())hooks_.sleep(50000000);}
            if(path_.empty())throw std::runtime_error("XRService did not produce recording");
            created_=!sizes.count(path_);
        }
        GrowingOcc prime(path_);
        for(auto& r:prime.poll(512,16*1024*1024))for(auto& au:camera_packets(r.kind,r.payload)){
            if(au.key.kind!=38)continue;
            auto p=parameter_sets(au.annex_b);if(!p.empty())parameters_[au.key]=std::move(p);
        }
        reader_=std::make_unique<GrowingOcc>(path_);reader_->seek_complete_tail();
        start_size_=std::filesystem::file_size(path_);last_growth_=hooks_.now();
        if(hooks_.continuous)follower_=std::thread([this]{
            try {
                std::unique_lock<std::mutex> stop_lock(stop_mutex_);
                while(!stop_){
                    stop_lock.unlock();
                    {std::lock_guard<std::mutex> lock(queue_mutex_);pump();}
                    stop_lock.lock();
                    wake_.wait_for(stop_lock,std::chrono::milliseconds(5),[this]{return stop_;});
                }
            }catch(...){std::lock_guard<std::mutex> lock(queue_mutex_);failure_=std::current_exception();}
        });
        return path_.parent_path();
    }catch(...){close();throw;}
}
void LiveCameraSource::poll(){
    std::lock_guard<std::mutex> lock(queue_mutex_);
    if(failure_)std::rethrow_exception(failure_);
    if(hooks_.continuous){if(!reader_||closed_)throw std::runtime_error("Camera source not running");
    return;}
    pump();
}
std::deque<AccessUnit> LiveCameraSource::take_metadata(){
    std::lock_guard<std::mutex> lock(queue_mutex_);
    if(failure_)std::rethrow_exception(failure_);
    std::deque<AccessUnit> result;result.swap(metadata);return result;
}
std::deque<GrayFrame> LiveCameraSource::take_frames(){
    std::lock_guard<std::mutex> lock(queue_mutex_);
    if(failure_)std::rethrow_exception(failure_);
    std::deque<GrayFrame> result;result.swap(frames);return result;
}
void LiveCameraSource::stop_follow(){
    {std::lock_guard<std::mutex> lock(stop_mutex_);stop_=true;}
    wake_.notify_all();
    if(follower_.joinable())follower_.join();
}
LiveCameraSource::~LiveCameraSource(){stop_follow();}
void LiveCameraSource::pump(){
    pico::guard_evidence::Scope guard_evidence_scope(24,"source-growth-owner");
    if(!reader_||closed_)throw std::runtime_error("Camera source not running");
    auto size=std::filesystem::file_size(path_);if(size<start_size_||size-start_size_>budget_)throw std::runtime_error("OCC recording budget/truncation");
    auto before=reader_->offset();
    for(auto& r:reader_->poll())for(auto au:camera_packets(r.kind,r.payload)){
        if(au.key.kind!=38)continue;
        auto metadata_only=au;metadata_only.annex_b.clear();latest(metadata,std::move(metadata_only),32,dropped);
        if(!au.upper())continue;
        auto p=parameter_sets(au.annex_b);if(!p.empty())parameters_[au.key]=std::move(p);
        if(!decoders_.count(au.key)&&decoders_.size()>=2)throw std::runtime_error("Upper camera stream identity changed; restart required");
        auto& decoder=decoders_[au.key];if(!decoder)decoder=std::make_unique<GrayDecoder>(au.key,hooks_.decoder());
        // Drain between units too: a single OCC read can contain >32 pictures.
        decoder->poll(hooks_.now());
        auto params=parameters_[au.key];params.insert(params.end(),au.annex_b.begin(),au.annex_b.end());au.annex_b=std::move(params);decoder->feed(au);
    }
    auto now=hooks_.now();if(reader_->offset()>before)last_growth_=now;
    else if(now-last_growth_>2000000000LL)throw std::runtime_error("Live camera recording stopped growing");
    for(auto& d:decoders_){d.second->poll(now);auto& q=d.second->frames;while(!q.empty()){latest(frames,std::move(q.front()),8,dropped);q.pop_front();}}
}
void LiveCameraSource::close(){
    if(closed_)return;
    if(hooks_.before_close)hooks_.before_close();
    stop_follow(); // Join before decoder/recording closure, after the MCU gate.
    std::exception_ptr failure;
    for(auto& d:decoders_)try{d.second->close();}catch(...){if(!failure)failure=std::current_exception();}
    if(owned_)try{hooks_.command(recording_argv(vrcmd_,as_root_,false));owned_=false;}
        catch(...){if(!failure)failure=std::current_exception();}
    if(failure)std::rethrow_exception(failure);
    if(created_&&!path_.empty()){
        // A copy failure retains the original. Never merge an old archive.
        if(std::filesystem::exists(archive_))throw std::runtime_error("Camera archive target exists");
        auto relative=std::filesystem::weakly_canonical(archive_).lexically_relative(std::filesystem::canonical(path_.parent_path()));
        if(relative.empty()||*relative.begin()!="..")throw std::runtime_error("Archive must be outside recording dataset");
        std::filesystem::create_directories(archive_.parent_path());
        std::filesystem::copy(path_.parent_path(),archive_,std::filesystem::copy_options::recursive);
        // Retain source evidence: delayed XRService writes require a separately
        // verified finalization protocol before automatic deletion is admitted.
        created_=false;
    }
    closed_=true;
}
}

#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_camera, "camera");




