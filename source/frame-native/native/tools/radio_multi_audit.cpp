// radio_multi_audit <radio.bin> [bin_s]: per-chip data-plane audit of a PICO_RADIO_DUMP ([i64 host_ns][112-byte record]).
// Per chip: replies, empty replies (ACK only), replies with command 2, Alpha/legacy samples, counter gaps, module
// packet-counter steps between consecutive records (all peers), and a per-bin timeline. 2026-11-02 multi data plane.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <map>
#include <vector>
namespace {
struct Chip {
    uint64_t replies = 0, empty = 0, cmd2 = 0, samples = 0, gaps = 0, mag = 0, other = 0, bad = 0;
    std::map<unsigned, uint64_t> commands, body_len;
    int last_counter = -1;
    bool have_cycles = false;
    uint32_t last_cycles = 0;
    // Roy DWT capture interval to the previous reply of the same chip, in 6-ms slots; split by whether this reply
    // carries command 2 (sensor body) or not.
    std::map<long, uint64_t> interval_6ms, interval_data;
    double first = 0, last = 0;
    std::map<long, uint64_t> bin_replies, bin_samples, bin_empty;
};
}
int main(int argc, char **argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: radio_multi_audit radio.bin [bin_s]\n");
        return 2;
    }
    const double bin = argc > 2 ? std::atof(argv[2]) : 10.0;
    // argv[3] = chip (hex), argv[4] = max lines: print that chip's non-empty replies (time, packet counter, DMA hex)
    const uint32_t dump_chip = argc > 3 ? uint32_t(std::strtoul(argv[3], nullptr, 16)) : 0;
    const unsigned long dump_max = argc > 4 ? std::strtoul(argv[4], nullptr, 10) : 40;
    unsigned long dumped = 0;
    std::ifstream f(argv[1], std::ios::binary);
    unsigned char b[120];
    std::map<uint32_t, Chip> chips;
    uint64_t total = 0;
    double t0 = -1, tl = 0;
    uint32_t last_seq = 0;
    bool have_seq = false;
    std::map<uint32_t, uint64_t> seq_step;
    while (f.read(reinterpret_cast<char *>(b), 120)) {
        int64_t ns;
        std::memcpy(&ns, b, 8);
        const unsigned char *r = b + 8;
        auto u32 = [&](int o) { return uint32_t(r[o]) | uint32_t(r[o + 1]) << 8 | uint32_t(r[o + 2]) << 16 | uint32_t(r[o + 3]) << 24; };
        const double t = double(ns) / 1e9;
        if (ns <= 0)  // records written before the first host stamp
            continue;
        if (dump_chip && u32(100) == dump_chip && r[16] > 3 && dumped < dump_max) {
            ++dumped;
            std::printf("%.6f seq %u len %u:", t, u32(0) & 0x7ffff, unsigned(r[16]));
            for (unsigned i = 0; i < r[16] && i < 80; ++i)
                std::printf(" %02x", r[20 + i]);
            std::printf("\n");
        }
        if (t0 < 0)
            t0 = t;
        tl = t;
        ++total;
        const uint32_t seq = u32(0) & 0x7ffff, chip = u32(100);
        if (have_seq)
            ++seq_step[(seq - last_seq) & 0x7ffff];
        last_seq = seq;
        have_seq = true;
        const unsigned n = unsigned(r[16]) | unsigned(r[17]) << 8;
        auto &c = chips[chip];
        if (!c.replies)
            c.first = t;
        c.last = t;
        ++c.replies;
        long interval = -1;
        {
            const uint32_t cycles = u32(104);
            if (c.have_cycles) {
                const double ms = double(uint32_t(cycles - c.last_cycles)) / 64000.0;
                const long slots = long(ms / 6.0 + 0.5);
                interval = slots > 40 ? 40 : slots;
            }
            c.have_cycles = true;
            c.last_cycles = cycles;
        }
        const long k = long((t - t0) / bin);
        ++c.bin_replies[k];
        if (n < 3 || n > 80 || r[20] + 2u != n) {
            ++c.bad;
            continue;
        }
        const unsigned char *d = r + 20;
        unsigned o = 2, end = n - 1;
        bool any = false, this_cmd2 = false;
        while (o + 3 <= end) {
            const unsigned cmd = d[o + 1], len = d[o + 2];
            if (!cmd || o + 3 + len > end)
                break;
            any = true;
            ++c.commands[cmd];
            if (cmd == 2) {
                ++c.cmd2;
                this_cmd2 = true;
                ++c.body_len[len];
                const unsigned char *p = d + o + 3;
                std::vector<int> counters;
                if (len >= 3 && (p[0] & 0xf8) == 0xc8 && len == 3 + 16 * (p[0] & 3) + ((p[0] & 4) ? 6u : 0u)) {
                    for (unsigned i = 0; i < (p[0] & 3u); ++i)
                        counters.push_back(p[3 + 16 * i + 15]);
                    c.mag += (p[0] & 4) != 0;
                } else if (len % 18 == 0 || len % 18 == 6)
                    for (unsigned i = 0; i + 18 <= len; i += 18)
                        counters.push_back(p[i + 17]);
                for (int v : counters) {
                    if (c.last_counter >= 0) {
                        const int dlt = (v - c.last_counter + 256) % 256;
                        if (dlt >= 1 && dlt <= 127) {
                            c.gaps += unsigned(dlt - 1);
                            c.last_counter = v;
                            ++c.samples;
                            ++c.bin_samples[k];
                        }
                    } else {
                        c.last_counter = v;
                        ++c.samples;
                        ++c.bin_samples[k];
                    }
                }
            } else
                ++c.other;
            o += 3 + len;
        }
        if (!any) {
            ++c.empty;
            ++c.bin_empty[k];
        }
        if (interval >= 0)
            ++(this_cmd2 ? c.interval_data : c.interval_6ms)[interval];
    }
    const double span = tl - t0;
    std::printf("records %llu span %.1f s (%.1f/s)\n", (unsigned long long)total, span, total / (span > 0 ? span : 1));
    std::printf("module packet-counter step between consecutive ring records (1 = no packet missing in between):");
    unsigned shown = 0;
    for (auto &s : seq_step)
        if (shown++ < 14)
            std::printf(" %u:%llu", s.first, (unsigned long long)s.second);
    std::printf("\n");
    for (auto &p : chips) {
        auto &c = p.second;
        const double s = c.last - c.first > 0 ? c.last - c.first : 1;
        std::printf("chip %08x replies %llu (%.1f/s) empty %llu cmd2 %llu samples %llu (%.1f/s) counter_gaps %llu mag %llu bad %llu |",
                    p.first, (unsigned long long)c.replies, c.replies / s, (unsigned long long)c.empty,
                    (unsigned long long)c.cmd2, (unsigned long long)c.samples, c.samples / s, (unsigned long long)c.gaps,
                    (unsigned long long)c.mag, (unsigned long long)c.bad);
        for (auto &x : c.commands)
            std::printf(" c%u:%llu", x.first, (unsigned long long)x.second);
        std::printf(" | cmd2 len");
        for (auto &x : c.body_len)
            std::printf(" %u:%llu", x.first, (unsigned long long)x.second);
        std::printf("\n  reply interval in 6-ms slots (40 = >=40), replies without command 2:");
        for (auto &x : c.interval_6ms)
            std::printf(" %ld:%llu", x.first, (unsigned long long)x.second);
        std::printf("\n  reply interval in 6-ms slots, replies with command 2:");
        for (auto &x : c.interval_data)
            std::printf(" %ld:%llu", x.first, (unsigned long long)x.second);
        std::printf("\n  timeline (per %.0f s: replies/empty/samples):", bin);
        for (auto &x : c.bin_replies)
            std::printf(" [%ld] %llu/%llu/%llu", x.first, (unsigned long long)x.second,
                        (unsigned long long)c.bin_empty[x.first], (unsigned long long)c.bin_samples[x.first]);
        std::printf("\n");
    }
    return 0;
}
