#pragma once
#ifndef _WIN32
#include <cerrno>
#include <cstring>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
namespace pico {
namespace owned_files {
struct Fd {
    int value=-1;
    explicit Fd(int fd=-1):value(fd){}
    Fd(const Fd&)=delete;
    Fd& operator=(const Fd&)=delete;
    ~Fd(){if(value>=0)::close(value);}
};
inline int parent(const std::filesystem::path& path){
    if(!path.is_absolute()||path.filename().empty()||path.string().find('\0')!=std::string::npos)
        throw std::runtime_error("Absolute owned file path required");
    for(const auto& part:path.relative_path())
        if(part=="."||part==".."||part.empty())
            throw std::runtime_error("Owned file traversal refused");
    int fd=::open("/",O_RDONLY|O_DIRECTORY|O_CLOEXEC);
    if(fd<0){
        const int error=errno;
        throw std::runtime_error("Owned root open failed: path=/ errno="+
            std::to_string(error)+" ("+std::strerror(error)+")");
    }
    std::filesystem::path traversed="/";
    for(const auto& part:path.parent_path().relative_path()){
        traversed/=part;
        int next=openat(fd,part.c_str(),O_RDONLY|O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
        const int error=errno;
        ::close(fd);
        if(next<0)
            throw std::runtime_error("Owned directory openat(O_RDONLY|O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC) refused: path="+
                traversed.string()+" requested="+path.string()+" errno="+
                std::to_string(error)+" ("+std::strerror(error)+")");
        fd=next;
    }
    return fd;
}
inline std::string read(int fd,size_t cap){
    struct stat st{};
    if(fstat(fd,&st)||!S_ISREG(st.st_mode)||st.st_size<0||uint64_t(st.st_size)>cap)
        throw std::runtime_error("Bounded regular owned file required");
    std::string bytes(size_t(st.st_size),'\0');
    size_t at=0;
    while(at<bytes.size()){
        auto n=pread(fd,bytes.data()+at,bytes.size()-at,static_cast<off_t>(at));
        if(n<0&&errno==EINTR)continue;
        if(n<=0)throw std::runtime_error("Owned file short read");
        at+=size_t(n);
    }
    char extra=0;
    if(pread(fd,&extra,1,static_cast<off_t>(at))!=0)
        throw std::runtime_error("Owned file grew/read failed");
    return bytes;
}
inline void write(int fd,const std::string& bytes){
    size_t at=0;
    while(at<bytes.size()){
        auto n=::write(fd,bytes.data()+at,bytes.size()-at);
        if(n<0&&errno==EINTR)continue;
        if(n<=0)throw std::runtime_error("Owned file write failed");
        at+=size_t(n);
    }
}
inline bool same(const struct stat& a,const struct stat& b){
    return a.st_dev==b.st_dev&&a.st_ino==b.st_ino&&a.st_size==b.st_size&&a.st_mode==b.st_mode
        &&a.st_nlink==b.st_nlink&&a.st_uid==b.st_uid&&a.st_gid==b.st_gid
        &&a.st_mtim.tv_sec==b.st_mtim.tv_sec&&a.st_mtim.tv_nsec==b.st_mtim.tv_nsec
        &&a.st_ctim.tv_sec==b.st_ctim.tv_sec&&a.st_ctim.tv_nsec==b.st_ctim.tv_nsec;
}
}
}
#endif
