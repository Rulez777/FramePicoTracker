#pragma once
#include "device_config.hpp"
#include <array>
#include <cmath>
#include <cstdio>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include "../driver/sensor_wire.h"
#include <cstdint>
#include <deque>
#include <functional>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace pico {
using Bytes = std::vector<uint8_t>;
enum class SensorFormat { Stock189, Diagnostic7, Release11, Alpha12, Unknown };
struct Sample {
    std::array<int16_t, 3> gyro{}, accel{};
    int16_t temperature = 0;
    uint32_t ticks = 0;
    uint8_t counter = 0;
    bool operator==(const Sample &b) const;
};
struct MagInterval {
    uint64_t count=0;
    std::array<uint16_t,3> raw{};
    std::array<double,3> last{}, minimum{}, maximum{};
    double normMin=0,normSum=0,normMax=0,maxStep=0;
    void add(const std::array<uint16_t,3>&m,
             const std::optional<std::array<uint16_t,3>>&previous) {
        double norm2=0,step2=0;
        raw=m;
        for(size_t i=0;i<3;++i) {
            last[i]=(int(m[i])-32768)*100.0/1024.0;
            norm2+=last[i]*last[i];
            if(previous) {
                const double d=(int(m[i])-int((*previous)[i]))*100.0/1024.0;
                step2+=d*d;
            }
            minimum[i]=count?std::min(minimum[i],last[i]):last[i];
            maximum[i]=count?std::max(maximum[i],last[i]):last[i];
        }
        const double norm=std::sqrt(norm2);
        normMin=count?std::min(normMin,norm):norm;
        normMax=std::max(normMax,norm); normSum+=norm;
        maxStep=std::max(maxStep,std::sqrt(step2)); ++count;
    }
    std::string log() const {
        if(!count) return " mag_interval_n=0 mag_raw=NA mag_uT=NA mag_norm_uT=NA mag_min_uT=NA mag_max_uT=NA mag_step_max_uT=NA";
        char text[768];
        std::snprintf(text,sizeof(text),
            " mag_interval_n=%llu mag_raw=%u,%u,%u mag_uT=%.3f,%.3f,%.3f mag_norm_uT=%.3f,%.3f,%.3f mag_min_uT=%.3f,%.3f,%.3f mag_max_uT=%.3f,%.3f,%.3f mag_step_max_uT=%.3f",
            (unsigned long long)count,unsigned(raw[0]),unsigned(raw[1]),unsigned(raw[2]),
            last[0],last[1],last[2],normMin,normSum/double(count),normMax,
            minimum[0],minimum[1],minimum[2],maximum[0],maximum[1],maximum[2],maxStep);
        return text;
    }
};
struct SensorStats {
    MagInterval magInterval;
    uint64_t magFresh=0,magRepeat=0,imu=0,sendFailures=0;
    double tempMin=std::numeric_limits<double>::infinity(),tempMax=-std::numeric_limits<double>::infinity();
    int64_t start=0,lastLog=0;
    int batteryLevel=-1,chargerRaw=-1;
    std::optional<std::array<uint16_t,3>> previous;
    bool mag(const std::array<uint16_t,3>&m) {
        const bool fresh=!previous || *previous!=m;
        if(fresh) magInterval.add(m,previous);
        previous=m;
        if(fresh) ++magFresh; else ++magRepeat;
        return fresh;
    }
    void temperature(int16_t t) { ++imu; const double c=pico_sensors::Temperature(t); tempMin=std::min(tempMin,c);tempMax=std::max(tempMax,c); }
};

