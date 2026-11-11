#include "guard_evidence.hpp"
#include "openocd_process.hpp"
#include <stdexcept>
#include <iostream>
namespace pico {
std::vector<std::string> openocd_argv(){return {"openocd","-f",
    "/opt/steamvr/drivers/frame_hmd/resources/firmware/roy/openocd.cfg","-c",
    "bindto 127.0.0.1; gdb_port disabled; telnet_port disabled; tcl_port 6666; init; poll; poll off"};}
OpenOcdProcess::OpenOcdProcess(OpenOcdHooks h):h_(std::move(h)){
    if(!h_.now||!h_.sleep||!h_.listener_present||!h_.spawn||!h_.poll||!h_.connect||!h_.command||!h_.disconnect||!h_.terminate||!h_.kill)
        throw std::runtime_error("Incomplete OpenOCD hooks");
}
bool OpenOcdProcess::wait(int64_t ns){auto end=h_.now()+ns;do{if(h_.poll())return true;
h_.sleep(5000000);}while(h_.now()<end);
return bool(h_.poll());}
void OpenOcdProcess::reap_failed_start(){
    if(sram_unresolved_)throw std::runtime_error("OpenOCD retained: SRAM ownership unresolved");
    if(connected_){h_.disconnect();connected_=false;}
    if(spawned_&&!h_.poll()){h_.terminate();if(!wait(2000000000LL)){h_.kill();if(!wait(2000000000LL))std::cerr<<"OpenOCD reap unresolved; leaving process alive\n";}}
    closed_=true;
}
void OpenOcdProcess::start(){
    if(spawned_||closed_)throw std::runtime_error("OpenOCD already started/closed");
    try {
    if(!h_.verify_attribution)std::cerr<<"OpenOCD attribution adapter missing; continuing\n";
    if(h_.listener_present())std::cerr<<"OpenOCD listener already present; continuing with available state\n";
    h_.spawn(openocd_argv());spawned_=true;
        auto end=h_.now()+8000000000LL;
        while(h_.now()<end){
            if(h_.poll()){std::cerr<<"OpenOCD exited; inspect log; continuing without connection\n";return;}
            if(h_.connect()){
                connected_=true;
                if(h_.poll()){
                    std::cerr<<"OpenOCD exited during attach\n";
                }
                if(h_.verify_attribution){
                    try{h_.verify_attribution();}catch(const std::exception& e){std::cerr<<e.what()<<"; attribution report only\n";}
                }
                return;
            }
            h_.sleep(50000000);
        }
        std::cerr<<"OpenOCD TCL connection timeout; continuing without connection\n";
    }catch(const std::exception& e){std::cerr<<e.what()<<"; debugger startup report only\n";}
}
std::string OpenOcdProcess::command(const std::string& c){
    if(!connected_||closed_){std::cerr<<"OpenOCD not connected; skipping command\n";return {};}
    try{if(h_.verify_attribution)h_.verify_attribution();}catch(const std::exception& e){std::cerr<<e.what()<<"; attribution report only\n";}
    try{return h_.command(c);}catch(const std::exception& e){std::cerr<<e.what()<<"; command unavailable\n";return {};}
}
void OpenOcdProcess::restored(bool session,bool ram,bool mpu,bool trace){
    if(!session||!ram||!mpu||!trace){std::cerr<<"MCU restoration barriers incomplete; retaining SRAM owner\n";return;}
    sram_unresolved_=false;
}
void OpenOcdProcess::close(){
    if(closed_)return;
    pico::guard_evidence::emit(29,"retain-owner",!sram_unresolved_);
    if(sram_unresolved_)throw std::runtime_error("OpenOCD retained: SRAM ownership unresolved");
    try{
    if(!spawned_){closed_=true;return;}
    if(connected_){try{if(h_.verify_attribution)h_.verify_attribution();
    (void)h_.command("shutdown");}catch(const std::exception& e){std::cerr<<"OpenOCD shutdown: "<<e.what()<<"\n";}h_.disconnect();connected_=false;}
    if(!wait(2000000000LL)){h_.terminate();if(!wait(2000000000LL)){h_.kill();if(!wait(2000000000LL))std::cerr<<"OpenOCD reap unresolved; leaving process alive\n";}}
    closed_=true;
    }catch(const std::exception& e){std::cerr<<"OpenOCD cleanup: "<<e.what()<<"; continuing without killing uncertain owner\n";}
}
}
