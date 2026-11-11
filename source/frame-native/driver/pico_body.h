#pragma once
// Body model for worn trackers (2026-10-31, two-step calibration and IMU-held body 2026-11-02).
//
// Worn trackers (feet, pelvis) are below the field of view while the user looks ahead, and our cameras may not see
// them at all, so the body must work from the IMU alone and use optics only as a correction when it is there.
//
// Calibration (BodyRig, like PICO): step 1 "stand straight, look ahead": head level and still, every tracker still with
// a live IMU; records the head height/heading and each tracker's fusion orientation and gravity. Step 2 "look down at
// your trackers": collects optical fixes per tracker while the user stands still (until every tracker is seen or a
// timeout). Then roles (auto: pelvis = highest seen, feet left/right in the head-heading frame; unseen trackers take the
// remaining roles), per-tracker mounting (bone = world orientation * mount, bones upright along the head heading in the
// standing pose) and body offsets (seen: measured; unseen: proportions from the eye height). A calibration with no
// optics at all still finishes (IMU-only estimate) and says so in the status.
//
// Orientation of a tracker never seen by the optics: the fusion integrates the gyro from identity in an arbitrary frame.
// Each tracker keeps an alignment A (world <- fusion world): tilt from the accelerometer (gravity), yaw at step 1 from
// the mounting assumption "tracker face (-Z) points forward", a slow yaw pull (pelvis -> head heading, feet -> pelvis
// heading) against gyro drift. With fresh optics the fusion orientation is the world one and A relaxes to identity.
//
// Positions every frame: neck = head + R(head) neckInHead; torso trackers (pelvis, chest) = neck + R(bone) d (d learned
// from fresh optics); hips = neck + R(pelvis bone) hip (no pelvis tracker: bone = head heading, upright); a still foot
// holds its plant (ZUPT, dragged only beyond the leg reach), a moving foot follows a two-segment leg from the hip with
// the shank along the foot tracker's bone down axis and the knee along the foot heading, never below the floor. Fresh
// optics is the measurement; every switch between measurement / plant / swing carries the shown discrepancy as a
// decaying offset (no one-frame pop). Nothing assumes a particular number of trackers: each role is handled on its own.
#include "pico_fusion.h"
#include <algorithm>
#include <cstdio>
#include <map>
#include <string>
#include <vector>

