#pragma once
#include "data_plane.hpp"
#include <cstdlib>
#include <utility>
namespace pico {
inline uint32_t roy_env(const char *name, uint32_t fallback, int base = 16) {
    const char *text = std::getenv(name);
    if (!text || !*text)
        return fallback;
    char *end = nullptr;
    const unsigned long value = std::strtoul(text, &end, base);
    if (end == text || *end != '\0' || value > 0xfffffffful)
        return fallback;
    return static_cast<uint32_t>(value);
}
inline const uint32_t roy_code = roy_env("ROY_CODE", 0x2001151c), roy_data = roy_env("ROY_DATA", 0x20010c18),
                       roy_rtt = roy_env("ROY_RTT", 0x2001064c);
// Per-thread debugger traffic. DebugMemory counts its commands and memory
// payload bytes; wire counts every TclSocket round trip on this thread,
// including commands that bypass DebugMemory.
struct TclStats { uint64_t commands=0, bytes_read=0, bytes_written=0, wire=0; };
// No process launch or SRAM ownership is implied by this transport.
class TclSocket {
    intptr_t fd_=-1;
    bool poisoned_=false;
    std::string pending_; // bytes received after a terminator (none expected)
  public:
    explicit TclSocket(uint16_t port); // explicit endpoint; loopback only
    ~TclSocket();
    TclSocket(const TclSocket&)=delete;
    TclSocket& operator=(const TclSocket&)=delete;
    std::string command(const std::string&);
};
// One radio-loop debugger command (server-side Tcl): optional DWT probe, the
// header words the loop uses (0..3, 7, write/read index 28..29), the part of
// each slot snapshot() would copy that parse_record uses (see record_dma_words),
// extra word-aligned reads, the read-index ACK and the heartbeat (header word
// 3 + 1000). Reads complete before either write.
struct RingBatch {
    bool probe=false, heartbeat=false;
    std::vector<std::pair<uint32_t,size_t>> extra;
};
struct RingBatchResult {
    std::optional<uint32_t> cycles;
    Bytes header;   // 28 bytes: words 0..3, 7, 28, 29
    Bytes ring;     // 1120-byte ring image; in the slots snapshot() copies, bytes 0..19,
                    // the record's DMA words and 100..111 are filled, the rest is zero
    std::vector<Bytes> extra;
    uint32_t write=0, read=0;
};
class DebugMemory {
    std::function<std::string(const std::string&)> command_;
    std::string issue(const std::string&,size_t read,size_t written);
  public:
    using Region=std::pair<uint32_t,Bytes>;
    explicit DebugMemory(std::function<std::string(const std::string&)> c):command_(std::move(c)){}
    Bytes read(uint32_t,size_t);
    Bytes read_many(const std::vector<std::pair<uint32_t,size_t>>&);
    void write(uint32_t,const Bytes&);
    void write_many(const std::vector<Region>&);
    RingBatchResult ring_batch(const RingBatch&);
    static std::string ring_script(const RingBatch&);
    static std::vector<std::string> write_commands(uint32_t,const Bytes&);
    static TclStats& stats();
    // While a Batch is open on this thread, DebugMemory writes are queued and
    // sent as one command when the batch closes or before the next read; a
    // write_many covering exactly the regions of an earlier queued one from the
    // same DebugMemory replaces it. Exact read_many regions handed to prefetch()
    // are served once without a command (dropped if a queued write overlaps).
    class Batch {
      public:
        Batch();
        ~Batch();
        Batch(const Batch&)=delete;
        Batch& operator=(const Batch&)=delete;
        void flush();
        // read_many regions requested during this batch (the next prefetch candidates).
        std::vector<std::pair<uint32_t,size_t>> requested() const;
        static void prefetch(uint32_t,Bytes);
    };
};
}
