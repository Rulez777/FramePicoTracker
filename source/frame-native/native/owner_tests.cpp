#include "native_test_runtime.hpp"
#include <algorithm>
#include "fake_openocd.hpp"
#include <iostream>
#include <filesystem>
using namespace pico;
namespace {
size_t checks = 0;
std::string context;
void check(bool b) {
    ++checks;
    if (!b)
        throw std::runtime_error("owner check " + std::to_string(checks) + " " + context);
}
template <class F> void rejects(F f) {
    bool threw = false;
    try {
        f();
    } catch (const std::exception &) {
        threw = true;
    }
    check(threw);
}
struct Fixture {
    Bytes image = Bytes(0x19000, 0x91), module = Bytes{1, 2, 3, 4, 5}, state = Bytes(2048);
    FakeOpenOcd f;
    DebugMemory d;
    Checkpoint checkpoint;
    unsigned persisted = 0, fail_persist = 0;
    std::vector<Checkpoint> checkpoints;
    SramOwner owner;
    Fixture()
        : d([this](const std::string &c) { return f.command(c); }),
          owner(f.io(d), [this](const Checkpoint &c) {
              ++persisted;
              if (fail_persist == persisted)
                  throw std::runtime_error("checkpoint commit");
              checkpoint = c;
              checkpoints.push_back(c);
              if (persisted <= 2) {
                  check(f.read(roy_code, 2044) == Bytes(2044, 0xa5));
                  check(f.read(roy_rtt + 16, 8) == Bytes({3, 0, 0, 0, 3, 0, 0, 0}));
              }
          }) {
        f.stock(image);
        const auto m = le32(0x314d5250);
        std::copy(m.begin(), m.end(), state.begin());
        state[84] = 255;
    }
    void install() { owner.install(image, module, roy_code | 1, state); }
    void enter() {
        owner.enter([] {});
    }
    void restored() {
        check(owner.barriers.complete());
        check(owner.may_release_owner());
        check(f.read(roy_code, 2044) == checkpoint.code);
        check(f.read(roy_data, 2048) == checkpoint.data);
        check(f.read(roy_rtt + 16, 8) == checkpoint.counts);
        check(f.read(roy_static, 4) == checkpoint.static_word);
        check(f.read(0xe0001000, 4) == checkpoint.dwt);
        check(f.read(0xe000edfc, 4) == checkpoint.demcr);
        check(word(f.read(0xe000ed94, 4)) == 5);
    }
};
} // namespace
int main() {
    try {
        Fixture good;
        good.install();
        check(good.persisted == 3);
        check(good.owner.unresolved());
        check(good.checkpoints[0].static_word.empty() && !good.checkpoints[0].permissions_saved);
        check(good.checkpoints[1].static_word == Bytes({0, 2, 2, 0}) &&
              !good.checkpoints[1].permissions_saved);
        check(good.checkpoints[2].permissions_saved && !good.checkpoints[2].execute_permission);
        check(good.checkpoint.code == Bytes(2044, 0xa5));
        check(good.checkpoint.data == Bytes(2048, 0x5a));
        check(good.checkpoint.counts == Bytes({3, 0, 0, 0, 3, 0, 0, 0}));
        good.enter();
        good.owner.repeat();
        check(good.f.invocations == 2);
        good.owner.restore();
        good.restored();
        check(good.owner.barriers.json() ==
              "{\"session_closed_verified\":true,\"ram_restored_verified\":true,\"mpu_restored\":"
              "true,\"trace_controls_restored_verified\":true}");
        check(CleanupBarriers{}.json().find(":true") == std::string::npos);
        // Every TCL command in install/entry/restore is faulted both before and
        // after commit, including every 64-byte write chunk and every readback.
        Fixture baseline;
        baseline.install();
        baseline.enter();
        const auto restore_begin = baseline.f.operations;
        baseline.owner.restore();
        const auto count = baseline.f.operations;
        for (size_t n = 1; n <= count; ++n)
            for (bool after : {false, true}) {
                context = "fault " + std::to_string(n) + " after " + std::to_string(after);
                Fixture x;
                x.f.fail_at = n;
                x.f.fail_after = after;
                bool threw = false;
                try {
                    x.install();
                    x.enter();
                    x.owner.restore();
                } catch (const std::exception &) {
                    threw = true;
                }
                check(threw);
                check(x.owner.may_release_owner() == !x.owner.unresolved());
                if (n > restore_begin)
                    check(!x.owner.barriers.complete());
                // Failures in closure must never restore executable bytes/counts.
                if (n > restore_begin && !x.owner.barriers.session_closed_verified) {
                    check(x.f.read(roy_code, 8) == Bytes({1, 2, 3, 4, 5, 0, 0, 0}));
                    check(x.f.read(roy_rtt + 16, 8) == Bytes({1, 0, 0, 0, 1, 0, 0, 0}));
                }
                if (n <= restore_begin) {
                    x.f.fail_at = 0;
                    try {
                        x.owner.restore();
                    } catch (const std::exception &) {
                        check(x.owner.unresolved() || x.persisted < 2);
                    }
                    if (x.owner.unresolved()) {
                        // Interrupted hook installation must retain the complete code;
                        // interrupted entry with a committed session must be closed.
                        check(word(x.f.read(roy_hook, 4)) != 0);
                        check(x.f.read(roy_code, 8) == Bytes({1, 2, 3, 4, 5, 0, 0, 0}));
                    } else if (x.persisted == 3)
                        x.restored();
                }
            }
        for (unsigned reason = 0; reason < 6; ++reason) {
            context = "reason " + std::to_string(reason);
            Fixture x;
            x.install();
            x.enter();
            if (reason == 0)
                x.f.slot_stuck = true, x.f.set(roy_data + 28, 2);
            if (reason == 1)
                x.f.close_stuck = true;
            if (reason == 2)
                x.f.active_after_close = true;
            if (reason == 3)
                x.f.hook_stuck = true;
            if (reason == 4)
                x.f.corrupt_read = true, x.f.corrupt_address = 0xe0001000;
            if (reason == 5)
                x.f.corrupt_read = true, x.f.corrupt_address = roy_static;
            rejects([&] { x.owner.restore(); });
            check(x.owner.unresolved());
            check(!x.owner.may_release_owner());
            check(!x.owner.barriers.ram_restored_verified);
            check(x.f.read(roy_code, 8) == Bytes({1, 2, 3, 4, 5, 0, 0, 0}));
            if (reason <= 2)
                check(!x.owner.barriers.session_closed_verified);
            if (reason == 0)
                check(x.f.ns == 315000000); // > deadline, exact 300ms is still allowed
        }
        for (uint32_t addr : {roy_code, roy_data, roy_rtt + 16, 0xe000ed94u}) {
            context = "readback " + std::to_string(addr);
            Fixture x;
            x.install();
            x.enter();
            x.f.corrupt_read = true;
            x.f.corrupt_address = addr;
            rejects([&] { x.owner.restore(); });
            check(x.owner.unresolved());
            if (addr == 0xe000ed94)
                check(x.owner.barriers.ram_restored_verified && !x.owner.barriers.mpu_restored);
            else
                check(!x.owner.barriers.ram_restored_verified);
        }
        // Actual context status, not callback residue, is authoritative.
        {
            context = "inactive";
            Fixture x;
            x.install();
            x.enter();
            x.f.memory[roy_mpsl + 0x26] = 0;
            x.f.set(roy_mpsl + 32, roy_code | 1);
            x.owner.restore();
            check(x.f.invocations == 1);
            x.restored();
        }
        {
            context = "foreign callback";
            Fixture x;
            x.install();
            x.enter();
            x.f.set(roy_mpsl + 32, roy_code + 2046);
            rejects([&] { x.owner.restore(); });
            check(x.f.invocations == 1);
            check(x.owner.unresolved());
        }
        {
            context = "guard";
            Fixture x;
            x.install();
            rejects([&] { x.owner.enter([] { throw std::runtime_error("guard"); }); });
            x.owner.restore();
            x.restored();
            check(x.f.invocations == 0);
        }
        // Live anchor, active byte, hook, ABI, counts and static count reject before mutation.
        for (unsigned reason = 0; reason < 6; ++reason) {
            Fixture x;
            if (reason == 0)
                x.f.memory[0x189be] ^= 1;
            if (reason == 1)
                x.f.memory[roy_mpsl + 0x26] = 1;
            if (reason == 2)
                x.f.set(roy_hook, roy_code | 1);
            if (reason == 3)
                x.f.memory[roy_rtt] = 'X';
            if (reason == 4)
                x.f.set(roy_rtt + 16, 2);
            if (reason == 5)
                x.f.memory[roy_static + 2] = 3;
            rejects([&] { x.install(); });
            check(x.persisted == 0);
            check(x.f.read(roy_code, 2044) == Bytes(2044, 0xa5));
        }
        for (unsigned at = 1; at <= 3; ++at) {
            Fixture x;
            x.fail_persist = at;
            rejects([&] { x.install(); });
            if (at <= 2) {
                check(!x.owner.unresolved());
                check(x.f.read(roy_code, 2044) == Bytes(2044, 0xa5));
            } else {
                check(x.owner.unresolved());
                x.owner.restore();
                x.restored();
            }
        }
        // Independent finalizers all run after a sibling error. Unresolved SRAM
        // never reaches debugger close or shared-lock release.
        for (unsigned error = 0; error < 8; ++error) {
            Fixture x;
            x.install();
            x.enter();
            std::vector<std::string> calls;
            if (error == 0)
                x.f.close_stuck = true;
            auto resource = [&](const char *label, unsigned id) {
                return [&, label, id] {
                    calls.push_back(label);
                    if (error == id)
                        throw std::runtime_error(label);
                };
            };
            const auto errors = finalize_owner(
                x.owner,
                {resource("stopped", 1), resource("signals", 2), resource("debugger", 3),
                 resource("optics", 4), resource("guard", 5), resource("lock", 7),
                 [&](const CleanupBarriers &b, const std::vector<std::string> &, bool retained) {
                     calls.push_back("report");
                     check(retained == (error == 0 || error == 3));
                     check(b.complete() == (error != 0));
                     if (error == 6)
                         throw std::runtime_error("report");
                 }});
            check(!errors.empty());
            check(std::find(calls.begin(), calls.end(), "stopped") != calls.end());
            check(std::find(calls.begin(), calls.end(), "signals") != calls.end());
            check(std::find(calls.begin(), calls.end(), "optics") != calls.end());
            check(std::find(calls.begin(), calls.end(), "guard") != calls.end());
            check(std::find(calls.begin(), calls.end(), "report") != calls.end());
            check((std::find(calls.begin(), calls.end(), "debugger") != calls.end()) ==
                  (error != 0));
            check((std::find(calls.begin(), calls.end(), "lock") != calls.end()) ==
                  (error != 0 && error != 3 && error != 6));
        }
        const auto path =
            (std::filesystem::path(__FILE__).parent_path() / "build" / "owner-checkpoint.json")
                .string();
        persist_checkpoint(path, good.checkpoint);
        check(checkpoint_json(good.checkpoint).find("\"mpu_permissions\": []") !=
              std::string::npos);
        check(checkpoint_json(good.checkpoints[0]).find("mpu_static_word") == std::string::npos);
        rejects([&] { persist_checkpoint(path + "/absent/child", good.checkpoint); });
        {
            Fixture x;
            x.f.corrupt_read = true;
            x.f.corrupt_address = roy_code;
            rejects([&] { x.install(); });
            rejects([&] { x.enter(); });
            check(x.f.invocations == 0);
        }
        {
            Fixture x;
            x.install();
            x.owner.restore();
            rejects([&] { x.enter(); });
            rejects([&] { x.owner.repeat(); });
        }
        std::cout << checks << " SRAM owner checks PASS (synthetic source-derived TCL fixtures)\n";
        return 0;
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}
