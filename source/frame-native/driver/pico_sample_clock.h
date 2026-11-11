#pragma once
// IMU sample time from the sensor's own cadence (header-only, replay-testable), 2026-10-30.
//
// With the short-ring station module the tracker's 8-bit sample counter advances by one per sensor sample
// (25664 of 25711 steps in captures/static200-1, sensor ~201.7 Hz). A sample's physical time is then
//   t(index) = a + b * index
// with b the sensor period and a the offset. Receive times only add a positive, jittery transport delay, so the
// lower envelope of (receive - index * nominal) per 250 ms bucket lies on that line. The line is a least-squares fit
// of the bucket minima over the last 20 s; b is measured, not taken from the calibration (4.957 ms observed vs
// 4.94 nominal). Until 4 buckets exist the caller keeps its receive-based stamp.
// Counter wraps are resolved from the receive-time gap, as in ImuSampleClock.
#include <cmath>
#include <cstdint>
#include <deque>

namespace pico_fusion {

class SensorSampleClock {
public:
    explicit SensorSampleClock(double nominalPeriodNs) : nominal_(nominalPeriodNs) {}

    // receiveNs: the record's receive time (host clock). Returns false while the line is not fitted yet or the sample
    // is a duplicate; otherwise writes the sample time.
    bool Observe(uint8_t counter, double receiveNs, double &sampleNs) {
        int64_t index;
        if (!have_) {
            have_ = true; index = 0; newest_ = 0; counter_ = counter; lastReceive_ = receiveNs;
        } else {
            const int mod = (counter - counter_) & 255;
            const double expected = (receiveNs - lastReceive_) / nominal_;
            int64_t delta;
            if (mod == 0 && expected < 128) return false;                // duplicate of the newest
            if (mod > 127 && expected < 64) delta = mod - 256;            // older sample, behind the newest
            else {
                double k = std::round((expected - mod) / 256.0);
                if (k < 0) k = 0;
                delta = mod + int64_t(k) * 256;
            }
            index = newest_ + delta;
            if (delta > 0) { newest_ = index; counter_ = counter; lastReceive_ = receiveNs; }
        }
        Add(index, receiveNs);
        if (!fitted_) return false;
        sampleNs = a_ + b_ * double(index);
        return true;
    }
    bool Fitted() const { return fitted_; }
    double PeriodNs() const { return fitted_ ? b_ : nominal_; }

private:
    struct Point { int64_t bucket; int64_t index; double residual; };
    void Add(int64_t index, double receiveNs) {
        const double residual = receiveNs - double(index) * nominal_;
        const int64_t bucket = int64_t(std::floor(receiveNs / 250e6));
        bool found = false;
        for (auto &p : points_)
            if (p.bucket == bucket) { if (residual < p.residual) p = {bucket, index, residual};
            found = true; break; }
        if (!found) {
            if (!points_.empty() && bucket < points_.back().bucket) return;
            points_.push_back({bucket, index, residual});
        }
        while (!points_.empty() && points_.back().bucket - points_.front().bucket > 80) points_.pop_front();  // 20 s
        if (points_.size() < 4) return;
        // Least squares residual = c + d * index over the bucket minima (index centred for conditioning).
        double mi = 0; for (auto &p : points_) mi += double(p.index);
        mi /= double(points_.size());
        double sx = 0, sy = 0, sxx = 0, sxy = 0, n = double(points_.size());
        for (auto &p : points_) { const double x = double(p.index) - mi; sx += x; sy += p.residual; sxx += x * x; sxy += x * p.residual; }
        const double den = n * sxx - sx * sx;
        if (!(den > 0)) return;
        const double d = (n * sxy - sx * sy) / den, c = (sy - d * sx) / n;
        // A slope beyond +-2 % of the nominal period is a mis-indexed bucket, not the sensor: keep the previous line.
        if (std::fabs(d) > 0.02 * nominal_) return;
        b_ = nominal_ + d;
        a_ = c - d * mi;
        fitted_ = true;
    }

    double nominal_, a_ = 0, b_ = 0, lastReceive_ = 0;
    bool have_ = false, fitted_ = false;
    int64_t newest_ = 0;
    int counter_ = 0;
    std::deque<Point> points_;
};

}  // namespace pico_fusion
