#include "guard_evidence.hpp"
#include "roy_owner.hpp"
#include "peer_table.hpp"
#include "roy_locator.hpp"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <utility>
#include <fstream>
#include <cstdio>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
namespace pico {
namespace {
[[noreturn]] void fail(const char *s) { throw std::runtime_error(s); }
std::string address(uint32_t a) {
    std::ostringstream o;
    o << "0x" << std::hex << a;
    return o.str();
}
bool active(const Bytes &c) {
    if (c.size() != 48)
        fail("Expected exact 48-byte stock MPSL context");
    return c[0x26] != 0;
}
} // namespace
uint32_t word(const Bytes &b, size_t o) {
    if (o > b.size() || b.size() - o < 4)
        fail("short LE32");
    return uint32_t(b[o]) | (uint32_t(b[o + 1]) << 8) | (uint32_t(b[o + 2]) << 16) |
           (uint32_t(b[o + 3]) << 24);
}
Bytes le32(uint32_t x) { return {uint8_t(x), uint8_t(x >> 8), uint8_t(x >> 16), uint8_t(x >> 24)}; }
std::string checkpoint_json(const Checkpoint &c) {
    auto h = [](const Bytes &b) {
        const char *digits = "0123456789abcdef";
        std::string s;
        for (auto v : b) {
            s += digits[v >> 4];
            s += digits[v & 15];
        }
        return s;
    };
    std::string s =
        "{\n  \"stock_sha\": "
        "\"0375ab90c91fa0bce1522f8f510cabfca6667cceadce1e4737b73b82507edd50\",\n  \"code\": \"" +
        h(c.code) + "\",\n  \"data\": \"" + h(c.data) + "\",\n  \"counts\": \"" + h(c.counts) +
        "\",\n  \"trace_controls\": {\n    \"0xe0001000\": \"" + h(c.dwt) +
        "\",\n    \"0xe000edfc\": \"" + h(c.demcr) + "\"\n  }";
    if (!c.static_word.empty())
        s += ",\n  \"mpu_static_word\": \"" + h(c.static_word) + "\"";
    if (c.permissions_saved) {
        s += ",\n  \"mpu_permissions\": ";
        if (c.execute_permission)
            s += "[\n    {\n      \"index\": 1,\n      \"base\": 536870913,\n      \"attributes\": "
                 "285933601\n    }\n  ]";
        else
            s += "[]";
    }
    return s + "\n}\n";
}
void persist_checkpoint(const std::string &path, const Checkpoint &c) {
    const auto bytes = checkpoint_json(c);
    FILE *f = nullptr;
#ifdef _WIN32
    if (fopen_s(&f, path.c_str(), "wb") != 0)
        fail("Cannot open recovery checkpoint");
#else
    f = std::fopen(path.c_str(), "wb");
    if (!f)
        fail("Cannot open recovery checkpoint");
#endif
    bool ok = std::fwrite(bytes.data(), 1, bytes.size(), f) == bytes.size();
    if (std::fflush(f) != 0)
        ok = false;
#ifdef _WIN32
    if (_commit(_fileno(f)) != 0)
        ok = false;
#else
    if (fsync(fileno(f)) != 0)
        ok = false;
#endif
    if (std::fclose(f) != 0)
        ok = false;
    if (!ok)
        fail("Cannot durably write recovery checkpoint");
    std::ifstream in(path, std::ios::binary);
    const std::string actual((std::istreambuf_iterator<char>(in)), {});
    if (in.bad() || actual != bytes)
        fail("Recovery checkpoint failed readback");
}
std::string CleanupBarriers::json() const {
    auto b = [](bool v) { return v ? "true" : "false"; };
    return std::string("{\"session_closed_verified\":") + b(session_closed_verified) +
           ",\"ram_restored_verified\":" + b(ram_restored_verified) +
           ",\"mpu_restored\":" + b(mpu_restored) +
           ",\"trace_controls_restored_verified\":" + b(trace_controls_restored_verified) + "}";
}
SramOwner::SramOwner(OwnerIO io, std::function<void(const Checkpoint &)> p, bool r, bool d)
    : io_(std::move(io)), persist_(std::move(p)), reserve_(r), disable_(d) {
    if (!io_.read || !io_.write || !io_.command || !io_.now || !io_.sleep || !persist_)
        fail("Missing owner dependency");
    host_dwt = PeerTable::configured().multi || std::getenv("PICO_HOST_DWT") != nullptr;
}
// multi4 moved DEMCR.TRCENA / DWT_CTRL.CYCCNTENA out of the module (code space). install() already saved both
// words in the checkpoint and restore() (also the crash restore) writes them back with readback. Owner rule: the
// readback is logged, never a run abort; constant cycles would show up as a clock that never fits.
void SramOwner::enable_trace() {
    std::ofstream log("/tmp/pico-diag.log", std::ios::app);
    try {
        const uint32_t demcr = word(io_.read(0xe000edfc, 4)) | (1u << 24);
        io_.write(0xe000edfc, le32(demcr));
        const uint32_t demcr_read = word(io_.read(0xe000edfc, 4));
        const uint32_t ctrl = word(io_.read(0xe0001000, 4)) | 1u;
        io_.write(0xe0001000, le32(ctrl));
        const uint32_t ctrl_read = word(io_.read(0xe0001000, 4));
        const uint32_t c0 = word(io_.read(0xe0001004, 4)), c1 = word(io_.read(0xe0001004, 4));
        trace_enabled_verified = ((demcr_read >> 24) & 1) && (ctrl_read & 1);
        log << "host_dwt demcr=0x" << std::hex << demcr_read << " dwt_ctrl=0x" << ctrl_read << " cyccnt 0x" << c0
            << "->0x" << c1 << std::dec << (trace_enabled_verified ? " enabled" : " NOT ENABLED (readback)")
            << (c0 == c1 ? " cycles constant" : "") << std::endl;
    } catch (const std::exception &e) {
        log << "host_dwt enable failed: " << e.what() << "; continuing" << std::endl;
    }
}
void SramOwner::equal(uint32_t a, const Bytes &b, const char *reason) {
    if (io_.read(a, b.size()) != b)
        fail(reason);
}
void SramOwner::install(const Bytes &stock, const Bytes &module, uint32_t entry,
                        const Bytes &state) {
    if (saved_code_ || mutated_)
        fail("Owner transaction already started");
    if (stock.size() < 0x189be + 64 || module.empty() || ((module.size() + 3) / 4) * 4 > roy_code_size ||
        (entry & ~1u) < roy_code || (entry & ~1u) >= roy_code + module.size() ||
        state.size() != roy_data_size || word(state) != 0x314d5250)
        fail("Invalid prepared image/state");
    entry_ = entry;
    pico::guard_evidence::exercise(9,"stock-rtt",[&] {
    bool located = false;
    if (const char *report = std::getenv("ROY_LAYOUT_REPORT")) {
        std::ifstream in(report);
        std::string line;
        if (std::getline(in, line)) {
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            located = line == "roy_layout ok";
        }
    }
    if (located) {
        Bytes live;
        for(size_t off=0;off<stock.size();off+=16384) {
            auto part=io_.read(uint32_t(off),std::min<size_t>(16384,stock.size()-off));
            live.insert(live.end(),part.begin(),part.end());
        }
        auto layout=locate_roy(live);
        if(layout.ok) layout=locate_roy_buffers(layout,io_.read(layout.rtt,168));
        if(!layout.ok || layout.mpsl!=roy_mpsl || layout.rtt!=roy_rtt ||
           layout.hook!=roy_hook || layout.static_word!=roy_static ||
           layout.code!=roy_code || layout.data!=roy_data ||
           (layout.data_size&~3u)!=roy_data_size || layout.code_size!=roy_code_size)
            fail("Roy locator/live layout mismatch; refusing SRAM mutation");
        const char* keys[]={"ROY_FN_OPEN","ROY_FN_CLOSE","ROY_FN_REQUEST","ROY_FN_LOCK","ROY_FN_UNLOCK","ROY_FN_PRINT","ROY_REGIONS","ROY_MPU_BASE2"};
        const uint32_t values[]={layout.fn_open,layout.fn_close,layout.fn_request,layout.fn_lock,layout.fn_unlock,layout.fn_print,layout.regions,(layout.code&~0xfffu)|0x12u};
        for(size_t i=0;i<8;++i) if(roy_env(keys[i],0)!=values[i])
            fail("Roy module environment differs from live signatures");
        std::cerr << "Roy firmware located by signatures\n";
    }
    else
        for (size_t o :
             {size_t(0), size_t(0x10580), size_t(0x18948), size_t(0x189be), stock.size() - 64})
            equal(static_cast<uint32_t>(o), Bytes(stock.begin() + o, stock.begin() + o + 64),
                  "Live firmware anchor differs from stock");
    if (active(io_.read(roy_mpsl, 48)))
        fail("MPSL session is in use; cannot reserve its only slot");
    equal(roy_hook, Bytes(4), "Native shell hook already owned; refusing SRAM mutation");
    equal(roy_rtt, Bytes{'S', 'E', 'G', 'G', 'E', 'R', ' ', 'R', 'T', 'T'}, "RTT ABI mismatch");
    saved_.counts = io_.read(roy_rtt + 16, 8);
    if (saved_.counts != Bytes{3, 0, 0, 0, 3, 0, 0, 0})
        fail("Manufacturing RTT layout differs");
    });
    saved_.code = io_.read(roy_code, (roy_code_size & ~3u));
    saved_code_ = true;
    saved_.data = io_.read(roy_data, roy_data_size);
    if (reserve_) {
        saved_.static_word = io_.read(roy_static, 4);
        saved_static_ = true;
        if (saved_.static_word.size() != 4 || saved_.static_word[2] != 2)
            fail("Unexpected Zephyr static MPU count");
    }
    saved_.dwt = io_.read(0xe0001000, 4);
    saved_.demcr = io_.read(0xe000edfc, 4);
    saved_trace_ = true;
    auto initial = saved_;
    initial.static_word.clear();
    pico::guard_evidence::exercise(9,"checkpoint",[&]{persist_(initial);});
    if (saved_static_)
        persist_(saved_);
    // Both reference checkpoint commits precede the first SRAM mutation.
    mutated_ = true; // set BEFORE a possibly committed write throws
    io_.write(roy_rtt + 16, Bytes{1, 0, 0, 0, 1, 0, 0, 0});
    io_.write(roy_data, state);
    initialized_ = true;
    Bytes aligned = module;
    aligned.resize((module.size() + 3) / 4 * 4);
    io_.write(roy_code, aligned);
    equal(roy_code, module, "RAM code failed readback immediately after write");
    saved_.execute_permission = !reserve_;
    saved_.permissions_saved = true;
    persist_(saved_);
    const auto actual = io_.read(roy_code, module.size());
    if (io_.read(roy_data, 4) != le32(0x314d5250) || actual != module)
        fail("Module RAM changed before invocation; target may have restarted");
    installed_ = true;
}
void SramOwner::invoke(unsigned op) {
    equal(roy_hook, Bytes(4), "Stock console already has a bypass callback");
    if (disable_ || (reserve_ && op != 1))
        io_.command("mww 0xe000ed94 0");
    else if (!reserve_)
        io_.command("set original_mpu_index [read_memory 0xe000ed98 32 1]; mww 0xe000ed98 1; mww "
                    "0xe000eda0 0x010b0021; mww 0xe000ed98 [lindex $original_mpu_index 0]");
    io_.write(roy_hook, le32(entry_));
    const auto up = io_.read(roy_rtt + 24, 24), down = io_.read(roy_rtt + 96, 24);
    if (word(up, 4) < 0x20000000 || word(up, 4) > 0x2003f000 || word(up, 8) != 4096 ||
        word(down, 4) < 0x20000000 || word(down, 4) > 0x2003ffc0 ||
        word(down, 8) != 64)
        fail("Stock RTT layout differs");
    const std::string payload = std::to_string(op) + "\n";
    if (((word(down, 16) - word(down, 12) - 1) & 63) < payload.size())
        fail("Console input ring is full");
    io_.command("mww " + address(roy_rtt + 40) + " " + std::to_string(word(up, 12)));
    for (size_t i = 0; i < payload.size(); ++i)
        io_.command("mwb " +
                    address(word(down, 4) + ((word(down, 12) + static_cast<uint32_t>(i)) % 64)) +
                    " " + std::to_string(static_cast<unsigned char>(payload[i])));
    io_.command("mww " + address(roy_rtt + 108) + " " +
                std::to_string((word(down, 12) + static_cast<uint32_t>(payload.size())) % 64));
    io_.sleep(10000000);
    const auto after = io_.read(roy_rtt + 24, 24);
    io_.read(roy_rtt + 96, 24);
    io_.read(word(up, 4), word(up, 8));
    io_.command("mww " + address(roy_rtt + 40) + " " + std::to_string(word(after, 12)));
}
void SramOwner::enter(const std::function<void()> &guard) {
    if (!installed_ || invocation_ || restoring_ || !guard)
        fail("Owner not ready for guarded invocation");
    invocation_ = true;
    guard();
    if (host_dwt)
        enable_trace();  // before operation 0: the module reads CYCCNT from its first slot on
    invoke(0);
    if (reserve_) {
        const auto s = io_.read(roy_static, 4);
        const auto control = io_.read(0xe000ed94, 4);
        if (s.size() != 4 || s[2] != 3 || word(control) != 5)
            fail("RAM MPU reservation not acknowledged");
    }
    const auto status = io_.read(roy_data, 36);
    if (word(status) != 0x314d5250)
        fail("Module state lost during open/request");
    const auto index = io_.read(roy_data + 84, 1);
    if (index.size() != 1)
        fail("short session index");
    opened_ = index[0] != 255;
    if (word(status, 32) != 0)
        fail("MPSL open/request returned error");
    running_ = true;
}
void SramOwner::repeat() {
    if (!running_ || restoring_ || barriers.session_closed_verified)
        fail("No active invocation");
    invoke(1);
}
void SramOwner::restore() {
    restoring_ = true;
    running_ = false;
    if (initialized_ && invocation_) {
        const auto context = io_.read(roy_mpsl, 48);
        const uint32_t callback = word(context, 32) & ~1u;
        opened_ = active(context) && roy_code <= callback && callback < roy_code + roy_code_size;
    }
    if (opened_) {
        io_.write(roy_data + 4, le32(1));
        auto deadline = io_.now() + 300000000;
        while (word(io_.read(roy_data + 28, 4)) == 2) {
            if (io_.now() > deadline)
                fail("Radio slot has not finished; retaining module RAM");
            io_.sleep(5000000);
        }
        invoke(2);
        deadline = io_.now() + 300000000;
        while (word(io_.read(roy_data + 28, 4)) != 0) {
            if (io_.now() > deadline)
                fail("MPSL close not acknowledged; retaining module RAM");
            io_.sleep(5000000);
        }
    }
    if (saved_code_) {
        if (active(io_.read(roy_mpsl, 48)))
            fail("MPSL session remains active; retaining module RAM");
        barriers.session_closed_verified = true;
        if (saved_trace_) {
            io_.write(0xe0001000, saved_.dwt);
            equal(0xe0001000, saved_.dwt, "Roy trace control restoration failed");
            io_.write(0xe000edfc, saved_.demcr);
            equal(0xe000edfc, saved_.demcr, "Roy trace control restoration failed");
            barriers.trace_controls_restored_verified = true;
        }
        equal(roy_hook, Bytes(4), "Native callback still referenced; retaining code");
        if (saved_static_) {
            io_.write(roy_static, saved_.static_word);
            io_.sleep(30000000);
            equal(roy_static, saved_.static_word, "Static MPU count restoration failed");
        }
        // A checkpoint read may have failed before any mutation. Never write a
        // partial checkpoint (the Python finally would attempt it and then fail).
        if (saved_.code.size() != (roy_code_size & ~3u) || saved_.data.size() != roy_data_size || saved_.counts.size() != 8)
            fail("Incomplete checkpoint; retaining RAM");
        io_.write(roy_code, saved_.code);
        io_.write(roy_data, saved_.data);
        io_.write(roy_rtt + 16, saved_.counts);
        equal(roy_code, saved_.code, "RAM checkpoint restoration failed readback");
        equal(roy_data, saved_.data, "RAM checkpoint restoration failed readback");
        equal(roy_rtt + 16, saved_.counts, "RAM checkpoint restoration failed readback");
        barriers.ram_restored_verified = true;
    }
    if (saved_.execute_permission)
        io_.command("set mpu_index [read_memory 0xe000ed98 32 1]; mww 0xe000ed98 1; mww 0xe000eda0 "
                    "0x110b0021; mww 0xe000ed98 [lindex $mpu_index 0]");
    if (disable_ || reserve_) {
        io_.command("mww 0xe000ed94 5");
        if (word(io_.read(0xe000ed94, 4)) != 5)
            fail("MPU control restoration failed readback");
    }
    barriers.mpu_restored = true;
}
std::vector<std::string> finalize_owner(SramOwner &owner, const OwnerResources &r) {
    if (!r.publish_stopped || !r.restore_signals || !r.close_debugger || !r.close_optics ||
        !r.close_guard || !r.release_lock || !r.report)
        fail("Missing independent owner cleanup dependency");
    std::vector<std::string> errors;
    auto attempt = [&](const char *stage, const std::function<void()> &f) {
        try {
            f();
            return true;
        } catch (const std::exception &e) {
            errors.push_back(std::string(stage) + ": " + e.what());
        } catch (...) {
            errors.push_back(std::string(stage) + ": unknown exception");
        }
        return false;
    };
    attempt("MCU", [&] { owner.restore(); });
    attempt("stopped", r.publish_stopped);
    attempt("signals", r.restore_signals);
    bool debug_closed = false;
    if (owner.may_release_owner())
        debug_closed = attempt("debugger", r.close_debugger);
    attempt("optics", r.close_optics);
    attempt("guard", r.close_guard);
    // Keep the shared lock through the report commit, including a report error.
    const bool retained = !owner.may_release_owner() || !debug_closed;
    const bool reported = attempt("report", [&] { r.report(owner.barriers, errors, retained); });
    if (!retained && reported)
        attempt("lock", r.release_lock);
    return errors;
}
} // namespace pico

#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_sram, "sram");