namespace pico_body {
using namespace pico_fusion;

enum Role { Pelvis = 0, Chest = 1, LeftFoot = 2, RightFoot = 3, RoleCount = 4 };
inline const char *RoleName(int r) {
    static const char *nm[RoleCount] = {"pelvis", "chest", "left_foot", "right_foot"};
    return r >= 0 && r < RoleCount ? nm[r] : "none";
}

constexpr double kPi = 3.14159265358979323846;
inline Quat YawQ(double yaw) { return {std::cos(yaw * .5), 0, std::sin(yaw * .5), 0}; }
// Heading of a frame from its right axis (+X), stable when the frame pitches: YawQ(h) maps +X to (cos h, 0, -sin h).
inline double YawOfRight(const Quat &q) { const Vec3 r = Rotate(q, {1, 0, 0}); return std::atan2(-r.z, r.x); }
// Heading of a direction meant as "forward" (-Z): YawQ(h) maps -Z to (-sin h, 0, -cos h).
inline double YawOfForward(const Vec3 &f) { return std::atan2(-f.x, -f.z); }
inline double WrapPi(double a) {
    while (a > kPi) a -= 2 * kPi;
    while (a < -kPi) a += 2 * kPi;
    return a;
}
inline double Dot(const Vec3 &a, const Vec3 &b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline Vec3 Cross(const Vec3 &a, const Vec3 &b) { return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x}; }
// Rotation vector of the shortest rotation taking direction a onto direction b.
inline Vec3 SwingTo(const Vec3 &a, const Vec3 &b) {
    const double na = Norm(a), nb = Norm(b);
    if (na < 1e-9 || nb < 1e-9) return {};
    const Vec3 c = Cross(a, b);
    const double s = Norm(c), angle = std::atan2(s, Dot(a, b));
    if (s < 1e-12) return {};
    return Scale(c, angle / s);
}

struct TrackerIn {
    bool present = false;   // IMU alive: fusion output valid
    bool fresh = false;     // optical position is recent
    bool anchored = false;  // the fusion orientation has had an optical anchor (its frame is the world frame)
    bool still = false;     // IMU zero-velocity
    bool gravity = false;   // g is a live accelerometer average
    Vec3 p;                 // fusion position (world, m); meaningful only when fresh
    Quat q;                 // fusion orientation (fusion world <- tracker)
    Vec3 g;                 // accelerometer average in the tracker frame (g), "up" at rest
};
struct TrackerOut {
    bool have = false;      // body produced a pose for this role
    bool model = false;     // position from the body model (not a fresh optical measurement)
    Vec3 p;
    Quat q;                 // world orientation of the tracker (aligned)
    Quat align;             // world <- fusion world, for the angular velocity
};

// Inputs of one calibration: step 1 per role, optional step 2 optical observation per role.
struct Standing {
    Vec3 head; Quat headQ;  // step 1 head pose
    TrackerIn in[RoleCount];
};
struct Seen {
    bool seen = false;
    Vec3 p; Quat q;         // averaged optical position, latest fusion orientation (optically anchored)
    Vec3 head; Quat headQ;  // head pose at the observation (looking down)
};

struct Calibration {
    bool valid = false;
    bool imuOnly = true;           // no tracker was seen by the optics during the calibration
    double eyeHeight = 0, floorAnkleY = 0;
    bool seg[RoleCount] = {};
    bool optical[RoleCount] = {};  // calibrated from an optical observation (else proportions / IMU)
    Quat mount[RoleCount], align[RoleCount];
    Vec3 torsoFromNeck[2];         // pelvis, chest tracker offset from the neck in the bone frame
    Vec3 hipFromNeck[2];           // left, right hip from the neck in the pelvis bone frame
    double legLength[2] = {0.85, 0.85};  // hip to foot tracker
    Vec3 plant[2];
};

struct BodyParams {
    double eyeHeight = 1.62;      // m above the floor when no foot is seen (body_roles.txt "eye_height <m>")
    double ankleHeight = 0.09;    // m, foot tracker centre above the floor
    Vec3 neckInHead{0, -0.11, 0.09};       // head frame (x right, y up, z back): neck pivot
    Vec3 pelvisFromHip{0, 0.08, -0.11};    // bone frame (-z forward): belt tracker in front of the hip centre
    Vec3 chestFromNeck{0, -0.25, -0.12};   // bone frame: chest tracker
    double hipHalfWidth = 0.09;
    double thighFraction = 0.52;  // thigh share of the hip-to-foot-tracker leg
    double legSlack = 1.02;
    double blendTauS = 0.07;      // switch into the optical measurement
    double modelBlendTauS = 0.2;  // switch into a model (plant / swing / held torso)
    double tiltTauStillS = 0.5, tiltTauMovingS = 8.0;  // accelerometer tilt correction of the alignment
    double opticsAlignTauS = 0.15;  // fresh optics: alignment relaxes to identity
    double yawPullTauS = 30.0;      // IMU-only yaw drift: pelvis toward the head heading, feet toward the pelvis
    double offsetLearnTauS = 2.0;   // torso offset learned from fresh optics
    double floorLearnTauS = 1.0;    // floor learned from fresh still feet
};

// Role assignment from a standing pose with every tracker seen (n = 2..4 at positions p[i]). The two lowest are the
// feet, left/right by the side in the head-yaw frame; of the rest the higher is the chest, the lower the pelvis.
// role[i] = Role or -1. Returns false if the pose is not plausible (feet not well below the torso / ~1 m below the head).
inline bool AssignRoles(const Vec3 &head, const Quat &headQ, const Vec3 *p, int n, int *role) {
    if (n < 2 || n > RoleCount) return false;
    int idx[RoleCount];
    for (int i = 0; i < n; ++i) { idx[i] = i; role[i] = -1; }
    for (int i = 0; i < n; ++i)
        for (int j = i + 1; j < n; ++j)
            if (p[idx[j]].y < p[idx[i]].y) std::swap(idx[i], idx[j]);
    const double yaw = YawOfRight(headQ);
    const Vec3 right{std::cos(yaw), 0, -std::sin(yaw)};
    auto side = [&](int i) { return Dot(Sub(p[i], head), right); };
    const int a = idx[0], b = idx[1];
    if (n >= 3 && p[idx[2]].y - std::fmax(p[a].y, p[b].y) < 0.3) return false;
    if (head.y - p[a].y < 1.0) return false;
    if (side(a) < side(b)) { role[a] = LeftFoot; role[b] = RightFoot; } else { role[a] = RightFoot; role[b] = LeftFoot; }
    if (n >= 3) role[idx[2]] = Pelvis;
    if (n == 4) role[idx[3]] = Chest;
    return true;
}

// Role assignment with some trackers possibly unseen (two-step calibration). seen[i]: chip i has an optical position
// p[i]; prev[i]: its role from the previous calibration or -1. Seen trackers more than footDrop below the head are feet
// (two lowest at most), seen higher ones torso (highest = chest when n == 4); unseen trackers take the remaining roles,
// their previous role first. guessed = some role among unseen trackers was not determined (left/right or pelvis/foot).
inline bool AssignPartial(const Vec3 &head, const Quat &headQ, const bool *seen, const Vec3 *p, const int *prev, int n,
                          int *role, bool &guessed, double footDrop = 1.0) {
    guessed = false;
    if (n < 2 || n > RoleCount) return false;
    std::vector<int> need;
    if (n >= 3) need.push_back(Pelvis);
    if (n == 4) need.push_back(Chest);
    need.push_back(LeftFoot);
    need.push_back(RightFoot);
    for (int i = 0; i < n; ++i) role[i] = -1;
    std::vector<int> s;
    for (int i = 0; i < n; ++i)
        if (seen[i]) s.push_back(i);
    std::sort(s.begin(), s.end(), [&](int a, int b) { return p[a].y < p[b].y; });
    std::vector<int> feet, torso;
    for (int i : s) {
        if (head.y - p[i].y >= footDrop && feet.size() < 2) feet.push_back(i);
        else torso.push_back(i);
    }
    const int torsoRoles = n - 2;
    if (int(torso.size()) > torsoRoles) return false;  // more torso-high trackers than torso roles: not a standing pose
    const double yaw = YawOfRight(headQ);
    const Vec3 right{std::cos(yaw), 0, -std::sin(yaw)};
    auto side = [&](int i) { return Dot(Sub(p[i], head), right); };
    auto take = [&](int r) { need.erase(std::remove(need.begin(), need.end(), r), need.end()); };
    if (feet.size() == 2) {
        const bool aLeft = side(feet[0]) < side(feet[1]);
        role[feet[0]] = aLeft ? LeftFoot : RightFoot;
        role[feet[1]] = aLeft ? RightFoot : LeftFoot;
    } else if (feet.size() == 1) {
        role[feet[0]] = side(feet[0]) < 0 ? LeftFoot : RightFoot;
    }
    for (int i : feet) take(role[i]);
    // torso: two seen -> higher chest, lower pelvis; one seen -> pelvis
    std::sort(torso.begin(), torso.end(), [&](int a, int b) { return p[a].y > p[b].y; });
    if (torso.size() == 2) {
        role[torso[0]] = Chest;
        role[torso[1]] = Pelvis;
    } else if (torso.size() == 1) {
        role[torso[0]] = Pelvis;
    }
    for (int i : torso) take(role[i]);
    std::vector<int> unseen;
    for (int i = 0; i < n; ++i)
        if (role[i] < 0) unseen.push_back(i);
    for (int i : unseen) {  // previous role first
        if (prev && prev[i] >= 0 && std::find(need.begin(), need.end(), prev[i]) != need.end()) {
            role[i] = prev[i];
            take(role[i]);
        }
    }
    int open = 0;
    for (int i : unseen)
        if (role[i] < 0) ++open;
    if (open >= 2) guessed = true;
    for (int i : unseen) {
        if (role[i] >= 0) continue;
        if (need.empty()) return false;
        role[i] = need.front();
        take(role[i]);
    }
    return true;
}

class Body {
public:
    explicit Body(const BodyParams &p = BodyParams()) : prm_(p) {}
    const BodyParams &Params() const { return prm_; }
    void SetEyeHeight(double m) { prm_.eyeHeight = m; }

