#include "device_config.hpp"
// Content check of the lower xrtap node -> XRService serial mapping, offline, no decoder.
// usage: lower_map <pgm-dir> <dataset.occ> [tolerance_ms]
// <pgm-dir> holds xrtap frames video9_<v4l2 ns>.pgm and video13_<v4l2 ns>.pgm from the same session
// as the .occ recording. Per node the mean brightness sequence is compared with each OCC lower
// stream's per-frame metadata (timestamp jitter, exposure, coded access-unit size). Prints MAPPING
// (exit 0) or UNDECIDED (exit 3); the numbers are printed either way.
#include "camera_transport.hpp"
#include "lower_map.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>

namespace {
using pico::lower_map::Evidence;
using pico::lower_map::pearson;
const char *nodes[2] = {"video9", "video13"};
const std::string serials[2] = {pico::devices::lower(0), pico::devices::lower(1)}; // current unverified default order
struct Frame { int64_t ts; double mean; bool short_ir; };
struct Unit { int64_t ts; double exposure, bytes; };

bool read_pgm(const std::filesystem::path &path, double &mean) {
    std::ifstream in(path, std::ios::binary);
    std::string magic;
    if (!(in >> magic) || magic != "P5") return false;
    int values[3] = {};
    for (int &v : values) {
        in >> std::ws;
        while (in.peek() == '#') { std::string comment; std::getline(in, comment); in >> std::ws; }
        if (!(in >> v) || v <= 0) return false;
    }
    if (values[2] > 255) return false;
    in.get();
    std::vector<unsigned char> pixels(size_t(values[0])*size_t(values[1]));
    if (!in.read(reinterpret_cast<char *>(pixels.data()), std::streamsize(pixels.size()))) return false;
    double sum = 0;
    for (auto p : pixels) sum += p;
    mean = sum/double(pixels.size());
    return true;
}
// Cadence rule of XrtapSource: a frame less than 15 ms after the previous one is the short IR frame.
std::vector<Frame> load_node(const std::filesystem::path &dir, const std::string &node) {
    const std::regex name(node + "_(-?[0-9]+)\\.pgm");
    std::vector<Frame> out;
    for (const auto &entry : std::filesystem::directory_iterator(dir)) {
        std::smatch m;
        const auto file = entry.path().filename().string();
        if (!std::regex_match(file, m, name)) continue;
        double mean = 0;
        if (!read_pgm(entry.path(), mean)) { std::cerr << "unreadable PGM " << file << '\n'; continue; }
        out.push_back({std::stoll(m[1].str()), mean, false});
    }
    std::sort(out.begin(), out.end(), [](const Frame &a, const Frame &b) { return a.ts < b.ts; });
    for (size_t i = 1; i < out.size(); ++i) out[i].short_ir = out[i].ts-out[i-1].ts < 15000000;
    return out;
}
std::map<std::string, std::vector<Unit>> load_occ(const std::filesystem::path &path) {
    std::map<std::string, std::vector<Unit>> out;
    pico::camera::GrowingOcc occ(path);
    for (;;) {
        const auto records = occ.poll();
        if (records.empty()) break;
        for (const auto &r : records) for (const auto &u : pico::camera::camera_packets(r.kind, r.payload)) {
            if (!u.sensor || !u.timestamp) continue;
            if (*u.sensor != serials[0] && *u.sensor != serials[1]) continue;
            out[*u.sensor].push_back({int64_t(std::llround(*u.timestamp*1e9)), u.exposure ? *u.exposure : NAN,
                                      double(u.annex_b.size())});
        }
    }
    for (auto &entry : out)
        std::sort(entry.second.begin(), entry.second.end(), [](const Unit &a, const Unit &b) { return a.ts < b.ts; });
    return out;
}
Evidence compare(const std::string &node, const std::string &serial, const std::vector<Frame> &frames,
                 const std::vector<Unit> &units, int64_t tolerance) {
    Evidence e;
    if (frames.empty() || units.empty()) return e;
    // Nearest unit per frame, then keep the matches at the common (median) offset.
    std::vector<std::pair<size_t,size_t>> nearest;
    std::vector<int64_t> deltas;
    for (size_t i = 0; i < frames.size(); ++i) {
        auto it = std::lower_bound(units.begin(), units.end(), frames[i].ts,
                                   [](const Unit &u, int64_t t) { return u.ts < t; });
        size_t j = size_t(it-units.begin());
        if (j == units.size() || (j > 0 && frames[i].ts-units[j-1].ts < units[j].ts-frames[i].ts)) --j;
        nearest.push_back({i, j});
        deltas.push_back(frames[i].ts-units[j].ts);
    }
    auto sorted = deltas;
    std::nth_element(sorted.begin(), sorted.begin()+std::ptrdiff_t(sorted.size()/2), sorted.end());
    const int64_t median = sorted[sorted.size()/2];
    double square = 0;
    std::vector<double> mean[2], exposure[2], bytes[2];
    for (size_t k = 0; k < nearest.size(); ++k) {
        const auto d = deltas[k]-median;
        if (std::abs(d) > tolerance) continue;
        ++e.matched; square += double(d)*double(d);
        const auto &f = frames[nearest[k].first];
        const auto &u = units[nearest[k].second];
        mean[f.short_ir].push_back(f.mean); exposure[f.short_ir].push_back(u.exposure); bytes[f.short_ir].push_back(u.bytes);
    }
    if (e.matched) e.jitter_us = std::sqrt(square/double(e.matched))/1000;
    // Correlate within each exposure kind: the shared normal/short alternation says nothing about the camera.
    double sum = 0; int count = 0;
    double reported[2][2] = {{NAN, NAN}, {NAN, NAN}};
    for (int kind = 0; kind < 2; ++kind) {
        if (mean[kind].size() < 20) continue;
        const double c[2] = {pearson(mean[kind], exposure[kind]), pearson(mean[kind], bytes[kind])};
        for (int i = 0; i < 2; ++i) { reported[kind][i] = c[i]; if (std::isfinite(c[i])) { sum += c[i]; ++count; } }
    }
    if (count) e.score = sum/count;
    std::cout << "pair node=/dev/" << node << " serial=" << serial << " matched=" << e.matched
              << " median_offset_ms=" << double(median)/1e6 << " jitter_us=" << e.jitter_us
              << " normal_corr_exposure=" << reported[0][0] << " normal_corr_bytes=" << reported[0][1]
              << " short_corr_exposure=" << reported[1][0] << " short_corr_bytes=" << reported[1][1]
              << " score=" << e.score << '\n';
    return e;
}
} // namespace

