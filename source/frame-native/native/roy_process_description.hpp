#pragma once
#include "roy_process_evidence.hpp"
namespace pico {
inline std::string roy_process_description(const RoyProcessEvidence& row) {
    auto printable=[](std::string value){for(auto& c:value)if(static_cast<unsigned char>(c)<32)c=' ';
    return value;};
    return "pid="+std::to_string(row.pid)+" exe=["+printable(row.executable)+"] argv=["+printable(row.command)+"] comm=["+printable(row.comm)+"]";
}
}