    // Alignment of a tracker's fusion frame to the world at the standing pose (step 1): identity when the fusion is
    // optically anchored (yaw kept, tilt from gravity), else tilt from gravity and yaw so that the tracker face (-Z,
    // or +Y when the face is near vertical) points along the head heading.
    Quat StandingAlign(const TrackerIn &in, double headYaw, bool *assumed = nullptr) const {
        Quat a;
        if (in.gravity && Norm(in.g) > 0.5) a = Exp(SwingTo(Rotate(in.q, in.g), {0, 1, 0}));
        if (assumed) *assumed = !in.anchored;
        if (in.anchored) return a;
        const Quat w = Mul(a, in.q);
        Vec3 f = Rotate(w, {0, 0, -1});
        if (std::hypot(f.x, f.z) < 0.35) f = Rotate(w, {0, 1, 0});
        return Normalize(Mul(YawQ(headYaw - YawOfForward(f)), a));
    }

    // Two-step calibration (roles already known): step 1 standing pose per role, step 2 optical observations per role.
    bool Calibrate(const Standing &s1, const Seen seen[RoleCount]) {
        Calibration c;
        const double yaw1 = YawOfRight(s1.headQ);
        const Quat b0 = YawQ(yaw1), b0c = Conj(b0);
        const Vec3 neck1 = Neck(s1.head, s1.headQ);
        // Floor and proportions: from the seen feet, else from the eye height.
        int feetSeen = 0; double ankleY = 0;
        for (int s = 0; s < 2; ++s)
            if (seen[LeftFoot + s].seen) { ankleY += seen[LeftFoot + s].p.y; ++feetSeen; }
        if (feetSeen) {
            c.floorAnkleY = ankleY / feetSeen;
            c.eyeHeight = s1.head.y - (c.floorAnkleY - prm_.ankleHeight);
        } else {
            c.eyeHeight = prm_.eyeHeight;
            c.floorAnkleY = s1.head.y - c.eyeHeight + prm_.ankleHeight;
        }
        const double floorY = c.floorAnkleY - prm_.ankleHeight, stature = c.eyeHeight / 0.936;
        Vec3 hipCentre{0, floorY + 0.52 * stature - neck1.y, 0};  // bone frame, from the neck
        for (int r = Pelvis; r <= Chest; ++r) {
            if (!s1.in[r].present) continue;
            if (seen[r].seen) {
                const Vec3 neck2 = Neck(seen[r].head, seen[r].headQ);
                c.torsoFromNeck[r] = Rotate(b0c, Sub(seen[r].p, neck2));
            } else {
                c.torsoFromNeck[r] = r == Pelvis ? Add(hipCentre, prm_.pelvisFromHip) : prm_.chestFromNeck;
            }
        }
        if (s1.in[Pelvis].present && seen[Pelvis].seen) hipCentre = Sub(c.torsoFromNeck[Pelvis], prm_.pelvisFromHip);
        for (int s = 0; s < 2; ++s) {
            const int r = LeftFoot + s;
            Vec3 hip = Add(hipCentre, {s == 0 ? -prm_.hipHalfWidth : prm_.hipHalfWidth, 0, 0});
            if (seen[r].seen) {  // hip above the seen foot, at a plausible hip width
                const double x = Rotate(b0c, Sub(seen[r].p, neck1)).x;
                hip.x = (s == 0 ? -1 : 1) * std::fmin(0.2, std::fmax(0.05, std::fabs(x)));
            }
            c.hipFromNeck[s] = hip;
            const Vec3 hipW = Add(neck1, Rotate(b0, hip));
            if (seen[r].seen) {
                c.plant[s] = seen[r].p;
                c.legLength[s] = std::fmin(1.2, std::fmax(0.5, hipW.y - seen[r].p.y));
            } else {
                c.plant[s] = {hipW.x, c.floorAnkleY, hipW.z};
                c.legLength[s] = std::fmin(1.2, std::fmax(0.5, hipW.y - c.floorAnkleY));
            }
        }
        for (int r = 0; r < RoleCount; ++r) {
            if (!s1.in[r].present) continue;
            Quat w;
            if (seen[r].seen) {  // optically anchored fusion orientation, bones unchanged since step 1 (standing still)
                c.align[r] = Quat{};
                w = seen[r].q;
                c.optical[r] = true;
                c.imuOnly = false;
            } else {
                c.align[r] = StandingAlign(s1.in[r], yaw1);
                w = Mul(c.align[r], s1.in[r].q);
            }
            c.mount[r] = Normalize(Mul(Conj(w), b0));
            c.seg[r] = true;
        }
        c.valid = true;
        SetCalibration(c);
        return true;
    }
    void SetCalibration(const Calibration &c) {
        cal_ = c;
        for (int r = 0; r < RoleCount; ++r) {
            seg_[r] = Seg{};
            seg_[r].align = c.align[r];
        }
        for (int s = 0; s < 2; ++s) {
            seg_[LeftFoot + s].plant = c.plant[s];
            seg_[LeftFoot + s].havePlant = true;
            seg_[LeftFoot + s].wasStill = true;
        }
        for (int r = 0; r < 2; ++r) torsoD_[r] = c.torsoFromNeck[r];
        floorAnkleY_ = c.floorAnkleY;
        lastT_ = -1;
    }
    const Calibration &GetCalibration() const { return cal_; }
    double FloorAnkleY() const { return floorAnkleY_; }

