#pragma once
#include "tracker_ota_radio.hpp"
#include "ota_service_state.hpp"
#include <cstdlib>
#include <chrono>
#include <cerrno>
#include <csignal>
#include <iostream>
#include <iterator>
#include <random>
#include <thread>
#ifndef _WIN32
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <poll.h>
#include "observer_process.hpp"
#endif
namespace pico {
inline volatile std::sig_atomic_t ota_cancelled=0;
inline void ota_signal(int) { ota_cancelled=1; }
inline Bytes ota_file(const std::string &path) {
    std::ifstream in(path,std::ios::binary);
    if (!in) throw std::runtime_error("Cannot read OTA input: "+path);
    return Bytes((std::istreambuf_iterator<char>(in)),{});
}
// Explicit separate executable entry. Nothing in live-visual*.sh calls this.
inline int ota_live_entry(int argc,char **argv) {
#ifdef _WIN32
    (void)argc; (void)argv;
    throw std::runtime_error("Live OTA requires the Frame Linux owner");
#else
    std::map<std::string,std::string> args;
    for(int i=1;i<argc;i+=2) {
        if(i+1>=argc || !args.emplace(argv[i],argv[i+1]).second)
            throw std::runtime_error("OTA requires unique option/value pairs");
    }
    for(const auto &a:args) if(a.first!="--ota" && a.first!="--chip" && a.first!="--allowlist" &&
        a.first!="--module" && a.first!="--stock" && a.first!="--state" &&
        a.first!="--port" && a.first!="--checkpoint" && a.first!="--watch" &&
        a.first!="--watch-sha" && a.first!="--proof" && a.first!="--stock-approval" &&
        a.first!="--approval" && a.first!="--serial") throw std::runtime_error("Unknown OTA option");
    auto required=[&](const char *key)->std::string {
        const auto it=args.find(key); if(it==args.end()) throw std::runtime_error(std::string("Required ")+key);
        return it->second;
    };
    const auto image=ota_file(required("--ota")), module=ota_file(required("--module")),
               stock=ota_file(required("--stock")), state=ota_file(required("--state"));
    const auto chip=unhex(required("--chip")); // explicit little-endian wire bytes
    const auto checkpoint=required("--checkpoint");
    if(sha256(stock)!="0375ab90c91fa0bce1522f8f510cabfca6667cceadce1e4737b73b82507edd50" &&
       !(roy_layout_report().ok && sha256(stock)==roy_layout_report().flash_sha))
        throw std::runtime_error("Wrong Roy stock image");
    const auto parsed=elf_module(module);
    require_ota_target_state(state,chip);
    if(!OtaRadioAdapter::ota_module_admitted(module))
        throw std::runtime_error("OTA station module not tested");
    if(chip.size()!=4 || state.size()!=roy_data_size || word(state)!=0x314d5250 ||
       word(state,1504)!=word(chip) || word(state,1532)!=0)
        throw std::runtime_error("Prepared state must enable only target in peer slot 0");
    for(size_t base:{size_t(1536),size_t(1568)})
        if(word(state,base)!=0 || word(state,base+28)!=1)
            throw std::runtime_error("Other OTA peers must be empty and paused");
    size_t consumed=0; const auto port=std::stoul(required("--port"),&consumed);
    if(consumed!=required("--port").size() || !port || port>65535) throw std::runtime_error("Invalid Tcl port");
    auto nanos=[] {return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();};
    auto now=[&] {return double(nanos())/1e9;};
    TrackerOta sender(image,required("--allowlist"),now,4,1);
    const auto serial=required("--serial");
    std::unique_ptr<StockOtaApproval> approval;
    if(args.count("--stock-approval") && args.count("--approval"))
        throw std::runtime_error("Select one approval policy");
    {
        std::vector<std::string> exact_argv(argv,argv+argc);
        approval=std::make_unique<StockOtaApproval>(required(args.count("--stock-approval")?
            "--stock-approval":"--approval"),chip,sender.image_sha(),exact_argv,serial);
    }
    if(args.count("--stock-approval")) {
        std::random_device random;
        const auto challenge=std::to_string(random());
        std::cout<<"Fresh readiness for "<<serial<<": normal boot WITHOUT gesture, target ALONE, "
            "NO USB/charger, HMD tracked, controllers connected. Type: READY "<<challenge<<std::endl;
        const auto asked=now();std::string answer;
        // A blocked getline cannot enforce the advertised deadline. Read bytes
        // with poll against one absolute deadline, including partial lines.
        while(now()-asked<120 && answer.size()<128) {
            pollfd input{STDIN_FILENO,POLLIN,0};
            const int ms=std::max(1,int((120-(now()-asked))*1000));
            const int ready=poll(&input,1,ms);
            if(ready<0 && errno==EINTR)continue;
            if(ready<=0)break;
            char ch;const auto n=read(STDIN_FILENO,&ch,1);
            if(n!=1)break;
            if(ch=='\n')break;
            answer+=ch;
        }
        if(!ota_readiness_matches(answer,challenge,now()-asked))
            throw std::runtime_error("Explicit fresh physical readiness answer required");
    }
    // Same fixed owner lock as producer. Never unlink the lock inode.
    struct Lock { int fd=-1; ~Lock(){if(fd>=0)close(fd);} } lock;
    const char *lock_path="/home/steamos/.local/state/frame-pico/roy.lock";
    // The launcher holds this same open file description across debugger startup
    // and approval waiting. No unlock/relock race with another radio owner.
    if(const char *inherited=std::getenv("PICO_OTA_LOCK_FD")) {
        size_t used=0;const int fd=std::stoi(inherited,&used);
        struct stat actual{},expected{};
        if(used!=std::string(inherited).size() || fd<3 || fstat(fd,&actual) || stat(lock_path,&expected) ||
           actual.st_dev!=expected.st_dev || actual.st_ino!=expected.st_ino)
            throw std::runtime_error("Inherited Roy lock mismatch");
        lock.fd=dup(fd);
    } else lock.fd=open(lock_path,O_CREAT|O_RDWR,0600);
    if(lock.fd<0 || flock(lock.fd,LOCK_EX|LOCK_NB)) throw std::runtime_error("Roy owner lock unavailable");
    OtaNativeHistory history("/home/steamos/.local/state/frame-pico/ota-native-chips");
    TclSocket socket{uint16_t(port)};
    DebugMemory debug([&](const std::string &s){return socket.command(s);});
    OwnerIO io{[&](uint32_t a,size_t n){return debug.read(a,n);},
        [&](uint32_t a,const Bytes &b){debug.write(a,b);},
        [&](const std::string &s){return socket.command(s);},nanos,
        [](int64_t ns){std::this_thread::sleep_for(std::chrono::nanoseconds(ns));}};
    SramOwner owner(io,[&](const Checkpoint &c){persist_checkpoint(checkpoint,c);});
    auto previous_int=std::signal(SIGINT,ota_signal), previous_term=std::signal(SIGTERM,ota_signal);
    std::unique_ptr<ObserverProcess> observer;
    std::unique_ptr<TrackingObserver> tracking;
    phase5::BenchTrackingGuard bench;
    double monitor_log=-1;
    auto monitor=[&] {
        try {tracking->poll(bench);bench.check(nanos(),true,true);}
        catch(const std::exception &e) {
            if(now()-monitor_log>=1) {monitor_log=now();std::cerr<<"OTA tracking monitor: "<<e.what()<<std::endl;}
        }
    };
    int result=1;
    try {
        observer=std::make_unique<ObserverProcess>(required("--watch"),required("--proof"),required("--watch-sha"));
        tracking=std::make_unique<TrackingObserver>(observer->hooks([]{return bool(ota_cancelled);}));
        monitor();
        owner.install(stock,parsed.code,parsed.entry,state);
        owner.enter(monitor);
        Bytes nonce(16); std::random_device random; for(auto &b:nonce)b=uint8_t(random());
        OtaRadioAdapter adapter(debug,sender,chip,nonce,now,module,
            args.count("--stock-approval")?approval.get():nullptr,&history,[&] {
            Bytes n(16);for(auto &b:n)b=uint8_t(random());
            return n;},serial);
        Hooks hooks; hooks.now=nanos;
        hooks.canonical=[&](const Record &r,const std::vector<Command>&,const std::optional<AckContext>&){adapter.receive(r);};
        DataPlane plane(hooks); plane.chip=word(chip); plane.exclusive=true;
        unsigned progress=0;
        RadioLoop loop(io,plane,{monitor,[&](const Bytes &){adapter.publish();},
            [](uint32_t,bool){},[&]{owner.repeat();},[](const IterationLatency&){} });
        while(!sender.terminal()) {
            if(ota_cancelled) adapter.abort("Operator abort");
            loop.step();
            if(sender.acked_blocks!=progress) {
                progress=sender.acked_blocks;
                std::cout<<"progress "<<progress<<"/"<<sender.blocks()<<std::endl;
            }
        }
        adapter.publish();
        result=sender.state==TrackerOta::State::Complete?0:1;
        std::cout<<(result?sender.error:"finish_verified; activation_verified=false")<<std::endl;
    } catch(const std::exception &e) { std::cerr<<e.what()<<std::endl; }
    // Never release the debugger/lock while Roy SRAM restoration is unresolved.
    while(owner.unresolved()) {
        try {owner.restore(); if(owner.may_release_owner())break;}
        catch(const std::exception &e) {std::cerr<<"OTA cleanup pending: "<<e.what()<<std::endl;}
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    if(observer)observer->close();
    // Only emitted after SramOwner restoration finishes. A crashed tool must
    // leave the debugger and launcher lock alive for manual recovery.
    if(const char *released=std::getenv("PICO_OTA_RELEASE_PROOF")) {
        std::ofstream out(released,std::ios::binary);
        out<<"roy_sram_released\n";out.close();
        if(!out)throw std::runtime_error("Cannot write Roy release proof");
    }
    std::signal(SIGINT,previous_int); std::signal(SIGTERM,previous_term);
    return result;
#endif
}
} // namespace pico
