#pragma once
#include "live_camera.hpp"
#include <cstring>
#include <stdexcept>
namespace pico::camera {
// Explicit synthetic byte-pipe fixture, never a real ffmpeg or recording command.
struct FixturePipe final:DecoderPipe {
    bool threaded_io() const override {return false;} // Deterministic guard fixtures.
    Bytes output;std::string log;size_t offset=0,log_offset=0;bool closed=false,ended=false;
    void finish_input() override {ended=true;}
    std::optional<int> exit_status() override {return ended?std::optional<int>(0):std::nullopt;}
    size_t write(const uint8_t*,size_t n) override {return n;}
    std::optional<size_t> read(bool err,uint8_t* b,size_t n) override {
        auto total=err?log.size():output.size();auto& pos=err?log_offset:offset;
        auto count=(std::min)(n,total-pos);
        if(count)std::memcpy(b,err?reinterpret_cast<const uint8_t*>(log.data())+pos:output.data()+pos,count);
        pos+=count;if(!count&&ended)return {};
        return count;
    }
    void close() override {closed=true;}
    void image(uint64_t index,uint64_t pts,uint8_t value=37){
        const std::string h="P5\n1056 1024\n255\n";
        output.insert(output.end(),h.begin(),h.end());
        // Independent non-square raster: columns x, rows y, packed row stride 1056.
        for(size_t y=0;y<1024;++y)for(size_t x=0;x<1056;++x)
            output.push_back(uint8_t((value+3*x+7*y)%256));
        log+="[Parsed_showinfo_0] n: "+std::to_string(index)+" pts: "+std::to_string(pts)+" pts_time:1\n";
    }
};
inline size_t synthetic_camera_decode(){
    auto pipe=std::make_unique<FixturePipe>();auto* p=pipe.get();
    StreamKey key{38,0,0};GrayDecoder decoder(key,std::move(pipe));
    for(uint64_t i=0;i<3;++i){
        AccessUnit au;au.key=key;au.timestamp=1.0+double(i);au.exposure=.001;au.sensor="CAMUPPERLEFT";au.annex_b={0,0,0,1,0x65,1};
        decoder.feed(au);p->image(i,(i+1)*90000,uint8_t(37+i));
    }
    decoder.poll(1000000000);
    if(decoder.frames.size()!=2||decoder.dropped_frames!=1||decoder.frames.front().decode_index!=1||
       decoder.frames.back().pixels.size()!=1024*1056||decoder.frames.back().pixels[0]!=39||
       decoder.frames.back().pixels[1055]!=uint8_t((39+3*1055)%256)||
       decoder.frames.back().pixels[1056]!=46||
       decoder.frames.back().pixels[1023*1056+1055]!=uint8_t((39+3*1055+7*1023)%256)||
       !decoder.frames.back().packet_timestamp_matched||decoder.frames.back().source_time_verified||
       decoder.frames.back().metadata.exposure!=.001||decoder.frames.back().metadata.timestamp!=3.0)
        throw std::runtime_error("Synthetic camera decode contract failed");
    decoder.close();return 2;
}
}
