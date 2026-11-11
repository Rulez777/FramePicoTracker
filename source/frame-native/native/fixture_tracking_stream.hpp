#pragma once
#include <cstdint>
#include <optional>
#include <stdexcept>

namespace pico::live_fixture {
// A read-driven synthetic 90 Hz observer. Generate only ticks already reached
// by the session clock; never replay a precomputed wall-clock script.
class TrackingStream {
    std::optional<int64_t> origin_, last_;
public:
    std::optional<int64_t> read(int64_t now) {
        if (!origin_) origin_ = now;
        if (now < *origin_ || (last_ && now < *last_))
            throw std::runtime_error("Fixture observer clock regressed");
        constexpr int64_t period = 11111111;
        if (last_ && now - *last_ < period) return {}; // EAGAIN until next tick
        const auto stamp = last_ ? *last_ + period : now;
        last_ = stamp;
        return stamp;
    }
};
}
