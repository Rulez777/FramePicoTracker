// occ_annexb <file.occ> <out_dir>: write each upper short-exposure stream (kind 38) as raw Annex B
// (<sensor>.h264) plus its packet timestamps (<sensor>.ts), for offline comparison with xrtap frames.
#include "live_camera.hpp"
#include <cstdio>
#include <iostream>
#include <map>
using namespace pico::camera;
int main(int argc, char** argv) {
    try {
        if (argc != 3) throw std::runtime_error("Usage: occ_annexb <file.occ> <out_dir>");
        GrowingOcc reader(argv[1]);
        std::map<std::string, std::pair<FILE*, FILE*>> out;
        for (;;) {
            auto records = reader.poll(1);
            if (records.empty()) break;
            for (auto& r : records)
                for (auto a : camera_packets(r.kind, r.payload)) {
                    if (a.key.kind != 38 || !a.upper() || !a.sensor) continue;
                    auto& f = out[*a.sensor];
                    if (!f.first) {
                        f.first = std::fopen((std::string(argv[2]) + "/" + *a.sensor + ".h264").c_str(), "wb");
                        f.second = std::fopen((std::string(argv[2]) + "/" + *a.sensor + ".ts").c_str(), "w");
                        if (!f.first || !f.second) throw std::runtime_error("Cannot create output");
                    }
                    std::fwrite(a.annex_b.data(), 1, a.annex_b.size(), f.first);
                    std::fprintf(f.second, "%.9f\n", a.timestamp.value_or(0));
                }
        }
        for (auto& f : out) { std::fclose(f.second.first); std::fclose(f.second.second); }
        return 0;
    } catch (const std::exception& e) { std::cerr << "occ_annexb: " << e.what() << '\n'; return 1; }
}
