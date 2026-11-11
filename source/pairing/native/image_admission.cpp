#include "guard_evidence.hpp"
#include "image_admission.hpp"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
namespace pico {
namespace {
uint32_t rotate(uint32_t v, unsigned s) { return (v >> s) | (v << (32 - s)); }
constexpr uint32_t k[] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
[[noreturn]] void fail(const char *s) { throw std::runtime_error(s); }
uint16_t u16(const Bytes &b, size_t off) {
    if (off > b.size() || b.size() - off < 2)
        fail("Truncated ELF");
    return uint16_t(b[off]) | uint16_t(uint16_t(b[off + 1]) << 8);
}
} // namespace
std::string sha256(const Bytes &input) {
    Bytes b = input;
    const uint64_t bits = uint64_t(b.size()) * 8;
    b.push_back(0x80);
    while (b.size() % 64 != 56)
        b.push_back(0);
    for (int i = 7; i >= 0; --i)
        b.push_back(static_cast<uint8_t>(bits >> (8 * i)));
    std::array<uint32_t, 8> h = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                                 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
    for (size_t off = 0; off < b.size(); off += 64) {
        std::array<uint32_t, 64> w{};
        for (unsigned i = 0; i < 16; ++i)
            w[i] = (uint32_t(b[off + 4 * i]) << 24) | (uint32_t(b[off + 4 * i + 1]) << 16) |
                   (uint32_t(b[off + 4 * i + 2]) << 8) | b[off + 4 * i + 3];
        for (unsigned i = 16; i < 64; ++i) {
            const auto x = w[i - 15], y = w[i - 2];
            w[i] = w[i - 16] + (rotate(x, 7) ^ rotate(x, 18) ^ (x >> 3)) + w[i - 7] +
                   (rotate(y, 17) ^ rotate(y, 19) ^ (y >> 10));
        }
        auto v = h;
        for (unsigned i = 0; i < 64; ++i) {
            const uint32_t t1 = v[7] + (rotate(v[4], 6) ^ rotate(v[4], 11) ^ rotate(v[4], 25)) +
                                ((v[4] & v[5]) ^ (~v[4] & v[6])) + k[i] + w[i];
            const uint32_t t2 = (rotate(v[0], 2) ^ rotate(v[0], 13) ^ rotate(v[0], 22)) +
                                ((v[0] & v[1]) ^ (v[0] & v[2]) ^ (v[1] & v[2]));
            for (unsigned j = 7; j > 0; --j)
                v[j] = v[j - 1];
            v[4] += t1;
            v[0] = t1 + t2;
        }
        for (unsigned i = 0; i < 8; ++i)
            h[i] += v[i];
    }
    const char *digits = "0123456789abcdef";
    std::string out;
    for (auto v : h)
        for (int s = 28; s >= 0; s -= 4)
            out += digits[(v >> s) & 15];
    return out;
}
RoyLayoutReport roy_layout_report() {
    RoyLayoutReport report;
    const char *path = std::getenv("ROY_LAYOUT_REPORT");
    if (!path || !*path)
        return report;
    std::ifstream in(path);
    std::string line;
    if (!std::getline(in, line))
        return report;
    if (!line.empty() && line.back() == '\r')
        line.pop_back();
    if (line != "roy_layout ok")
        return report;
    report.ok = true;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        const std::string prefix = "module_sha256 ";
        if (line.compare(0, prefix.size(), prefix) == 0)
            report.module_sha = line.substr(prefix.size());
        if (line.rfind("flash_sha256 ",0)==0) report.flash_sha=line.substr(13);
    }
    report.ok = report.module_sha.size()==64 && report.flash_sha.size()==64;
    return report;
}
ModuleImage elf_module(const Bytes &b) {
    if (b.size() < 52 ||
        !std::equal(b.begin(), b.begin() + 7, Bytes{0x7f, 'E', 'L', 'F', 1, 1, 1}.begin()) ||
        u16(b, 18) != 40)
        fail("Expected little endian 32-bit ARM ELF");
    const auto entry = word(b, 24), shoff = word(b, 32);
    const auto stride = u16(b, 46), count = u16(b, 48);
    Bytes code;
    for (unsigned i = 0; i < count; ++i) {
        const uint64_t at = uint64_t(shoff) + uint64_t(i) * stride;
        if (at > b.size() || b.size() - size_t(at) < 40)
            fail("Truncated ELF section table");
        const size_t o = static_cast<size_t>(at);
        const auto kind = word(b, o + 4), flags = word(b, o + 8), address = word(b, o + 12),
                   offset = word(b, o + 16), size = word(b, o + 20);
        if (!(flags & 2) || !size)
            continue;
        if (kind != 1 || address < roy_code || uint64_t(address) + size > roy_code + 2044)
            fail("Module allocates outside reserved code buffer");
        if (offset > b.size() || size > b.size() - offset)
            fail("Truncated ELF section data");
        const auto end = address - roy_code + size;
        if (code.size() < end)
            code.resize(end);
        std::copy(b.begin() + offset, b.begin() + offset + size,
                  code.begin() + (address - roy_code));
    }
    if ((entry & ~1u) < roy_code || (entry & ~1u) >= roy_code + code.size())
        fail("Bad entry point");
    if ((code.size() + 3) / 4 * 4 > 2044)
        fail("Aligned module exceeds reserved code buffer");
    return {entry, code};
}
ModuleImage admit_original210(const Bytes &stock, const Bytes &elf) {
    pico::guard_evidence::Scope guard_evidence_scope(9,"module");
    const auto layout = roy_layout_report();
    if (layout.ok) {
        if(sha256(stock)!=layout.flash_sha || sha256(elf)!=layout.module_sha)
            fail("Roy locator flash/module report mismatch");
        std::cerr << "stock sha256 " << sha256(stock) << std::endl;
    }
    else if (sha256(stock) != "0375ab90c91fa0bce1522f8f510cabfca6667cceadce1e4737b73b82507edd50")
        fail("Wrong stock image for this fixed-address experimental loader");
    // 2026-10-30: also the reviewed MCU IMU-ack variant of original210 (outputs/station-ack-20261030), selected only by PICO_TEST_MODULE.
    if (sha256(elf) != "d8328717a5f6b757fd002ad6622e4fa5f07c45a1de935f6631267792deb286a3" &&
        sha256(elf) != "7c5edc6a0d6533c6d36c83b2a9017e5b32ca52106f5cb1b2994f07d50daa4d89" &&
        sha256(elf) != "c4f91b9c407656ff78edbbe4e81a4fbd99944e42f921a91d26c33d2f05fcc511" &&
        sha256(elf) != "489109544e07643afdb7c80ded43a63eac9b0786a8ac8c223ac06daa40eac806" &&
        sha256(elf) != "2c090600d9a8928024dee853b3e9d5efd097e243d7fdd8757ca92522613ef9ec" &&
        sha256(elf) != "d06ab16f32476ba10da64df03a4a3d7b4378b0c5599772e4d6624e1d53d7a220" &&
        sha256(elf) != "4ccbc17de37cb56605eb508f6f2c64c2afd8d3745edbe8a318b664078547c9e4" &&
        sha256(elf) != "46e35fd645da5df7b17ca5edb33aa639515f0412685fa850679b5c2340649039" &&
        sha256(elf) != "818f1d4e240ff6add7e382ad8a78f263aad9bd842adcb8f04989265cac389cbb" &&
        sha256(elf) != "c102350d2022e9d808352f7d38c5cda4fc0cebdbe376dbd2a5ab3ae5bf426efa" &&
        sha256(elf) != "efadd594323ff688dba4a3d063e55f7fd95d3846f207a62443a72b14d5b56a79" &&  // ackring8 2026-10-31: empty replies (ACKs) kept
        sha256(elf) != "d6ac20830458eec64fd769c1d238f9bfe9f989b8c47d507c5b00c5d6b1a488d9" &&
        sha256(elf) != "f788317ae6944f9cbb9dcd17b0af76061f8ae29cdccd7a7591aa0ccb5cb0aad3" &&
        sha256(elf) != "e91108910364433cee6762afbed6ae284a36073285176c4238bebed65418ce88" &&  // multi4 2026-10-31 (outputs/multi-tracker-radio-20261031, never loaded yet; needs PICO_PEERS)
        !(layout.ok && sha256(elf) == layout.module_sha))
        fail("Protected original210 ELF required");
    return elf_module(elf);
}
void install_original210(SramOwner& owner,const Bytes& stock,const Bytes& elf,const Bytes& state) {
    const auto module=admit_original210(stock,elf);
    owner.install(stock,module.code,module.entry,state);
}
} // namespace pico

#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_image_admission, "image-admission");
