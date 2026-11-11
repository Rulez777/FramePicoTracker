#pragma once
#include "roy_owner.hpp"
#include "imu_slot.hpp"
#include <algorithm>
#include <sstream>
#include <map>
namespace pico {
// Deterministic TCL endpoint. No socket, process, clock or hardware access.
// Actual DebugMemory codecs run against this endpoint in all Phase 2 tests.
struct FakeOpenOcd {
    std::map<uint32_t, uint8_t> memory;
    std::vector<std::string> transcript;
    int64_t ns = 0;
    size_t operations = 0, fail_at = 0;
    bool fail_after = false, slot_stuck = false, close_stuck = false, hook_stuck = false;
    bool active_after_close = false, corrupt_read = false;
    uint32_t corrupt_address = 0;
    std::string console;
    unsigned invocations = 0;
    Bytes read(uint32_t a, size_t n) {
        Bytes b(n);
        for (size_t i = 0; i < n; ++i)
            b[i] = memory[a + static_cast<uint32_t>(i)];
        return b;
    }
    void put(uint32_t a, const Bytes &b) {
        for (size_t i = 0; i < b.size(); ++i)
            memory[a + static_cast<uint32_t>(i)] = b[i];
    }
    void set(uint32_t a, uint32_t v) { put(a, le32(v)); }
    void fault(bool after) {
        if (fail_at == operations && after == fail_after)
            throw std::runtime_error("injected TCL failure");
    }
    void execute_console() {
        ++invocations;
        if (console == "0\n") {
            memory[roy_mpsl + 0x26] = 1;
            set(roy_mpsl + 32, roy_code | 1);
            set(roy_static, 0x00030200);
            set(0xe000ed94, 5);
            memory[roy_data + 84] = 0;
            set(roy_data + 28, slot_stuck ? 2 : 1);
        } else if (console == "2\n" && !close_stuck) {
            memory[roy_mpsl + 0x26] = active_after_close ? 1 : 0;
            set(roy_data + 28, 0);
        }
        if (!hook_stuck)
            set(roy_hook, 0);
        console.clear();
    }
    // DebugMemory::ring_batch's server-side Tcl, executed natively with the same plan.
    std::string ring_batch(const std::string &c) {
        auto has = [&](const std::string &s) { return c.find(s) != std::string::npos; };
        std::string out;
        auto words = [&](uint32_t a, size_t n) {
            for (size_t i = 0; i < n; ++i)
                out += std::to_string(word(read(a + static_cast<uint32_t>(4 * i), 4))) + " ";
        };
        if (has("set pico_o [read_memory 0xe0001004 32 1]"))
            words(0xe0001004, 1);
        const uint32_t w = word(read(roy_data + 112, 4)), r = word(read(roy_data + 116, 4)),
                       chip = word(read(roy_data + 12, 4));
        words(roy_data, 4);
        words(roy_data + 28, 1);
        words(roy_data + 112, 2);
        // Short IMU rings (State.enabled flag, imu_slot.hpp): one 12-word read per 48-byte slot, as ring_script.
        if (const uint32_t depth = imu_ring_depth_for(word(read(roy_data + 8, 4)))) {
            if (w > r)
                for (uint32_t i = w - r > depth ? w - depth : r; i != w; ++i)
                    words(roy_data + 380 + (i % depth) * imu_ring_stride_for(word(read(roy_data+8,4))), imu_ring_stride_for(word(read(roy_data+8,4)))/4);
        } else {
            for (uint32_t i = w > r ? (w - r > 10 ? w - 10 : r) : w; i != w; ++i) {
                const uint32_t slot = roy_data + 380 + (i % 10) * 112;
                words(slot, 5);
                words(slot + 20, record_dma_words(word(read(slot + 16, 4))));
                words(slot + 100, 3);
            }
        }
        const std::string extra = "set pico_o [concat $pico_o [read_memory 0x";
        for (auto at = c.find(extra); at != std::string::npos; at = c.find(extra, at + 1)) {
            std::istringstream in(c.substr(at + extra.size() - 2));
            std::string a, count;
            unsigned bits;
            in >> a >> bits >> count;
            if (!count.empty() && count[0] != '[')  // ring reads use computed operands
                words(static_cast<uint32_t>(std::stoul(a, nullptr, 0)), std::stoul(count));
        }
        if (w != r)
            set(roy_data + 116, w);
        if (has("write_memory 0x20011260 32"))
            set(roy_data + 1608, chip + 1000);
        return out;
    }
    std::string command(const std::string &c) {
        ++operations;
        transcript.push_back(c);
        fault(false);
        if (c.rfind("set pico_o {}", 0) == 0) {
            auto result = ring_batch(c);
            fault(true);
            return result;
        }
        std::string result;
        if (c.rfind("concat ", 0) == 0) {
            size_t start = 0;
            while ((start = c.find('[', start)) != std::string::npos) {
                const auto end = c.find(']', start);
                if (end == std::string::npos)
                    throw std::runtime_error("fake concat syntax");
                std::istringstream in(c.substr(start + 1, end - start - 1));
                std::string verb, a;
                unsigned bits;
                size_t count;
                in >> verb >> a >> bits >> count;
                if (verb != "read_memory" || bits != 32)
                    throw std::runtime_error("fake concat read");
                const auto addr = static_cast<uint32_t>(std::stoul(a, nullptr, 0));
                for (size_t i = 0; i < count; ++i)
                    result +=
                        std::to_string(word(read(addr + static_cast<uint32_t>(i * 4), 4))) + " ";
                start = end + 1;
            }
            fault(true);
            return result;
        }
        // DebugMemory's write_many is a sequence of complete TCL writes.
        size_t begin = 0;
        do {
            const auto end = c.find("; ", begin);
            const auto part = c.substr(begin, end == std::string::npos ? end : end - begin);
            std::istringstream in(part);
            std::string verb, a;
            in >> verb >> a;
            if (verb == "read_memory") {
                unsigned bits;
                size_t count;
                in >> bits >> count;
                if (bits != 32)
                    throw std::runtime_error("fake read width");
                auto addr = static_cast<uint32_t>(std::stoul(a, nullptr, 0));
                for (size_t i = 0; i < count; ++i) {
                    uint32_t v = word(read(addr + static_cast<uint32_t>(4 * i), 4));
                    if (corrupt_read && addr == corrupt_address && i == 0)
                        v ^= 1;
                    result += std::to_string(v) + " ";
                }
            } else if (verb == "write_memory") {
                unsigned bits;
                in >> bits;
                char brace;
                in >> brace;
                if (bits != 32 || brace != '{')
                    throw std::runtime_error("fake write syntax");
                auto addr = static_cast<uint32_t>(std::stoul(a, nullptr, 0));
                std::string v;
                while (in >> v) {
                    if (v == "}")
                        break;
                    if (v.back() == '}')
                        v.pop_back();
                    if (v.empty())
                        break;
                    set(addr, static_cast<uint32_t>(std::stoul(v, nullptr, 0)));
                    addr += 4;
                }
            } else if (verb == "mww" || verb == "mwb") {
                std::string v;
                in >> v;
                const auto addr = static_cast<uint32_t>(std::stoul(a, nullptr, 0));
                const auto value = static_cast<uint32_t>(std::stoul(v, nullptr, 0));
                if (verb == "mww") {
                    set(addr, value);
                    if (addr == roy_rtt + 108)
                        execute_console();
                } else {
                    memory[addr] = static_cast<uint8_t>(value);
                    if (addr >= 0x20012020 && addr < 0x20012060)
                        console += static_cast<char>(value);
                }
            } else
                throw std::runtime_error("Unsupported fake TCL command: " + part);
            if (end == std::string::npos)
                break;
            begin = end + 2;
        } while (true);
        fault(true);
        return result;
    }
    OwnerIO io(DebugMemory &d) {
        return {[&d](uint32_t a, size_t n) { return d.read(a, n); },
                [&d](uint32_t a, const Bytes &b) { d.write(a, b); },
                [this](const std::string &c) { return command(c); }, [this] { return ns; },
                [this](int64_t n) { ns += n; }};
    }
    void stock(const Bytes &image) {
        for (size_t o :
             {size_t(0), size_t(0x10580), size_t(0x18948), size_t(0x189be), image.size() - 64})
            put(static_cast<uint32_t>(o), Bytes(image.begin() + o, image.begin() + o + 64));
        put(roy_code, Bytes(2044, 0xa5));
        put(roy_data, Bytes(2048, 0x5a));
        put(roy_rtt, Bytes{'S', 'E', 'G', 'G', 'E', 'R', ' ', 'R', 'T', 'T'});
        put(roy_rtt + 16, Bytes{3, 0, 0, 0, 3, 0, 0, 0});
        set(roy_static, 0x00020200);
        set(0xe0001000, 0x76543210);
        set(0xe000edfc, 0x10203040);
        set(0xe000ed94, 5);
        put(roy_rtt + 24, Bytes(24));
        set(roy_rtt + 28, 0x20012060);
        set(roy_rtt + 32, 4096);
        put(roy_rtt + 96, Bytes(24));
        set(roy_rtt + 100, 0x20012020);
        set(roy_rtt + 104, 64);
        set(roy_rtt + 112, 63);
    }
};
} // namespace pico
