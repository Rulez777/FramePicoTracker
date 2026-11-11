#pragma once
#include "device_config.hpp"
// 2026-10-31 multi-tracker host (docs/MULTI_TRACKER_RADIO_2026-10-31.md "Host implementation").
// Enrollment table for the multi4 station module (outputs/multi-tracker-radio-20261031/roy_ram_station_v4_multi4.c).
// Configured only by PICO_PEERS; unset keeps the single-tracker path exactly as before (ackring8 layout, D1888=1).
//  PICO_PEERS        = CHIP[:PICO_SLOT[:SERIAL]] or -,...  module peer index = list position (0..3); "-" keeps that
//                      index paused (no tracker), so a tracker can keep its PICO slot as module index; index 0 must be the
//                      first target A1B2C3D4 (optics/identity stay bound to it). PICO_SLOT selects the prefix
//                      RBIT8(slot) as the PICO station used it (default: the index).
//  PICO_ROUND_ROBIN  = K, D(1888): slots per round (default: peer count). Indices count..K-1 are paused slots.
// Each tracker is polled every K x 6000 us, so its Frame wake divider is N = 2K-1:
// N*3000+2207 <= 6000K <= (N+1)*3000 (station_control.cpp window rule, owner rule 2026-10-30).
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace pico {
inline constexpr unsigned multi4_peer_limit = 4;
inline const uint32_t first_target_chip = pico::devices::chip();
inline bool stock_poll_compatible(double us) {
    return us>=9455 && std::abs(us/9455-std::round(us/9455))<1e-9;
}
struct PeerEntry {
    uint32_t chip = 0;
    unsigned pico_slot = 0;
    std::string serial;
    std::string profile{};
};
// Module index -> ABI4 peer slot: peers 0..2 as ABI4, peer 3 in the unused fifth slot (DATA+1632), multi4 PEER(i).
inline uint32_t multi4_abi_slot(unsigned index) { return index + ((index + 1) >> 2); }
inline uint32_t multi4_peer_offset(unsigned index) { return 1504 + 32 * multi4_abi_slot(index); }
// multi4 BOX(i): mailboxes 0..2 as ABI4, mailbox 3 in the free tail at DATA+1944.
inline uint32_t multi4_mailbox_offset(unsigned index) { return index < 3 ? 1664 + 64 * index : 1944; }
struct PeerTable {
    bool multi = false;  // PICO_PEERS given: multi4 layout, host DWT enable, per-peer N
    unsigned round_robin = 1;
    std::vector<PeerEntry> peers{{first_target_chip, 0, pico::devices::serial()}};
    unsigned tracker_n() const { return 2 * round_robin - 1; }
    double poll_period_us() const { return 6000.0 * round_robin; }
    void require_stock_timing() const {
        for(const auto &entry:peers)
            if(entry.chip && entry.profile=="stock" && !stock_poll_compatible(poll_period_us()))
                throw std::runtime_error("Stock N1 requires 9455-us wake-grid polling; multi4 6000*K is incompatible");
    }
    size_t enrolled() const {
        size_t n = 0;
        for (const auto &p : peers)
            n += p.chip != 0;
        return n;
    }
    std::array<uint32_t, multi4_peer_limit> chips() const {
        std::array<uint32_t, multi4_peer_limit> c{};
        for (size_t i = 0; i < peers.size() && i < c.size(); ++i)
            c[i] = peers[i].chip;
        return c;
    }
    // Returns the legacy table and sets error on any malformed field (owner rule: logged, never a run abort).
    static PeerTable parse(const char *list, const char *round_robin, std::string *error) {
        PeerTable t;
        if (!list)
            return t;
        auto bad = [&](const std::string &why) {
            if (error)
                *error = why;
            return PeerTable{};
        };
        PeerTable m;
        m.multi = true;
        m.peers.clear();
        std::stringstream all(list);
        std::string item;
        while (std::getline(all, item, ',')) {
            std::stringstream fields(item);
            std::string chip, slot, serial, profile, extra;
            std::getline(fields, chip, ':');
            std::getline(fields, slot, ':');
            std::getline(fields, serial, ':');
            std::getline(fields, profile, ':');
            if (std::getline(fields, extra, ':') ||
                (!profile.empty() && profile != "stock" && profile != "release-c2" && profile != "diagnostic7"))
                return bad("PICO_PEERS unknown profile or extra field");
            PeerEntry e;
            if (chip == "-" && slot.empty()) {  // paused placeholder
                e.pico_slot = unsigned(m.peers.size());
                m.peers.push_back(e);
                continue;
            }
            size_t used = 0;
            try {
                const auto v = std::stoul(chip, &used, 16);
                if (used != chip.size() || chip.size() != 8 || v == 0)
                    return bad("PICO_PEERS chip '" + chip + "' is not 8 hex digits");
                e.chip = uint32_t(v);
                e.pico_slot = unsigned(m.peers.size());
                if (!slot.empty()) {
                    const auto s = std::stoul(slot, &used, 10);
                    if (used != slot.size() || s > 3)
                        return bad("PICO_PEERS slot '" + slot + "' outside 0..3");
                    e.pico_slot = unsigned(s);
                }
            } catch (const std::exception &) {
                return bad("PICO_PEERS entry '" + item + "' unreadable");
            }
            e.serial = serial;
            e.profile = profile;
            for (const auto &p : m.peers)
                if (p.chip && p.chip == e.chip)
                    return bad("PICO_PEERS chip enrolled twice");
            m.peers.push_back(e);
        }
        if (m.peers.empty() || m.peers.size() > multi4_peer_limit)
            return bad("PICO_PEERS needs 1..4 peers");
        if (m.peers[0].chip != first_target_chip)
            return bad("PICO_PEERS index 0 must be the first target A1B2C3D4");
        if (m.peers[0].profile == "stock")
            return bad("Stock control peer must use a secondary module slot");
        if (m.peers[0].serial.empty())
            m.peers[0].serial = pico::devices::serial();
        m.round_robin = unsigned(m.peers.size());
        if (round_robin) {
            const std::string k = round_robin;
            if (k.size() != 1 || k[0] < '1' || k[0] > '4' || unsigned(k[0] - '0') < m.peers.size())
                return bad("PICO_ROUND_ROBIN must be peer count..4");
            m.round_robin = unsigned(k[0] - '0');
        }
        return m;
    }
    static const PeerTable &configured() {
        static const PeerTable table = [] {
            std::string error;
            auto t = parse(std::getenv("PICO_PEERS"), std::getenv("PICO_ROUND_ROBIN"), &error);
            std::ofstream log("/tmp/pico-diag.log", std::ios::app);
            if (!error.empty())
                log << "multi_peer: " << error << "; single-tracker path kept" << std::endl;
            else if (t.multi) {
                log << "multi_peer: multi4 table K=" << t.round_robin << " N=" << t.tracker_n()
                    << " period_us=" << t.poll_period_us();
                for (size_t i = 0; i < t.peers.size(); ++i)
                    log << " peer" << i << "=" << std::hex << t.peers[i].chip << std::dec << "@slot"
                        << t.peers[i].pico_slot << ":" << t.peers[i].serial;
                log << std::endl;
            }
            return t;
        }();
        return table;
    }
};
} // namespace pico
