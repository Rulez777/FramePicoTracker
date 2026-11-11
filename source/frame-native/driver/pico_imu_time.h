#pragma once
// IMU sample time from the 8-bit sample counter and the host receive time (header-only, replay-testable).
//
// The tracker samples at a fixed period (~4.94 ms) and sends an 8-bit counter; packets reach the host in bursts with
// a variable transport delay. A sample's physical time = index * period + offset, where offset is the minimum transport
// delay (lower envelope of receive - index*period).
//
// The previous host implementation unwrapped the counter by "delta in 1..127", so any radio gap longer than 127
// samples (0.63 s) shifted the index by 256 and the drift check then rejected every sample for the 30 s envelope window
// (r13b: a 29.79 s IMU blackout). Here the wrap count is chosen from the receive-time gap, and a sample that is
// inconsistent with the envelope is dropped individually instead of poisoning the fit.
#include <cmath>
#include <cstdint>
#include <deque>

namespace pico_fusion {

class ImuSampleClock {
public:
    explicit ImuSampleClock(double periodNs) : period_(periodNs) {}

    // Returns false if the sample is a duplicate/unplaceable; otherwise writes its estimated physical time.
    bool Observe(uint8_t counter, double receiveNs, double &sampleNs) {
        if (!have_) {
            have_ = true; index_ = 0; counter_ = counter; lastReceive_ = receiveNs;
            AddEnvelope(index_, receiveNs);
            sampleNs = receiveNs;  // first sample: no envelope yet
            return Ready(index_, sampleNs);
        }
        int mod = (counter - counter_) & 255;
        double expected = (receiveNs - lastReceive_) / period_;  // samples elapsed by receive time (jittery)
        int64_t delta;
        if (mod == 0 && expected < 128) return false;            // duplicate
        if (mod > 127 && expected < 64) {
            delta = mod - 256;                                     // late, reordered sample (behind newest)
        } else {
            // choose delta = mod + 256k closest to the receive-time estimate
            double k = std::round((expected - mod) / 256.0);
            if (k < 0) k = 0;
            delta = mod + int64_t(k) * 256;
        }
        int64_t index = index_ + delta;
        if (delta > 0) { index_ = index; counter_ = counter; lastReceive_ = receiveNs; }
        // Owner rule 2026-10-29: an early sample is counted, not dropped; the envelope fit absorbs it.
        if (fitted_ && receiveNs - (index * period_ + offset_) < -20e6) rejected_++;

        AddEnvelope(index, receiveNs);
        return Ready(index, sampleNs);
    }
    uint64_t Rejected() const { return rejected_; }
    bool Fitted() const { return fitted_; }

private:
    struct Point { int64_t index; double residual; int64_t bucket; };
    void AddEnvelope(int64_t index, double receiveNs) {
        double residual = receiveNs - index * period_;
        int64_t bucket = int64_t(receiveNs / 1e9);
        // one minimum per receive second (jittered receive times may revisit an older second)
        bool found = false;
        for (auto &p : env_) if (p.bucket == bucket) { if (residual < p.residual) p = {index, residual, bucket}; found = true; }
        if (!found) {
            if (!env_.empty() && bucket < env_.back().bucket) return;  // a second already rotated out
            env_.push_back({index, residual, bucket});
        }
        while (!env_.empty() && env_.back().bucket - env_.front().bucket > 10) env_.pop_front();
        // offset = median of the per-second minima (robust to one mis-indexed second)
        if (env_.size() >= 3) {  // >=3 receive-seconds: the first second alone biases the offset by tens of ms
            double v[16]; size_t n = 0;
            for (auto it = env_.rbegin(); it != env_.rend() && n < 16; ++it) v[n++] = it->residual;
            for (size_t i = 1; i < n; ++i) for (size_t j = i; j > 0 && v[j - 1] > v[j]; --j) std::swap(v[j - 1], v[j]);
            offset_ = v[n / 2]; fitted_ = true;
        }
    }
    bool Ready(int64_t index, double &sampleNs) const {
        if (!fitted_) return false;
        sampleNs = index * period_ + offset_;
        return true;
    }

    double period_;
    bool have_ = false, fitted_ = false;
    int64_t index_ = 0;
    int counter_ = 0;
    double lastReceive_ = 0, offset_ = 0;
    std::deque<Point> env_;
    uint64_t rejected_ = 0;
};

}  // namespace pico_fusion
