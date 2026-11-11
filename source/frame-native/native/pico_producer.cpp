#include "device_config.hpp"
#include "session_duration.hpp"
#include "native_test_runtime.hpp"
#include "phase5/orchestrator.hpp"
#include "recording_replay.hpp"
#include "producer_replay.hpp"
#include "live_guard_inventory.hpp"
#include "owner_decision.hpp"
#include "live_preflight_linux.hpp"
#include "live_session_factory_linux.hpp"
#include "live_camera_worker_linux.hpp"
#include "live_recording_linux.hpp"
#include "live_entry_fixture_linux.hpp"
#include "replay_calibration.hpp"
#include "replay_pose_text.hpp"
#include <chrono>
#include <csignal>
#include "synthetic_tcl_server.hpp"
#include "camera_decode_fixture.hpp"
#include "openocd_process.hpp"
#include "phase5/os_adapters.hpp"
#include "live_session_owner.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <iomanip>
#include <cmath>
#include <set>
using namespace phase5;
volatile std::sig_atomic_t replay_interrupted=0;
void replay_signal(int){replay_interrupted=1;}
int64_t steady_ns(){return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();}
namespace {
void check(bool b,const char* why){if(!b)throw std::runtime_error(why);}
Value text(std::string s){Value v;v.type='s';v.text=std::move(s);return v;}
std::string config=R"({"diagnostic_pause_peers":[1,2],"slot_cycle":1,"tracker_poll_decimation_n":1,"omit_command19":true,"dense_frame_diagnostic":false,"compact_mailbox_batch":false,"fast_ring_snapshot":false,"interval_us":6000,"steady_interval_us":6000,"slot_us":900,"optics":{"phase_at_exposure_ticks":261700,"timed_windows":false,"optical_method":"calibrated_stereo_v1","imu_calibration_method":"sparse_observations_v1"},"coexistence":{"controllers_off":true,"min_controllers":0}})";
std::string registry=R"({"peers":[{"slot":0,"chip_id_le":"d4c3b2a1","product_sn":"PCTESTTRACKER0001"}]})";
std::string report=R"({"session_closed_verified":true,"ram_restored_verified":true,"mpu_restored":true,"trace_controls_restored_verified":true})";
std::string norm(const std::string& p){return std::filesystem::path(p).lexically_normal().generic_string();}
struct Fixture {
    std::map<std::string,std::string> disk;
    std::vector<std::string> events;
    std::string symlink_from,symlink_to;
    int64_t now=0,pid=10;
    double power_value=120,age=20;
    int point=0,fail_at=0;
    bool replace_after_throw=false,restart=true,age_advances=true;
    std::function<void()> before_replace;
    std::function<std::string(std::optional<double>)> power_override;
    Files fs;
    Runtime rt;
    BoundInvocation inv;
    void event(std::string s){events.push_back(std::move(s));if(++point==fail_at)throw std::runtime_error("Injected boundary failure");}
    Fixture() {
        inv.root="/candidate";inv.sha=sha256("synthetic invocation");inv.native="/external/XRService";inv.native_sha=sha256("native fixture");inv.power_tool="/external/power";inv.settings="/external/XRServiceSettings.json";inv.command={"/candidate/native_runner","--duration","150"};
        disk[inv.native]="native fixture";disk[inv.power_tool]="power fixture";
        fs.writer_exclusion=[&]{event("writer-exclusion");};
        fs.resolve=[&](const std::string& p){event("resolve:"+p);auto n=norm(p);if(n==symlink_from)return symlink_to;
        return n;};
        fs.read=[&](const std::string& p)->std::optional<std::string>{event("read:"+p);auto it=disk.find(norm(p));if(it==disk.end())return {};
        return it->second;};
        fs.exclusive=[&](const std::string& p,const std::string& b){event("exclusive:"+p);if(!disk.emplace(norm(p),b).second)throw std::runtime_error("Exclusive marker exists");};
        fs.compare_replace=[&](const std::string& p,const std::optional<std::string>& expected,const std::optional<std::string>& replacement){event("replace:"+p);if(before_replace)before_replace();
        auto it=disk.find(norm(p));std::optional<std::string> current;if(it!=disk.end())current=it->second;
        if(current!=expected)throw std::runtime_error("Concurrent file change");
        if(replacement)disk[norm(p)]=*replacement;else disk.erase(norm(p));if(replace_after_throw)throw std::runtime_error("Write succeeded but status lost");};
        rt.boot_id=[]{return std::string("boot-A");};rt.clock.now=[&]{return now;};rt.clock.sleep=[&](int64_t ns){event("sleep");now+=ns;if(age_advances)age+=double(ns)/1e9;};
        rt.managed=[&]{event("managed");return std::vector<ManagedProcess>{{pid,age}};};
        rt.terminate_native=[&](int64_t p){event("native-SIGTERM");check(p==pid,"Native PID binding");if(restart){++pid;age=0;}};
        rt.power=[&](const std::string& tool,std::optional<double> n,int64_t timeout){event(n?"power-set":"power-read");check(tool==inv.power_tool&&timeout==8000000000LL,"Exact power tool / timeout");if(power_override)return power_override(n);
        auto before=power_value;if(n)power_value=*n;
        return std::string("{\"read_error\":0,\"set_error\":0,\"before\":")+std::to_string(before)+(n?",\"set\":"+std::to_string(*n):"")+"}";};
    }
    void ready(){create_readiness(fs,inv,{"  Exact human answer: worn and tracked HMD, controllers OFF.\n",true,true,true,true},"boot-A",now);}
    void seal(const std::string& module) {
        disk["/candidate/build/roy_station.elf"]=module;disk["/candidate/attempt/station-config.json"]=config;disk["/candidate/attempt/registry.json"]=registry;disk["/candidate/native_runner"]="native executable fixture";
        auto v=parse(R"({"launch_admitted":true,"preparation_only":false,"working_directory":"/candidate","target":{"serial":"PCTESTTRACKER0001","chip_id_le":"d4c3b2a1","slot":0},"required_tracker_image_sha256":"040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d","files":{},"external_files":{},"external_paths":{"native":"/external/XRService","settings":"/external/XRServiceSettings.json","power_tool":"/external/power"},"command":["/candidate/native_runner","--duration","150"]})");
        for(auto name:{"build/roy_station.elf","attempt/station-config.json","attempt/registry.json","native_runner"}) {auto& b=disk["/candidate/"+std::string(name)];v.object["files"].object[name]=parse("{\"bytes\":"+std::to_string(b.size())+",\"sha256\":\""+sha256(b)+"\"}");}
        for(auto name:{inv.native,inv.power_tool})v.object["external_files"].object[name]=text(sha256(disk[name]));
        v.object["native_xrservice_sha256"]=text(inv.native_sha);reseal(v);
    }
    void reseal(const Value& v){disk["/invocation.json"]=encode(v);disk["/invocation.sha256"]=sha256(disk["/invocation.json"])+"\n";}
    BoundInvocation verify(){return verify_seals(fs,inv.root,"/invocation.json","/invocation.sha256",inv.command);}
};
int live_entry(int argc,char** argv){
    const bool camera_preflight=argc>2&&std::string(argv[2])=="--preflight-camera";
    const bool preflight=camera_preflight||(argc>2&&std::string(argv[2])=="--preflight-only");
    for(int i=3;i<argc;++i)if(std::string(argv[i])=="--preflight-camera")
        check(false,"--preflight-camera is accepted only immediately after --live");
    for(int i=2;i<argc;++i)if(std::string(argv[i])=="--skip-camera-preflight")
        check(preflight&&!camera_preflight&&i==3,"--skip-camera-preflight is accepted only immediately after --live --preflight-only");
#ifdef PICO_LIVE_FIXTURE_TEST_ONLY
#ifdef _WIN32
    throw std::runtime_error("Live entry fixture requires Linux");
#else
    return pico::live_fixture::entry(argc,argv);
#endif
#endif
    const bool owner_override=argc>2&&std::string(argv[2])=="--owner-admit-unproven";
    for(int i=2;i<argc;++i)if(std::string(argv[i])=="--owner-admit-unproven")
        check(owner_override&&i==2&&!preflight,"Owner override refused for preflight or misplaced option");
    if(owner_override)for(int i=3;i<argc;++i)
        check(std::string(argv[i]).rfind("--preflight",0)!=0,"Owner override refused for preflight");
    if(preflight){
        const bool skip=argc>3&&std::string(argv[3])=="--skip-camera-preflight";
        const int offset=skip?1:0;
        check(argc>=8+offset&&std::string(argv[6+offset])=="--",
            "Usage: pico_producer --live {--preflight-only [--skip-camera-preflight]|--preflight-camera} ROOT INVOCATION_JSON INVOCATION_SHA256 -- REVIEWED_NATIVE_ARGV...");
        std::vector<std::string> reviewed;for(int i=7+offset;i<argc;++i)reviewed.emplace_back(argv[i]);
        auto shell_word=[](const std::string& word){
            std::string result="'";
            for(char c:word)result+=c=='\''?"'\\''":std::string(1,c);
            return result+"'";
        };
        std::cout<<"preflight exact_command=";
        for(int i=0;i<argc;++i)std::cout<<shell_word(argv[i])<<' ';
        std::cout<<"\npreflight expected readiness: ROOT/attempt/human-readiness.json; current boot, matching sealed invocation, fresh human answer\n"<<std::flush;
        // Full A inventory is reported even when a read-only adapter later fails.
        std::cerr<<pico::missing_live_guards()<<'\n';
        std::signal(SIGINT,replay_signal);std::signal(SIGTERM,replay_signal);
        pico::linux_read_only_preflight(argv[3+offset],argv[4+offset],argv[5+offset],reviewed,[]{return replay_interrupted!=0;},skip,camera_preflight);
        std::cout<<"preflight_only_complete=true full_live_admitted=false\n";
        return 0;
    }
    std::cerr << pico::missing_live_guards() << '\n';
    if (!pico::live_parity_complete()) std::cerr<<"Live guard implementation report incomplete; continuing\n";
    if(!owner_override)pico::require_live_validation();
#ifndef _WIN32
    const int offset=owner_override?2:0;
    check(argc==7+offset && std::string(argv[5+offset])=="--duration" && pico::parse_session_duration(argv[6+offset])!=0,
          "Usage: pico_producer --live [--owner-admit-unproven ROOT/attempt/owner-decision.json] ROOT INVOCATION_JSON INVOCATION_SHA256 --duration 5..150");
    pico::session_duration_seconds()=pico::parse_session_duration(argv[6+offset]);
    std::cerr<<"session duration "<<pico::session_duration_seconds()<<" s"<<'\n';
    std::vector<std::string> actual;
    actual.push_back(std::filesystem::canonical(argv[0]).string());
    for(int i=1;i<argc;++i)actual.emplace_back(argv[i]);
    phase5::Files files;
    files.resolve=[](const std::string& path){
        return std::filesystem::canonical(path).string();
    };
    files.read=pico::preflight_detail::read_file;
    auto bound=pico::load_live_invocation(files,argv[2+offset],argv[3+offset],argv[4+offset],actual);
    if(std::filesystem::canonical(std::filesystem::current_path()).string()!=bound.root)std::cerr<<"Native live cwd differs from sealed invocation; continuing\n";
    if(owner_override)std::cerr<<"Owner override supplied; owner decision is informational\n";
    const auto invocation=phase5::parse(bound.invocation_bytes);
    const auto& paths=phase5::field(invocation,"external_paths");
    const auto optics=pico::preflight_detail::string_field(paths,"optics");
    const auto detector=pico::preflight_detail::string_field(paths,"detector");
    const auto station=phase5::parse(bound.sealed_files.at("attempt/station-config.json"));
    const auto ffmpeg=pico::use_xrtap(phase5::field(station,"optics"))?std::string{}:
        pico::preflight_detail::string_field(paths,"ffmpeg");
    for(const auto& path:{optics,detector})pico::preflight_detail::seal(invocation,path);
    if(!ffmpeg.empty())pico::preflight_detail::seal(invocation,ffmpeg);
    std::signal(SIGINT,replay_signal);
    std::signal(SIGTERM,replay_signal);
    pico::linux_supervised_session(bound,argv[3+offset],argv[4+offset],[bound,optics,detector,ffmpeg,owner_override]{
        auto sink=std::make_shared<pico::LiveEventSink>(bound.root+"/attempt/live-events.jsonl");
        sink->append("invocation",bound.sha);
        if(owner_override)sink->append("owner-admit-unproven",pico::owner_unproven_families());
        return std::make_unique<pico::LinuxLiveRecordingSession>(bound,optics,detector,ffmpeg,
            [sink](const std::string& name,const std::string& detail){sink->append(name,detail);});
    },[]{return replay_interrupted!=0;});
    return 0;
#else
    throw std::runtime_error("Native full-live parent requires Linux on Frame");
#endif
}
}
int main(int argc,char** argv){
    try{
#ifdef PICO_LIVE_FIXTURE_TEST_ONLY
        check(argc>1&&std::string(argv[1])=="--live", "Dedicated fixture binary only supports --live --test-fixture");
#endif
        if(argc==2&&std::string(argv[1])=="--missing-live-guards"){std::cout<<pico::missing_live_guards()<<'\n';return 0;}
        if(argc>1&&std::string(argv[1])=="--live")return live_entry(argc,argv);
        bool recorded=argc>1&&std::string(argv[1])=="--replay-live";
        if(recorded ? (argc<7) : ((argc!=3&&argc!=4)||std::string(argv[1])!="--dry-run"))
            throw std::runtime_error("Usage: pico_producer --dry-run <module> [fault] | --replay-live <module> <recording-directory> <absolute-ffmpeg> <private-led-json> <factory-camera-json> [seconds 1..900] [--poses output.jsonl] [--driver-proof PATH --camera-clock PATH] [--exposure-map PATH]");
        int seconds=900;
        std::string poses_path;
        std::string driver_proof_path,camera_clock_path,exposure_map_path;
        if(recorded){int next=7;if(next<argc&&std::string(argv[next]).rfind("--",0)!=0){std::string arg=argv[next++];size_t used=0;seconds=std::stoi(arg,&used);check(used==arg.size()&&seconds>=1&&seconds<=900,"Replay duration must be 1..900 seconds");}
            while(next<argc){std::string option=argv[next++];check(next<argc,"Replay option requires a path");
                std::string* target=nullptr;
                if(option=="--poses")target=&poses_path;
                else if(option=="--driver-proof")target=&driver_proof_path;
                else if(option=="--camera-clock")target=&camera_clock_path;
                else if(option=="--exposure-map")target=&exposure_map_path;
                check(target!=nullptr,"Unknown replay option");check(target->empty(),"Duplicate replay option");
                *target=argv[next++];check(!target->empty()&&target->rfind("--",0)!=0,"Replay option requires a path");
            }
            check(driver_proof_path.empty()==camera_clock_path.empty(),"--driver-proof and --camera-clock must be supplied together");
        }
        std::ofstream poses;
        if(!poses_path.empty()){check(!std::filesystem::exists(poses_path),"Pose output already exists");poses.open(poses_path);check(bool(poses),"Cannot create pose output");poses<<std::setprecision(17);}
        const auto deadline=steady_ns()+int64_t(seconds)*1000000000LL;
        if(recorded){std::signal(SIGINT,replay_signal);std::signal(SIGTERM,replay_signal);}
        auto guard=[&]{check(!replay_interrupted,"Replay cancelled");check(steady_ns()<deadline,"Unified replay deadline exceeded");};
        std::ifstream file(argv[2],std::ios::binary);if(!file)throw std::runtime_error("Cannot read module fixture");
        std::string module{std::istreambuf_iterator<char>(file),std::istreambuf_iterator<char>()};
        std::string fault=!recorded&&argc==4?argv[3]:"";check(fault.empty()||fault=="frame-release"||fault=="roy-probe-read"||fault=="worker-stopped"||fault=="worker-reentrant-stop"||fault=="identity-construction"||fault=="missing-report"||fault=="false-barrier"||fault=="camera-close"||fault=="session-failure"||fault=="session-cancel"||fault=="tracking-loss"||fault=="observer-loss"||fault=="observer-eof"||fault=="publication-send"||fault=="camera-stale"||fault=="phase-exhausted"||fault=="parent-loss"||fault=="wall-bound"||fault=="seal-change"||fault=="argv-change"||fault=="owner-unknown"||fault=="owner-lock-loss"||fault=="listener-change"||fault=="profile-writer"||fault=="controllers-on"||fault=="sram-write"||fault=="checkpoint-failure"||fault=="query-recovery-success"||fault=="query-recovery-exhaust"||fault=="query-link-loss"||fault=="query-native-n"||fault=="query-native-pending"||fault=="query-stale-nonce"||fault=="query-wrong-chip"||fault=="query-lease-expired"||fault=="replay-over-180","Unknown synthetic fault");
        std::filesystem::path evidence;
        if(recorded){
            evidence=std::filesystem::current_path()/("replay-live-evidence-"+std::to_string(steady_ns()));
            check(std::filesystem::create_directory(evidence),"Cannot reserve replay evidence directory");
            std::cerr<<"replay_evidence="<<evidence.string()<<'\n';
        }
        auto persist=[&](const char* name,const std::string& bytes){
            if(!recorded)return;
            std::ofstream out(evidence/name,std::ios::binary|std::ios::trunc);
            check(bool(out),"Cannot create replay artifact");out.write(bytes.data(),static_cast<std::streamsize>(bytes.size()));
            out.flush();check(bool(out),"Replay artifact write/flush failed");out.close();check(!out.fail(),"Replay artifact close failed");
        };
        persist("mode.json",R"({"mode":"replay-live","synthetic_radio":true,"synthetic_identity":true,"real_pixel_geometry":true,"world_reference":"measured_head_world","measured_standing_world":true,"live_publication":false,"crash_durable":false})");
        Fixture f;f.seal(module);auto bound=f.verify();f.inv=bound;f.ready();
        pico::LiveSessionOwner owner(f.fs,f.rt,bound);owner.apply();bool camera_closed=false;
        owner.own_camera([&]{if(fault=="camera-close")throw std::runtime_error("Injected camera close failure");
        camera_closed=true;});owner.launch_begin();
        int exit=0;bool running=true;Child child{123,[&]()->std::optional<int>{if(running)return {};
        return exit;},[]{throw std::runtime_error("Unexpected interrupt");}};
        owner.attach_child(child);
        try{
            check(pico::camera::synthetic_camera_decode()==2,"synthetic camera byte-pipe/PTS/drop fixture");
            uint64_t real_frames=0,radio_ticks=0;
            pico::ReplayOpticalCounts optical_counts;
            int seal_checks=0;
            auto result=unified_synthetic_replay([&](const std::function<void()>& tick){
                if(fault=="session-failure")throw std::runtime_error("Injected installed-session failure");
                if(fault=="session-cancel"){replay_interrupted=1;tick();}
                if(recorded){
                    const auto model=pico::load_tracker_leds(argv[5],pico::devices::serial());
                    auto library_root=std::filesystem::canonical(argv[0]).parent_path().parent_path().parent_path()/"candidate/release/optical";
                    pico::NativeOpticalLibraries libraries((library_root/"libpico_optics.so").string(),(library_root/"libpico_detector.so").string());
                    std::unique_ptr<pico::ReplayOptical> optics;
                    uint64_t file_id=0,stereo_id=0;
                    pico::camera::RecordingReplayHooks hooks;
                    hooks.begin_file=[&](const std::filesystem::path& dataset){
                        auto history=std::make_shared<pico::RecordingHeadHistory>(dataset,driver_proof_path,camera_clock_path,exposure_map_path);
                        if(!exposure_map_path.empty())std::cerr<<"exposure_map: mapped_rows="<<history->mapped_rows()
                            <<" skipped_missing_mapping="<<history->skipped_mapping_rows()<<'\n';
                        optics=std::make_unique<pico::ReplayOptical>(pico::load_replay_rig(dataset,argv[6]),model,libraries.api(),[history](const std::array<pico::camera::GrayFrame,2>& pair){return history->lookup(pair);});
                        ++file_id;
                        if(poses.is_open())optics->pose_sink=[&](const pico::Rigid& pose,const std::array<pico::camera::GrayFrame,2>& pair,uint64_t pair_id){
                            pico::write_replay_pose(poses,pose,pair,file_id,++stereo_id,pair_id,&optics->source_times);
                            poses.flush();check(bool(poses),"Pose output flush failed");};
                    };
                    hooks.end_file=[&]{
                        optics->finish();const auto& c=optics->counts;
                        optical_counts.pairs+=c.pairs;optical_counts.candidates+=c.candidates;optical_counts.stereo_verified+=c.stereo_verified;optical_counts.world_poses+=c.world_poses;optical_counts.unpaired+=c.unpaired;
                        for(const auto& r:c.rejected)optical_counts.rejected[r.first]+=r.second;
                        optics.reset();
                    };
                    hooks.tick=[&]{guard();tick();++radio_ticks;};
                    hooks.frame=[&](const pico::camera::GrayFrame& frame){
                        guard();check(!frame.source_time_verified,"Offline source cannot assert live clock provenance");
                        check(frame.packet_timestamp_matched&&frame.pixels.size()==1056u*1024u,"Recorded raster/PTS admission");
                        ++real_frames;
                        optics->frame(frame);guard();tick();++radio_ticks;
                    };
                    pico::camera::replay_recording(argv[3],argv[4],deadline,hooks);
                    if(poses.is_open()){poses.close();check(!poses.fail(),"Pose output close failed");}
                    check(real_frames>0&&radio_ticks>0,"No camera/radio interleaving");
                    check(optical_counts.world_poses>0,"Replay produced no measured-world poses; inspect head_history_or_clock rejections");
                }else {tick();++radio_ticks;}
            },guard,[&](const std::string& verified){
                check(cleanup_barriers(verified),"Restoration callback requires four barriers");
                persist("radio-run.json",verified);
                if(fault!="missing-report")f.disk[bound.root+"/attempt/radio-run.json"]=fault=="false-barrier"?"{}":verified;
            },fault,[&]{
                if(++seal_checks==4){
                    if(fault=="seal-change")f.disk["/candidate/build/roy_station.elf"]+="tamper";
                    if(fault=="argv-change"){auto inv=parse(f.disk["/invocation.json"]);inv.object["command"].array.push_back(text("--unreviewed"));f.reseal(inv);}
                }
                auto current=f.verify();check(current.sha==bound.sha&&current.command==bound.command,"Invocation changed within shared session");
            },[&]{owner.verify_active();},&owner,seconds);
            check(result.publications>=2,"synthetic identity/phase/publication replay");  // poses also publish before phase lock now
            auto barriers=result.barriers;check(cleanup_barriers(barriers),"MCU report admission");
            if(fault=="false-barrier")barriers="{}";
            if(fault!="missing-report")f.disk[bound.root+"/attempt/radio-run.json"]=barriers;
            running=false;
            std::cout<<"replay_real_decoded_frames="<<real_frames<<" radio_ticks="<<radio_ticks
                     <<" real_camera_candidates="<<optical_counts.candidates
                     <<" real_camera_stereo_verified_candidates="<<optical_counts.stereo_verified
                     <<" real_camera_world_poses="<<optical_counts.world_poses
                     <<" real_camera_optical_admitted="<<optical_counts.world_poses
                     <<" stereo_pairs="<<optical_counts.pairs<<" unpaired="<<optical_counts.unpaired
                     <<" world_reference="<<(recorded?"measured_head_world":"synthetic_fixture")
                     <<" measured_standing_world="<<(recorded&&optical_counts.world_poses>0?"true":"false")
                     <<" synthetic_identity=true synthetic_ptp2=2 pti1=0\n";
            for(const auto& r:optical_counts.rejected)std::cout<<"replay_optical_rejection="<<r.first<<" count="<<r.second<<'\n';
        }catch(...){running=false;exit=1;auto cleanup=owner.cleanup();std::cerr<<cleanup.json();persist("outer-cleanup.json",cleanup.json());persist("child-state.json",R"({"synthetic_child":true,"terminated":true,"exit_code":1})");throw;}
        auto state=bounded_wait(child,f.rt.clock,[]{return false;});check(state.terminated&&state.exit_code==0,"child completion");
        auto cleanup=owner.cleanup();persist("outer-cleanup.json",cleanup.json());persist("child-state.json",R"({"synthetic_child":true,"terminated":true,"exit_code":0})");std::cout<<cleanup.json();check(cleanup.errors.empty()&&!cleanup.deferred&&camera_closed&&cleanup.owned_camera_closed&&cleanup.native_settings_restored&&cleanup.power_restored&&cleanup.mcu_cleanup_verified,"outer cleanup");
        std::cout<<"REPLAY ONLY: synthetic profile/radio/identity; real-pixel calibrated stereo geometry in recorded measured HMD world; no live pose publication\n"<<cleanup.json();return 0;
    }catch(const std::exception& e){std::cerr<<"pico_producer: "<<e.what()<<'\n';return 1;}
}


#include "live_implementation_registry.hpp"
#ifndef _WIN32
PICO_LIVE_IMPLEMENTATION(pico_impl_native_entry, "native-entry");
#endif
