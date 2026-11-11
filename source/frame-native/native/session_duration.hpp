#pragma once
// Session length in seconds, taken from the sealed "--duration N" argument (5..150, default scripts pass 150).
#include <string>
namespace pico {
inline int parse_session_duration(const std::string& text) {
    if (text.empty() || text.size() > 3 || text.front() == '0') return 0;
    int value = 0;
    for (char c : text) { if (c < '0' || c > '9') return 0; value = value * 10 + (c - '0'); }
    return value >= 5 && value <= 150 ? value : 0;
}
inline int& session_duration_seconds() { static int seconds = 150; return seconds; }
}