struct Command {
    uint8_t tag, command;
    Bytes body;
    uint8_t sequence() const { return tag & 63; }
};
std::vector<Command> decode_records(const Bytes &);
std::vector<Sample> decode_imu(const Bytes &);
struct Record {
    uint32_t sequence = 0, crc = 0, rssi = 0, channel = 0, chip = 0, cycles = 0;
    uint16_t partial_bits = 0, callback_us = 0;
    bool hardware_crc = false, partial = false;
    int profile = -1;
    uint32_t slot = 0;
    Bytes dma;
    int64_t host_read_ns = 0;
    uint32_t generation = 0;
    uint32_t tx_generation() const { return generation; }
};
Record parse_record(const Bytes &, bool crc_enabled);
// parse_record uses only bytes 0..19, the DMA bytes 20..20+n (n = low half of
// word 4) and words 25..27 of a 112-byte slot. These are the DMA words to read
// for a given word 4; above 80 bytes it is all 20 words, so the slot still
// fails parse_record exactly as a full read would.
inline size_t record_dma_words(uint32_t length_word) {
    const size_t n = ((length_word & 0xffff) + 3) / 4;
    return n > 20 ? 20 : n;
}
// Pure snapshot adapter: read uses offsets relative to ring start, acknowledge
// receives captured write_count. No hardware implementation is supplied.
using Read = std::function<Bytes(size_t, size_t)>;
struct Counters;
std::optional<Bytes> snapshot(uint32_t write, uint32_t read, const Read &,
                              const std::function<void(uint32_t)> &acknowledge,
                              Counters * = nullptr);
struct Stamps {
    uint32_t record_sequence = 0, slot = 0;
    int64_t ring_start = 0, ring_end = 0, snapshot_free = 0, poll_start = 0, poll_end = 0;
    int64_t admitted = 0, decode = 0, insert = 0, release = 0, convert = 0, send_begin = 0,
            send_end = 0;
};
struct Item {
    int64_t key = 0;
    Sample sample;
    uint32_t cycles = 0;
    Stamps stamps;
};
struct ReorderStats {
    uint64_t pushed = 0, released = 0, identical_repeats = 0, conflicting_counters = 0,
             late_after_release = 0;
};
class Reorder {
    std::optional<int64_t> highest_, last_;
    std::map<int64_t, Item> pending_;
    std::set<int64_t> conflicts_;

  public:
    ReorderStats stats;
    // Hold before release. The live DataPlane uses 0: the 50 ms hold added ~100 ms of IMU latency (2026-10-29).
    int64_t hold_ns = 50000000;
    std::vector<Item> push(Sample, int64_t, uint32_t, Stamps = {});
    std::vector<Item> release(int64_t, bool all_pending = false);
    size_t pending() const { return pending_.size(); }
};
struct Fit {
    int64_t reference;
    double stamp, slope, uncertainty;
};
class RoyClock {
    struct Point {
        int64_t cycles;
        double mono, width;
        std::optional<double> raw;
    };
    std::deque<Point> points_;
    std::optional<uint32_t> last_;
    int64_t unwrapped_ = 0;

  public:
    std::optional<Fit> fit;
    uint64_t rejected = 0;
    int64_t last_read_ns = 0;
    double uncertainty = 0, crystal_ppm = 0, host_slew_ppm = 0;
    // Owner-thread rendezvous (radio_loop.cpp): while a RadioLoop carries the DWT
    // probe in its own debugger commands, RoyClockPoll parks a due probe here and
    // the loop runs it, bracketing whichever command performs the read.
    bool probe_carrier = false;
    std::function<void(const std::function<uint32_t()> &)> probe_due;
    bool observe(int64_t, int64_t, int64_t, std::optional<int64_t> = {},
                 std::optional<int64_t> = {});
    int64_t convert(uint32_t, int64_t) const;
    uint32_t cycles_at(int64_t) const;
    size_t points() const { return points_.size(); }
};
int64_t nearest_even(double);
struct AckContext {
    uint32_t generation;
    int64_t source_ns;
    uint32_t uncertainty_ns;
};
// Call only after canonical packet admission. Absence gates special ACK
// retirement, not the ordinary data/diagnostics path.
// chip: the admitted tracker of the calling DataPlane (multi4: one DataPlane per tracker).
std::optional<AckContext> ack_context(const Record &, const RoyClock &, int64_t now,
                                      uint32_t chip = pico::devices::chip());
std::array<uint8_t, 32> pti1(uint32_t, const Sample &, uint64_t);
class UdpSender {
    int fd_ = -1;
    uint16_t port_;

