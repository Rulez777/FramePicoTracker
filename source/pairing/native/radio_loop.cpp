#include <cstdio>
#include <cstdlib>
#include "guard_evidence.hpp"
#include "imu_slot.hpp"
#include "peer_table.hpp"
#include "radio_loop.hpp"
#include <exception>
#include <utility>
#include <fstream>
namespace pico {
namespace {
void diag(const std::string &line) {
    std::ofstream("/tmp/pico-diag.log", std::ios::app) << line << std::endl;
}
} // namespace
RadioLoop::RadioLoop(OwnerIO io, DataPlane &p, LoopHooks h, bool crc, bool repeat)
    : io_(std::move(io)), plane_(p), hooks_(std::move(h)), crc_enabled_(crc), repeat_host_(repeat) {
    if (!io_.read || !io_.write || !io_.now || !io_.sleep || !hooks_.guard || !hooks_.poll ||
        !hooks_.publish || (repeat_host_ && !hooks_.repeat) || !hooks_.latency)
        throw std::invalid_argument("Missing loop dependency");
    if (io_.command) {
        batch_ = std::make_unique<DebugMemory>(io_.command);
        plane_.clock.probe_carrier = true;
    }
}
RadioLoop::~RadioLoop() {
    plane_.clock.probe_carrier = false;
    plane_.clock.probe_due = nullptr;
}
void RadioLoop::count(IterationLatency &s) {
    const auto now = DebugMemory::stats();
    if (counted_) {
        s.tcl_commands = now.commands - counted_->commands;
        s.wire_commands = now.wire - counted_->wire;
        s.bytes_read = now.bytes_read - counted_->bytes_read;
        s.bytes_written = now.bytes_written - counted_->bytes_written;
    }
    counted_ = now;
}
void RadioLoop::step() {
    IterationLatency s;
    s.iteration = ++iterations_;
    s.begin = io_.now();
    if (!counted_)
        counted_ = DebugMemory::stats();
    try {
        hooks_.guard();
        Bytes header;
        {
            std::optional<DebugMemory::Batch> writes;
            std::vector<std::pair<uint32_t, Bytes>> prefetched;
            uint32_t w = 0, r = 0;
            bool heartbeat = false, imu_ring = false;
            uint32_t slot_stride = 48, imu_depth = 0;  // selected by State.enabled (imu_slot.hpp)
            std::optional<Bytes> copied;
            {   // PICO_STATION_COUNTERS: once a second log slots/packets/dropped/blocked/phase/result and D1880..1916 (diagnostic only)
                static int64_t stc_last=0;
                static const bool stc_env = std::getenv("PICO_RADIO_DUMP") || std::getenv("PICO_STATION_COUNTERS");
                if(stc_env && io_.now()-stc_last>5000000000){stc_last=io_.now();  // every 5 s: 188 B and two debugger round trips
                    try{auto a=io_.read(roy_data+12,32);auto b=io_.read(roy_data+1876,156);
                        std::ofstream o("/tmp/pico-diag.log",std::ios::app);o<<"station_counters t="<<stc_last;
                        for(int i=0;i<8;++i)o<<" s"<<(12+4*i)<<"="<<word(a,4*i);
                        for(int i=0;i<39;++i)o<<" d"<<(1876+4*i)<<"="<<word(b,4*i);
                        o<<std::endl;}catch(const std::exception&e){diag(std::string("radio_soft: station counters: ")+e.what());}}
            }
            plane_.clock.probe_carrier = batch_ && iterations_ >= batch_off_until_;
            if (!plane_.clock.probe_carrier && plane_.clock.probe_due) {
                auto probe = std::move(plane_.clock.probe_due);  // parked before a fallback began
                plane_.clock.probe_due = nullptr;
                s.probe = true;
                try {
                    probe([&] { return word(io_.read(0xe0001004, 4)); });
                } catch (const std::exception &e) {
                    diag(std::string("radio_soft: Roy clock probe: ") + e.what());
                }
            }
            if (plane_.clock.probe_carrier) {
                s.batched = true;
                RingBatch plan;
                plan.heartbeat = heartbeat = s.begin - heartbeat_ > 50000000;
                plan.extra = prefetch_;
                {   // 2026-10-31 hang test: PICO_SWD_HAMMER=N adds N extra words read from the module code (Roy RAM8) per batch
                    static const size_t hammer = std::getenv("PICO_SWD_HAMMER") ? std::strtoul(std::getenv("PICO_SWD_HAMMER"), nullptr, 0) : 0;
                    if (hammer) plan.extra.push_back({0x20011600u, hammer * 4});
                }
                RingBatchResult got;
                bool ran = false;
                std::exception_ptr failed;
                auto run = [&] {
                    const auto before = io_.now();
                    try {
                        got = batch_->ring_batch(plan);
                    } catch (...) {
                        failed = std::current_exception();
                        throw;
                    }
                    last_batch_ns_ = io_.now() - before;
                    ran = true;
                };
                try {
                    if (plane_.clock.probe_due) {
                        auto probe = std::move(plane_.clock.probe_due);
                        plane_.clock.probe_due = nullptr;
                        s.probe = true;
                        // RoyClock::observe drops brackets over 10 ms; ride along only on a short batch.
                        // 2026-10-31: with the fast SWD every batch is under 6 ms and the folded probe (cycles read at the
                        // batch start, bracket = whole batch) biased the Roy clock by ~half a batch: optics never locked.
                        // The probe now always runs alone (one word, ~0.1 ms bracket). PICO_PROBE_FOLD=1 restores folding.
                        s.probe_folded = plan.probe = std::getenv("PICO_PROBE_FOLD") && last_batch_ns_ <= 6000000;
                        try {
                            if (plan.probe)
                                probe([&] {
                                    run();
                                    return *got.cycles;
                                });
                            else
                                probe([&] { return word(batch_->read(0xe0001004, 4)); });
                        } catch (const std::exception &e) {
                            if (failed)
                                throw;
                            diag(std::string("radio_soft: Roy clock probe: ") + e.what());
                        }
                    }
                    if (!ran)
                        run();
                    batch_failures_ = 0;
                } catch (const std::exception &e) {
                    if (++batch_failures_ >= 3) {  // owner rule: logged; per-region reads take over for a while
                        diag(std::string("radio_soft: batched ring command failed 3 times (") + e.what() +
                             "); per-region reads for 200 iterations");
                        batch_failures_ = 0;
                        batch_off_until_ = iterations_ + 200;
                        plane_.clock.probe_carrier = false;
                    }
                    throw;
                }
                // Words 0..3, 7, 28, 29 at their v4 offsets; the rest stay zero (unused).
                header.assign(120, 0);
                std::copy(got.header.begin(), got.header.begin() + 16, header.begin());
                std::copy(got.header.begin() + 16, got.header.begin() + 20, header.begin() + 28);
                std::copy(got.header.begin() + 20, got.header.end(), header.begin() + 112);
                w = got.write;
                r = got.read;
                imu_depth = imu_ring_depth_for(word(header, 8)); slot_stride=imu_ring_stride_for(word(header,8));
                imu_ring = imu_depth != 0;
                if (imu_ring) {
                    if (w < r)
                        ++plane_.counters.ring_loss;
                    copied = std::move(got.ring);
                } else
                    copied = pico::guard_evidence::exercise(11, "ring-capacity", [&] {
                        return snapshot(
                            w, r,
                            [&](size_t o, size_t n) {
                                return Bytes(got.ring.begin() + o, got.ring.begin() + o + n);
                            },
                            [](uint32_t) {},  // the batch acknowledged write_count after its reads
                            &plane_.counters);
                    });
                for (size_t i = 0; i < plan.extra.size(); ++i)
                    prefetched.push_back({plan.extra[i].first, std::move(got.extra[i])});
            } else {
                header = io_.read(roy_data, 120);
                if (header.size() != 120)
                    throw std::runtime_error("short v4 header");
                w = word(header, 112);
                r = word(header, 116);
                imu_depth = imu_ring_depth_for(word(header, 8)); slot_stride=imu_ring_stride_for(word(header,8));
                imu_ring = imu_depth != 0;
                if (imu_ring) {
                    Bytes ring(1120, 0);
                    if (w >= r) {
                        const uint32_t first = w - r > imu_depth ? w - imu_depth : r;
                        for (uint32_t index = first; index != w; ++index) {
                            auto part = io_.read(roy_data + 380 + (index % imu_depth) * slot_stride,
                                                  slot_stride);
                            if (part.size() == slot_stride)
                                std::copy(part.begin(), part.end(),
                                          ring.begin() + (index % imu_depth) * slot_stride);
                        }
                        io_.write(roy_data + 116, le32(w));
                    } else
                        ++plane_.counters.ring_loss;
                    copied = std::move(ring);
                } else
                    copied = pico::guard_evidence::exercise(11, "ring-capacity", [&] {
                        return snapshot(
                            w, r,
                            [&](size_t o, size_t n) {
                                return io_.read(roy_data + 380 + static_cast<uint32_t>(o), n);
                            },
                            [&](uint32_t captured) { io_.write(roy_data + 116, le32(captured)); },
                            &plane_.counters);
                    });
            }
            s.snapshot_done = io_.now();
            {   // multi4 (PICO_PEERS or the multi4 flag): State.enabled is the observable that the multi4 ring is decoded
                static uint32_t logged_flag = 0;
                const uint32_t flag = word(header, 8);
                if (flag != logged_flag && (flag == imu_ring_flag_multi4 || PeerTable::configured().multi)) {
                    logged_flag = flag;
                    char line[96];
                    std::snprintf(line, sizeof(line), "radio_soft: ring flag 0x%08x depth %u", flag, imu_depth);
                    diag(line);
                }
            }
            // Mailbox/station writes from poll and dispatch leave as one command at the end.
            writes.emplace();
            for (auto &region : prefetched)
                DebugMemory::Batch::prefetch(region.first, std::move(region.second));
            hooks_.poll(header);
            s.poll_done = io_.now();
            pico::guard_evidence::emit(11, "heartbeat", word(header, 4) == 0);
            if (word(header, 4))  // owner rule: logged, never ends the run
                diag("radio_soft: MCU service heartbeat flag set");
            if (!s.batched)
                heartbeat = io_.now() - heartbeat_ > 50000000;
            if (heartbeat) {
                if (!s.batched)
                    io_.write(roy_data + 1608, le32(word(header, 12) + 1000));
                hooks_.publish(word(header, 12), true);
                heartbeat_ = io_.now();
            }
            {   // Poll-interval experiment: State.interval_us (offset 40) is read by the module on every slot request.
                static const char* iv=std::getenv("PICO_TEST_INTERVAL_US");
                static bool iv_done=false;
                if(iv&&!iv_done){io_.write(roy_data+40,le32(uint32_t(std::atoi(iv))));iv_done=true;std::ofstream("/tmp/pico-diag.log",std::ios::app)<<"interval_us set "<<iv<<std::endl;}
            }
            if (word(header) != 0x314d5250)  // owner rule: logged; the next iteration reads the header again
                diag("radio_soft: module magic missing in header");
            { static uint32_t sensor_flag=0;const auto flag=word(header,8);
              if(sensor_flag!=flag) {sensor_flag=flag;
                if(flag==alpha_ring_single||flag==alpha_ring_multi)
                    diag("sensor_transport: complete sensor envelopes, depth 14 stride 80; Alpha IMU/temperature/mag preserved");
                else if(imu_ring)
                    diag("sensor_transport: legacy compact ring loses ALL revision-12 Alpha bodies and magnetic tails; select station_alpha module");
                else if(flag==0x53454e31u) diag("sensor_transport: SEN1 full 112-byte records, depth 10");
              }
            }
            // snapshot() resynchronises a lost ring: nothing to read after a rollback, only the newest ten after an overflow.
            const uint32_t depth = imu_ring ? imu_depth : 10u;
            const uint32_t stride = imu_ring ? slot_stride : 112u;
            const uint32_t first = imu_ring ? (w < r ? w : (w - r > depth ? w - depth : r))
                                            : (!copied ? w : (w - r > 10 ? w - 10 : r));
            for (uint32_t index = first; index != w; ++index) {
                const auto offset = (index % depth) * stride;
                Record record;
                if(imu_ring && (word(header,8)==alpha_ring_single||word(header,8)==alpha_ring_multi)) {
                    record=expand_alpha_slot(copied->data()+offset,plane_.peer_chips,word(header,8)==alpha_ring_multi);
                    if(word(header,8)==alpha_ring_multi && record.dma.empty()) {
                        const auto meta=word(*copied,offset);
                        const unsigned peer=(meta>>21)&3;
                        if((meta>>31) && (peer>=3 || !plane_.peer_chips[peer])) {
                            ++plane_.counters.peer_rejected;
                            diag("radio_soft: Alpha slot of unenrolled peer " + std::to_string(peer) + " dropped");
                        }
                    }
                } else if (imu_ring && word(header, 8) == imu_ring_flag_multi4) {
                    // multi4: the module wrote the polled peer's index; the chip comes from the enrollment table.
                    auto slot = expand_multi4_slot(copied->data() + offset, plane_.peer_chips);
                    if (slot.rejected) {
                        ++plane_.counters.peer_rejected;  // owner rule: logged and dropped, never another tracker
                        static int64_t peer_log = 0;
                        if (io_.now() - peer_log > 1000000000) {
                            peer_log = io_.now();
                            diag("radio_soft: multi4 slot of unenrolled peer " + std::to_string(slot.peer) +
                                 " dropped (total " + std::to_string(plane_.counters.peer_rejected) + ")");
                        }
                    }
                    record = std::move(slot.record);
                } else
                    record = imu_ring ? expand_imu_slot(copied->data() + offset, word(header, 8) == imu_ring_flag23t)
                                      : parse_record(Bytes(copied->begin() + offset, copied->begin() + offset + 112),
                                                     crc_enabled_);
                if (imu_ring && record.dma.empty())
                    continue;
                record.host_read_ns = io_.now();
                static std::FILE* radio_dump=[]{const char* p=std::getenv("PICO_RADIO_DUMP");return p?std::fopen(p,"ab"):nullptr;}();
                if(radio_dump){
                    const int64_t t=record.host_read_ns;std::fwrite(&t,8,1,radio_dump);
                    if(!imu_ring)std::fwrite(copied->data()+offset,1,112,radio_dump);
                    else{
                        uint8_t image[112]={};
                        auto put=[&](int o,uint32_t v){for(int i=0;i<4;++i)image[o+i]=uint8_t(v>>(8*i));};
                        put(0,record.sequence);put(12,record.channel);
                        image[16]=uint8_t(record.dma.size());image[17]=uint8_t(record.dma.size()>>8);
                        std::copy(record.dma.begin(),record.dma.end(),image+20);
                        put(100,record.chip);put(104,record.cycles);
                        image[108]=uint8_t(record.callback_us);image[109]=uint8_t(record.callback_us>>8);
                        std::fwrite(image,1,112,radio_dump);
                    }
                    std::fflush(radio_dump);
                }
                Stamps stamps;
                stamps.ring_start = s.begin;
                stamps.ring_end = s.snapshot_done;
                stamps.snapshot_free = s.snapshot_done;
                stamps.poll_start = s.snapshot_done;
                stamps.poll_end = s.poll_done;
                plane_.record(record, stamps);
                ++s.records;
            }
            // Station/query reads repeated every iteration ride in the next batch (bounded, SRAM data only).
            prefetch_.clear();
            size_t bytes = 0;
            for (auto region : writes->requested())
                if (region.first >= roy_data && region.first + region.second <= roy_data + 2048 &&
                    bytes + region.second <= 256) {
                    prefetch_.push_back(region);
                    bytes += region.second;
                }
            writes->flush();
        }
        s.dispatch_done = io_.now();
        if (word(header, 28) == 3 && repeat_host_)
            hooks_.repeat();
        s.end = io_.now();
        // Idle: 5 ms as the reference. With records the ring is polled again at once,
        // after a 0.5 ms pause: the sleep is where the session lock is handed to other
        // threads (the debugger command itself runs under it).
        io_.sleep(s.records ? 500000 : 5000000);
        s.wake = io_.now();
        s.complete = true;
        latency[0].observe(s.begin, s.snapshot_done);
        latency[1].observe(s.snapshot_done, s.poll_done);
        latency[2].observe(s.poll_done, s.dispatch_done);
        latency[3].observe(s.begin, s.end);
        latency[4].observe(s.end, s.wake);
        count(s);
        last = s;
        hooks_.latency(s);
    } catch (...) {
        count(s);
        last = s;
        hooks_.latency(s);
        throw;
    }
}
RoyClockPoll::RoyClockPoll(OwnerIO io, RoyClock &c, std::function<int64_t()> raw,
                           std::function<void(const ClockProbeEvent &)> log)
    : io_(std::move(io)), clock_(c), raw_(std::move(raw)), log_(std::move(log)) {
    if (!io_.now || !io_.read || !raw_ || !log_)
        throw std::invalid_argument("Missing Roy clock probe dependency");
}
RoyClockPoll::~RoyClockPoll() { clock_.probe_due = nullptr; }
void RoyClockPoll::poll(bool ready) {
    if (!ready)
        return;
    const auto now = io_.now();
    if (!acquisition_)
        acquisition_ = now;
    if (now - *acquisition_ > 8000000000LL && !clock_.fit && now - last_clock_log_ > 5000000000LL) {
        // owner rule: keep fitting; log every 5 s instead of ending the run
        std::ofstream("/tmp/pico-diag.log", std::ios::app) << "radio_soft: Roy clock not admitted yet" << std::endl;
        last_clock_log_ = now;
    }
    // 2026-10-30: 50 ms until the clock is fitted, then 250 ms. A probe runs alone (a batch over 6 ms cannot carry
    // it), so at ~50 ms radio iterations every iteration paid one more debugger round trip.
    if (now - last_poll_ > (clock_.fit ? 250000000 : 50000000)) {
        auto probe = [this, now](const std::function<uint32_t()> &read) {
            const auto raw_before = raw_();
            const auto before = io_.now();
            const auto cycles = read();
            const auto after = io_.now();
            const auto raw_after = raw_();
            const bool admitted = clock_.observe(cycles, before, after, raw_before, raw_after);
            last_poll_ = now;
            log_({cycles, before, after, raw_before, raw_after, admitted});
        };
        if (clock_.probe_carrier)  // the radio loop runs it inside its next debugger command
            clock_.probe_due = probe;
        else
            probe([this] { return word(io_.read(0xe0001004, 4)); });
    }
}
} // namespace pico

#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_radio_loop, "radio-loop");
