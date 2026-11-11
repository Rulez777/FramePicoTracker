#pragma once
#include "phase5/orchestrator.hpp"
#include "phase5/file_digest.hpp"
#include <set>
#include <utility>
#include <memory>
#include <algorithm>
namespace pico {
// Independently reviewed manifest edges. This checks graph completeness only;
// it cannot discover dlopen, Tcl evaluation, environment or loader searches.
// No method here declares a production dependency closure admitted.
struct ReviewedInput {
    std::string path,sha256;
    bool executable=false;
    std::vector<std::string> dependencies;
};
inline void validate_input_graph(const std::vector<ReviewedInput>& inputs,const std::string& entry){
    if(inputs.empty()||inputs.size()>256)
        throw std::runtime_error("Reviewed input count outside cap");
    std::map<std::string,const ReviewedInput*> index;
    size_t edges=0;
    for(const auto& input:inputs){
        if(input.path.empty()||input.path.front()!='/'||input.path.find('\0')!=std::string::npos
           ||input.path.size()>4096||input.path.back()=='/'||input.path.find("//")!=std::string::npos
           ||input.path.find("/./")!=std::string::npos||input.path.find("/../")!=std::string::npos
           ||(input.path.size()>=2&&input.path.compare(input.path.size()-2,2,"/.")==0)
           ||(input.path.size()>=3&&input.path.compare(input.path.size()-3,3,"/..")==0)
           ||input.sha256.size()!=64||input.sha256.find_first_not_of("0123456789abcdef")!=std::string::npos
           ||!index.emplace(input.path,&input).second)
            throw std::runtime_error("Invalid/duplicate reviewed dependency");
        edges+=input.dependencies.size();
        if(edges>4096)throw std::runtime_error("Reviewed dependency edge cap exceeded");
        std::set<std::string> unique(input.dependencies.begin(),input.dependencies.end());
        if(unique.size()!=input.dependencies.size())throw std::runtime_error("Duplicate dependency edge");
    }
    if(!index.count(entry)||!index.at(entry)->executable)
        throw std::runtime_error("Reviewed executable entry missing");
    std::set<std::string> visited;
    std::vector<std::string> pending{entry};
    while(!pending.empty()){
        auto path=std::move(pending.back());pending.pop_back();
        if(!index.count(path))throw std::runtime_error("Unlisted dependency edge");
        if(!visited.insert(path).second)continue;
        for(const auto& child:index.at(path)->dependencies)pending.push_back(child);
    }
    if(visited.size()!=inputs.size())throw std::runtime_error("Unreachable reviewed input");
}
// Length-prefixed fields, canonical path/edge order and a version/domain bind
// the complete graph and entry to an independent review anchor. The anchor must
// never be generated from the untrusted manifest at a production admission site.
inline std::string input_graph_sha256(std::vector<ReviewedInput> inputs,const std::string& entry){
    validate_input_graph(inputs,entry);
    std::sort(inputs.begin(),inputs.end(),[](const ReviewedInput& a,const ReviewedInput& b){return a.path<b.path;});
    std::string bytes="pico-reviewed-input-graph-v1:";
    auto field=[&](const std::string& s){bytes+=std::to_string(s.size())+":"+s;};
    field(entry);
    field(std::to_string(inputs.size()));
    for(auto& input:inputs){
        field(input.path);field(input.sha256);field(input.executable?"executable":"data");
        std::sort(input.dependencies.begin(),input.dependencies.end());
        field(std::to_string(input.dependencies.size()));
        for(const auto& dependency:input.dependencies)field(dependency);
    }
    return phase5::sha256(bytes);
}
inline void verify_input_graph(const std::vector<ReviewedInput>& manifest,const std::string& entry,
                               const std::string& independently_reviewed_sha){
    if(independently_reviewed_sha.size()!=64
       ||independently_reviewed_sha.find_first_not_of("0123456789abcdef")!=std::string::npos
       ||input_graph_sha256(manifest,entry)!=independently_reviewed_sha)
        throw std::runtime_error("Independent dependency graph anchor mismatch");
}
}
#ifndef _WIN32
#include "linux_owned_files.hpp"
#include <sys/syscall.h>
#include <linux/memfd.h>
namespace pico {
// Source capture requires stable timestamps too. A sealed memfd does not:
// seals protect bytes/length, not timestamps (futimens is still permitted;
// a failed write can also update mtime/ctime on Linux).
inline std::string sealed_identity_changes(const struct stat& before,const struct stat& after,
                                           bool timestamps=false){
    std::string changes;
    auto field=[&](const char* name,auto a,auto b){
        if(a!=b)changes+=std::string(changes.empty()?"":"; ")+name+
            " before="+std::to_string(a)+" after="+std::to_string(b);
    };
    field("st_dev",before.st_dev,after.st_dev);
    field("st_ino",before.st_ino,after.st_ino);
    field("st_size",before.st_size,after.st_size);
    field("st_mode",before.st_mode,after.st_mode);
    field("st_nlink",before.st_nlink,after.st_nlink);
    field("st_uid",before.st_uid,after.st_uid);
    field("st_gid",before.st_gid,after.st_gid);
    if(timestamps){
        field("st_mtim.tv_sec",before.st_mtim.tv_sec,after.st_mtim.tv_sec);
        field("st_mtim.tv_nsec",before.st_mtim.tv_nsec,after.st_mtim.tv_nsec);
        field("st_ctim.tv_sec",before.st_ctim.tv_sec,after.st_ctim.tv_sec);
        field("st_ctim.tv_nsec",before.st_ctim.tv_nsec,after.st_ctim.tv_nsec);
    }
    return changes;
}
// Immutable content snapshots, never a path-only metadata seal. All four
// kernel seals are mandatory. No fallback to an unsealed temporary file.
class SealedInput {
    int fd_=-1;
    struct stat identity_{};
    std::string sha_;
public:
    SealedInput(const std::filesystem::path& path,const std::string& reviewed,bool executable){
        owned_files::Fd parent(owned_files::parent(path));
        owned_files::Fd source(openat(parent.value,path.filename().c_str(),O_RDONLY|O_CLOEXEC|O_NOFOLLOW|O_NONBLOCK));
        struct stat before{},after{},named{};
        if(source.value<0||fstat(source.value,&before)||!S_ISREG(before.st_mode)||before.st_nlink!=1
           ||(executable&&!(before.st_mode&0111)))
            throw std::runtime_error("Invalid sealed input source");
        int fd=int(syscall(SYS_memfd_create,"pico-reviewed-input",MFD_CLOEXEC|MFD_ALLOW_SEALING));
        if(fd<0)throw std::runtime_error("Immutable memfd input unavailable");
        if(fd<3){
            const int retained=fcntl(fd,F_DUPFD_CLOEXEC,3);
            ::close(fd);
            if(retained<0)throw std::runtime_error("Immutable input descriptor reservation failed");
            fd=retained;
        }
        try{
            const uint64_t cap=(executable?256ULL:16ULL)*1024*1024;
            if(before.st_size<0||uint64_t(before.st_size)>cap)throw std::runtime_error("Sealed input size cap");
            char magic[4];
            if(executable&&(pread(source.value,magic,4,0)!=4||std::string(magic,4)!=std::string("\177ELF",4)))
                throw std::runtime_error("Sealed executable must be native ELF");
            uint64_t count=0;
            const auto hash=phase5::sha256_stream([&](char* buffer,size_t capacity)->size_t{
                ssize_t n;do{n=::read(source.value,buffer,capacity);}while(n<0&&errno==EINTR);
                if(n<0)throw std::runtime_error("Sealed input read failed");
                count+=uint64_t(n);if(count>cap)throw std::runtime_error("Sealed input grew beyond cap");
                if(n)owned_files::write(fd,std::string(buffer,size_t(n)));
                return size_t(n);
            });
            if(reviewed.size()!=64||hash!=reviewed)throw std::runtime_error("Sealed input digest mismatch");
            if(fstat(source.value,&after)||fstatat(parent.value,path.filename().c_str(),&named,AT_SYMLINK_NOFOLLOW)
               ||count!=uint64_t(before.st_size)||!sealed_identity_changes(before,after,true).empty()
               ||!sealed_identity_changes(before,named,true).empty())
                throw std::runtime_error("Sealed input source changed during capture");
            if(fchmod(fd,executable?0500:0400)
               ||fcntl(fd,F_ADD_SEALS,F_SEAL_WRITE|F_SEAL_GROW|F_SEAL_SHRINK|F_SEAL_SEAL))
                throw std::runtime_error("Immutable input sealing failed");
            if(fstat(fd,&identity_))throw std::runtime_error("Sealed input identity failed");
            fd_=fd;sha_=reviewed;
            verify();
            off_t offset=0;
            const auto readback=phase5::sha256_stream([&](char* buffer,size_t capacity)->size_t{
                ssize_t n;do{n=pread(fd_,buffer,capacity,offset);}while(n<0&&errno==EINTR);
                if(n<0)throw std::runtime_error("Immutable input readback failed");
                offset+=n;return size_t(n);
            });
            if(readback!=sha_||uint64_t(offset)!=count)
                throw std::runtime_error("Immutable input readback mismatch");
        }catch(...){::close(fd);fd_=-1;throw;}
    }
    SealedInput(const SealedInput&)=delete;
    SealedInput& operator=(const SealedInput&)=delete;
    ~SealedInput(){if(fd_>=0)::close(fd_);}
    void verify()const{
        const int required=F_SEAL_WRITE|F_SEAL_GROW|F_SEAL_SHRINK|F_SEAL_SEAL;
        struct stat actual{};
        const int seals=fcntl(fd_,F_GET_SEALS);
        if(seals<0||(seals&required)!=required)
            throw std::runtime_error("Immutable input seals lost: required="+std::to_string(required)+
                                     " actual="+std::to_string(seals));
        if(fstat(fd_,&actual))
            throw std::runtime_error("Immutable input fstat failed errno="+std::to_string(errno));
        const auto changes=sealed_identity_changes(identity_,actual);
        if(!changes.empty())
            throw std::runtime_error("Immutable input identity/content lost: "+changes);
    }
    void verify_child(pid_t pid)const{
        struct stat actual{};
        if(pid<=0||stat(("/proc/"+std::to_string(pid)+"/exe").c_str(),&actual)
           ||actual.st_dev!=identity_.st_dev||actual.st_ino!=identity_.st_ino)
            throw std::runtime_error("Child did not execute sealed input");
        verify();
    }
    std::string exec_path()const{return "/proc/self/fd/"+std::to_string(fd_);}
    int descriptor()const{return fd_;}
    size_t size()const{return size_t(identity_.st_size);}
};
class SealedInputs {
    std::map<std::string,std::unique_ptr<SealedInput>> inputs_;
public:
    SealedInputs(const std::vector<ReviewedInput>& manifest,const std::string& entry,
                 const std::string& independently_reviewed_sha){
        verify_input_graph(manifest,entry,independently_reviewed_sha);
        size_t total=0;
        for(const auto& row:manifest){
            auto input=std::make_unique<SealedInput>(row.path,row.sha256,row.executable);
            total+=input->size();
            if(total>512ULL*1024*1024)throw std::runtime_error("Sealed input aggregate cap exceeded");
            inputs_.emplace(row.path,std::move(input));
        }
    }
    const SealedInput& at(const std::string& path)const{
        auto it=inputs_.find(path);
        if(it==inputs_.end())throw std::runtime_error("Unlisted sealed input requested");
        it->second->verify();
        return *it->second;
    }
};
}
#endif

#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_seals, "seals");