  public:
    explicit UdpSender(uint16_t destination_port = 39571) : port_(destination_port) {}
    UdpSender(const UdpSender &) = delete;
    UdpSender &operator=(const UdpSender &) = delete;
    ~UdpSender();
    bool send(const std::array<uint8_t, 32> &);
    bool send_sensor(const pico_sensors::Wire &);
};
bool send_udp(const std::array<uint8_t, 32> &); // UDP localhost:39571; never used by replay
int64_t monotonic_ns();
struct Histogram {
    // <=1ms, <=10ms, <=50ms, <=100ms, <=250ms, >250ms.
    std::array<uint64_t, 6> bins{};
    uint64_t negative = 0;
    void observe(int64_t begin, int64_t end);
};
struct Counters {
    uint64_t malformed = 0, checksum = 0, source = 0, crc = 0, ring_loss = 0;
    uint64_t decoded = 0, samples = 0, sample_gaps = 0, feedback = 0, no_clock = 0,
             unregistered = 0, peer_rejected = 0, diverted = 0;
    uint64_t stale_roy = 0, send_failures = 0, encoded = 0, sent = 0, telemetry_dropped = 0;
    Histogram receive_age; // includes future and stale conversion attempts
    // ring->decode, decode->release, release->convert, convert->send, send duration
    std::array<Histogram, 5> hop_latency;
};
struct Event {
    uint32_t chip, sequence, slot;
    Item item;
    std::optional<std::array<uint8_t, 32>> wire;
    std::string disposition;
};
struct Hooks {
    std::function<int64_t()> now = monotonic_ns;
    std::function<void(const Record &, const std::vector<Command> &,
                       const std::optional<AckContext> &)>
        canonical;
    std::function<void(const Record &)> discovery;
    std::function<bool(const pico_sensors::Wire &)> sensor;
    std::function<void(uint8_t)> feedback;
    std::function<void(const Record &, const Sample &)> decoded;
    std::function<void()> mailbox;
    std::function<bool(const std::array<uint8_t, 32> &)> send;
    // multi4: a working-channel record of another enrolled chip goes to that tracker's DataPlane (true = taken).
    std::function<bool(const Record &, const Stamps &)> divert;
};
class DataPlane {
    std::optional<uint8_t> last_counter_;
    std::deque<Event> events_;
    size_t telemetry_capacity_;

  public:
    explicit DataPlane(Hooks h = {}, size_t capacity = 1024)
        : telemetry_capacity_(capacity), hooks(std::move(h)) {}
    Hooks hooks;
    Counters counters;
    SensorStats sensors;
    SensorFormat sensor_format=SensorFormat::Stock189;
    bool sensor_id_loaded=false;
    uint8_t imu_id=0; // Unknown until per-chip sensor_ids.txt configuration supplies WHO_AM_I.
    void sensor_record(const Record &, const std::vector<Command> &);
    void publish_sensor(pico_sensors::Wire);
    Reorder reorder = [] { Reorder r; r.hold_ns = 0; return r; }();
    RoyClock clock;
    bool enrolled = false, feedback_enabled = true, exclusive = false;
    // Admitted tracker of this plane; multi4 secondary planes convert with the primary's fitted clock.
    uint32_t chip = pico::devices::chip();
    const RoyClock *shared_clock = nullptr;
    // multi4 ring: module peer index -> chip (radio_loop.cpp); index 0 is the first target.
    std::array<uint32_t, 4> peer_chips{pico::devices::chip(), 0, 0, 0};
    const RoyClock &roy() const { return shared_clock ? *shared_clock : clock; }
    int64_t last_imu_ns = 0;
    void reset_peer() {
        last_counter_.reset();
        sensors=SensorStats{};
        enrolled = false;
    } // deliberately retains reorder
    void record(const Record &, Stamps = {});
    std::optional<Event> next_event();
};
Bytes unhex(const std::string &);
std::string hex(const uint8_t *, size_t);
} // namespace pico
