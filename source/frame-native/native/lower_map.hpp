#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <string>
#include <vector>

namespace pico::lower_map {
// Evidence for one (xrtap node, OCC lower serial) combination.
struct Evidence {
    size_t matched = 0;   // PGM frames with an OCC unit of that serial near the common timestamp offset
    double jitter_us = NAN; // RMS of (node stamp - serial stamp) around its median
    double score = NAN;     // mean brightness correlation with that serial's per-frame metadata
};
// mapping: 0 = node0->serial0 and node1->serial1, 1 = swapped, -1 = UNDECIDED.
struct Verdict { int mapping = -1; std::string basis, reason; };
inline double pearson(const std::vector<double> &a, const std::vector<double> &b) {
    const size_t n = std::min(a.size(), b.size());
    if (n < 3) return NAN;
    double ma = 0, mb = 0;
    for (size_t i = 0; i < n; ++i) { ma += a[i]; mb += b[i]; }
    ma /= double(n); mb /= double(n);
    double sab = 0, saa = 0, sbb = 0;
    for (size_t i = 0; i < n; ++i) {
        sab += (a[i]-ma)*(b[i]-mb); saa += (a[i]-ma)*(a[i]-ma); sbb += (b[i]-mb)*(b[i]-mb);
    }
    return saa > 0 && sbb > 0 ? sab/std::sqrt(saa*sbb) : NAN;
}
// Timing decides when one assignment's per-frame jitter is clearly (>20 us and 2x) smaller;
// brightness when the correlation sums differ by more than 0.3. Disagreement is UNDECIDED.
inline Verdict decide(const std::array<std::array<Evidence,2>,2> &e, size_t min_matches = 50) {
    Verdict v;
    for (const auto &row : e) for (const auto &x : row) if (x.matched < min_matches) {
        v.reason = "fewer than " + std::to_string(min_matches) + " timestamp matches for a node/serial";
        return v;
    }
    int timing = -1, brightness = -1;
    const double td = e[0][0].jitter_us+e[1][1].jitter_us, ts = e[0][1].jitter_us+e[1][0].jitter_us;
    if (std::isfinite(td) && std::isfinite(ts) && std::abs(td-ts) > 20 && std::min(td,ts) < .5*std::max(td,ts))
        timing = td < ts ? 0 : 1;
    const double bd = e[0][0].score+e[1][1].score, bs = e[0][1].score+e[1][0].score;
    if (std::isfinite(bd) && std::isfinite(bs) && std::abs(bd-bs) > .3) brightness = bd > bs ? 0 : 1;
    if (timing >= 0 && brightness >= 0 && timing != brightness) { v.reason = "timing and brightness disagree"; return v; }
    if (timing < 0 && brightness < 0) { v.reason = "no decisive timing or brightness difference"; return v; }
    v.mapping = timing >= 0 ? timing : brightness;
    v.basis = timing >= 0 && brightness >= 0 ? "timing,brightness" : timing >= 0 ? "timing" : "brightness";
    return v;
}
} // namespace pico::lower_map
