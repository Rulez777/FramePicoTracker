#include "device_config.hpp"
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <chrono>
#include "guard_evidence.hpp"
#include "peer_engine.hpp"
#include "ota_trace.hpp"
#include "sensors.hpp"
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <iostream>
namespace pico {
namespace {
[[noreturn]] void fail(const char *s) { throw std::runtime_error(s); }
// Owner rule 2026-10-29: run-time checks log and continue; they never end a hardware run.
void soft(const std::string &s) {
    std::cerr << "peer_soft: " << s << std::endl;
    std::ofstream("/tmp/pico-diag.log", std::ios::app) << "peer_soft: " << s << std::endl;
}
Bytes pack(uint8_t seq, uint8_t command, const Bytes &body) {
    if (seq >= 64 || command == 0 || command >= 30 || body.size() > 255)
        fail("record body too large");
    Bytes b{seq, command, static_cast<uint8_t>(body.size())};
    b.insert(b.end(), body.begin(), body.end());
    unsigned sum = 0;
    for (auto c : b)
        sum += c;
    b.push_back(static_cast<uint8_t>(sum));
    return b;
}
bool prefix(const Bytes &b, uint8_t a, uint8_t c) {
    return b.size() >= 2 && b[0] == a && b[1] == c;
}
bool equal_at(const Bytes &b, size_t off, const Bytes &v) {
    return off <= b.size() && v.size() <= b.size() - off &&
           std::equal(v.begin(), v.end(), b.begin() + off);
}
void append(Bytes &a, const Bytes &b) { a.insert(a.end(), b.begin(), b.end()); }
} // namespace
PeerEngine::PeerEngine(Bytes c, int s, std::function<double()> n, std::function<Bytes()> rng)
    : now_(std::move(n)), nonce_(std::move(rng)), stock_mode_requested([] { const char *v = std::getenv("PICO_SET_STOCK_MODE"); return v && std::string(v) == "1"; }()), chip(std::move(c)), slot(s) {
    const char *mag = std::getenv("PICO_MAG_WORKMODE");
    mag_workmode_enabled_ = mag && std::string(mag) == "1";
    if (chip.size() != 4 ||
        !std::any_of(chip.begin(), chip.end(), [](uint8_t v) { return v != 0; }) || slot < 0 ||
        slot > 3 || !now_ || !nonce_)  // slot 3: multi4 peer 3
        fail("Expected a real LE32 identity and slot0..3 with explicit clock/nonce source");
}
Bytes PeerEngine::fresh_nonce() {
    auto b = nonce_();
    if (b.size() != 16)
        fail("Expected 16-byte nonce");
    return b;
}
void PeerEngine::remember(uint16_t tag, uint8_t s1) {
    if (!ack_tags_.count(tag))
        ack_order_.push_back(tag);
    ack_tags_[tag] = s1;
    if (ack_order_.size() > 32) {
        ack_tags_.erase(ack_order_.front());
        ack_order_.pop_front();
    }
}
void PeerEngine::clear_mode() {
    stock_mode_verified = false;
    stock_independent_acked = false;
    mode_nonce.reset();
    mode_profile.reset();
    mode_confirmed_at.reset();
    resume_.reset();
    mode_retry_after = 0;
    mode_set_target.reset();
    mode_switch_started.reset();
    mode_preparing = false;
}
void PeerEngine::disconnect() {
    imu_start_acked_ = false;
    if (mag_workmode_ != MagWorkMode::Idle && mag_workmode_ != MagWorkMode::Done) {
        mag_workmode_log("interrupted; continuing session, mode unknown", pending ? pending->sequence : sequences[17]);
        mag_workmode_ = MagWorkMode::Done;
    }
    sensor_nonce.reset();sensor_status=SensorStatus{};
    clear_mode();
    state = PeerState::Reconnecting;
    sequences.fill(0);
    pending.reset();
    configure_index = 0;
    last_reply.reset();
    last_sample.reset();
    ack_notification = false;
    phase_verified = false;
    product_info.reset();
    queued.clear();
    ack_tags_.clear();
    ack_order_.clear();
    sample_feedback_counter.reset();
    sample_feedback_enabled = false;
}
Mailbox PeerEngine::start() {
    std::optional<Bytes> early;
    if (state == PeerState::Absent && last_reply && 0 <= now() - *last_reply &&
        now() - *last_reply <= .5)
        early = product_info;
    clear_mode();
    phase_verified = false;
    product_info.reset();
    state = PeerState::Confirming;
    pending.reset();
    configure_index = 0;
    sequences.fill(0);
    next_generation();
    confirm_started = now();
    published_s1 = uint8_t(ack_notification ? 3 : 2);
    if (early) {
        product_info = early;
        confirmed();
    }
    return mailbox();
}
void PeerEngine::confirmed() {
    confirm_started.reset();
    sequences[9] = 1;
    state = PeerState::Configuring;
    pending.reset();
    if (mode_required)
        query_mode();
    else
        next_config();
}
void PeerEngine::next_config() {
    if (configure_index < configuration.size()) {
        const auto c = configuration[configure_index++];
        begin(c.first, c.second);
    } else
        state = PeerState::WaitSync;
}
uint32_t PeerEngine::next_generation() {
    generation += 2;
    if ((generation & 0xffff) == 0)
        generation += 2;
    return generation;
}
Mailbox PeerEngine::begin(uint8_t command, const Bytes &body) {
    if (command >= sequences.size())
        fail("Command sequence index out of range");
    const auto g = next_generation();
    const double stamp = now();
    Pending p;
    p.command = command;
    p.body = body;
    p.generation = g;
    p.sequence = sequences[command];
    p.first_sent = p.last_sent = stamp;
    p.s1 = ack_notification ? 3 : 2;
    p.attempts.push_back(g);
    pending = p;
    published_s1 = p.s1;
    return mailbox();
}
std::optional<uint32_t> PeerEngine::retime() {
    if (!pending)
        return {};
    auto &p = *pending;
    p.generation = next_generation();
    p.attempts.push_back(p.generation);
    if (p.attempts.size() > 8)
        p.attempts.pop_front();
    if (p.retimes < 3)
        p.first_sent = now();
    ++p.retimes;
    return p.generation;
}
Mailbox PeerEngine::mailbox() {
    const uint8_t s1 = blind_ack || ack_notification ? 3 : 2;
    if (!published_s1 || s1 != *published_s1) {
        published_s1 = s1;
        next_generation();
        if (pending) {
            pending->generation = generation;
            pending->attempts.push_back(generation);
            if (pending->attempts.size() > 8)
                pending->attempts.pop_front();
        }
    }
    remember(static_cast<uint16_t>(generation), s1);
    if (!pending)
        return {generation, s1, {0}};
    pending->s1 = s1;
    return {pending->generation, s1, pack(pending->sequence, pending->command, pending->body)};
}
bool PeerEngine::mode_verified() const {
    if (continuity_lease && mode_profile &&
        (state == PeerState::WaitSync || state == PeerState::Streaming))
        {pico::guard_evidence::emit(12,"mode-lease",true);return true;}
    const bool accepted=mode_profile && (*mode_profile == "stock" || *mode_profile == "frame") &&
           mode_confirmed_at && now() - *mode_confirmed_at < mode_lease_s;
    pico::guard_evidence::emit(12,"mode-lease",accepted);return accepted;
}
uint8_t PeerEngine::query_schema() const {
    return desired_mode ? 2 : (mode_query_schema ? mode_query_schema : 1);
}
void PeerEngine::query_mode() {
    if (stock_firmware) return;
    Resume previous{mode_profile, state, phase_verified};
    if (!mode_verified()) {
        auto started = mode_switch_started;
        clear_mode();
        mode_switch_started = started;
        phase_verified = false;
        state = PeerState::VerifyingMode;
    }
    resume_ = previous;
    queued.clear();
    mode_error.clear();
    mode_nonce = fresh_nonce();
    Bytes b{0x7e, query_schema()};
    append(b, *mode_nonce);
    begin(1, b);
}
void PeerEngine::request_mode(const std::string &profile) {
    if (stock_firmware) fail("Stock firmware has no remote mode setter");
    if ((profile != "stock" && profile != "frame") || !mode_required ||
        !mode_configurations.count(profile))
        fail("Remote mode requires explicit dual-mode configuration");
    if (pending || mode_set_target)
        fail("Command pipeline busy; do not replace an in-flight request");
    desired_mode = profile;
    mode_switch_started = now();
    query_mode();
}
void PeerEngine::set_mode() {
    phase_verified = false;
    mode_confirmed_at.reset();
    state = PeerState::VerifyingMode;
    queued.clear();
    if (!mode_switch_started)
        mode_switch_started = now();
    mode_set_target = desired_mode;
    mode_nonce = fresh_nonce();
    if (!desired_mode || mode_configurations.at(*desired_mode).empty() ||
        mode_configurations.at(*desired_mode)[0].second.empty())
        fail("Missing desired mode configuration");
    Bytes b{0x7d, 2};
    append(b, *mode_nonce);
    append(b, chip);
    b.push_back(*desired_mode == "frame");
    b.push_back(mode_configurations.at(*desired_mode)[0].second[0]);
    begin(1, b);
}
bool PeerEngine::accept_mode_set(const std::vector<Command> &commands) {
    if (!mode_set_target || !mode_nonce || !pending ||
        now() - pending->first_sent >= mode_query_timeout_s)
        return false;
    for (const auto &c : commands) {
        const auto &b = c.body;
        if (c.command == 1 && b.size() == 26 && prefix(b, 0xfd, 2) && equal_at(b, 2, *mode_nonce) &&
            equal_at(b, 18, chip) && b[22] <= 1 &&
            b[23] == static_cast<uint8_t>(*mode_set_target == "frame") && b[24] <= 2 &&
            b[25] == firmware_revision) {
            sequences[1] = (pending->sequence + 1) & 63;
            pending.reset();
            mode_set_target.reset();
            mode_nonce.reset();
            if (b[24] == 2) {
                mode_error = "Remote mode rejected: tracker OTA busy";
                disconnect();
                return true;
            }
            query_mode();
            return true;
        }
    }
    return false;
}
bool PeerEngine::accept_mode(const std::vector<Command> &commands) {
    if (mode_set_target || !mode_nonce || !pending || pending->command != 1 ||
        now() - pending->first_sent >= mode_query_timeout_s)
        return false;
    const bool previous_fresh = mode_verified();
    for (const auto &c : commands) {
        const auto &b = c.body;
        const auto schema = query_schema();
        const bool matched = c.command == 1 && b.size() == (schema == 2 ? 26u : 24u) && prefix(b, 0xfe, schema) &&
            equal_at(b, 2, *mode_nonce) && equal_at(b, 18, chip) && b[22] <= 1 &&
            (schema == 2 ? supported_wire_revision(b[23]) : b[23] == 1);
        if (c.command == 1) pico::guard_evidence::emit(12,"nonce-link",matched);
        if (matched) {
            if (schema == 2) {
                firmware_revision = b[23];
                std::ofstream("/tmp/pico-diag.log",std::ios::app)<<"sensor_decoder chip="<<std::hex<<word(chip)<<std::dec<<" verified_revision="<<unsigned(firmware_revision)<<" decoder="<<sensor_format_name(firmware_revision,stock_firmware)<<"\n";
            }
            const std::string profile = b[22] ? "frame" : "stock";
            if (required_query_profile && profile != *required_query_profile)
                soft("Dense Frame diagnostic received a non-Frame mode proof");
            const auto it = mode_configurations.find(profile);
            if (it == mode_configurations.end())
                return false;
            const auto config = it->second;
            if (config.empty() || config[0].second.empty())
                fail("Missing mode configuration");
            if (schema == 2 && n_readback)
                n_readback(config[0].second[0], b[24], b[25]);
            mode_profile = profile;
            mode_confirmed_at = pending->first_sent;
            mode_nonce.reset();
            mode_error.clear();
            sequences[1] = (pending->sequence + 1) & 63;
            pending.reset();
            if (schema == 2 && b[25] != 0) {
                phase_verified = false;
                state = PeerState::VerifyingMode;
                if (!mode_switch_started)
                    mode_switch_started = now();
                query_mode();
                return true;
            }
            if (schema == 2 && b[24] != config[0].second[0]) {
                phase_verified = false;
                state = PeerState::VerifyingMode;
                mode_preparing = true;
                if (!mode_switch_started)
                    mode_switch_started = now();
                begin(config[0].first, config[0].second);
                return true;
            }
            if (desired_mode && profile != *desired_mode) {
                set_mode();
                return true;
            }
            mode_switch_started.reset();
            if (stock_mode_requested && profile == "stock" && schema == 2) {
                soft("stock_mode verified mode=0 rev=" + std::to_string(b[23]) +
                     " pending=" + std::to_string(b[25]) + " N=" + std::to_string(b[24]));
                stock_mode_verified = true;
                resume_.reset();
                queued.clear();
                state = PeerState::WaitSync;
                phase_verified = false;
                begin(1, Bytes{0x31,0});
                soft("stock_mode independent off sent chip=" + std::to_string(word(chip)));
                return true;
            }
            const auto resume = resume_;
            resume_.reset();
            if (previous_fresh && resume && resume->profile && *resume->profile == profile &&
                (resume->state == PeerState::WaitSync || resume->state == PeerState::Streaming)) {
                state = resume->state;
                phase_verified = resume->phase;
            } else {
                phase_verified = false;
                configuration = config;
                configure_index = 0;
                state = PeerState::Configuring;
                next_config();
            }
            return true;
        }
    }
    return false;
}
bool PeerEngine::receive(const Bytes &payload, int s1, uint16_t tag, bool crc_valid) {
    if (!crc_valid || (s1 != 2 && s1 != 3)) {
        ++rejected;
        return false;
    }
    std::vector<Command> commands;
    try {
        commands = decode_records(payload);
    } catch (const std::exception &) {
        ++rejected;
        return false;
    }
    last_reply = now();
    if (ack_tags_.count(tag) && (ack_tags_.at(tag) & 1))
        ack_notification = false;
    const bool reliable = std::any_of(commands.begin(), commands.end(), [](const Command &c) {
        return c.command != 2 && c.command != 10 && c.command != 11;
    });
    if (reliable)
        ack_notification = true;
    const bool mode_accepted = accept_mode_set(commands) || accept_mode(commands);
    if(sensor_nonce) for(const auto& c:commands) {
        if(c.command!=1 || !sensor_status.accept(c.body,*sensor_nonce,chip,sensor_page,now())) continue;
        uint32_t id=0;for(unsigned k=0;k<4;++k)id|=uint32_t(chip[k])<<(8*k);
        char name[64];std::snprintf(name,sizeof(name),"/pico-tracker-status-%08X.txt",id);
        const std::string path=sensor_status_directory+name;
        const auto text=sensor_status.text(id,now());
        const std::string temporary=std::string(path)+".tmp";
        bool written=false;
        {std::ofstream f(temporary);f<<text;f.close();written=bool(f);}
        if(!written || std::rename(temporary.c_str(),path.c_str())!=0)
            std::cerr<<"sensor_status write failed: "<<path<<"\n";
        std::ofstream("/tmp/pico-diag.log",std::ios::app)<<"sensor_status "<<std::hex<<id<<std::dec<<" page="<<sensor_page<<" "<<text;
        if(pending && pending->command==1) {sequences[1]=(pending->sequence+1)&63;pending.reset();}
        sensor_nonce.reset();sensor_page=(sensor_page+1)%((sensor_status.mask&(1u<<3)) && sensor_status.raw[22]==0x13?10:8);sensor_due=now()+1;
    }
    for (const auto &c : commands) {
        if (c.command == 1 && c.body.size() == 35 && c.body[0] == 0x82)
            product_info = c.body;
        bool has_sample=false;
        if(c.command==2) try {has_sample=!decode_sensors(c.body,sensor_format_for_revision(firmware_revision,stock_firmware)).imu.empty();}catch(...){}
        if (has_sample) {
            last_sample = now();
            if ((phase_verified && (!mode_required || mode_verified())) ||
                (stock_firmware && product_info && !pending && configure_index == configuration.size() &&
                 (state == PeerState::WaitSync || state == PeerState::Streaming)))
                state = PeerState::Streaming;
        }
    }
    if (state == PeerState::Confirming && product_info) {
        confirmed();
        advance();
        return false;
    }
    if (mode_accepted)
        return false;
    const bool accepted = ack_tag(tag);
    if (reliable)
        ack_notification = true;
    return accepted;
}
bool PeerEngine::ack_tag(uint16_t tag) {
    if (native25_ack_bridge)
        return native25_ack_bridge(tag, [&] { return retire_ack_tag(tag); });
    return retire_ack_tag(tag);
}
bool PeerEngine::retire_ack_tag(uint16_t tag) {
    if(sensor_nonce && pending && pending->command==1)return false;
    last_reply = now();
    if (ack_tags_.count(tag) && (ack_tags_.at(tag) & 1))
        ack_notification = false;
    if (mode_required && (mode_nonce || !mode_verified()))
        return false;
    if (!pending || std::none_of(pending->attempts.begin(), pending->attempts.end(),
                                 [&](uint32_t g) { return uint16_t(g) == tag; }))
        return false;
    const auto p = *pending;
    if(stock_mode_requested && stock_mode_verified && p.command==1 && p.body==Bytes{0x31,0}) {
        stock_independent_acked=true;
        soft("stock_mode independent off ACK chip=" + std::to_string(word(chip)) +
             " seq=" + std::to_string(p.sequence) + " tag=" + std::to_string(tag));
    }
    if (p.command == 19 && now() - p.first_sent < command19_hold_s)
        return false;
    ++transport_acks;
    acked_generation = tag;
    sequences[p.command] = (p.sequence + 1) & 63;
    if (p.command == 19) imu_start_acked_ = true;
    if (p.command == 17 && mag_workmode_ == MagWorkMode::Wait09 && p.body == Bytes{9}) {
        mag_workmode_log("acked 09", p.sequence);
        mag_workmode_ = MagWorkMode::Delay;
        mag_workmode_due_ = now() + 1;
    } else if (p.command == 17 && mag_workmode_ == MagWorkMode::Wait07 && p.body == Bytes{7}) {
        mag_workmode_log("acked 07", p.sequence);
        mag_workmode_ = MagWorkMode::Done;
    }
    if (p.command == 10)
        phase_verified = true;
    pending.reset();
    if (mode_preparing) {
        mode_preparing = false;
        query_mode();
    } else if (state == PeerState::Configuring)
        next_config();
    advance();
    return true;
}
void PeerEngine::advance() {
    if (stock_mode_requested && stock_mode_verified) {
        // Keep this exact command pending until its attributed ACK. Missing
        // ACKs are logged; do not resume Frame configuration or guess sequence.
        if(pending && now()-pending->last_sent>=.05) {
            pending->last_sent=now();++pending->retries;++retry_count;
            if(pending->retries%60==0) soft("stock_mode independent off ACK pending chip="+std::to_string(word(chip)));
        }
        return;
    }
    const double stamp = now();
    if (pending && pending->command == 17 &&
        (mag_workmode_ == MagWorkMode::Wait09 || mag_workmode_ == MagWorkMode::Wait07) &&
        stamp - pending->first_sent >= 3) {
        mag_workmode_log("ack timeout; continuing session, mode unknown", pending->sequence);
        // Never guess the next expected sequence on an unacknowledged command.
        // Leave sequence[17] unchanged and do not repeat this experiment on reconnect.
        pending.reset();
        mag_workmode_ = MagWorkMode::Done;
    }
    if(sensor_nonce && stamp-sensor_sent>=1) {
        if(pending && pending->command==1) {sequences[1]=(pending->sequence+1)&63;pending.reset();}
        sensor_nonce.reset();sensor_page=(sensor_page+1)%((sensor_status.mask&(1u<<3)) && sensor_status.raw[22]==0x13?10:8);sensor_due=stamp+1; // rotate past missing pages
    }
    if (mode_switch_started && stamp - *mode_switch_started >= mode_switch_timeout_s) {
        mode_error = "Remote switch timed out; applied profile unknown, phase invalid";
        disconnect();
        return;
    }
    if (mode_required && mode_confirmed_at && !continuity_lease) {
        if (stamp - *mode_confirmed_at >= mode_lease_s) {
            std::ostringstream lease;
            lease << mode_lease_s;
            mode_error =
                "Mode proof expired after " + lease.str() + "s; cadence must be confirmed again";
            disconnect();
            return;
        }
        if (!pending && stamp - *mode_confirmed_at >= mode_refresh_s && stamp >= mode_retry_after) {
            query_mode();
            return;
        }
    }
    if (last_reply && stamp - *last_reply >= 3) {
        disconnect();
        return;
    }
    if (state == PeerState::Confirming && confirm_started && stamp - *confirm_started >= 3) {
        disconnect();
        return;
    }
    if (pending) {
        auto &p = *pending;
        if (mode_nonce && p.command == 1) {
            if (stamp - p.first_sent >= mode_query_timeout_s) {
                ++mode_query_failures;
                mode_error = "Mode 7e reply missing: unsupported firmware, wrong dual-mode ID "
                             "configuration, or radio loss";
                if (mode_verified()) {
                    sequences[1] = (p.sequence + 1) & 63;
                    pending.reset();
                    mode_nonce.reset();
                    resume_.reset();
                    mode_retry_after = stamp + mode_retry_backoff_s;
                } else
                    disconnect();
                return;
            }
        } else if (stamp - p.first_sent >= 3) {
            disconnect();
            return;
        }
        if (stamp - p.last_sent >= .05) {
            p.last_sent = stamp;
            ++p.retries;
            ++retry_count;
        }
        return;
    }
    if (native25_exclusive)
        return;
    if (!queued.empty()) {
        const auto c = queued.front();
        queued.pop_front();
        begin(c.first, c.second);
    } else if (mag_workmode_enabled_ && imu_start_acked_ && last_sample &&
               state == PeerState::Streaming && mode_profile == std::optional<std::string>("frame") &&
               mode_verified() && (mag_workmode_ == MagWorkMode::Idle ||
               (mag_workmode_ == MagWorkMode::Delay && stamp >= mag_workmode_due_))) {
        const uint8_t value = mag_workmode_ == MagWorkMode::Idle ? 9 : 7;
        mag_workmode_ = value == 9 ? MagWorkMode::Wait09 : MagWorkMode::Wait07;
        begin(17, {value}); // ordinary mailbox, retries, ACK attribution and per-command sequence
        mag_workmode_log(value == 9 ? "sent 09" : "sent 07", pending->sequence);
    } else if(firmware_revision==12 && mode_profile==std::optional<std::string>("frame") && state==PeerState::Streaming && mode_verified() && stamp>=sensor_due) {
        sensor_nonce=fresh_nonce();sensor_sent=stamp;
        Bytes request{0x7e,3};append(request,*sensor_nonce);request.push_back(uint8_t(sensor_page));begin(1,request);
    } else if (sample_feedback_enabled && sample_feedback_counter &&
               state == PeerState::Streaming && mode_verified()) {
        const auto counter = *sample_feedback_counter;
        sample_feedback_counter.reset();
        ++sample_feedback_publications;
        begin(11, {counter});
    }
}
void PeerEngine::mag_workmode_log(const std::string &event, uint8_t sequence) {
    std::ostringstream line;
    line << "mag_workmode " << event << " seq=" << unsigned(sequence)
         << " time=" << now() << " chip=" << std::hex << word(chip) << " slot=" << std::dec << slot;
    soft(line.str());
}
void PeerEngine::sample_feedback(int c) {
    if (c < 0 || c > 255)
        fail("Invalid measured sample counter");
    // 2026-10-30: with the MCU-side IMU ack (PICO_TEST_IMU_ACK, D1928) the module appends command 11 with the
    // newest counter to every streaming poll, after and over any host command 11. Publishing the host one too
    // rewrote the mailbox (~72 B, one more debugger round trip) on every radio iteration.
    static const bool mcu_ack = std::getenv("PICO_TEST_IMU_ACK") != nullptr;
    if (mcu_ack)
        return;
    if (!sample_feedback_enabled)
        return;
    sample_feedback_counter = static_cast<uint8_t>(c);
    advance();
}
void PeerEngine::queue_exposure_sync(int ticks, int count, bool verified, bool own, int reason) {
    if (stock_mode_requested) return;
    if (mode_required && !mode_verified())
        soft("Exposure sync before mode confirmation");
    if (ticks < 0 || ticks >= 533333 || count < 1 || count > 3) {
        soft("Exposure sync with invalid phase/count skipped");
        return;
    }
    if (!verified)
        soft("Exposure sync with unverified phase");
    if (reason < 0 || reason > 255)
        fail("Invalid light-sync reason");
    queued.erase(std::remove_if(queued.begin(), queued.end(),
                                [](const auto &c) { return c.first == 10 || c.first == 14; }),
                 queued.end());
    if (own)
        queued.push_back(
            {uint8_t(14), {1, static_cast<uint8_t>(count), static_cast<uint8_t>(reason)}});
    queued.push_back({uint8_t(10), le32(static_cast<uint32_t>(ticks))});
    advance();
}
bool PeerEngine::advance_initial_query() {
    if (mode_confirmed_at) {
        if (initial_recovery)
            initial_recovery->completed = true;
        return false;
    }
    if (initial_recovery && initial_recovery->completed)
        return false;
    if (chip != pico::devices::chip_le() || !mode_required ||
        desired_mode != std::optional<std::string>("frame") || mode_set_target ||
        mode_switch_started || state != PeerState::VerifyingMode || phase_verified || !pending ||
        pending->command != 1 || !prefix(pending->body, 0x7e, 2) || !mode_nonce)
        return false;
    if (mode_query_timeout_s != 8)
        fail("Initial mode recovery requires the sealed 8s query bound");
    if (!recovery_event)
        fail("Initial mode recovery requires durable event sink");
    if (!initial_recovery) {
        initial_recovery = Recovery{1, pending->first_sent, pending->first_sent + 24, false};
        recovery_event("initial_query", 1, pending->sequence, *mode_nonce);
    }
    const double stamp = now();
    if (stamp < pending->first_sent)
        fail("Initial mode recovery clock moved backwards");
    if (stamp - pending->first_sent < 8)
        return false;
    if (!last_reply || !(0 <= stamp - *last_reply && stamp - *last_reply < 3))
        return false;
    ++mode_query_failures;
    if (initial_recovery->attempts >= 3 || stamp >= initial_recovery->deadline) {
        mode_error =
            "Tracker mode unknown: no fresh 7e reply after " +
            std::to_string(initial_recovery->attempts) +
            " queries (24s maximum). No optical start or blind mode switch. Check charger/USB, "
            "power state and pairing; power-cycle and prepare a new run. Pairing is not diagnosed.";
        recovery_event("exhausted", initial_recovery->attempts, pending->sequence, *mode_nonce);
        soft(mode_error + " (logged; querying again)");
        mode_error.clear();
        mode_query_failures = 0;
        initial_recovery = Recovery{1, stamp, stamp + 24, false};
    }
    const auto previous = *mode_nonce;
    const auto sequence = pending->sequence;
    sequences[1] = (sequence + 1) & 63;
    pending.reset();
    mode_nonce.reset();
    query_mode();
    if (mode_nonce == previous)
        soft("Mode retry nonce did not change");
    ++initial_recovery->attempts;
    recovery_event("new_sequence_query", initial_recovery->attempts, pending->sequence,
                   *mode_nonce);
    return true;
}
void PeerEngine::configure(std::optional<uint8_t> n, bool omit, std::optional<uint8_t> imu,
                           uint8_t m) {
    if (!n)
        return;
    Configuration base;
    for (const auto &c : configuration)
        if (c.first != 16 && !(omit && c.first == 19))
            base.push_back(c.first == 17 && imu ? std::make_pair(uint8_t(17), Bytes{*imu}) : c);
    configuration = {{16, {*n, m}}};
    configuration.insert(configuration.end(), base.begin(), base.end());
}
bool MailboxWriter::publish(int slot, uint32_t generation, int s1, const Bytes &payload,
                            std::optional<PhaseReference> phase,
                            std::optional<CycleWindow> window) {
    if (slot < 0 || slot > 3 || (generation & 1) || (s1 != 2 && s1 != 3) || payload.empty() ||
        payload.size() > 56)
        fail("Invalid ABI4 mailbox");
    // 2026-10-31 hang hunt: PICO_TX_LOG=<file> journals every new mailbox (what the station sends to the tracker), with
    // CLOCK_MONOTONIC ns like radio.bin, so a tracker hang can be lined up with the exact command. Log only.
    if (const char *txPath = std::getenv("PICO_TX_LOG")) {
        static FILE *tx = std::fopen(txPath, "a");
        static Bytes lastPayload[4];
        static uint32_t lastGeneration[4] = {1, 1, 1, 1};
        if (tx && (payload != lastPayload[slot] || generation != lastGeneration[slot])) {
            timespec ts{};
#ifdef _WIN32
            const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count();
            ts.tv_sec = static_cast<time_t>(ns / 1000000000);
            ts.tv_nsec = static_cast<long>(ns % 1000000000);
#else
            clock_gettime(CLOCK_MONOTONIC, &ts);
#endif
            std::fprintf(tx, "%lld.%09ld slot %d gen %u s1 %d phase %d window %d len %zu:", (long long)ts.tv_sec, ts.tv_nsec,
                         slot, generation, s1, phase ? 1 : 0, window ? 1 : 0, payload.size());
            for (uint8_t b : payload) std::fprintf(tx, " %02x", b);
            std::fputc(10, tx);
            std::fflush(tx);
            lastPayload[slot] = payload;
            lastGeneration[slot] = generation;
        }
    }
    uint32_t meta = static_cast<uint32_t>(payload.size()) | (static_cast<uint32_t>(s1) << 8);
    Bytes data = payload;
    data.resize(56);
    auto put = [&](size_t off, uint32_t x) {
        auto b = le32(x);
        std::copy(b.begin(), b.end(), data.begin() + off);
    };
    if (phase) {
        if (payload.size() != 8 || payload[1] != 10 || payload[2] != 4)
            fail("Timed phase requires exactly command10");
        if (phase->ticks >= 533333)
            fail("Invalid timed phase reference");
        put(48, phase->cycles);
        put(52, phase->ticks);
        meta |= 1 << 16;
    }
    if (window) {
        if (payload.size() > 40)
            fail("Timed command exceeds mailbox timing reservation");
        uint32_t d = window->end - window->start;
        if (d == 0 || d >= 0x80000000)
            fail("Invalid command cycle window");
        put(40, window->start);
        put(44, window->end);
        meta |= 1 << 17;
    }
    const auto it = sent_.find(slot);
    if (it != sent_.end() && it->second.generation == generation && it->second.meta == meta &&
        it->second.bytes == data)
        return false;
    // Mailboxes 0..2 as ABI4; multi4 mailbox 3 lives in the free tail at DATA+1944 (peer_table.hpp).
    const auto a = roy_data + (slot < 3 ? 1664 + 64 * static_cast<uint32_t>(slot) : 1944u);
    Bytes body = le32(meta);
    append(body, data);
    debug_.write_many({{a, le32(generation | 1)}, {a + 4, body}, {a, le32(generation)}});
    ota_trace("mailbox slot="+std::to_string(slot)+" generation="+std::to_string(generation)+" s1="+std::to_string(s1)+" payload="+ota_trace_hex(payload));
    sent_[slot] = {generation, meta, data};
    return true;
}
bool LaterImuStart::bench_admission(PeerEngine &e, const BenchImuEvidence &b,
                                    const std::function<void()> &guard) {
    if (!b.forbid_controllers || b.min_controllers != 0 || b.guard_mode == "observe")
        soft("command19 with controllers allowed (joint bench)");
    if (guard)
        guard();
    if (e.mode_profile != std::optional<std::string>("frame") || !e.mode_verified())
        return false;  // wait for a fresh Frame mode proof
    const auto now = e.now();
    if (!e.last_reply || !(0 <= now - *e.last_reply && now - *e.last_reply < .25)) {
        reply_stable_.reset();
        return false;
    }
    if (!reply_stable_)
        reply_stable_ = now;
    if (now - *reply_stable_ < 3)
        return false;
    // Owner rule: the IMU must run from the start so it can carry the pose; do not wait for optics.
    return true;
}
bool LaterImuStart::prepare(PeerEngine &e, const BenchImuEvidence &b,
                            const std::function<void()> &guard,
                            const std::function<void(double)> &persist) {
    if (started_)
        return false;
    if (!bench_admission(e, b, guard))
        return false;
    if (!b.bootstrap_producer_verified || b.samples.size() != 2)
        soft("command19 without bootstrap producer evidence");
    const std::map<std::string, int> required = {
        {"irq_gate_00441", 1}, {"irq_gate_00fa7", 1}, {"irq_gate_01fb0", 1}};
    if (b.upstream != required)
        soft("command19 with upstream gates not all open");
    const double age = e.now() - b.producer_verified_at;
    if (age > 3)
        soft("command19 producer evidence age " + std::to_string(age) + " s");
    if (persist)
        persist(age);
    // Invoked only by the diagnostic dispatcher at tx19 with no pending work.
    if (e.pending)
        return false;  // retry when the pipeline is free
    return true;
}
bool LaterImuStart::start(PeerEngine &e, const BenchImuEvidence &b,
                          const std::function<void()> &guard,
                          const std::function<void(double)> &persist) {
    // 2026-10-31 hang localization: PICO_NO_IMU_START=1 never sends command 19 (no IMU stream, optics unchanged).
    if (std::getenv("PICO_NO_IMU_START")) { static bool said = false; if (!said) { said = true; soft("command19 suppressed by PICO_NO_IMU_START"); } return false; }
    if (!prepare(e, b, guard, persist))
        return false;
    e.sample_feedback_enabled = true;
    started_ = true;
    e.begin(19, {0});
    return true;
}
void verify_bootstrap_producer(BenchImuEvidence &b, double now) {
    if (b.samples.size() != 2 || b.samples[0].counter == b.samples[1].counter)
        soft("No bootstrap producer advancement");
    if (std::any_of(b.samples.begin(), b.samples.end(), [](const ProducerSample &x) {
            return x.mode != 7 || !x.enabled || !x.post_gate;
        }))
        soft("Bootstrap sensor disabled");
    if (b.upstream.empty() || std::any_of(b.upstream.begin(), b.upstream.end(),
                                          [](const auto &x) { return x.second != 1; }))
        soft("Bootstrap upstream gates not open");
    b.bootstrap_producer_verified = true;
    b.producer_verified_at = now;
}
} // namespace pico
