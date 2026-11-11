#include "live_camera.hpp"
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <thread>
using namespace pico::camera;
namespace {
using Clock=std::chrono::steady_clock;
void require(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
struct SlowPipe final:DecoderPipe {
    std::mutex mutex;
    std::deque<Bytes> pictures;
    std::string log;
    size_t offset=0;
    uint64_t index=0;
    size_t rasters_read=0;
    bool ended=false,serial=false,blocked=false;
    int fault=0;
    explicit SlowPipe(bool synchronous=false,bool stall=false):serial(synchronous),blocked(stall){}
    bool threaded_io() const override{return !serial;}
    size_t write(const uint8_t* b,size_t n) override {
        if(blocked)return 0;
        if(fault==5)return n+1;
        std::this_thread::sleep_for(std::chrono::milliseconds(8));
        uint64_t pts=0;bool found=false;
        for(size_t i=0;i+14<=n;++i)if(b[i]==0&&b[i+1]==0&&b[i+2]==1&&b[i+3]==0xe0){
            auto p=b+i+9;
            pts=(uint64_t((p[0]>>1)&7)<<30)|(uint64_t(p[1])<<22)|
                (uint64_t(p[2]>>1)<<15)|(uint64_t(p[3])<<7)|(p[4]>>1);
            found=true;break;
        }
        require(found,"fake PES timestamp missing");
        const std::string header="P5\n1056 1024\n255\n";
        Bytes frame(header.begin(),header.end());frame.resize(header.size()+1056*1024,uint8_t(index));
        std::lock_guard<std::mutex> lock(mutex);
        require(pictures.size()<8,"fake stdout stalled");
        if(fault==1)frame[0]='X';
        pictures.push_back(std::move(frame));
        if(fault!=4)log+="n: "+std::to_string(index+(fault==2?1:0))+" pts: "+std::to_string(pts+(fault==3?1:0))+" pts_time:1\n";
        ++index;
        if(fault==7)for(int i=0;i<34;++i)log+="n: 0 pts: 90000 pts_time:1\n";
        if(fault==8)log.assign(65537,'x');
        return n;
    }
    std::optional<size_t> read(bool err,uint8_t* b,size_t n) override {
        if(fault==6)return {};
        if(!err&&offset==0)std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::lock_guard<std::mutex> lock(mutex);
        if(err){
            // Reproduce stdout backpressure: showinfo is unavailable until two
            // complete rasters have been drained. One-raster waiting deadlocks.
            if((fault==9&&rasters_read<2)||(fault==10&&rasters_read<3))return size_t(0);
            auto count=(std::min)(n,log.size());std::memcpy(b,log.data(),count);log.erase(0,count);
            if(!count&&ended)return {};
            return count;
        }
        if(pictures.empty()){if(ended)return {};return size_t(0);}
        auto& frame=pictures.front();auto count=(std::min)(n,frame.size()-offset);
        std::memcpy(b,frame.data()+offset,count);offset+=count;
        if(offset==frame.size()){pictures.pop_front();offset=0;++rasters_read;}
        return count;
    }
    void finish_input() override {std::lock_guard<std::mutex> lock(mutex);ended=true;}
    std::optional<int> exit_status() override {return 0;}
    void close() override {}
};
AccessUnit unit(int stream,int i,const Bytes& bytes){
    AccessUnit a;a.key={38,uint8_t(stream),0};a.timestamp=1.+i/30.;a.exposure=.002;
    a.sensor="synthetic";a.annex_b=bytes;return a;
}
void stress(const std::string& exe,const Bytes& h264){
    std::unique_ptr<GrayDecoder> decoders[2];
    for(int s=0;s<2;++s){
        std::unique_ptr<DecoderPipe> pipe;
        if(exe.empty())pipe=std::make_unique<SlowPipe>();else pipe=linux_decoder(exe);
        decoders[s]=std::make_unique<GrayDecoder>(unit(s,0,h264).key,std::move(pipe));
    }
    const auto start=Clock::now();
    for(int i=0;i<300;++i){
        std::this_thread::sleep_until(start+std::chrono::nanoseconds(int64_t(i)*1000000000/30));
        for(int s=0;s<2;++s)decoders[s]->feed(unit(s,i,h264));
        // Deliberately no consumer poll during the entire feed interval.
    }
    std::this_thread::sleep_until(start+std::chrono::seconds(10));
    const double feed_seconds=std::chrono::duration<double>(Clock::now()-start).count();
    for(int s=0;s<2;++s)
        std::cout<<(exe.empty()?"slow fake":"real ffmpeg")<<" stream "<<s
            <<": "<<decoders[s]->decoded_count()/feed_seconds<<" fps during "<<feed_seconds<<"s feed\n"<<std::flush;
    for(auto& d:decoders)d->finish();
    const auto deadline=Clock::now()+std::chrono::seconds(5);
    for(int s=0;s<2;++s){
        auto& d=decoders[s];
        while(!d->finished()&&Clock::now()<deadline){d->poll(1);std::this_thread::sleep_for(std::chrono::milliseconds(5));}
        std::cout<<(exe.empty()?"slow fake":"real ffmpeg")<<" stream "<<s<<" drained: "<<d->decoded_count()
            <<" frames, "<<d->decoded_count()/std::chrono::duration<double>(Clock::now()-start).count()
            <<" fps including drain\n"<<std::flush;
    }
    for(auto& d:decoders){
        require(d->finished(),"decoder drain deadline");
        require(d->frames.size()==2,"latest-wins frame capacity");
        require(d->frames.back().decode_index>=298,"decoder did not sustain 30fps");
        require(d->frames.back().packet_timestamp_matched,"PTS not matched");
        require(d->frames.back().metadata.timestamp==1.+299/30.,"wrong final metadata");
        require(!d->frames.back().source_time_verified,"camera clock guard bypassed");
        require(d->unmatched_metadata()<=1,"metadata not consumed");
        require(d->dropped_frames>=297,"missing latest-wins drops");
        d->close();
    }
    std::cout<<(exe.empty()?"slow fake":"real ffmpeg")<<": two streams, 300 inputs each, 10s PASS\n";
}
}
int main(int argc,char** argv){
    try{
        Bytes bytes{0,0,0,1,0x65,1};
        // Regression: a consumer-driven decoder without poll fails at chunk 33.
        {GrayDecoder old(unit(0,0,bytes).key,std::make_unique<SlowPipe>(true));
            bool rejected=false;try{for(int i=0;i<34;++i)old.feed(unit(0,i,bytes));}catch(...){rejected=true;}
            require(rejected,"old single-thread regression did not fail");}
        // Threaded stalled stdin must still fail, never drop a reference picture.
        {GrayDecoder blocked(unit(0,0,bytes).key,std::make_unique<SlowPipe>(false,true));
            bool rejected=false;try{for(int i=0;i<40;++i)blocked.feed(unit(0,i,bytes));}catch(...){rejected=true;}
            require(rejected,"compressed overflow guard missing");}
        // Run the real worker path through strict header/index/PTS/timeout failures.
        {auto pipe=std::make_unique<SlowPipe>();pipe->fault=9;
            GrayDecoder d(unit(0,0,bytes).key,std::move(pipe));
            d.feed(unit(0,0,bytes));d.feed(unit(0,1,bytes));d.finish();
            auto deadline=Clock::now()+std::chrono::seconds(3);
            while(!d.finished()&&Clock::now()<deadline){d.poll(1);std::this_thread::sleep_for(std::chrono::milliseconds(5));}
            require(d.finished()&&d.decoded_count()==2,"stdout must drain two rasters before timestamp pairing");}
        // A third raster cannot extend the budget when timestamps never arrive.
        {auto pipe=std::make_unique<SlowPipe>();pipe->fault=10;auto* raw=pipe.get();
            GrayDecoder d(unit(0,0,bytes).key,std::move(pipe));
            for(int i=0;i<3;++i)d.feed(unit(0,i,bytes));
            d.finish();bool rejected=false;auto deadline=Clock::now()+std::chrono::seconds(3);
            while(!rejected&&Clock::now()<deadline){
                try{d.poll(1);}catch(const std::exception& e){
                    require(std::string(e.what())=="Decoded timestamp timeout","wrong raster budget failure");rejected=true;}
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
            require(rejected,"missing timestamp must time out at raster budget");
            d.close();require(raw->rasters_read==2,"stdout exceeded two-raster budget");}
        for(int fault=1;fault<=8;++fault){
            auto pipe=std::make_unique<SlowPipe>();pipe->fault=fault;
            GrayDecoder d(unit(0,0,bytes).key,std::move(pipe));d.feed(unit(0,0,bytes));
            bool rejected=false;auto deadline=Clock::now()+std::chrono::seconds(3);
            while(Clock::now()<deadline&&!rejected){
                try{d.poll(1);}catch(...){rejected=true;}
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
            require(rejected,"threaded guard failed to reject");
            bool stopped=false;try{d.feed(unit(0,1,bytes));}catch(...){stopped=true;}
            require(stopped,"failed decoder accepted new input");d.close();
        }
        // Join promptly while stdin is stalled and both readers are idle.
        {GrayDecoder d(unit(0,0,bytes).key,std::make_unique<SlowPipe>(false,true));
            d.feed(unit(0,0,bytes));auto start=Clock::now();d.close();d.close();
            require(Clock::now()-start<std::chrono::seconds(1),"close/join deadline");}
        stress("",bytes);
        if(argc==3){
            std::ifstream input(argv[2],std::ios::binary);require(bool(input),"missing H264 fixture");
            bytes.assign(std::istreambuf_iterator<char>(input),{});
            require(!bytes.empty(),"empty H264 fixture");stress(argv[1],bytes);
        }else std::cout<<"real ffmpeg: SKIP (supply absolute ffmpeg and single 1056x1024 IDR H264 fixture)\n";
        return 0;
    }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}




