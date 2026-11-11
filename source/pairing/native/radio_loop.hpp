#pragma once
#include "roy_owner.hpp"
#include <memory>
namespace pico {
struct IterationLatency {
    uint64_t iteration = 0;
    int64_t begin = 0, snapshot_done = 0, poll_done = 0, dispatch_done = 0, end = 0, wake = 0;
    uint32_t records = 0;
    // Debugger traffic on the owner thread since the previous iteration ended
    // (DebugMemory commands and payload bytes; wire = every TclSocket round trip).
    uint64_t tcl_commands = 0, wire_commands = 0, bytes_read = 0, bytes_written = 0;
    bool batched = false, probe = false, probe_folded = false;
    bool complete = false;
};
struct LoopHooks {
    std::function<void()> guard;
    std::function<void(const Bytes &)> poll;
    std::function<void(uint32_t, bool)> publish;
    std::function<void()> repeat;
    std::function<void(const IterationLatency &)> latency;
};
// One admitted ABI4/service iteration, no process or hardware entry point.
// Sleep is RELATIVE after work with injected time: 5ms when no record arrived,
// 0.5ms (session lock hand-off) after records.
// With OwnerIO::command the used header words, the used part of each new ring
// slot, ACK, heartbeat and the previous iteration's station reads travel in one
// debugger command (see DebugMemory::ring_batch); LoopHooks::poll then gets a
// 120-byte header holding only words 0..3, 7, 28, 29. Without it every region
// is a separate read/write and poll gets the full header.
class RadioLoop {
    OwnerIO io_;
    DataPlane &plane_;
    LoopHooks hooks_;
    int64_t heartbeat_ = 0;
    uint64_t iterations_ = 0;
    bool crc_enabled_, repeat_host_;
    std::unique_ptr<DebugMemory> batch_;
    int64_t last_batch_ns_ = INT64_MAX;
    unsigned batch_failures_ = 0;
    uint64_t batch_off_until_ = 0;
    std::vector<std::pair<uint32_t, size_t>> prefetch_;
    std::optional<TclStats> counted_;
    void count(IterationLatency &);

  public:
    std::array<Histogram, 5> latency; // snapshot, poll, dispatch, work, sleep
    IterationLatency last;
    RadioLoop(OwnerIO, DataPlane &, LoopHooks, bool crc_enabled = true, bool repeat_host = true);
    ~RadioLoop();
    RadioLoop(const RadioLoop &) = delete;
    RadioLoop &operator=(const RadioLoop &) = delete;
    void step();
};
struct ClockProbeEvent {
    uint32_t cycles;
    int64_t before, after, raw_before, raw_after;
    bool admitted;
};
class RoyClockPoll {
    OwnerIO io_;
    RoyClock &clock_;
    std::function<int64_t()> raw_;
    std::function<void(const ClockProbeEvent &)> log_;
    std::optional<int64_t> acquisition_;
    int64_t last_poll_ = 0;
    int64_t last_clock_log_ = 0;

  public:
    RoyClockPoll(OwnerIO, RoyClock &, std::function<int64_t()> raw_clock,
                 std::function<void(const ClockProbeEvent &)>);
    ~RoyClockPoll();
    RoyClockPoll(const RoyClockPoll &) = delete;
    RoyClockPoll &operator=(const RoyClockPoll &) = delete;
    void poll(bool producer_diagnostics_ready);
};
} // namespace pico
