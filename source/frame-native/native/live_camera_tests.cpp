#include "native_test_runtime.hpp"
#include "camera_decode_fixture.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
using namespace pico::camera;
namespace {
int checks=0;
void check(bool b){++checks;if(!b)throw std::runtime_error("camera test failed");}
template<class F> void rejects(F f){bool caught=false;try{f();}catch(const std::exception&){caught=true;}check(caught);}
AccessUnit au(double time=1){AccessUnit a;a.key={38,0,0};a.timestamp=time;a.exposure=.002;a.sensor="CAMUPPERLEFT";a.annex_b={0,0,0,1,0x65,1};return a;}
}
int main(int argc,char** argv){
    if(argc==2&&std::string(argv[1])=="--fixture-ack"){std::cout<<"response: ok\n";return 0;}
    if(argc==2&&std::string(argv[1])=="--fixture-stderr-ack"){std::cerr<<"response: ok\n";return 0;}
    if(argc==2&&std::string(argv[1])=="--fixture-bad-exit"){std::cout<<"response: ok\n";return 7;}
    // Same executable can act as a fake ffmpeg child on Linux; no hardware IO.
    if(argc>1&&std::string(argv[1])=="-v"){
        std::cerr<<"[Parsed_showinfo_0] n: 0 pts: 90000 pts_time:1\n"<<std::flush;
        std::cout<<"P5\n1056 1024\n255\n"<<std::string(1024*1056,'x')<<std::flush;
        char b[4096];while(std::cin.read(b,sizeof(b))||std::cin.gcount()){}return 0;
    }
    try{
        check(synthetic_camera_decode()==2);
        // Old transposed header and square images have the same/nearby byte count;
        // neither may pass the strict full-camera shape guard.
        for(const std::string dimensions:{"1024 1056","1024 1024","1056 1056","1055 1024","1 1"}){
            auto p=std::make_unique<FixturePipe>();auto* raw=p.get();GrayDecoder d({38,0,0},std::move(p));
            d.feed(au());raw->image(0,90000);
            const std::string header="P5\n"+dimensions+"\n255\n";
            raw->output.assign(header.begin(),header.end()); // header alone must diagnose even tiny dimensions
            bool caught=false;
            try{d.poll(1);}catch(const std::exception& e){
                const std::string message=e.what();caught=message.find("actual header=\"P5\\n"+dimensions+"\\n255\\n\"")!=std::string::npos;
            }
            check(caught);rejects([&]{d.feed(au(2));});
        }
        {auto p=std::make_unique<FixturePipe>();auto* raw=p.get();GrayDecoder d({38,0,0},std::move(p));d.feed(au());d.poll(1);d.finish();d.poll(2);
            raw->image(0,90000);d.poll(3);d.poll(4);check(d.finished()&&d.frames.size()==1&&d.unmatched_metadata()==0);rejects([&]{d.feed(au(2));});}
        {auto p=std::make_unique<FixturePipe>();auto* raw=p.get();GrayDecoder d({38,0,0},std::move(p));d.feed(au());d.poll(1);d.finish();d.poll(2);
            raw->output={'P','5'};rejects([&]{d.poll(3);});}
        {auto p=std::make_unique<FixturePipe>();auto* raw=p.get();GrayDecoder d({38,0,0},std::move(p));raw->ended=true;rejects([&]{d.poll(1);});}
        check(ffmpeg_argv("/ffmpeg")==std::vector<std::string>({"/ffmpeg","-v","info","-threads","1","-fflags","nobuffer","-probesize","32768","-analyzeduration","0","-f","mpegts","-i","pipe:0","-an","-copyts","-vf","showinfo","-fps_mode","passthrough","-f","image2pipe","-vcodec","pgm","pipe:1"}));
        check(recording_argv("/vrcmd",true,true)==std::vector<std::string>({"runuser","-u","steamos","--","/vrcmd","--cmd","0","start_tracking_recording 1 1 0"}));
        check(recording_argv("/vrcmd",false,false).back()=="stop_tracking_recording");
        {auto p=std::make_unique<FixturePipe>();GrayDecoder d({38,0,0},std::move(p));for(int i=0;i<32;++i)d.feed(au(i+1));
        rejects([&]{d.feed(au(34));});rejects([&]{d.feed(au(35));});}
        {auto p=std::make_unique<FixturePipe>();GrayDecoder d({38,0,0},std::move(p));d.feed(au());rejects([&]{d.feed(au());});}
        for(int fault=0;fault<4;++fault){auto p=std::make_unique<FixturePipe>();auto* raw=p.get();GrayDecoder d({38,0,0},std::move(p));d.feed(au());raw->image(fault==0?1:0,fault==1?90001:90000);
            if(fault==2)raw->output[0]='X';
            if(fault==3)raw->log.clear();
            if(fault<3)rejects([&]{d.poll(1);});else{d.poll(1);rejects([&]{d.poll(2000000002LL);});}}
        // Owned recording: start -> tail only -> stop -> archive, no deletion.
        auto root=std::filesystem::path("build/camera-source-fixture");
        std::filesystem::create_directories(root);
        auto stamp=std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
        root/=stamp;std::filesystem::create_directory(root);
        auto dataset=root/"Deckard_1.dataset";auto archive=root/"archive"/"recording";
        // Synthetic RF wait: never poll the consumer while the source drains
        // a burst larger than the compressed queue and keeps latest metadata.
        {
            auto live_root=root/"continuous";
            std::filesystem::create_directory(live_root);
            auto live_dataset=live_root/"current.dataset";
            CameraHooks live;
            live.continuous=true;
            std::atomic<int64_t> live_now{1};
            live.now=[&]{return live_now.load();};
            live.sleep=[](int64_t){};
            live.decoder=[]{return std::make_unique<FixturePipe>();};
            live.command=[&](const std::vector<std::string>& args){
                if(args.back()=="start_tracking_recording 1 1 0"){
                    std::filesystem::create_directory(live_dataset);
                    std::ofstream(live_dataset/"frames.occ",std::ios::binary);
                }
            };
            LiveCameraSource continuous(live_root,root/"continuous-archive",live,false,1024*1024);
            continuous.start();
            Bytes burst;
            auto word=[](uint32_t v){return Bytes{uint8_t(v),uint8_t(v>>8),uint8_t(v>>16),uint8_t(v>>24)};};
            auto append=[](Bytes& dst,const Bytes& bytes){dst.insert(dst.end(),bytes.begin(),bytes.end());};
            for(int i=0;i<100;++i){
                Bytes payload;
                auto field=[&](uint32_t tag,Bytes value){append(payload,word(tag));append(payload,word(uint32_t(value.size())));append(payload,value);};
                double time=1.+i/30.;Bytes timestamp(sizeof(time));std::memcpy(timestamp.data(),&time,sizeof(time));
                field(17,{0});field(3,timestamp);
                const std::string sensor="CAMUPPERLEFT";field(55,Bytes(sensor.begin(),sensor.end()));
                field(14,{0x65,1});
                append(burst,{'o','c','c','i','p','i','t','l'});append(burst,word(38));append(burst,word(uint32_t(payload.size())));append(burst,payload);
            }
            {std::ofstream output(live_dataset/"frames.occ",std::ios::binary|std::ios::app);output.write(reinterpret_cast<const char*>(burst.data()),std::streamsize(burst.size()));}
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            // No consumer poll preceded this read. A dormant source has no rows;
            // feeding the whole burst before draining fails the 32-unit guard.
            auto rows=continuous.take_metadata();
            check(rows.size()==32 && rows.back().timestamp && *rows.back().timestamp==1.+99/30.);
            continuous.poll();
            live_now=3000000001LL;
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            rejects([&]{continuous.poll();});
            rejects([&]{(void)continuous.take_frames();});
            continuous.close();
        }
        int64_t now=0;int starts=0,stops=0;bool stop_failure=true,barriers=false;
        CameraHooks hooks;hooks.now=[&]{return now;};hooks.sleep=[&](int64_t n){now+=n;};
        hooks.decoder=[]{return std::make_unique<FixturePipe>();};
        hooks.before_close=[&]{if(!barriers)throw std::runtime_error("MCU barriers unresolved");};
        hooks.command=[&](const std::vector<std::string>& a){if(a.back()=="start_tracking_recording 1 1 0"){
            ++starts;std::filesystem::create_directory(dataset);std::ofstream(dataset/"frames.occ",std::ios::binary);
        }else{++stops;if(stop_failure)throw std::runtime_error("stop failed");}};
        LiveCameraSource source(root,archive,hooks,false,1024);
        auto selected=source.start();check(selected==std::filesystem::canonical(dataset));check(starts==1&&source.owns_recording());
        source.poll();now+=2000000001LL;rejects([&]{source.poll();});
        rejects([&]{source.close();});check(stops==0&&source.owns_recording());
        barriers=true;
        rejects([&]{source.close();});check(source.owns_recording()&&!std::filesystem::exists(archive));
        stop_failure=false;source.close();check(!source.owns_recording()&&stops==2&&std::filesystem::exists(archive/"frames.occ"));
        check(std::filesystem::exists(dataset/"frames.occ"));source.close();check(stops==2);
        // A preexisting growing recording is borrowed: no start/stop/archive.
        auto borrowed_root=root/"borrowed";auto borrowed_dataset=borrowed_root/"Deckard_2.dataset";
        std::filesystem::create_directories(borrowed_dataset);std::ofstream(borrowed_dataset/"frames.occ",std::ios::binary);
        auto borrowed_hooks=hooks;borrowed_hooks.sleep=[&](int64_t n){now+=n;std::ofstream append(borrowed_dataset/"frames.occ",std::ios::binary|std::ios::app);append.put('\0');};
        LiveCameraSource borrowed(borrowed_root,root/"unused-archive",borrowed_hooks,false,1024);
        (void)borrowed.start();check(!borrowed.owns_recording());borrowed.close();check(starts==1&&stops==2);
        check(!std::filesystem::exists(root/"unused-archive"));
        // The source's internal failed-start cleanup must honor the same gate:
        // an ambiguous recording start is retained until barriers are complete.
        {
            const auto failed_root=root/"failed-start";
            std::filesystem::create_directory(failed_root);
            auto failed_hooks=hooks;
            int failed_stops=0;bool released=false;
            failed_hooks.before_close=[&]{if(!released)throw std::runtime_error("Retained MCU");};
            failed_hooks.command=[&](const std::vector<std::string>& args){
                if(args.back()=="start_tracking_recording 1 1 0")throw std::runtime_error("Ambiguous start");
                ++failed_stops;
            };
            LiveCameraSource failed(failed_root,root/"failed-archive",failed_hooks,false,1024);
            rejects([&]{failed.start();});check(failed.owns_recording()&&failed_stops==0);
            rejects([&]{failed.close();});check(failed_stops==0);
            released=true;failed.close();check(!failed.owns_recording()&&failed_stops==1);
        }
#ifndef _WIN32
        {auto p=linux_decoder(std::filesystem::canonical(argv[0]).string());GrayDecoder d({38,0,0},std::move(p));d.feed(au());
        for(int i=0;i<400&&d.frames.empty();++i){d.poll(int64_t(i+1)*5000000);std::this_thread::sleep_for(std::chrono::milliseconds(5));}
        check(d.frames.size()==1&&d.frames[0].pixels[0]=='x');d.close();}
        rejects([]{(void)linux_decoder("/no/such/ffmpeg");});
        auto executable=std::filesystem::canonical(argv[0]).string();auto os_hooks=linux_camera_hooks(executable);
        os_hooks.command({executable,"--fixture-ack"});check(true);
        rejects([&]{os_hooks.command({executable,"--fixture-stderr-ack"});});
        rejects([&]{os_hooks.command({executable,"--fixture-bad-exit"});});
#endif
        std::cout<<"live camera: "<<checks<<" checks passed\n";return 0;
    }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}