    // One output frame. out[r] for every present calibrated role.
    void Update(double tS, const Vec3 &head, const Quat &headQ, const TrackerIn in[RoleCount], TrackerOut out[RoleCount]) {
        if (!cal_.valid) return;
        const double dt = lastT_ < 0 ? 0.0 : std::fmax(0.0, std::fmin(0.2, tS - lastT_));
        lastT_ = tS;
        const double headYaw = YawOfRight(headQ);
        Quat bone[RoleCount];
        for (int r = 0; r < RoleCount; ++r) {
            out[r] = TrackerOut{};
            if (!in[r].present || !cal_.seg[r]) continue;
            Align(r, in[r], dt);
            out[r].q = Normalize(Mul(seg_[r].align, in[r].q));
            out[r].align = seg_[r].align;
            out[r].have = true;
            bone[r] = Normalize(Mul(out[r].q, cal_.mount[r]));
        }
        // Pelvis heading: the pelvis tracker's bone, else the head heading (upright). IMU-only yaw drift pull.
        const bool pelvis = out[Pelvis].have;
        if (pelvis && !in[Pelvis].fresh) {
            PullYaw(Pelvis, bone[Pelvis], headYaw, dt);
            out[Pelvis].q = Normalize(Mul(seg_[Pelvis].align, in[Pelvis].q));
            out[Pelvis].align = seg_[Pelvis].align;
            bone[Pelvis] = Normalize(Mul(out[Pelvis].q, cal_.mount[Pelvis]));
        }
        const Quat pelvisBone = pelvis ? bone[Pelvis] : YawQ(headYaw);
        const double pelvisYaw = YawOfRight(pelvisBone);
        const Vec3 neck = Neck(head, headQ);
        for (int r = Pelvis; r <= Chest; ++r) {
            if (!out[r].have) continue;
            Torso(tS, r, in[r], neck, bone[r], dt, out[r]);
        }
        for (int s = 0; s < 2; ++s) {
            const int r = LeftFoot + s;
            if (!out[r].have) continue;
            if (!in[r].fresh) {
                PullYaw(r, bone[r], pelvisYaw, dt);
                out[r].q = Normalize(Mul(seg_[r].align, in[r].q));
                out[r].align = seg_[r].align;
                bone[r] = Normalize(Mul(out[r].q, cal_.mount[r]));
            }
            const Vec3 hip = Add(neck, Rotate(pelvisBone, cal_.hipFromNeck[s]));
            Foot(tS, s, in[r], bone[r], hip, pelvisBone, dt, out[r]);
        }
    }

