#include "guard_evidence.hpp"
#include "data_plane.hpp"
#include "ota_trace.hpp"
#include "sensors.hpp"
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <cmath>
#include <limits>
#include <stdexcept>
#ifndef _WIN32
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#endif
namespace pico {
namespace {
[[noreturn]] void fail(const char *s) { throw std::runtime_error(s); }
uint32_t le(const Bytes &b, size_t o, size_t n) {
    if (o > b.size() || n > b.size() - o)
        fail("truncated");
    uint32_t v = 0;
    for (size_t i = 0; i < n; ++i)
        v |= uint32_t(b[o + i]) << (8 * i);
    return v;
}
int64_t add(int64_t a, int64_t b) {
    if ((b > 0 && a > INT64_MAX - b) || (b < 0 && a < INT64_MIN - b))
        fail("integer overflow");
    return a + b;
}
// NumPy's contiguous double reduction tree (PW_BLOCKSIZE=128). Preserve
// operations separately: w*x*x is not w*(x*x) at a guard boundary.
double sum(const double *a, size_t n) {
    if (n < 8) {
        double r = -0.0;
        for (size_t i = 0; i < n; ++i)
            r += a[i];
        return r;
    }
    if (n <= 128) {
        double r[8];
        for (size_t i = 0; i < 8; ++i)
            r[i] = a[i];
        size_t i = 8;
        for (; i + 8 <= n; i += 8)
            for (size_t j = 0; j < 8; ++j)
                r[j] += a[i + j];
        double v = ((r[0] + r[1]) + (r[2] + r[3])) + ((r[4] + r[5]) + (r[6] + r[7]));
        for (; i < n; ++i)
            v += a[i];
        return v;
    }
    size_t half = (n / 2) & ~size_t(7);
    return sum(a, half) + sum(a + half, n - half);
}
double sum(const std::vector<double> &a) { return sum(a.data(), a.size()); }
double midpoint(int64_t a, int64_t b) {
    // Form the integer sum before converting, as Python integer true division
    // does. Unsigned magnitudes avoid signed overflow at long host uptimes.
    uint64_t x = a < 0 ? uint64_t(0) - uint64_t(a) : uint64_t(a);
    uint64_t y = b < 0 ? uint64_t(0) - uint64_t(b) : uint64_t(b);
    if ((a < 0) == (b < 0)) {
        uint64_t s = x + y;
        if (s < x)
            return -9223372036854775808.0; // only INT64_MIN + INT64_MIN
        return (a < 0 ? -double(s) : double(s)) / 2;
    }
    return x >= y ? (a < 0 ? -double(x - y) : double(x - y)) / 2
                  : (b < 0 ? -double(y - x) : double(y - x)) / 2;
}
} // namespace
bool Sample::operator==(const Sample &b) const {
    return gyro == b.gyro && accel == b.accel && temperature == b.temperature && ticks == b.ticks &&
           counter == b.counter;
}
std::vector<Command> decode_records(const Bytes &p) {
    if (p.empty())
        fail("checksum");
    unsigned sum = 0;
    for (size_t i = 0; i + 1 < p.size(); ++i)
        sum += p[i];
    if ((sum & 255) != p.back())
        fail("checksum");
    std::vector<Command> out;
    size_t o = 0, end = p.size() - 1;
    while (o < end) {
        if (end - o < 3)
            fail("header");
        uint8_t tag = p[o++], cmd = p[o++], n = p[o++];
        if (!cmd) {
            if (n || std::any_of(p.begin() + o, p.begin() + end, [](uint8_t v) { return v != 0; }))
                fail("terminator");
            break;
        }
        if (cmd > 29 || n > end - o)
            fail("command length");
        out.push_back({tag, cmd, Bytes(p.begin() + o, p.begin() + o + n)});
        o += n;
    }
    return out;
}
std::vector<Sample> decode_imu(const Bytes &b) {
    if (b.size() != 6 && b.size() != 18 && b.size() != 24 && b.size() != 36 && b.size() != 42)
        fail("imu length");
    std::vector<Sample> out;
    for (size_t o = 0; o + 18 <= b.size(); o += 18) {
        Sample s;
        auto word = [&](size_t k) {
            uint32_t v = le(b, o + k, 2);
            return int16_t(v < 32768 ? int(v) : int(v) - 65536);
        };
        for (size_t i = 0; i < 3; ++i) {
            s.gyro[i] = word(i * 2);
            s.accel[i] = word(6 + i * 2);
        }
        s.temperature = word(12);
        s.ticks = le(b, o + 14, 3);
        s.counter = b[o + 17];
        if (s.ticks >= 533333)
            fail("sync ticks");
        out.push_back(s);
    }
    return out;
}
Record parse_record(const Bytes &b, bool enabled) {
    if (b.size() != 112)
        fail("record size");
    Record r;
    r.sequence = le(b, 0, 4);
    r.crc = le(b, 4, 4);
    r.generation = r.crc >> 16;
    r.rssi = le(b, 8, 4);
    r.channel = le(b, 12, 4);
    auto n = le(b, 16, 2);
    r.partial_bits = uint16_t(le(b, 18, 2));
    if (n > 80)
        fail("dma overlaps attribution");
    r.partial = (r.partial_bits & 1) != 0;
    r.hardware_crc = enabled && !r.partial;
    r.profile = int(r.rssi >> 8);
    r.rssi &= 255;
    r.slot = r.partial_bits >> 1;
    r.chip = le(b, 100, 4);
    r.cycles = le(b, 104, 4);
    r.callback_us = uint16_t(le(b, 108, 2));
    r.dma = Bytes(b.begin() + 20, b.begin() + 20 + n);
    return r;
}
std::optional<Bytes> snapshot(uint32_t w, uint32_t r, const Read &read,
                              const std::function<void(uint32_t)> &ack, Counters *counters) {
    // Owner rule: ring loss is counted and resynchronised, never a run abort.
    if (w < r) {
        if (counters)
            ++counters->ring_loss;
        ack(w);
        return {};
    }
    if (w - r > 10) {
        if (counters)
            ++counters->ring_loss;
        r = w - 10;
    }
    if (w == r)
        return {};
    size_t first = r % 10, n = std::min<size_t>(w - r, 10 - first);
    Bytes out(1120);
    auto copy = [&](size_t o, size_t count) {
        Bytes b = read(o, count);
        if (b.size() != count)
            fail("short snapshot read");
        std::copy(b.begin(), b.end(), out.begin() + o);
    };
    copy(first * 112, n * 112);
    if (n < w - r)
        copy(0, (w - r - n) * 112);
    ack(w);
    return out;
}
std::vector<Item> Reorder::push(Sample s, int64_t now, uint32_t cycles, Stamps stamps) {
    ++stats.pushed;
    int64_t key = s.counter;
    if (highest_) {
        int64_t mod = *highest_ % 256;
        if (mod < 0)
            mod += 256;
        int64_t d = (int64_t(s.counter) - mod + 384) % 256 - 128;
        key = add(*highest_, d);
    }
    highest_ = highest_ ? std::max(*highest_, key) : key;
    if (last_ && key <= *last_)
        ++stats.late_after_release;
    else if (conflicts_.count(key))
        ++stats.conflicting_counters;
    else if (pending_.count(key)) {
        if (pending_.at(key).sample == s)
            ++stats.identical_repeats;
        else {
            pending_.erase(key);
            conflicts_.insert(key);
            ++stats.conflicting_counters;
        }
    } else {
        stamps.insert = now;
        pending_.emplace(key, Item{key, s, cycles, stamps});
    }
    return release(now);
}
std::vector<Item> Reorder::release(int64_t now, bool all) {
    std::optional<int64_t> threshold;
    for (const auto &p : pending_)
        if (all || (now >= p.second.stamps.insert &&
                    uint64_t(now) - uint64_t(p.second.stamps.insert) >= uint64_t(hold_ns)))
            threshold = p.first;
    std::vector<Item> out;
    if (!threshold)
        return out;
    for (auto it = pending_.begin(); it != pending_.end() && it->first <= *threshold;) {
        auto v = it->second;
        v.stamps.release = now;
        last_ = v.key;
        out.push_back(v);
        it = pending_.erase(it);
    }
    stats.released += out.size();
    if (last_)
        conflicts_.erase(conflicts_.begin(), conflicts_.upper_bound(*last_));
    return out;
}
int64_t nearest_even(double v) {
    if (!std::isfinite(v) || v >= 9223372036854775808.0 || v < -9223372036854775808.0)
        fail("round overflow");
    double f = std::floor(v), d = v - f;
    if (d > 0.5 || (d == 0.5 && std::fmod(f, 2.0) != 0))
        f += 1;
    return static_cast<int64_t>(f);
}
bool RoyClock::observe(int64_t cycles, int64_t before, int64_t after, std::optional<int64_t> rb,
                       std::optional<int64_t> ra) {
    last_read_ns = nearest_even(std::max(
        -9223372036854774784.0, std::min(9223372036854774784.0, double(after) - double(before))));
    if (after < before || uint64_t(after) - uint64_t(before) > 10000000 || cycles < 0 ||
        cycles > UINT32_MAX) {
        ++rejected;
        return false;
    }
    last_read_ns = after - before;
    if (last_) {
        uint32_t d = uint32_t(cycles) - *last_;
        if (d >= 0x80000000u)
            fail("Roy half wrap/restart");
        unwrapped_ = add(unwrapped_, d);
    } else
        unwrapped_ = cycles;
    last_ = uint32_t(cycles);
    points_.push_back({unwrapped_, midpoint(before, after), double(after - before) / 2,
                       rb && ra ? std::optional<double>(midpoint(*rb, *ra)) : std::nullopt});
    if (points_.size() > 400)
        points_.pop_front();
    if (points_.size() < 6 || points_.back().mono - points_.front().mono < 300000000)
        return false;
    bool learned =
        points_.size() >= 100 && points_.back().mono - points_.front().mono >= 5000000000.;
    size_t start = learned ? 0 : (points_.size() > 40 ? points_.size() - 40 : 0),
           n = points_.size() - start;
    const auto &ref = points_.back();
    bool raw = true;
    std::vector<double> x(n), y(n), w(n), r(n), wx(n), wy(n), wr(n), v(n), cov(n), cr(n), rv(n),
        hy(n);
    for (size_t j = 0; j < n; ++j) {
        const auto &p = points_[start + j];
        double width = std::max(p.width, 10000.);
        w[j] = 1 / (width * width);
        x[j] = double(p.cycles) - double(ref.cycles);
        y[j] = p.mono - ref.mono;
        wx[j] = w[j] * x[j];
        wy[j] = w[j] * y[j];
        raw = raw && p.raw.has_value();
    }
    double sw = sum(w), mx = sum(wx) / sw, my = sum(wy) / sw, mr = 0, slope = 15.625;
    if (learned) {
        if (raw) {
            for (size_t j = 0; j < n; ++j) {
                r[j] = *points_[start + j].raw - *ref.raw;
                wr[j] = w[j] * r[j];
            }
            mr = sum(wr) / sw;
        }
        for (size_t j = 0; j < n; ++j) {
            double dx = x[j] - mx, dy = y[j] - my;
            v[j] = w[j] * (dx * dx);
            cov[j] = (w[j] * dx) * dy;
            if (raw) {
                double dr = r[j] - mr;
                cr[j] = (w[j] * dx) * dr;
                rv[j] = w[j] * (dr * dr);
                hy[j] = (w[j] * dr) * dy;
            }
        }
        double var = sum(v);
        if (var <= 0) {
            fit.reset();
            return false;
        }
        slope = sum(cov) / var;
        bool bad = false;
        if (raw) {
            double crystal = sum(cr) / var, host = sum(hy) / sum(rv);
            crystal_ppm = (crystal / 15.625 - 1) * 1e6;
            host_slew_ppm = (host - 1) * 1e6;
            bad = std::abs(crystal / 15.625 - 1) > 100e-6 || std::abs(host - 1) > 500e-6 ||
                  std::abs(slope / (crystal * host) - 1) > 100e-6;
        } else
            bad = std::abs(slope / 15.625 - 1) > 100e-6;
        if (bad) {
            // Owner rule: never drop the clock. Log and continue with the nominal 64 MHz slope.
            pico::guard_evidence::emit(25,"raw-fit",false);
            std::ofstream("/tmp/pico-diag.log", std::ios::app) << "clock_soft: slope out of bounds (crystal "
                << crystal_ppm << " ppm, host " << host_slew_ppm << " ppm); using nominal" << std::endl;
            slope = 15.625;
        }
    }
    for (size_t j = 0; j < n; ++j)
        v[j] = w[j] * (y[j] - slope * x[j]);
    double intercept = sum(v) / sw;
    double minw = points_[start].width;
    for (size_t j = 0; j < n; ++j) {
        double residual = y[j] - (slope * x[j] + intercept);
        v[j] = residual * residual;
        minw = std::min(minw, points_[start + j].width);
    }
    uncertainty = std::sqrt((sum(v) / double(n)) / double(n)) + minw;
    // Native OpenOCD reads take 3.7..4.6 ms, so the best half-width alone exceeded the old 1 ms bound and the
    // clock never admitted (2026-10-29). The weighted fit over many reads carries the precision.
    if (!(14 < slope && slope < 18)) {  // no uncertainty bound (owner rule 2026-10-29)
        fit.reset();
        return false;
    }
    fit = Fit{unwrapped_, ref.mono + intercept, slope, uncertainty};
    if(learned && raw) pico::guard_evidence::emit(25,"raw-fit",true);
    return true;
}
int64_t RoyClock::convert(uint32_t cycles, int64_t) const {
    pico::guard_evidence::Scope guard_evidence_scope(25,"source-horizon");
    if (!fit)
        fail("no clock");
    uint32_t u = cycles - uint32_t(fit->reference);
    int64_t d = u < 0x80000000u ? int64_t(u) : int64_t(u) - 4294967296LL;
    int64_t v = nearest_even(fit->stamp + fit->slope * double(d));
    // No stale/future bound (owner rule 2026-10-29): ACKs and IMU samples are never dropped by age here.
    return v;
}
uint32_t RoyClock::cycles_at(int64_t ns) const {
    pico::guard_evidence::Scope guard_evidence_scope(25,"projection-horizon");
    if (!fit)
        fail("no clock");
    // Projection beyond 250 ms is allowed (logged by callers if needed); never a run abort.
    return uint32_t(add(fit->reference, nearest_even((double(ns) - fit->stamp) / fit->slope)));
}
std::optional<AckContext> ack_context(const Record &r, const RoyClock &clock, int64_t now, uint32_t chip) {
    if (!r.hardware_crc || r.partial || r.channel != 2 || r.profile != 0 || r.chip != chip ||
        r.generation == 0 || r.generation >= 65536) {
        pico::guard_evidence::emit(10,"ack-tag",false);return {};
    }
    try {
        int64_t source = clock.convert(r.cycles, now);
        double error = clock.fit->uncertainty;
        // Owner rule 2026-10-29: no uncertainty bound on ACK attribution; the value is only carried along.
        if (!std::isfinite(error) || error < 0)
            error = 0;
        pico::guard_evidence::emit(10,"ack-tag",true);
        return AckContext{r.generation, source, uint32_t(std::ceil(error))};
    } catch (const std::exception &) {
        return {};
    }
}
std::array<uint8_t, 32> pti1(uint32_t chip, const Sample &s, uint64_t ns) {
    std::array<uint8_t, 32> out{};
    out[0] = 'P';
    out[1] = 'T';
    out[2] = 'I';
    out[3] = '1';
    auto put = [&](size_t o, uint64_t v, size_t n) {
        for (size_t i = 0; i < n; ++i)
            out[o + i] = uint8_t(v >> (i * 8));
    };
    put(4, chip, 4);
    out[8] = s.counter;
    put(12, ns, 8);
    for (size_t i = 0; i < 3; ++i) {
        put(20 + i * 2, uint16_t(s.gyro[i]), 2);
        put(26 + i * 2, uint16_t(s.accel[i]), 2);
    }
    return out;
}
UdpSender::~UdpSender() {
#ifndef _WIN32
    if (fd_ >= 0)
        close(fd_);
#endif
}
bool UdpSender::send(const std::array<uint8_t, 32> &p) {
#ifdef _WIN32
    (void)p;
    return false;
#else
    if (fd_ < 0)
          fd_ = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
    if (fd_ < 0)
        return false;
    sockaddr_in a{};
    a.sin_family = AF_INET;
    a.sin_port = htons(port_);
    a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    return sendto(fd_, p.data(), p.size(), MSG_DONTWAIT, reinterpret_cast<sockaddr *>(&a),
                  sizeof(a)) == 32;
#endif
}
bool send_udp(const std::array<uint8_t, 32> &p) {
    static thread_local UdpSender sender;
    return sender.send(p);
}
bool UdpSender::send_sensor(const pico_sensors::Wire &w) {
#ifdef _WIN32
    (void)w; return false;
#else
    if(fd_<0) fd_=socket(AF_INET,SOCK_DGRAM|SOCK_CLOEXEC,0);
    if(fd_<0) return false;
    sockaddr_in a{}; a.sin_family=AF_INET; a.sin_port=htons(port_); a.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
    return sendto(fd_,&w,sizeof(w),MSG_DONTWAIT,reinterpret_cast<sockaddr*>(&a),sizeof(a))==sizeof(w);
#endif
}
void DataPlane::publish_sensor(pico_sensors::Wire w) {
    if(!hooks.sensor) return;
    if(!sensors.start) sensors.start=hooks.now();
    if(w.flags&pico_sensors::Power) { sensors.batteryLevel=w.batteryLevel;sensors.chargerRaw=w.chargerRaw; }
    w.chip=chip; w.hostNs=uint64_t(hooks.now()); w.imuId=imu_id;
    if(pico_sensors::AccelLsb(imu_id)>0) w.flags|=pico_sensors::Scale;
    if(!hooks.sensor(w)) ++sensors.sendFailures;
    const int64_t now=hooks.now();
    if(now-sensors.lastLog>=5000000000LL) {
        sensors.lastLog=now;
        const double seconds=std::max(1e-9,double(now-sensors.start)/1e9);
        std::ofstream f("/tmp/pico-diag.log",std::ios::app);
        char line[400];
        std::snprintf(line,sizeof(line),"sensors %08X imu_id=%02X imu=%llu temp_min=%.3f temp_max=%.3f mag_fresh=%llu mag_repeat=%llu mag_fresh_hz=%.3f send_fail=%llu battery_level=%d charger_raw=%d",
            chip,imu_id,(unsigned long long)sensors.imu,sensors.imu?sensors.tempMin:NAN,sensors.imu?sensors.tempMax:NAN,
            (unsigned long long)sensors.magFresh,(unsigned long long)sensors.magRepeat,seconds>=1?sensors.magFresh/seconds:NAN,
            (unsigned long long)sensors.sendFailures,sensors.batteryLevel,sensors.chargerRaw);
        f<<line<<sensors.magInterval.log()<<'\n';
        sensors.magInterval=MagInterval{};
    }
}
void DataPlane::sensor_record(const Record &r,const std::vector<Command>&commands) {
    if(r.chip!=chip || exclusive) return;
    if(!sensor_id_loaded) {
        sensor_id_loaded=true;
        const char *path=std::getenv("PICO_SENSOR_IDS");
        if(path) { std::ifstream f(path); std::string line;
            while(std::getline(f,line)) { unsigned id=0,c=0; std::istringstream in(line);
                if(in>>std::hex>>c>>id && c==chip && pico_sensors::AccelLsb(uint8_t(id))>0 && id<=255) imu_id=uint8_t(id);
            }
        }
    }
    pico_sensors::Wire base; base.royCycles=r.cycles;
    try { base.receiveNs=uint64_t(roy().convert(r.cycles,hooks.now())); base.flags|=pico_sensors::RoyTime; } catch(...) {}
    if(commands.empty()) {
        if(decode_discovery_power(r,base)) publish_sensor(base);
        return;
    }
    if(!enrolled) return;
    for(const auto&c:commands) if(c.command==2) {
        try {
            const auto b=decode_sensors(c.body,sensor_format);
            if(b.temperature && b.imu.empty()) {
                auto w=base;w.flags|=pico_sensors::TemperatureSnapshot;w.temperature=*b.temperature;publish_sensor(w);
            }
            for(const auto&s:b.imu) {
                auto w=base; w.flags|=pico_sensors::Imu; w.counter=s.counter; w.ticks=s.ticks; w.temperature=s.temperature;
                std::copy(s.gyro.begin(),s.gyro.end(),w.gyro); std::copy(s.accel.begin(),s.accel.end(),w.accel);
                sensors.temperature(s.temperature); publish_sensor(w);
            }
            if(b.mag) {
                auto w=base; w.flags|=pico_sensors::Mag;
                if(sensors.mag(*b.mag)) w.flags|=pico_sensors::Fresh;
                std::copy(b.mag->begin(),b.mag->end(),w.mag); publish_sensor(w);
            }
        } catch(const std::exception&) { /* malformed sensor block drops, never aborts */ }
    }
}
int64_t monotonic_ns() {
#ifdef _WIN32
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               std::chrono::steady_clock::now().time_since_epoch())
        .count();
#else
    timespec ts{};
    if (clock_gettime(CLOCK_MONOTONIC, &ts))
        fail("monotonic clock");
    return int64_t(ts.tv_sec) * 1000000000 + ts.tv_nsec;
#endif
}
void Histogram::observe(int64_t begin, int64_t end) {
    if (end < begin) {
        ++negative;
        return;
    }
    const uint64_t edges[] = {1000000, 10000000, 50000000, 100000000, 250000000};
    uint64_t duration = uint64_t(end) - uint64_t(begin);
    size_t i = 0;
    while (i < 5 && duration > edges[i])
        ++i;
    ++bins[i];
}
void DataPlane::record(const Record &r, Stamps stamps) {
    ota_trace_record(r);
    stamps.record_sequence = r.sequence;
    stamps.slot = r.slot;
    if (!r.hardware_crc || r.partial) {
        pico::guard_evidence::emit(10,"canonical",false);
        ota_trace("ignore data-plane crc/partial");
        ++counters.crc;
        return;
    }
    if (r.channel == 1 || r.channel == 37 || r.channel == 77 || r.channel == 4 || r.channel == 78) {
        Record discovery=r;
        if(const auto id=discovery_sensor_chip(r.dma)) discovery.chip=*id;
        if(discovery.chip!=chip && hooks.divert && hooks.divert(discovery,stamps)) { ++counters.diverted;return; }
        sensor_record(discovery, {});
        if (hooks.discovery)
            hooks.discovery(r);
        return;
    }
    if (r.channel == 2 && r.profile == 0 && r.chip != chip && hooks.divert && hooks.divert(r, stamps)) {
        ++counters.diverted;
        return;
    }
    if (r.channel != 2 || r.profile != 0 || r.chip != chip) {
        pico::guard_evidence::emit(10,"canonical",false);
        ota_trace("ignore data-plane channel/profile/chip");
        ++counters.source;
        return;
    }
    if (r.dma.size() < 3 || r.dma.size() != size_t(r.dma[0]) + 2 ||
        (r.dma[1] != 2 && r.dma[1] != 3)) {
        {ota_trace("ignore data-plane malformed"); ++counters.malformed;}
        return;
    }
    std::vector<Command> cmds;
    try {
        cmds = decode_records(Bytes(r.dma.begin() + 2, r.dma.end()));
    } catch (const std::exception &e) {
        pico::guard_evidence::emit(10,"canonical",false);
        if (std::string(e.what()) == "checksum")
            {ota_trace("ignore data-plane checksum"); ++counters.checksum;}
        else
            {ota_trace("ignore data-plane malformed"); ++counters.malformed;}
        return;
    }
    pico::guard_evidence::emit(10,"canonical",true);
    stamps.admitted = hooks.now();
    if (hooks.canonical)
        hooks.canonical(r, cmds, ack_context(r, roy(), hooks.now(), chip)); // ACK/control before samples
    sensor_record(r, cmds);
    if (exclusive)
        return;
    if (!enrolled) {
        ++counters.unregistered;
        return;
    }
    for (const auto &cmd : cmds)
        if (cmd.command == 2) {
            std::vector<Sample> samples;
            try {
                samples = decode_sensors(cmd.body,sensor_format).imu;
            } catch (const std::exception &) {
                {ota_trace("ignore data-plane malformed"); ++counters.malformed;}
                continue;
            }
            stamps.decode = hooks.now();
            for (const auto &s : samples) {
                if (hooks.decoded)
                    hooks.decoded(r, s);
                ++counters.decoded;
                int d = last_counter_ ? (int(s.counter) - int(*last_counter_) + 256) % 256 : 1;
                if (d >= 1 && d <= 127) {
                    last_counter_ = s.counter;
                    counters.sample_gaps += uint64_t(d - 1);
                    if (feedback_enabled && hooks.feedback) {
                        hooks.feedback(s.counter);
                        ++counters.feedback;
                    }
                }
                last_imu_ns = hooks.now();
                for (auto item : reorder.push(s, hooks.now(), r.cycles, stamps)) {
                    ++counters.samples;
                    Event event{r.chip, item.stamps.record_sequence, item.stamps.slot, item, {},
                                ""};
                    const RoyClock &fitted_clock = roy();
                    if (!fitted_clock.fit) {
                        ++counters.no_clock;
                        event.disposition = "no_clock";
                    } else {
                        try {
                            event.item.stamps.convert = hooks.now();
                            int64_t now = hooks.now();
                            uint32_t u = item.cycles - uint32_t(fitted_clock.fit->reference);
                            int64_t delta =
                                u < 0x80000000u ? int64_t(u) : int64_t(u) - 4294967296LL;
                            int64_t projected =
                                nearest_even(fitted_clock.fit->stamp + fitted_clock.fit->slope * double(delta));
                            counters.receive_age.observe(projected, now);
                            int64_t ns = fitted_clock.convert(item.cycles, now);
                            if (ns < 0)
                                fail("negative PTI1 timestamp");
                            event.wire = pti1(r.chip, item.sample, uint64_t(ns));
                            pico::guard_evidence::emit(26,"PTI1",true);
                            ++counters.encoded;
                            event.item.stamps.send_begin = hooks.now();
                            if (hooks.send && !hooks.send(*event.wire)) {
                                ++counters.send_failures;
                                event.disposition = "send_failed";
                            } else {
                                if (hooks.send)
                                    ++counters.sent;
                                event.disposition = hooks.send ? "sent" : "encoded";
                            }
                            event.item.stamps.send_end = hooks.now();
                        } catch (const std::exception &) {
                            ++counters.stale_roy;
                            event.disposition = "stale_future";
                        }
                    }
                    const auto &t = event.item.stamps;
                    const int64_t beginnings[] = {t.ring_end, t.decode, t.release, t.convert,
                                                  t.send_begin};
                    const int64_t endings[] = {t.decode, t.release, t.convert, t.send_begin,
                                               t.send_end};
                    for (size_t i = 0; i < 5; ++i)
                        if (beginnings[i] && endings[i])
                            counters.hop_latency[i].observe(beginnings[i], endings[i]);
                    if (events_.size() < telemetry_capacity_)
                        events_.push_back(std::move(event));
                    else
                        ++counters.telemetry_dropped;
                }
            }
        }
    if (hooks.mailbox)
        hooks.mailbox();
}
std::optional<Event> DataPlane::next_event() {
    if (events_.empty())
        return {};
    auto e = events_.front();
    events_.pop_front();
    return e;
}
Bytes unhex(const std::string &s) {
    Bytes b;
    int hi = -1;
    for (unsigned char c : s) {
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            if (hi >= 0)
                fail("hex spacing");
            continue;
        }
        int v = c >= '0' && c <= '9'   ? c - '0'
                : c >= 'a' && c <= 'f' ? c - 'a' + 10
                : c >= 'A' && c <= 'F' ? c - 'A' + 10
                                       : -1;
        if (v < 0)
            fail("hex");
        if (hi < 0)
            hi = v;
        else {
            b.push_back(uint8_t(hi * 16 + v));
            hi = -1;
        }
    }
    if (hi >= 0)
        fail("odd hex");
    return b;
}
std::string hex(const uint8_t *p, size_t n) {
    const char *digits = "0123456789abcdef";
    std::string s;
    for (size_t i = 0; i < n; ++i) {
        s += digits[p[i] >> 4];
        s += digits[p[i] & 15];
    }
    return s;
}
} // namespace pico


#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_data_plane, "data-plane");