int main(int argc, char **argv) {
    if (argc < 3 || argc > 4) {
        std::cerr << "usage: lower_map <pgm-dir> <dataset.occ> [tolerance_ms]\n";
        return 2;
    }
    try {
        const int64_t tolerance = int64_t((argc == 4 ? std::stod(argv[3]) : 2.0)*1e6);
        std::vector<Frame> frames[2];
        for (int n = 0; n < 2; ++n) {
            frames[n] = load_node(argv[1], nodes[n]);
            size_t short_ir = 0;
            for (const auto &f : frames[n]) short_ir += f.short_ir;
            std::cout << "pgm node=/dev/" << nodes[n] << " frames=" << frames[n].size() << " short=" << short_ir << '\n';
        }
        auto units = load_occ(argv[2]);
        for (const auto &serial : serials)
            std::cout << "occ serial=" << serial << " units=" << units[serial].size() << '\n';
        std::array<std::array<Evidence,2>,2> evidence;
        for (int n = 0; n < 2; ++n) for (int s = 0; s < 2; ++s)
            evidence[n][s] = compare(nodes[n], serials[s], frames[n], units[serials[s]], tolerance);
        const auto v = pico::lower_map::decide(evidence);
        if (v.mapping < 0) { std::cout << "UNDECIDED reason=\"" << v.reason << "\"\n"; return 3; }
        std::cout << "MAPPING /dev/" << nodes[0] << '=' << serials[v.mapping] << " /dev/" << nodes[1] << '='
                  << serials[1-v.mapping] << " basis=" << v.basis
                  << (v.mapping == 0 ? " (matches the default)" : " (default is swapped)") << '\n';
        return 0;
    } catch (const std::exception &e) {
        std::cerr << "lower_map: " << e.what() << '\n';
        return 1;
    }
}
