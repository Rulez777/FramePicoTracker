#include "native_test_runtime.hpp"
#include <algorithm>
#include "image_admission.hpp"
#include "fake_openocd.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
using namespace pico;
namespace {
size_t checks = 0;
void check(bool b) {
    ++checks;
    if (!b)
        throw std::runtime_error("image check " + std::to_string(checks));
}
template <class F> void rejects(F f) {
    bool b = false;
    try {
        f();
    } catch (const std::exception &) {
        b = true;
    }
    check(b);
}
Bytes read(const std::filesystem::path &p) {
    std::ifstream f(p, std::ios::binary);
    if (!f)
        throw std::runtime_error("fixture not found: " + p.string());
    return Bytes(std::istreambuf_iterator<char>(f), {});
}
void set(Bytes &b, size_t o, uint32_t v) {
    auto x = le32(v);
    std::copy(x.begin(), x.end(), b.begin() + o);
}
} // namespace
int main() {
    try {
        check(sha256({}) == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
        check(sha256({'a', 'b', 'c'}) ==
              "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
        check(sha256(Bytes(1000000, 'a')) ==
              "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0");
        const auto root = std::filesystem::path(__FILE__).parent_path() / ".." / "candidate";
        const auto stock = read(root / "stock.bin"), elf = read(root / "build" / "roy_station.elf");
        const auto admitted = admit_original210(stock, elf);
        check(!admitted.code.empty() && admitted.code.size() <= 2044);
        check((admitted.entry & ~1u) >= roy_code);
        auto changed = stock;
        changed[0] ^= 1;
        rejects([&] { admit_original210(changed, elf); });
        changed = elf;
        changed.back() ^= 1;
        rejects([&] { admit_original210(stock, changed); });
    rejects([&] { admit_original210(stock, read(root / "release" / "roy_station.elf")); });
    {FakeOpenOcd f;f.stock(stock);DebugMemory d([&](const std::string& c){return f.command(c);});unsigned commits=0;
        SramOwner owner(f.io(d),[&](const Checkpoint&){++commits;});Bytes state(2048);set(state,0,0x314d5250);state[84]=255;
        rejects([&]{install_original210(owner,stock,changed,state);});check(f.transcript.empty()&&commits==0);
        install_original210(owner,stock,elf,state);check(commits==3&&owner.unresolved());
        check(f.read(roy_code,admitted.code.size())==admitted.code);owner.enter([]{});owner.restore();check(owner.barriers.complete());
    }
        Bytes small(96);
        std::copy_n(elf.begin(), 52, small.begin());
        set(small, 24, roy_code | 1);
        set(small, 32, 52);
        small[46] = 40;
        small[47] = 0;
        small[48] = 1;
        small[49] = 0;
        set(small, 56, 1);
        set(small, 60, 2);
        set(small, 64, roy_code);
        set(small, 68, 92);
        set(small, 72, 4);
        small[92] = 1;
        small[93] = 2;
        small[94] = 3;
        small[95] = 4;
        check(elf_module(small).code == Bytes({1, 2, 3, 4}));
        for (unsigned reason = 0; reason < 8; ++reason) {
            auto bad = small;
            if (reason == 0)
                bad[5] = 2;
            if (reason == 1)
                bad[18] = 0;
            if (reason == 2)
                set(bad, 24, roy_code + 4);
            if (reason == 3)
                set(bad, 56, 8);
            if (reason == 4)
                set(bad, 64, roy_code - 4);
            if (reason == 5)
                set(bad, 72, 2045);
            if (reason == 6)
                set(bad, 68, 95);
            if (reason == 7)
                set(bad, 32, 0xfffffffe);
            rejects([&] { elf_module(bad); });
        }
        std::cout << checks << " image admission checks PASS (protected files read only)\n";
        return 0;
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}
