#include "native_test_runtime.hpp"
#include "live_session.hpp"
#include "camera_preflight_tests.hpp"
#include "live_session_owner.hpp"
#include "phase5/orchestrator.hpp"
#include "live_guard_inventory.hpp"
#include "listener_attribution.hpp"
#include "fake_openocd.hpp"
#include "live_pixel_pairs.hpp"
#include "live_query_evidence.hpp"
#include "fixture_tracking_stream.hpp"
#include "live_fixture_enforcement.hpp"
#include "live_entry_parent_refusal.hpp"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace pico;
namespace {
int checks=0;
std::string context;
struct CheckFailure:std::runtime_error{using std::runtime_error::runtime_error;};
void result(bool ok,const char* name,int line,const std::string& actual,const std::string& expected){
    ++checks;if(!ok)throw CheckFailure("live session check "+std::to_string(checks)+" ["+name+"]"+context+
        " line="+std::to_string(line)+" actual="+actual+" expected="+expected);
}
void check_bool(bool actual,const char* name,int line){result(actual,name,line,actual?"true":"false","true");}
template<class F>void checked(F f,const char* name,int line){
    try{check_bool(bool(f()),name,line);}catch(const CheckFailure&){throw;}
    catch(const std::exception& e){result(false,name,line,std::string("exception: ")+e.what(),"true");}
}
template<class F>void rejection(F f,const char* name,int line){
    bool bad=false;std::string actual="no exception";
    try{f();}catch(const CheckFailure&){throw;}
    catch(const std::exception& e){bad=true;actual=std::string("std::exception: ")+e.what();}
    result(bad,name,line,actual,"std::exception");
}
#define check(...) checked([&]{return (__VA_ARGS__);},#__VA_ARGS__,__LINE__)
#define rejects(...) rejection(__VA_ARGS__,#__VA_ARGS__,__LINE__)
struct Fixture {
    int64_t now=1000000000;
    bool alive=true,ready=false,rows=true,controller=false,hmd=true,worker_failed=false,camera_failed=false;
    bool parent=true,cancel=false;
    bool malformed=false,roy_failed=false,exhaust_in_camera=false;
    int camera_calls=0,roy_calls=0,publication_calls=0;
    int seal_calls=0,owner_calls=0,listener_calls=0;
    bool seal_bad=false,listener_bad=false,profile_bad=false;
    int listener_fault=0;
    SessionOwnership ownership{"/home/steamos/.local/state/frame-pico/roy.lock","inactive",1,2,3,10,11,3,true,true,{10,11}};
    std::vector<std::string> order;
    std::optional<int64_t> last;
    OpticalPhaseVerification phase{261700,{0xA1B2C3D4},true};
    std::unique_ptr<LiveSessionSafety> storage;
    LiveSessionSafety& session;
    LiveSessionHooks hooks(){return {[&]{return now;},[&](phase5::BenchTrackingGuard& g){
        if(malformed){g.ingest("{}");return;}
        if(!rows||(last&&*last==now))return;
        g.ingest("{\"t_ns\":"+std::to_string(now)+",\"devices\":[{\"index\":0,\"kind\":1,\"connected\":true,\"valid\":"+(hmd?"true":"false")+",\"result\":200,\"matrix\":[1,0,0,0,0,1,0,0,0,0,1,0]}"+(controller?",{\"index\":1,\"kind\":2,\"connected\":true,\"valid\":false,\"result\":0,\"matrix\":[1,0,0,0,0,1,0,0,0,0,1,0]}":"")+"]}");last=now;
    },[&]{return alive;},[&]{if(worker_failed)throw std::runtime_error("worker exited");},[&]{return ready;},[&]{++camera_calls;if(camera_failed)throw std::runtime_error("OCC no growth >2s");
    if(exhaust_in_camera){phase.ack(0xA1B2C3D4,261700);for(int64_t i=900;i<=960;++i)phase.observe(i,false,false);}},[&](bool b){check(b);++roy_calls;if(roy_failed)throw std::runtime_error("Roy clock acquisition >8s without fit");},[&]{++publication_calls;},[&]{return parent;},[&]{return cancel;},
    [&]{order.push_back("seals");++seal_calls;if(seal_bad)throw std::runtime_error("Seal/argv changed");},
    [&]{order.push_back("owner");++owner_calls;return ownership;},
    [&]{order.push_back("listener");++listener_calls;if(listener_bad)throw std::runtime_error("Listener attribution lost");
        ListenerIdentity launch;launch.child=11;launch.child_start=3;
        launch.argv={"synthetic-openocd","-f","interface","-f","target"};
        launch.executable_seal="synthetic executable";launch.dependency_seals={"interface","target"};
        auto e=launch;e.owner=11;e.observed_start=3;e.socket_inode=4;e.address="127.0.0.1";e.port=6666;e.matching_listeners=1;
        if(listener_fault==1)++e.owner;
        if(listener_fault==2)++e.observed_start;
        if(listener_fault==3)e.socket_inode=0;
        if(listener_fault==4)e.matching_listeners=2;
        if(listener_fault==5)e.address="0.0.0.0";
        if(listener_fault==6)e.port=6667;
        if(listener_fault==7)e.argv.push_back("unreviewed");
        if(listener_fault==8)e.executable_seal="changed";
        if(listener_fault==9)e.dependency_seals[0]="changed";
        verify_listener_identity(e,launch);
    },[&]{if(profile_bad)throw std::runtime_error("Profile writer exclusion lost");}};}
    Fixture(int replay_seconds=0):storage(replay_seconds?std::make_unique<LiveSessionSafety>(hooks(),phase,ReplaySessionLimit{replay_seconds}):std::make_unique<LiveSessionSafety>(hooks(),phase)),session(*storage){}
};
}
int main(){try{
    // Every recoverable hardware observation logs and leaves dispatch usable.
    for(int fault=0;fault<12;++fault){
        context=" fault="+std::to_string(fault);Fixture f;f.session.admit();f.ready=true;++f.now;
        std::ostringstream logged;auto* old=std::cerr.rdbuf(logged.rdbuf());
        if(fault==0)f.seal_bad=true;
        if(fault==1)f.listener_bad=true;
        if(fault==2)f.profile_bad=true;
        if(fault==3)f.worker_failed=true;
        if(fault==4)f.camera_failed=true;
        if(fault==5)f.roy_failed=true;
        if(fault==6)f.alive=false;
        if(fault==7)f.malformed=true;
        if(fault==8)f.controller=true;
        if(fault==9)f.hmd=false;
        if(fault==10){f.rows=false;f.now+=1000000000;}
        if(fault==11)f.ownership.lock_inode++;
        if(fault==0)f.session.before_sram();else f.session.poll();
        std::cerr.rdbuf(old);
        check(!logged.str().empty());
        f.seal_bad=f.listener_bad=f.profile_bad=f.worker_failed=f.camera_failed=f.roy_failed=false;
        f.alive=f.rows=f.hmd=true;f.malformed=f.controller=false;
        if(fault==11)f.ownership.lock_inode--;
        ++f.now;f.session.poll();
        check(f.camera_calls>=1&&f.roy_calls>=1);
    }
    for(int fault=0;fault<5;++fault){
        Fixture f;
        if(fault==0)f.rows=false;
        if(fault==1)f.alive=false;
        if(fault==2)f.seal_bad=true;
        if(fault==3)f.worker_failed=true;
        if(fault==4)f.ownership.service_state="active";
        f.session.admit(); // readiness is a report, never an admission gate
        f.rows=f.alive=true;f.seal_bad=f.worker_failed=false;
        f.ownership.service_state="inactive";++f.now;
        f.ready=true;f.session.poll();check(f.camera_calls==1);
    }
    {
        Fixture f;f.session.admit();f.ready=true;f.exhaust_in_camera=true;
        std::ostringstream logged;auto* old=std::cerr.rdbuf(logged.rdbuf());
        // A failed 61-frame identity window is logged and retried; it never ends polling.
        f.session.poll();f.session.poll();std::cerr.rdbuf(old);
        check(logged.str().find("identity_phase_window_retry")!=std::string::npos);
        check(logged.str().find("exhausted")==std::string::npos);
        check(f.phase.state()==IdentityPhaseWindow::State::Verifying);
        check(f.camera_calls==2&&f.roy_calls==2);
    }
    {Fixture f;f.session.admit();f.cancel=true;rejects([&]{f.session.guard();});}
    {Fixture f;f.session.admit();f.parent=false;rejects([&]{f.session.guard();});}
    {Fixture f;f.session.admit();f.now+=180000000000LL;rejects([&]{f.session.guard();});}
    {Fixture f;rejects([&]{LiveSessionSafety missing({},f.phase);});}
    {Fixture f;rejects([&]{f.session.guard();});}
    {Fixture f;f.session.admit();rejects([&]{f.session.admit();});}
    {Fixture f;rejects([&]{LiveSessionSafety invalid(f.hooks(),f.phase,ReplaySessionLimit{0});});}
    {
        bool installed=false,entered=false;
        preflight_then_enter([]{throw std::runtime_error("camera proof unavailable");},
            [&]{installed=true;},[&]{entered=true;});
        check(installed&&entered);
        bool camera_closed=false;
        stop_camera_worker([]{throw std::runtime_error("worker close failed");},[&]{camera_closed=true;});
        check(camera_closed);
    }
    {
        auto read=[](const std::filesystem::path& path){
            std::ifstream input(path,std::ios::binary);
            if(!input)throw std::runtime_error("Fixture missing: "+path.string());
            return Bytes(std::istreambuf_iterator<char>(input),{});
        };
        const auto candidate=std::filesystem::path(__FILE__).parent_path()/".."/"candidate";
        const auto stock=read(candidate/"stock.bin"),elf=read(candidate/"build"/"roy_station.elf");
        FakeOpenOcd target;target.stock(stock);
        DebugMemory debug([&](const std::string& c){return target.command(c);});
        SramOwner ram(target.io(debug),[](const Checkpoint&){});
        Fixture f;f.session.admit();
        Bytes prepared(2048);auto magic=le32(0x314d5250);std::copy(magic.begin(),magic.end(),prepared.begin());
        f.session.install_sram(ram,stock,elf,prepared);f.session.enter_sram();
        target.close_stuck=true;
        auto incomplete=f.session.restore_sram();
        check(!incomplete.complete()&&ram.unresolved()&&!ram.may_release_owner());
        target.close_stuck=false;
        check(f.session.restore_sram().complete()&&ram.may_release_owner());
    }
    require_live_validation();
    std::cout<<"live session "<<checks<<" checks PASS\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}