    enum Mode { Measured = 0, Held = 1, Planted = 2, Swing = 3 };
    int ModeOf(int r) const { return seg_[r].mode; }

private:
    struct Seg {
        Quat align;
        bool anchoredPrev = false;
        bool have = false, havePlant = false, wasStill = false;
        int mode = -1;
        double lastT = 0, tau = 0.07;
        Vec3 shown, blend, plant;
    };
    Vec3 Neck(const Vec3 &head, const Quat &headQ) const { return Add(head, Rotate(headQ, prm_.neckInHead)); }
    // Alignment upkeep: first optical anchor -> identity at once (the fusion frame jumped to the world); fresh optics ->
    // relax to identity; else tilt from the accelerometer.
    void Align(int r, const TrackerIn &in, double dt) {
        Seg &st = seg_[r];
        if (in.anchored && !st.anchoredPrev && !cal_.optical[r] && in.fresh) st.align = Quat{};
        st.anchoredPrev = in.anchored;
        if (in.fresh) {
            const Vec3 e = Log(st.align);
            st.align = Normalize(Exp(Scale(e, std::exp(-dt / prm_.opticsAlignTauS))));
            return;
        }
        if (!in.gravity) return;
        const double gn = Norm(in.g);
        if (std::fabs(gn - 1) > 0.05) return;
        const Vec3 up = Rotate(Mul(st.align, in.q), in.g);
        const double tau = in.still ? prm_.tiltTauStillS : prm_.tiltTauMovingS;
        const double k = 1 - std::exp(-dt / tau);
        st.align = Normalize(Mul(Exp(Scale(SwingTo(up, {0, 1, 0}), k)), st.align));
    }
    void PullYaw(int r, const Quat &bone, double targetYaw, double dt) {
        const double k = 1 - std::exp(-dt / prm_.yawPullTauS);
        const double d = WrapPi(targetYaw - YawOfRight(bone));
        seg_[r].align = Normalize(Mul(YawQ(d * k), seg_[r].align));
    }
    // Shown position = base + decaying blend; a mode switch re-bases on the last shown point.
    void Show(double tS, Seg &st, int mode, const Vec3 &base, Vec3 &out) {
        if (st.have) {
            if (mode != st.mode) {
                st.blend = Sub(st.shown, base);
                st.tau = mode == Measured ? prm_.blendTauS : prm_.modelBlendTauS;
            } else {
                st.blend = Scale(st.blend, std::exp(-std::fmax(0.0, tS - st.lastT) / st.tau));
            }
        } else {
            st.blend = {};
        }
        st.mode = mode;
        st.shown = Add(base, st.blend);
        st.lastT = tS;
        st.have = true;
        out = st.shown;
    }
    void Torso(double tS, int r, const TrackerIn &in, const Vec3 &neck, const Quat &bone, double dt, TrackerOut &o) {
        Seg &st = seg_[r];
        if (in.fresh) {
            const Vec3 d = Rotate(Conj(bone), Sub(in.p, neck));
            torsoD_[r] = Add(torsoD_[r], Scale(Sub(d, torsoD_[r]), 1 - std::exp(-dt / prm_.offsetLearnTauS)));
            Show(tS, st, Measured, in.p, o.p);
            return;
        }
        Show(tS, st, Held, Add(neck, Rotate(bone, torsoD_[r])), o.p);
        o.model = true;
    }
    Vec3 Leg(int s, const Quat &bone, const Vec3 &hip, const Quat &pelvisBone) const {
        const double len = cal_.legLength[s], thigh = len * prm_.thighFraction, shank = len - thigh;
        const Vec3 sdir = Rotate(bone, {0, -1, 0});
        Vec3 fwd = Rotate(bone, {0, 0, -1});
        fwd.y = 0;
        if (Norm(fwd) < 0.3) {
            fwd = Rotate(pelvisBone, {0, 0, -1});
            fwd.y = 0;
        }
        fwd = Scale(fwd, 1 / std::fmax(1e-9, Norm(fwd)));
        // Thigh tilt so that the ankle reaches the floor (hip height known): standing -> straight, sitting -> horizontal.
        const double drop = hip.y - floorAnkleY_;
        const double down = std::fmin(1.0, std::fmax(0.0, (drop - shank * -sdir.y) / thigh));
        const Vec3 knee = Add(hip, Scale(Add(Vec3{0, -down, 0}, Scale(fwd, std::sqrt(1 - down * down))), thigh));
        Vec3 p = Add(knee, Scale(sdir, shank));
        p.y = std::fmax(p.y, floorAnkleY_);
        return p;
    }
    void Foot(double tS, int s, const TrackerIn &in, const Quat &bone, const Vec3 &hip, const Quat &pelvisBone, double dt, TrackerOut &o) {
        Seg &st = seg_[LeftFoot + s];
        if (in.fresh) {
            if (in.still) {
                st.plant = in.p;
                st.havePlant = true;
                floorAnkleY_ += (in.p.y - floorAnkleY_) * (1 - std::exp(-dt / prm_.floorLearnTauS));
            }
            st.wasStill = in.still;
            Show(tS, st, Measured, in.p, o.p);
            return;
        }
        o.model = true;
        if (!in.still) {
            st.wasStill = false;
            Show(tS, st, Swing, Leg(s, bone, hip, pelvisBone), o.p);
            return;
        }
        // ZUPT: a still foot stays where it stopped.
        if (!st.havePlant || !st.wasStill) {
            st.plant = st.have ? st.shown : Leg(s, bone, hip, pelvisBone);
            st.havePlant = true;
        }
        st.wasStill = true;
        st.plant.y = std::fmax(st.plant.y, floorAnkleY_);
        const Vec3 d = Sub(st.plant, hip);
        const double len = Norm(d), maxLen = cal_.legLength[s] * prm_.legSlack;
        if (len > maxLen) {  // the hip moved beyond the leg: the foot was dragged along
            st.plant = Add(hip, Scale(d, maxLen / len));
            st.plant.y = std::fmax(st.plant.y, floorAnkleY_);
        }
        Show(tS, st, Planted, st.plant, o.p);
    }

