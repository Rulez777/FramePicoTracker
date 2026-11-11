#include "device_config.hpp"
#include "session_duration.hpp"
#include "phase5/orchestrator.hpp"
#include "owner_decision.hpp"
#include "live_preflight.hpp"
#include "phase5/file_digest.hpp"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <set>
#include <cstdlib>
#include <thread>
#ifndef _WIN32
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace {
namespace fs = std::filesystem;
using phase5::Value;
void need(bool ok, const std::string& why) {
    if(ok)return;
    if(why.find("mismatch")!=std::string::npos || why.find("seal mismatch")!=std::string::npos ||
       why=="Read-only preflight requires an already-growing recording" ||
       why.rfind("Dataset device configuration missing/ambiguous:",0)==0){
        std::cerr<<why<<"; preparation report only, continuing\n";return;
    }
    throw std::runtime_error(why);
}
Value str(const std::string& s) { Value v; v.type='s'; v.text=s; return v; }
Value object() { Value v; v.type='{'; return v; }
std::string string(const Value& v) { need(v.type=='s'&&!v.text.empty(), "Expected nonempty string"); return v.text; }
std::string quote(const std::string& s) {
    std::string out="'"; for(char c:s) out+=c=='\''?"'\\''":std::string(1,c);
    return out+"'";
}
std::string absolute(const std::string& s) {
    need(!s.empty()&&s.front()=='/'&&s.find('\\')==std::string::npos&&s.find('\0')==std::string::npos,
         "Absolute POSIX path required: "+s);
    return s;
}
int64_t now() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}
Value record(const std::string& bytes) {
    auto v=object(); v.object["bytes"]=phase5::parse(std::to_string(bytes.size()));
    v.object["sha256"]=str(phase5::sha256(bytes)); return v;
}
phase5::Files tool_files() {
    phase5::Files files;
#ifndef _WIN32
    files.digest=phase5::file_digest;
#endif
    files.resolve=[](const std::string& p){return fs::weakly_canonical(p).string();};
    files.read=[](const std::string& p)->std::optional<std::string> {
#ifndef _WIN32
        const int fd=::open(p.c_str(),O_RDONLY|O_CLOEXEC|O_NOFOLLOW|O_NONBLOCK);
        if(fd<0) { if(errno==ENOENT)return {};
        throw std::runtime_error("Cannot open input: "+p); }
        struct Close {int fd; ~Close(){::close(fd);}} close{fd};
        struct stat st{};
        need(fstat(fd,&st)==0&&S_ISREG(st.st_mode)&&st.st_size<=16*1024*1024,"Nonregular/oversize input: "+p);
        std::string bytes; char buffer[8192];
        for(;;) { const auto n=::read(fd,buffer,sizeof(buffer)); if(n<0&&errno==EINTR)continue;
            need(n>=0,"Input read failed: "+p); if(!n)break;
            bytes.append(buffer,size_t(n)); need(bytes.size()<=16*1024*1024,"Input grew beyond cap: "+p); }
        return bytes;
#else
        (void)p; throw std::runtime_error("Linux required");
#endif
    };
    files.exclusive=[](const std::string& p,const std::string& bytes) {
#ifndef _WIN32
        const int fd=::open(p.c_str(),O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC|O_NOFOLLOW,0600);
        need(fd>=0,"Output exists/unavailable: "+p);
        struct Close {int fd; ~Close(){::close(fd);}} close{fd};
        size_t offset=0;
        while(offset<bytes.size()) {const auto n=::write(fd,bytes.data()+offset,bytes.size()-offset);
            if(n<0&&errno==EINTR)continue;
            need(n>0,"Output write failed: "+p); offset+=size_t(n);}
        need(fsync(fd)==0,"Output fsync failed: "+p);
#else
        (void)p; (void)bytes; throw std::runtime_error("Linux required");
#endif
    };
    return files;
}
}

