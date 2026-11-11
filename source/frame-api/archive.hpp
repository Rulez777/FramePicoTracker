#pragma once
#include "runtime.hpp"
#ifndef FRAME_API_HOST_TEST
#include <grp.h>
#include <pwd.h>
#endif
namespace frame {
inline uint32_t crc32(const std::string &s) {
  uint32_t c = 0xffffffff;
  for (unsigned char b : s) {
    c ^= b;
    for (int i = 0; i < 8; ++i)
      c = (c >> 1) ^ (0xedb88320U & uint32_t(-int(c & 1)));
  }
  return c ^ 0xffffffff;
}
inline void little(std::string &s, uint32_t n, unsigned bytes) {
  for (unsigned i = 0; i < bytes; ++i)
    s += char(n >> (i * 8));
}
inline std::string gzip(const std::string &input, const std::string &filename) {
  std::string out("\x1f\x8b\x08\x08", 4);
  little(out, uint32_t(time(nullptr)), 4);
  out += '\0';
  out += '\xff';
  out += filename;
  out += '\0';
  size_t at = 0;
  do {
    size_t n = std::min(size_t(65535), input.size() - at);
    bool last = at + n == input.size();
    out += char(last ? 1 : 0);
    little(out, uint32_t(n), 2);
    little(out, uint32_t(n) ^ 65535, 2);
    out.append(input, at, n);
    at += n;
  } while (at < input.size());
  little(out, crc32(input), 4);
  little(out, uint32_t(input.size()), 4);
  return out;
}
inline void tar_header(std::string &tar, const std::string &name,
                       const struct stat &st, size_t bytes, char type = '0') {
  std::array<char, 512> h{};
  auto copy = [&](size_t at, size_t n, const std::string &v) {
    std::memcpy(h.data() + at, v.data(), std::min(n, v.size()));
  };
  auto oct = [&](size_t at, size_t n, unsigned long long v) {
    char b[32];
    std::snprintf(b, sizeof b, "%0*llo", int(n - 1), v);
    copy(at, n - 1, b);
  };
  copy(0, 100, name);
  oct(100, 8, unsigned(st.st_mode & 07777));
  oct(108, 8, unsigned(st.st_uid));
  oct(116, 8, unsigned(st.st_gid));
  oct(124, 12, bytes);
  oct(136, 12, static_cast<unsigned long long>(st.st_mtim.tv_sec));
  std::fill(h.begin() + 148, h.begin() + 156, ' ');
  h[156] = type;
  copy(257, 6, std::string("ustar\0", 6));
  copy(263, 2, "00");
  // reentrant lookups: capture workers and request handlers run concurrently.
  char buffer[16384];
  passwd pw{}, *p = nullptr;
  if (!getpwuid_r(st.st_uid, &pw, buffer, sizeof buffer, &p) && p)
    copy(265, 32, p->pw_name);
  group gr{}, *g = nullptr;
  if (!getgrgid_r(st.st_gid, &gr, buffer, sizeof buffer, &g) && g)
    copy(297, 32, g->gr_name);
  unsigned sum = 0;
  for (unsigned char c : h)
    sum += c;
  char checksum[8];
  std::snprintf(checksum, sizeof checksum, "%06o", sum);
  copy(148, 6, checksum);
  h[154] = 0;
  h[155] = ' ';
  tar.append(h.data(), h.size());
}
inline void bundle(const fs::path &prefix) {
  std::string tar;
  for (const char *suffix : {".records.jsonl", ".manifest.json"}) {
    fs::path path = prefix.string() + suffix;
    struct stat st{};
    if (stat(path.c_str(), &st))
      throw std::runtime_error("Cannot stat capture artifact");
    auto bytes = readfile(path);
    // Python tarfile's default PAX format stores subsecond mtime.
    std::string value = "mtime=" + J(mtime(path)).text + "\n";
    size_t len = value.size() + 2;
    while (std::to_string(len).size() + 1 + value.size() != len)
      len = std::to_string(len).size() + 1 + value.size();
    auto pax = std::to_string(len) + " " + value;
    struct stat ph = st;
    ph.st_mode = 0644;
    tar_header(tar, "././@PaxHeader", ph, pax.size(), 'x');
    tar += pax;
    tar.append((512 - pax.size() % 512) % 512, '\0');
    tar_header(tar, path.filename().string(), st, bytes.size());
    tar += bytes;
    tar.append((512 - bytes.size() % 512) % 512, '\0');
  }
  tar.append(1024, '\0');
  tar.append((10240 - tar.size() % 10240) % 10240, '\0');
  writefile(prefix.string() + ".tar.gz",
            gzip(tar, prefix.filename().string() + ".tar"));
}
} // namespace frame
