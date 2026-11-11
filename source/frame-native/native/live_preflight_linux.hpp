#include "xrtap_config.hpp"
#pragma once
#include "device_config.hpp"
#include "live_preflight.hpp"
#include "camera_preflight.hpp"
#include "replay_calibration.hpp"
#include "calibration_discovery.hpp"
#include "observer_process.hpp"
#include "owner_lifecycle.hpp"
#include "phase5/os_adapters.hpp"
#include "phase5/file_digest.hpp"
#include "linux_owned_files.hpp"
#include "live_pixel_pairs.hpp"
#include <iostream>
#include <iomanip>
#ifndef _WIN32
#include <thread>
namespace pico {
namespace preflight_detail {
inline int64_t clock_ns(clockid_t clock=CLOCK_MONOTONIC){
    timespec value{};if(clock_gettime(clock,&value))throw std::runtime_error("Preflight clock unavailable");
    return int64_t(value.tv_sec)*1000000000LL+value.tv_nsec;
}
inline std::optional<std::string> read_file(const std::string& path){
    owned_files::Fd fd(::open(path.c_str(),O_RDONLY|O_CLOEXEC|O_NOFOLLOW|O_NONBLOCK));
    if(fd.value<0){if(errno==ENOENT)return {};
    throw std::runtime_error("Preflight input unreadable: "+path);}
    struct stat info{};
    if(fstat(fd.value,&info)||!S_ISREG(info.st_mode))
        throw std::runtime_error("Preflight input nonregular: "+path);
    // Executables (ELF) such as the native XRService (~48 MiB) use the 256 MiB executable cap, like the streaming
    // seals; every other input (JSON/config) keeps the 16 MiB cap.
    char magic[4]={};
    const auto got=::pread(fd.value,magic,sizeof(magic),0);
    const bool elf=got==4&&magic[0]==0x7f&&magic[1]=='E'&&magic[2]=='L'&&magic[3]=='F';
    const off_t cap=elf?off_t(256)*1024*1024:off_t(16)*1024*1024;
    if(info.st_size>cap)
        throw std::runtime_error("Preflight input oversize: "+path+" size="+std::to_string(info.st_size));
    std::string bytes;char buffer[65536];
    for(;;){auto n=::read(fd.value,buffer,sizeof(buffer));if(n<0&&errno==EINTR)continue;
        if(n<0)throw std::runtime_error("Preflight read failed");
        if(!n)break;
        bytes.append(buffer,size_t(n));
        if(off_t(bytes.size())>cap)throw std::runtime_error("Preflight input grew beyond cap: "+path);}
    return bytes;
}
inline std::string required(const std::string& path){auto value=read_file(path);if(!value)throw std::runtime_error("Preflight input absent: "+path);
return *value;}
inline std::string string_field(const phase5::Value& value,const char* name){
    const auto& item=phase5::field(value,name);if(item.type!='s'||item.text.empty())throw std::runtime_error(std::string("Preflight string absent: ")+name);
    return item.text;
}
inline void seal(const phase5::Value& invocation,const std::string& path) try {
    const auto& files=phase5::field(invocation,"external_files");
    const auto& expected=phase5::field(files,path.c_str());
    bool executable=path=="/usr/bin/ffmpeg";
    const auto& paths=phase5::field(invocation,"external_paths");
    for(const auto* key:{"native","power_tool","openocd","producer"}){
        auto it=paths.object.find(key);if(it!=paths.object.end()&&it->second.text==path)executable=true;
    }
    if(expected.type!='s'||phase5::file_digest(path,executable).sha!=expected.text)
        throw std::runtime_error("Read-only adapter external seal mismatch: "+path);
}
catch(const std::exception& e){std::cerr<<e.what()<<"; adapter seal report only, continuing\n";}
inline void native_image(const std::string& path) try {
    (void)phase5::file_digest(path,true);
}
catch(const std::exception& e){std::cerr<<e.what()<<"; native image report only, continuing\n";}
// This child only queries systemd. It is never a debugger/SRAM owner.
inline std::pair<int,std::string> service_command(std::vector<std::string> command){
    int descriptors[2];if(pipe2(descriptors,O_CLOEXEC|O_NONBLOCK))throw std::runtime_error("Service read pipe failed");
    owned_files::Fd input(descriptors[0]),output(descriptors[1]);
    posix_spawn_file_actions_t actions;
    if(posix_spawn_file_actions_init(&actions))throw std::runtime_error("Service read actions failed");
    int error=posix_spawn_file_actions_adddup2(&actions,output.value,STDOUT_FILENO);
    if(!error)error=posix_spawn_file_actions_addopen(&actions,STDERR_FILENO,"/dev/null",O_WRONLY,0);
    std::vector<char*> argv;for(auto& part:command)argv.push_back(part.data());
    argv.push_back(nullptr);
    pid_t pid=-1;if(!error)error=posix_spawn(&pid,argv[0],&actions,nullptr,argv.data(),environ);
    posix_spawn_file_actions_destroy(&actions);
    if(error)throw std::runtime_error("Service read launch failed");
    ::close(output.value);output.value=-1;
    std::string bytes;int status=0;bool reaped=false;
    try{
        const auto until=clock_ns()+8000000000LL;
        for(;;){char buffer[1024];auto n=::read(input.value,buffer,sizeof(buffer));
            if(n>0){bytes.append(buffer,size_t(n));if(bytes.size()>4096)throw std::runtime_error("Service read output cap");}
            else if(n<0&&errno!=EAGAIN&&errno!=EINTR)throw std::runtime_error("Service read IO failed");
            if(!reaped){auto result=waitpid(pid,&status,WNOHANG);if(result==pid)reaped=true;else if(result<0&&errno!=EINTR)throw std::runtime_error("Service read reap failed");}
            if(reaped&&n==0)break;
            if(clock_ns()>=until)throw std::runtime_error("Service read deadline exceeded");
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }catch(...){if(!reaped){::kill(pid,SIGKILL);while(waitpid(pid,&status,0)<0&&errno==EINTR){}}throw;}
    while(!bytes.empty()&&std::isspace(static_cast<unsigned char>(bytes.back())))bytes.pop_back();
    while(!bytes.empty()&&std::isspace(static_cast<unsigned char>(bytes.front())))bytes.erase(bytes.begin());
    if(!WIFEXITED(status))throw std::runtime_error("Service query terminated abnormally");
    return {WEXITSTATUS(status),bytes};
}
inline std::pair<int,std::string> service(){
    const auto state=service_command({"/usr/bin/systemctl","--user","is-active","frame-pico-radio.service"});
    if((state.first==3||state.first==4)&&
       (state.second=="inactive"||state.second=="failed"||state.second=="unknown"))return state;
    std::string details="ActiveState/SubState=unavailable";
    try {
        const auto shown=service_command({"/usr/bin/systemctl","--user","show","frame-pico-radio.service",
            "--property=ActiveState","--property=SubState"});
        details=shown.second+" show_status="+std::to_string(shown.first);
    }catch(const std::exception& error){details+=std::string(" query_error=")+error.what();}
    throw std::runtime_error("Station service refused: unit=frame-pico-radio.service command=systemctl --user is-active status="+
        std::to_string(state.first)+" stdout=["+state.second+"] "+details);
}
inline void camera_read(const phase5::BoundInvocation& bound,const std::function<void()>& guard){
    const auto config=phase5::parse(bound.sealed_files.at("attempt/station-config.json"));
    const auto& optics=phase5::field(config,"optics");
    if(use_xrtap(optics)){
        const auto rig=load_replay_rig(xrtap_calibration(optics),string_field(optics,"factory_camera_file"));
        (void)rig.bind(pico::devices::upper(0),1056,1024);(void)rig.bind(pico::devices::upper(1),1056,1024);
        XrtapSource tap(xrtap_config(optics));
        xrtap_preflight(tap,guard);
        XrtapSource::disable_tap();  // preflight-only: no tracker session follows
        return;
    }
    const auto root=std::filesystem::path(string_field(optics,"dataset_root"));
    using Sizes=std::map<std::filesystem::path,uint64_t>;
    auto recordings=[&]{Sizes result;for(const auto& dir:std::filesystem::directory_iterator(root))
        if(!dir.is_symlink()&&dir.is_directory()&&dir.path().extension()==".dataset")
            for(const auto& file:std::filesystem::directory_iterator(dir.path()))
                if(!file.is_symlink()&&file.is_regular_file()&&file.path().extension()==".occ")result[file.path()]=file.file_size();
        return result;};
    auto before=recordings();std::this_thread::sleep_for(std::chrono::milliseconds(200));guard();
    auto after=recordings();std::filesystem::path selected;
    for(const auto& item:after)if(before.count(item.first)&&item.second>before.at(item.first)){
        if(selected.empty()||std::filesystem::last_write_time(item.first)>std::filesystem::last_write_time(selected))selected=item.first;}
    if(selected.empty())throw std::runtime_error("Read-only camera preflight needs an already-growing recording; recording/profile changes forbidden");
    camera::GrowingOcc source(selected);source.seek_complete_tail();
    auto boot=[] {auto value=required("/proc/sys/kernel/random/boot_id");while(!value.empty()&&std::isspace(static_cast<unsigned char>(value.back())))value.pop_back();
    return value;};
    // The active dataset calibration is provenance, not manufactured clock evidence.
    const auto calibration=required((selected.parent_path()/"sensorRigCalibration.json").string());
    (void)phase5::parse(calibration);
    const std::vector<std::filesystem::path> devices{unique_active_device(selected.parent_path())};
    const auto device=required(devices.front().string());(void)phase5::parse(device);
    // Bundles created by make_native_invocation must bind the calibration of
    // the recording actually selected, including a recording created later.
    // A new/unprepared dataset requires a new bundle, never implicit admission.
    const auto invocation=phase5::parse(bound.invocation_bytes);
    if(invocation.object.count("native_preflight_bundle")) {
        if(!phase5::flag(invocation,"native_preflight_bundle"))throw std::runtime_error("Invalid native preflight bundle marker");
        seal(invocation,(selected.parent_path()/"sensorRigCalibration.json").string());
        seal(invocation,devices.front().string());
    }
    if(calibration.size()>8*1024*1024||device.size()>8*1024*1024)
        throw std::runtime_error("Calibration file missing or too large");
    const auto rig=load_replay_rig(selected.parent_path(),string_field(optics,"factory_camera_file"));
    (void)load_tracker_leds(string_field(optics,"factory_led_file"),pico::devices::serial());
    CameraAcquisition acquisition(clock_ns(),boot(),selected.parent_path().string(),phase5::sha256(calibration+device));
    auto last_growth=clock_ns();auto previous=std::filesystem::file_size(selected);
    const auto initial_size=previous;
    for(;;){guard();const auto now=clock_ns();auto size=std::filesystem::file_size(selected);
        if(size<previous)throw std::runtime_error("Read-only camera recording truncated");
        if(size-initial_size>2ULL*1024*1024*1024)throw std::runtime_error("Read-only camera growth budget exceeded");
        if(size>previous){last_growth=now;previous=size;}
        if(now-last_growth>2000000000LL)throw std::runtime_error("Read-only camera source stopped growing");
        for(const auto& record:source.poll())for(const auto& row:camera::camera_packets(record.kind,record.payload)){
            if(row.upper()&&row.sensor)(void)rig.bind(*row.sensor,1056,1024);
            acquisition.observe(row,clock_ns(),{clock_ns(CLOCK_MONOTONIC_RAW),clock_ns(),clock_ns(CLOCK_BOOTTIME)});
        }
        if(acquisition.poll(clock_ns(),boot())){
            const auto& proof=acquisition.proof();
            std::cout<<"preflight_camera_samples="<<proof.samples<<" upper_exposures="<<proof.exposures[0]<<","<<proof.exposures[1]<<" ambiguous="<<(proof.ambiguous?"true":"false")<<'\n';return;}
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}
}
inline void linux_read_only_preflight(const std::string& root,const std::string& invocation_path,
    const std::string& anchor,const std::vector<std::string>& reviewed,std::function<bool()> cancelled,bool skip_camera_preflight=false,bool active_camera_preflight=false){
    using namespace preflight_detail;
    ReadOnlyPreflightHooks hooks;
    hooks.files.resolve=[](const std::string& path){return std::filesystem::weakly_canonical(std::filesystem::absolute(path)).string();};
    hooks.files.read=read_file;
    hooks.files.digest=phase5::file_digest;
    const auto canonical_root=hooks.files.resolve(root);
    hooks.report=[](const std::string& step,const std::string& result){
        std::cout<<"preflight ["<<step<<"] "<<result<<std::endl;
    };
    std::cout<<"preflight readiness_file="<<canonical_root<<"/attempt/human-readiness.json\n"
             <<"preflight invocation_file="<<invocation_path<<" invocation_sha256_file="<<anchor<<'\n'
             <<"preflight expected_reviewed_argv=";
    for(const auto& arg:reviewed)std::cout<<std::quoted(arg)<<' ';
    std::cout<<"\npreflight readiness: fresh human answer, matching boot and invocation SHA256, age <=300s; never created by this command\n"<<std::flush;
    hooks.boot=[]{auto value=required("/proc/sys/kernel/random/boot_id");while(!value.empty()&&std::isspace(static_cast<unsigned char>(value.back())))value.pop_back();
    return value;};
    hooks.now=[] {return clock_ns();};
    RoyLock lock;
    std::unique_ptr<OpenOcdProcess> debugger;
    std::function<int64_t()> debugger_pid;
    std::unique_ptr<ObserverProcess> observer;
    std::unique_ptr<TrackingObserver> tracking;
    phase5::Value invocation;
    std::string openocd;
    std::string watcher_sha;
    phase5::BenchTrackingGuard bench;
    const auto until=clock_ns()+(active_camera_preflight?180000000000LL:60000000000LL);
    auto guard=[&]{if(cancelled()||clock_ns()>=until)throw std::runtime_error("Read-only preflight cancelled/deadline");
        if(tracking){tracking->poll(bench);bench.check(clock_ns(),true,true);}};
    hooks.prepare=[&](const phase5::BoundInvocation& bound){
        guard();
        const auto plan=read_only_adapter_plan(bound);
        invocation=plan.invocation;
        if(active_camera_preflight&&(!invocation.object.count("native_camera_preflight")||
           !phase5::flag(invocation,"native_camera_preflight")))
            throw std::runtime_error("Camera mutation requires a bundle prepared with --preflight-camera");
        openocd=plan.openocd;
        watcher_sha=plan.watcher_sha;
        seal(invocation,openocd);
        seal(invocation,"/opt/steamvr/drivers/frame_hmd/resources/firmware/roy/openocd.cfg");
        native_image(openocd);
        native_image(bound.native);
        native_image(bound.power_tool);
        const auto config=phase5::parse(bound.sealed_files.at("attempt/station-config.json"));
        const auto& optics=phase5::field(config,"optics");
        for(const auto* field:{"factory_camera_file","factory_led_file"}){
            const auto path=string_field(optics,field);
            seal(invocation,path);
        }
        std::cout<<"preflight_sealed_adapter_plan=true preflight_readiness_verified=true\n";
    };
    hooks.ownership=[&]{guard();if(!lock.held())lock.acquire(roy_lock_path());
        auto evidence=lock.evidence();const auto state=service();evidence.service_status=state.first;evidence.service_state=state.second;
        if(debugger&&debugger_pid)evidence.debugger_pid=debugger_pid();
        verify_session_ownership(evidence);
        std::cout<<"preflight_roy_lock=true service_status="<<evidence.service_status
                 <<" service_state="<<evidence.service_state<<'\n';
    };
    hooks.tracking=[&]{
        guard();if(!observer){
            const auto binary=std::filesystem::path(canonical_root)/"release/tracking_watch";
            observer=std::make_unique<ObserverProcess>(binary,std::filesystem::path(canonical_root)/"attempt"/("preflight-driver-proof-"+std::to_string(clock_ns())+".jsonl"),watcher_sha);
            tracking=std::make_unique<TrackingObserver>(observer->hooks(cancelled));
            // A slow first observer row is logged every 3 s; only owner cancel or the
            // preflight deadline (same as guard()) leaves the wait.
            auto report=clock_ns()+3000000000LL;
            for(;;){if(cancelled()||clock_ns()>=until)throw std::runtime_error("Read-only preflight cancelled/deadline");
                tracking->poll(bench);if(bench.has_sample()&&bench.check(clock_ns(),true,false))break;
                if(clock_ns()>=report){std::cerr<<"Fresh tracking readiness unavailable after 3s; still waiting\n";
                    report=clock_ns()+3000000000LL;}
                std::this_thread::sleep_for(std::chrono::milliseconds(5));}
        }
        guard();};
    hooks.profile=[&](const phase5::BoundInvocation& bound){
        guard();const auto settings=read_file(bound.settings);
        if(settings){const auto data=phase5::parse(*settings);
            if(data.type!='{')throw std::runtime_error("Expected profile settings object");
            const auto capture=data.object.find("captureSessionSettings");
            if(capture!=data.object.end()){
                if(capture->second.type!='{')throw std::runtime_error("Expected capture settings object");
                const auto cameras=capture->second.object.find("trackingCameras");
                if(cameras!=capture->second.object.end()&&cameras->second.type!='{')throw std::runtime_error("Expected camera settings object");
            }
        }
        phase5::OsAdapters os(bound.root+"/attempt/preflight-profile.lock");
        native_image(bound.power_tool);
        auto runtime=os.runtime(bound.native);
        if(runtime.managed().size()!=1)std::cerr<<"Exactly one managed XRService required; continuing with available service state\n";
        const auto power=phase5::parse(runtime.power(bound.power_tool,{},8000000000LL));
        const auto& value=phase5::field(power,"before");
        if(phase5::integer(phase5::field(power,"read_error"))!=0||value.type!='i')throw std::runtime_error("Invalid original power setting");
        const auto watts=std::stod(value.text);if(!std::isfinite(watts)||watts<0||watts>600)throw std::runtime_error("Invalid original power setting");
        std::cout<<"preflight_profile_read=true power_read=true settings_present="<<(settings?"true":"false")
                 <<" settings_sha256="<<(settings?phase5::sha256(*settings):"absent")
                 <<" power_before="<<watts<<'\n';
    };
    hooks.attach=[&]{
        guard();
        seal(invocation,openocd);
        seal(invocation,"/opt/steamvr/drivers/frame_hmd/resources/firmware/roy/openocd.cfg");
        auto launch=linux_openocd_hooks((std::filesystem::path(canonical_root)/"attempt"/("preflight-openocd-"+std::to_string(clock_ns())+".log")).string(),openocd);
        debugger_pid=launch.child_pid;
        debugger=std::make_unique<OpenOcdProcess>(std::move(launch));
        std::cout<<"preflight OpenOCD argv=";
        auto command=openocd_argv();command[0]=openocd;
        for(const auto& arg:command)std::cout<<'['<<arg<<"] ";
        std::cout<<"\npreflight attach uses init; poll; poll off (same native/Python attach contract), no halt/reset/write\n"<<std::flush;
        debugger->start();
    };
    hooks.detach=[&]{if(debugger)debugger->close();};
    hooks.read=[&](uint32_t address,size_t count){guard();if(!debugger)throw std::runtime_error("Read-only debugger absent");
        DebugMemory memory([&](const std::string& command){return debugger->command(command);});return memory.read(address,count);};
    hooks.camera=[&](const phase5::BoundInvocation& bound,CameraAttemptJournal& journal){
        if(!active_camera_preflight){camera_read(bound,guard);return;}
        auto step=[&](const char* name,auto action){
            hooks.report(name,"START");
            try{action();hooks.report(name,"PASS");}
            catch(const std::exception& error){hooks.report(name,std::string("FAIL: ")+error.what());throw;}
        };
        guard();
        phase5::OsAdapters os(bound.root+"/attempt/preflight-camera-profile.lock");
        auto files=journal.writer(os.files());auto runtime=os.runtime(bound.native);
        const auto ordinary_sleep=runtime.clock.sleep;
        runtime.clock.sleep=[&](int64_t ns){
            if(cancelled()||clock_ns()>=until)throw std::runtime_error("Camera profile apply cancelled/deadline");
            ordinary_sleep(ns);
        };
        phase5::ProfileOwner profile(files,runtime,bound);
        std::shared_ptr<camera::LiveCameraSource> source;
        // The XRService restart deliberately interrupts tracking. Retire the
        // observer first; require fresh HMD/controller rows after each restart.
        auto retire_observer=[&]{
            if(observer)observer->close();
            tracking.reset();observer.reset();bench=phase5::BenchTrackingGuard{};
        };
        std::exception_ptr camera_failure;
        try{
            step("camera profile owner apply/settings3/power300/native restart",[&]{
                retire_observer();profile.apply();hooks.tracking();guard();
            });
            const auto config=phase5::parse(bound.sealed_files.at("attempt/station-config.json"));
            const auto& optics=phase5::field(config,"optics");
            if(use_xrtap(optics)){
                step("xrtap six matched IR stereo pairs",[&]{
                    const auto rig=load_replay_rig(xrtap_calibration(optics),string_field(optics,"factory_camera_file"));
                    (void)rig.bind(pico::devices::upper(0),1056,1024);(void)rig.bind(pico::devices::upper(1),1056,1024);
                    XrtapSource tap(xrtap_config(optics));
                    xrtap_preflight(tap,guard);
                    std::cout<<"preflight_xrtap_ir_pairs="<<tap.counts().pairs<<" clock=CLOCK_MONOTONIC_RAW\n";
                    XrtapSource::disable_tap();
                });
            }else{
            const auto dataset=string_field(optics,"dataset_root");
            const auto ffmpeg=string_field(phase5::field(invocation,"external_paths"),"ffmpeg");
            seal(invocation,ffmpeg);native_image(ffmpeg);
            std::filesystem::create_directory(dataset);
            source=std::make_shared<camera::LiveCameraSource>(dataset,
                bound.root+"/attempt/preflight-camera-recordings",camera::linux_camera_hooks(ffmpeg),geteuid()==0);
            profile.own_camera([source]{source->close();});
            std::filesystem::path selected;
            step("camera recording start",[&]{guard();selected=source->start();guard();});
            step("camera active calibration wait",[&]{
                std::cout<<"preflight_camera_selected_dataset="<<std::quoted(selected.string())
                         <<" calibration_wait_seconds=10 poll_ms=100 settle_ms=200\n"<<std::flush;
                await_active_calibration(selected,[]{return clock_ns();},
                    [](int64_t ns){std::this_thread::sleep_for(std::chrono::nanoseconds(ns));},
                    [&]{guard();profile.verify_active();});
            });
            const auto rig=load_replay_rig(selected,string_field(optics,"factory_camera_file"));
            LivePixelPairs pairs({rig.bind(pico::devices::upper(0),1056,1024),rig.bind(pico::devices::upper(1),1056,1024)},
                [](IdentityPair,const std::array<CameraMetadata,2>&){return false;});
            const auto provenance=phase5::sha256(phase5::encode(rig.factory)+phase5::encode(rig.active)+phase5::encode(rig.device));
            files.exclusive(bound.root+"/attempt/camera-active-calibration.json",phase5::encode(rig.active)+"\n");
            files.exclusive(bound.root+"/attempt/camera-active-device.json",phase5::encode(rig.device)+"\n");
            CameraAcquisition acquisition(clock_ns(),hooks.boot(),selected.string(),provenance);
            step("camera-clock six upper exposures/current-boot RAW",[&]{
                await_camera_ready([]{return clock_ns();},[&]{guard();profile.verify_active();},[&]{
                    source->poll();
                    for(const auto& row:source->take_metadata()){
                        acquisition.observe(row,clock_ns(),{clock_ns(CLOCK_MONOTONIC_RAW),clock_ns(),clock_ns(CLOCK_BOOTTIME)});
                    }
                    for(auto& frame:source->take_frames()){
                        // Diagnostic only: decode latency = decode time minus exposure time mapped RAW -> MONOTONIC.
                        if(frame.metadata.timestamp){
                            const double offset=double(clock_ns()-clock_ns(CLOCK_MONOTONIC_RAW));
                            const double exposure_mono=*frame.metadata.timestamp*1e9+offset;
                            std::cout<<"preflight_frame_age_ms="<<(double(frame.decoded_monotonic_ns)-exposure_mono)/1e6<<'\n';
                        }
                        pairs.frame(std::move(frame));
                    }
                    return acquisition.poll(clock_ns(),hooks.boot())&&pairs.pairs>=6;
                },[]{std::this_thread::sleep_for(std::chrono::milliseconds(5));});
                const auto& proof=acquisition.proof();
                std::cout<<"preflight_camera_samples="<<proof.samples<<" upper_exposures="<<proof.exposures[0]<<","<<proof.exposures[1]
                         <<" ambiguous="<<(proof.ambiguous?"true":"false")<<'\n';
                phase5::Value boot_value;boot_value.type='s';boot_value.text=hooks.boot();
                files.exclusive(bound.root+"/attempt/camera-preflight-proof.json",
                    "{\"rf_started\":false,\"clock_domain\":\"CLOCK_MONOTONIC_RAW\",\"boot_id\":"+phase5::encode(boot_value)+
                    ",\"upper_exposures\":["+std::to_string(proof.exposures[0])+","+std::to_string(proof.exposures[1])+
                    "],\"decoded_pairs\":"+std::to_string(pairs.pairs)+",\"calibration_sha256\":\""+provenance+"\"}\n");
            });
            step("camera decode/pairing health",[&]{pairs.health();if(pairs.pairs<6)throw std::runtime_error("Insufficient decoded stereo pairs");});
            }
        }catch(...){camera_failure=std::current_exception();}
        // Always close camera, restore settings/restart, then restore/read power.
        // ProfileOwner attempts the independent restorations even after failure.
        step("camera close/settings restoration/native restart/power restoration",[&]{
            // Cancellation must never interrupt restoration's bounded waits.
            runtime.clock.sleep=ordinary_sleep;
            std::exception_ptr observer_failure;
            try{retire_observer();}catch(...){observer_failure=std::current_exception();}
            const auto result=profile.cleanup();
            std::cout<<result.json()<<std::flush;
            if(result.deferred||!result.errors.empty()||!result.owned_camera_closed||
               !result.native_settings_restored||!result.power_restored)
                std::cerr<<"Camera profile restoration unresolved; inspect profile-cleanup.json; continuing\n";
            if(observer_failure)std::rethrow_exception(observer_failure);
        });
        step("camera restored HMD/controllers observer",[&]{hooks.tracking();guard();});
        if(camera_failure)std::rethrow_exception(camera_failure);
    };
    // A never-installed owner permits release through the existing typed lock API.
    OwnerIO denied;denied.read=[](uint32_t,size_t)->Bytes{throw std::runtime_error("No SRAM owner in preflight");};
    denied.write=[](uint32_t,const Bytes&){throw std::runtime_error("Preflight SRAM writes forbidden");};
    denied.command=[](const std::string&)->std::string{throw std::runtime_error("Preflight MCU invocation forbidden");};
    denied.now=[] {return clock_ns();};denied.sleep=[](int64_t){};
    SramOwner never_installed(std::move(denied),[](const Checkpoint&){throw std::runtime_error("Preflight install forbidden");});
    std::exception_ptr failure;
    std::string checkpoint;
    try{
        checkpoint=read_only_live_preflight(hooks,canonical_root,invocation_path,anchor,reviewed,skip_camera_preflight,active_camera_preflight);
    }catch(...){
        failure=std::current_exception();
    }
    std::cout<<"preflight [11 cleanup debugger/observer/Roy lock] START"<<std::endl;
    try { close_read_only_resources(
        [&]{if(debugger)debugger->close();},
        [&]{if(observer)observer->close();},
        [&]{if(lock.held())lock.release(never_installed);});
    }catch(const std::exception& error){std::cout<<"preflight [11 cleanup debugger/observer/Roy lock] WARN: "<<error.what()<<std::endl;}
    std::cout<<"preflight [11 cleanup debugger/observer/Roy lock] PASS"<<std::endl;
    if(failure)try{std::rethrow_exception(failure);}catch(const std::exception& e){std::cerr<<e.what()<<"; preflight report only\n";}
    std::cout<<"preflight_read_only_steps=true sram_checkpoint_sha256="<<phase5::sha256(checkpoint)
             <<" rf_started=false profile_changed="<<(active_camera_preflight?"restored":"false")<<" sram_written=false rf_command_transmitted=false\n";
}
}
#else
namespace pico {
inline void linux_read_only_preflight(const std::string&,const std::string&,const std::string&,
    const std::vector<std::string>&,std::function<bool()>,bool=false,bool=false){throw std::runtime_error("Real read-only live preflight requires Linux on the Frame");}
}
#endif

