#include "breadcrumb_sites.h"
#include "diagnostic_sites.h"
#include "sha256.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <stdexcept>
#include <string>
using Bytes = std::vector<uint8_t>;
void require(bool b, const std::string &s) {
  if (!b)
    throw std::runtime_error(s);
}
Bytes read(const std::string &p) {
  std::ifstream f(p, std::ios::binary);
  require(bool(f), "open " + p);
  return Bytes(std::istreambuf_iterator<char>(f), {});
}
void write(const std::string &p, const Bytes &b) {
  std::ofstream f(p, std::ios::binary);
  f.write(reinterpret_cast<const char *>(b.data()), b.size());
  require(bool(f), "write " + p);
}
unsigned u16(const Bytes &b, size_t p) {
  return b.at(p) | (unsigned(b.at(p + 1)) << 8);
}
uint32_t u32(const Bytes &b, size_t p) {
  return u16(b, p) | (uint32_t(u16(b, p + 2)) << 16);
}
Bytes hex(const std::string &s) {
  require(s.size() % 2 == 0, "hex length");
  Bytes b;
  for (size_t i = 0; i < s.size(); i += 2)
    b.push_back(uint8_t(std::stoul(s.substr(i, 2), nullptr, 16)));
  return b;
}
std::string hx(const Bytes &b) {
  std::ostringstream s;
  for (auto v : b)
    s << std::hex << std::setw(2) << std::setfill('0') << unsigned(v);
  return s.str();
}
struct Elf {
  Bytes code;
  std::map<std::string, unsigned> symbols;
  std::map<unsigned,unsigned> function_ends;
};
Elf elf(const std::string &path) {
  auto b = read(path);
  require(b.size() > 52 && u32(b, 0) == 0x464c457f && b[4] == 1 && b[5] == 1 &&
              u16(b, 18) == 40,
          "ARM ELF32 required");
  unsigned start = u32(b, 32), stride = u16(b, 46), count = u16(b, 48);
  require(stride == 40, "ELF section stride");
  auto sec = [&](unsigned n, unsigned field) {
    require(n < count, "ELF section index");
    return u32(b, start + n * stride + field * 4);
  };
  Elf e;
  for (unsigned n = 0; n < count; ++n)
    if (sec(n, 1) == 1 && (sec(n, 2) & 2) && sec(n, 5)) {
      unsigned va = sec(n, 3), off = sec(n, 4), len = sec(n, 5);
      require(va >= 0x18918 && uint64_t(va) + len < 0x2f000 &&
                  uint64_t(off) + len <= b.size(),
              "ELF load bounds");
      e.code.resize(std::max(e.code.size(), size_t(va - 0x18918 + len)), 255);
      std::copy_n(b.begin() + off, len, e.code.begin() + va - 0x18918);
    }
  for (unsigned n = 0; n < count; ++n)
    if (sec(n, 1) == 2) {
      unsigned off = sec(n, 4), len = sec(n, 5), str = sec(sec(n, 6), 4);
      require(sec(n, 9) == 16, "ELF symbol stride");
      for (unsigned p = off; p < off + len; p += 16) {
        size_t q = str + u32(b, p);
        std::string name;
        while (b.at(q))
          name += char(b.at(q++));
        if (!name.empty() && name[0]=='$')
          name += "." + std::to_string(u32(b,p+4));
        if ((b.at(p+12)&15)==2 && u32(b,p+8))
          e.function_ends[u32(b,p+4)&~1u]=(u32(b,p+4)&~1u)+u32(b,p+8);
        if (!name.empty())
          e.symbols[name] = u32(b, p + 4);
      }
    }
  require(!e.code.empty(), "ELF empty");
  return e;
}
Bytes branch(unsigned site, unsigned target, bool link) {
  int64_t d = int64_t(target & ~1u) - site - 4;
  require(!(d & 1) && d >= -(1 << 24) && d < (1 << 24), "branch bounds");
  unsigned v = unsigned(d) & 0x1ffffff, s = v >> 24, i1 = (v >> 23) & 1,
           i2 = (v >> 22) & 1, a = 0xf000 | (s << 10) | ((v >> 12) & 1023),
           z = (link ? 0xd000 : 0x9000) | ((1 ^ i1 ^ s) << 13) |
               ((1 ^ i2 ^ s) << 11) | ((v >> 1) & 2047);
  return {uint8_t(a), uint8_t(a >> 8), uint8_t(z), uint8_t(z >> 8)};
}
const std::string stock_hash =
    "477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89";