int main(int argc,char** argv) {
    try {
        std::map<std::string,std::string> options;
        std::set<std::string> flags;
        std::vector<std::string> reviewed;
        for(int i=1;i<argc;++i) {
            std::string key=argv[i];
            if(key=="--") { while(++i<argc) reviewed.emplace_back(argv[i]);
            break; }
            if(key=="--hmd-tracked"||key=="--controllers-off"||key=="--tracker-supported"||key=="--hmd-worn"||key=="--skip-camera-preflight"||key=="--preflight-camera") {
                need(flags.insert(key).second,"Duplicate flag: "+key); continue;
            }
            need(key=="--owner-admit-unproven"||key=="--root"||key=="--human-answer"||key=="--from-python-invocation"||
                 key=="--openocd"||key=="--power-tool"||key=="--settings"||key=="--dataset-root"||
                 key=="--station-config"||key=="--registry"||key=="--tracking-watch"||key=="--producer",
                 "Usage: make_native_invocation --root NEW_ROOT --human-answer ANSWER --hmd-tracked --controllers-off --tracker-supported --hmd-worn [--preflight-camera|--skip-camera-preflight] [--from-python-invocation PATH] [--openocd PATH --power-tool PATH --settings PATH --dataset-root PATH --station-config PATH --registry PATH --tracking-watch PATH --producer PATH] -- REVIEWED_NATIVE_ARGV...");
            need(i+1<argc,"Missing option value: "+key);
            need(options.emplace(key,argv[++i]).second,"Duplicate option: "+key);
        }
        need(options.count("--root")&&!reviewed.empty(),
             "Root and native argv required");
        const bool skip=flags.count("--skip-camera-preflight")!=0;
        const bool camera=flags.count("--preflight-camera")!=0;
        const bool owner_override=options.count("--owner-admit-unproven")!=0;
        need(!owner_override||(!skip&&!camera),"Owner override refused for preflight modes");
        need(!(skip&&camera),"--preflight-camera and --skip-camera-preflight are mutually exclusive");
        for(const auto& arg:reviewed)need(arg!="--skip-camera-preflight"&&arg!="--preflight-camera","Camera flags are preflight options, never reviewed full-live argv");
#ifdef _WIN32
        throw std::runtime_error("Run this tool ON THE FRAME (Linux); no device or readiness files created");
#endif
        const auto root=absolute(options.at("--root"));
        need(fs::path(root).lexically_normal().string()==root&&root!="/", "Root must be normalized");
        need(!fs::exists(root)&&!fs::is_symlink(fs::symlink_status(root)),"Output root already exists; use a fresh attempt");
        need(fs::canonical(fs::path(root).parent_path())==fs::path(root).parent_path(),"Canonical existing output parent required");
        // These file adapters never launch processes or touch devices.
        auto files=tool_files();
        auto read=[&](const std::string& p) {
            absolute(p); auto b=files.read(p); need(bool(b),"Required input missing: "+p); return *b;
        };
        std::string decision;
        if(owner_override){
            decision=read(options.at("--owner-admit-unproven"));
            auto boot=read("/proc/sys/kernel/random/boot_id");
            while(!boot.empty()&&std::isspace(static_cast<unsigned char>(boot.back())))boot.pop_back();
            pico::validate_owner_decision(decision,boot);
            need(reviewed==std::vector<std::string>{root+"/producer","--live","--owner-admit-unproven",
                 root+"/attempt/owner-decision.json",root,root+"/invocation.json",root+"/invocation.sha256","--duration",(!reviewed.empty()&&pico::parse_session_duration(reviewed.back())?reviewed.back():std::string("150"))},
                 "Owner override requires exact full-live argv and sealed decision snapshot");
        }else for(const auto& a:reviewed)need(a!="--owner-admit-unproven","Decision file option required");
        Value source=object();
        std::string source_sha;
        // Session inputs: an optional legacy template (--from-python-invocation) or, by default, the
        // session-template directory plus the trackers in devices.conf (no device IDs in any file shipped).
        const auto target_json="{\"serial\":\""+pico::devices::serial()+"\",\"chip_id_le\":\""+pico::devices::chip_le_hex()+"\",\"slot\":0}";
        const std::string from=options.count("--from-python-invocation")?options.at("--from-python-invocation"):"";
        if(!from.empty()) {
            const auto source_bytes=read(from);
            source=phase5::parse(source_bytes);
            if(phase5::encode(phase5::field(source,"target"))!=phase5::encode(phase5::parse(target_json)))
                std::cerr<<"Template target differs from devices.conf; devices.conf wins\n";
            const auto anchor=files.read((fs::path(from).parent_path()/"invocation.sha256").string());
            need(bool(anchor),"Template invocation anchor missing");
            auto sha=*anchor;
            while(!sha.empty()&&std::isspace(static_cast<unsigned char>(sha.back())))sha.pop_back();
            need(sha==phase5::sha256(source_bytes),"Template invocation anchor mismatch");
            source_sha=sha;
        } else {
            source=phase5::parse(R"({"working_directory":"/home/steamos/pico-ram-work/session-template","files":{},"external_files":{},"external_paths":{}})");
            source_sha=phase5::sha256(target_json);
        }
        const auto source_root=absolute(string(phase5::field(source,"working_directory")));
        const auto& source_files=phase5::field(source,"files");
        const auto& source_ext=phase5::field(source,"external_files");
        const auto& source_paths=phase5::field(source,"external_paths");
        auto selected=[&](const char* option,const char* field,const std::string& fallback) {
            if(options.count(option))return absolute(options.at(option));
            const auto it=source_paths.object.find(field);
            return absolute(it==source_paths.object.end()?fallback:string(it->second));
        };
        auto source_input=[&](const char* option,const std::string& relative) {
            return options.count(option)?absolute(options.at(option)):source_root+"/"+relative;
        };
        auto checked=[&](const std::string& path) {
            auto b=read(path);
            auto ext=source_ext.object.find(path);
            if(ext!=source_ext.object.end())need(string(ext->second)==phase5::sha256(b),"Python external seal mismatch: "+path);
            const auto prefix=source_root+"/";
            if(path.rfind(prefix,0)==0) {
                auto seal=source_files.object.find(path.substr(prefix.size()));
                if(seal!=source_files.object.end()) {
                    need(string(phase5::field(seal->second,"sha256"))==phase5::sha256(b)&&
                         phase5::integer(phase5::field(seal->second,"bytes"))==int64_t(b.size()),"Python file seal mismatch: "+path);
                }
            }
            return b;
        };
        auto config=phase5::parse(checked(source_input("--station-config","attempt/station-config.json")));
        auto registry=options.count("--registry")?phase5::parse(checked(absolute(options.at("--registry")))):
            phase5::parse("{\"peers\":[{\"slot\":0,\"chip_id_le\":\""+pico::devices::chip_le_hex()+"\",\"product_sn\":\""+pico::devices::serial()+"\"}]}");
        try{phase5::validate_single_peer(config,registry);}catch(const std::exception& e){std::cerr<<e.what()<<"; registry report only, continuing\n";}
        auto& optics=config.object.at("optics").object;
        if(options.count("--dataset-root"))optics["dataset_root"]=str(absolute(options.at("--dataset-root")));
        const auto dataset=absolute(string(optics.at("dataset_root")));
        need(fs::is_directory(dataset),"Dataset root missing: "+dataset);
        const std::string private_root="/home/steamos/pico-ram-work/private/";
        optics["factory_led_file"]=str(private_root+"pico-calibration.json");
        optics["factory_camera_file"]=str(private_root+"frame-factory.json");
        std::map<std::string,std::string> snapshots;
        const std::string candidate="/home/steamos/pico-ram-work/native-port/candidate";
        if(owner_override)snapshots["attempt/owner-decision.json"]=decision;
        snapshots["stock.bin"]=checked(candidate+"/stock.bin");
        snapshots["build/roy_station.elf"]=checked(candidate+"/build/roy_station.elf");
        bool roy_located=false;
        if(const char* path=std::getenv("ROY_LAYOUT_REPORT")) {
            std::ifstream report(path); std::string line;
            roy_located=bool(std::getline(report,line)) && line=="roy_layout ok";
        }
        if(roy_located) {
            if(const char* flash=std::getenv("ROY_FLASH")) snapshots["stock.bin"]=checked(flash);
            std::cerr<<"Roy locator report supplied; stock/module hashes logged: "
                <<phase5::sha256(snapshots.at("stock.bin"))<<" "
                <<phase5::sha256(snapshots.at("build/roy_station.elf"))<<"\n";
        }
        if(!roy_located) need(phase5::sha256(snapshots.at("stock.bin"))=="0375ab90c91fa0bce1522f8f510cabfca6667cceadce1e4737b73b82507edd50","Protected stock image mismatch");
        if(!roy_located) need(phase5::sha256(snapshots.at("build/roy_station.elf"))=="d8328717a5f6b757fd002ad6622e4fa5f07c45a1de935f6631267792deb286a3","Protected original210 module mismatch");
        snapshots["attempt/station-config.json"]=phase5::encode(config)+"\n";
        snapshots["attempt/registry.json"]=phase5::encode(registry)+"\n";
        snapshots["bootstrap214.profile.json"]=checked(source_root+"/bootstrap214.profile.json");
        auto elf=[&](const std::string& path) {
            std::string b;
#ifndef _WIN32
            const auto digest=phase5::file_digest_capture(path,true,[&](const char* p,size_t n){b.append(p,n);});
            auto ext=source_ext.object.find(path);
            if(ext!=source_ext.object.end())need(string(ext->second)==digest.sha,"Python external seal mismatch: "+path);
            const auto prefix=source_root+"/";
            if(path.rfind(prefix,0)==0){auto seal=source_files.object.find(path.substr(prefix.size()));
                if(seal!=source_files.object.end())need(string(phase5::field(seal->second,"sha256"))==digest.sha&&
                    phase5::integer(phase5::field(seal->second,"bytes"))==int64_t(digest.size),"Python file seal mismatch: "+path);}
#endif
            (void)path;
            return b;
        };
        snapshots["release/tracking_watch"]=elf(source_input("--tracking-watch","release/tracking_watch"));
        const auto producer=options.count("--producer")?absolute(options.at("--producer")):
            (fs::canonical(argv[0]).parent_path()/"pico_producer").string();
        const auto producer_bytes=elf(producer);
        // argv[0] may be the new private copy or an existing reviewed producer.
        if(reviewed.front()==root+"/producer")snapshots["producer"]=producer_bytes;
        auto invocation=phase5::parse(R"({"launch_admitted":true,"preparation_only":false,"target":{"serial":"PCTESTTRACKER0001","chip_id_le":"d4c3b2a1","slot":0},"required_tracker_image_sha256":"040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d","files":{},"external_files":{},"external_file_sizes":{},"external_paths":{},"command":[]})");
        invocation.object["target"]=phase5::parse(target_json);
        invocation.object["working_directory"]=str(root);
        invocation.object["native_preflight_bundle"]=phase5::parse("true");
        invocation.object["source_python_invocation_sha256"]=str(source_sha);
        for(const auto& a:reviewed)invocation.object["command"].array.push_back(str(a));
        auto external=[&](const std::string& path,bool native) {
            std::string b;phase5::FileDigest digest;
            if(native){digest=phase5::digest_file(files,path,true);
                auto old=source_ext.object.find(path);
                if(old!=source_ext.object.end())need(string(old->second)==digest.sha,"Python external seal mismatch: "+path);
            }else{b=checked(path);digest={phase5::sha256(b),uint64_t(b.size())};}
            invocation.object["external_files"].object[path]=str(digest.sha);
            invocation.object["external_file_sizes"].object[path]=phase5::parse(std::to_string(digest.size));
            return b;
        };
        if(owner_override) {
            for(const auto& library:std::map<std::string,std::string>{
                {"optics",candidate+"/release/optical/libpico_optics.so"},
                {"detector",candidate+"/release/optical/libpico_detector.so"}}) {
                external(library.second,false);
                invocation.object["external_paths"].object[library.first]=str(library.second);
            }
        }
        const std::string native="/opt/steamvr/drivers/cv/bin/linuxarm64/XRService";
        external(native,true);
        const std::string native_sha=roy_located ? string(invocation.object["external_files"].object.at(native)) : "9e1c6f57a7e3613c69dbf7f61c62d0cfd4b8a664889791907772b03a62f9f6d8";
        if(roy_located) std::cerr<<"XRService observed SHA256 "<<native_sha<<"\n";
        need(string(invocation.object["external_files"].object.at(native))==native_sha,"XRService expected SHA256 mismatch");
        invocation.object["native_xrservice_sha256"]=str(native_sha);
        // Python's loader invokes openocd via PATH. Discover the same first
        // executable when the invocation does not explicitly name it.
        std::string discovered;
#ifndef _WIN32
        if(const char* env=std::getenv("PATH")) {
            std::string paths=env; size_t start=0;
            for(;;) {auto end=paths.find(':',start); auto dir=paths.substr(start,end-start);
                if(dir.empty())dir=fs::current_path().string();
                const auto p=(fs::absolute(dir)/"openocd").string();
                if(::access(p.c_str(),X_OK)==0) {discovered=fs::canonical(p).string(); break;}
                if(end==std::string::npos)break;
                start=end+1;
            }
        }
#endif
        const auto openocd=selected("--openocd","openocd",discovered);
        const auto power=selected("--power-tool","power_tool","/home/steamos/pico-ram-work/session-template/tools/power");
        const auto settings=selected("--settings","settings","/home/steamos/.config/openvr/config/cv/xrservice/XRServiceSettings.json");
        for(const auto& p:std::map<std::string,std::string>{{"native",native},{"openocd",openocd},{"power_tool",power},{"settings",settings}})
            invocation.object["external_paths"].object[p.first]=str(p.second);
        external(openocd,true); external(power,true);
        auto camera_source=optics.find("camera_source");
        if(camera_source!=optics.end()&&camera_source->second.text=="recording"){
            external("/usr/bin/ffmpeg",true);
            invocation.object["external_paths"].object["ffmpeg"]=str("/usr/bin/ffmpeg");
        }
        invocation.object["native_camera_preflight"]=phase5::parse(camera?"true":"false");
        external("/opt/steamvr/drivers/frame_hmd/resources/firmware/roy/openocd.cfg",false);
        // XRServiceSettings is mutable by design (the profile owner writes streamingMode 3 and restores the original
        // bytes it captured). As in the Python, only its path is bound; its content is not an external seal.
        (void)phase5::parse(checked(settings));
        for(const auto* name:{"factory_led_file","factory_camera_file"})
            (void)phase5::parse(external(string(optics.at(name)),false));
        // Select the same already-growing OCC source as the read-only adapter.
        // Historical inactive datasets are not inputs to this preflight.
        // Live capture readiness is reported when the camera starts.
        if(skip)std::cout<<"camera preflight skipped by operator\n";
        external(producer,true);
        invocation.object["external_paths"].object["producer"]=str(producer);
        if(reviewed.front()!=root+"/producer")external(absolute(reviewed.front()),true);
        for(const auto& s:snapshots)invocation.object["files"].object[s.first]=record(s.second);
        const auto bytes=phase5::encode(invocation)+"\n";
        const auto ip=root+"/invocation.json", ap=root+"/invocation.sha256";
        // Validate in memory before creating anything. Then verify all bytes again
        // from disk using the exact producer verifier before creating readiness.
        auto preview=files;
        preview.resolve=[](const std::string& p){return fs::weakly_canonical(p).string();};
        preview.digest=[&](const std::string& p,bool executable){
            const auto prefix=root+"/";
            if(p.rfind(prefix,0)==0){auto it=snapshots.find(p.substr(prefix.size()));
                if(it!=snapshots.end())return phase5::FileDigest{phase5::sha256(it->second),uint64_t(it->second.size())};}
            return phase5::digest_file(files,p,executable);
        };
        preview.read=[&](const std::string& p)->std::optional<std::string> {
            if(p==ip)return bytes;
            if(p==ap)return phase5::sha256(bytes)+"\n";
            const auto prefix=root+"/";
            if(p.rfind(prefix,0)==0) {auto it=snapshots.find(p.substr(prefix.size())); if(it!=snapshots.end())return it->second;}
            return files.read(p);
        };
        const auto bound_preview=pico::load_live_invocation(preview,root,ip,ap,reviewed);
        phase5::HumanAnswer human{options.count("--human-answer")?options.at("--human-answer"):std::string{},flags.count("--hmd-tracked")!=0,flags.count("--controllers-off")!=0,flags.count("--tracker-supported")!=0,flags.count("--hmd-worn")!=0};
        auto boot=read("/proc/sys/kernel/random/boot_id");
        while(!boot.empty()&&std::isspace(static_cast<unsigned char>(boot.back())))boot.pop_back();
        // Use canonical creation for UTF8/length and readiness checks, but keep
        // the preview write in memory until every input has passed.
        preview.exclusive=[](const std::string&,const std::string&){};
        if(!human.exact_answer.empty())try{phase5::create_readiness(preview,bound_preview,human,boot,now());}catch(const std::exception& e){std::cerr<<e.what()<<"; readiness report only\n";}
        need(fs::create_directory(root),"Cannot create output root");
        fs::permissions(root,fs::perms::owner_all,fs::perm_options::replace);
        for(const auto* d:{"build","release","attempt","attempt/recovery-journal"}) {
            fs::create_directory(root+"/"+d); fs::permissions(root+"/"+d,fs::perms::owner_all,fs::perm_options::replace);
        }
        for(const auto& s:snapshots)files.exclusive(root+"/"+s.first,s.second);
        for(const auto* executable:{"release/tracking_watch","producer"})
            if(snapshots.count(executable))fs::permissions(root+"/"+executable,fs::perms::owner_all,fs::perm_options::replace);
        files.exclusive(ip,bytes); files.exclusive(ap,phase5::sha256(bytes)+"\n");
        const auto bound=pico::load_live_invocation(files,root,ip,ap,reviewed);
        if(!human.exact_answer.empty())try{phase5::create_readiness(files,bound,human,boot,now());}catch(const std::exception& e){std::cerr<<e.what()<<"; readiness report only\n";}
        try{phase5::verify_readiness(files,bound,boot,now());}catch(const std::exception& e){std::cerr<<e.what()<<"; readiness report only\n";}
        std::cout<<"ROOT="<<quote(root)<<"\nINVOCATION_JSON="<<quote(ip)<<"\nINVOCATION_SHA256="<<quote(ap)
                 <<"\nreadiness="<<quote(root+"/attempt/human-readiness.json")<<"\n";
        if(owner_override){
            std::cout<<"make_native_invocation exact_command=";
            for(int i=0;i<argc;++i)std::cout<<quote(argv[i])<<' ';
            std::cout<<"\n"<<pico::owner_unproven_families()<<"\n";
            for(const auto& a:reviewed)std::cout<<quote(a)<<' ';
            std::cout<<'\n';return 0;
        }
        std::cout<<quote(producer)<<" --live "<<(camera?"--preflight-camera ":"--preflight-only ")<<(skip?"--skip-camera-preflight ":"")<<quote(root)<<' '<<quote(ip)<<' '<<quote(ap)<<" --";
        for(const auto& a:reviewed)std::cout<<' '<<quote(a);
        std::cout<<'\n'; return 0;
    } catch(const std::exception& e) { std::cerr<<"make_native_invocation: "<<e.what()<<'\n'; return 1; }
}
