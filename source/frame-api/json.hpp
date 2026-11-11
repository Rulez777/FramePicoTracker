#pragma once
#include <cctype>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace frame {
struct J {
  char type = 'n';
  std::string text;
  std::vector<J> array;
  std::vector<std::pair<std::string, J>> object;
  J() = default;
  J(std::nullptr_t) {}
  J(const char *s) : type('s'), text(s) {}
  J(std::string s) : type('s'), text(std::move(s)) {}
  J(bool b) : type('b'), text(b ? "true" : "false") {}
  template <class T,
            std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>,
                             int> = 0>
  J(T n) : type('d'), text(std::to_string(n)) {}
  J(double n) : type('d') {
    char b[64];
    if (!std::isfinite(n)) {
      text = std::isnan(n) ? "NaN" : n < 0 ? "-Infinity" : "Infinity";
      return;
    }
    auto r = std::to_chars(b, b + 64, n, std::chars_format::scientific);
    std::string scientific(b, r.ptr);
    auto e = scientific.find('e');
    int exponent = std::stoi(scientific.substr(e + 1));
    if (exponent >= -4 && exponent < 16) {
      bool negative = scientific[0] == '-';
      std::string digits =
          scientific.substr(negative ? 1 : 0, e - (negative ? 1 : 0));
      auto dot = digits.find('.');
      if (dot != std::string::npos)
        digits.erase(dot, 1);
      int point = exponent + 1;
      text = negative ? "-" : "";
      if (point <= 0)
        text += "0." + std::string(size_t(-point), '0') + digits;
      else if (size_t(point) >= digits.size())
        text += digits + std::string(size_t(point) - digits.size(), '0') + ".0";
      else
        text += digits.substr(0, size_t(point)) + "." +
                digits.substr(size_t(point));
    } else {
      text = scientific.substr(0, e) + "e" + (exponent < 0 ? "-" : "+");
      int absolute = std::abs(exponent);
      if (absolute < 10)
        text += '0';
      text += std::to_string(absolute);
    }
  }
  static J obj(std::initializer_list<std::pair<std::string, J>> v = {}) {
    J j;
    j.type = 'o';
    j.object = v;
    return j;
  }
  static J list() {
    J j;
    j.type = 'a';
    return j;
  }
  J &operator[](const std::string &k) {
    for (auto &p : object)
      if (p.first == k)
        return p.second;
    object.emplace_back(k, J{});
    return object.back().second;
  }
  const J &operator[](const std::string &k) const {
    for (auto &p : object)
      if (p.first == k)
        return p.second;
    static const J nil;
    return nil;
  }
  bool truth() const {
    return type == 'b'   ? text == "true"
           : type == 'n' ? false
           : type == 'a' ? !array.empty()
           : type == 'o' ? !object.empty()
           : type == 'd' ? std::stod(text) != 0
                         : !text.empty();
  }
  bool yes() const { return type == 'b' && text == "true"; }
  bool no() const { return type == 'b' && text == "false"; }
  double number(double fallback = 0) const {
    return type == 'd' ? std::stod(text) : fallback;
  }
  bool operator==(const char *s) const { return type == 's' && text == s; }
};
inline void utf8(std::string &s, unsigned c) {
  if (c < 128)
    s += char(c);
  else if (c < 2048) {
    s += char(192 | (c >> 6));
    s += char(128 | (c & 63));
  } else if (c < 65536) {
    s += char(224 | (c >> 12));
    s += char(128 | ((c >> 6) & 63));
    s += char(128 | (c & 63));
  } else {
    s += char(240 | (c >> 18));
    s += char(128 | ((c >> 12) & 63));
    s += char(128 | ((c >> 6) & 63));
    s += char(128 | (c & 63));
  }
}
class Parser {
  const std::string &s;
  size_t at = 0;
  char take() {
    if (at == s.size())
      throw std::runtime_error("JSON EOF");
    return s[at++];
  }
  void ws() {
    while (at < s.size() && std::isspace(static_cast<unsigned char>(s[at])))
      ++at;
  }
  unsigned hex() {
    unsigned c = 0;
    for (int i = 0; i < 4; ++i) {
      char h = take();
      int v = h >= '0' && h <= '9'   ? h - '0'
              : h >= 'a' && h <= 'f' ? h - 'a' + 10
              : h >= 'A' && h <= 'F' ? h - 'A' + 10
                                     : -1;
      if (v < 0)
        throw std::runtime_error("JSON escape");
      c = c * 16 + unsigned(v);
    }
    return c;
  }
  std::string str() {
    if (take() != '"')
      throw std::runtime_error("JSON string");
    std::string o;
    for (;;) {
      char c = take();
      if (c == '"')
        return o;
      if (static_cast<unsigned char>(c) < 32)
        throw std::runtime_error("JSON control");
      if (c != '\\') {
        o += c;
        continue;
      }
      c = take();
      switch (c) {
      case '"':
      case '\\':
      case '/':
        o += c;
        break;
      case 'b':
        o += '\b';
        break;
      case 'f':
        o += '\f';
        break;
      case 'n':
        o += '\n';
        break;
      case 'r':
        o += '\r';
        break;
      case 't':
        o += '\t';
        break;
      case 'u': {
        unsigned u = hex();
        if (u >= 0xd800 && u <= 0xdbff) {
          if (take() != '\\' || take() != 'u')
            throw std::runtime_error("JSON surrogate");
          unsigned lo = hex();
          if (lo < 0xdc00 || lo > 0xdfff)
            throw std::runtime_error("JSON surrogate");
          u = 0x10000 + ((u - 0xd800) << 10) + lo - 0xdc00;
        }
        utf8(o, u);
        break;
      }
      default:
        throw std::runtime_error("JSON escape");
      }
    }
  }
  J value(unsigned depth) {
    if (depth > 256)
      throw std::runtime_error("JSON depth");
    ws();
    if (at == s.size())
      throw std::runtime_error("JSON EOF");
    char c = s[at];
    if (c == '"')
      return J(str());
    if (c == '{' || c == '[') {
      ++at;
      J j = c == '{' ? J::obj() : J::list();
      ws();
      if (at < s.size() && s[at] == (c == '{' ? '}' : ']')) {
        ++at;
        return j;
      }
      for (;;) {
        ws();
        if (c == '{') {
          auto k = str();
          ws();
          if (take() != ':')
            throw std::runtime_error("JSON colon");
          j[k] = value(depth + 1);
        } else
          j.array.push_back(value(depth + 1));
        ws();
        char end = take();
        if (end == (c == '{' ? '}' : ']'))
          return j;
        if (end != ',')
          throw std::runtime_error("JSON comma");
      }
    }
    for (const auto &lit :
         {"true", "false", "null", "NaN", "Infinity", "-Infinity"}) {
      std::string t = lit;
      if (s.compare(at, t.size(), t) == 0) {
        at += t.size();
        if (t == "NaN" || t == "Infinity" || t == "-Infinity") {
          J j;
          j.type = 'd';
          j.text = t;
          return j;
        }
        return t == "null" ? J{} : J(t == "true");
      }
    }
    size_t start = at;
    if (s[at] == '-')
      ++at;
    if (at == s.size())
      throw std::runtime_error("JSON number");
    if (s[at] == '0')
      ++at;
    else {
      if (s[at] < '1' || s[at] > '9')
        throw std::runtime_error("JSON number");
      while (at < s.size() && std::isdigit(static_cast<unsigned char>(s[at])))
        ++at;
    }
    for (char marker : {'.', 'e'}) {
      if (at < s.size() &&
          (s[at] == marker || (marker == 'e' && s[at] == 'E'))) {
        ++at;
        if (marker == 'e' && at < s.size() && (s[at] == '+' || s[at] == '-'))
          ++at;
        size_t b = at;
        while (at < s.size() && std::isdigit(static_cast<unsigned char>(s[at])))
          ++at;
        if (b == at)
          throw std::runtime_error("JSON number");
      }
    }
    J j;
    j.type = 'd';
    j.text = s.substr(start, at - start);
    if (j.text.find_first_of(".eE") != std::string::npos)
      j = J(std::strtod(j.text.c_str(), nullptr));
    else if (j.text == "-0")
      j.text = "0";
    return j;
  }

public:
  explicit Parser(const std::string &input) : s(input) {}
  J parse() {
    J j = value(0);
    ws();
    if (at != s.size())
      throw std::runtime_error("JSON trailing data");
    return j;
  }
};
inline std::string quote(const std::string &s) {
  std::string o = "\"";
  for (size_t i = 0; i < s.size(); ++i) {
    unsigned c = static_cast<unsigned char>(s[i]);
    switch (c) {
    case '"':
      o += "\\\"";
      continue;
    case '\\':
      o += "\\\\";
      continue;
    case '\b':
      o += "\\b";
      continue;
    case '\f':
      o += "\\f";
      continue;
    case '\n':
      o += "\\n";
      continue;
    case '\r':
      o += "\\r";
      continue;
    case '\t':
      o += "\\t";
      continue;
    default:
      break;
    }
    if (c >= 128) {
      unsigned n = c < 224 ? 1 : c < 240 ? 2 : 3;
      c &= n == 1 ? 31 : n == 2 ? 15 : 7;
      while (n-- && i + 1 < s.size())
        c = (c << 6) | (static_cast<unsigned char>(s[++i]) & 63);
    }
    auto esc = [&](unsigned x) {
      char b[16];  // "\uXXXX" plus slack for gcc format-truncation analysis
      std::snprintf(b, sizeof b, "\\u%04x", x);
      o += b;
    };
    if (c < 32 || c >= 127) {
      if (c > 65535) {
        c -= 65536;
        esc(0xd800 + (c >> 10));
        esc(0xdc00 + (c & 1023));
      } else
        esc(c);
    } else
      o += char(c);
  }
  return o + '"';
}
inline std::string dump(const J &j, int indent = -1, int level = 0) {
  if (j.type == 's')
    return quote(j.text);
  if (j.type == 'n')
    return "null";
  if (j.type != 'o' && j.type != 'a')
    return j.text;
  std::string o = j.type == 'o' ? "{" : "[";
  size_t n = j.type == 'o' ? j.object.size() : j.array.size();
  for (size_t i = 0; i < n; ++i) {
    if (i)
      o += ",";
    if (indent >= 0)
      o += '\n' + std::string(size_t((level + 1) * indent), ' ');
    else if (i)
      o += ' ';
    if (j.type == 'o')
      o += quote(j.object[i].first) + ": ";
    o += dump(j.type == 'o' ? j.object[i].second : j.array[i], indent,
              level + 1);
  }
  if (n && indent >= 0)
    o += '\n' + std::string(size_t(level * indent), ' ');
  return o + (j.type == 'o' ? "}" : "]");
}
} // namespace frame