const std::string diag_hash =
    "040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d";
Bytes word(unsigned v) {
  return {uint8_t(v), uint8_t(v >> 8), uint8_t(v >> 16), uint8_t(v >> 24)};
}
Bytes movw(unsigned rd, unsigned v) {
  require(rd < 16 && v < 65536, "MOVW operand");
  unsigned a = 0xf240 | ((v >> 12) & 15) | ((v >> 1) & 0x400),
           b = ((v >> 8) & 7) << 12 | (rd << 8) | (v & 255);
  return {uint8_t(a), uint8_t(a >> 8), uint8_t(b), uint8_t(b >> 8)};
}
void audit_control_flow(const Elf &e, const Bytes &image,
                        const std::vector<std::pair<unsigned, Bytes>> &edits,
                        const std::string &out) {
  std::map<unsigned, bool> mapping;
  std::set<unsigned> instructions, stock_instructions;
  for (auto &p : e.symbols)
    if (p.first.rfind("$t", 0) == 0 || p.first.rfind("$d", 0) == 0)
      mapping[p.second] = p.first[1] == 't';
  require(!mapping.empty() && mapping.begin()->first == 0x18918,
          "ELF instruction mapping");
  auto is_code = [&](unsigned va) {
    auto it = mapping.upper_bound(va);
    require(it != mapping.begin(), "mapping bounds");
    return std::prev(it)->second;
  };
  unsigned end = 0x18918 + unsigned(e.code.size());
  for (unsigned va = 0x18918; va < end;) {
    auto fn=e.function_ends.upper_bound(va);
    if(fn!=e.function_ends.begin()) {
      auto prev=std::prev(fn);
      // ELF st_size excludes inter-function alignment emitted as UDF bytes.
      if(va>=prev->second && va<((prev->second+3)&~3u)) {va+=2;continue;}
    }
    if (!is_code(va)) {
      va += 2;
      continue;
    }
    instructions.insert(va);
    unsigned h = u16(image, va - 0x1000);
    va += ((h & 0xf800) >= 0xe800) ? 4 : 2;
    require(va <= end, "instruction extent");
  }
  std::ifstream listing("inputs/swift189-full.asm");
  require(bool(listing), "stock listing");
  std::string line;
  std::vector<std::pair<unsigned, unsigned>> branches;
  const std::set<std::string> branch_names = {
      "b",   "bl",  "beq", "bne", "bhs", "blo", "bcs", "bcc", "bmi", "bpl",
      "bvs", "bvc", "bhi", "bls", "bge", "blt", "bgt", "ble", "cbz", "cbnz"};
  auto inside = [&](unsigned va) {
    for (auto &p : edits)
      if (va >= p.first + 0x1000 && va < p.first + 0x1000 + p.second.size())
        return true;
    return false;
  };
  while (std::getline(listing, line)) {
    if (line.size() < 8 || line[6] != ':')
      continue;
    unsigned va = std::stoul(line.substr(0, 6), nullptr, 16);
    stock_instructions.insert(va);
    std::istringstream ss(line.substr(8));
    std::string op;
    ss >> op;
    auto dot = op.find('.');
    if (dot != std::string::npos)
      op.resize(dot);
    auto pos = line.find("#0x");
    if (branch_names.count(op) && pos != std::string::npos)
      branches.push_back(
          {va, unsigned(std::stoul(line.substr(pos + 3), nullptr, 16))});
  }
  for (auto p : branches)
    if (!inside(p.first))
      for (auto &q : edits) {
        unsigned start = q.first + 0x1000;
        require(!(p.second > start && p.second < start + q.second.size()),
                "stock branch into hook interior at " +
                    std::to_string(p.first));
      }
  auto target = [&](unsigned va) {
    require(instructions.count(va) || stock_instructions.count(va),
            "control target is not a listed instruction: " +
                std::to_string(va));
  };
  std::ofstream report(out + "/control-flow.txt");
  unsigned checked = 0;
  for (unsigned va : instructions) {
    unsigned h = u16(image, va - 0x1000),
             w = va+2<0x1000+image.size() ? u16(image, va - 0x1000 + 2) : 0;
    int32_t d = 0;
    bool direct = false;
    if ((h & 0xf800) == 0xe000) {
      d = int32_t((h & 2047) << 21) >> 20;
      direct = true;
    } else if ((h & 0xf000) == 0xd000 && ((h >> 8) & 15) < 14) {
      d = int32_t((h & 255) << 24) >> 23;
      direct = true;
    } else if ((h & 0xf500) == 0xb100) {
      d = ((h >> 3) & 31) * 2 + ((h >> 9) & 1) * 64;
      direct = true;
    } else if ((h & 0xf800) == 0xf000 &&
               ((w & 0xd000) == 0x9000 || (w & 0xd000) == 0xd000 ||
                ((w & 0xd000) == 0x8000 && ((h >> 6) & 15) < 14))) {
      unsigned s = (h >> 10) & 1, v;
      if ((w & 0xd000) == 0x8000) {
        v = s << 20 | ((w >> 11) & 1) << 19 | ((w >> 13) & 1) << 18 |
            (h & 63) << 12 | (w & 2047) << 1;
        d = int32_t(v << 11) >> 11;
      } else {
        v = s << 24 | (1 ^ ((w >> 13) & 1) ^ s) << 23 |
            (1 ^ ((w >> 11) & 1) ^ s) << 22 | (h & 1023) << 12 |
            (w & 2047) << 1;
        d = int32_t(v << 7) >> 7;
      }
      direct = true;
    }
    if (direct) {
      unsigned to = va + 4 + d;
      target(to);
      report << std::hex << va << " direct " << to << "\n";
      ++checked;
    }
    if (h == 0xf8df && (w >> 12) == 15) {
      unsigned lit = ((va + 4) & ~3u) + (w & 4095),
               to = u32(image, lit - 0x1000);
      require(to & 1, "Thumb literal return");
      target(to & ~1u);
      report << std::hex << va << " literal-pc " << (to & ~1u) << "\n";
      ++checked;
    }
  }
  report << std::dec << "checked_control_targets " << checked
         << "\nstock_direct_branches_scanned " << branches.size() << "\n";
}
Bytes stock() {
  auto b = read("inputs/stock-fw189.bin");
  if (b.size() == 193385) {
    require(
        sha256(b) ==
            "41bd2d24dd9293e7d17c62ab18acd1001d6085da393123a67d226b28d2132e54",
        "container hash");
    b = Bytes(b.begin() + 321, b.begin() + 321 + 96532);
  }
  require(b.size() == 96532 && sha256(b) == stock_hash, "frozen stock hash");
  require(b == read("inputs/candidate10-timer-observer/fw189-stock.bin"),
          "stock reference");
  return b;
}
int diagnostic_gate(int argc, char **argv) {
  try {
    require(
        sha256({}) ==
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
        "SHA empty KAT");
    require(
        sha256({'a', 'b', 'c'}) ==
            "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad",
        "SHA abc KAT");
    std::string out = argc > 1 ? argv[1] : "outputs/diagnostic10";
    std::filesystem::create_directories(out);
    auto s = stock(), b = s;
    auto e = elf("inputs/candidate10-timer-observer/dual-mode.elf");
    unsigned edits = 0;
    std::ofstream m(out + "/manifest.txt");
    m << "image_base 0x1000\nsource_sha256 " << stock_hash << "\n";
    for (auto &p : diagnostic_sites) {
      auto before = hex(p.before), after = hex(p.after);
      unsigned off = p.va - 0x1000;
      require(std::equal(before.begin(), before.end(), s.begin() + off),
              "stock precondition");
      if (p.va >= 0x6000) {
        bool link = (u16(after, 2) & 0x4000) != 0;
        unsigned v = u16(after, 0), w = u16(after, 2), sign = (v >> 10) & 1;
        unsigned bits = (sign << 24) | ((1 ^ ((w >> 13) & 1) ^ sign) << 23) |
                        ((1 ^ ((w >> 11) & 1) ^ sign) << 22) |
                        ((v & 1023) << 12) | ((w & 2047) << 1);
        int32_t d = int32_t(bits << 7) >> 7;
        unsigned target = p.va + 4 + d;
        require(target >= 0x18918 && target < 0x18918 + e.code.size(),
                "branch target appendix");
        require(branch(p.va, target, link) == after, "branch encode roundtrip");
      }
      std::copy(after.begin(), after.end(), b.begin() + off);
      m << std::hex << p.va << " " << p.before << " " << p.after << " "
        << p.reason << "\n";
      ++edits;
    }
    b.resize(0x18918 - 0x1000, 255);
    b.insert(b.end(), e.code.begin(), e.code.end());
    require(b == read("inputs/diagnostic10.bin") && sha256(b) == diag_hash,
            "diagnostic10 byte reproduction");
    auto reverse = b;
    for (auto &p : diagnostic_sites) {
      auto before = hex(p.before);
      std::copy(before.begin(), before.end(), reverse.begin() + p.va - 0x1000);
    }
    reverse.resize(s.size());
    require(reverse == s, "reverse stock");
    require(std::equal(s.begin(), s.begin() + 0x100, b.begin()),
            "vector table intact");
    write(out + "/diagnostic10.bin", b);
    m << "append_base 0x18918\nappend_bytes " << std::dec << e.code.size()
      << "\nappend_sha256 " << sha256(e.code) << "\nsha256 " << sha256(b)
      << "\nverified_edits " << edits << "\n";
    std::cout << "PASS SHA256 KAT 2; stock/container; " << edits
              << " stock preconditions; branch encode/targets; ELF appendix; "
                 "vector; reverse; byte equality\n"
              << sha256(b) << "\n";
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "FAIL " << e.what() << "\n";
    return 1;
  }
}

