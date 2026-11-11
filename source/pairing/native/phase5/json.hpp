#pragma once
#include <cstddef>
#include <initializer_list>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <stdexcept>
#include <cctype>
namespace phase5 {
// Strict parser adapted read-only from native/replay.cpp; integer text preserved.
struct Value {
    char type = 'n';
    std::string text;
    std::map<std::string, Value> object;
    std::vector<Value> array;
};
class Json {
    const std::string &s;
    size_t p = 0;
    void ws() {
        while (p < s.size() && std::isspace(static_cast<unsigned char>(s[p])))
            ++p;
    }
    char take() {
        if (p == s.size())
            throw std::runtime_error("JSON EOF");
    return s[p++];
    }
    std::string str() {
        if (take() != '"')
            throw std::runtime_error("JSON string");
        std::string o;
        while (true) {
            char c = take();
            if (c == '"')
                return o;
            if (static_cast<unsigned char>(c) < 32)
                throw std::runtime_error("JSON control");
            if (c == '\\') {
                c = take();
                switch (c) {
                case '"':
                case '\\':
                case '/':
                    break;
                case 'n':
                    c = '\n';
                    break;
                case 'r':
                    c = '\r';
                    break;
                case 't':
                    c = '\t';
                    break;
                case 'b':
                    c = '\b';
                    break;
                case 'f':
                    c = '\f';
                    break;
                case 'u': {
                    unsigned code = 0;
                    for (int i = 0; i < 4; ++i) {
                        char h = take();
                        int v = h >= '0' && h <= '9'   ? h - '0'
                                : h >= 'a' && h <= 'f' ? h - 'a' + 10
                                : h >= 'A' && h <= 'F' ? h - 'A' + 10
                                                       : -1;
                        if (v < 0)
                            throw std::runtime_error("JSON unicode");
                        code = code * 16 + unsigned(v);
                    }
                    if (code >= 0xd800 && code <= 0xdfff)
                        throw std::runtime_error("JSON surrogate unsupported");
                    if (code < 128)
                        o += char(code);
                    else if (code < 2048) {
                        o += char(0xc0 | (code >> 6));
                        o += char(0x80 | (code & 63));
                    } else {
                        o += char(0xe0 | (code >> 12));
                        o += char(0x80 | ((code >> 6) & 63));
                        o += char(0x80 | (code & 63));
                    }
                    continue;
                }
                default:
                    throw std::runtime_error("JSON escape");
                }
            }
            o += c;
        }
    }
    Value value(unsigned depth) {
        if (depth > 64)
            throw std::runtime_error("JSON depth");
        ws();
        Value v;
        if (p == s.size())
            throw std::runtime_error("JSON empty");
        char c = s[p];
        if (c == '"') {
            v.type = 's';
            v.text = str();
            return v;
        }
        if (c == '{' || c == '[') {
            ++p;
            v.type = c;
            ws();
            char close = c == '{' ? '}' : ']';
            if (p < s.size() && s[p] == close) {
                ++p;
                return v;
            }
            while (true) {
                ws();
                if (c == '{') {
                    auto key = str();
                    ws();
                    if (take() != ':')
                        throw std::runtime_error("JSON colon");
                    if (!v.object.emplace(key, value(depth + 1)).second)
                        throw std::runtime_error("JSON duplicate key");
                } else
                    v.array.push_back(value(depth + 1));
                ws();
                char sep = take();
                if (sep == close)
                    break;
                if (sep != ',')
                    throw std::runtime_error("JSON separator");
            }
            return v;
        }
        size_t begin = p;
        while (p < s.size() && s[p] != ',' && s[p] != ']' && s[p] != '}' &&
               !std::isspace(static_cast<unsigned char>(s[p])))
            ++p;
        v.text = s.substr(begin, p - begin);
        if (v.text == "true" || v.text == "false") {
            v.type = 'b';
            return v;
        }
        if (v.text == "null")
            return v;
        // Validate the complete JSON number grammar, even unknown fields.
        size_t i = 0;
        if (i < v.text.size() && v.text[i] == '-')
            ++i;
        if (i == v.text.size())
            throw std::runtime_error("JSON number");
        if (v.text[i] == '0')
            ++i;
        else {
            if (v.text[i] < '1' || v.text[i] > '9')
                throw std::runtime_error("JSON token");
            while (i < v.text.size() && std::isdigit(static_cast<unsigned char>(v.text[i])))
                ++i;
        }
        for (char marker : {'.', 'e'}) {
            if (i < v.text.size() && (v.text[i] == marker || (marker == 'e' && v.text[i] == 'E'))) {
                ++i;
                if (marker == 'e' && i < v.text.size() && (v.text[i] == '+' || v.text[i] == '-'))
                    ++i;
                size_t b = i;
                while (i < v.text.size() && std::isdigit(static_cast<unsigned char>(v.text[i])))
                    ++i;
                if (i == b)
                    throw std::runtime_error("JSON number digits");
            }
        }
        if (i != v.text.size())
            throw std::runtime_error("JSON number tail");
        v.type = 'i';
        return v;
    }

  public:
    explicit Json(const std::string &text) : s(text) {}
    Value parse() {
        Value v = value(0);
        ws();
        if (p != s.size())
            throw std::runtime_error("JSON tail");
    return v;
    }
};
inline const Value &field(const Value &v, const char *k) {
    auto i = v.object.find(k);
    if (i == v.object.end())
        throw std::runtime_error(std::string("missing ") + k);
    return i->second;
}
inline int64_t integer(const Value &v) {
    if (v.type != 'i' || v.text.find_first_of(".eE") != std::string::npos)
        throw std::runtime_error("expected integer");
    size_t end = 0;
    auto n = std::stoll(v.text, &end);
    if (end != v.text.size())
        throw std::runtime_error("integer tail");
    return n;
}
inline int64_t number(const Value &v, const char *k) { return integer(field(v, k)); }
inline uint32_t u32(const Value &v, const char *k) {
    auto n = number(v, k);
    if (n < 0 || n > UINT32_MAX)
        throw std::runtime_error("u32 range");
    return uint32_t(n);
}
inline bool flag(const Value &v, const char *k) {
    const auto &b = field(v, k);
    return b.type == 'b' && b.text == "true";
}

} // namespace phase5