    BodyParams prm_;
    Calibration cal_;
    Seg seg_[RoleCount];
    Vec3 torsoD_[2];
    double floorAnkleY_ = 0, lastT_ = -1;
};

// Driver glue: roles per tracker chip, the two-step calibration, applied to fusion outputs.
// Config (resources/body_roles.txt next to the driver): lines "<chip_hex> pelvis|chest|left_foot|right_foot" or one
// line "auto <n>" (n trackers, roles from the standing pose), optional "eye_height <m>". No config: inactive.
class BodyRig {
public:
    struct Slot { uint32_t chip; Tracker::Output *o; };
    enum Step { Idle = 0, Stand = 1, LookDown = 2, Done = 3 };
    struct Timing {
        double standHoldS = 1.0;      // step 1: still and level this long
        int seenSamples = 8;          // step 2: fresh still optical fixes per tracker
        double lookTimeoutS = 12.0;   // step 2: after the head first pitched down
        double stepTimeoutS = 25.0;   // step 2: overall
    };
    bool Active() const { return autoCount_ > 0 || !fixed_.empty(); }
    void SetRole(uint32_t chip, int role) { fixed_[chip] = role; roles_ = fixed_; }
    void SetAuto(int n) { autoCount_ = (n >= 2 && n <= RoleCount) ? n : 0; }
    void SetEyeHeight(double m) {
        if (m > 0.8 && m < 2.3) eyeHeight_ = m;
    }
    void SetTiming(const Timing &t) { timing_ = t; }
    bool Calibrated() const { return step_ == Done; }
    int CurrentStep() const { return step_; }
    bool RolesGuessed() const { return guessed_; }
    const Body &GetBody() const { return body_; }
    // New two-step calibration (app button); with "auto" the roles are assigned again (previous ones kept as a hint).
    void Recalibrate() {
        step_ = Idle;
        readySince_ = -1;
        body_ = Body();
        if (autoCount_ > 0) {
            prevRoles_ = roles_;
            roles_.clear();
        }
    }
    void Reset() { *this = BodyRig(); }
    int RoleOf(uint32_t chip) const { auto it = roles_.find(chip); return it == roles_.end() ? -1 : it->second; }

    // Status for the app. Line 1 keeps the ack13 grammar; the other lines carry no '=' (old app parses '=' as roles).
    std::string Status() const {
        std::string s;
        char line[512];
        int w = std::snprintf(line, sizeof(line), "active %d auto %d calibrated %d roles", Active() ? 1 : 0, autoCount_, Calibrated() ? 1 : 0);
        for (auto &kv : roles_) w += std::snprintf(line + w, sizeof(line) - w, " %08X=%s", kv.first, RoleName(kv.second));
        s += line; s += "\n";
        static const char *stepName[] = {"idle", "stand", "look_down", "done"};
        std::snprintf(line, sizeof(line), "step %s %.1f left %.1f\n", stepName[step_], lastT_ >= 0 && stepT0_ >= 0 ? lastT_ - stepT0_ : 0.0,
                      step_ == LookDown ? StepLeft(lastT_) : 0.0);
        s += line;
        std::snprintf(line, sizeof(line), "result %s guessed %d expected %d eye_height %.2f\n", result_.c_str(), guessed_ ? 1 : 0, Expected(), eyeHeight_);
        s += line;
        for (auto &kv : chips_) {
            const ChipState &c = kv.second;
            std::snprintf(line, sizeof(line), "tracker %08X role %s imu %d still %d optics %d seen %d position %s mount %s\n", kv.first,
                          RoleName(RoleOf(kv.first)), c.imu ? 1 : 0, c.still ? 1 : 0, c.fresh ? 1 : 0, c.n2, c.position.c_str(), c.mount.c_str());
            s += line;
        }
        s += "note " + note_ + "\n";
        return s;
    }

