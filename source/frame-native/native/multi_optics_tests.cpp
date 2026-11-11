// 2026-10-31 optical identity for several trackers (multi_optics.hpp). Synthetic stereo rasters with 2-4 tracker
// constellations rendered by the REAL projection and solved by the REAL detector/solver (candidate/release/optical).
// The first target is solved as OpticalWorker does (identity_stereo_candidate under the multi mask). No hardware.
#include "multi_optics.hpp"
#include <chrono>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <fstream>

using namespace pico;
namespace {
size_t checks = 0;
void check_at(bool b, int line) {
    ++checks;
    if (!b) throw std::runtime_error("multi optics check " + std::to_string(checks) + " line " + std::to_string(line));
}
#define check(...) check_at((__VA_ARGS__), __LINE__)
template <class F> void rejects(F f) { bool bad = false; try { f(); } catch (const std::invalid_argument &) { bad = true; } check(bad); }
constexpr uint32_t P = 0xA1B2C3D4, A = 0xA1B2C3E2, B = 0x0bfc3001, STRAY = 0x5a5a5a5a;
constexpr int64_t period = 33333333, latency = 5000000;
// Synthetic constellation shaped like the factory one (mm, rounded); per chip a sub-millimetre offset, as the three
// factory models differ by 0.1-0.4 mm per LED (so the model alone can never tell trackers apart).
LedModel model(double skew) {
    const double mm[24] = {7.4, 16.0, -9.4, -4.6, 13.6, -4.7, -9.3, -1.4, -9.9, -3.7, -14.4, -9.5,
                           10.2, -18.3, -9.4, 22.0, -13.4, -5.0, 24.7, -1.3, -9.3, 19.3, 13.4, -4.3};
    LedModel m{};
    for (size_t i = 0; i < 24; ++i) m[i] = (mm[i] + skew * double(int(i % 3) - 1)) / 1000.;
    return m;
}
Rigid pose(double x, double y, double z, double yaw = 0) {
    auto r = identity_rigid();
    r[0] = std::cos(yaw); r[2] = std::sin(yaw); r[8] = -std::sin(yaw); r[10] = std::cos(yaw);
    r[3] = x; r[7] = y; r[11] = z;
    return r;
}
double dist(const OpticalPoseFrame &f, const Rigid &t) {
    return std::sqrt(std::pow(f.position[0] - t[3], 2) + std::pow(f.position[1] - t[7], 2) + std::pow(f.position[2] - t[11], 2));
}
double dist(const Rigid &a, const Rigid &t) {
    return std::sqrt(std::pow(a[3] - t[3], 2) + std::pow(a[7] - t[7], 2) + std::pow(a[11] - t[11], 2));
}
struct Scene {
    explicit Scene(NativeOpticalApi a) : api(a) {}
    NativeOpticalApi api;
    bool look_down = false, few = false;
    std::map<uint32_t, LedModel> models{{P, model(0)}, {A, model(.2)}, {B, model(-.2)}, {STRAY, model(.1)}};
    std::map<uint32_t, Rigid> truth;
    IdentityView view(double x) {
        IdentityView v;
        v.camera = {900, 900, 528, 512, {0, 0, 0, 0}, 1056, 1024};  // centroids are whole pixels: long focal/baseline
        v.head_from_camera[3] = x;
        if (look_down) { v.head_from_camera[5]=v.head_from_camera[10]=0; v.head_from_camera[6]=-1; v.head_from_camera[9]=1; }
        return v;
    }
    std::shared_ptr<IdentityPair> render(int64_t stamp, const std::set<uint32_t> &lit) {
        auto p = std::make_shared<IdentityPair>();
        p->timestamp_matched = true;
        for (size_t v = 0; v < 2; ++v) {
            auto &f = p->frames[v];
            f.serial = v ? "CAMUPPERRIGHT" : "CAMUPPERLEFT";
            f.metadata = "synthetic multi " + std::to_string(stamp) + "/" + std::to_string(v);
            f.timestamp_matched = true;
            f.stamp = stamp;
            f.exposure_us = 800;
            f.view = view(v ? .15 : -.15);
            f.pixels.assign(1056u * 1024u, 0);
            for (auto chip : lit) {
                const auto &m = models.at(chip);
                const auto &w = truth.at(chip);
                for (size_t i = 0; i < 8; ++i) {
                    // Two LEDs turned away (as in real views): with all eight of this shape the blind solver
                    // overflows its 1024 hypotheses (PICO_HYPOTHESIS_CAPACITY), also for one tracker alone.
                    if (i == 1 || i == 5 || (few && chip != P && i != 0 && i != 3)) continue;
                    double world[3], local[3], px[2];
                    for (int r = 0; r < 3; ++r) { world[r] = w[4 * r + 3]; for (int c = 0; c < 3; ++c) world[r] += w[4 * r + c] * m[3 * i + c]; }
                    for (int r = 0; r < 3; ++r) { local[r]=0; for(int k=0;k<3;++k) local[r]+=f.view.head_from_camera[4*k+r]*(world[k]-f.view.head_from_camera[4*k+3]); }
                    if (api.project(&f.view.camera, local, 1, px) != 0) continue;
                    const int x = int(std::lround(px[0])), y = int(std::lround(px[1]));
                    for (int dy = -1; dy <= 1; ++dy)
                        for (int dx = -1; dx <= 1; ++dx)
                            if (x + dx >= 0 && x + dx < 1056 && y + dy >= 0 && y + dy < 1024)
                                f.pixels[size_t(y + dy) * 1056 + size_t(x + dx)] = 255;
                }
            }
        }
        return p;
    }
};
// One session-like loop: multi mask -> first-target solve (as OpticalWorker) -> multi solve -> publish -> expire.
struct Loop {
    Loop(Scene &s, MultiChipOptics &m) : scene(s), optics(m) {}
    Scene &scene;
    MultiChipOptics &optics;
    int64_t t = 3000000000LL;
    std::optional<Rigid> prior;
    std::function<void(int64_t)> imu;
    uint64_t primary_fits = 0, primary_wrong = 0, masked_blind_ok = 0, imprecise = 0;
    double max_error = 0;
    std::map<uint32_t, uint64_t> published, wrong;
    std::vector<OpticalPoseFrame> last;
    std::vector<OpticalPoseFrame> step(const std::set<uint32_t> &lit, bool blind_primary = false) {
        if (imu) imu(t);
        auto pair = scene.render(t, lit);
        const auto now = t + latency;
        auto mask = optics.before(pair, now);
        std::optional<IdentityCandidate> primary;
        if (lit.count(P)) {
            IdentityBlobMask::Scope scope(mask.get());
            auto c = identity_stereo_candidate(scene.api, scene.models.at(P), pair, now,
                                               blind_primary || !prior ? nullptr : prior->data());
            if (c.pair && (c.stereo_verified || c.mono)) primary = c;
        }
        optics.solved(pair, primary, now, false);
        if (primary) {
            ++primary_fits;
            if (dist(primary->world, scene.truth.at(P)) > .01) ++primary_wrong;
            else if (blind_primary) ++masked_blind_ok;
            prior = primary->world;
            optics.primary_pose(primary->world, primary->stamp);
        }
        optics.expire(now);
        last = optics.take_frames();
        for (const auto &f : last) {
            ++published[f.chip];
            if (!scene.truth.count(f.chip)) { ++wrong[f.chip]; continue; }
            const double own = dist(f, scene.truth.at(f.chip));
            max_error = std::max(max_error, own);
            bool nearer_other = false;
            for (const auto &o : scene.truth) if (o.first != f.chip && dist(f, o.second) < own) nearer_other = true;
            if (own > .05 || nearer_other) ++wrong[f.chip];
            if (own > .01) ++imprecise;
        }
        t += period;
        return last;
    }
    bool has(uint32_t chip) const {
        for (const auto &f : last) if (f.chip == chip) return true;
        return false;
    }
};
// Functional tests do not depend on this host's speed (the sanitizer build is several times slower): budgets are
// effectively unlimited here; budget_exhausted() checks the time limit itself.
MultiOpticsParams untimed() { MultiOpticsParams p; p.budget_ms = p.episode_budget_ms = 60000; return p; }
// Radio emulation: a 25 command published 22 ms before the next pair, acknowledged 9 ms later.
void light(Loop &l, uint32_t chip) { l.optics.lit(chip, l.t - 22000000, l.t - 13000000); }
void unlight(Loop &l, uint32_t chip) { l.optics.off(chip, l.t - 22000000, l.t - 13000000); }
// One identity episode for chip c, as the radio scheduler runs it: ON, (provisional) OFF, (darkened) ON.
// follows=false: the physical tracker does not obey our OFF (a stray/unsynchronised one); visible=false: chip c
// itself is out of view.
MultiChipState episode(Loop &l, uint32_t c, std::set<uint32_t> lit, bool follows = true, bool visible = true) {
    auto &m = l.optics;
    light(l, c);
    if (visible) lit.insert(c);
    for (int i = 0; i < 40 && m.state(c) == MultiChipState::Acquiring; ++i) { l.step(lit); check(!l.has(c)); }
    if (m.state(c) != MultiChipState::Provisional) return m.state(c);
    unlight(l, c);
    if (follows) lit.erase(c);
    for (int i = 0; i < 40 && m.state(c) == MultiChipState::Darkening; ++i) { l.step(lit); check(!l.has(c)); }
    if (m.state(c) != MultiChipState::Darkened) return m.state(c);
    light(l, c);
    if (visible) lit.insert(c);
    for (int i = 0; i < 40 && m.state(c) == MultiChipState::Relit; ++i) l.step(lit);
    return m.state(c);
}

void env(const char* key,const char* value) {
#ifdef _WIN32
    _putenv_s(key,value);
#else
    setenv(key,value,1);
#endif
}
void gyro_recovery(const NativeOpticalApi& api) {
    env("PICO_TRACKING_RECOVERY","1");
    const char* path="build/mrec-calibration.txt";
    { std::ofstream f(path); f<<"A1B2C3E2 1 0 0 0 1 0 0 0 1 .001 0 0 0 0\n"; }
    env("PICO_PRIOR_CALIBRATION",path);
    Scene s(api);
    MultiChipOptics m(api,{P,"",s.models.at(P)},{{A,"",s.models.at(A)}},untimed());
    Loop l(s,m);
    s.truth={{P,pose(0,.02,.6)},{A,pose(.16,.06,.66)}};
    l.imu=[&](int64_t t) {
        for(int64_t stamp=t-50000000;stamp<=t;stamp+=5000000) {
            std::array<uint8_t,32> wire{}; std::copy_n("PTI1",4,wire.begin());
            auto put=[&](int pos,uint64_t value,int n){for(int k=0;k<n;++k)wire[size_t(pos+k)]=uint8_t(value>>(8*k));};
            put(4,A,4); put(12,uint64_t(stamp),8); m.prior_imu(wire);
            // Another chip's high angular velocity cannot rotate A's prior.
            put(4,B,4); put(20,30000,2); m.prior_imu(wire);
        }
    };
    for(int i=0;i<6;++i) l.step({P});
    check(episode(l,A,{P})==MultiChipState::Tracking);
    for(int i=0;i<15;++i) l.step({P}); // >300 ms: old claims expire
    const auto before=l.published[A]; s.few=true;
    for(int i=0;i<5;++i) l.step({P,A});
    check(l.published[A]>before && l.wrong[A]==0);
    const auto supported=l.published[A];
    l.imu={};
    for(int i=0;i<6;++i) l.step({P,A});
    const auto unsupported=l.published[A];
    for(int i=0;i<4;++i) l.step({P,A});
    check(l.published[A]==unsupported); // no stale-tail gyro continuation
    std::cout<<"gyro_recovery few_poses="<<supported-before<<" stale_tail_stopped=1 wrong_chip=0\n";
    env("PICO_PRIOR_CALIBRATION",""); env("PICO_TRACKING_RECOVERY","0");
}
// Synthetic world: pelvis 0.6 m and feet 1.5 m below head. The camera
// looks down; each LED face points up. No claim about real visibility.
void look_down_scene(const NativeOpticalApi& api) {
    Scene s(api); s.look_down=true;
    auto down=[](double x,double depth) {
        auto w=identity_rigid(); w[5]=w[10]=0; w[6]=-1; w[9]=1;
        w[3]=x; w[7]=-depth; return w;
    };
    s.truth={{P,down(0,.6)},{A,down(-.16,1.5)},{B,down(.16,1.5)}};
    MultiChipOptics m(api,{P,"",s.models.at(P)},{{A,"",s.models.at(A)},{B,"",s.models.at(B)}},untimed());
    Loop l(s,m);
    for(int i=0;i<6;++i) l.step({P});
    check(l.primary_fits>0);
    { const auto state=episode(l,A,{P}); const auto c=m.counters(); std::cout<<"look_down state="<<multi_chip_state_name(state)<<" foreign="<<c.foreign<<" provisional="<<c.provisional<<" incomplete="<<c.incomplete<<" overlap="<<c.primary_overlap<<std::endl; check(state==MultiChipState::Tracking); }
    check(episode(l,B,{P,A})==MultiChipState::Tracking);
    for(int i=0;i<6;++i) l.step({P,A,B});
    check(l.published[A]>0 && l.published[B]>0);
    check(l.wrong[A]==0 && l.wrong[B]==0 && l.primary_wrong==0);
    std::cout<<"look_down pelvis=.6m feet=1.5m A="<<l.published[A]<<" B="<<l.published[B]<<" max_error_mm="<<l.max_error*1000<<'\n';
}
void construction(const NativeOpticalApi &api) {
    rejects([&] { MultiChipOptics m(NativeOpticalApi{}, {P, "", model(0)}, {}); });
    rejects([&] { MultiChipOptics m(api, {A, "", model(0)}, {}); });                      // primary must be A1B2C3D4
    rejects([&] { MultiChipOptics m(api, {P, "", model(0)}, {{P, "", model(0)}}); });      // primary not secondary
    rejects([&] { MultiChipOptics m(api, {P, "", model(0)}, {{A, "", model(0)}, {A, "", model(0)}}); });
    auto bad = model(0); bad[5] = std::nan("");
    rejects([&] { MultiChipOptics m(api, {P, "", model(0)}, {{A, "", bad}}); });
}
void identity_and_tracking(const NativeOpticalApi &api) {
    Scene s(api);
    MultiChipOptics m(api, {P, "PCTESTTRACKER0001", s.models.at(P)},
                      {{A, "PCTESTTRACKER0003", s.models.at(A)}, {B, "PCTESTTRACKER0002", s.models.at(B)}}, untimed());
    Loop l(s, m);
    s.truth = {{P, pose(0, .02, .60)}, {A, pose(.16, .06, .66, .2)}, {B, pose(-.16, -.05, .70, -.2)}};
    // 1. Only the first target lit (secondaries dark from configuration): nothing published for A/B.
    for (int i = 0; i < 6; ++i) l.step({P});
    check(l.primary_fits == 6 && l.primary_wrong == 0 && l.published.empty());
    check(m.state(A) == MultiChipState::Dark && m.state(B) == MultiChipState::Dark && m.all_bound_tracked(l.t));
    // 2. One chip at a time: a lit() of B while A is in its episode is ignored.
    light(l, A);
    m.lit(B, l.t, l.t + 1);
    check(m.state(A) == MultiChipState::Acquiring && m.state(B) == MultiChipState::Dark && m.acquiring() == A);
    m.dark(A);
    // 3. A's episode: appears after ON (provisional, not published), vanishes after OFF, back after ON -> bound.
    check(episode(l, A, {P}) == MultiChipState::Tracking && l.has(A) && m.take_bound() == std::vector<uint32_t>{A});
    check(dist(l.last.at(0), s.truth.at(A)) < .01 && l.last.at(0).chip == A && l.last.at(0).valid);
    check(m.counters().provisional == 1 && m.counters().bound == 1 && l.published[A] == 1);
    for (int i = 0; i < 4; ++i) l.step({P, A});
    check(l.published[A] == 5 && l.wrong[A] == 0 && l.primary_wrong == 0);
    // 4. B the same way, while P and A are held (A keeps publishing through B's episode).
    check(episode(l, B, {P, A}) == MultiChipState::Tracking && l.has(A) && l.has(B));
    const auto b_bound = l.published[B];
    // 5. All three moving in different directions: every pose of every chip at its own constellation.
    for (int i = 0; i < 30; ++i) {
        s.truth[P] = pose(.003 * i, .02, .60, .01 * i);
        s.truth[A] = pose(.16, .06 + .004 * i, .66 - .002 * i, .2 - .01 * i);
        s.truth[B] = pose(-.16 - .002 * i, -.05, .70 + .004 * i, -.2);
        l.step({P, A, B});
        check(l.has(A) && l.has(B));
    }
    check(l.wrong[A] == 0 && l.wrong[B] == 0 && l.primary_wrong == 0 && l.published[B] == b_bound + 30);
    const auto c = m.counters();
    // Six rendered LEDs per tracker and view: 2 secondaries x 6 x 2 views masked from the first target per pair.
    check(c.bound == 2 && c.primary_masked >= 30 * 24 && c.ambiguous == 0 && c.foreign == 0 && c.not_dark == 0);
    // 6. The mask lets even a BLIND first-target solve (no prior) find the first target among three constellations.
    for (int i = 0; i < 5; ++i) l.step({P, A, B}, true);
    check(l.masked_blind_ok == 5 && l.primary_wrong == 0);
    // 7. B occluded: no pose of B, A and P continue; after lost_ns B is reported lost (radio darkens it).
    const auto b_before = l.published[B];
    for (int i = 0; i < 33; ++i) { l.step({P, A}); check(!l.has(B) && l.has(A)); }
    check(l.published[B] == b_before && m.state(B) == MultiChipState::Dark && m.take_lost() == std::vector<uint32_t>{B});
    check(m.counters().lost == 1);
    // 8. Unsynchronised B: its LEDs light BEFORE its ON command (foreign), so the window never binds it.
    for (int i = 0; i < 6; ++i) { l.step({P, A, B}); check(!l.has(B)); }
    light(l, B);
    for (int i = 0; i < 32; ++i) { l.step({P, A, B}); check(!l.has(B)); }
    check(m.state(B) == MultiChipState::Dark && m.counters().foreign > 0 && m.counters().windows_expired == 1);
    check(m.take_lost() == std::vector<uint32_t>{B});
    // 9. Two new constellations after B's ON (B and a stray tracker): ambiguous, never bound.
    s.truth[STRAY] = pose(.02, -.18, .75);
    for (int i = 0; i < 15; ++i) l.step({P, A});
    light(l, B);
    for (int i = 0; i < 3; ++i) { l.step({P, A, B, STRAY}); check(!l.has(B)); }
    check(m.counters().ambiguous >= 3 && m.state(B) == MultiChipState::Acquiring);
    m.dark(B);
    // 10. A stray tracker appears alone after B's ON (B itself out of view): provisional, but it ignores our OFF.
    for (int i = 0; i < 15; ++i) l.step({P, A});
    const auto stray = episode(l, B, {P, A, STRAY}, false, false);
    check(stray == MultiChipState::Dark || stray == MultiChipState::Darkening);
    for (int i = 0; i < 40 && m.state(B) != MultiChipState::Dark; ++i) l.step({P, A, STRAY});
    check(m.state(B) == MultiChipState::Dark && m.counters().not_dark == 1 && m.counters().bound == 2);
    check(l.wrong[A] == 0 && l.wrong[B] == 0 && l.primary_wrong == 0);
    std::cout << "identity_and_tracking primary_fits=" << l.primary_fits << " A=" << l.published[A]
              << " B=" << l.published[B] << " contested=" << m.counters().contested
              << " primary_masked=" << m.counters().primary_masked << " foreign=" << m.counters().foreign
              << " ambiguous=" << m.counters().ambiguous << " not_dark=" << m.counters().not_dark
              << " poses_over_1cm=" << l.imprecise << " max_error_mm=" << l.max_error * 1000 << '\n';
}
void swapped_positions(const NativeOpticalApi &api) {
    Scene s(api);
    MultiChipOptics m(api, {P, "", s.models.at(P)}, {{A, "", s.models.at(A)}, {B, "", s.models.at(B)}}, untimed());
    Loop l(s, m);
    const auto left = pose(-.16, -.04, .68), right = pose(.16, .05, .66, .3);
    s.truth = {{P, pose(0, .02, .60)}, {A, left}, {B, right}};
    for (int i = 0; i < 6; ++i) l.step({P});
    check(episode(l, A, {P}) == MultiChipState::Tracking);
    check(episode(l, B, {P, A}) == MultiChipState::Tracking);
    // Both hidden longer than lost_ns, then back with their places swapped.
    for (int i = 0; i < 33; ++i) l.step({P});
    auto lost = m.take_lost();
    check(lost.size() == 2 && m.state(A) == MultiChipState::Dark && m.state(B) == MultiChipState::Dark);
    s.truth[A] = right; s.truth[B] = left;
    // (a) Radio failed to darken them (both lit when they come back): no pose for either, nothing bound.
    for (int i = 0; i < 6; ++i) { l.step({P, A, B}); check(!l.has(A) && !l.has(B)); }
    light(l, A);
    for (int i = 0; i < 4; ++i) { l.step({P, A, B}); check(!l.has(A) && !l.has(B)); }
    check(m.state(A) == MultiChipState::Acquiring && m.counters().foreign > 0);
    m.dark(A);
    // (b) Radio darkened both: A's episode binds A at its NEW place (B's old one), then B at A's old place.
    for (int i = 0; i < 15; ++i) l.step({P});
    check(episode(l, A, {P}) == MultiChipState::Tracking && l.has(A) && dist(l.last.at(0), right) < .01);
    check(episode(l, B, {P, A}) == MultiChipState::Tracking && l.has(B));
    for (int i = 0; i < 10; ++i) l.step({P, A, B});
    check(l.wrong[A] == 0 && l.wrong[B] == 0 && l.published[A] > 10 && l.published[B] > 10);
    // mid-way; the first target moved up out of their way first): identity follows the motion, never swaps, and both
    const auto a_before = l.published[A], b_before = l.published[B];
    for (int i = 0; i <= 40; ++i) {
        const double u = double(i) / 40;
        s.truth[A] = pose(.16 - .32 * u, .05, .66, .3);
        s.truth[B] = pose(-.16 + .32 * u, -.04 + .11 * u, .68 + .27 * u);
        l.step({P, A, B});
    }
    for (int i = 0; i < 4; ++i) l.step({P, A, B});
    check(m.state(A) == MultiChipState::Tracking && m.state(B) == MultiChipState::Tracking);
    check(l.published[A] - a_before >= 30 && l.published[B] - b_before >= 30);
    check(l.wrong[A] == 0 && l.wrong[B] == 0 && l.primary_wrong == 0);
    std::cout << "swapped_positions A=" << l.published[A] << " B=" << l.published[B]
              << " contested=" << m.counters().contested << " fit_drops=" << m.counters().fit_drops
              << " jump_drops=" << m.counters().jump_drops << " lost=" << m.counters().lost
              << " poses_over_1cm=" << l.imprecise << " max_error_mm=" << l.max_error * 1000 << '\n';
}
void primary_not_held(const NativeOpticalApi &api) {
    // The first target occluded: no binding (its constellation could be the one that appears), window expires.
    Scene s(api);
    MultiChipOptics m(api, {P, "", s.models.at(P)}, {{A, "", s.models.at(A)}}, untimed());
    Loop l(s, m);
    s.truth = {{P, pose(0, .02, .60)}, {A, pose(.16, .06, .66)}};
    for (int i = 0; i < 6; ++i) l.step({P});
    for (int i = 0; i < 12; ++i) l.step({});
    light(l, A);
    for (int i = 0; i < 31; ++i) { l.step({A}); check(!l.has(A)); }
    check(m.state(A) == MultiChipState::Dark && m.counters().provisional == 0 && m.take_lost() == std::vector<uint32_t>{A});
    // A window whose pairs never arrive (camera stall) still closes by time.
    for (int i = 0; i < 6; ++i) l.step({P});
    light(l, A);
    m.expire(l.t + 2000000000LL);
    check(m.state(A) == MultiChipState::Dark && m.counters().windows_expired == 2);
}
void budget_exhausted(const NativeOpticalApi &api) {
    // No time for the blind search: the search is incomplete, so nothing becomes provisional (a skipped pairing could
    // be a second constellation); the window just expires. Also reports the measured episode-pair cost.
    Scene s(api);
    MultiOpticsParams p;
    p.budget_ms = p.episode_budget_ms = 0;
    MultiChipOptics m(api, {P, "", s.models.at(P)}, {{A, "", s.models.at(A)}}, p);
    Loop l(s, m);
    s.truth = {{P, pose(0, .02, .60)}, {A, pose(.16, .06, .66)}};
    for (int i = 0; i < 6; ++i) l.step({P});
    light(l, A);
    for (int i = 0; i < 31; ++i) { l.step({P, A}); check(!l.has(A)); }
    check(m.counters().incomplete >= 25 && m.counters().provisional == 0 && m.state(A) == MultiChipState::Dark);
    // Cost of one episode pair with an unlimited budget (two new constellations: 4 stereo pairings), this host.
    MultiChipOptics timed(api, {P, "", s.models.at(P)}, {{A, "", s.models.at(A)}}, untimed());
    Loop t(s, timed);
    s.truth[STRAY] = pose(-.15, -.1, .7);
    for (int i = 0; i < 6; ++i) t.step({P});
    const auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < 3; ++i) t.step({P});
    const auto plain = std::chrono::steady_clock::now();
    light(t, A);
    for (int i = 0; i < 3; ++i) t.step({P, A, STRAY});
    const auto end = std::chrono::steady_clock::now();
    check(timed.counters().ambiguous == 3);
    std::cout << "budget_exhausted incomplete=" << m.counters().incomplete
              << " pair_ms(render + first target)=" << std::chrono::duration<double, std::milli>(plain - t0).count() / 3
              << " pair_ms(episode, 2 new constellations)="
              << std::chrono::duration<double, std::milli>(end - plain).count() / 3 << '\n';
}
} // namespace
int main() {
    try {
#ifdef _WIN32
        NativeOpticalLibraries libraries("build/mrec_optics.dll", "build/mrec_detector.dll");
#else
        NativeOpticalLibraries libraries("../candidate/release/optical/libpico_optics.so",
                                         "../candidate/release/optical/libpico_detector.so");
#endif
        const auto api = libraries.api();
        gyro_recovery(api);
        look_down_scene(api);
        env("PICO_TRACKING_RECOVERY","1");
        look_down_scene(api); // missing calibration still allows blind full measured fits
        env("PICO_TRACKING_RECOVERY","0");
        construction(api);
        identity_and_tracking(api);
        swapped_positions(api);
        primary_not_held(api);
        budget_exhausted(api);
        std::cout << "multi optics PASS " << checks << " checks\n";
        return 0;
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
