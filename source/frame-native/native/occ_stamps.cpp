// occ_stamps <file.occ>: print every camera packet's sensor, stream, RAW timestamp and exposure
// (diagnostic for mapping xrtap V4L2 timestamps to XRService exposure stamps).
#include "live_camera.hpp"
#include <cstdio>
#include <iostream>
using namespace pico::camera;
int main(int argc, char** argv) {
    try {
        if (argc != 2) throw std::runtime_error("Usage: occ_stamps <file.occ>");
        GrowingOcc reader(argv[1]);
        for (;;) {
            auto records = reader.poll(1);
            if (records.empty()) break;
            for (auto& r : records)
                for (auto a : camera_packets(r.kind, r.payload))
                    std::printf("%s kind=%u sel=%u ord=%u upper=%d ts=%.9f exp=%.9f\n", a.sensor.value_or("?").c_str(),
                                unsigned(a.key.kind), unsigned(a.key.selector), unsigned(a.key.ordinal), int(a.upper()),
                                a.timestamp.value_or(0), a.exposure.value_or(-1));
        }
        return 0;
    } catch (const std::exception& e) { std::cerr << "occ_stamps: " << e.what() << '\n'; return 1; }
}
