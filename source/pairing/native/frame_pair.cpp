#include "frame_pair.hpp"
#include "image_admission.hpp"
#include "radio_loop.hpp"
#include <cstdio>
#include <fstream>
#include <iterator>
#include <chrono>
#include <thread>
#include <csignal>
#ifndef _WIN32
#include "observer_process.hpp"
#include <sys/file.h>
#endif
using namespace pico;
static Bytes file(const std::string &p){std::ifstream f(p,std::ios::binary);if(!f)throw std::runtime_error("Cannot read "+p);return Bytes(std::istreambuf_iterator<char>(f),{});}
#ifndef _WIN32
static volatile std::sig_atomic_t cancelled=0;
static void cancel(int){cancelled=1;}
#endif
int main(int argc,char **argv) {
    std::string release_path;bool may_release=true;
    try {
        std::map<std::string,std::string> args;
        for(int i=1;i<argc;i+=2){if(i+1>=argc||!args.emplace(argv[i],argv[i+1]).second)throw std::runtime_error("Unique option/value pairs required");}
        auto arg=[&](const char *k){if(!args.count(k))throw std::runtime_error(std::string("Required ")+k);return args.at(k);};
        auto number=[&](const char *k,unsigned base){auto s=arg(k);size_t n=0;auto v=std::stoul(s,&n,base);if(n!=s.size()||v>0xffffffffu)throw std::runtime_error("Invalid number");return uint32_t(v);};
        for(const auto &a:args)if(a.first!="--chip"&&a.first!="--slot"&&a.first!="--serial"&&a.first!="--module"&&a.first!="--module-sha"&&a.first!="--stock"&&a.first!="--port"&&a.first!="--attempt"&&a.first!="--watch"&&a.first!="--watch-sha"&&a.first!="--trackerless"&&a.first!="--inspect")throw std::runtime_error("Unknown option "+a.first);
        auto chip=number("--chip",16);auto state=pair_state(chip,number("--slot",10));
        auto module=file(arg("--module"));auto parsed=elf_module(module);
        const auto layout=roy_layout_report();const auto module_sha=sha256(module);
        if(parsed.code.size()>2044||(module_sha!=arg("--module-sha")&&!(layout.ok&&module_sha==layout.module_sha)))throw std::runtime_error("Pair module seal/size mismatch");
        std::cout<<"pair target="<<std::hex<<chip<<" seed=15547374 base="<<word(state,1508)<<" prefix="<<word(state,1512)<<std::dec<<" slot="<<pico_offer_slot(word(state,1512))<<" code="<<parsed.code.size()<<std::endl;
        if(args.count("--inspect"))return 0;
#ifdef _WIN32
        throw std::runtime_error("Live pairing requires Frame Linux");
#else
        auto stock=file(arg("--stock"));
        if(layout.ok){if(sha256(stock)!=layout.flash_sha)throw std::runtime_error("Roy flash report mismatch");std::cerr<<"stock sha256 "<<sha256(stock)<<std::endl;}
        else if(sha256(stock)!="0375ab90c91fa0bce1522f8f510cabfca6667cceadce1e4737b73b82507edd50")throw std::runtime_error("Wrong Roy stock image");
        auto port=number("--port",10);if(!port||port>65535)throw std::runtime_error("Invalid Tcl port");
        auto path=arg("--attempt");auto serial=arg("--serial");
        release_path=path+"/roy-release.txt";
        if(serial!="-"&&serial.size()!=17)throw std::runtime_error("Expected serial or - required");
        bool trackerless=arg("--trackerless")=="1";
        if(arg("--trackerless")!="0"&&!trackerless)throw std::runtime_error("trackerless must be 0 or 1");
        auto nanos=[] {return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();};
        if(!trackerless) {
            std::cout<<"Fresh readiness: target in discovery, other trackers OFF, NO USB, HMD tracked, controllers ON. Ordinary tracker bond flash write authorized by this pairing task. Reply GO (or да):"<<std::endl;
            const auto deadline=nanos()+120000000000LL;std::string answer;
            while(nanos()<deadline&&answer.size()<64){pollfd p{STDIN_FILENO,POLLIN,0};if(poll(&p,1,1000)<=0)continue;char c;if(read(STDIN_FILENO,&c,1)!=1)break;if(c=='\n')break;if(c!='\r')answer+=c;}
            if(answer!="GO"&&answer!="go"&&answer!="да")throw std::runtime_error("No fresh readiness; no SRAM mutation");
        }
        // Launcher retains this same open file description until release proof.
        const char *fdtext=std::getenv("PICO_PAIR_LOCK_FD");if(!fdtext)throw std::runtime_error("Use frame-pair.sh owner launcher");
        struct Lock{int fd=-1;~Lock(){if(fd>=0)close(fd);}} lock;
        lock.fd=dup(std::stoi(fdtext));struct stat a{},b{};
        if(lock.fd<0||fstat(lock.fd,&a)||stat("/home/steamos/.local/state/frame-pico/roy.lock",&b)||a.st_ino!=b.st_ino||a.st_dev!=b.st_dev||flock(lock.fd,LOCK_EX|LOCK_NB))throw std::runtime_error("Roy lock mismatch");
        TclSocket socket{uint16_t(port)};DebugMemory debug([&](const std::string &s){return socket.command(s);});
        OwnerIO io{[&](uint32_t a,size_t n){return debug.read(a,n);},[&](uint32_t a,const Bytes &b){debug.write(a,b);},[&](const std::string &s){return socket.command(s);},nanos,[](int64_t ns){std::this_thread::sleep_for(std::chrono::nanoseconds(ns));}};
        SramOwner owner(io,[&](const Checkpoint &c){persist_checkpoint(path+"/checkpoint",c);});
        auto oldint=std::signal(SIGINT,cancel),oldterm=std::signal(SIGTERM,cancel);
        std::unique_ptr<ObserverProcess> observer;std::unique_ptr<TrackingObserver> tracking;phase5::CoexistenceTrackingGuard bench;
        int64_t lastlog=0;
        auto monitor=[&]{try{tracking->poll(bench);bench.check(nanos(),tracking->alive(),true);}catch(const std::exception &e){if(nanos()-lastlog>1000000000){lastlog=nanos();std::cerr<<"pair tracking monitor: "<<e.what()<<std::endl;}}};
        PairProduct product(chip,serial=="-"?"":serial);unsigned bootstrap=0;int result=1;
        try {
            observer=std::make_unique<ObserverProcess>(arg("--watch"),path+"/observer-proof.json",arg("--watch-sha"));
            tracking=std::make_unique<TrackingObserver>(observer->hooks([]{return bool(cancelled);}));
            monitor();may_release=false;owner.install(stock,parsed.code,parsed.entry,state);
            // Exact configuration readback before entering the session.
            if(debug.read(roy_data,state.size())!=state)throw std::runtime_error("Pair state readback differs");
            std::cout<<"pair_config_readback interval_us=6000 slot_us=900 rx_end_us=700 K=1"<<std::endl;
            owner.enter(monitor);MailboxWriter writer(debug);uint32_t generation=2;bool lastack=false;
            writer.publish(0,generation,2,{0});
            Hooks hooks;hooks.now=nanos;hooks.canonical=[&](const Record &r,const std::vector<Command>&,const std::optional<AckContext>&){product.receive(r);};
            DataPlane plane(hooks);plane.chip=chip;plane.peer_chips={chip,0,0,0};plane.exclusive=true;
            RadioLoop loop(io,plane,{monitor,[&](const Bytes &){if(lastack!=product.ack){lastack=product.ack;generation+=2;writer.publish(0,generation,lastack?3:2,{0});}},[](uint32_t,bool){},[&]{owner.repeat();},[](const IterationLatency&){} });
            // Owner 2026-11-02: pairing must take <=30 s. Stop 2 s after the fresh product reply on the working address
            // (the bond is in use); PICO_PAIR_MAX_S caps the session (default 30 s, was a fixed 150 s).
            const char* max_env=std::getenv("PICO_PAIR_MAX_S");
            const int64_t max_ns=int64_t((max_env?std::atof(max_env):30.0)*1e9);
            auto start=nanos();int64_t next=start,verified_at=0;
            uint32_t scanned=0;
            while(!cancelled&&nanos()-start<max_ns){
                // --chip 0: scan mode. The module reports the chip of the first tracker in discovery; nothing is offered.
                if(!chip){scanned=word(debug.read(roy_data+1936,4));if(scanned)break;std::this_thread::sleep_for(std::chrono::milliseconds(200));continue;}
                if(!trackerless&&product.products&&product.working){
                    if(!verified_at)verified_at=nanos();
                    else if(nanos()-verified_at>2000000000LL)break;
                }
                try{loop.step();if(nanos()>=next){next=nanos()+1000000000;bootstrap=word(debug.read(roy_data+1932,4));auto counts=debug.read(roy_data,36),peer=debug.read(roy_data+1516,12);std::cout<<"pair_progress bootstrap="<<bootstrap<<" working="<<product.working<<" products="<<product.products<<" serial="<<product.serial<<" rejected="<<product.rejected<<" slots="<<word(counts,12)<<" packets="<<word(counts,16)<<" blocked="<<word(counts,24)<<" stage="<<word(peer)<<" search="<<word(peer,8)<<std::endl;}}
                catch(const std::exception &e){std::cerr<<"pair log/wait: "<<e.what()<<std::endl;std::this_thread::sleep_for(std::chrono::milliseconds(100));}
            }
            if(!chip){char hex[9];std::snprintf(hex,sizeof hex,"%08X",scanned);
                std::cout<<"scan_result chip="<<(scanned?hex:"none")<<std::endl;result=scanned?0:1;}
            else result=trackerless?(product.working==0&&bootstrap==0?0:1):(product.products?0:1);
            std::cout<<"pair_result discovery_seen="<<(bootstrap>0)<<" working_address_verified="<<(product.working>0)<<" product_verified="<<(product.products>0)<<" persistent_bond_verified=false serial="<<product.serial<<" trackerless="<<trackerless<<std::endl;
        }catch(const std::exception &e){std::cerr<<"pair error: "<<e.what()<<std::endl;}
        while(owner.unresolved()){try{owner.restore();}catch(const std::exception &e){std::cerr<<"pair cleanup pending: "<<e.what()<<std::endl;std::this_thread::sleep_for(std::chrono::seconds(1));}}
        may_release=true;
        std::cout<<"pair_cleanup "<<owner.barriers.json()<<std::endl;
        if(observer)observer->close();
        std::ofstream proof(path+"/roy-release.txt");proof<<"roy_sram_released\n";proof.close();if(!proof)throw std::runtime_error("Release proof write failed");
        std::signal(SIGINT,oldint);std::signal(SIGTERM,oldterm);return result;
#endif
    }catch(const std::exception &e){std::cerr<<e.what()<<'\n';
        // Pre-mutation readiness/argument failures must not strand the launcher.
        // Any possible SRAM mutation is released only through owner restoration.
        if(may_release&&!release_path.empty()){std::ofstream proof(release_path);proof<<"roy_sram_released\n";}
        return 1;}
}
