#pragma once
#include "mag_store.h"
namespace pico_fusion {
struct MagTimed {
  double t;
  Vec3 b;
};
struct RotationSpan {
  double first, last;
};
inline void ObserveRotation(std::vector<RotationSpan> &spans, double t,
                            double gyroRadPerSec) {
  if (gyroRadPerSec <= .3)
    return;
  if (spans.empty() || t - spans.back().last > 2e9)
    spans.push_back({t, t});
  else
    spans.back().last = t;
}
struct RotationFit {
  RotationSpan span;
  MagFit fit;
  size_t inputSamples = 0;
};
// Split rejected motion spans by SAMPLE COUNT, retain all attempts. No session
// time constants.
inline std::vector<RotationFit> FitRotations(const std::vector<MagTimed> &raw,
                                             std::vector<RotationSpan> spans) {
  const size_t initialSpans = spans.size();
  std::vector<RotationFit> result;
  for (size_t k = 0; k < spans.size(); ++k) {
    auto span = spans[k];
    if (span.last - span.first < 3e9)
      continue;
    std::vector<Vec3> b;
    std::vector<double> t;
    for (auto s : raw)
      if (s.t >= span.first && s.t <= span.last) {
        b.push_back(s.b);
        t.push_back(s.t);
      }
    if (b.size() < 240)
      continue;
    auto f = MagSphere(b);
    std::vector<Vec3> a(b.begin(), b.begin() + b.size() / 2),
        c(b.begin() + b.size() / 2, b.end());
    auto l = MagSphere(a), r = MagSphere(c);
    f.halfDifference =
        l.radius && r.radius
            ? Norm(Sub(l.calibration.offset, r.calibration.offset))
            : 1e9;
    Vec3 lo{1e9, 1e9, 1e9}, hi{-1e9, -1e9, -1e9};
    for (auto p : b) {
      lo = {std::min(lo.x, p.x), std::min(lo.y, p.y), std::min(lo.z, p.z)};
      hi = {std::max(hi.x, p.x), std::max(hi.y, p.y), std::max(hi.z, p.z)};
    }
    f.envelopeDifference =
        Norm(Sub(f.calibration.offset, Scale(Add(lo, hi), .5)));
    if (f.samples < 240)
      f.qualityFailures |= 1;
    if (f.radius < 30 || f.radius > 65)
      f.qualityFailures |= 8;
    if (f.halfDifference >= 6)
      f.qualityFailures |= 16;
    f.ready = !f.qualityFailures && f.radius > 0;
    result.push_back({span, f, b.size()});
    // Overlapping coarse boundaries avoid a binary split cutting through a
    // useful rotation. Search is disclosed; acceptance still requires
    // independently fitted temporal halves.
    if (k < initialSpans && !f.ready && b.size() >= 960)
      for (size_t beginBin = 0; beginBin < 5; ++beginBin)
        for (size_t z = beginBin + 5; z <= 10; ++z) {
          if (beginBin == 0 && z == 10)
            continue;
          spans.push_back({t[beginBin * t.size() / 10],
                           t[std::min(t.size() - 1, z * t.size() / 10 - 1)]});
        }
    if (!f.ready && b.size() >= 960) {
      double mid = t[t.size() / 2];
      spans.push_back({span.first, mid - 1});
      spans.push_back({mid, span.last});
    }
  }
  return result;
}
} // namespace pico_fusion