int release(const std::string &ep, const std::string &out) {
  auto s = stock(), b = s;
  auto e = elf(ep);
  std::filesystem::create_directories(out);
  std::ofstream m(out + "/manifest.txt"), changes(out + "/changed-bytes.csv");
  changes << "address,stock,release,reason\n";
  std::map<unsigned, std::string> reasons;
  std::vector<std::pair<unsigned, Bytes>> reverse_edits;
  auto patch = [&](unsigned va, const Bytes &before, const Bytes &after,
                   const std::string &reason) {
    require(before.size() == after.size() && va >= 0x1000 &&
                size_t(va - 0x1000) + before.size() <= s.size(),
            "patch bounds");
    unsigned off = va - 0x1000;
    require(std::equal(before.begin(), before.end(), s.begin() + off),
            "stock precondition at " + std::to_string(va));
    for (unsigned i = 0; i < after.size(); ++i) {
      require(!reasons.count(off + i), "patch overlap");
      reasons[off + i] = reason;
    }
    reverse_edits.push_back({off, before});
    std::copy(after.begin(), after.end(), b.begin() + off);
    m << std::hex << va << " " << hx(before) << " " << hx(after) << " "
      << reason << "\n";
  };
  auto hook = [&](unsigned va, const Bytes &before, const std::string &name,
                  bool link) {
    require(e.symbols.count(name), "missing ELF symbol " + name);
    unsigned target = e.symbols.at(name);
    require((target & 1) && target >= 0x18919 &&
                target < 0x18918 + e.code.size(),
            "hook symbol bounds");
    patch(va, before, branch(va, target, link), name);
  };
  for (auto &p : diagnostic_sites) {
    if (p.va == 0x8d58 || p.va == 0x7b8e || p.va == 0x7be2 || p.va == 0x65c0 ||
        p.va == 0x785a)
      continue;
    if (p.va == 0x1348 || p.va == 0x134c) {
      patch(p.va, hex(p.before), word(e.symbols.count("alpha_mag_reconcile") ? 0x20005178 : e.symbols.count("alpha_imu") ? 0x20005160 : 0x20005158),
            e.symbols.count("alpha_mag_reconcile") ? "profile/ack/mag reservation 40 bytes; heap 472 bytes" : e.symbols.count("alpha_imu") ? "profile/ack reservation 16 bytes; heap 496 bytes" : "profile state 8 bytes; heap 504 bytes");
      continue;
    }
    std::string name;
    switch (p.va) {
    case 0xc3ec:
      name = "profile_boot_entry";
      break;
    case 0xc444:
      name = "profile_boot_hold";
      break;
    case 0xc4aa:
      name = "profile_boot_release";
      break;
    case 0xbce6:
      name = "profile_power_off";
      break;
    case 0x8d16:
      name = "profile_command";
      break;
    case 0x6c98:
      name = "profile_rx_boundary";
      break;
    default: {
      std::ostringstream t;
      t << "select_" << std::hex << p.va;
      name = t.str();
    }
    }
    hook(p.va, hex(p.before), name, p.va == 0xbce6 || p.va == 0x6c98);
  }
  if (e.symbols.count("alpha_imu")) {
    for (auto item : {std::pair<unsigned,const char *>{0x9458,"alpha_imu_gate"},
                      {0x97f4,"alpha_ack_gate"}, {0x9516,"alpha_dispatch_guard"}}) {
      unsigned off=item.first-0x1000;
      hook(item.first, Bytes(s.begin()+off,s.begin()+off+4), item.second, false);
    }
  }
  if (e.symbols.count("alpha_mag_reconcile")) {
    hook(0xda02,hex("94e80f00"),"alpha_mag_worker_gate",false);
    hook(0xb448,hex("fff75dff"),"alpha_mag_product_gate",true);
  }
  if (e.symbols.count("alpha13_timer_gate")) {
    for (auto item : {std::pair<unsigned,const char *>{0xb2b0,"alpha13_timer_gate"},
                      {0xda18,"alpha13_event_gate"}, {0xb3f0,"alpha13_read_gate"}}) {
      unsigned off=item.first-0x1000;
      hook(item.first,Bytes(s.begin()+off,s.begin()+off+4),item.second,item.first!=0xb2b0);
    }
  }
  for (unsigned va : {0x6538u, 0x65a2u, 0x67deu, 0x6c42u, 0x6c5cu, 0x6eacu}) {
    std::ostringstream t;
    t << "delay_" << std::hex << va;
    if (e.symbols.count(t.str()))
      hook(va, movw(0, va == 0x6eac ? 10980 : 9922), t.str(), false);
  }
  if (e.symbols.count("breadcrumb_boot")) {
    patch(0x1350, word(0x20005350), word(0x20005310),
          "reserve retained heap tail 0x20005310..0x2000534f; heap 440 bytes");
    // Archived C can still be reproduced; C2 has no ADDRESS symbol or hook.
    if (e.symbols.count("breadcrumb_address"))
      hook(0x532c, hex("00f0daff"), "breadcrumb_address", true);
    for (auto &p : breadcrumb_sites)
      hook(p.va, hex(p.before), p.symbol, p.link);
  }
  b.resize(0x18918 - 0x1000, 255);
  b.insert(b.end(), e.code.begin(), e.code.end());
  require(0x1000 + b.size() < 0x2f000, "metadata bound");
  audit_control_flow(e, b, reverse_edits, out);
  std::map<unsigned, std::string> owners;
  for (auto &p : e.symbols)
    if (p.second >= 0x18918 && p.second < 0x18918 + e.code.size() &&
        p.first[0] != '$')
      owners[p.second & ~1u] = p.first;
  auto explanation = [&](size_t off) {
    if (off < s.size())
      return reasons.count(unsigned(off))
                 ? reasons.at(unsigned(off))
                 : std::string("restored stock / observer removed");
    if (off < 0x18918 - 0x1000)
      return std::string("alignment FF");
    auto it = owners.upper_bound(unsigned(off) + 0x1000);
    require(it != owners.begin(), "appendix ownership");
    return std::string("ELF instruction/literal owned by ") +
           std::prev(it)->second;
  };
  for (unsigned i = 0; i < s.size(); ++i)
    if (s[i] != b[i]) {
      require(reasons.count(i), "unexplained stock change");
      changes << std::hex << i + 0x1000 << "," << unsigned(s[i]) << ","
              << unsigned(b[i]) << "," << reasons.at(i) << "\n";
    }
  for (size_t i = s.size(); i < b.size(); ++i)
    changes << std::hex << i + 0x1000 << ",absent," << unsigned(b[i]) << ","
            << explanation(i) << "\n";
  auto diagnostic = read("inputs/diagnostic10.bin");
  require(sha256(diagnostic) == diag_hash, "diagnostic reference hash");
  std::ofstream dd(out + "/vs-diagnostic10.csv");
  dd << "address,diagnostic10,release,reason\n";
  for (size_t i = 0; i < std::max(diagnostic.size(), b.size()); ++i)
    if (i >= diagnostic.size() || i >= b.size() || diagnostic[i] != b[i]) {
      dd << std::hex << i + 0x1000 << ","
         << (i < diagnostic.size() ? hx({diagnostic[i]}) : "absent") << ","
         << (i < b.size() ? hx({b[i]}) : "absent") << ","
         << (i < b.size() ? explanation(i) : "removed diagnostic appendix")
         << "\n";
    }
  auto rev = b;
  for (auto &p : reverse_edits)
    std::copy(p.second.begin(), p.second.end(), rev.begin() + p.first);
  rev.resize(s.size());
  require(rev == s, "reverse stock");
  require(std::equal(s.begin(), s.begin() + 0x200, b.begin()), "vector table");
  for (unsigned va : {0x8d58u, 0x7b8eu, 0x7be2u, 0x65c0u, 0x785au})
    require(std::equal(s.begin() + va - 0x1000, s.begin() + va - 0x1000 + 4,
                       b.begin() + va - 0x1000),
            "observer restoration");
  std::ofstream sy(out + "/symbols.txt");
  for (auto &p : e.symbols)
    sy << std::hex << p.second << " " << p.first << "\n";
  write(out + "/image.bin", b);
  m << "source_sha256 " << sha256(s) << "\nsha256 " << sha256(b)
    << "\nelf_sha256 " << sha256(read(ep)) << "\nappend_sha256 "
    << sha256(e.code) << "\nimage_bytes " << std::dec << b.size()
    << "\nappend_bytes " << e.code.size()
    << "\nrevision " << (e.symbols.count("alpha_imu") ? 12 : 11)
    << "\nprofile_ram " << (e.symbols.count("alpha_mag_reconcile") ? "0x20005150..0x20005177" : e.symbols.count("alpha_imu") ? "0x20005150..0x2000515f" : "0x20005150..0x20005157")
    << "\nhardware_verified "
       "false\napproved_for_ota false\n";
  m << "retained_ram "
    << (e.symbols.count("breadcrumb_boot")
            ? "0x20005310..0x2000534f; current 20 bytes at 0x20005310; frozen "
              "20 bytes at 0x20005328; heap 440 bytes"
            : e.symbols.count("alpha_mag_reconcile") ? "none; heap 472 bytes" : e.symbols.count("alpha_imu") ? "none; heap 496 bytes" : "none; heap 504 bytes")
    << "\n";
  std::cout << "PASS release preconditions/accounting/reverse/vectors/observer "
               "restoration: "
            << out << " " << b.size() << " " << sha256(b) << "\n";
  return 0;
}
int main(int argc, char **argv) {
  try {
    if (argc == 3 && std::string(argv[1]) == "check-elf") {
      auto a = elf(argv[2]),
           b = elf("inputs/candidate10-timer-observer/dual-mode.elf");
      require(a.code == b.code, "reassembled appendix differs");
      std::cout << "PASS reassembled diagnostic .text byte equality "
                << sha256(a.code) << "\n";
      return 0;
    }
    if (argc == 4 && std::string(argv[1]) == "release")
      return release(argv[2], argv[3]);
    return diagnostic_gate(argc, argv);
  } catch (const std::exception &e) {
    std::cerr << "FAIL " << e.what() << "\n";
    return 1;
  }
}
