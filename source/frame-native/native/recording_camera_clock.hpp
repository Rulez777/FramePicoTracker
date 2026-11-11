#pragma once
#include "phase5/json.hpp"
#include "optical_timing.hpp"
#include <filesystem>
#include <fstream>

namespace pico {
// Historical evidence only. Never read this machine's clocks or boot ID.
inline CameraClockEvidence read_recording_camera_clock(const std::filesystem::path& path) {
    if(std::filesystem::is_symlink(path)||!std::filesystem::is_regular_file(path)||
       std::filesystem::file_size(path)>65536)
        throw std::runtime_error("Missing/oversize original camera-clock.json");
    std::ifstream in(path,std::ios::binary);
    std::string bytes{std::istreambuf_iterator<char>(in),std::istreambuf_iterator<char>()};
    if(!in||bytes.empty())throw std::runtime_error("Cannot read original camera-clock.json");
    const auto v=phase5::Json(bytes).parse();
    auto string_field=[&](const char* key){const auto& s=phase5::field(v,key);
        if(s.type!='s'||s.text.empty())throw std::runtime_error("Invalid recording camera clock string");
    return s.text;};
    CameraClockEvidence out;
    if(phase5::number(v,"version")!=1)throw std::runtime_error("Unsupported recording camera clock version");
    out.version=1;out.boot=string_field("boot_id");out.domain=string_field("source_domain");
    out.method=string_field("method");out.verified=phase5::flag(v,"verified");
    out.offset=phase5::number(v,"offset_ns");out.uncertainty=phase5::number(v,"uncertainty_ns");
    if(!out.verified||(out.domain!="CLOCK_MONOTONIC"&&out.domain!="CLOCK_MONOTONIC_RAW")||
       out.uncertainty<0||out.uncertainty>500000||out.offset<=-INT64_MAX/8||out.offset>=INT64_MAX/8)
        throw std::runtime_error("Invalid original camera clock evidence");
    return out;
}
}
