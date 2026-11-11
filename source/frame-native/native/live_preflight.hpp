#pragma once
#include "phase5/orchestrator.hpp"
#include "image_admission.hpp"
#include <exception>
#include <utility>
#include <random>
#include <iostream>

namespace pico {
// Seals remain a diagnostic. Load current inputs when their historical seal no
// longer matches; missing inputs are reported individually.
inline phase5::BoundInvocation load_live_invocation(phase5::Files& files,
    const std::string& root,const std::string& invocation,const std::string& anchor,
    const std::vector<std::string>& command) {
    try{return phase5::verify_seals(files,root,invocation,anchor,command);}
    catch(const std::exception& e){std::cerr<<e.what()<<"; seal report only, loading current inputs\n";}
    phase5::BoundInvocation out;
    out.root=files.resolve(root);out.command=command;
    out.invocation_bytes=files.read(invocation).value_or("{}");
    out.sha=phase5::sha256(out.invocation_bytes);
    const auto value=phase5::parse(out.invocation_bytes);
    auto paths=value.object.find("external_paths");
    if(paths!=value.object.end()){
        auto path=[&](const char* key){auto it=paths->second.object.find(key);return it==paths->second.object.end()?std::string{}:it->second.text;};
        out.native=path("native");out.power_tool=path("power_tool");out.settings=path("settings");
    }
    auto native_sha=value.object.find("native_xrservice_sha256");
    if(native_sha!=value.object.end())out.native_sha=native_sha->second.text;
    const auto inputs=value.object.find("files");
    if(inputs!=value.object.end())for(const auto& input:inputs->second.object){
        try{auto bytes=files.read(out.root+"/"+input.first);
            if(bytes)out.sealed_files[input.first]=*bytes;
            else std::cerr<<"Live input missing: "<<input.first<<"; continuing\n";
        }catch(const std::exception& e){std::cerr<<e.what()<<"; skipping unavailable input\n";}
    }
    return out;
}
// Invocation-local receipts, populated only after an exclusive write succeeds.
// Never reconstruct these from files discovered after the camera transaction.
class CameraAttemptJournal {
    std::string root_, invocation_, run_;
    std::map<std::string,std::string> writes_;
public:
    explicit CameraAttemptJournal(const phase5::BoundInvocation& bound):root_(bound.root),invocation_(bound.sha){
        std::random_device entropy;
        for(int i=0;i<8;++i)run_+=std::to_string(entropy())+":";
    }
    phase5::Files writer(phase5::Files files) {
        const auto exclusive=files.exclusive;
        files.exclusive=[this,exclusive](const std::string& path,const std::string& bytes){
            if(path.compare(0,root_.size()+9,root_+"/attempt/")!=0 || writes_.count(path))
                throw std::runtime_error("Camera attempt journal path/repeated write refused");
            auto written=bytes;
            if(path==root_+"/attempt/profile-start.json") {
                auto marker=phase5::parse(bytes);
                if(marker.type!='{' || phase5::flag(marker,"rf_started"))
                    throw std::runtime_error("Camera profile marker must precede RF");
                phase5::Value id;id.type='s';id.text=run_;
                marker.object["camera_preflight_run_id"]=id;
                id.text=invocation_;marker.object["invocation_sha256"]=id;
                written=phase5::encode(marker)+"\n";
            }
            exclusive(path,written);
            writes_.emplace(path,written);
        };
        return files;
    }
    phase5::Files readiness_files(phase5::Files files,const phase5::BoundInvocation& bound) const {
        if(bound.root!=root_||bound.sha!=invocation_)
            throw std::runtime_error("Camera attempt journal invocation mismatch");
        for(const auto& item:writes_)
            if(files.read(item.first)!=std::optional<std::string>(item.second))
                throw std::runtime_error("Camera attempt journal file changed/missing");
        const auto read=files.read;
        const auto marker=root_+"/attempt/profile-start.json";
        const auto owned=writes_.find(marker);
        if(owned!=writes_.end()) {
            const auto expected=owned->second;
            files.read=[read,marker,expected](const std::string& path)->std::optional<std::string>{
                const auto bytes=read(path);
                if(path!=marker)return bytes;
                if(bytes!=std::optional<std::string>(expected))
                    throw std::runtime_error("Camera consumption marker changed/missing");
                return {};
            };
        }
        return files;
    }
};
// Capability boundary: there is no memory write, arbitrary Tcl, RF dispatch or
// MCU invocation callback. In explicit camera mode only, camera owns a separate
// ProfileOwner transaction which must restore before debugger construction.
struct ReadOnlyPreflightHooks {
    phase5::Files files;
    std::function<std::string()> boot;
    std::function<int64_t()> now;
    std::function<void(const phase5::BoundInvocation&)> prepare;
    std::function<void()> ownership, tracking;
    std::function<void(const phase5::BoundInvocation&)> profile;
    std::function<void()> attach, detach;
    std::function<Bytes(uint32_t,size_t)> read;
    std::function<void(const phase5::BoundInvocation&,CameraAttemptJournal&)> camera;
    std::function<void(const std::string&,const std::string&)> report;
};
struct ReadOnlyAdapterPlan {
    phase5::Value invocation;
    std::string openocd,watcher_sha;
};
// This applies only to a debugger which has never installed SRAM. Independent
// observer cleanup must run even when debugger shutdown/reaping fails.
inline void close_read_only_resources(const std::function<void()>& debugger,
    const std::function<void()>& observer,const std::function<void()>& release){
    std::string errors;
    auto attempt=[&](const char* name,const std::function<void()>& operation){
        try{
            operation();
            return true;
        }catch(const std::exception& error){
            errors+=std::string(name)+": "+error.what()+"; ";
        }catch(...){
            errors+=std::string(name)+": unknown cleanup failure; ";
        }
        return false;
    };
    (void)attempt("debugger",debugger);
    (void)attempt("observer",observer);
    (void)attempt("Roy lock",release); // read-only preflight never owns SRAM
    if(!errors.empty())std::cerr<<"Read-only preflight cleanup unresolved: "<<errors<<"; continuing\n";
}
inline ReadOnlyAdapterPlan read_only_adapter_plan(const phase5::BoundInvocation& bound,
    const std::string& debugger_config="/opt/steamvr/drivers/frame_hmd/resources/firmware/roy/openocd.cfg"){
    ReadOnlyAdapterPlan plan;
    plan.invocation=phase5::parse(bound.invocation_bytes);
    const auto& paths=phase5::field(plan.invocation,"external_paths");
    const auto& external=phase5::field(plan.invocation,"external_files");
    auto path_string=[](const phase5::Value& value){
        if(value.type!='s'||value.text.empty()||value.text.front()!='/'||
           value.text.find('\0')!=std::string::npos||value.text.find('\\')!=std::string::npos)
            throw std::runtime_error("Absolute POSIX adapter path required");
        return value.text;
    };
    plan.openocd=path_string(phase5::field(paths,"openocd"));
    auto sealed=[&](const std::string& path){
        const auto it=external.object.find(path);
        if(it==external.object.end()||it->second.type!='s'||it->second.text.size()!=64)
            std::cerr<<"Adapter external seal missing: "<<path<<"; continuing\n";
    };
    sealed(plan.openocd);
    sealed(debugger_config);
    try{
        const auto& watcher=phase5::field(phase5::field(plan.invocation,"files"),"release/tracking_watch");
        const auto& hash=phase5::field(watcher,"sha256");
        if(hash.type!='s'||hash.text.size()!=64)std::cerr<<"Watcher seal missing; continuing\n";
        else plan.watcher_sha=hash.text;
    }catch(const std::exception& e){std::cerr<<e.what()<<"; watcher seal report only\n";}
    const auto& config=bound.sealed_files.at("attempt/station-config.json");
    const auto parsed=phase5::parse(config);
    const auto& optics=phase5::field(parsed,"optics");
    for(const auto* name:{"factory_camera_file","factory_led_file"})
        sealed(path_string(phase5::field(optics,name)));
    (void)path_string(phase5::field(optics,"dataset_root"));
    return plan;
}
inline Checkpoint read_only_checkpoint(const std::function<Bytes(uint32_t,size_t)>& read,
                                      const Bytes& stock) {
    if(!read||stock.size()<0x189be + 64)throw std::runtime_error("Read-only stock/checkpoint dependency missing");
    auto exact=[&](uint32_t address,size_t size){
        auto value=read(address,size);
        if(value.size()!=size)throw std::runtime_error("Incomplete read-only SRAM checkpoint");
        return value;
    };
    for(size_t offset:{size_t(0),size_t(0x10580),size_t(0x18948),size_t(0x189be),stock.size()-64})
        if(!std::getenv("ROY_LAYOUT_REPORT") && exact(uint32_t(offset),64)!=Bytes(stock.begin()+offset,stock.begin()+offset+64))
            throw std::runtime_error("Live firmware anchor differs from stock");
    if(exact(roy_mpsl,48)[0x26])throw std::runtime_error("MPSL session is in use; cannot reserve its only slot");
    if(exact(roy_hook,4)!=Bytes(4))throw std::runtime_error("Native shell hook already owned; refusing SRAM mutation");
    if(exact(roy_rtt,10)!=Bytes{'S','E','G','G','E','R',' ','R','T','T'})throw std::runtime_error("RTT ABI mismatch");
    Checkpoint result;
    result.counts=exact(roy_rtt+16,8);
    if(result.counts!=Bytes{3,0,0,0,3,0,0,0})throw std::runtime_error("Manufacturing RTT layout differs");
    result.code=exact(roy_code,roy_code_size&~3u);result.data=exact(roy_data,roy_data_size);
    result.static_word=exact(roy_static,4);
    if(result.static_word[2]!=2)throw std::runtime_error("Unexpected Zephyr static MPU count");
    result.dwt=exact(0xe0001000,4);result.demcr=exact(0xe000edfc,4);
    // Do not enumerate MPU regions: doing so writes MPU_RNR and halts the MCU.
    // This is a read-only observation, never an install/recovery checkpoint.
    return result;
}
inline std::string read_only_live_preflight(ReadOnlyPreflightHooks hooks,
    const std::string& root,const std::string& invocation,const std::string& anchor,
    const std::vector<std::string>& reviewed,bool skip_camera_preflight=false,bool active_camera_preflight=false) {
    if(skip_camera_preflight&&active_camera_preflight)
        throw std::runtime_error("Camera preflight and camera skip are mutually exclusive");
    if(!hooks.boot||!hooks.now||!hooks.prepare||!hooks.ownership||!hooks.tracking||!hooks.profile||
       !hooks.attach||!hooks.detach||!hooks.read||!hooks.camera)
        throw std::runtime_error("Read-only preflight adapters incomplete");
    // Refuse any accidental route to file mutation even in an injected adapter.
    hooks.files.exclusive=[](const std::string&,const std::string&){throw std::runtime_error("Preflight file mutation forbidden");};
    hooks.files.compare_replace=[](const std::string&,const std::optional<std::string>&,const std::optional<std::string>&){throw std::runtime_error("Preflight profile mutation forbidden");};
    auto step=[&](const char* name,auto action) {
        if(hooks.report)hooks.report(name,"START");
        try { action(); if(hooks.report)hooks.report(name,"PASS"); }
        catch(const std::exception& error){if(hooks.report)hooks.report(name,std::string("WARN: ")+error.what());else std::cerr<<error.what()<<"; preflight report only\n";}
    };
    phase5::BoundInvocation bound;
    step("1 seals",[&]{bound=load_live_invocation(hooks.files,root,invocation,anchor,reviewed);});
    step("2 readiness record",[&]{phase5::verify_readiness(hooks.files,bound,hooks.boot(),hooks.now());});
    step("3 sealed read-only adapters",[&]{hooks.prepare(bound);});
    step("4 Roy owner/service",[&]{hooks.ownership();});
    step("5 HMD/controllers observer",[&]{hooks.tracking();});
    step("6 profile/power read",[&]{hooks.profile(bound);});
    auto bytes=[&](const std::string& path){
        const auto value=bound.sealed_files.find(path);
        if(value==bound.sealed_files.end())throw std::runtime_error("Read-only preflight input unsealed: "+path);
        return Bytes(value->second.begin(),value->second.end());
    };
    const auto stock=bytes("stock.bin"),module=bytes("build/roy_station.elf");
    step("6b protected stock/module",[&]{(void)admit_original210(stock,module);});
    // Camera ownership is fully closed/restored before a debugger is launched.
    CameraAttemptJournal camera_journal(bound);
    if(active_camera_preflight)
        step("6c camera profile transaction",[&]{hooks.camera(bound,camera_journal);});
    bool attached=false;
    try {
        // Mark before attach: a failed partial launch must still be reaped.
        attached=true;step("7 OpenOCD read-only attach",[&]{hooks.attach();});
        Checkpoint checkpoint;
        step("8 read-only SRAM checkpoint",[&]{checkpoint=read_only_checkpoint(hooks.read,stock);});
        step("tracking/ownership report",[&]{hooks.tracking();hooks.ownership();});
        if(skip_camera_preflight) {
            if(hooks.report)for(const auto* name:{"9 camera preflight read","9 recording preflight read","9 camera-clock preflight read"})
                hooks.report(name,"NOT VALIDATED (skipped by operator)");
        } else if(!active_camera_preflight) step("9 camera-clock preflight read",[&]{hooks.camera(bound,camera_journal);});
        step("9b final seals/readiness/ownership",[&]{
            const auto current=phase5::verify_seals(hooks.files,root,invocation,anchor,reviewed);
            if(current.sha!=bound.sha)throw std::runtime_error("Invocation changed during read-only preflight");
            // Only this camera transaction's exact exclusive writes may explain
            // profile-start. Runner/radio/child markers remain unconditional refusals.
            auto readiness_files=active_camera_preflight ? camera_journal.readiness_files(hooks.files,bound) : hooks.files;
            phase5::verify_readiness(readiness_files,bound,hooks.boot(),hooks.now());
            step("tracking/ownership report",[&]{hooks.tracking();hooks.ownership();});
        });
        step("10 OpenOCD detach",[&]{hooks.detach();});attached=false;
        return checkpoint_json(checkpoint);
    } catch(...) {
        auto error=std::current_exception();
        if(attached)hooks.detach();
        std::rethrow_exception(error);
    }
}
}

#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_bench_only, "bench-only");
