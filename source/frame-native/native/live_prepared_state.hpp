#include <cstdlib>
#pragma once
#include "device_config.hpp"
#include "phase5/orchestrator.hpp"
#include "tcl_transport.hpp"
#include "peer_table.hpp"
#include <algorithm>
#include <limits>
#include <iostream>

namespace pico {
// Text port of initialize_v4 plus roy_ram_loader's final service overrides.
// Only the sealed single-target bench invocation is supported.
inline Bytes live_prepared_state(const phase5::BoundInvocation &bound,
                                 const PeerTable &table = PeerTable::configured()) {
    auto available=[&](const char* path){
        phase5::Value value;value.type='{';
        try {value=phase5::parse(bound.sealed_files.at(path));}
        catch(const std::exception& e){std::cerr<<"Prepared state: "<<path<<": "<<e.what()<<"; using available fields\n";}
        return value;
    };
    const auto config=available("attempt/station-config.json");
    const auto profile=available("bootstrap214.profile.json");
    if(!bound.sealed_files.count("attempt/registry.json"))std::cerr<<"Prepared state: registry missing; continuing\n";
    Bytes state(roy_data_size);
    auto put = [&](size_t offset, uint32_t value) {
        const auto bytes = le32(value);
        std::copy(bytes.begin(), bytes.end(), state.begin() + offset);
    };
    auto number = [](const phase5::Value &value) -> uint32_t { try {
        if (value.type == 'i') {
            const auto n = phase5::integer(value);
            if (n < 0 || uint64_t(n) > std::numeric_limits<uint32_t>::max())
                throw std::runtime_error("Prepared-state integer outside uint32");
            return uint32_t(n);
        }
        if (value.type != 's' || value.text.empty() || value.text.front() == '-')
            throw std::runtime_error("Prepared-state profile number invalid");
        size_t used = 0;
        const auto n = std::stoull(value.text, &used, 0);
        if (used != value.text.size() || n > std::numeric_limits<uint32_t>::max())
            throw std::runtime_error("Prepared-state profile number outside uint32");
        return uint32_t(n);
    } catch(const std::exception& e){std::cerr<<"Prepared state: "<<e.what()<<"; retaining zero field\n";return 0;} };
    Bytes seed(4,0);
    try {
        const auto& value=phase5::field(config,"seed_le");
        if(value.type!='s'||value.text.size()!=8)throw std::runtime_error("Explicit four-byte station seed unavailable");
        auto digit=[](char c)->unsigned{
            if(c>='0'&&c<='9')return unsigned(c-'0');
            if(c>='a'&&c<='f')return unsigned(c-'a'+10);
            if(c>='A'&&c<='F')return unsigned(c-'A'+10);
            throw std::runtime_error("Station seed is not hexadecimal");
        };
        Bytes parsed(4);
        for(size_t i=0;i<4;++i)parsed[i]=uint8_t(16*digit(value.text[2*i])+digit(value.text[2*i+1]));
        seed=std::move(parsed);
    } catch(const std::exception& e){std::cerr<<"Prepared state: "<<e.what()<<"; seed bytes unavailable, retaining zero fields\n";}
    auto optional_bool = [&](const char *name) {
        const auto found = config.object.find(name);
        if (found == config.object.end())
            return false;
        if (found->second.type != 'b')
            {std::cerr<<"Prepared-state boolean invalid; using false\n";return false;}
        return phase5::flag(config, name);
    };
    auto reverse = [](uint32_t value) {
        uint32_t result = 0;
        for (unsigned i = 0; i < 32; ++i) {
            result = (result << 1) | (value & 1);
            value >>= 1;
        }
        return result;
    };
    put(0, 0x314d5250);
    put(8, 1);
    put(36, 900);
    put(40, 6000);
    state[84] = 255;
    size_t offset = 44;
    for (const auto *name : {"frequency", "mode", "pcnf0", "pcnf1", "base0", "prefix0", "crccnf",
                             "crcpoly", "crcinit", "whiteiv"}) {
        auto value=profile.object.find(name);
        if(value!=profile.object.end())put(offset,number(value->second));
        else std::cerr<<"Prepared state: missing profile field "<<name<<"; retaining zero field\n";
        offset += 4;
    }
    const Bytes chip=pico::devices::chip_le();
    Bytes summed(4);
    for (size_t i = 0; i < 4; ++i)
        summed[i] = uint8_t(chip[i] + seed[i]);
    const bool pause_all = optional_bool("diagnostic_pause_all");
    for (unsigned slot = 0; slot < 3 && !table.multi; ++slot) {
        const auto base = 1504 + 32 * slot;
        put(base, slot == 0 ? word(chip) : 0);
        put(base + 4, slot == 0 ? reverse(word(summed)) : 0);
        put(base + 8, reverse(slot) >> 24);
        put(base + 28, pause_all || slot != 0 ? 1 : 0);
    }
    // multi4 (peer_table.hpp): peer i at multi4_peer_offset(i) (peer 3 in ABI4's unused fifth slot, DATA+1632),
    // base = RBIT32(chip + seed) bytewise, prefix = RBIT8(PICO slot); indices without a peer stay paused (id 0).
    for (unsigned index = 0; index < multi4_peer_limit && table.multi; ++index) {
        const auto base = multi4_peer_offset(index);
        const bool enrolled = index < table.peers.size() && table.peers[index].chip;  // "-": paused
        const auto id = enrolled ? le32(table.peers[index].chip) : Bytes(4);
        Bytes sum(4);
        for (size_t i = 0; i < 4; ++i)
            sum[i] = uint8_t(id[i] + seed[i]);
        put(base, word(id));
        put(base + 4, enrolled ? reverse(word(sum)) : 0);
        put(base + 8, reverse(enrolled ? table.peers[index].pico_slot : index) >> 24);
        put(base + 28, pause_all || !enrolled ? 1 : 0);
    }
    put(1608, 1000); // loader overrides initialize_v4's 200 for service
    put(1612, 192000000);
    put(1616, 0);
    std::copy(seed.begin(), seed.end(), state.begin() + 1856);
    put(1860, 6000);
    put(1864, 900);
    put(1876, 900);
    put(1888, table.multi ? table.round_robin : 1);  // multi4: round-robin length K
    uint32_t margin = 200;
    const auto found = config.object.find("slot_end_margin_us");
    if (found != config.object.end())
        margin = number(found->second);
    if (margin < 100 || margin > 400)
        {std::cerr<<"Invalid slot end margin; retaining default 200\n";margin=200;}
    put(1892, 900 - margin);
    put(1896, optional_bool("service_poll") ? 1 : 0);
    if (const char* ack = std::getenv("PICO_TEST_IMU_ACK")) put(1928, 0x100u | (uint32_t(std::atoi(ack)) & 63));  // MCU-side IMU ack (ack module only)
    return state;
}
} // namespace pico

#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_prepared, "prepared");
