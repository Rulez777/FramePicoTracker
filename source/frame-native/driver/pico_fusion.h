#pragma once
#include <vector>
#include <algorithm>
// Header-only fusion, no OpenVR dependency. Tracker below implements fixed-lag replay.
// LegacyTracker preserves the input implementation for deterministic before/after comparisons.
// The following description documents that historical implementation only.
//
// Orientation: calibrated gyro is integrated at the IMU rate. Each optical fix arrives ~120 ms late; it is compared with
// the IMU orientation at its own source time (history buffer), gated against LED mis-association bursts, and applied
// as a world-frame correction (complementary gain). Bias is learned only in sustained stillness.
// Position: delayed optical anchors with bounded, leaky accelerometer integration and smooth reacquisition.
// Output at any time: orientation propagated from the last IMU sample, position, and world-frame velocities so SteamVR
// can extrapolate between driver frames.
#include <cmath>
#include <cstdint>
#include <cstring>
#include <deque>
#include <mutex>

#ifndef PICO_POS_BETA
#define PICO_POS_BETA 0.7  // 2026-10-31 evening (IMU fresh after TLMM SWD): owner6dof-1/motion2/hand8-2 moving error at exposure p50 18.4/22.0/30.3 -> 11.5/17.4/22.0 mm, lag motion2 40 -> 30 ms, jerk>5mm 269/179/117 -> 253/190/128, rest unchanged. Was 0.3: motion2 replay with PICO_ACCEL_TRACKED: translation lag 100 -> 40 ms (was 0.15: hand3 rest velocity max 92 -> 51 mm/s)
#endif
#ifndef PICO_A_DEADBAND
#define PICO_A_DEADBAND 0.0
#endif
#ifndef PICO_ZUPT_GYRO
#define PICO_ZUPT_GYRO 0.0
#endif
#ifndef PICO_AB_TAU
#define PICO_AB_TAU 2.0
#endif
#ifndef PICO_DR_NS
#define PICO_DR_NS 0
#endif
#ifndef PICO_LEAK
#define PICO_LEAK 2.0  // motion2 replay with PICO_ACCEL_TRACKED (was 0.6)
#endif
#ifndef PICO_POS_TAU
#define PICO_POS_TAU 0.07  // 2026-10-31 (with ACCEL_TRACKED): lag 60 -> 50 ms, jumps 11 -> 19 (motion2); 0.05 tripled the jumps. Was 0.1 (hand2: 0.025 gave 99 jumps)
#endif
#ifndef PICO_POS_GAIN
#define PICO_POS_GAIN 0.95  // 2026-10-31 evening, with PICO_POS_BETA 0.7 (was 0.85)
#endif
// Visible orientation correction speed limit, rad/s = GAIN*|w| + BASE (replay-tuned).
#ifndef PICO_CORR_GAIN
#define PICO_CORR_GAIN .20
#endif
#ifndef PICO_CORR_BASE
#define PICO_CORR_BASE .02
#endif
// Rest smoothing (2026-10-30): while the smoothed gyro rate is below PICO_STILL_W rad/s and the optical innovation is
// small, optical fixes use these low gains (0 disables). PICO_OPT_RATE_ALPHA smooths the optics-only angular rate.
#ifndef PICO_STILL_W
#define PICO_STILL_W 0.05  // rad/s; hand3-ack rest replay: rotation dev p90 1.09 -> 0.33 deg, position 1.22 -> 0.82 mm
#endif
#ifndef PICO_STILL_ALPHA
#define PICO_STILL_ALPHA 0.2
#endif
#ifndef PICO_GRAVITY_ALPHA
#define PICO_GRAVITY_ALPHA 0.05  // body model gravity low-pass per IMU sample (~0.1 s at 200 Hz); output only
#endif
#ifndef PICO_STILL_POS_ERR
#define PICO_STILL_POS_ERR 0.010
#endif
#ifndef PICO_STILL_ORI_ERR
#define PICO_STILL_ORI_ERR (10.0*M_PI/180)
#endif
#ifndef PICO_POS_GAIN_STILL
#define PICO_POS_GAIN_STILL 0.05
#endif
#ifndef PICO_POS_BETA_STILL
#define PICO_POS_BETA_STILL 0.0
#endif
#ifndef PICO_STILL_VDECAY
#define PICO_STILL_VDECAY 0.5
#endif
#ifndef PICO_ORI_GAIN_STILL
#define PICO_ORI_GAIN_STILL 0.01
#endif
#ifndef PICO_OPT_RATE_ALPHA
#define PICO_OPT_RATE_ALPHA 0.0  // optics-only rate from 33 ms fix differences was noise: 22 deg rest swing
#endif
// Grok review 2026-10-30: C.1 rest rate low-pass ON (alpha 0.1); rest correction base 0 and C.2 tested and rejected (OFF):
// base 0 raised >20 deg disagreement 167 -> 244; C.2 stuck on a wrong orientation (167 -> 984, held position error 239 mm).
// Reanchor evidence (2026-10-30): consecutive mutually consistent optical fixes that all disagree with the IMU.
// Render residual larger than this (rad) is applied at once (2026-10-30); a large value disables it.
#ifndef PICO_RESIDUAL_SNAP_RAD
#define PICO_RESIDUAL_SNAP_RAD 1.0472  // 60 deg (2026-11-02: was 30 deg; returns after 10-60 s outages carry 20-40 deg and snapped up to 37 deg in one frame); replays: >20 deg output-vs-optical fixes -45..70 %, longest run 219 -> 14
#endif
// A residual above PICO_RESIDUAL_FAST_RAD (an optics return after a gap) converges at least with time constant PICO_RESIDUAL_FAST_TAU s
// instead of the speed-bounded bleed (2026-11-02, mag-map-claude2/flip); 0 disables.
#ifndef PICO_RESIDUAL_FAST_RAD
#define PICO_RESIDUAL_FAST_RAD 0.0873  // 5 deg
#endif
#ifndef PICO_RESIDUAL_FAST_TAU
#define PICO_RESIDUAL_FAST_TAU 0.25
#endif
#ifndef PICO_FLIP_CONSECUTIVE
#define PICO_FLIP_CONSECUTIVE 5
#endif
#ifndef PICO_FLIP_MAX_NS
#define PICO_FLIP_MAX_NS 200e6
#endif
// Flip gate after an optics gap (2026-11-02): the gyro yaw drifts ~0.5-1 deg/s, so after 20-60 s without optics a correct fix can
// exceed 25 deg; it was rejected until 5 consistent fixes forced a reanchor jump (up to 24 deg / 13 cm, mag-map-claude2/long).
// The gate grows by this rate with the time since the last accepted orientation, up to the cap; 0 = old fixed gate.
#ifndef PICO_FLIP_GROW_DEG_S
#define PICO_FLIP_GROW_DEG_S 1.0
#endif
#ifndef PICO_FLIP_GATE_MAX_DEG
#define PICO_FLIP_GATE_MAX_DEG 60.0
#endif
#ifndef PICO_REST_RATE_ALPHA
#define PICO_REST_RATE_ALPHA 0.1  // C.1 replay (candidate cal): rest rotation step p90 0.0188 -> 0.0038 deg, motion metrics unchanged
#endif
#ifndef PICO_REST_CORR_BASE
#define PICO_REST_CORR_BASE PICO_CORR_BASE
#endif
#ifndef PICO_NO_REANCHOR_FRESH_IMU
#define PICO_NO_REANCHOR_FRESH_IMU 0
#endif
#ifndef PICO_REANCHOR_FRESH_COUNT
#define PICO_REANCHOR_FRESH_COUNT 1000000
#endif
#ifndef PICO_POS_CORR_GAIN
#define PICO_POS_CORR_GAIN 0.0  // 0 = exponential render-residual decay (PICO_POS_TAU); >0 = speed-bounded correction
#endif
#ifndef PICO_POS_CORR_BASE
#define PICO_POS_CORR_BASE 0.02  // m/s, correction speed at rest (speed-bounded mode)
#endif
#ifndef PICO_POS_CORR_TAU
#define PICO_POS_CORR_TAU 0.02  // s, fastest residual removal (speed-bounded mode)
#endif
#ifndef PICO_POS_GAMMA
#define PICO_POS_GAMMA 0.0  // alpha-beta-gamma accelerometer correction gain (0 = off)
#endif
#ifndef PICO_AWC_MAX
#define PICO_AWC_MAX 2.0  // m/s^2, bound of the learned correction
#endif
#ifndef PICO_BLIND_KEEP_V
#define PICO_BLIND_KEEP_V 0  // 1 = keep the inertial velocity when the optics returns (alpha-beta corrects it)
#endif
#ifndef PICO_AB_REST_ONLY
#define PICO_AB_REST_ONLY 0
#endif
#ifndef PICO_ZUPT_STILL
#define PICO_ZUPT_STILL 0.0  // rad/s smoothed gyro rate below which (with |a|~1 g) the velocity is stopped (0 = off)
#endif
#ifndef PICO_PIVOT
#define PICO_PIVOT 0  // 2026-10-31 evening OFF: on REAL optical gaps (owner6dof-1, motion2, hand8-2) it is no better than hold and adds opposite-direction shifts (hand8-2 1 -> 5); the lever fit residual is 20-60 mm (hand translation, not a segment). Was 1: body-segment model (anchor + (R(q)-R(q_anchor)) r)
#endif
#ifndef PICO_PIVOT_TAU
#define PICO_PIVOT_TAU 1.0  // s, forgetting time of the lever fit
#endif
#ifndef PICO_PIVOT_RIDGE
#define PICO_PIVOT_RIDGE 0.05  // ridge on the lever (relative to the fit weight): no rotation -> lever 0 -> hold
#endif
#ifndef PICO_PIVOT_AFTER_NS
#define PICO_PIVOT_AFTER_NS 150e6  // switch to the segment model where the optical extrapolation ends (positionExtrapolateNs)
#endif
#ifndef PICO_PIVOT_MAX_LEVER
#define PICO_PIVOT_MAX_LEVER 0.8  // m
#endif
#ifndef PICO_PIVOT_MAX_RMS
#define PICO_PIVOT_MAX_RMS 1e9  // m, RMS 3D lever-fit residual above which the segment model is not used
#endif
#ifndef PICO_BIAS_LEARN
#define PICO_BIAS_LEARN 1  // 2026-10-31: learn the gyro bias at rest (LearnBias); zero-bias start on ack12-bench converges to 14.2/-4.2/2.5 (cal 14/-4/3), rest error vs optics 15.6 -> 0.25 deg p50; motion metrics unchanged
#endif
#ifndef PICO_BIAS_WINDOW_S
#define PICO_BIAS_WINDOW_S 1.0
#endif
#ifndef PICO_BIAS_STILL_STD
#define PICO_BIAS_STILL_STD 3.0  // LSB
#endif
#ifndef PICO_BIAS_TAU_S
#define PICO_BIAS_TAU_S 5.0
#endif
#ifndef PICO_BIAS_MAX_LSB
#define PICO_BIAS_MAX_LSB 60.0
#endif
#ifndef PICO_VEL_RESIDUAL
#define PICO_VEL_RESIDUAL 1  // 2026-10-31 (Grok): published linear velocity includes the render-residual decay
#endif
#ifndef PICO_ORI_GAIN
#define PICO_ORI_GAIN 0.25  // fraction of the optical orientation error applied per fix
#endif
#ifndef PICO_ACCEL_MOVING_V
#define PICO_ACCEL_MOVING_V 0.05  // m/s optical velocity that also counts as moving (pure translation, no rotation)
#endif
#ifndef PICO_ACCEL_TRACKED
#define PICO_ACCEL_TRACKED 1  // motion2 replay: shown position lag 100 -> 40 ms, error at exposure p50 54 -> 24 mm, jumps 10 -> 11; 1 = integrate the accelerometer into velocity while optics is fresh, too
#endif
namespace pico_fusion {

struct Quat { double w = 1, x = 0, y = 0, z = 0; };
struct Vec3 { double x = 0, y = 0, z = 0; };

inline Quat Mul(const Quat &a, const Quat &b) {
    return {a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z, a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
            a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x, a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w};
}
inline Quat Conj(const Quat &q) { return {q.w, -q.x, -q.y, -q.z}; }
inline Quat Normalize(Quat q) {
    double n = std::sqrt(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
    if (n < 1e-12) return Quat{};
    q.w /= n; q.x /= n; q.y /= n; q.z /= n;
    return q;
}
inline Quat Exp(const Vec3 &v) {  // rotation vector -> quaternion
    double a = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (a < 1e-12) return Normalize({1, v.x / 2, v.y / 2, v.z / 2});
    double s = std::sin(a / 2) / a;
    return {std::cos(a / 2), v.x * s, v.y * s, v.z * s};
}
inline Vec3 Log(Quat q) {  // quaternion -> rotation vector (shortest)
    if (q.w < 0) { q.w = -q.w; q.x = -q.x; q.y = -q.y; q.z = -q.z; }
    double s = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z);
    if (s < 1e-12) return {2 * q.x, 2 * q.y, 2 * q.z};
    double a = 2 * std::atan2(s, q.w);
    return {q.x / s * a, q.y / s * a, q.z / s * a};
}
inline Vec3 Rotate(const Quat &q, const Vec3 &v) {
    Quat r = Mul(Mul(q, Quat{0, v.x, v.y, v.z}), Conj(q));
    return {r.x, r.y, r.z};
}
inline double Norm(const Vec3 &v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }
inline Vec3 Scale(const Vec3 &v, double s) { return {v.x * s, v.y * s, v.z * s}; }
inline Vec3 Add(const Vec3 &a, const Vec3 &b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
inline Vec3 Sub(const Vec3 &a, const Vec3 &b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }

struct Calibration {
    double r[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};  // body <- imu
    double gyroRadPerLsb = 0;                          // isotropic scale
    Vec3 gyroBiasLsb;                                  // initial bias (raw)
    double imuTimeOffsetNs = 0;                        // add to IMU source time
    bool valid = false;
};

struct Params {
    double flipGateRad = 25.0 * M_PI / 180; // Tracker exposure-time orientation gate
    int flipConsecutive = PICO_FLIP_CONSECUTIVE;
    double flipMaxIntervalNs = PICO_FLIP_MAX_NS; // separated fixes are not consecutive evidence
    double orientationGain = PICO_ORI_GAIN;          // fraction of the optical orientation error applied per fix
    double gateRad = 20.0 * M_PI / 180;     // optical vs IMU disagreement beyond this is suspicious
    int gateConsecutive = 4;                // ...unless this many consecutive fixes agree with each other
    double biasGain = 0.0;                  // compatibility only; correction-driven bias learning is disabled
    double maxImuStepNs = 60e6;             // longer radio gaps integrate with the last rate, capped
    double maxPredictNs = 300e6;            // output prediction horizon: IMU samples arrive ~150 ms late on the native radio path (2026-10-29)
    double positionExtrapolateNs = 150e6;   // extrapolate optical position at most this long, then hold
    double opticalOkNs = 1500e6;            // Running_OK while an accepted optical fix is this recent
    double imuAliveNs = 500e6;              // pose stays valid while IMU samples keep arriving
    double historyNs = 600e6;
    double reinitGapNs = 200e6;             // an IMU gap longer than this re-initialises orientation from optics
    double deadReckonNs = PICO_DR_NS;      // optical-loss accelerometer carry window (0 = hold position)
    double velocityLeak = PICO_LEAK;             // inverse seconds; applies to inertial propagation
    double positionBlendNs = 150e6;
    double maxOpticalSpeed = 5.0;          // m/s, position outlier gate
    double positionGateSlack = 0.08;       // metres of measurement slack
    double maxAcceleration = 30.0;         // m/s^2, reject impact/invalid acceleration
    double stillWindowNs = 800e6;
    double stillGyroRad = 0.06;
    double stillAccelFraction = 0.035;
    double biasTimeConstant = 8.0;         // seconds of confirmed stillness
    double biasClampLsb = 40.0;            // maximum change from supplied calibration, per axis
};

class LegacyTracker {
public:
    explicit LegacyTracker(const Calibration &c, const Params &p = Params()) : cal_(c), prm_(p), bias_(c.gyroBiasLsb) {}

    // Raw IMU sample with its host CLOCK_MONOTONIC source time.
    void Imu(uint64_t sourceNs, const int16_t gyro[3], const int16_t accel[3]) {
        if (!cal_.valid) return;
        stats_.imu++;
        double t = double(sourceNs) + cal_.imuTimeOffsetNs;
        if (haveImu_ && t <= tImu_) return;
        LearnStill(t, {double(gyro[0]), double(gyro[1]), double(gyro[2])},
                   {double(accel[0]), double(accel[1]), double(accel[2])});
        Vec3 g{gyro[0] - bias_.x, gyro[1] - bias_.y, gyro[2] - bias_.z};
        Vec3 w{cal_.r[0][0] * g.x + cal_.r[0][1] * g.y + cal_.r[0][2] * g.z,
               cal_.r[1][0] * g.x + cal_.r[1][1] * g.y + cal_.r[1][2] * g.z,
               cal_.r[2][0] * g.x + cal_.r[2][1] * g.y + cal_.r[2][2] * g.z};
        w = Scale(w, cal_.gyroRadPerLsb);
        if (haveImu_) {
            if (t <= tImu_) { stats_.imuLate++; return; }  // late/duplicate sample: chronology is owned by the producer
            if (t - tImu_ > prm_.reinitGapNs) { reinit_ = true; stats_.reinitGaps++; }  // orientation lost across a long radio gap
            double dt = std::fmin(t - tImu_, prm_.maxImuStepNs) / 1e9;
            // trapezoid on body rate
            q_ = Normalize(Mul(q_, Exp(Scale(Add(w, wBody_), dt / 2))));
        }
        Vec3 rawA{double(accel[0]), double(accel[1]), double(accel[2])};
        Vec3 bodyA = Scale(Body(rawA), 9.80665 / gravityLsb_);
        if (havePos_ && haveImu_) AdvancePosition(tImu_, t, bodyA, q_, drPos_, drVel_);
        tImu_ = t; wBody_ = w; haveImu_ = true;
        history_.push_back({t, q_, bodyA});
        while (!history_.empty() && t - history_.front().t > prm_.historyNs) history_.pop_front();
    }

    // Accepted/rejected result of one delayed optical fix (world <- body orientation, position in metres).
    bool Optical(uint64_t sourceNs, const Vec3 &position, const Quat &orientation) {
        double t = double(sourceNs);
        Quat qo = Normalize(orientation);
        if (!std::isfinite(Norm(position)) || !std::isfinite(orientation.w) ||
            !std::isfinite(orientation.x) || !std::isfinite(orientation.y) || !std::isfinite(orientation.z)) return false;
        if (havePos_ && (t <= tPos_ || Norm(Sub(position, pos_)) >
            prm_.positionGateSlack + prm_.maxOpticalSpeed * (t - tPos_) / 1e9)) { ++rejected_; return false; }
        stats_.optical++; stats_.lastOpticalMinusImuNs = haveImu_ ? t - tImu_ : 0;
        if (!haveImu_ || t - tImu_ > prm_.imuAliveNs) {  // no IMU (yet / any more): optics only
            stats_.opticsOnly++;
            q_ = qo; wBody_ = Vec3{}; reinit_ = true; history_.clear();
            AcceptPosition(t, position); tOptical_ = t; haveOptical_ = true;
            return true;
        }
        // Optical fixes arrive ~120 ms after exposure: compare with the IMU orientation AT the exposure time.
        // A world-frame correction C applied as q <- C q to the whole history is exact for body-rate integration.
        Quat qi;
        if (!OrientationAt(t, qi)) {
            if (t > tImu_ && tImu_ > 0 && t - tImu_ < prm_.maxImuStepNs) qi = q_;  // IMU slightly behind the fix
            else { return false; }                    // older than the history: skip
        }
        Quat err = Normalize(Mul(qo, Conj(qi)));  // world-frame correction
        Vec3 e = Log(err);
        double angle = Norm(e);
        if (!initialized_ || reinit_) {
            // first IMU-backed fix, or the IMU stream had a long gap: adopt the optical orientation exactly
            Rebase(err); initialized_ = true; reinit_ = false; suspicious_ = 0;
            AcceptPosition(t, position); tOptical_ = t; haveOptical_ = true;
            return true;
        }
        if (angle > prm_.gateRad) {
            // Possible LED mis-association. Accept only after several consecutive mutually consistent fixes
            // (real disagreement, e.g. accumulated drift during a long occlusion).
            Vec3 de = Log(Normalize(Mul(qo, Conj(lastRejected_))));
            suspicious_ = (suspicious_ > 0 && Norm(de) < prm_.gateRad / 2) ? suspicious_ + 1 : 1;
            lastRejected_ = qo;
            rejected_++;
            if (suspicious_ < prm_.gateConsecutive) return false;
            suspicious_ = 0;
            Rebase(Exp(e));  // full correction
        } else {
            suspicious_ = 0;
            Rebase(Exp(Scale(e, prm_.orientationGain)));
            // biasGain retained for source compatibility, intentionally unused.
        }
        AcceptPosition(t, position);
        tOptical_ = t; haveOptical_ = true;
        return true;
    }

    struct Output {
        bool valid = false, opticalRecent = false, positionTracked = false;
        bool still = false;          // body model (pico_body.h): smoothed gyro rate at rest with a live IMU
        double opticalAgeNs = 1e18;  // body model: time since the newest accepted optical fix (source time)
        bool gravityValid = false;   // body model: gravityBody is from a live accelerometer
        Vec3 gravityBody;            // body model: low-passed accelerometer in the tracker frame (g units, "up" at rest)
        Vec3 position, linearVelocity, angularVelocityWorld;
        Quat orientation;
    };
    Output Pose(uint64_t nowNs) const {
        Output o;
        double now = double(nowNs);
        if (!haveOptical_) return o;
        bool imuAlive = haveImu_ && initialized_ && now - tImu_ < prm_.imuAliveNs;
        double sinceOptical = now - tOptical_;
        o.opticalRecent = sinceOptical < prm_.opticalOkNs;
        if (!imuAlive && !o.opticalRecent) return o;
        o.valid = true;
        if (imuAlive) {
            double dt = std::fmin(std::fmax(now - tImu_, 0.0), prm_.maxPredictNs) / 1e9;  // smooth per-frame output
            o.orientation = Normalize(Mul(q_, Exp(Scale(wBody_, dt))));
            o.angularVelocityWorld = Rotate(o.orientation, wBody_);
        } else {
            o.orientation = q_;
        }
        PositionAt(now, o.position, o.linearVelocity);
        return o;
    }

    uint64_t Rejected() const { return rejected_; }
    // Diagnostics for the driver log (counts since start).
    struct Stats { uint64_t imu = 0, imuLate = 0, reinitGaps = 0, optical = 0, opticsOnly = 0; double lastOpticalMinusImuNs = 0; };
    const Stats &Diagnostics() const { return stats_; }
    double LastImuNs() const { return tImu_; }
    Vec3 BiasLsb() const { return bias_; }

private:
    struct Sample { double t; Quat q; Vec3 accel; };
    struct StillSample { double t; Vec3 gyro, accel; };

    Vec3 Body(const Vec3 &v) const {
        return {cal_.r[0][0]*v.x + cal_.r[0][1]*v.y + cal_.r[0][2]*v.z,
                cal_.r[1][0]*v.x + cal_.r[1][1]*v.y + cal_.r[1][2]*v.z,
                cal_.r[2][0]*v.x + cal_.r[2][1]*v.y + cal_.r[2][2]*v.z};
    }
    void LearnStill(double t, Vec3 g, Vec3 a) {
        stationary_ = false;
        if (haveImu_ && t - tImu_ > prm_.maxImuStepNs) still_.clear();
        still_.push_back({t, g, a});
        while (still_.size() > 1 && t - still_[1].t >= prm_.stillWindowNs) still_.pop_front();
        if (t - still_.front().t < prm_.stillWindowNs || cal_.gyroRadPerLsb <= 0) return;
        Vec3 meanG{}, meanA{};
        for (const auto &s : still_) {
            if (Norm(Sub(s.gyro, bias_))*cal_.gyroRadPerLsb > prm_.stillGyroRad ||
                std::fabs(Norm(s.accel)/gravityLsb_ - 1) > prm_.stillAccelFraction) return;
            meanG = Add(meanG, s.gyro); meanA = Add(meanA, s.accel);
        }
        meanG = Scale(meanG, 1.0/still_.size()); meanA = Scale(meanA, 1.0/still_.size());
        // Also require stable gravity direction: rotation at constant |a| is not stillness.
        for (const auto &s : still_) if (Norm(Sub(s.accel, meanA)) > gravityLsb_*0.015) return;
        stationary_ = true;
        double gain = 1 - std::exp(-(t-tImu_)/1e9 / std::fmax(prm_.biasTimeConstant, 0.1));
        Vec3 target = Sub(meanG, cal_.gyroBiasLsb);
        auto clamp = [this](double x) { return std::fmax(-prm_.biasClampLsb, std::fmin(prm_.biasClampLsb, x)); };
        target = Add(cal_.gyroBiasLsb, {clamp(target.x), clamp(target.y), clamp(target.z)});
        bias_ = Add(bias_, Scale(Sub(target, bias_), gain));
        gravityLsb_ += gain * (Norm(meanA) - gravityLsb_);
    }
    void AdvancePosition(double from, double to, const Vec3 &bodyA, const Quat &q, Vec3 &p, Vec3 &v) const {
        double end = std::fmin(to, tPos_ + prm_.deadReckonNs);
        double dt = std::fmax(0.0, end - std::fmax(from, tPos_))/1e9;
        if (dt > 0) {
            Vec3 a = Sub(Rotate(q, bodyA), {0, 9.80665, 0});
            if (Norm(a) > prm_.maxAcceleration || !std::isfinite(Norm(a))) a = {};
            // No invented acceleration over a radio gap. Velocity still leaks to a bounded displacement.
            if (to - from > prm_.maxImuStepNs) a = {};
            if (stationary_) { a = {}; v = {}; }
            double leak = std::fmax(0.001, prm_.velocityLeak), decay = std::exp(-leak*dt);
            double k = (1-decay)/leak;
            p = Add(p, Add(Scale(v, k), Scale(a, (dt-k)/leak)));
            v = Add(Scale(v, decay), Scale(a, k));
        }
        if (to >= tPos_ + prm_.deadReckonNs) v = {};
    }
    void PositionAt(double now, Vec3 &p, Vec3 &v) const {
        if (haveImu_ && initialized_ && tImu_ >= tPos_) {
            p = drPos_; v = drVel_;
            // Extrapolate only a short time beyond the newest actual IMU sample.
            double end = std::fmin(now, tImu_ + prm_.maxImuStepNs);
            Vec3 a = history_.empty() ? Rotate(Conj(q_), {0, 9.80665, 0}) : history_.back().accel;
            AdvancePosition(tImu_, end, a, q_, p, v);
            if (now > end || now >= tPos_ + prm_.deadReckonNs) v = {};
        } else {
            double dt = std::fmin(std::fmax(now-tPos_, 0.0), prm_.positionExtrapolateNs)/1e9;
            p = Add(pos_, Scale(vel_, dt));
            v = now-tPos_ < prm_.positionExtrapolateNs ? vel_ : Vec3{};
        }
        double f = std::fmax(0.0, 1 - std::fmax(0.0, now-blendStart_)/std::fmax(1.0, prm_.positionBlendNs));
        // Cubic fade gives zero correction velocity at either end.
        p = Add(p, Scale(blendOffset_, f*f*(3-2*f)));
        if (f > 0 && f < 1) v = Add(v, Scale(blendOffset_, -6*f*(1-f)*1e9/prm_.positionBlendNs));
    }

    bool OrientationAt(double t, Quat &out) const {
        if (history_.empty() || t < history_.front().t || t > history_.back().t) return false;
        for (size_t i = 1; i < history_.size(); ++i)
            if (history_[i].t >= t) {
                const Sample &a = history_[i - 1], &b = history_[i];
                double f = b.t > a.t ? (t - a.t) / (b.t - a.t) : 0;
                Vec3 d = Log(Normalize(Mul(Conj(a.q), b.q)));
                out = Normalize(Mul(a.q, Exp(Scale(d, f))));
                return true;
            }
        out = history_.back().q;
        return true;
    }
    void Rebase(const Quat &worldCorrection) {
        q_ = Normalize(Mul(worldCorrection, q_));
        for (auto &s : history_) s.q = Normalize(Mul(worldCorrection, s.q));
    }
    void AcceptPosition(double t, const Vec3 &p) {
        double now = haveImu_ ? std::fmax(t, tImu_) : t;
        Vec3 oldP{}, oldV{};
        if (havePos_) PositionAt(now, oldP, oldV);
        bool blend = havePos_;
        if (havePos_) {
            double dt = (t-tPos_)/1e9;
            if (dt >= 0.03 && dt < 0.5) {
                Vec3 measured = Scale(Sub(p, pos_), 1/dt);
                double speed = Norm(measured);
                if (speed > prm_.maxOpticalSpeed) measured = Scale(measured, prm_.maxOpticalSpeed/speed);
                opticalVelocities_.push_back(measured);
                if (opticalVelocities_.size() > 3) opticalVelocities_.pop_front();
                if (opticalVelocities_.size() == 3) {
                    auto median=[](double a, double b, double c) { return a+b+c-std::fmin(a,std::fmin(b,c))-std::fmax(a,std::fmax(b,c)); };
                    const auto &a=opticalVelocities_[0], &b=opticalVelocities_[1], &c=opticalVelocities_[2];
                    measured = {median(a.x,b.x,c.x), median(a.y,b.y,c.y), median(a.z,b.z,c.z)};
                }
                vel_ = haveVel_ ? Add(Scale(vel_, 0.5), Scale(measured, 0.5)) : measured;
                haveVel_ = true;
            } else if (dt >= 0.5) { vel_ = {}; haveVel_ = false; opticalVelocities_.clear(); }
        }
        pos_ = p; tPos_ = t; havePos_ = true;
        drPos_ = p; drVel_ = vel_;
        double from = t;
        for (const auto &s : history_) if (s.t > t) {
            AdvancePosition(from, s.t, s.accel, s.q, drPos_, drVel_); from = s.t;
        }
        blendOffset_ = {}; blendStart_ = now;
        if (blend) { Vec3 newP, newV; PositionAt(now, newP, newV); blendOffset_ = Sub(oldP, newP); }
    }

    Calibration cal_;
    Params prm_;
    Vec3 bias_;
    Quat q_;
    Vec3 wBody_;
    double tImu_ = 0, tPos_ = 0, tOptical_ = 0;
    bool haveImu_ = false, initialized_ = false, havePos_ = false, haveVel_ = false, haveOptical_ = false, reinit_ = false;
    Vec3 pos_, vel_;
    Vec3 drPos_, drVel_, blendOffset_;
    double blendStart_ = 0, gravityLsb_ = 1011;
    bool stationary_ = false;
    std::deque<StillSample> still_;
    std::deque<Vec3> opticalVelocities_;
    std::deque<Sample> history_;
    int suspicious_ = 0;
    Quat lastRejected_;
    uint64_t rejected_ = 0;
    Stats stats_;
};



// Fixed-lag replay: measurements are ordered by exposure/sample time, never by delivery time.
// Pose() is an observation of the filter, not an integration anchor. Delayed packets cannot
// overwrite the output-time state. A short render residual removes arrival discontinuities.
#include "pico_magnetic.h"
#include "mag_calibration.h"
class Tracker {
public:
    using Output = LegacyTracker::Output;
    using Stats = LegacyTracker::Stats;
    explicit Tracker(const Calibration &c, const Params &p = Params()) : cal_(c), prm_(p) {}
    MagneticState MagneticDiagnostics() const {
#if PICO_MAG_FUSION
        std::lock_guard<std::mutex> lock(mutex_);return Replay(LatestTime()).magnetic;
#else
        return {};
#endif
    }
    bool MagOrientation(uint64_t t,Quat &q) const {
        std::lock_guard<std::mutex> lock(mutex_);
        if(double(t)<base_.t)return false;
        const State s=Replay(double(t));
        if(!s.opt || double(t)-s.lastOpt>100e6 || double(t)<s.lastOpt)return false;
        q=s.q;return true;
    }
    void SetMagFieldCalibration(const MagCalibration &c) {
        std::lock_guard<std::mutex> lock(mutex_);
        magCal_.offset=c.offset;
        for(int i=0;i<9;++i)magCal_.soft[i]=c.soft[i];
        if(c.radius>0) magCal_.radius=c.radius;
        // Preserve separately verified mount and validity; a field fit cannot establish them.
    }
    void SetMagCalibration(const MagCalibration &c) { std::lock_guard<std::mutex> lock(mutex_); magCal_=c; }
    void ClearSensorImu() { std::lock_guard<std::mutex> lock(mutex_);haveTemperature_=false; }
    void SensorImu(int16_t temperature,uint8_t id) {
        std::lock_guard<std::mutex> lock(mutex_);
        temperature_=25+temperature/132.48; haveTemperature_=true;
#if PICO_SENSOR_SCALES
        if(id==0x44||id==0x47) { cal_.gyroRadPerLsb=(id==0x44?4000:2000)*3.141592653589793/180/32768; fixedGLsb_=id==0x44?1024:2048; }
#else
        (void)id;
#endif
    }
    // sourceNs is on the IMU sample clock (newest IMU stamp of the packet minus PICO_MAG_LATENCY_NS), so the
    // IMU time offset applies to it as to Imu().
    void Magnetometer(uint64_t sourceNs,Vec3 microtesla) {
#if PICO_MAG_FUSION
        std::lock_guard<std::mutex> lock(mutex_);
        if(!magCal_.valid || !std::isfinite(Norm(microtesla))) return;
        Event e{};e.t=double(sourceNs)+cal_.imuTimeOffsetNs;e.magnetic=true;e.a=microtesla;Insert(e);
#else
        (void)sourceNs;(void)microtesla;
#endif
    }
    void Imu(uint64_t sourceNs, const int16_t gyro[3], const int16_t accel[3]) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!cal_.valid) return;
        Event e{}; e.t = double(sourceNs) + cal_.imuTimeOffsetNs;
        if (PICO_BIAS_LEARN) LearnBias(e.t, gyro, accel);
        Vec3 bias=PICO_BIAS_LEARN?bias_:cal_.gyroBiasLsb;
#if PICO_TEMP_BIAS
        if(PICO_BIAS_LEARN && haveTemperature_) bias=Add(bias,thermal_.Term(temperature_,biasTemperature_));
#endif
        Vec3 g{gyro[0]-bias.x,gyro[1]-bias.y,gyro[2]-bias.z};
        e.w = Scale({cal_.r[0][0]*g.x+cal_.r[0][1]*g.y+cal_.r[0][2]*g.z,
                     cal_.r[1][0]*g.x+cal_.r[1][1]*g.y+cal_.r[1][2]*g.z,
                     cal_.r[2][0]*g.x+cal_.r[2][1]*g.y+cal_.r[2][2]*g.z},cal_.gyroRadPerLsb);
        // Accelerometer in the body frame (same mounting rotation as the gyro), in g units.
        // The LSB-per-g scale is a slow mean of the norm (faster while the gyro says still).
        Vec3 a{double(accel[0]),double(accel[1]),double(accel[2])};
        double an=Norm(a);
        if (an>0) { gLsb_=gLsb_>0?gLsb_+(Norm(e.w)<prm_.stillGyroRad?0.02:0.002)*(an-gLsb_):an; ++gSamples_; }
#if PICO_SENSOR_SCALES
        if(fixedGLsb_>0) gLsb_=fixedGLsb_;
#endif
        if (gLsb_>0) e.a = Scale({cal_.r[0][0]*a.x+cal_.r[0][1]*a.y+cal_.r[0][2]*a.z,
                                  cal_.r[1][0]*a.x+cal_.r[1][1]*a.y+cal_.r[1][2]*a.z,
                                  cal_.r[2][0]*a.x+cal_.r[2][1]*a.y+cal_.r[2][2]*a.z},1/gLsb_);
        #ifdef PICO_ACCEL_MOUNT_W
        // Accelerometer mounting differs from the gyro one (Horn fit of gravity vs optical orientation).
        e.a = Rotate(Normalize(Quat{PICO_ACCEL_MOUNT_W,PICO_ACCEL_MOUNT_X,PICO_ACCEL_MOUNT_Y,PICO_ACCEL_MOUNT_Z}), e.a);
        #endif
        e.haveA = gSamples_>=200;
        if (!std::isfinite(e.t) || !std::isfinite(Norm(e.w))) { ++stats_.imuLate; return; }
        if (!Insert(e)) { ++stats_.imuLate; return; }
        ++stats_.imu; lastImu_ = std::fmax(lastImu_,e.t);
    }
    bool Optical(uint64_t sourceNs, const Vec3 &p, const Quat &q) {
        std::lock_guard<std::mutex> lock(mutex_);
        double n=q.w*q.w+q.x*q.x+q.y*q.y+q.z*q.z;
        if (!std::isfinite(Norm(p)) || !std::isfinite(n) || n<1e-12) { ++rejected_; return false; }
        Event e{}; e.t=double(sourceNs); e.optical=true; e.p=p; e.q=Normalize(q);
        if (!Insert(e)) { ++rejected_; return false; }
        ++stats_.optical; stats_.lastOpticalMinusImuNs=e.t-lastImu_;
        return true;
    }
    Output Pose(uint64_t nowNs) const {
        std::lock_guard<std::mutex> lock(mutex_);
        double now=double(nowNs);
        State s=Replay(now);
        Output o;
        if (!s.opt && !s.imu) return o;
        // 2026-10-31: a tracker whose data stopped (session ended, tracker off or hung) is not "tracked" any more: SteamVR
        // kept showing the last pose with result 200 for 17+ min. Valid while the IMU (imuAliveNs) or optics (opticalOkNs) is live.
        if (!((s.imu && now-s.lastImu<prm_.imuAliveNs) || (s.opt && now-s.lastOpt<prm_.opticalOkNs))) return o;
        o.valid=true; o.opticalRecent=now-s.lastOpt<prm_.opticalOkNs;
        o.positionTracked=s.opt;
        o.opticalAgeNs=s.opt?now-s.lastOpt:1e18; o.still=s.imu && now-s.lastImu<100e6 && s.wAvg<PICO_STILL_W;
        o.gravityValid=s.haveGa && s.imu && now-s.lastImu<100e6; o.gravityBody=s.ga;
        o.orientation=s.q; o.position=Shown(s,now);
        o.linearVelocity=(now-s.lastOpt<prm_.positionExtrapolateNs || Blind(s,now))?s.v:Vec3{};
        if(Pivoting(s,now)) { const Vec3 w=Rotate(s.q,s.w),r=Rotate(s.q,s.pivR); o.linearVelocity={w.y*r.z-w.z*r.y,w.z*r.x-w.x*r.z,w.x*r.y-w.y*r.x}; }  // segment: v = w x R r
        o.angularVelocityWorld=now-s.lastRate<prm_.maxPredictNs?Rotate(s.q,s.w):Vec3{};
        if (renderHave_ && now>=renderTime_) {
            // Compare old and revised histories at exactly the previous output time.
            // Carry the discrepancy as a decaying render residual, rather than snapping.
            if (renderRevision_!=revision_) {
                State revised=Replay(renderTime_);
                qResidual_=Normalize(Mul(renderQ_,Conj(revised.q)));
                pResidual_=Sub(renderP_,Shown(revised,renderTime_));
            }
            double dt=(now-renderTime_)/1e9;
            // Keep rate prediction immediate. Remove history corrections at bounded
            // angular speed so an arriving burst cannot produce a one-frame snap.
            Vec3 residual=Log(qResidual_);
            double angle=Norm(residual);
            double correction=std::fmin(angle*(1-std::exp(-dt/.025)),
                dt*(PICO_CORR_GAIN*Norm(s.w)+(s.wAvg<PICO_STILL_W?PICO_REST_CORR_BASE:PICO_CORR_BASE)));
            // A residual this large is a reanchor (flipped start, recovered optics), not history noise: bleeding
            // 167 deg at <=0.02+0.2|w| rad/s kept the shown pose wrong for ~25 s (ab2-baseline). Take it at once.
            if(PICO_RESIDUAL_FAST_RAD>0 && angle>PICO_RESIDUAL_FAST_RAD)
                correction=std::fmax(correction,angle*(1-std::exp(-dt/PICO_RESIDUAL_FAST_TAU)));
            if(angle>PICO_RESIDUAL_SNAP_RAD) correction=angle;
            qResidual_=Exp(Scale(residual,angle>0?1-correction/angle:0));
            if(PICO_POS_CORR_GAIN>0) {
                // Speed-bounded like the rotation: a correction hides inside real motion, at rest it creeps.
                double len=Norm(pResidual_);
                double step=std::fmin(len*(1-std::exp(-dt/PICO_POS_CORR_TAU)),dt*(PICO_POS_CORR_GAIN*Norm(s.v)+PICO_POS_CORR_BASE));
                pResidual_=Scale(pResidual_,len>0?1-step/len:0);
            } else {
                double decay=std::exp(-dt/PICO_POS_TAU);
                pResidual_=Scale(pResidual_,decay);
            }
            o.orientation=Normalize(Mul(qResidual_,s.q));
            o.position=Add(Shown(s,now),pResidual_);
            // SteamVR extrapolates the shown position with linearVelocity: include the residual decay (d/dt of p+r = v - r/tau).
            if(PICO_VEL_RESIDUAL && PICO_POS_CORR_GAIN<=0 && o.linearVelocity.x==o.linearVelocity.x) o.linearVelocity=Sub(o.linearVelocity,Scale(pResidual_,1.0/PICO_POS_TAU));
            // SteamVR extrapolates with these: the latest gyro rate and the filtered optical
            // velocity, not a finite difference of two Pose() calls (that lagged half a frame
            // and turned residual corrections and the extra GetPose() call into velocity).
        }
        renderHave_=true; renderTime_=now; renderQ_=o.orientation; renderP_=o.position; renderRevision_=revision_;
        return o;
    }
    uint64_t Rejected() const { return rejected_; }
    const Stats &Diagnostics() const { return stats_; }
    double LastImuNs() const { return lastImu_; }
    Vec3 BiasLsb() const { return PICO_BIAS_LEARN?bias_:cal_.gyroBiasLsb; }
    double GLsb() const { return gLsb_; }
    uint64_t GSamples() const { return gSamples_; }
    // Counts belong to source-time replay state, never to Pose() call frequency.
    uint64_t OrientationRejected() const { return Replay(LatestTime()).orientationRejected; }
    uint64_t OrientationReanchors() const { return Replay(LatestTime()).orientationReanchors; }
private:
    struct Event { double t=0; bool optical=false,haveA=false,magnetic=false; Vec3 w,p,a; Quat q; };
    struct State {
        double t=0,lastImu=0,lastOpt=0,lastRate=0,lastOrientation=0,suspectTime=0,wAvg=1;  // wAvg: smoothed gyro rate (rad/s)
        MagneticState magnetic;
        bool imu=false,opt=false,rate=false;
        double pivN[6][7]={},pivW=0,pivPP=0,pivRms=0; bool pivOk=false; Vec3 pivR,pivAnchorP; Quat pivAnchorQ;  // segment model (lever fit, anchor)
        Vec3 p,v,w,ab,awc; Quat q,optQ,suspectQ;  // awc: world accelerometer correction (m/s^2), gamma term  // ab: accelerometer body bias (g), learned while optics is fresh
        Vec3 ga; bool haveGa=false;  // body model (2026-11-02): low-passed accelerometer in the body frame (g), "up" at rest
        int suspectCount=0;
        uint64_t orientationRejected=0,orientationReanchors=0;
    };
    double LatestTime() const { return events_.empty()?base_.t:events_.back().t; }
    void Advance(State &s,double t) const {
        // Segment model anchor: the shown position and orientation where the optical extrapolation ends.
        const double tc=s.lastOpt+PICO_PIVOT_AFTER_NS;
        if (PICO_PIVOT && s.opt && s.pivOk && s.t<tc && t>=tc) { AdvanceRaw(s,tc); s.pivAnchorP=s.p; s.pivAnchorQ=s.q; }
        AdvanceRaw(s,t);
    }
    void AdvanceRaw(State &s,double t) const {
        if (t<=s.t) return;
        double rotEnd=std::fmin(t,s.lastRate+prm_.maxPredictNs);
        double rd=std::fmax(0.0,rotEnd-s.t)/1e9;
        if(s.rate) s.q=Normalize(Mul(s.q,Exp(Scale(s.w,rd))));
        double posEnd=std::fmin(t,s.lastOpt+std::fmax(prm_.positionExtrapolateNs,prm_.deadReckonNs));
        s.p=Add(s.p,Scale(s.v,std::fmax(0.0,posEnd-s.t)/1e9));
        s.t=t;
    }
    // Segment model: the tracker is the end of a body segment turning about a joint, p_i = c + R_i r. While the optics
    // is visible the lever r is fitted (exponential forgetting PICO_PIVOT_TAU, ridge -> 0 without rotation); on loss
    // the shown position follows the gyro orientation: anchor + R(q) r - R(q_anchor) r. The joint c cancels out.
    bool Pivoting(const State &s,double t) const { return PICO_PIVOT && s.pivOk && s.opt && t-s.lastOpt>PICO_PIVOT_AFTER_NS; }
    Vec3 PivotPosition(const State &s) const { return Add(s.pivAnchorP,Sub(Rotate(s.q,s.pivR),Rotate(s.pivAnchorQ,s.pivR))); }
    Vec3 Shown(const State &s,double t) const { return Pivoting(s,t)?PivotPosition(s):s.p; }
    void PivotFit(State &s,const Event &e,double dtNs) const {
        const double lam=std::exp(-std::fmax(0.0,dtNs)/1e9/PICO_PIVOT_TAU);
        for(auto &row:s.pivN) for(double &x:row) x*=lam;
        s.pivW=s.pivW*lam+1; s.pivPP=s.pivPP*lam+e.p.x*e.p.x+e.p.y*e.p.y+e.p.z*e.p.z;
        const Vec3 ex=Rotate(e.q,{1,0,0}),ey=Rotate(e.q,{0,1,0}),ez=Rotate(e.q,{0,0,1});
        const double R[3][3]={{ex.x,ey.x,ez.x},{ex.y,ey.y,ez.y},{ex.z,ey.z,ez.z}},pv[3]={e.p.x,e.p.y,e.p.z};
        for(int row=0;row<3;++row){double A[6]={0,0,0,R[row][0],R[row][1],R[row][2]};A[row]=1;
            for(int a=0;a<6;++a){for(int b=0;b<6;++b) s.pivN[a][b]+=A[a]*A[b];
            s.pivN[a][6]+=A[a]*pv[row];}}
        s.pivOk=false;
        if(s.pivW<5) return;
        double M[6][7]; for(int a=0;a<6;++a) for(int b=0;b<7;++b) M[a][b]=s.pivN[a][b];
        for(int a=3;a<6;++a) M[a][a]+=PICO_PIVOT_RIDGE*s.pivW;
        for(int c=0;c<6;++c){int p=c;for(int r=c+1;r<6;++r) if(std::fabs(M[r][c])>std::fabs(M[p][c])) p=r;
            for(int k=0;k<7;++k) std::swap(M[c][k],M[p][k]);
            if(std::fabs(M[c][c])<1e-12) return;
            for(int r=0;r<6;++r) if(r!=c){const double f=M[r][c]/M[c][c];for(int k=0;k<7;++k) M[r][k]-=f*M[c][k];}}
        const Vec3 r{M[3][6]/M[3][3],M[4][6]/M[4][4],M[5][6]/M[5][5]};
        if(!std::isfinite(Norm(r)) || Norm(r)>PICO_PIVOT_MAX_LEVER) return;
        {double x[6]; for(int a=0;a<6;++a) x[a]=M[a][6]/M[a][a];
        double rss=s.pivPP; for(int a=0;a<6;++a){rss-=2*x[a]*s.pivN[a][6]; for(int b=0;b<6;++b) rss+=x[a]*x[b]*s.pivN[a][b];}
            s.pivRms=std::sqrt(std::fmax(0.0,rss)/s.pivW);}  // RMS 3D fit residual (m)
        if(s.pivRms>PICO_PIVOT_MAX_RMS) return;  // the fixes do not move like a segment end (hand translation): no lever, hold
        s.pivR=r; s.pivOk=true;
    }
    bool Blind(const State &s,double t) const {
        return prm_.deadReckonNs>0 && s.opt && t-s.lastOpt>prm_.positionExtrapolateNs && t-s.lastOpt<prm_.deadReckonNs;
    }
    void Apply(State &s,const Event &e) const {
        Advance(s,e.t);
#if PICO_MAG_FUSION
        if(e.magnetic) { s.magnetic.Update(e.t,e.a,s.q,s.opt && e.t-s.lastOpt<150e6,magCal_,PICO_MAG_LEARN!=0); return; }
#endif
        if (!e.optical) {
            // Optical loss: carry position on the accelerometer (gravity removed in world frame),
            // with a velocity leak bounding drift. No integration across a radio gap.
            if (e.haveA && s.opt && s.imu && e.t-s.lastOpt<100e6 && e.t-s.lastImu<40e6 && (!PICO_AB_REST_ONLY || (s.wAvg<PICO_STILL_W && std::fabs(Norm(e.a)-1)<0.02))) {  // PICO_AB_REST_ONLY: learn the body zero only at rest
                double k=std::fmin(1.0,(e.t-s.lastImu)/1e9/PICO_AB_TAU);  // mean specific force while tracked ~= gravity
                s.ab=Add(s.ab,Scale(Sub(Sub(e.a,Rotate(Conj(s.q),{0,1,0})),s.ab),k));
            }
            if (e.haveA && (Blind(s,e.t) || (PICO_ACCEL_TRACKED && s.opt && e.t-s.lastOpt<=prm_.positionExtrapolateNs &&
                 (s.wAvg>=PICO_STILL_W || Norm(s.v)>PICO_ACCEL_MOVING_V) && e.t-s.lastImu<12e6)) && s.imu && e.t-s.lastImu<40e6) {
                // Tracked mode: dense IMU only (the ~200 Hz stream), never at rest (accel drift there beats optics noise).
                double dt=(e.t-s.lastImu)/1e9;
                Vec3 aw=Scale(Sub(Rotate(s.q,Sub(e.a,s.ab)),{0,1,0}),9.80665);
                if (!(Norm(aw)<prm_.maxAcceleration)) aw={};
                // Alpha-beta-gamma: the world-frame accelerometer error learned from the optical innovations
                // (tilt leak of gravity, scale) keeps being removed after the optics is lost.
                aw=Add(aw,s.awc);
                // Orientation error (~2-6 deg) leaks 0.3-1 m/s^2 of gravity: drop accelerations inside the
                // deadband (soft threshold) and stop the velocity when the tracker is at rest (ZUPT).
                {double m=Norm(aw);aw=m>PICO_A_DEADBAND?Scale(aw,(m-PICO_A_DEADBAND)/m):Vec3{};}
                if ((Norm(aw)==0 && Norm(e.w)<PICO_ZUPT_GYRO) || (PICO_ZUPT_STILL && s.wAvg<PICO_ZUPT_STILL && std::fabs(Norm(e.a)-1)<0.03)) { s.v=Scale(s.v,std::exp(-10*dt)); aw={}; }  // ZUPT: a still tracker does not move
                s.v=Add(Scale(s.v,std::exp(-prm_.velocityLeak*dt)),Scale(aw,dt));
            }
            s.wAvg+=PICO_STILL_ALPHA*(Norm(e.w)-s.wAvg);
            if (e.haveA) {
                s.ga=s.haveGa?Add(s.ga,Scale(Sub(e.a,s.ga),PICO_GRAVITY_ALPHA)):e.a;
                s.haveGa=true;
            }
            // C.1 (Grok, opt-in): at rest the published/integrated rate is a low-passed gyro rate.
            s.w=(PICO_REST_RATE_ALPHA<1.0 && s.wAvg<PICO_STILL_W)?Add(Scale(s.w,1-PICO_REST_RATE_ALPHA),Scale(e.w,PICO_REST_RATE_ALPHA)):e.w;
            s.lastImu=e.t; s.lastRate=e.t; s.imu=true; s.rate=true; return;
        }
        // Back from the segment model: continue from the shown position, so the returning fix is a small innovation.
        if (Pivoting(s,e.t)) { s.p=PivotPosition(s); s.v={}; }
        // Back from a blind stretch: the inertial velocity has drifted; restart the optical estimate.
        if (s.opt && prm_.deadReckonNs>0 && e.t-s.lastOpt>prm_.positionExtrapolateNs && !PICO_BLIND_KEEP_V) s.v={};
        if (!s.opt) { s.q=e.q; s.p=e.p; s.v={}; s.lastOrientation=e.t; s.optQ=e.q; }
        else {
            // Alpha-beta innovation at exposure time. This corrects both position and
            // velocity before replaying every later measurement, including late IMU.
            double dt=std::fmax(0.005,(e.t-s.lastOpt)/1e9);
            Quat errorQ=Normalize(Mul(e.q,Conj(s.q)));
            const double gate=std::fmax(prm_.flipGateRad,std::fmin(PICO_FLIP_GATE_MAX_DEG*M_PI/180,
                prm_.flipGateRad+PICO_FLIP_GROW_DEG_S*M_PI/180*std::fmax(0.0,e.t-s.lastOrientation)/1e9));
            bool disagree=Norm(Log(errorQ))>gate;
            bool reanchor=false;
            if(disagree) {
                bool consistent=s.suspectCount>0 && e.t-s.suspectTime<=prm_.flipMaxIntervalNs &&
                    Norm(Log(Mul(e.q,Conj(s.suspectQ))))<=prm_.flipGateRad;
                s.suspectCount=consistent?s.suspectCount+1:1;
                s.suspectQ=e.q; s.suspectTime=e.t;
                reanchor=s.suspectCount>=prm_.flipConsecutive;
                // C.2 (Grok, opt-in): with a fresh IMU a run of consistent optical fixes far from the gyro is a flip.
                if(PICO_NO_REANCHOR_FRESH_IMU && reanchor && s.imu && e.t-s.lastImu<prm_.imuAliveNs && s.suspectCount<PICO_REANCHOR_FRESH_COUNT) reanchor=false;
                if(!reanchor) ++s.orientationRejected;
            } else s.suspectCount=0;
            // A fix whose orientation is rejected is a suspect (foreign glint / flipped ring):
            // it does not move position or extend the optical anchor either.
            if(disagree && !reanchor) return;
            Vec3 error=Sub(e.p,s.p);
            // Resting tracker (gyro still, small innovation): optical noise is averaged with low gains.
            // Any motion or a larger innovation restores the normal gains at once.
            const bool imuNow=s.imu && e.t-s.lastImu<100e6, still=imuNow && s.wAvg<PICO_STILL_W;
            const bool restPos=still && Norm(error)<PICO_STILL_POS_ERR;
            s.p=Add(s.p,Scale(error,restPos?PICO_POS_GAIN_STILL:PICO_POS_GAIN));
            s.v=Add(s.v,Scale(error,(restPos?PICO_POS_BETA_STILL:PICO_POS_BETA)/dt));
            if(restPos) s.v=Scale(s.v,PICO_STILL_VDECAY);
            // Gamma term: learn the world accelerometer correction only while it is being integrated (moving).
            if(PICO_POS_GAMMA>0 && !restPos && imuNow && (s.wAvg>=PICO_STILL_W || Norm(s.v)>PICO_ACCEL_MOVING_V) && dt<0.1) {
                s.awc=Add(s.awc,Scale(error,PICO_POS_GAMMA/(dt*dt)));
                const double m=Norm(s.awc); if(m>PICO_AWC_MAX) s.awc=Scale(s.awc,PICO_AWC_MAX/m);
            }
            double speed=Norm(s.v);
            if(speed>prm_.maxOpticalSpeed) s.v=Scale(s.v,prm_.maxOpticalSpeed/speed);
            if(!disagree || reanchor) {
                if(reanchor) {
                    s.q=e.q; ++s.orientationReanchors; s.suspectCount=0;
                    for(auto &row:s.pivN) for(double &x:row) x=0;
                    s.pivW=0; s.pivPP=0; s.pivOk=false;  // new orientation branch: refit the lever
                    // The discontinuity is a state correction, never an angular rate.
                    if(!s.imu || e.t-s.lastImu>prm_.imuAliveNs) { s.w={}; s.rate=false; }
                } else {
                    if(!s.imu || e.t-s.lastImu>prm_.imuAliveNs) {
                        double orientationDt=std::fmax(.005,(e.t-s.lastOrientation)/1e9);
                        s.w=Add(Scale(s.w,1-PICO_OPT_RATE_ALPHA),Scale(Scale(Log(Mul(Conj(s.optQ),e.q)),1/orientationDt),PICO_OPT_RATE_ALPHA));
                        s.lastRate=e.t; s.rate=true;
                    }
                    const bool restOri=still && Norm(Log(errorQ))<PICO_STILL_ORI_ERR;
                    s.q=Normalize(Mul(Exp(Scale(Log(errorQ),((s.imu && e.t-s.lastImu<prm_.imuAliveNs)?(restOri?PICO_ORI_GAIN_STILL:prm_.orientationGain):1.0))),s.q));
                }
                s.optQ=e.q; s.lastOrientation=e.t;
            }
        }
        if(PICO_PIVOT) PivotFit(s,e,s.opt?e.t-s.lastOpt:0);
        s.opt=true; s.lastOpt=e.t;
    }
    State Replay(double t) const {
        State s=base_;
        for(const auto &e:events_) { if(e.t>t) break;
        Apply(s,e); }
        Advance(s,t); return s;
    }
    bool Insert(const Event &e) {
        if(e.t<=base_.t) return false; // outside retained support; recover on next packet
        auto it=events_.begin();
        while(it!=events_.end() && (it->t<e.t || (it->t==e.t && (int(it->optical)+2*int(it->magnetic))<(int(e.optical)+2*int(e.magnetic))))) ++it;
        if(it!=events_.end() && it->t==e.t && it->optical==e.optical && it->magnetic==e.magnetic) return false;
        events_.insert(it,e); ++revision_;
        double cutoff=events_.back().t-std::fmax(prm_.historyNs,2000e6);
        // Keep the last output time replayable even if a packet has a bad future stamp.
        if(renderHave_) cutoff=std::fmin(cutoff,renderTime_-1);
        while(!events_.empty() && events_.front().t<cutoff) { Apply(base_,events_.front()); events_.pop_front(); }
        return true;
    }
    MagCalibration magCal_;
    double temperature_=25,biasTemperature_=25,fixedGLsb_=0,lastThermalLearn_=0; bool haveTemperature_=false; ThermalBias thermal_;
    Calibration cal_; Params prm_; Stats stats_;
    std::deque<Event> events_; State base_;
    double lastImu_=0; uint64_t rejected_=0,revision_=0;
    double gLsb_=0; uint64_t gSamples_=0;  // learned accelerometer LSB per g
    // Rest gyro-bias learning (PICO_BIAS_LEARN, 2026-10-31): a tracker using another unit's calibration has a bias
    // error of a few LSB (~1-2 deg/s). Over a window of PICO_BIAS_WINDOW_S where every gyro axis and the accelerometer
    // norm stay nearly constant (std below PICO_BIAS_STILL_STD LSB / 1 %), the bias moves toward the window mean with
    // time constant PICO_BIAS_TAU_S, bounded to PICO_BIAS_MAX_LSB around the calibration.
    struct RawSample { double t; Vec3 g; double an; double temperature; };
    std::deque<RawSample> rawWindow_;
    Vec3 bias_=cal_.gyroBiasLsb; double lastBiasT_=0; uint64_t biasUpdates_=0;
    void LearnBias(double t,const int16_t gyro[3],const int16_t accel[3]) {
        const Vec3 g{double(gyro[0]),double(gyro[1]),double(gyro[2])};
        const double an=std::sqrt(double(accel[0])*accel[0]+double(accel[1])*accel[1]+double(accel[2])*accel[2]);
        if(!rawWindow_.empty() && (t<=rawWindow_.back().t || t-rawWindow_.back().t>50e6)) rawWindow_.clear();
        rawWindow_.push_back({t,g,an,haveTemperature_?temperature_:NAN});
        while(rawWindow_.size()>1 && t-rawWindow_.front().t>PICO_BIAS_WINDOW_S*1e9) rawWindow_.pop_front();
        if(t-rawWindow_.front().t<0.8*PICO_BIAS_WINDOW_S*1e9 || rawWindow_.size()<20) return;
        Vec3 m{}; double ma=0;
        for(const auto &s:rawWindow_){m=Add(m,s.g);ma+=s.an;}
        const double k=1.0/rawWindow_.size(); m=Scale(m,k); ma*=k;
        Vec3 v{}; double va=0;
        for(const auto &s:rawWindow_){const Vec3 d=Sub(s.g,m);v=Add(v,{d.x*d.x,d.y*d.y,d.z*d.z});va+=(s.an-ma)*(s.an-ma);}
        v=Scale(v,k); va*=k;
        const double sd2=PICO_BIAS_STILL_STD*PICO_BIAS_STILL_STD;
        if(v.x>sd2||v.y>sd2||v.z>sd2||std::sqrt(va)>0.01*ma) { lastBiasT_=t; return; }
        const double dt=lastBiasT_>0?std::fmin(std::fmax(t-lastBiasT_,0.0)/1e9,0.1):0; lastBiasT_=t;
        const double a=1-std::exp(-dt/PICO_BIAS_TAU_S);
        auto clamp=[](double x,double c){return std::fmax(c-PICO_BIAS_MAX_LSB,std::fmin(c+PICO_BIAS_MAX_LSB,x));};
        bias_=Add(bias_,Scale(Sub(m,bias_),a));
        bias_={clamp(bias_.x,cal_.gyroBiasLsb.x),clamp(bias_.y,cal_.gyroBiasLsb.y),clamp(bias_.z,cal_.gyroBiasLsb.z)};
#if PICO_TEMP_BIAS
        if(haveTemperature_ && t-lastThermalLearn_>500e6) {
            const auto state=Replay(t);
            double low=1e9,high=-1e9,sum=0;
            for(const auto &sample:rawWindow_) { low=std::fmin(low,sample.temperature);high=std::fmax(high,sample.temperature);sum+=sample.temperature; }
            if(state.opt && t-state.lastOpt<150e6 && low>=-40 && high<=85 && high-low<.25) {
                thermal_.Observe(sum/rawWindow_.size(),m);lastThermalLearn_=t;
            }
        }
        if(haveTemperature_) biasTemperature_=temperature_;
#endif
        ++biasUpdates_;
    }
    mutable bool renderHave_=false;
    mutable double renderTime_=0;
    mutable uint64_t renderRevision_=0;
    mutable Quat renderQ_,qResidual_;
    mutable Vec3 renderP_,pResidual_;
    mutable std::mutex mutex_;
};
} // namespace pico_fusion
