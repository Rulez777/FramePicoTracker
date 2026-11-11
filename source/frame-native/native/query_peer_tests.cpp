#include "native_test_runtime.hpp"
#include "fake_openocd.hpp"
#include "query_peer.hpp"
#include "station_control.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
using namespace pico;
namespace {
size_t checks = 0;
void independent_option(const char *v) {
#ifdef _WIN32
    _putenv_s("PICO_INDEPENDENT_MODE", v);
#else
    if (*v) setenv("PICO_INDEPENDENT_MODE", v, 1);
    else unsetenv("PICO_INDEPENDENT_MODE");
#endif
}
void check(bool b, int line = __builtin_LINE()) {
    ++checks;
    if (!b)
        throw std::runtime_error("query check " + std::to_string(checks) + " line " + std::to_string(line));
}
template <class F> void rejects(F f, int line = __builtin_LINE()) {
    bool b = false;
    try {
        f();
    } catch (const std::exception &) {
        b = true;
    }
    check(b, line);
}
void set(Bytes &b, size_t off, uint32_t v) {
    const auto x = le32(v);
    std::copy(x.begin(), x.end(), b.begin() + off);
}
Bytes packet(const Bytes &body) {
    Bytes p{0, 1, static_cast<uint8_t>(body.size())};
    p.insert(p.end(), body.begin(), body.end());
    unsigned sum = 0;
    for (auto v : p)
        sum += v;
    p.push_back(static_cast<uint8_t>(sum));
    return p;
}
struct Fixture {
    FakeOpenOcd f;
    DebugMemory debug;
    MailboxWriter writer;
    double t = 0;
    uint8_t nonce = 0, counter = 0;
    unsigned timers = 0, orders = 0, proofs = 0, guards = 0, prereq = 0, lifetime = 0;
    int sensor_mode = 0;
    bool optical = true, active_ledger = false;
    bool torn_breadcrumb = false, mixed_source = false;
    uint8_t source = 1;
    uint8_t independent_gate = 1;
    unsigned independent_writes = 0;
    unsigned breadcrumb_reads = 0;
    std::vector<std::pair<uint8_t, Bytes>> actions;
    std::vector<std::string> sequence;
    std::string prereq_reason;
    QueryPeer e;
    Fixture()
        : debug([this](const std::string &c) { return f.command(c); }), writer(debug),
          e(
              {0xd4, 0xc3, 0xb2, 0xa1}, 0, [this] { return t; },
              [this] { return Bytes(16, ++nonce); },
              {[this] {
                   BenchImuEvidence b;
                   b.forbid_controllers = true;
                   b.min_controllers = 0;
                   b.guard_mode = "strict";
                   b.temporal_identity_verified = optical;
                   b.phase = "locked";
                   b.pose_valid = optical;
                   return b;
               },
               [this] { ++guards; }, [this] { return active_ledger; },
               [this](unsigned, const Bytes &, const Page &) {
                   ++proofs;
                   sequence.push_back("proof");
               },
               [this](uint8_t c, const Bytes &b) {
                   actions.push_back({c, b});
                   sequence.push_back("action");
                   if (c == 17)
                       sensor_mode = 7;
               },
               [this](const std::vector<std::pair<unsigned, Page>> &, const std::string &reason) {
                   ++prereq;
                   prereq_reason = reason;
                   check(reason.find("Inferred") == 0 ||
                         reason.find("prerequisites incomplete (logged)") == 0);
               },
               [this](double age) {
                   ++lifetime;
                   check(age <= 3);
                   sequence.push_back("lifetime");
               }}) {
        configure_dual_mode(e, 1, 2, true);
        e.mode_profile = "frame";
        e.mode_confirmed_at = 0;
        e.state = PeerState::WaitSync;
        e.continuity_lease = true;
        e.recovery_event = [](const std::string &, unsigned, uint8_t, const Bytes &) {};
    }
    Bytes reply(unsigned schema, const Bytes &n) {
        Bytes b{0xfe, static_cast<uint8_t>(schema)};
        b.insert(b.end(), n.begin(), n.end());
        b.insert(b.end(), e.chip.begin(), e.chip.end());
        b.resize(schema == 2 ? 26 : schema == 4 ? 25 : 32);
        if (schema == 2) {
            b[22] = 1;
            b[23] = e.firmware_revision;
            b[24] = 1;
        }
        if (schema == 3) {
            b[22] = static_cast<uint8_t>(sensor_mode);
            b[24] = sensor_mode == 7;
            b[26] = ++counter;
            b[29] = 1;
        }
        if (schema >= 12 && schema <= 17) {
            if (schema == 12) ++breadcrumb_reads;
            Bytes data(54);
            set(data,0,0x42435231);
            set(data,4,torn_breadcrumb ? breadcrumb_reads : 10);
            data[8]=2; data[9]=19; data[10]=20; data[11]=1;
            set(data,12,4); set(data,16,2);
            for(unsigned i=20;i<54;++i)data[i]=uint8_t(i);
            b[22]=mixed_source && schema==14 ? uint8_t(1-source) : source;
            std::copy_n(data.begin()+(schema-12)*9,9,b.begin()+23);
        }
        if (schema == 4)
            b[22] = b[23] = b[24] = 1;
        if (schema == 6) {
            b[23] = 3;
            b[25] = static_cast<uint8_t>(timers++ % 2);
            set(b, 26, 500 + timers);
            b[30] = 4;
            b[31] = 2;
        }
        if (schema == 7) {
            set(b, 22, 100);
            set(b, 26, 533333);
        }
        if (schema == 8) {
            set(b, 22, 0x40001104);
            set(b, 26, 0x40009048);
            b[30] = 8;
        }
        if (schema == 9) {
            set(b, 22, 10 + 10 * orders++);
            set(b, 26, 3);
        }
        if (schema == 10) {
            set(b, 22, 2);
            set(b, 26, 1);
        }
        if (schema == 11) {
            set(b, 22, 4);
            set(b, 26, 5);
            b[31] = independent_gate;
        }
        return b;
    }
    bool step() {
        e.last_reply = t;
        e.advance();
        if (e.pending) {
            const auto p = *e.pending;
            const auto m = e.mailbox();
            writer.publish(0, m.generation, m.s1, m.payload);
            if (e.active_query()) {
                check(!e.ack_tag(static_cast<uint16_t>(p.generation)));
                check(e.pending.has_value());
                const auto schema = p.body[1];
                const Bytes n(p.body.begin() + 2, p.body.end());
                e.receive(packet(reply(schema, n)), 3, static_cast<uint16_t>(p.generation), true);
            } else {
                if (p.command == 1 && p.body.size() == 2 && p.body[0] == 0x31) {
                    // Fake tracker consumes the actual wire record, including explicit zero.
                    const auto records = decode_records(m.payload);
                    check(records.size() == 1 && records[0].command == 1 && records[0].body == p.body);
                    independent_gate = records[0].body[1];
                    ++independent_writes;
                    check(!e.ack_tag(static_cast<uint16_t>(p.generation + 100)));
                    check(e.pending.has_value());
                }
                e.ack_tag(static_cast<uint16_t>(p.generation));
            }
        }
        t = std::max(t + .1, e.diagnostic_due);
        return e.diagnostic_index == 26 && !e.pending;
    }
};
} // namespace
int main() {
    try {
        independent_option("");
        for (const char *option : {"", "0", "1", "invalid"}) {
            for (uint8_t revision : {uint8_t(7), uint8_t(11), uint8_t(12)}) {
                independent_option(option);
                const uint8_t expected = std::string(option) == "0" ? 0 : 1;
                Fixture x;
                x.e.firmware_revision = revision;
                // Selection is frozen per peer, including when environment later changes.
                independent_option(expected ? "0" : "1");
                std::ostringstream log;
                auto *previous = std::cerr.rdbuf(log.rdbuf());
                try {
                    const size_t end = revision == 7 ? 26 : 7;
                    for (unsigned i=0; i<200 && (x.e.diagnostic_index<end || x.e.pending); ++i) x.step();
                } catch (...) { std::cerr.rdbuf(previous); throw; }
                std::cerr.rdbuf(previous);
                check(x.independent_writes == 1 && x.independent_gate == expected);
                check(x.e.native_ir31_applied && x.e.sample_feedback_enabled && !x.e.pending);
                check(x.actions == Configuration({{17,{7}}, {10,Bytes(4)}, {25,{2}},
                      {14,{1,1,0}}, {1,{0x31,expected}}, {19,{0}}}));
                const std::string message = "independent_mode sent value=" + std::to_string(expected) + " acked";
                check(log.str().find(message) != std::string::npos);
                check(log.str().find(message, log.str().find(message)+1) == std::string::npos);
                check(log.str().find("flag not applied") == std::string::npos);
            }
        }
        independent_option("");
        {
            Fixture off;
            check(!off.e.stock_mode_requested && off.e.desired_mode == "frame");
#ifdef _WIN32
            _putenv_s("PICO_SET_STOCK_MODE", "1");
#else
            setenv("PICO_SET_STOCK_MODE", "1", 1);
#endif
            Fixture x;
#ifdef _WIN32
            _putenv_s("PICO_SET_STOCK_MODE", "");
#else
            unsetenv("PICO_SET_STOCK_MODE");
#endif
            check(x.e.stock_mode_requested && x.e.desired_mode == "stock");
            x.e.query_mode();
            auto reply = [&](uint8_t mode, uint8_t n, uint8_t pending) {
                auto b = x.reply(2, *x.e.mode_nonce);
                b[22]=mode; b[23]=12; b[24]=n; b[25]=pending;
                x.e.receive(packet(b), 2, 0, true);
            };
            reply(1,2,0);
            check(x.e.pending && x.e.pending->body[0]==0x7d);
            check(x.e.pending->body[22]==0 && x.e.pending->body[23]==1);
            const auto setter_nonce=*x.e.mode_nonce;
            Bytes fd{0xfd,2};
            fd.insert(fd.end(),setter_nonce.begin(),setter_nonce.end());
            fd.insert(fd.end(),x.e.chip.begin(),x.e.chip.end());
            fd.insert(fd.end(),{1,0,0,12});
            x.e.receive(packet(fd),2,0,true);
            check(x.e.pending && x.e.pending->body[0]==0x7e && x.e.pending->body[1]==2);
            check(*x.e.mode_nonce != setter_nonce && !x.e.stock_mode_verified);
            reply(0,1,1);
            check(!x.e.stock_mode_verified);
            auto stale=x.reply(2,setter_nonce); stale[22]=0; stale[23]=12; stale[24]=1;
            x.e.receive(packet(stale),2,0,true);
            check(!x.e.stock_mode_verified);
            reply(0,1,0);
            check(x.e.stock_mode_verified && x.e.pending && x.e.mode_profile=="stock");
            check(x.e.pending->command==1 && x.e.pending->body==Bytes({0x31,0}));
            check(!x.e.stock_independent_acked);
            auto generation=uint16_t(x.e.pending->generation);
            check(!x.e.ack_tag(uint16_t(generation+100)) && !x.e.stock_independent_acked);
            check(x.e.ack_tag(generation));
            check(x.e.stock_independent_acked && !x.e.pending);
            for(unsigned i=0;i<400;++i){x.t+=.5;x.e.advance();}
            x.e.queue_exposure_sync(1,1,true,true);
            check(!x.e.pending && x.e.queued.empty() && x.actions.empty());
            check(x.e.desired_mode=="stock" && x.e.diagnostic_index==0);
        }
        {
            Fixture x;
            bool done = false;
            for (unsigned i = 0; i < 200 && !done; ++i)
                done = x.step();
            check(done);
            check(x.actions == Configuration({{17, {7}},
                                              {10, {0, 0, 0, 0}},
                                              {25, {2}},
                                              {14, {1, 1, 0}},
                                              {1, {0x31, 1}},
                                              {19, {0}}}));
            check(x.e.native_ir31_applied && x.e.sample_feedback_enabled && x.prereq == 1 &&
                  x.lifetime == 1 && x.guards > 3);
            check(x.sequence[x.sequence.size() - 2] == "lifetime" && x.sequence.back() == "action");
            check(x.proofs == 20);
            check(!x.f.transcript.empty());
        }
        {
            Fixture x;
            x.sensor_mode = 7;
            bool done = false;
            for (unsigned i = 0; i < 200 && !done; ++i)
                done = x.step();
            check(done);
            check(std::none_of(x.actions.begin(), x.actions.end(),
                               [](const auto &c) { return c.first == 17; }));
        }
        for(uint8_t revision:{uint8_t(11),uint8_t(12)}) {
            Fixture x;
            x.e.firmware_revision = revision;
            for (unsigned i = 0; i < 180 && (x.e.diagnostic_index < 7 || x.e.pending); ++i)
                x.step();
            check(x.e.diagnostic_index == 7 && !x.e.pending);
            check(x.proofs == 1 && x.prereq == 0 && x.lifetime == 0);
            check(x.actions == Configuration({{17,{7}}, {10,Bytes(4)}, {25,{2}},
                                              {14,{1,1,0}}, {1,{0x31,1}}, {19,{0}}}));
        }
        for (uint8_t source : {uint8_t(0),uint8_t(1)}) for (bool torn : {false,true}) {
            Fixture x;
            x.e.firmware_revision = 11;
            x.e.breadcrumb_capable = true;
            x.e.chip = {0xe2,0xc3,0xb2,0xa1};
            x.torn_breadcrumb = torn; x.source=source;
            for (unsigned i=0;i<180 && x.proofs<13;++i) x.step();
            check(x.e.diagnostic_index==7 && x.e.native_ir31_applied);
            check(x.breadcrumb_reads==2 && x.proofs==13);
            check(x.e.breadcrumb.has_value()==!torn);
            if(!torn) {
                check(x.e.breadcrumb->at("stage")==20 && x.e.breadcrumb->at("resetreas")==4);
                check(x.e.breadcrumb->at("source")==source && x.e.breadcrumb->at("command")==19);
                check(x.e.breadcrumb->at("tail_33")==53);
            }
            for(unsigned i=0;i<10;++i) x.step();
            check(x.breadcrumb_reads==2);
        }
        {
            Fixture x;
            x.e.firmware_revision=11;
            x.e.breadcrumb_capable=true;
            for(unsigned i=0;i<180 && x.e.diagnostic_index<7;++i) x.step();
            x.e.advance();
            check(x.e.active_query() && x.e.pending->body[1]==12);
            x.t+=8;
            x.e.last_reply=x.t;
            x.e.advance();
            check(!x.e.active_query() && !x.e.pending);
            for(unsigned i=0;i<180 && (x.e.diagnostic_index<7 || x.e.pending);++i) x.step();
            check(x.e.diagnostic_index==7 && x.breadcrumb_reads==0);
        }
        {
            Fixture x;x.e.firmware_revision=11;x.e.breadcrumb_capable=true;x.mixed_source=true;
            for(unsigned i=0;i<180 && x.proofs<13;++i)x.step();
            check(x.proofs==13 && !x.e.breadcrumb);
        }
        // Each observer schema has independently derived LE field assertions and
        // nonce/chip/length negative vectors, without executing the Python codec.
        for (unsigned schema = 2; schema <= 17; ++schema) {
            Fixture x;
            const Bytes nonce(16, 0x81);
            auto body = x.reply(schema, nonce);
            const auto p = parse_page(body, schema, nonce, x.e.chip);
            check(!p.empty());
            if (schema == 6)
                check(p.at("cc2") == 501 && p.at("timer_bitmode") == 3);
            if (schema == 8)
                check(p.at("channel3_eep") == 0x40001104 && p.at("channel3_tep") == 0x40009048);
            if(schema>=12) {
                check(p.at("source")==1 && p.at("page")==schema);
                auto invalid=body;invalid[22]=2;
                rejects([&]{parse_page(invalid,schema,nonce,x.e.chip);});
                invalid[22]=0x31; // old release-C magic in byte 22
                rejects([&]{parse_page(invalid,schema,nonce,x.e.chip);});
            }
            auto bad = body;
            bad[2] ^= 1;
            rejects([&] { parse_page(bad, schema, nonce, x.e.chip); });
            bad = body;
            bad[18] ^= 1;
            rejects([&] { parse_page(bad, schema, nonce, x.e.chip); });
            body.push_back(0);
            rejects([&] { parse_page(body, schema, nonce, x.e.chip); });
        }
        {
            Fixture x;
            Bytes n(16);
            auto b = x.reply(3, n);
            b[24] = 2;
            rejects([&] { parse_page(b, 3, n, x.e.chip); });
            b = x.reply(5, n);
            b[26] = 64;
            rejects([&] { parse_page(b, 5, n, x.e.chip); });
        }
        // An application query cannot be retired by MCU/transport ACK alone.
        {
            Fixture x;
            x.e.bootstrap_n1_requested = true;
            x.e.advance();
            check(x.e.active_query());
            const auto g = x.e.pending->generation;
            check(!x.e.ack_tag(static_cast<uint16_t>(g)));
            x.t = 8;
            x.e.advance();  // 8 s deadline retries the query instead of ending the run
            check(!x.e.pending && !x.e.active_query());
        }
        // Bounded native N settling: eight fresh proofs, no command16 resends.
        {
            Fixture x;
            x.e.bootstrap_n1_requested = true;
            for (unsigned i = 0; i < 8; ++i) {
                x.e.last_reply = x.t;
                x.e.advance();
                check(x.e.active_query());
                const auto pending = *x.e.pending;
                auto b = x.reply(2, Bytes(pending.body.begin() + 2, pending.body.end()));
                b[24] = 2;
                b[25] = 1;
                // The eighth unsettled proof is logged; the settle window restarts.
                x.e.receive(packet(b), 3, static_cast<uint16_t>(pending.generation), true);
                check(!x.e.active_query() && x.e.diagnostic_index == 0);
                x.t += .4;
            }
            check(x.actions.empty());
            x.e.last_reply = x.t;
            x.e.advance();
            check(x.e.active_query() && x.e.pending->body[1] == 2);  // still querying
        }
        // Owner rule: the IMU starts without waiting for optics; missing optics is not a gate.
        {
            Fixture x;
            x.optical = false;
            bool done = false;
            for (unsigned i = 0; i < 200 && !done; ++i)
                done = x.step();
            check(done && x.e.sample_feedback_enabled && x.lifetime == 1);
            check(x.actions.back() == std::make_pair(uint8_t(19), Bytes{0}));
        }
        // Alter each prerequisite in the retained pages just before native10.
        for (unsigned reason = 0; reason < 10; ++reason) {
            Fixture x;
            for (unsigned i = 0; i < 100 && x.e.diagnostic_index < 15; ++i)
                x.step();
            check(x.e.diagnostic_index == 15);
            auto find = [&](unsigned schema) -> Page & {
                for (auto &row : x.e.observer_pages)
                    if (row.first == schema)
                        return row.second;
                throw std::runtime_error("missing page");
            };
            if (reason == 0)
                find(6)["timer_prescaler"] = 1;
            if (reason == 1)
                find(5)["command10_guard_e08"] = 1;
            if (reason == 2)
                find(5)["sync_busy_f98"] = 1;
            if (reason == 3)
                find(8)["channel3_eep"] = 0;
            if (reason == 4)
                find(8)["chen_low8"] = 0;
            if (reason == 5)
                find(7)["nominal_period"] = 0;
            if (reason == 6)
                find(7)["cc0"] = 0;
            if (reason == 7)
                find(10)["start_completed_sequence"] = 1;
            if (reason == 8)
                find(11)["gate_set_sequence"] = 3;
            if (reason == 9)
                find(11)["other_clear_sequence"] = 100;
            x.t = x.e.diagnostic_due;
            x.e.last_reply = x.t;
            // Native10 prerequisites are evidence only: logged, command10 still goes out.
            x.e.advance();
            check(x.prereq == 1 && x.prereq_reason.find("prerequisites incomplete (logged)") == 0);
            check(x.actions.back() == std::make_pair(uint8_t(10), Bytes(4)));
            check(x.e.pending && x.e.pending->command == 10 && x.e.diagnostic_index == 16);
            check(!x.e.sample_feedback_enabled);
        }
        // Three independent fake trackers through discovery, fresh mode query,
        // N write/readback, complete dialog, command19 and real IMU decoding.
        // None starts with an injected mode proof or WaitSync state.
        {
            Fixture a,b,c;
            Fixture *peers[]={&a,&b,&c};
            const unsigned revisions[]={7,11,0};
            unsigned queries[3]={}, starts[3]={}, samples[3]={};
            uint8_t tracker_n[3]={1,1,1}, tracker_mode[3]={0,0,0};
            unsigned setters[3]={};
            for(unsigned k=0;k<3;++k) {
                auto &x=*peers[k];
                x.e.disconnect(); x.e.chip=le32(0xA1B2C3D4 + k); x.e.slot=int(k);
                x.e.firmware_revision=7; x.e.bootstrap_n=5;
                if(k==2) {
                    x.e.stock_firmware=true; x.e.mode_required=false;
                    x.e.desired_mode.reset(); x.e.mode_configurations.clear();
                    x.e.configuration={{16,{1,2}},{17,{7}},{19,{0}}};
                } else configure_frame_peer(x.e,5,18000,true);
                x.e.start();
                Bytes info(35); info[0]=0x82;
                x.e.receive(packet(info),2,uint16_t(x.e.generation),true);
                check(x.e.product_info.has_value());
            }
            for(unsigned tick=0;tick<1400;++tick) for(unsigned k=0;k<3;++k) {
                auto &x=*peers[k]; x.t+=.05; x.e.last_reply=x.t; x.e.advance();
                if(x.e.pending) {
                    const auto p=*x.e.pending; const auto box=x.e.mailbox();
                    x.writer.publish(int(k),box.generation,box.s1,box.payload);
                    if(p.command==1 && p.body.size()==18 && p.body[0]==0x7e) {
                        ++queries[k]; const unsigned schema=p.body[1];
                        check(k!=2 && (k!=1 || schema==2));
                        Bytes nonce(p.body.begin()+2,p.body.end());
                        auto page=x.reply(schema,nonce);
                        if(schema==2) {page[22]=tracker_mode[k];page[23]=uint8_t(revisions[k]);page[24]=tracker_n[k];}
                        // A different peer's reply must not satisfy this query.
                        auto wrong=page; wrong[18]^=0x40;
                        x.e.receive(packet(wrong),3,uint16_t(box.generation),true);
                        check(x.e.pending.has_value());
                        x.e.receive(packet(page),3,uint16_t(box.generation),true);
                    } else if(p.command==1 && p.body.size()==24 && p.body[0]==0x7d) {
                        ++setters[k];tracker_mode[k]=p.body[22];tracker_n[k]=p.body[23];
                        Bytes reply{0xfd,2};reply.insert(reply.end(),p.body.begin()+2,p.body.begin()+22);
                        reply.insert(reply.end(),{tracker_mode[k],tracker_mode[k],0,uint8_t(revisions[k])});
                        x.e.receive(packet(reply),3,uint16_t(box.generation),true);
                    } else {
                        if(p.command==16)tracker_n[k]=p.body[0];
                        if(p.command==19)++starts[k];
                        x.e.ack_tag(uint16_t(box.generation));
                    }
                }
                if(starts[k] && !x.e.pending) {
                    Bytes imu(18); imu[17]=uint8_t(tick);
                    Bytes record{0,2,18};record.insert(record.end(),imu.begin(),imu.end());
                    unsigned sum=0;for(auto v:record)sum+=v;
                    record.push_back(uint8_t(sum));
                    check(decode_imu(imu).size()==1);
                    x.e.receive(record,2,uint16_t(x.e.generation),true);++samples[k];
                }
            }
            for(unsigned k=0;k<3;++k) {
                check(starts[k]==1 && samples[k]>0 && peers[k]->e.state==PeerState::Streaming);
                check(k==2 ? queries[k]==0 : queries[k]>0);
                if(k!=2)check(peers[k]->e.firmware_revision==revisions[k] && tracker_n[k]==5 && setters[k]==1);
            }
        }
        // Mandatory N readback must survive repeated timeouts, unlike observers.
        {
            Fixture x; x.e.bootstrap_n1_requested=true;
            for(unsigned attempt=0;attempt<4;++attempt) {
                x.e.last_reply=x.t;x.e.advance();
                check(x.e.active_query() && x.e.pending && x.e.pending->body[1]==2);
                x.t+=8.01;x.e.last_reply=x.t;x.e.advance();x.t+=.4;
                check(x.e.diagnostic_index==0 && !x.e.active_query());
                check(x.actions.empty());
            }
        }
        // Missing legacy observer: at most two application windows, then continue.
        {
            Fixture x; x.e.bootstrap_n1_requested=true; x.e.diagnostic_index=1;
            for(unsigned attempt=0;attempt<2;++attempt) {
                x.e.last_reply=x.t; x.e.advance();check(x.e.active_query());
                x.t+=8.01;x.e.last_reply=x.t;x.e.advance();
                x.t+=.4;
            }
            check(!x.e.active_query() && !x.e.pending && x.e.diagnostic_index==2);
            x.e.last_reply=x.t;x.e.advance();
            check(x.e.pending && x.e.pending->body[1]==4);
        }
        std::cout << checks << " installed query policy checks PASS (synthetic plan, fake TCL)\n";
        return 0;
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}
