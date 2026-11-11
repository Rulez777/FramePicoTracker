#pragma once
#include "linux_owned_files.hpp"
#include "phase5/json.hpp"
#include <memory>
#ifndef _WIN32
namespace pico {
class LiveEventSink {
    owned_files::Fd directory_,file_;
    uint64_t sequence_=0,bytes_=0;
    bool failed_=false;
  public:
    explicit LiveEventSink(const std::filesystem::path& path)
        :directory_(owned_files::parent(path)),
         file_(openat(directory_.value,path.filename().c_str(),
                       O_WRONLY|O_APPEND|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,0600)) {
        if(file_.value<0 || fsync(file_.value) || fsync(directory_.value))
            throw std::runtime_error("Durable live event creation failed");
    }
    void append(const std::string& name,const std::string& detail){
        if(failed_)throw std::runtime_error("Durable live event sink latched");
        try {
            if(name.empty() || name.size()>128 || detail.size()>1024*1024)
                throw std::runtime_error("Live event bound exceeded");
            timespec stamp{};
            if(clock_gettime(CLOCK_MONOTONIC,&stamp))
                throw std::runtime_error("Live event clock failed");
            phase5::Value row;
            row.type='{';
            row.object["sequence"]=phase5::parse(std::to_string(sequence_+1));
            row.object["t_ns"]=phase5::parse(std::to_string(int64_t(stamp.tv_sec)*1000000000+stamp.tv_nsec));
            row.object["name"].type='s';
            row.object["name"].text=name;
            row.object["detail"].type='s';
            row.object["detail"].text=detail;
            const auto line=phase5::encode(row)+"\n";
            if(bytes_+line.size()>64ULL*1024*1024)
                throw std::runtime_error("Live event budget exceeded");
            owned_files::write(file_.value,line);
            if(fdatasync(file_.value))throw std::runtime_error("Live event sync failed");
            ++sequence_;
            bytes_+=line.size();
        }catch(...){failed_=true;throw;}
    }
};
}
#endif

#include "live_implementation_registry.hpp"
#ifndef _WIN32
PICO_LIVE_IMPLEMENTATION(pico_impl_event_sink, "event-sink");
#endif