    // Returns a log line when something happened (step change, roles, calibrated), else empty.
    std::string Step(double tS, bool headOk, const Vec3 &head, const Quat &headQ, Slot *slots, int n, double freshNs = 150e6) {
        std::string log;
        lastT_ = tS;
        if (!Active()) return log;
        UpdateChips(slots, n, freshNs);
        if (!headOk) {
            note_ = "headset not tracked";
            return log;
        }
        headStill_ = haveHead_ && Norm(Sub(head, lastHead_)) < 0.002 && Norm(Log(Mul(headQ, Conj(lastHeadQ_)))) < 0.005;
        lastHead_ = head; lastHeadQ_ = headQ; haveHead_ = true;
        if (step_ != Done) {
            log = Calibrating(tS, head, headQ, slots, n, freshNs);
            if (step_ != Done) return log;
        }
        if (roles_.empty()) return log;
        TrackerIn in[RoleCount];
        Fill(slots, n, in, freshNs);
        TrackerOut out[RoleCount];
        body_.Update(tS, head, headQ, in, out);
        for (int i = 0; i < n; ++i) {
            if (!slots[i].o) continue;
            const int r = RoleOf(slots[i].chip);
            if (r < 0 || !out[r].have) continue;
            Tracker::Output &o = *slots[i].o;
            o.position = out[r].p;
            o.orientation = out[r].q;
            o.angularVelocityWorld = Rotate(out[r].align, o.angularVelocityWorld);
            o.positionTracked = true;  // a body estimate: published as tracked (status says model)
            if (out[r].model) o.linearVelocity = {};
            auto c = chips_.find(slots[i].chip);
            if (c != chips_.end()) c->second.position = out[r].model ? "model" : "optical";
        }
        return log;
    }

private:
    struct ChipState {
        bool imu = false, still = false, fresh = false;
        int n2 = 0;
        std::string position = "none", mount = "-";
        Tracker::Output s1;
        bool hasS1 = false;
        Vec3 sum2; Quat q2; Vec3 head2; Quat headQ2;
    };
    int Expected() const { return autoCount_ > 0 ? autoCount_ : int(fixed_.size()); }
    double StepLeft(double tS) const {
        double end = stepT0_ + timing_.stepTimeoutS;
        if (lookSince_ >= 0) end = std::fmin(end, lookSince_ + timing_.lookTimeoutS);
        return std::fmax(0.0, end - tS);
    }
    static bool Fresh(const Tracker::Output &o, double freshNs) { return o.valid && o.positionTracked && o.opticalAgeNs < freshNs; }
    void UpdateChips(Slot *slots, int n, double freshNs) {
        for (auto &kv : chips_) kv.second.imu = false;
        for (int i = 0; i < n; ++i) {
            if (!slots[i].o) continue;
            ChipState &c = chips_[slots[i].chip];
            c.imu = slots[i].o->valid;
            c.still = slots[i].o->still;
            c.fresh = Fresh(*slots[i].o, freshNs);
            if (step_ != Done) c.position = "none";
        }
    }
    // Chips taking part: the fixed-role chips, or (auto) every chip with a live IMU.
    bool Member(uint32_t chip) const { return autoCount_ > 0 || fixed_.count(chip) > 0; }
    std::string Calibrating(double tS, const Vec3 &head, const Quat &headQ, Slot *slots, int n, double freshNs) {
        std::string log;
        if (step_ == Idle) {
            step_ = Stand;
            stepT0_ = tS;
            readySince_ = -1;
            result_ = "-";
            guessed_ = false;
            for (auto &kv : chips_) {
                kv.second.n2 = 0;
                kv.second.hasS1 = false;
                kv.second.mount = "-";
            }
            log = "pico_body: calibration step 1: stand straight, look ahead\n";
        }
        const double fy = Rotate(headQ, {0, 0, -1}).y;
        if (step_ == Stand) {
            int live = 0, moving = 0, noGravity = 0;
            for (int i = 0; i < n; ++i) {
                const Tracker::Output *o = slots[i].o;
                if (!o || !o->valid || !Member(slots[i].chip)) continue;
                ++live;
                if (!o->still) ++moving;
                if (!o->gravityValid && !o->positionTracked) ++noGravity;
            }
            bool allThere = live == Expected();
            if (autoCount_ == 0) {
                for (auto &kv : fixed_) {
                    bool found = false;
                    for (int i = 0; i < n; ++i)
                        if (slots[i].chip == kv.first && slots[i].o && slots[i].o->valid) found = true;
                    allThere = allThere && found;
                }
            }
            const bool level = std::fabs(fy) < 0.34;
            char buf[160];
            if (!allThere) std::snprintf(buf, sizeof(buf), "waiting for trackers: %d of %d have a live IMU", live, Expected());
            else if (noGravity) std::snprintf(buf, sizeof(buf), "waiting for the accelerometer of %d tracker(s)", noGravity);
            else if (!level) std::snprintf(buf, sizeof(buf), "%s", fy < 0 ? "look straight ahead (head is pitched down)" : "look straight ahead (head is pitched up)");
            else if (!headStill_ || moving) std::snprintf(buf, sizeof(buf), "hold still (%d tracker(s) moving%s)", moving, headStill_ ? "" : ", head moving");
            else std::snprintf(buf, sizeof(buf), "hold still");
            note_ = buf;
            if (!failNote_.empty()) note_ += " (previous attempt: " + failNote_ + ")";
            const bool ready = allThere && !noGravity && level && headStill_ && !moving;
            if (!ready) {
                readySince_ = -1;
                return log;
            }
            if (readySince_ < 0) readySince_ = tS;
            if (tS - readySince_ < timing_.standHoldS) return log;
            head1_ = head; headQ1_ = headQ;
            bool allFresh = true;
            for (int i = 0; i < n; ++i) {
                if (!slots[i].o || !slots[i].o->valid || !Member(slots[i].chip)) continue;
                ChipState &c = chips_[slots[i].chip];
                c.s1 = *slots[i].o;
                c.hasS1 = true;
                if (Fresh(c.s1, freshNs)) {
                    c.sum2 = Scale(c.s1.position, timing_.seenSamples);
                    c.n2 = timing_.seenSamples;
                    c.q2 = c.s1.orientation; c.head2 = head; c.headQ2 = headQ;
                } else {
                    allFresh = false;
                }
            }
            if (allFresh) return log + Finish(tS);  // every tracker already in view: no need to look down
            step_ = LookDown;
            stepT0_ = tS;
            lookSince_ = -1;
            return log + "pico_body: calibration step 2: look down at your trackers\n";
        }
        // step 2: look down, collect optical fixes while standing still
        if (fy < -0.5 && lookSince_ < 0) lookSince_ = tS;
        int members = 0, seen = 0;
        for (int i = 0; i < n; ++i) {
            if (!slots[i].o || !Member(slots[i].chip)) continue;
            auto it = chips_.find(slots[i].chip);
            if (it == chips_.end() || !it->second.hasS1) continue;
            ChipState &c = it->second;
            ++members;
            const Tracker::Output &o = *slots[i].o;
            if (c.n2 < timing_.seenSamples && Fresh(o, freshNs) && o.still) {
                if (c.n2 == 0) c.sum2 = {};
                c.sum2 = Add(c.sum2, o.position);
                ++c.n2;
                c.q2 = o.orientation; c.head2 = head; c.headQ2 = headQ;
            }
            if (c.n2 >= timing_.seenSamples) ++seen;
        }
        char buf[160];
        std::snprintf(buf, sizeof(buf), "%s: %d of %d trackers seen, %.0f s left",
                      lookSince_ < 0 ? "look down at your trackers" : "keep looking down, stand still", seen, members, StepLeft(tS));
        note_ = buf;
        if ((members > 0 && seen == members) || StepLeft(tS) <= 0) return log + Finish(tS);
        return log;
    }
    std::string Finish(double tS) {
        std::vector<uint32_t> chips;
        for (auto &kv : chips_)
            if (kv.second.hasS1) chips.push_back(kv.first);
        const int k = int(chips.size());
        if (autoCount_ > 0) {
            bool seen[RoleCount] = {}; Vec3 p[RoleCount]; int prev[RoleCount], role[RoleCount];
            if (k < 2 || k > RoleCount) return Fail(tS, "need 2 to 4 trackers");
            for (int i = 0; i < k; ++i) {
                const ChipState &c = chips_[chips[i]];
                seen[i] = c.n2 >= timing_.seenSamples;
                p[i] = seen[i] ? Scale(c.sum2, 1.0 / c.n2) : Vec3{};
                auto pr = prevRoles_.find(chips[i]);
                prev[i] = pr == prevRoles_.end() ? -1 : pr->second;
            }
            if (!AssignPartial(head1_, headQ1_, seen, p, prev, k, role, guessed_))
                return Fail(tS, "standing pose not plausible for the roles (are all trackers worn?)");
            roles_.clear();
            for (int i = 0; i < k; ++i) roles_[chips[i]] = role[i];
        }
        Standing st; st.head = head1_; st.headQ = headQ1_;
        Seen sn[RoleCount];
        int seenCount = 0, total = 0;
        for (uint32_t chip : chips) {
            const int r = RoleOf(chip);
            if (r < 0) continue;
            const ChipState &c = chips_[chip];
            st.in[r] = In(c.s1, 150e6);
            ++total;
            if (c.n2 >= timing_.seenSamples) {
                sn[r].seen = true;
                sn[r].p = Scale(c.sum2, 1.0 / c.n2);
                sn[r].q = c.q2; sn[r].head = c.head2; sn[r].headQ = c.headQ2;
                ++seenCount;
            }
        }
        body_ = Body();
        body_.SetEyeHeight(eyeHeight_);
        body_.Calibrate(st, sn);
        for (uint32_t chip : chips) {
            const int r = RoleOf(chip);
            if (r < 0) continue;
            ChipState &c = chips_[chip];
            c.mount = sn[r].seen ? "optical" : st.in[r].anchored ? "imu_gravity" : "imu_assumed";
        }
        failNote_.clear();
        result_ = seenCount == 0 ? "imu_only" : seenCount == total ? "optical" : "partial";
        step_ = Done;
        stepT0_ = tS;
        char buf[256];
        if (seenCount == 0) std::snprintf(buf, sizeof(buf), "calibrated IMU-only: no tracker seen, positions estimated from eye height %.2f m%s", body_.GetCalibration().eyeHeight, guessed_ ? ", roles guessed (Swap if wrong)" : "");
        else std::snprintf(buf, sizeof(buf), "calibrated: %d of %d trackers seen by the cameras%s", seenCount, total, guessed_ ? ", roles of the unseen ones guessed" : "");
        note_ = buf;
        char line[512];
        int w = std::snprintf(line, sizeof(line), "pico_body: %s; roles", buf);
        for (auto &kv : roles_) w += std::snprintf(line + w, sizeof(line) - w, " %08X=%s", kv.first, RoleName(kv.second));
        std::snprintf(line + w, sizeof(line) - w, "\n");
        return line;
    }
    std::string Fail(double tS, const char *why) {
        step_ = Stand;
        stepT0_ = tS;
        readySince_ = -1;
        for (auto &kv : chips_) {
            kv.second.n2 = 0;
            kv.second.hasS1 = false;
        }
        failNote_ = std::string(why) + ", calibration restarts at step 1";
        note_ = failNote_;
        return "pico_body: " + note_ + "\n";
    }
    static TrackerIn In(const Tracker::Output &o, double freshNs) {
        TrackerIn t;
        t.present = o.valid;
        t.fresh = Fresh(o, freshNs);
        t.anchored = o.valid && o.positionTracked;
        t.still = o.still;
        t.gravity = o.gravityValid;
        t.p = o.position; t.q = o.orientation; t.g = o.gravityBody;
        return t;
    }
    void Fill(Slot *slots, int n, TrackerIn in[RoleCount], double freshNs) const {
        for (int i = 0; i < n; ++i) {
            if (!slots[i].o) continue;
            const int r = RoleOf(slots[i].chip);
            if (r < 0) continue;
            in[r] = In(*slots[i].o, freshNs);
        }
    }
    std::map<uint32_t, int> fixed_, roles_, prevRoles_;
    std::map<uint32_t, ChipState> chips_;
    int autoCount_ = 0;
    int step_ = Idle;
    bool haveHead_ = false, headStill_ = false, guessed_ = false;
    double readySince_ = -1, stepT0_ = -1, lookSince_ = -1, lastT_ = -1, eyeHeight_ = 1.62;
    std::string result_ = "-", note_ = "-", failNote_;
    Timing timing_;
    Vec3 lastHead_, head1_; Quat lastHeadQ_, headQ1_;
    Body body_;
};

}  // namespace pico_body
