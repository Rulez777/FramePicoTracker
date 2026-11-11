#include <cstdlib>
#include "guard_evidence.hpp"
#include "query_peer.hpp"
#include "sensors.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
namespace pico {
namespace {
[[noreturn]] void fail(const char *s) { throw std::runtime_error(s); }
// Owner rule 2026-10-29: run-time checks log and continue/retry; they never end a hardware run.
void soft(const std::string &s) {
    std::cerr << "query_soft: " << s << std::endl;
    std::ofstream("/tmp/pico-diag.log", std::ios::app) << "query_soft: " << s << std::endl;
}
bool matches(const Bytes &b, unsigned schema, const Bytes &nonce, const Bytes &chip) {
    return b.size() >= 22 && b[0] == 0xfe && b[1] == schema &&
           std::equal(nonce.begin(), nonce.end(), b.begin() + 2) &&
           std::equal(chip.begin(), chip.end(), b.begin() + 18);
}
// Positive entries are schemas, negative entries name exact actions.
constexpr int plan[] = {2, 3, 4,   -17, 3,   4,   9,  5, 6, 7, 8, 10, 11,
                        6, 9, -10, -25, -14, -31, 11, 6, 8, 3, 4, 3,  -19};
// Release-firmware startup, not teardown. -31 sends command 1 / sub-command 0x31.
constexpr int release_plan[] = {2, -17, -10, -25, -14, -31, -19};
} // namespace
size_t QueryPeer::plan_size() const { return (firmware_revision == 11 || firmware_revision == 12) ? std::size(release_plan) : std::size(plan); }
int QueryPeer::plan_step(size_t index) const {
    return index < plan_size() ? ((firmware_revision == 11 || firmware_revision == 12) ? release_plan[index] : plan[index]) : 0;
}
size_t QueryPeer::step_position(int step, bool last) const {
    size_t found = plan_size();
    for (size_t i=0; i<plan_size(); ++i) if (plan_step(i)==step) {
        found=i; if (!last) break;
    }
    return found;
}
bool QueryPeer::diagnostics_ready() const {
    // The diagnostic bracket ends after IR31 and its legacy observer checks.
    return diagnostic_index >= ((firmware_revision == 11 || firmware_revision == 12) ? step_position(-19) : 22u);
}
Page parse_page(const Bytes &b, unsigned schema, const Bytes &nonce, const Bytes &chip) {
    if (nonce.size() != 16 || chip.size() != 4)
        fail("Invalid expected identity");
    if (!matches(b, schema, nonce, chip))
        fail("Stale nonce/wrong chip");
    if (schema == 2) {
        if (b.size() != 26 || b[22] > 1 || !supported_wire_revision(b[23]))
            fail("Invalid native mode proof");
        return {{"actual_radio_mode", b[22]},
                {"wire_revision", b[23]},
                {"actual_N", b[24]},
                {"native_N_pending", b[25]}};
    }
    if (schema == 4) {
        if (b.size() != 25)
            fail("Wrong schema/length");
        return {{"irq_gate_00441", b[22]}, {"irq_gate_00fa7", b[23]}, {"irq_gate_01fb0", b[24]}};
    }
    if (b.size() != 32)
        fail("Wrong schema/length");
    if (schema >= 12 && schema <= 17) {
        if(b[22]>1) fail("Invalid C2 source");
        return {{"page",schema},{"source",b[22]}};
    }
    if (schema == 3) {
        if (b[24] > 1)
            fail("Noncanonical enabled Boolean");
        const char *names[] = {"sensor_mode",        "sensor_identity",  "sensor_enabled",
                               "tx_permission",      "sample_counter",   "ring_latest",
                               "uart_flag",          "sensor_post_gate", "main_queue_producer",
                               "main_queue_consumer"};
        Page p;
        for (unsigned i = 0; i < 10; ++i)
            p[names[i]] = b[22 + i];
        return p;
    }
    if (schema == 5) {
        if (b[26] > 63)
            fail("Invalid command21 expected sequence");
        const char *names[] = {"command21_guard_count",
                               "timer_start_completed_count",
                               "fa7_clear_summary_count",
                               "fa7_set_completed_count",
                               "command21_expected_sequence",
                               "command10_guard_e08",
                               "fa7",
                               "sync_busy_f98",
                               "main_queue_producer",
                               "main_queue_consumer"};
        Page p;
        for (unsigned i = 0; i < 10; ++i)
            p[names[i]] = b[22 + i];
        return p;
    }
    if (schema == 6)
        return {{"timer_mode", b[22]},     {"timer_bitmode", b[23]}, {"timer_prescaler", b[24]},
                {"shorts_low8", b[25]},    {"cc2", word(b, 26)},     {"inten_compare_low8", b[30]},
                {"capture_channel", b[31]}};
    if (schema == 7)
        return {{"cc0", word(b, 22)},
                {"nominal_period", word(b, 26)},
                {"connected_e10", b[30]},
                {"pending_pairing_e48", b[31]}};
    if (schema == 8)
        return {{"channel3_eep", word(b, 22)},
                {"channel3_tep", word(b, 26)},
                {"chen_low8", b[30]},
                {"compare2_event_low8", b[31]}};
    if (schema == 9)
        return {{"event_sequence", word(b, 22)},
                {"accepted21_sequence", word(b, 26)},
                {"last_event_kind", b[30]},
                {"calibration_state_1f99", b[31]}};
    if (schema == 10)
        return {{"start_completed_sequence", word(b, 22)},
                {"stop_clear_sequence", word(b, 26)},
                {"fa7", b[30]},
                {"sensor_fb0", b[31]}};
    if (schema == 11)
        return {{"gate_set_sequence", word(b, 22)},
                {"other_clear_sequence", word(b, 26)},
                {"sensor_fcc", b[30]},
                {"fusion_skip_1ff6", b[31]}};
    fail("Invalid observer request");
}
QueryPeer::QueryPeer(Bytes c, int slot, std::function<double()> now, std::function<Bytes()> nonce,
                     QueryHooks hooks)
    : PeerEngine(std::move(c), slot, std::move(now), std::move(nonce)), hooks_(std::move(hooks)),
      independent_mode_([] {
          const char *v = std::getenv("PICO_INDEPENDENT_MODE");
          return uint8_t(v && std::string(v) == "0" ? 0 : 1);
      }()) {
    if (!hooks_.bench || !hooks_.fresh_guard || !hooks_.native_radio_tick || !hooks_.proof ||
        !hooks_.action || !hooks_.prerequisites || !hooks_.producer_lifetime)
        fail("Missing installed query/identity/evidence policy");
    configuration = {{16, {1, 2}}};
}
BenchImuEvidence QueryPeer::bench() {
    auto b = hooks_.bench();
    b.bootstrap_producer_verified = producer_.bootstrap_producer_verified;
    b.producer_verified_at = producer_.producer_verified_at;
    b.samples = producer_.samples;
    b.upstream = producer_.upstream;
    return b;
}
bool QueryPeer::bench_admission() {
    return bench_gate_.bench_admission(*this, bench(), hooks_.fresh_guard);
}
bool QueryPeer::ack_tag(uint16_t tag) {
    if (diagnostic_active_ && pending && pending->command == 1)
        return false;
    const bool independent = pending && pending->command == 1 &&
                             pending->body == Bytes({0x31, independent_mode_});
    const bool accepted = PeerEngine::ack_tag(tag);
    if (accepted && independent) {
        soft("independent_mode sent value=" + std::to_string(independent_mode_) + " acked");
        if (release_revision()) native_ir31_applied = true;
    }
    return accepted;
}
void QueryPeer::prerequisites() {
    // Native10 prerequisites are evidence only: a missing or odd page is logged and command10 still goes out.
    try { prerequisites_checked(); }
    catch (const std::exception &e) {
        soft(std::string("native10 prerequisites: ") + e.what());
        try { hooks_.prerequisites(observer_pages, std::string("prerequisites incomplete (logged): ") + e.what()); }
        catch (const std::exception &h) { soft(std::string("native10 prerequisites hook: ") + h.what()); }
    }
}
void QueryPeer::prerequisites_checked() {
    std::map<unsigned, Page> pages;
    std::vector<Page> timers, orders;
    for (const auto &row : observer_pages) {
        pages[row.first] = row.second;
        if (row.first == 6)
            timers.push_back(row.second);
        if (row.first == 9)
            orders.push_back(row.second);
    }
    if (timers.size() != 2 || orders.size() != 2)
        fail("Missing native timer/action bracket");
    if (orders[1].at("event_sequence") < orders[0].at("event_sequence"))
        fail("Native action observer reset");
    if (orders[1].at("accepted21_sequence") != orders[0].at("accepted21_sequence"))
        fail("Concurrent command21 changed initialization");
    for (const char *k : {"timer_mode", "timer_bitmode", "timer_prescaler", "inten_compare_low8",
                          "capture_channel"})
        if (timers[0].at(k) != timers[1].at(k))
            fail("Changed timer configuration");
    const auto &t = timers[1];
    if (t.at("timer_mode") != 0 || t.at("timer_bitmode") != 3 || t.at("timer_prescaler") != 0)
        fail("Unexpected TIMER1 clock config");
    std::string evidence =
        "Inferred from fresh native compare0 SHORTS toggling; no direct counter read";
    if (std::set<uint32_t>{timers[0].at("shorts_low8"), timers[1].at("shorts_low8")} !=
        std::set<uint32_t>{0, 1}) {
        const auto period = pages.at(7).at("nominal_period");
        if (timers[0].at("cc2") == t.at("cc2") || !(t.at("inten_compare_low8") & 4) ||
            std::any_of(timers.begin(), timers.end(), [&](const Page &p) {
                return p.at("shorts_low8") > 1 || !(475 < p.at("cc2") && p.at("cc2") < period);
            }))
            fail("No bounded fresh capture progression evidence; no10");
        evidence = "Inferred from changing CC2, enabled ADDRESS/CAPTURE2 and stable completed "
                   "START; no direct counter/IRQ read";
    }
    const auto &ppi = pages.at(8);
    const auto &guard = pages.at(5);
    const auto &actions = pages.at(10);
    if (guard.at("command10_guard_e08") != 0 || guard.at("sync_busy_f98") != 0)
        fail("Native10 E08/F98 guard closed");
    if (ppi.at("channel3_eep") != 0x40001104 || ppi.at("channel3_tep") != 0x40009048 ||
        !(ppi.at("chen_low8") & 8))
        fail("Capture PPI prerequisite absent");
    if (actions.at("start_completed_sequence") <= actions.at("stop_clear_sequence"))
        fail("Missing START-after-STOP signature");
    if (pages.at(7).at("nominal_period") == 0 || pages.at(7).at("cc0") == 0)
        fail("Missing actual timer period");
    std::vector<uint32_t> stamps = {
        orders[0].at("accepted21_sequence"), actions.at("start_completed_sequence"),
        actions.at("stop_clear_sequence"), pages.at(11).at("gate_set_sequence"),
        pages.at(11).at("other_clear_sequence")};
    std::set<uint32_t> nonzero;
    size_t n = 0;
    for (auto s : stamps)
        if (s) {
            ++n;
            nonzero.insert(s);
        }
    if (n != nonzero.size() ||
        *std::max_element(stamps.begin(), stamps.end()) > orders[1].at("event_sequence"))
        fail("Contradictory action stamps");
    hooks_.prerequisites(observer_pages, evidence);
}
void QueryPeer::advance() {
    if (stock_mode_requested) {
        if (!mode_error.empty()) { soft("stock_mode failure: " + mode_error); mode_error.clear(); }
        PeerEngine::advance();
        return;
    }
    const bool release = (firmware_revision == 11 || firmware_revision == 12);
    if (restart_plan_after_reconnect && disconnects != plan_disconnects_) {
        plan_disconnects_ = disconnects;
        if (release && diagnostic_index > 0)
            plan_restart_pending_ = true;
    }
    if (plan_restart_pending_ && (state == PeerState::WaitSync || state == PeerState::Streaming) && !pending &&
        !diagnostic_active_) {
        plan_restart_pending_ = false;
        ++plan_restarts;
        diagnostic_index = 0;
        diagnostic_due = now() + .37;
        page_failures_.clear();
        soft("link back after a loss; release plan restarted (N readback, 17/10/25/14/31, command 19), restart " +
             std::to_string(plan_restarts));
    }

    if (native25_ack_retiring) {
        PeerEngine::advance();
        return;
    }
    if (diagnostics_ready() && hooks_.native_radio_tick()) {
        PeerEngine::advance();
        return;
    }
    if (advance_initial_query())
        return;
    if (mode_query_failures && !mode_error.empty()) {
        soft(mode_error);
        mode_error.clear();
    }
    if (diagnostics_ready())
        bench_admission();
    // A lost or unanswered diagnostic query is re-sent with a fresh nonce instead of ending the run.
    auto retry_step = [&](const char *why) {
        if (diagnostic_active_ && diagnostic_active_->schema >= 12) {
            soft("breadcrumb unavailable; configuration continues");
            std::ofstream("/tmp/pico-diag.log", std::ios::app)
                << "breadcrumb chip=" << std::hex << word(chip, 0) << " available=0" << std::endl;
            diagnostic_active_.reset();
            if (pending && pending->command == 1) sequences[1] = (pending->sequence + 1) & 63;
            pending.reset();
            breadcrumb_done_ = true;
            return;
        }
        const size_t failed = diagnostic_index ? diagnostic_index - 1 : 0;
        const bool mandatory = plan_step(failed)==2;
        const bool exhausted = ++page_failures_[failed] >= 2 && !mandatory;
        soft(std::string(why) + " at step " + std::to_string(failed) +
             (exhausted ? "; unavailable, continuing" : mandatory ?
              "; mandatory N readback unavailable, retrying" : "; retrying once"));
        if (pending && pending->command == 1) sequences[1] = (pending->sequence + 1) & 63;
        pending.reset();
        diagnostic_active_.reset();
        if (!exhausted && diagnostic_index > 0)
            --diagnostic_index;
        diagnostic_due = now() + .37;
    };
    if (diagnostic_active_) {
        if (!pending || pending->command != 1) {
            retry_step("Pending application proof lost");
            return;
        }
        if (now() - diagnostic_active_->sent >= 8) {
            retry_step("8s application deadline");
            return;
        }
        if (now() - pending->last_sent >= .05) {
            pending->last_sent = now();
            ++pending->retries;
            ++retry_count;
        }
        return;
    }
    PeerEngine::advance();
    if (stock_firmware)
        return;
    if (diagnostic_active_) {
        if (now() - diagnostic_active_->sent >= 8)
            retry_step("Diagnostic application reply deadline expired");
        return;
    }
    const auto index = diagnostic_index;
    if (pending ||
        (state != PeerState::WaitSync && state != PeerState::Streaming) || now() < diagnostic_due)
        return;
    if (release && index >= plan_size() && breadcrumb_capable && !breadcrumb_done_) {
        const unsigned schema = 12 + breadcrumb_step_ % 6;
        const auto nonce = nonce_bytes();
        diagnostic_active_ = Active{schema, nonce, now()};
        Bytes body{0x7e, static_cast<uint8_t>(schema)};
        body.insert(body.end(), nonce.begin(), nonce.end());
        begin(1, body);
        return;
    }
    if (index >= plan_size()) return;
    if (plan_step(index) == 2 && !bootstrap_n1_requested) {
        if (mode_profile != std::optional<std::string>("frame") || !mode_verified())
            return;  // wait for a fresh Frame mode proof
        bootstrap_n1_requested = true;
        continuity_lease = true;
        if (bootstrap_n) { begin(16, {*bootstrap_n, 2}); return; }  // multi4: N = 2K-1, read back below
        { const char* tn=std::getenv("PICO_TEST_N"); begin(16, {uint8_t(tn?std::atoi(tn):1), 2}); } // PICO_TEST_N: tracker wake divider experiment
        return;
    }
    const int schema = plan_step(index);
    if (diagnostics_ready() && !bench_admission())
        return;
    if (schema < 0) {
        if (schema == -17 && !producer_.samples.empty() && producer_.samples.back().mode == 7 &&
            producer_.samples.back().enabled) {
            diagnostic_index = index + 1;
            diagnostic_due = now() + .37;
            return;
        }
        if (schema == -10) {
            if (!release) prerequisites();
            producer_.samples.clear();
            producer_.upstream.clear();
        }
        uint8_t command = 0;
        Bytes body;
        if (schema == -19) {
            // start repeats bench_admission just as the tx19 reference branch.
            if (!release && !bench_gate_.prepare(*this, bench(), hooks_.fresh_guard, hooks_.producer_lifetime))
                return;
            sample_feedback_enabled = true;
            command = 19;
            body = {0};
        } else {
            if (schema == -10) {
                command = 10;
                body = Bytes(4);
            }
            if (schema == -17) {
                command = 17;
                body = {7};
            }
            if (schema == -25) {
                command = 25;
                body = {2};
            }
            if (schema == -14) {
                command = 14;
                body = {1, 1, 0};
            }
            if (schema == -31) {
                command = 1;
                body = {0x31, independent_mode_};
            }
        }
        diagnostic_index = index + 1;
        diagnostic_due = now() + 1.2;
            hooks_.action(command, body);
        begin(command, body);
        return;
    }
    const auto nonce = nonce_bytes();
    diagnostic_active_ = Active{static_cast<unsigned>(schema), nonce, now()};
    diagnostic_index = index + 1;
    Bytes body{0x7e, static_cast<uint8_t>(schema)};
    body.insert(body.end(), nonce.begin(), nonce.end());
    begin(1, body);
}
bool QueryPeer::receive(const Bytes &payload, int s1, uint16_t tag, bool crc) {
    const auto active = diagnostic_active_;
    if (active && crc) {
        for (const auto &record : decode_records(payload)) {
            const auto &body = record.body;
            const auto schema = active->schema;
            if (record.command != 1) continue;
            const bool matched=matches(body,schema,active->nonce,chip);
            pico::guard_evidence::emit(12,"nonce-link",matched);
            if(!matched) continue;
            Page parsed;
            try { parsed = parse_page(body, schema, active->nonce, chip); }
            catch (const std::exception &e) { soft(std::string("diagnostic page dropped: ") + e.what()); continue; }
            const auto &fields = parsed;
            if (schema >= 12) {
                // C2: source at 22, nine bytes at 23..31. Read all six
                // pages twice; reject mixed sources and any changing payload.
                if (breadcrumb_step_ == 0) breadcrumb_source_ = body[22];
                breadcrumb_coherent_ &= body[22] == breadcrumb_source_;
                auto &assembled = breadcrumb_step_ < 6 ? breadcrumb_first_ : breadcrumb_second_;
                assembled.insert(assembled.end(), body.begin()+23, body.end());
                if (breadcrumb_step_ == 11) {
                    breadcrumb_done_ = true;
                    const auto &part = breadcrumb_first_;
                    if (breadcrumb_coherent_ && part == breadcrumb_second_ &&
                        part.size() == 54 && word(part,0) == 0x42435231) {
                        breadcrumb = Page{{"source",breadcrumb_source_},
                            {"sequence",word(part,4)}, {"event",part[8]}, {"command",part[9]},
                            {"stage",part[10]}, {"primask",part[11]},
                            {"resetreas",word(part,12)}, {"reason",word(part,16)}};
                        for (unsigned i=20;i<54;++i)
                            (*breadcrumb)["tail_"+std::to_string(i-20)] = part[i];
                    }
                    std::ofstream("/tmp/pico-diag.log",std::ios::app)
                        << "breadcrumb source=" << (breadcrumb_source_ ? "frozen" : "live")
                        << " coherent=" << bool(breadcrumb) << std::endl;
                }
                ++breadcrumb_step_;
            }
            if (schema == 2) {
                firmware_revision = body[23];
                std::ofstream("/tmp/pico-diag.log",std::ios::app)<<"sensor_decoder chip="<<std::hex<<word(chip)<<std::dec<<" verified_revision="<<unsigned(firmware_revision)<<" decoder="<<sensor_format_name(firmware_revision,stock_firmware)<<"\n";
                if (body[22] == 1) {
                    mode_profile = "frame";
                    mode_confirmed_at = now();
                }
                if (n_readback)
                    n_readback(bootstrap_n ? *bootstrap_n : 1, body[24], body[25]);
                const bool unexpected = body[22] != 1 || !supported_wire_revision(body[23]) || body[25] > 1 ||
                    (bootstrap_n ? !(body[24] == *bootstrap_n || (body[25] == 1 && bootstrap_n1_requested))
                                 : !(body[24] == 1 || std::getenv("PICO_TEST_N") || (body[24] == 2 && body[25] == 1 && bootstrap_n1_requested)));
                if (unexpected)
                    soft("Unexpected mode/N/pending state; querying again");
                if (body[25] || unexpected) {
                    if (!settle_started_)
                        settle_started_ = now();
                    ++settle_queries_;
                    if (now() - *settle_started_ >= 8 || settle_queries_ >= 8) {
                        soft("Native16 did not settle within 8 s/8 queries; still querying");
                        settle_started_ = now();
                        settle_queries_ = 0;
                    }
                    diagnostic_index = step_position(2);
                    diagnostic_due = now() + .37;
                    unsettled_ = true;
                } else
                    unsettled_ = false;
                if (!unsettled_)
                    continuity_lease = true;
            }
            hooks_.proof(schema, body, fields);
            if (!release_revision() && schema == 11 && diagnostic_index == step_position(-31) + 2) {
                if (fields.at("fusion_skip_1ff6") != independent_mode_)
                    soft("Native IR31 flag not applied; continuing to optics");
                native_ir31_applied = true;
            }
            if (schema >= 5)
                observer_pages.push_back({schema, fields});
            if (schema == 3) {
                producer_.samples.push_back(
                    {fields.at("sample_counter"), static_cast<int>(fields.at("sensor_mode")),
                     fields.at("sensor_enabled") != 0, fields.at("sensor_post_gate") != 0});
                if (!release_revision() && diagnostic_index == step_position(3, true) + 1)
                    verify_bootstrap_producer(producer_, now());
            }
            if (schema == 4) {
                producer_.upstream.clear();
                for (const auto &entry : fields)
                    producer_.upstream[entry.first] = static_cast<int>(entry.second);
            }
            diagnostic_active_.reset();
            Bytes expected{0x7e, static_cast<uint8_t>(schema)};
            expected.insert(expected.end(), active->nonce.begin(), active->nonce.end());
            if (pending && pending->command == 1 && pending->body == expected) {
                sequences[1] = (pending->sequence + 1) & 63;
                pending.reset();
            }
            diagnostic_due = now() + ((unsettled_ || diagnostic_index % 2) ? .37 : .83);
            break;
        }
    }
    return PeerEngine::receive(payload, s1, tag, crc);
}
} // namespace pico

#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_query, "query");
