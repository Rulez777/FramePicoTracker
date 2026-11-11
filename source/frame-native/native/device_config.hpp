#pragma once
// Device identities of THIS installation, read once from devices.conf (no IDs are compiled in).
// File: $PICO_DEVICES, else /home/steamos/pico-ram-work/devices.conf. Lines (# = comment):
//   tracker <CHIP_HEX> <PICO_SLOT> <SERIAL>     first tracker line = primary tracker (optics/identity target)
//   camera <ROLE> <NAME> [<V4L2 node>]          ROLE upper0|upper1|lower0|lower1, NAME = XRService camera name
//                                               (slam_left, slam_right, upper_left, upper_right) or factory serial
//   controller <SERIAL>
// Cameras default to the Steam Frame wiring (verified by image content): /dev/video0 slam_right, /dev/video3
// slam_left (upper, 1056x1024), /dev/video13 upper_left, /dev/video9 upper_right (side, 640x480).
// A missing or broken file is logged once and leaves zero/empty values (owner rule: log, never abort).
#include <array>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace pico::devices {

struct Tracker { uint32_t chip = 0; int slot = 0; std::string serial; };
struct Camera { std::string serial, node; };
struct Config {
    std::vector<Tracker> trackers;
    std::array<Camera, 2> upper{{{"slam_right", "/dev/video0"}, {"slam_left", "/dev/video3"}}};
    std::array<Camera, 2> lower{{{"upper_left", "/dev/video13"}, {"upper_right", "/dev/video9"}}};
    std::vector<std::string> controllers;
    std::string path;
};

inline std::string config_path() {
    if (const char *p = std::getenv("PICO_DEVICES")) return p;
    return "/home/steamos/pico-ram-work/devices.conf";
}

inline Config load() {
    Config c; c.path = config_path();
    std::ifstream in(c.path);
    if (!in) std::cerr << "devices: " << c.path << " missing; no tracker/camera identities (see START_HERE.md)\n";
    std::string line;
    while (std::getline(in, line)) {
        if (auto h = line.find('#'); h != std::string::npos) line.resize(h);
        std::istringstream f(line);
        std::string kind; if (!(f >> kind)) continue;
        if (kind == "tracker") {
            std::string chip; Tracker t;
            if (f >> chip >> t.slot >> t.serial) { t.chip = uint32_t(std::stoul(chip, nullptr, 16)); c.trackers.push_back(t); }
            else std::cerr << "devices: bad tracker line: " << line << "\n";
        } else if (kind == "camera") {
            std::string role, serial, node; f >> role >> serial >> node;
            Camera *cam = role == "upper0" ? &c.upper[0] : role == "upper1" ? &c.upper[1] :
                          role == "lower0" ? &c.lower[0] : role == "lower1" ? &c.lower[1] : nullptr;
            if (!cam) { std::cerr << "devices: bad camera role: " << line << "\n"; continue; }
            cam->serial = serial; if (!node.empty()) cam->node = node;
        } else if (kind == "controller") {
            std::string s; if (f >> s) c.controllers.push_back(s);
        } else std::cerr << "devices: unknown line: " << line << "\n";
    }
    if (c.trackers.empty()) std::cerr << "devices: no tracker line in " << c.path << "\n";
    return c;
}

inline const Config &get() { static const Config c = load(); return c; }

// Primary tracker (first tracker line).
inline uint32_t chip() { return get().trackers.empty() ? 0 : get().trackers[0].chip; }
inline std::string serial() { return get().trackers.empty() ? "" : get().trackers[0].serial; }
// Chip as transmitted on air (little-endian bytes), e.g. chip 11223344 -> {0x44,0x33,0x22,0x11}.
inline std::vector<uint8_t> chip_le(uint32_t c) { return {uint8_t(c), uint8_t(c >> 8), uint8_t(c >> 16), uint8_t(c >> 24)}; }
inline std::vector<uint8_t> chip_le() { return chip_le(chip()); }
inline std::string chip_le_hex(uint32_t c) {
    static const char *d = "0123456789abcdef"; std::string s;
    for (auto b : chip_le(c)) { s += d[b >> 4]; s += d[b & 15]; }
    return s;
}
inline std::string chip_le_hex() { return chip_le_hex(chip()); }
// PICO slot of a tracker by its on-air chip bytes; -1 if not listed.
inline int slot_of(const std::vector<uint8_t> &le) {
    for (const auto &t : get().trackers) if (chip_le(t.chip) == le) return t.slot;
    return -1;
}
inline const Tracker *tracker_in_slot(int slot) {
    for (const auto &t : get().trackers) if (t.slot == slot) return &t;
    return nullptr;
}
inline std::string upper(size_t i) { return get().upper[i].serial; }
inline std::string lower(size_t i) { return get().lower[i].serial; }
inline bool is_upper(const std::string &s) { return !s.empty() && (s == upper(0) || s == upper(1)); }

} // namespace pico::devices
