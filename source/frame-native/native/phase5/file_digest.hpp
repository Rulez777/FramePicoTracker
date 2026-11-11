#pragma once
#include "orchestrator.hpp"
#ifndef _WIN32
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>
namespace phase5 {
inline bool digest_identity(const struct stat& a,const struct stat& b) {
    return a.st_dev==b.st_dev&&a.st_ino==b.st_ino&&a.st_size==b.st_size&&a.st_mode==b.st_mode
        &&a.st_mtim.tv_sec==b.st_mtim.tv_sec&&a.st_mtim.tv_nsec==b.st_mtim.tv_nsec
        &&a.st_ctim.tv_sec==b.st_ctim.tv_sec&&a.st_ctim.tv_nsec==b.st_ctim.tv_nsec;
}
inline FileDigest file_digest_capture(const std::string& path,bool executable,
                                     const std::function<void(const char*,size_t)>& capture={}) {
    const int fd=::open(path.c_str(),O_RDONLY|O_CLOEXEC|O_NOFOLLOW|O_NONBLOCK);
    if(fd<0)throw std::runtime_error("Digest open failed: "+path);
    struct Close {int fd;~Close(){::close(fd);}} close{fd};
    struct stat before{},after{},named{};
    const uint64_t cap=(executable?256ULL:16ULL)*1024*1024;
    if(fstat(fd,&before)||!S_ISREG(before.st_mode)||before.st_size<0||uint64_t(before.st_size)>cap)
        throw std::runtime_error("Digest nonregular/oversize input: "+path);
    if(executable){
        char magic[4];
        if(!(before.st_mode&0111)||pread(fd,magic,4,0)!=4||std::string(magic,4)!=std::string("\177ELF",4))
            throw std::runtime_error("Digest requires executable ELF: "+path);
    }
    FileDigest result;
    result.sha=sha256_stream([&](char* bytes,size_t count)->size_t {
        ssize_t n;
        do{n=::read(fd,bytes,count);}while(n<0&&errno==EINTR);
        if(n<0)throw std::runtime_error("Digest read failed: "+path);
        result.size+=uint64_t(n);
        if(result.size>cap)throw std::runtime_error("Digest grew beyond cap: "+path);
        if(capture&&n)capture(bytes,size_t(n));
        return size_t(n);
    });
    if(fstat(fd,&after)||lstat(path.c_str(),&named)||!digest_identity(before,after)
       ||!digest_identity(before,named)||result.size!=uint64_t(before.st_size))
        throw std::runtime_error("Digest input changed during read: "+path);
    return result;
}
inline FileDigest file_digest(const std::string& path,bool executable) {
    return file_digest_capture(path,executable);
}
}
#endif
