#pragma once
#include "data_plane.hpp"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <chrono>
namespace pico {
inline void ota_trace(const std::string &event) {
    if(const char *path=std::getenv("PICO_OTA_TRACE")) {
        const auto ns=std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        std::ofstream out(path,std::ios::app);
        out<<ns<<" "<<event<<"\n";
    }
}
inline std::string ota_trace_hex(const Bytes &b) {
    const char *digits="0123456789abcdef";std::string s;
    for(auto v:b){s+=digits[v>>4];s+=digits[v&15];}return s;
}
inline void ota_trace_record(const Record &r) {
    if(!std::getenv("PICO_OTA_TRACE"))return;
    std::ostringstream s;
    s<<"rx host_ns="<<r.host_read_ns<<" generation="<<r.generation<<" chip="<<r.chip
     <<" channel="<<r.channel<<" profile="<<r.profile<<" hardware_crc="<<r.hardware_crc
     <<" partial="<<r.partial<<" dma="<<ota_trace_hex(r.dma);
    if(r.dma.size()>=2)try {
        for(const auto &c:decode_records(Bytes(r.dma.begin()+2,r.dma.end())))
            s<<" command="<<unsigned(c.command)<<" tag="<<unsigned(c.tag)<<" body="<<ota_trace_hex(c.body);
    }catch(const std::exception &e){s<<" decode_error="<<e.what();}
    ota_trace(s.str());
}
}
