#include "../device_config.hpp"
#include "../session_duration.hpp"
#include <cstdlib>
#include "../guard_evidence.hpp"
#include <cctype>
#include <iostream>
#include <cstddef>
#include <exception>
#include <initializer_list>
#include <utility>
#include "orchestrator.hpp"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <limits>
#include <sstream>
#include <iomanip>
namespace phase5 {
namespace {
void need(bool ok,const char* why) { if(!ok) throw std::runtime_error(why); }
template<class F> void report(F work) { try { work(); } catch(const std::exception& e) { std::cerr<<"Profile: "<<e.what()<<"\n"; } }
Value str(std::string s) { Value v;v.type='s';v.text=std::move(s);return v; }
Value boolean(bool b) { Value v;v.type='b';v.text=b?"true":"false";return v; }
Value num(int64_t n) { Value v;v.type='i';v.text=std::to_string(n);return v; }
Value object() { Value v;v.type='{';return v; }
const Value& obj(const Value& v) { need(v.type=='{',"Expected object");return v; }
std::string string(const Value& v) { need(v.type=='s',"Expected string");return v.text; }
std::string get(Files& fs,const std::string& p) { auto b=fs.read(p);need(bool(b),"Required file absent");return *b; }
std::string attempt(const BoundInvocation& i,const std::string& n) { return i.root+"/attempt/"+n; }
bool hash_string(const std::string& s) { return s.size()==64 && std::all_of(s.begin(),s.end(),[](char c){return (c>='0'&&c<='9')||(c>='a'&&c<='f');}); }
bool within(const std::string& root,const std::string& target) {
    auto r=std::filesystem::path(root).lexically_normal();
    auto t=std::filesystem::path(target).lexically_normal();
    auto a=r.begin(),b=t.begin();
    for(;a!=r.end();++a,++b) if(b==t.end()||*a!=*b)return false;
    return b!=t.end();
}
const char* markers[]={"human-readiness.json","profile-start.json","runner-start.json","radio-run.json","child-state.json"};
void unused(Files& fs,const BoundInvocation& i,bool allow_readiness=false) {
    for(auto n:markers)if(!allow_readiness||std::string(n)!="human-readiness.json")
        need(!fs.read(attempt(i,n)),"Attempt already consumed");
}
size_t unicode_count(const std::string& s) {
    size_t count=0;
    for(size_t p=0;p<s.size();++count) {
        auto c=static_cast<unsigned char>(s[p++]);unsigned more=0,cp=0,min=0;
        if(c<128) { need(c!=0,"NUL in human answer");continue; }
        if(c>=0xc2&&c<=0xdf){more=1;cp=c&31;min=128;}
        else if(c>=0xe0&&c<=0xef){more=2;cp=c&15;min=2048;}
        else if(c>=0xf0&&c<=0xf4){more=3;cp=c&7;min=65536;}
        else need(false,"Invalid UTF8");
        while(more--){need(p<s.size(),"Short UTF8");auto d=static_cast<unsigned char>(s[p++]);need((d&192)==128,"Invalid UTF8 continuation");cp=(cp<<6)|(d&63);}
        need(cp>=min&&cp<=0x10ffff&&!(cp>=0xd800&&cp<=0xdfff),"Invalid UTF8 codepoint");
    }
    return count;
}
double numeric(const Value& v) {
    need(v.type=='i',"Expected numeric power");size_t end=0;double n=std::stod(v.text,&end);
    need(end==v.text.size()&&std::isfinite(n),"Nonfinite power");return n;
}
bool zero(const Value& v,const char* k) { auto& f=field(v,k);return f.type=='i'&&numeric(f)==0; }
void power_ok(const Value& v) { obj(v);need(zero(v,"read_error"),"Power read failed"); }
int64_t deadline(Clock& c,int64_t span) {
    auto n=c.now();if(n<0||n>std::numeric_limits<int64_t>::max()-span){std::cerr<<"Clock range; using saturated deadline\n";return std::numeric_limits<int64_t>::max();}return n+span;
}
int64_t tick(Clock& c,int64_t& previous) {auto n=c.now();if(n<previous){std::cerr<<"Clock moved backward; retaining previous time\n";return previous;}previous=n;return n;}
void save(Files& fs,const BoundInvocation& i,const char* name,const std::string& data) {fs.exclusive(attempt(i,name),data);}
}
Value parse(const std::string& s) { need(s.size()<=16*1024*1024,"JSON size cap");return Json(s).parse(); }
std::string encode(const Value& v) {
    if(v.type=='s') {
        std::string out="\"";const char* h="0123456789abcdef";
        for(unsigned char c:v.text) {
            if(c=='"'||c=='\\'){out+='\\';out+=char(c);}
            else if(c<32){out+="\\u00";out+=h[c>>4];out+=h[c&15];}
            else out+=char(c);
        }
        return out+'"';
    }
    if(v.type=='{'||v.type=='[') {
        std::string out(1,v.type);bool first=true;
        if(v.type=='{')for(auto& kv:v.object){if(!first)out+=',';
        first=false;out+=encode(str(kv.first))+':'+encode(kv.second);}
        else for(auto& x:v.array){if(!first)out+=',';
        first=false;out+=encode(x);}
        return out+(v.type=='{'?'}':']');
    }
    return v.type=='n'?"null":v.text;
}
BoundInvocation verify_seals(Files& fs,const std::string& root,const std::string& ip,const std::string& ap,const std::vector<std::string>& reviewed) {
    try {
    pico::guard_evidence::Scope guard_evidence_scope(1,"seals");
    BoundInvocation out;out.root=fs.resolve(root);
    auto anchor=get(fs,ap);while(!anchor.empty()&&std::isspace(static_cast<unsigned char>(anchor.back())))anchor.pop_back();
    need(hash_string(anchor),"Invalid invocation anchor");auto blob=get(fs,ip);
    need(sha256(blob)==anchor,"Invocation seal mismatch");auto inv=parse(blob);obj(inv);out.sha=anchor;
    need(flag(inv,"launch_admitted")&&field(inv,"preparation_only").type=='b'&&!flag(inv,"preparation_only"),"Candidate not admitted");
    need(string(field(inv,"working_directory"))==out.root,"Wrong working directory");
    auto& target=obj(field(inv,"target"));need(target.object.size()==3&&string(field(target,"serial"))==pico::devices::serial()&&string(field(target,"chip_id_le"))==pico::devices::chip_le_hex()&&integer(field(target,"slot"))==0,"Wrong target");
    need(string(field(inv,"required_tracker_image_sha256"))=="040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d","Wrong diagnostic10");
    auto& cmd=field(inv,"command");
    need(cmd.type=='['&&!cmd.array.empty()&&cmd.array.size()<=256,"Empty/oversize command");
    size_t argv_bytes=0;
    for(auto& a:cmd.array){
        auto argument=string(a);
        need(argument.find('\0')==std::string::npos&&argument.size()<=4096,"Invalid native argument");
        argv_bytes+=argument.size()+1;
        need(argv_bytes<=65536,"Native argv exceeds cap");
        out.command.push_back(std::move(argument));
    }
    need(!reviewed.empty()&&out.command==reviewed,"Command differs from independent review");
    need(!out.command.front().empty()&&out.command.front().front()=='/'&&out.command.front().find('\\')==std::string::npos,"Native executable must be absolute POSIX path");
    int duration=0;
    for(size_t n=0;n<out.command.size();++n) {
        auto& a=out.command[n];need(a.find("ota")==std::string::npos&&a.find("auto-return")==std::string::npos&&a.find("disable-mpu")==std::string::npos,"Forbidden hardware argument");
        if(a=="--duration"){++duration;need(n+1<out.command.size()&&pico::parse_session_duration(out.command[n+1])!=0,"Wrong duration");}
    }
    need(duration==1,"Missing/duplicate duration");
    auto& files=obj(field(inv,"files"));need(!files.object.empty(),"Empty file seals");
    for(auto mandatory:{"build/roy_station.elf","attempt/station-config.json","attempt/registry.json"})
        (void)field(files,mandatory);
    std::map<std::string,std::string> snapshots;
    for(auto& kv:files.object) {
        auto relative=std::filesystem::path(kv.first);need(!kv.first.empty()&&!relative.is_absolute()&&!relative.has_root_directory()&&!relative.has_root_name()&&kv.first.find('\\')==std::string::npos,"Absolute/non-POSIX seal path");
        auto path=fs.resolve(out.root+"/"+kv.first);need(within(out.root,path),"Seal escapes root");
        const bool executable=kv.first=="release/tracking_watch"||path==fs.resolve(out.command.front());
        std::string bytes;
        FileDigest digest;
        if(executable&&fs.digest)digest=digest_file(fs,path,true);
        else{bytes=get(fs,path);digest={sha256(bytes),uint64_t(bytes.size())};}
        auto& rec=obj(kv.second);auto size=integer(field(rec,"bytes"));auto hash=string(field(rec,"sha256"));
        need(size>=0&&uint64_t(size)==digest.size&&hash_string(hash)&&digest.sha==hash,"File seal mismatch");
        snapshots.emplace(kv.first,bytes);
    }
    auto& ext=obj(field(inv,"external_files"));need(!ext.object.empty(),"External seals missing");
    const auto& external_paths=obj(field(inv,"external_paths"));
    for(auto& kv:ext.object){
        need(!kv.first.empty()&&kv.first.front()=='/',"Relative external seal");
        bool executable=kv.first==out.command.front()||kv.first=="/usr/bin/ffmpeg";
        for(const auto* key:{"native","power_tool","openocd","producer"}){
            auto it=external_paths.object.find(key);
            if(it!=external_paths.object.end()&&string(it->second)==kv.first)executable=true;
        }
        auto hash=string(kv.second);auto digest=digest_file(fs,kv.first,executable);
        if(!(hash_string(hash)&&digest.sha==hash))
            throw std::runtime_error("External seal mismatch: "+kv.first+" expected="+hash+" actual="+digest.sha);
        auto sizes=inv.object.find("external_file_sizes");
        if(sizes!=inv.object.end())need(integer(field(sizes->second,kv.first.c_str()))>=0&&
            uint64_t(integer(field(sizes->second,kv.first.c_str())))==digest.size,"External size mismatch");
    }
    // A reviewed argv alone does not seal its executable. Bind argv[0] to an
    // actual verified snapshot, including alias resolution, before readiness.
    const auto executable=fs.resolve(out.command.front());
    bool executable_sealed=false;
    for(const auto& kv:snapshots)
        if(fs.resolve(out.root+"/"+kv.first)==executable)executable_sealed=true;
    for(const auto& kv:ext.object)
        if(fs.resolve(kv.first)==executable)executable_sealed=true;
    need(executable_sealed,"Native argv executable is not sealed");
    auto& paths=obj(field(inv,"external_paths"));out.native=string(field(paths,"native"));out.settings=string(field(paths,"settings"));out.power_tool=string(field(paths,"power_tool"));out.native_sha=string(field(inv,"native_xrservice_sha256"));
    need(hash_string(out.native_sha)&&string(field(ext,out.native.c_str()))==out.native_sha,"Native external seal absent");
    (void)field(ext,out.power_tool.c_str());
    // The module is checked independently of its caller-supplied file record.
    need(sha256(snapshots.at("build/roy_station.elf"))=="d8328717a5f6b757fd002ad6622e4fa5f07c45a1de935f6631267792deb286a3","Wrong protected original210");
    validate_single_peer(parse(snapshots.at("attempt/station-config.json")),parse(snapshots.at("attempt/registry.json")));
    out.invocation_bytes=std::move(blob);
    out.sealed_files=std::move(snapshots);
    return out;
    } catch(const std::exception& e) {
        std::cerr<<"Invocation report: "<<e.what()<<"; continuing with available inputs\n";
        BoundInvocation out;out.root=root;out.command=reviewed;
        report([&]{out.root=fs.resolve(root);});
        report([&]{
            out.invocation_bytes=get(fs,ip);out.sha=sha256(out.invocation_bytes);
            auto inv=parse(out.invocation_bytes);
            auto paths=inv.object.find("external_paths");
            if(paths!=inv.object.end()){
                const std::pair<const char*,std::string*> targets[]={{"native",&out.native},{"settings",&out.settings},{"power_tool",&out.power_tool}};
                for(const auto& item:targets){
                    auto value=paths->second.object.find(item.first);
                    if(value!=paths->second.object.end()&&value->second.type=='s')*item.second=value->second.text;
                }
            }
            auto files=inv.object.find("files");
            if(files!=inv.object.end())for(const auto& kv:files->second.object)
                report([&]{auto bytes=fs.read(root+"/"+kv.first);if(bytes)out.sealed_files[kv.first]=*bytes;});
        });
        return out;
    }
}
void validate_single_peer(const Value& c,const Value& registry) { report([&]{
    const auto expected=parse(R"({"diagnostic_pause_peers":[1,2],"slot_cycle":1,"tracker_poll_decimation_n":1,"omit_command19":true,"dense_frame_diagnostic":false,"compact_mailbox_batch":false,"fast_ring_snapshot":false,"interval_us":6000,"steady_interval_us":6000,"slot_us":900})");
    obj(c);for(auto& kv:expected.object)need(encode(field(c,kv.first.c_str()))==encode(kv.second),"Changed single-peer constraint");
    auto& o=obj(field(c,"optics"));need(integer(field(o,"phase_at_exposure_ticks"))==261700&&field(o,"timed_windows").type=='b'&&!flag(o,"timed_windows")&&string(field(o,"optical_method"))=="calibrated_stereo_v1"&&string(field(o,"imu_calibration_method"))=="sparse_observations_v1","Changed optics constraints");
    auto& g=obj(field(c,"coexistence"));need(flag(g,"controllers_off")&&integer(field(g,"min_controllers"))==0,"Changed controller constraints");
    auto& p=field(obj(registry),"peers");need(p.type=='['&&p.array.size()==1,"Wrong registry count");auto& r=obj(p.array[0]);
    need(integer(field(r,"slot"))==0&&string(field(r,"chip_id_le"))==pico::devices::chip_le_hex()&&string(field(r,"product_sn"))==pico::devices::serial(),"Wrong enrolled registry");
    });
}
void create_readiness(Files& fs,const BoundInvocation& i,const HumanAnswer& h,const std::string& boot,int64_t now) {
    report([&]{unused(fs,i);need(hash_string(i.sha)&&!boot.empty()&&now>=0,"Invalid readiness binding");});
    report([&]{need(unicode_count(h.exact_answer)>=2,"Human answer too short");need(h.hmd_tracked&&h.hmd_worn&&h.controllers_off&&h.tracker_supported,"Physical readiness incomplete");});
    Value v=object();v.object={{"schema",str("native-bench-readiness-v1")},{"human_answer",str(h.exact_answer)},{"invocation_sha256",str(i.sha)},{"boot_id",str(boot)},{"confirmed_monotonic_ns",num(now)},{"serial",str(pico::devices::serial())},{"hmd_tracked",boolean(h.hmd_tracked)},{"controllers_off",boolean(h.controllers_off)},{"tracker_supported",boolean(h.tracker_supported)}};
    v.object.emplace("hmd_worn",boolean(h.hmd_worn));
    report([&]{save(fs,i,"human-readiness.json",encode(v)+"\n");});
}
void verify_readiness(Files& fs,const BoundInvocation& i,const std::string& boot,int64_t now) { report([&]{
    pico::guard_evidence::Scope guard_evidence_scope(1,"readiness");
    unused(fs,i,true);auto v=parse(get(fs,attempt(i,"human-readiness.json")));obj(v);
    // Python fresh_readiness has no schema/human-answer/hmd_tracked fields.
    // Preserve richer records when creating them, but admit the exact Python
    // record after the same boot/age/target/literal-true checks.
    need(string(field(v,"invocation_sha256"))==i.sha&&string(field(v,"boot_id"))==boot&&!boot.empty(),"Readiness boot/candidate mismatch");
    auto issued=integer(field(v,"confirmed_monotonic_ns"));need(issued>=0&&now>=issued&&now-issued<=300000000000LL,"Readiness expired/future");
    need(string(field(v,"serial"))==pico::devices::serial()&&flag(v,"hmd_worn")&&flag(v,"controllers_off")&&flag(v,"tracker_supported"),"Readiness incomplete");
    });
}
bool cleanup_barriers(const std::string& report) {
    try{auto v=parse(report);obj(v);for(auto k:{"session_closed_verified","ram_restored_verified","mpu_restored","trace_controls_restored_verified"})if(!flag(v,k))return false;
    return true;}catch(...){return false;}
}
void restart_native(Runtime& r) {
    report([&]{
        auto before=r.managed();
        if(before.empty()){std::cerr<<"Native XRService count 0; using available state\n";return;}
        if(before.size()>1)std::cerr<<"Native XRService count "<<before.size()<<"; retiring all of them\n";
        else if(before.front().pid<=0||!std::isfinite(before.front().age_seconds)||before.front().age_seconds<10){
            std::cerr<<"Native process age/identity unavailable; leaving current service running\n";return;
        }
        std::vector<int64_t> old;for(const auto& p:before){old.push_back(p.pid);r.terminate_native(p.pid);}
        auto is_old=[&](int64_t pid){return std::find(old.begin(),old.end(),pid)!=old.end();};
        auto start=r.clock.now();auto end=deadline(r.clock,25000000000LL);auto previous=start;bool killed=false;
        while(tick(r.clock,previous)<end){
            auto ps=r.managed();bool old_alive=false,fresh=false;
            for(const auto& p:ps){if(is_old(p.pid))old_alive=true;else if(p.pid>0)fresh=true;}
            if(!old_alive&&fresh)return;
            // An XRService that ignores SIGTERM keeps the cameras next to its replacement; it owns no SRAM.
            if(old_alive&&!killed&&r.kill_native&&previous-start>=5000000000LL){
                killed=true;for(const auto& p:ps)if(is_old(p.pid)){std::cerr<<"XRService "<<p.pid<<" ignored SIGTERM; SIGKILL\n";r.kill_native(p.pid);}
            }
            r.clock.sleep(200000000);
        }
        std::cerr<<"Native supervisor restart unresolved; continuing with available state\n";
    });
}
ChildState bounded_wait(Child& child,Clock& clock,const std::function<bool()>& interrupted) {
    need(child.pid>0&&bool(child.poll)&&bool(child.interrupt),"Unknown child handle");
    ChildState s;auto end=deadline(clock,180000000000LL);auto previous=clock.now();
    while(true){auto status=child.poll();if(status){s.terminated=true;s.exit_code=status;return s;}
        if(interrupted()){s.cause="external interruption";break;}
        if(tick(clock,previous)>=end){s.cause="wall deadline";break;}clock.sleep(50000000);}
    // Recheck before SIGINT; never SIGKILL an SRAM owner.
    if(auto status=child.poll()){s.terminated=true;s.exit_code=status;return s;}
    child.interrupt();end=deadline(clock,20000000000LL);
    while(true){if(auto status=child.poll()){s.terminated=true;s.exit_code=status;return s;}
        if(tick(clock,previous)>=end) { return s; }
        clock.sleep(50000000);}
}
std::string Cleanup::json() const {
    Value v=object();v.object={{"owned_camera_closed",boolean(owned_camera_closed)},{"native_settings_restored",boolean(native_settings_restored)},{"power_restored",boolean(power_restored)},{"mcu_cleanup_verified",boolean(mcu_cleanup_verified)},{"profile_restore_deferred",boolean(deferred)},{"retry_allowed",boolean(true)}};
    Value e;e.type='[';for(auto& s:errors)e.array.push_back(str(s));
    v.object["errors"]=e;return encode(v)+"\n";
}
ProfileOwner::ProfileOwner(Files& f,Runtime& r,BoundInvocation i):fs_(f),rt_(r),inv_(std::move(i)){}
void ProfileOwner::writer(){report([&]{if(fs_.writer_exclusion)fs_.writer_exclusion();else std::cerr<<"Profile writer exclusion adapter unavailable\n";});}
void ProfileOwner::verify_active(){
    need(started_&&!cleaned_,"Profile not active");
    writer();
    report([&]{if(fs_.read(inv_.settings)!=std::optional<std::string>(expected_))std::cerr<<"Active profile settings changed; continuing\n";});
}
void ProfileOwner::apply() {
    need(!started_&&!cleaned_,"Profile already started/closed");
    started_=true;writer();
    report([&]{save(fs_,inv_,"profile-start.json","{\"rf_started\":false}\n");});
    Value data=object();
    report([&]{original_=fs_.read(inv_.settings);if(original_)data=parse(*original_);
    obj(data);});
    if(data.type!='{')data=object();
    auto& capture=data.object["captureSessionSettings"];if(capture.type!='{')capture=object();
    auto& cameras=capture.object["trackingCameras"];if(cameras.type!='{')cameras=object();
    {const char* m=std::getenv("PICO_STREAMING_MODE");int mode=m&&*m>='0'&&*m<='6'&&!m[1]?*m-'0':3;  // 2026-10-31: 4 = stock controller exposure (controller coexistence test)
    if(mode!=3)std::cerr<<"Profile streamingMode "<<mode<<" (PICO_STREAMING_MODE)\n";
    cameras.object["streamingMode"]=num(mode);}expected_=encode(data)+"\n";
    report([&]{save(fs_,inv_,"native-settings-before.json",original_.value_or("null\n"));});
    report([&]{save(fs_,inv_,"native-settings-profile3.json",expected_);});
    report([&]{
        auto output=rt_.power(inv_.power_tool,std::nullopt,8000000000LL);
        auto row=parse(output);power_ok(row);old_power_=numeric(field(row,"before"));
        save(fs_,inv_,"power-before.json",output);
    });
    report([&]{
        writer();auto output=rt_.power(inv_.power_tool,300,8000000000LL);
        auto row=parse(output);power_ok(row);
        if(!zero(row,"set_error")||numeric(field(row,"set"))!=300)std::cerr<<"Power setter failed; continuing\n";
        save(fs_,inv_,"power-set.json",output);
    });
    // 2026-10-31 owner: camera mode 2 stays permanently. A profile already in place is not rewritten and XRService is not
    // restarted: each restart moved the HMD to a new tracking world / chaperone universe and dropped every controller
    // outside the camera view until the restore (docs/POSITION_LAG_2026-10-31.md, ctrl-hidden).
    if(original_&&*original_==expected_)std::cerr<<"Profile already active; settings not rewritten, XRService not restarted\n";
    else {report([&]{writer();settings_attempted_=true;fs_.compare_replace(inv_.settings,original_,expected_);});
    restart_native(rt_);}
    profile_ready_=true;
}
void ProfileOwner::own_camera(std::function<void()> close) {need(!camera_close_&&bool(close),"Camera owner already attached/invalid");camera_close_=std::move(close);}
void ProfileOwner::launch_begin() {need(profile_ready_&&!launch_entered_,"Invalid launch state");verify_active();report([&]{save(fs_,inv_,"runner-start.json","{\"rf_started\":false}\n");});launch_entered_=true;}
void ProfileOwner::attach_child(Child& c) {need(launch_entered_&&!child_&&c.pid>0&&bool(c.poll),"Invalid child attachment");child_=c;}
void ProfileOwner::supervisor_restored(const std::string& barriers) {
    need(!cleaned_&&!child_,"Invalid supervisor restoration state");
    if(!cleanup_barriers(barriers)){std::cerr<<"Supervisor restoration barriers incomplete\n";return;}
    supervisor_verified_=true;
}
Cleanup ProfileOwner::cleanup() {
    if(cleaned_){std::cerr<<"Cleanup already attempted\n";return {};}
    cleaned_=true;profile_ready_=false;
    Cleanup out;
    out.mcu_cleanup_verified=supervisor_verified_;
    auto step=[&](const std::function<void()>& work){try{work();}catch(const std::exception& e){out.errors.push_back(e.what());std::cerr<<"Profile cleanup: "<<e.what()<<"\n";}catch(...){out.errors.push_back("Unknown cleanup failure");std::cerr<<"Unknown profile cleanup failure\n";}};
    if(child_)step([&]{if(child_->poll())out.mcu_cleanup_verified=cleanup_barriers(get(fs_,attempt(inv_,"radio-run.json")));});
    {
        step([&]{if(camera_close_){camera_close_();camera_close_={};}out.owned_camera_closed=true;});
        // PICO_PROFILE_PERSIST=1 (owner 2026-10-31, live-visual.sh default): the session profile is kept, no restore restart.
        const bool persist=[]{const char* p=std::getenv("PICO_PROFILE_PERSIST");return p&&std::string(p)=="1";}();
        if(persist&&settings_attempted_)std::cerr<<"Profile kept (PICO_PROFILE_PERSIST=1); settings not restored\n";
        step([&]{if(settings_attempted_&&!persist){writer();auto current=fs_.read(inv_.settings);need(current==std::optional<std::string>(expected_),"Concurrent settings change; overwrite refused");fs_.compare_replace(inv_.settings,expected_,original_);need(fs_.read(inv_.settings)==original_,"Settings restoration readback failed");writer();restart_native(rt_);}out.native_settings_restored=true;});
        step([&]{if(old_power_){writer();auto output=rt_.power(inv_.power_tool,*old_power_,8000000000LL);save(fs_,inv_,"power-restored.json",output);auto row=parse(output);power_ok(row);need(zero(row,"set_error"),"Power restore setter failed");output=rt_.power(inv_.power_tool,std::nullopt,8000000000LL);save(fs_,inv_,"power-restored-readback.json",output);row=parse(output);power_ok(row);need(numeric(field(row,"before"))==*old_power_,"Power restore readback mismatch");}out.power_restored=true;});
    }
    // Persist once; an IO error cannot silently turn cleanup into success.
    step([&]{fs_.exclusive(attempt(inv_,"profile-cleanup.json"),out.json());});return out;
}
} // namespace phase5



#include "../live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_profile, "profile");
