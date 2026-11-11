#pragma once
#include "data_plane.hpp"
#include <array>
#include <cstdint>
#include <optional>
namespace pico {
struct OpticalPoseFrame {
    uint32_t chip = 0;
    uint64_t sequence = 0;
    int64_t stamp = 0;
    std::array<double,3> position{}, velocity{}, omega{};
    std::array<double,4> quaternion{0,0,0,1}; // xyzw, OpenVR standing space
    bool connected = false, valid = false;
    double quality = 0;
    std::optional<double> battery;
    std::array<uint8_t,88> encode(int64_t now) const;
};
// Linux loopback UDP; production default 39571, explicit test destinations allowed.
class OpticalPosePublisher {
    int fd_ = -1;
    uint16_t port_;
    uint64_t dropped_ = 0;
  public:
    explicit OpticalPosePublisher(uint16_t port = 39571);
    OpticalPosePublisher(const OpticalPosePublisher &) = delete;
    OpticalPosePublisher &operator=(const OpticalPosePublisher &) = delete;
    ~OpticalPosePublisher();
    // False (logged) for an invalid/stale pose or socket failure; never throws on pose data.
    bool send(const OpticalPoseFrame &, int64_t now);
    uint64_t dropped() const { return dropped_; }
};
} // namespace pico


