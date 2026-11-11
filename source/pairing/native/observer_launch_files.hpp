#pragma once
#include "phase5/orchestrator.hpp"
#include "sealed_inputs.hpp"
#include <filesystem>
#include <memory>

namespace pico {
inline void verify_observer_bytes(const std::string& bytes,const std::string& reviewed_sha){
    if(reviewed_sha.size()!=64||reviewed_sha.find_first_not_of("0123456789abcdef")!=std::string::npos)
        throw std::runtime_error("Reviewed observer SHA256 required");
    if(bytes.size()<4||bytes.size()>256ULL*1024*1024||bytes.compare(0,4,"\177ELF",4)!=0)
        throw std::runtime_error("Bounded native ELF observer required");
    if(phase5::sha256(bytes)!=reviewed_sha)
        throw std::runtime_error("Observer executable seal mismatch");
}
}
#ifndef _WIN32
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
namespace pico {
// Refuse symlinks in EVERY component, including the proof directory. Retain
// descriptors, not a canonicalize-then-open pathname vulnerable to substitution.
class ObserverLaunchFiles {
    int executable_=-1,proof_=-1;
    std::filesystem::path binary_,proof_path_;
    struct stat identity_{};
    std::unique_ptr<SealedInput> sealed_;
    static int open_parent(const std::filesystem::path& path){
        if(!path.is_absolute()||path.filename().empty())
            throw std::runtime_error("Absolute observer file path required");
        int fd=::open("/",O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        if(fd<0)
            throw std::runtime_error("Observer root directory open failed");
        try{
            for(const auto& part:path.relative_path()){
                if(part=="."||part==".."||part.empty())
                    throw std::runtime_error("Observer path traversal refused");
            }
            for(const auto& part:path.parent_path().relative_path()){
                int next=openat(fd,part.c_str(),O_RDONLY|O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
                if(next<0)
                    throw std::runtime_error("Observer directory symlink or open failure");
                ::close(fd);
                fd=next;
            }
            return fd;
        }catch(...){
            ::close(fd);
            throw;
        }
    }
    static bool same(const struct stat& a,const struct stat& b){
        return a.st_dev==b.st_dev&&a.st_ino==b.st_ino&&a.st_size==b.st_size&&a.st_mode==b.st_mode
            &&a.st_mtim.tv_sec==b.st_mtim.tv_sec&&a.st_mtim.tv_nsec==b.st_mtim.tv_nsec
            &&a.st_ctim.tv_sec==b.st_ctim.tv_sec&&a.st_ctim.tv_nsec==b.st_ctim.tv_nsec;
    }
public:
    ObserverLaunchFiles(const std::filesystem::path& binary,const std::filesystem::path& proof,
                        const std::string& reviewed_sha):binary_(binary),proof_path_(proof){
        int parent=-1;
        try{
            parent=open_parent(binary);
            executable_=openat(parent,binary.filename().c_str(),O_RDONLY|O_NOFOLLOW|O_CLOEXEC|O_NONBLOCK);
            ::close(parent);
            parent=-1;
            if(executable_<0||fstat(executable_,&identity_)||!S_ISREG(identity_.st_mode)
               ||!(identity_.st_mode&0111)||identity_.st_size<4||identity_.st_size>256*1024*1024)
                throw std::runtime_error("Observer bounded regular executable required");
            if(phase5::file_digest(binary.string(),true).sha!=reviewed_sha)
                throw std::runtime_error("Observer executable seal mismatch");
            verify();
            sealed_=std::make_unique<SealedInput>(binary,reviewed_sha,true);
            parent=open_parent(proof);
            proof_=openat(parent,proof.filename().c_str(),O_WRONLY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,0600);
            if(proof_<0)
                throw std::runtime_error("Exclusive driver proof creation failed");
            if(fsync(parent))
                throw std::runtime_error("Driver proof directory flush failed");
            ::close(parent);
        }catch(...){
            if(parent>=0)
                ::close(parent);
            for(auto fd:{executable_,proof_}){
                if(fd>=0)
                    ::close(fd);
            }
            throw;
        }
    }
    ObserverLaunchFiles(const ObserverLaunchFiles&)=delete;
    ObserverLaunchFiles& operator=(const ObserverLaunchFiles&)=delete;
    ~ObserverLaunchFiles(){
        for(auto fd:{executable_,proof_}){
            if(fd>=0)
                ::close(fd);
        }
    }
    void verify()const{
        struct stat held{},named{};
        int parent=open_parent(binary_);
        int result=fstatat(parent,binary_.filename().c_str(),&named,AT_SYMLINK_NOFOLLOW);
        ::close(parent);
        if(result||fstat(executable_,&held)||!same(held,identity_)||!same(named,identity_))
            throw std::runtime_error("Observer executable changed after review");
        if(proof_>=0){
            struct stat saved{},current{};
            parent=open_parent(proof_path_);
            result=fstatat(parent,proof_path_.filename().c_str(),&current,AT_SYMLINK_NOFOLLOW);
            ::close(parent);
            if(result||fstat(proof_,&saved)||!S_ISREG(current.st_mode)
               ||saved.st_dev!=current.st_dev||saved.st_ino!=current.st_ino||saved.st_nlink!=1)
                throw std::runtime_error("Observer driver proof replaced or unlinked");
        }
    }
    // posix_spawn returns after exec. Compare the actual direct child's image
    // with the reviewed descriptor before accepting any observer output.
    void verify_child(pid_t pid)const{
        if(!sealed_)throw std::runtime_error("Observer immutable executable missing");
        sealed_->verify_child(pid);
        verify();
    }
    std::string exec_path()const{
        if(!sealed_)throw std::runtime_error("Observer immutable executable missing");
        sealed_->verify();
        return sealed_->exec_path();
    }
    int proof()const{return proof_;}
};
}
#endif
