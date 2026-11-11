#include "native_test_runtime.hpp"
#include "live_preflight.hpp"
#include "live_guard_inventory.hpp"
#include "live_prepared_state.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <algorithm>
using namespace phase5;
namespace {
int checks=0;
void check_result(bool value,const char* name,int line){
    ++checks;
    if(!value){
        std::cerr<<"live_preflight_tests name="<<name<<" line="<<line<<" actual=false expected=true\n";
        throw std::runtime_error("Preflight assertion failed");
    }
}
#define check(...) check_result(bool(__VA_ARGS__),#__VA_ARGS__,__LINE__)
std::string load(const std::string& path){std::ifstream in(path,std::ios::binary);if(!in)throw std::runtime_error("fixture input absent");
return {std::istreambuf_iterator<char>(in),{}};}
Value str(const std::string& value){Value result;result.type='s';result.text=value;return result;}
struct Fixture {
    std::map<std::string,std::string> disk;
    std::vector<std::string> events;
    pico::ReadOnlyPreflightHooks hooks;
    std::vector<std::string> reviewed{"/candidate/native_runner","--duration","150"};
    std::string fail;
    int writes=0,reads=0,detaches=0;
    Fixture(const std::string& stock,const std::string& module){
        disk["/candidate/stock.bin"]=stock;disk["/candidate/build/roy_station.elf"]=module;
        disk["/candidate/native_runner"]="sealed native fixture";
        disk["/native"]="native image";disk["/power"]="power image";
        disk["/candidate/attempt/station-config.json"]=R"({"diagnostic_pause_peers":[1,2],"slot_cycle":1,"tracker_poll_decimation_n":1,"omit_command19":true,"dense_frame_diagnostic":false,"compact_mailbox_batch":false,"fast_ring_snapshot":false,"interval_us":6000,"steady_interval_us":6000,"slot_us":900,"optics":{"phase_at_exposure_ticks":261700,"timed_windows":false,"optical_method":"calibrated_stereo_v1","imu_calibration_method":"sparse_observations_v1"},"coexistence":{"controllers_off":true,"min_controllers":0}})";
        disk["/candidate/attempt/registry.json"]=R"({"peers":[{"slot":0,"chip_id_le":"d4c3b2a1","product_sn":"PCTESTTRACKER0001"}]})";
        auto invocation=parse(R"({"launch_admitted":true,"preparation_only":false,"working_directory":"/candidate","target":{"serial":"PCTESTTRACKER0001","chip_id_le":"d4c3b2a1","slot":0},"required_tracker_image_sha256":"040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d","files":{},"external_files":{},"external_paths":{"native":"/native","settings":"/settings","power_tool":"/power"},"command":["/candidate/native_runner","--duration","150"]})");
        for(const auto* name:{"stock.bin","build/roy_station.elf","native_runner","attempt/station-config.json","attempt/registry.json"}){
            const auto& value=disk["/candidate/"+std::string(name)];
            invocation.object["files"].object[name]=parse("{\"bytes\":"+std::to_string(value.size())+",\"sha256\":\""+phase5::sha256(value)+"\"}");
        }
        for(const auto* path:{"/native","/power"})invocation.object["external_files"].object[path]=str(phase5::sha256(disk[path]));
        invocation.object["native_xrservice_sha256"]=str(phase5::sha256(disk["/native"]));
        disk["/invocation"]=encode(invocation);const auto seal=phase5::sha256(disk["/invocation"]);disk["/anchor"]=seal;
        disk["/candidate/attempt/human-readiness.json"]="{\"schema\":\"native-bench-readiness-v1\",\"human_answer\":\"Explicit synthetic test readiness only\",\"invocation_sha256\":\""+seal+"\",\"boot_id\":\"boot\",\"confirmed_monotonic_ns\":100,\"serial\":\"PCTESTTRACKER0001\",\"hmd_tracked\":true,\"hmd_worn\":true,\"controllers_off\":true,\"tracker_supported\":true}";
        hooks.files.resolve=[](const std::string& path){return path;};
        hooks.files.read=[this](const std::string& path)->std::optional<std::string>{auto item=disk.find(path);if(item==disk.end())return {};
        return item->second;};
        hooks.files.exclusive=[this](const std::string&,const std::string&){++writes;throw std::runtime_error("Unexpected file write");};
        hooks.files.compare_replace=[this](const std::string&,const std::optional<std::string>&,const std::optional<std::string>&){++writes;throw std::runtime_error("Unexpected profile mutation");};
        hooks.boot=[] {return std::string("boot");};hooks.now=[] {return int64_t(101);};
        hooks.prepare=[this](const BoundInvocation& bound){
            event("prepare");
            check(bound.invocation_bytes==disk.at("/invocation"));
            check(bound.sealed_files.at("stock.bin")==disk.at("/candidate/stock.bin"));
        };
        hooks.ownership=[this]{event("ownership");};hooks.tracking=[this]{event("tracking");};
        hooks.profile=[this](const BoundInvocation&){event("profile");};
        hooks.attach=[this]{event("attach");};hooks.detach=[this]{++detaches;event("detach");};
        hooks.read=[this,stock](uint32_t address,size_t count){++reads;event("read");
            if(uint64_t(address)+count<=stock.size())return pico::Bytes(stock.begin()+address,stock.begin()+address+count);
            if(address==pico::roy_rtt)return pico::Bytes{'S','E','G','G','E','R',' ','R','T','T'};
            if(address==pico::roy_rtt+16)return pico::Bytes{3,0,0,0,3,0,0,0};
            if(address==pico::roy_static)return pico::Bytes{0,0,2,0};
            return pico::Bytes(count);
        };
        hooks.camera=[this](const BoundInvocation&,pico::CameraAttemptJournal&){event("camera");};
    }
    void event(const std::string& phase){events.push_back(phase);if(fail==phase)throw std::runtime_error("Injected "+phase);}
    void run(){(void)pico::read_only_live_preflight(hooks,"/candidate","/invocation","/anchor",reviewed);}
};
}
int main(int argc,char** argv){try{
    if(argc!=3)throw std::runtime_error("Usage: live_preflight_tests STOCK MODULE");
    const auto stock=load(argv[1]),module=load(argv[2]);
    for(const std::string fault:{"","prepare","ownership","tracking","profile","attach","read","camera","detach"}){
        Fixture fixture(stock,module);fixture.fail=fault;
        std::vector<std::string> reports;
        fixture.hooks.report=[&](const std::string& name,const std::string& result){reports.push_back(name+" "+result);};
        fixture.run();
        check(fixture.writes==0&&fixture.detaches==1);
        check(std::find(fixture.events.begin(),fixture.events.end(),"camera")!=fixture.events.end());
        if(!fault.empty())check(std::any_of(reports.begin(),reports.end(),[&](const std::string& row){return row.find("WARN: Injected "+fault)!=std::string::npos;}));
    }
    {Fixture fixture(stock,module);
     fixture.disk.erase("/candidate/attempt/human-readiness.json");
     fixture.disk["/candidate/attempt/profile-start.json"]="old finished attempt";
     fixture.disk["/anchor"]="mismatched digest";
     fixture.run();check(fixture.detaches==1&&fixture.writes==0);}
    {BoundInvocation bound;
     std::ostringstream log;auto* previous=std::cerr.rdbuf(log.rdbuf());
     auto state=pico::live_prepared_state(bound);std::cerr.rdbuf(previous);
     check(state.size()==2048&&!log.str().empty());
     check(pico::word(state,0)==0x314d5250&&pico::word(state,1856)==0);
     bound.sealed_files["attempt/station-config.json"]=R"({"seed_le":"01020304","service_poll":true,"slot_end_margin_us":999})";
     bound.sealed_files["bootstrap214.profile.json"]=R"({"frequency":1,"mode":4})";
     state=pico::live_prepared_state(bound);
     check(pico::word(state,44)==1&&pico::word(state,48)==4);
     check(pico::word(state,1856)==0x04030201&&pico::word(state,1892)==700&&pico::word(state,1896)==1);
     bound.sealed_files["attempt/station-config.json"]=R"({"seed_le":"not-hex","service_poll":"wrong"})";
     check(pico::live_prepared_state(bound).size()==2048);}
    for(int fault=0;fault<4;++fault){
        std::vector<std::string> order;
        auto action=[&](const char* name,int index){order.push_back(name);if(fault==index)throw std::runtime_error(name);};
        pico::close_read_only_resources([&]{action("debugger",1);},[&]{action("observer",2);},[&]{action("release",3);});
        check((order==std::vector<std::string>{"debugger","observer","release"}));
    }
    pico::require_live_validation();
    std::cout<<"Preflight continuation "<<checks<<" checks PASS\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 1;}}