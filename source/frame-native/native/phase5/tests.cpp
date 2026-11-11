#include "../native_test_runtime.hpp"
#include <algorithm>
#include <utility>
#include "orchestrator.hpp"
#include "../live_session_owner.hpp"
#include <memory>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
#include <sstream>
using namespace phase5;
namespace {
int checks=0;
void check(bool ok,const char* label) {++checks;if(!ok)throw std::runtime_error(label);}
template<class F> void rejects(F f,const char* label) {bool caught=false;try{f();}catch(...){caught=true;}check(caught,label);}
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
        fs.resolve=[&](const std::string& p){event("resolve:"+p);auto n=norm(p);if(n==symlink_from)return symlink_to;return n;};
        fs.read=[&](const std::string& p)->std::optional<std::string>{event("read:"+p);auto it=disk.find(norm(p));if(it==disk.end())return {};return it->second;};
        fs.exclusive=[&](const std::string& p,const std::string& b){event("exclusive:"+p);if(!disk.emplace(norm(p),b).second)throw std::runtime_error("Exclusive marker exists");};
        fs.compare_replace=[&](const std::string& p,const std::optional<std::string>& expected,const std::optional<std::string>& replacement){event("replace:"+p);if(before_replace)before_replace();auto it=disk.find(norm(p));std::optional<std::string> current;if(it!=disk.end())current=it->second;if(current!=expected)throw std::runtime_error("Concurrent file change");if(replacement)disk[norm(p)]=*replacement;else disk.erase(norm(p));if(replace_after_throw)throw std::runtime_error("Write succeeded but status lost");};
        rt.boot_id=[]{return std::string("boot-A");};rt.clock.now=[&]{return now;};rt.clock.sleep=[&](int64_t ns){event("sleep");now+=ns;if(age_advances)age+=double(ns)/1e9;};
        rt.managed=[&]{event("managed");return std::vector<ManagedProcess>{{pid,age}};};
        rt.terminate_native=[&](int64_t p){event("native-SIGTERM");check(p==pid,"Native PID binding");if(restart){++pid;age=0;}};
        rt.power=[&](const std::string& tool,std::optional<double> n,int64_t timeout){event(n?"power-set":"power-read");check(tool==inv.power_tool&&timeout==8000000000LL,"Exact power tool / timeout");if(power_override)return power_override(n);auto before=power_value;if(n)power_value=*n;return std::string("{\"read_error\":0,\"set_error\":0,\"before\":")+std::to_string(before)+(n?",\"set\":"+std::to_string(*n):"")+"}";};
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
void crypto_json() {
    check(sha256("")=="e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855","SHA empty");
    check(sha256("abc")=="ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad","SHA abc");
    check(sha256(std::string(1000000,'a'))=="cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0","SHA million a");
    check(sha256("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq")=="248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1","SHA multi-block padding");
    for(auto s:{"{\"x\":1,\"x\":true}","[1,]","1e","01","NaN","true false","\"\\ud800\""})rejects([&]{parse(s);},"Malformed JSON rejection");
    auto v=parse("{\"i\":9223372036854775807,\"s\":\" \\n\\t\\\" \"}");check(integer(field(v,"i"))==INT64_MAX,"Integer precision retained");check(encode(parse(encode(v)))==encode(v),"JSON escaping roundtrip");
}
void continuation() {
    for(int fault=0;fault<8;++fault){
        Fixture f;
        if(fault==1)f.disk["/candidate/attempt/profile-start.json"]="old";
        if(fault==2)f.rt.managed=[]{return std::vector<ManagedProcess>{};};
        if(fault==3)f.rt.managed=[]{return std::vector<ManagedProcess>{{10,20},{11,20}};};
        if(fault==4)f.fs.writer_exclusion=[]{throw std::runtime_error("writer unavailable");};
        if(fault==5)f.disk[f.inv.settings]="malformed";
        if(fault==6)f.power_override=[](std::optional<double>){throw std::runtime_error("power unavailable");return std::string();};
        if(fault==7)f.restart=false;
        std::ostringstream log;auto* previous=std::cerr.rdbuf(log.rdbuf());
        ProfileOwner owner(f.fs,f.rt,f.inv);
        owner.apply();owner.verify_active();owner.launch_begin();
        owner.supervisor_restored("{}");
        auto result=owner.cleanup();
        std::cerr.rdbuf(previous);
        check(!result.deferred,"Cleanup never gated by MCU report");
        check(!result.mcu_cleanup_verified,"Missing MCU proof never fabricated");
        check(!log.str().empty(),"Missing proof/fault logged");
        if(fault!=6)check(f.power_value==120,"Power restored independently");
    }
    {Fixture f;ProfileOwner owner(f.fs,f.rt,f.inv);owner.apply();
     bool camera_closed=false;owner.own_camera([&]{camera_closed=true;throw std::runtime_error("camera close");});
     owner.launch_begin();Child child{22,[]{return std::optional<int>{};},[]{throw std::runtime_error("must never signal SRAM owner");}};
     owner.attach_child(child);auto result=owner.cleanup();
     check(camera_closed&&result.power_restored&&!result.errors.empty(),"Live child does not block independent cleanup or get signaled");}
    {Fixture f;Child child{22,[]{return std::optional<int>(0);},[]{throw std::runtime_error("Unexpected signal");}};
     check(bounded_wait(child,f.rt.clock,[]{return false;}).terminated,"Exited child no signal");}
}
}
int main(){
    try{crypto_json();continuation();std::cout<<"Phase5 "<<checks<<" checks PASS\n";return 0;}
    catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 1;}
}