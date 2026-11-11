#pragma once
#include <cstddef>
#include <array>
#include <cstdint>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace pico::camera {
using Bytes = std::vector<uint8_t>;
struct Record { uint32_t kind; Bytes payload; };
struct StreamKey {
    uint32_t kind; uint8_t selector; uint32_t ordinal;
    bool operator<(const StreamKey& b) const {
        return std::array<uint32_t,3>{kind,selector,ordinal} <
               std::array<uint32_t,3>{b.kind,b.selector,b.ordinal};
    }
};
struct AccessUnit {
    StreamKey key;
    std::optional<uint32_t> camera_id;
    std::optional<double> timestamp, exposure;
    std::optional<std::string> sensor;
    Bytes annex_b;
    bool upper() const;
};
std::vector<AccessUnit> camera_packets(uint32_t kind, const Bytes& payload);
Bytes parameter_sets(const Bytes& annex_b);

// Read-only transport. No recording ownership, camera control or pose publication.
class GrowingOcc {
    std::filesystem::path path_;
    uint64_t offset_ = 0, records_ = 0, bytes_ = 0;
    uint32_t limit_;
    std::optional<std::array<uint64_t,2>> identity_;
  public:
    explicit GrowingOcc(std::filesystem::path path, uint32_t limit = 8*1024*1024);
    std::vector<Record> poll(size_t max_records = 128, uint64_t max_bytes = 16*1024*1024);
    uint64_t seek_complete_tail();
    uint64_t offset() const { return offset_; }
    uint64_t records_emitted() const { return records_; }
    uint64_t bytes_emitted() const { return bytes_; }
};
struct Muxed { uint64_t pts; Bytes transport; };
class TimestampMux {
    std::map<uint16_t,uint8_t> continuity_;
    Bytes packet(uint16_t pid, const Bytes&, bool start, std::optional<uint64_t> pcr = {});
  public:
    Muxed encode(const Bytes& annex_b, double timestamp_seconds);
};
uint32_t mpeg_crc(const Bytes&);
} // namespace pico::camera


