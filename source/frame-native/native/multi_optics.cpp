#include "multi_optics.hpp"
#include "mono_solver.hpp"
#include "prior_consensus.hpp"
#include "peer_table.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <map>
#include <numeric>
#include <sstream>

namespace pico {
namespace {
using Pixels = std::vector<std::array<double, 2>>;
void log_multi_optics(const std::string &line) {
    std::ofstream("/tmp/pico-diag.log", std::ios::app) << "multi_optics: " << line << std::endl;
}
std::string hex(uint32_t chip) { std::ostringstream o; o << std::hex << chip; return o.str(); }
Rigid mul(const Rigid &a, const Rigid &b) {
    Rigid c{};
    for (int r = 0; r < 4; ++r)
        for (int col = 0; col < 4; ++col)
            for (int k = 0; k < 4; ++k) c[4 * r + col] += a[4 * r + k] * b[4 * k + col];
    return c;
}
Rigid inverse_rigid(const Rigid &a) {
    Rigid b = identity_rigid();
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) b[4 * i + j] = a[4 * j + i];
        b[4 * i + 3] = 0;
        for (int j = 0; j < 3; ++j) b[4 * i + 3] -= a[4 * j + i] * a[4 * j + 3];
    }
    return b;
}
std::array<double, 3> apply(const Rigid &a, const double *p) {
    std::array<double, 3> o{};
    for (int r = 0; r < 3; ++r) { o[r] = a[4 * r + 3]; for (int c = 0; c < 3; ++c) o[r] += a[4 * r + c] * p[c]; }
    return o;
}
double distance3(const Rigid &a, const Rigid &b) {
    double d = 0;
    for (int i = 0; i < 3; ++i) d += std::pow(a[4 * i + 3] - b[4 * i + 3], 2);
    return std::sqrt(d);
}
// Projected LEDs of one tracker pose in an upper view (solver pixel convention: offset included).
Pixels project_leds(const NativeOpticalApi &api, const IdentityExposure &f, const Rigid &world, const LedModel &m) {
    Pixels out;
    const auto camera_from_world = inverse_rigid(mul(f.world_from_head, f.view.head_from_camera));
    for (size_t i = 0; i < 8; ++i) {
        const auto w = apply(world, m.data() + 3 * i);
        const auto c = apply(camera_from_world, w.data());
        if (!(c[2] > 1e-3)) continue;
        double px[2];
        if (api.project(&f.view.camera, c.data(), 1, px) == 0 && std::isfinite(px[0]) && std::isfinite(px[1]))
            out.push_back({px[0], px[1]});
    }
    return out;
}
double nearest(const Pixels &leds, double x, double y) {
    double best = 1e30;
    for (const auto &p : leds) best = std::min(best, std::hypot(p[0] - x, p[1] - y));
    return best;
}
bool fit_bounds(const OpticalFit &f) {  // optical_worker.cpp fit_valid
    return std::isfinite(f.residual) && f.residual >= 0 && f.residual <= 2.5 && std::isfinite(f.condition) &&
           f.condition >= 0 && f.condition <= 1e7 && std::isfinite(f.position_std) && f.position_std >= 0 &&
           f.position_std <= .03 && std::isfinite(f.rotation_std_deg) && f.rotation_std_deg >= 0 &&
           f.rotation_std_deg <= 5;
}
std::array<double, 4> quaternion(const Rigid &m) {  // optical_worker.cpp
    std::array<double, 4> q{};
    double trace = m[0] + m[5] + m[10];
    if (trace > 0) {
        double s = 2 * std::sqrt(trace + 1);
        q = {(m[9] - m[6]) / s, (m[2] - m[8]) / s, (m[4] - m[1]) / s, s / 4};
    } else {
        int i = m[5] > m[0] ? 1 : 0;
        if (m[10] > m[5 * i]) i = 2;
        int j = (i + 1) % 3, k = (i + 2) % 3;
        double s = 2 * std::sqrt(1 + m[5 * i] - m[5 * j] - m[5 * k]);
        q[i] = s / 4; q[j] = (m[4 * i + j] + m[4 * j + i]) / s;
        q[k] = (m[4 * i + k] + m[4 * k + i]) / s; q[3] = (m[4 * k + j] - m[4 * j + k]) / s;
    }
    return q;
}
// Seeded (prior) or blind solve on one tracker's own blobs; stereo first, otherwise calibrated mono.
std::optional<OpticalFit> solve_subset(const NativeOpticalApi &api, const LedModel &model,
                                       const IdentityPair &pair, const std::array<std::vector<double>, 2> &xy,
                                       const double *prior, double dt = 0, double max_turn = 180, bool *few = nullptr) {
    OpticalView views[2]{};
    for (size_t v = 0; v < 2; ++v) {
        const auto &f = pair.frames[v];
        views[v].camera = f.view.camera;
        const auto t = mul(f.world_from_head, f.view.head_from_camera);
        std::copy(t.begin(), t.end(), views[v].head_from_camera);
        views[v].count = xy[v].size() / 2;
        views[v].pixels = xy[v].data();
    }
    auto consistent = [&](const OpticalFit& candidate) {
        if (!prior || max_turn >= 180) return true;
        Rigid seed, world;
        std::copy(prior, prior+16, seed.begin());
        std::copy(std::begin(candidate.pose), std::end(candidate.pose), world.begin());
        return OpticalPrior::angle(seed, world) <= max_turn;
    };
    const bool full = views[0].count >= 4 && views[1].count >= 4;
    const bool three = prior && views[0].count >= 3 && views[1].count >= 3;
    OpticalFit fit{};
    if (full || three) {
        int rc = full ? api.solve(model.data(), views, nullptr, &fit) : 1;
        if (prior) {
            OpticalFit seeded{};
            auto innovation = [&](const OpticalFit &f) {
                double trace = 0;
                for (int i = 0; i < 3; ++i) for (int k = 0; k < 3; ++k) trace += prior[4 * k + i] * f.pose[4 * k + i];
                return std::acos(std::clamp((trace - 1) / 2, -1., 1.));
            };
            if (api.solve(model.data(), views, prior, &seeded) == 0 && (rc != 0 || innovation(seeded) <= innovation(fit))) {
                fit = seeded;
                rc = 0;
            }
        }
        if (rc == 0 && (!tracking_recovery_enabled() || (fit_bounds(fit) && consistent(fit)))) return fit;
    }
    if (prior && tracking_recovery_enabled() && dt > 0) {
        Rigid seed; std::copy(prior, prior + 16, seed.begin());
        int observations = 0; bool rotation = false;
        if (solve_prior_consensus(api, model, views, seed, dt, fit, observations, rotation) ||
            solve_few_led(api, model, views, seed, dt, fit, observations, rotation, true)) {
            if (few) *few = true;
            return fit;
        }
    }
    const int mono = views[0].count >= 4 ? 0 : views[1].count >= 4 ? 1 : -1;
    if (mono >= 0 && solve_monocular(api, model, views[mono], fit, prior) && consistent(fit)) return fit;
    return {};
}
// Single linkage of the given blob indices (pixel distance <= limit).
std::vector<std::vector<size_t>> clusters(const std::vector<double> &blobs, const std::vector<size_t> &ids, double limit) {
    std::vector<size_t> parent(ids.size());
    std::iota(parent.begin(), parent.end(), size_t(0));
    auto root = [&](size_t i) { while (parent[i] != i) i = parent[i] = parent[parent[i]];
    return i; };
    for (size_t a = 0; a < ids.size(); ++a)
        for (size_t b = a + 1; b < ids.size(); ++b)
            if (std::hypot(blobs[5 * ids[a]] - blobs[5 * ids[b]], blobs[5 * ids[a] + 1] - blobs[5 * ids[b] + 1]) <= limit)
                parent[root(a)] = root(b);
    std::map<size_t, std::vector<size_t>> groups;
    for (size_t a = 0; a < ids.size(); ++a) groups[root(a)].push_back(ids[a]);
    std::vector<std::vector<size_t>> out;
    for (auto &g : groups) out.push_back(std::move(g.second));
    return out;
}
std::array<double, 2> centroid(const std::vector<double> &blobs, const std::vector<size_t> &ids) {
    std::array<double, 2> c{};
    for (auto i : ids) { c[0] += blobs[5 * i]; c[1] += blobs[5 * i + 1]; }
    c[0] /= double(ids.size()); c[1] /= double(ids.size());
    return c;
}
std::vector<double> xy_of(const std::vector<double> &blobs, const std::vector<size_t> &ids) {
    std::vector<double> xy;
    for (auto i : ids) { xy.push_back(blobs[5 * i]); xy.push_back(blobs[5 * i + 1]); }
    return xy;
}
struct Claim { int owner; std::array<Pixels, 2> leds; double gate; };
// Constant-velocity prediction of a pose to the pair stamp (rotation held), at most 300 ms ahead.
Rigid predicted(Rigid world, const std::array<double, 3> &velocity, int64_t from, int64_t to) {
    const double dt = std::clamp(double(to - from) / 1e9, 0., .3);
    for (int i = 0; i < 3; ++i) world[4 * i + 3] += velocity[i] * dt;
    return world;
}
// owner per blob: -1 unclaimed, -2 contested, else index into claims. A fresh primary fit (gate own_px) always
// keeps its own blobs; otherwise a blob that two tracks reach within contest_ratio of each other goes to nobody.
// Claims use the velocity-predicted pose: from the last pose alone, a fast near tracker's blob was nearer a far
// tracker's LED mid-crossing (tests: a 23 mm error that still passed the fit bounds).
std::array<std::vector<int>, 2> assign(const IdentityBlobMask &m, const std::vector<Claim> &claims, double ratio,
                                       bool primary_fresh, double own_px, uint64_t &contested) {
    std::array<std::vector<int>, 2> owner;
    for (size_t v = 0; v < 2; ++v) {
        const auto &b = m.blobs[v];
        owner[v].assign(b.size() / 5, -1);
        for (size_t i = 0; i < b.size() / 5; ++i) {
            int k1 = -1, k2 = -1;
            double d1 = 1e30, d2 = 1e30;
            for (size_t k = 0; k < claims.size(); ++k) {
                const double d = nearest(claims[k].leds[v], b[5 * i], b[5 * i + 1]);
                if (d > claims[k].gate) continue;
                if (d < d1) { k2 = k1; d2 = d1; k1 = int(k); d1 = d; }
                else if (d < d2) { k2 = int(k); d2 = d; }
            }
            if (k1 < 0) continue;
            if (primary_fresh && claims[size_t(k1)].owner == 0 && d1 <= own_px) owner[v][i] = k1;
            else if (k2 >= 0 && d2 < ratio * std::max(d1, .5)) { owner[v][i] = -2; ++contested; }
            else owner[v][i] = k1;
        }
    }
    return owner;
}
} // namespace

const char *multi_chip_state_name(MultiChipState s) {
    switch (s) {
    case MultiChipState::Dark: return "dark";
    case MultiChipState::Acquiring: return "acquiring";
    case MultiChipState::Provisional: return "provisional";
    case MultiChipState::Darkening: return "darkening";
    case MultiChipState::Darkened: return "darkened";
    case MultiChipState::Relit: return "relit";
    case MultiChipState::Tracking: return "tracking";
    }
    return "?";
}
MultiChipOptics::MultiChipOptics(NativeOpticalApi api, MultiChipModel primary, std::vector<MultiChipModel> secondaries,
                                 Params p)
    : api_(api), primary_(std::move(primary)), p_(p) {
    if (!api_.detect || !api_.solve || !api_.project || primary_.chip != first_target_chip)
        throw std::invalid_argument("Multi-tracker optics needs the native ABI and the first target as primary");
    for (const auto &s : secondaries) {
        for (double x : s.model)
            if (!std::isfinite(x)) throw std::invalid_argument("Finite factory LED model required");
        if (!s.chip || s.chip == primary_.chip || find(s.chip))
            throw std::invalid_argument("Secondary optical chip invalid or enrolled twice");
        Track t;
        t.chip = s.chip;
        t.model = s.model;
        if (const char* path = std::getenv("PICO_PRIOR_CALIBRATION")) t.calibrated = t.gyro->load(path, s.chip);
        tracks_.push_back(t);
        log_multi_optics("chip " + hex(s.chip) + " serial " + s.serial + " optical model loaded; LEDs dark until lit");
    }
}
MultiChipOptics::Track *MultiChipOptics::find(uint32_t chip) {
    for (auto &t : tracks_) if (t.chip == chip) return &t;
    return nullptr;
}
const MultiChipOptics::Track *MultiChipOptics::find(uint32_t chip) const {
    for (const auto &t : tracks_) if (t.chip == chip) return &t;
    return nullptr;
}
void MultiChipOptics::prior_imu(const std::array<uint8_t,32>& bytes) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& t : tracks_) t.gyro->wire(bytes.data(), bytes.size());
}
void MultiChipOptics::close(Track &t, const char *why) {
    log_multi_optics("chip " + hex(t.chip) + " " + multi_chip_state_name(t.state) + " -> dark: " + why);
    t.state = MultiChipState::Dark;
    ++t.epoch;
    t.world.reset();
    t.last.reset();
    t.dark_seen = 0;
    lost_.push_back(t.chip);
}
void MultiChipOptics::lit(uint32_t chip, int64_t published, int64_t ack) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto *t = find(chip);
    if (!t) return;
    if (t->state == MultiChipState::Darkened) {  // second ON of the episode
        t->state = MultiChipState::Relit;
        ++t->epoch;
        t->on_published = published;
        t->on_ack = ack;
        log_multi_optics("chip " + hex(chip) + " relit: second ON ack " + std::to_string(ack));
        return;
    }
    for (const auto &o : tracks_)
        if (o.chip != chip && o.state != MultiChipState::Dark && o.state != MultiChipState::Tracking) {
            log_multi_optics("lit " + hex(chip) + " while " + hex(o.chip) + " is in its episode; ignored (one chip at a time)");
            return;
        }
    if (t->state != MultiChipState::Dark) {
        log_multi_optics("lit " + hex(chip) + " in state " + multi_chip_state_name(t->state) + "; ignored");
        return;
    }
    t->state = MultiChipState::Acquiring;
    ++t->epoch;
    t->world.reset();
    t->dark_seen = 0;
    t->on_published = published;
    t->on_ack = ack;
    log_multi_optics("chip " + hex(chip) + " lit: ON published " + std::to_string(published) + " ack " +
                     std::to_string(ack) + "; acquisition window open");
}
void MultiChipOptics::off(uint32_t chip, int64_t published, int64_t ack) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto *t = find(chip);
    if (!t || t->state != MultiChipState::Provisional) return;
    t->state = MultiChipState::Darkening;
    ++t->epoch;
    t->off_published = published;
    t->off_ack = ack;
    t->dark_seen = 0;
    log_multi_optics("chip " + hex(chip) + " OFF ack " + std::to_string(ack) + "; its constellation must vanish");
}
void MultiChipOptics::dark(uint32_t chip) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (auto *t = find(chip)) {
        t->state = MultiChipState::Dark;
        ++t->epoch;
        t->world.reset();
        t->last.reset();
        t->dark_seen = 0;
    }
}
void MultiChipOptics::expire(int64_t now) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto &t : tracks_) {
        switch (t.state) {
        case MultiChipState::Acquiring:
        case MultiChipState::Relit:
            if (now > t.on_ack + p_.window_ns) {
                ++counters_.windows_expired;
                if (t.state == MultiChipState::Relit) ++counters_.not_back;
                close(t, "window expired without a unique new constellation");
            }
            break;
        case MultiChipState::Darkening:
            if (now > t.off_ack + p_.window_ns) {
                ++counters_.not_dark;
                close(t, "constellation did not vanish after OFF (not this tracker)");
            }
            break;
        case MultiChipState::Darkened:
            if (now > t.off_ack + 2 * p_.window_ns) close(t, "second ON not acknowledged in time");
            break;
        case MultiChipState::Provisional:
        case MultiChipState::Tracking:
            if (now > t.stamp + (t.state == MultiChipState::Tracking && tracking_recovery_enabled() &&
                t.calibrated && t.gyro->predict(now) ? std::max(p_.lost_ns, prior_hold_ns()) : p_.lost_ns)) {
                ++counters_.lost;
                close(t, "lost (no fit within lost_ns); rebinding needs a new episode");
            }
            break;
        default: break;
        }
    }
}
MultiChipState MultiChipOptics::state(uint32_t chip) const {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto *t = find(chip);
    return t ? t->state : MultiChipState::Dark;
}
std::optional<uint32_t> MultiChipOptics::acquiring() const {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto &t : tracks_)
        if (t.state != MultiChipState::Dark && t.state != MultiChipState::Tracking) return t.chip;
    return {};
}
bool MultiChipOptics::primary_tracked(int64_t now) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return primary_world_ && now >= primary_stamp_ && now - primary_stamp_ <= p_.track_ns;
}
bool MultiChipOptics::all_bound_tracked(int64_t now) const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!(primary_world_ && now >= primary_stamp_ && now - primary_stamp_ <= p_.track_ns)) return false;
    for (const auto &t : tracks_)
        if (t.state == MultiChipState::Tracking && !(now >= t.stamp && now - t.stamp <= p_.track_ns)) return false;
    return true;
}
std::vector<uint32_t> MultiChipOptics::take_lost() { std::lock_guard<std::mutex> lock(mutex_); return std::exchange(lost_, {}); }
std::vector<uint32_t> MultiChipOptics::take_bound() { std::lock_guard<std::mutex> lock(mutex_); return std::exchange(bound_, {}); }
std::vector<uint32_t> MultiChipOptics::chips() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<uint32_t> c;
    for (const auto &t : tracks_) c.push_back(t.chip);
    return c;
}
std::vector<OpticalPoseFrame> MultiChipOptics::take_frames() {
    std::lock_guard<std::mutex> lock(mutex_);
    return std::exchange(frames_, {});
}
void MultiChipOptics::publish(int64_t now) {
    std::vector<OpticalPoseFrame> frames;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!send) return;  // kept for take_frames()
        frames = std::exchange(frames_, {});
    }
    for (const auto &f : frames)
        try {
            if (!send(f, now)) log_optical_drop("Multi-tracker PTP2 pose not sent");
        } catch (const std::exception &e) {
            log_multi_optics(std::string("PTP2 send of chip ") + hex(f.chip) + " failed: " + e.what());
        }
}
void MultiChipOptics::discard() {
    std::lock_guard<std::mutex> lock(mutex_);
    frames_.clear();
    mask_.reset();
}
MultiOpticsCounters MultiChipOptics::counters() const { std::lock_guard<std::mutex> lock(mutex_); return counters_; }
uint64_t MultiChipOptics::chip_published(uint32_t chip) const {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto *t = find(chip);
    return t ? t->published : 0;
}
void MultiChipOptics::primary_pose(const Rigid &world, int64_t stamp) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (stamp < primary_stamp_) return;
    if (primary_world_ && stamp > primary_stamp_ && stamp - primary_stamp_ < 250000000) {
        const double dt = double(stamp - primary_stamp_) / 1e9, alpha = 1 - std::exp(-dt / .05);
        for (int i = 0; i < 3; ++i)
            primary_velocity_[i] = (1 - alpha) * primary_velocity_[i] + alpha * (world[4 * i + 3] - (*primary_world_)[4 * i + 3]) / dt;
    } else primary_velocity_ = {};
    primary_world_ = world;
    primary_stamp_ = stamp;
}
namespace {
bool claims_blobs(MultiChipState s) { return s == MultiChipState::Tracking || s == MultiChipState::Provisional; }
}
std::shared_ptr<const IdentityBlobMask> MultiChipOptics::before(const std::shared_ptr<const IdentityPair> &pair,
                                                                int64_t now) {
    auto mask = std::make_shared<IdentityBlobMask>();
    mask->detector = api_.detect;
    for (size_t v = 0; v < 2; ++v) {
        if (identity_detect_blobs(api_, pair->frames[v], mask->blobs[v])) mask->source[v] = pair->frames[v].pixels.data();
        else mask->blobs[v].clear();  // logged; the primary detector runs (and drops) as before
        mask->excluded[v].assign(mask->blobs[v].size() / 5, false);
    }
    std::vector<Claim> claims;
    bool episode = false;
    const int64_t pair_stamp = std::min(pair->frames[0].stamp, pair->frames[1].stamp);
    {
        std::lock_guard<std::mutex> lock(mutex_);
        ++counters_.pairs;
        if (primary_world_ && now - primary_stamp_ <= p_.track_ns) {
            Claim c{0, {}, p_.gate_px};
            const auto w = predicted(*primary_world_, primary_velocity_, primary_stamp_, pair_stamp);
            for (size_t v = 0; v < 2; ++v) c.leds[v] = project_leds(api_, pair->frames[v], w, primary_.model);
            claims.push_back(std::move(c));
        }
        for (const auto &t : tracks_) {
            episode = episode || (t.state != MultiChipState::Dark && t.state != MultiChipState::Tracking);
            if (!claims_blobs(t.state) || !t.world || now - t.stamp > (tracking_recovery_enabled() && t.calibrated ? std::max(p_.track_ns, prior_hold_ns()) : p_.track_ns)) continue;
            Claim c{1, {}, p_.gate_px};
            const auto g = tracking_recovery_enabled() ? t.gyro->predict(pair_stamp) : std::optional<Rigid>{};
            if (tracking_recovery_enabled() && !g && now - t.stamp > p_.track_ns) continue;
            const auto w = predicted(g.value_or(*t.world), t.velocity, t.stamp, pair_stamp);
            for (size_t v = 0; v < 2; ++v) c.leds[v] = project_leds(api_, pair->frames[v], w, t.model);
            claims.push_back(std::move(c));
        }
    }
    uint64_t contested = 0, masked = 0;
    const auto owner = assign(*mask, claims, p_.contest_ratio, false, p_.own_px, contested);
    const bool primary_predicted = !claims.empty() && claims[0].owner == 0;
    for (size_t v = 0; v < 2; ++v)
        for (size_t i = 0; i < owner[v].size(); ++i) {
            bool drop = owner[v][i] == -2 || (owner[v][i] >= 0 && claims[size_t(owner[v][i])].owner != 0);
            // While a secondary is in its episode, its new constellation must not feed the first target's solve.
            if (!drop && episode && primary_predicted && owner[v][i] == -1) drop = true;
            if (drop) { mask->excluded[v][i] = true; ++masked; }
        }
    std::lock_guard<std::mutex> lock(mutex_);
    counters_.primary_masked += masked;
    mask_ = mask;
    return mask;
}
void MultiChipOptics::solved(const std::shared_ptr<const IdentityPair> &pair,
                             const std::optional<IdentityCandidate> &primary, int64_t now, bool dark_frame) {
    const auto start = std::chrono::steady_clock::now();
    auto deadline = start + std::chrono::milliseconds(p_.budget_ms);
    std::shared_ptr<IdentityBlobMask> mask;
    std::vector<Track> tracks;
    std::optional<Rigid> primary_world;
    int64_t primary_stamp = 0;
    std::array<double, 3> primary_velocity{};
    std::deque<Seen> seen;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        mask = mask_;
        tracks = tracks_;
        primary_world = primary_world_;
        primary_stamp = primary_stamp_;
        primary_velocity = primary_velocity_;
        seen = seen_;
    }
    if (!mask || mask->source[0] != pair->frames[0].pixels.data() || mask->source[1] != pair->frames[1].pixels.data()) {
        mask = std::make_shared<IdentityBlobMask>();
        mask->detector = api_.detect;
        for (size_t v = 0; v < 2; ++v) {
            if (identity_detect_blobs(api_, pair->frames[v], mask->blobs[v])) mask->source[v] = pair->frames[v].pixels.data();
            else mask->blobs[v].clear();
        }
    }
    const int64_t stamp = std::min(pair->frames[0].stamp, pair->frames[1].stamp);
    // The first target's blobs: its fresh fit of this pair (tight gate), else its last pose (prediction gate).
    std::vector<Claim> claims;
    std::vector<int> claim_track;
    const bool primary_fresh = primary && primary->pair.get() == pair.get();
    const std::optional<Rigid> primary_now = primary_fresh ? std::optional<Rigid>(primary->world)
        : (primary_world && now - primary_stamp <= p_.track_ns
               ? std::optional<Rigid>(predicted(*primary_world, primary_velocity, primary_stamp, stamp)) : std::nullopt);
    if (primary_now) {
        Claim c{0, {}, primary_fresh ? p_.own_px : p_.gate_px};
        for (size_t v = 0; v < 2; ++v) c.leds[v] = project_leds(api_, pair->frames[v], *primary_now, primary_.model);
        claims.push_back(std::move(c));
        claim_track.push_back(-1);
    }
    for (size_t k = 0; k < tracks.size(); ++k) {
        const auto &t = tracks[k];
        if (!claims_blobs(t.state) || !t.world || now - t.stamp > (tracking_recovery_enabled() && t.calibrated ? std::max(p_.track_ns, prior_hold_ns()) : p_.track_ns)) continue;
        Claim c{1, {}, p_.gate_px};
        const auto g = tracking_recovery_enabled() ? t.gyro->predict(stamp) : std::optional<Rigid>{};
        if (tracking_recovery_enabled() && !g && now - t.stamp > p_.track_ns) continue;
        const auto w = predicted(g.value_or(*t.world), t.velocity, t.stamp, stamp);
        for (size_t v = 0; v < 2; ++v) c.leds[v] = project_leds(api_, pair->frames[v], w, t.model);
        claims.push_back(std::move(c));
        claim_track.push_back(int(k));
    }
    uint64_t contested = 0;
    const auto owner = assign(*mask, claims, p_.contest_ratio, primary_fresh, p_.own_px, contested);
    MultiOpticsCounters delta;
    std::vector<OpticalPoseFrame> frames;
    std::vector<uint32_t> newly_bound;
    auto accept = [&](Track &t, const Rigid &world, const OpticalFit &fit, bool few = false) {
        OpticalPoseFrame f;
        f.chip = t.chip; f.sequence = t.sequence; f.stamp = stamp;
        f.connected = f.valid = true;
        f.quality = std::min(1., fit.count / 8.) * std::exp(-fit.residual / 2);
        if (few) f.quality = std::min(.24, f.quality);
        f.quaternion = quaternion(world);
        for (size_t i = 0; i < 3; ++i) f.position[i] = world[4 * i + 3];
        if (t.last && stamp > t.last->stamp) {  // same velocity filter as OpticalWorker
            const double dt = double(stamp - t.last->stamp) / 1e9;
            double distance = 0;
            for (size_t i = 0; i < 3; ++i) distance += std::pow(f.position[i] - t.last->position[i], 2);
            distance = std::sqrt(distance);
            if (dt > .005 && dt < .25 && distance / dt <= 8) {
                const auto alpha = 1 - std::exp(-dt / .05);
                for (size_t i = 0; i < 3; ++i)
                    f.velocity[i] = (1 - alpha) * t.last->velocity[i] + alpha * (f.position[i] - t.last->position[i]) / dt;
            }
        }
        if (t.world && stamp > t.stamp && stamp - t.stamp < 250000000) {
            const double dt = double(stamp - t.stamp) / 1e9, alpha = 1 - std::exp(-dt / .05);
            for (int i = 0; i < 3; ++i) t.velocity[i] = (1 - alpha) * t.velocity[i] + alpha * (world[4 * i + 3] - (*t.world)[4 * i + 3]) / dt;
        } else t.velocity = {};
        t.world = world;
        t.stamp = stamp;
        // Provisional constellations are tracked, never published; dark exposures never publish (OpticalWorker rule).
        // (The PTP2 sink validates and encodes; this unit stays free of the publication library.)
        if (t.state != MultiChipState::Tracking || dark_frame) return;
        ++t.sequence; ++t.published; ++delta.published;
        t.last = f;
        frames.push_back(f);
    };
    // 1. Tracking/provisional secondaries: seeded solve on their own blobs only.
    for (size_t c = 0; c < claims.size(); ++c) {
        if (claim_track[c] < 0) continue;
        auto &t = tracks[size_t(claim_track[c])];
        std::array<std::vector<double>, 2> xy;
        for (size_t v = 0; v < 2; ++v)
            for (size_t i = 0; i < owner[v].size(); ++i)
                if (owner[v][i] == int(c)) { xy[v].push_back(mask->blobs[v][5 * i]); xy[v].push_back(mask->blobs[v][5 * i + 1]); }
        if (std::chrono::steady_clock::now() >= deadline) { ++delta.fit_drops; ++delta.incomplete; continue; }
        const auto gyro = tracking_recovery_enabled() ? t.gyro->predict(stamp) : std::optional<Rigid>{};
        const bool recovery = tracking_recovery_enabled();
        const auto seed = gyro ? predicted(*gyro, t.velocity, t.stamp, stamp) : *t.world;
        // Unsupported IMU may still admit a blind full measured fit, never a
        // seeded/few-LED continuation (same policy as the primary worker).
        bool few = false;
        OpticalComputeBudget budget; budget.end = std::min(budget.end, deadline);
        const auto fit = solve_subset(api_, t.model, *pair, xy, recovery && !gyro ? nullptr : seed.data(),
                                      recovery && gyro ? double(stamp-t.stamp)*1e-9 : 0,
                                      recovery && gyro ? t.gyro->consistency_bound_deg(stamp) : 180, &few);
        if (!fit || !fit_bounds(*fit)) { ++delta.fit_drops; continue; }
        Rigid world; std::copy(std::begin(fit->pose), std::end(fit->pose), world.begin());
        if (recovery && !t.gyro->consistent(world, stamp, gyro)) { ++delta.fit_drops; continue; }
        const double dt = std::max(0., double(stamp - t.stamp) / 1e9);
        if (distance3(world, *t.world) > std::max(p_.jump_m, p_.speed_mps * dt)) { ++delta.jump_drops; continue; }
        ++delta.fits;
        accept(t, world, *fit, few);
    }
    // Unclaimed blobs grouped per view: the "scene" record and the acquisition candidates.
    std::array<std::vector<std::array<double, 2>>, 2> unclaimed_clusters;
    std::array<std::vector<std::vector<size_t>>, 2> groups;
    for (size_t v = 0; v < 2; ++v) {
        std::vector<size_t> free;
        for (size_t i = 0; i < owner[v].size(); ++i) if (owner[v][i] == -1) free.push_back(i);
        for (auto &g : clusters(mask->blobs[v], free, p_.cluster_px)) {
            if (g.size() >= 2) unclaimed_clusters[v].push_back(centroid(mask->blobs[v], g));
            if (g.size() >= 3 && g.size() <= 12) groups[v].push_back(std::move(g));
        }
    }
    // 2. The one chip in its identity episode, while every bound tracker is held.
    Track *episode = nullptr;
    for (auto &t : tracks)
        if (t.state == MultiChipState::Acquiring || t.state == MultiChipState::Darkening || t.state == MultiChipState::Relit)
            episode = &t;
    bool held = primary_now.has_value();
    for (const auto &t : tracks)
        if (t.state == MultiChipState::Tracking && !(t.world && now - t.stamp <= p_.track_ns)) held = false;
    if (episode && episode->state == MultiChipState::Darkening && stamp >= episode->off_ack && episode->world) {
        // OFF acknowledged: no blob may remain at the provisional constellation's place, in either view.
        bool any = false;
        for (size_t v = 0; v < 2; ++v) {
            const auto leds = project_leds(api_, pair->frames[v], *episode->world, episode->model);
            const auto &b = mask->blobs[v];
            for (size_t i = 0; i < b.size() / 5; ++i)
                if (nearest(leds, b[5 * i], b[5 * i + 1]) <= p_.gate_px) {
                    // A fresh measured primary fit explains its own LEDs even when the
                    // distant secondary projects within the broad continuation gate.
                    if (primary_fresh && owner[v][i] >= 0 && claim_track[size_t(owner[v][i])] < 0) continue;
                    any = true;
                }
        }
        if (any) episode->dark_seen = 0;
        else if (++episode->dark_seen >= p_.dark_pairs) episode->state = MultiChipState::Darkened;
    }
    const bool window = episode && (episode->state == MultiChipState::Acquiring || episode->state == MultiChipState::Relit) &&
                        stamp >= episode->on_published && now <= episode->on_ack + p_.window_ns;
    if (window && held) {
        deadline = std::max(deadline, start + std::chrono::milliseconds(p_.episode_budget_ms));
        // The "before" scene: Acquiring = pairs in [on_published - before_ns, on_published);
        // Relit = the dark pairs between the OFF ACK and the second ON publication.
        const int64_t from = episode->state == MultiChipState::Relit ? episode->off_ack : episode->on_published - p_.before_ns;
        std::array<std::vector<std::array<double, 2>>, 2> foreign;
        bool have_before = false;
        for (const auto &s : seen)
            if (s.stamp < episode->on_published && s.stamp >= from) {
                have_before = true;
                for (size_t v = 0; v < 2; ++v) foreign[v].insert(foreign[v].end(), s.clusters[v].begin(), s.clusters[v].end());
            }
        auto is_foreign = [&](size_t v, const std::vector<size_t> &g) {
            const auto c = centroid(mask->blobs[v], g);
            for (const auto &f : foreign[v]) if (std::hypot(f[0] - c[0], f[1] - c[1]) <= p_.cluster_px) return true;
            return false;
        };
        if (!have_before) ++delta.no_before;
        else {
            std::array<std::vector<const std::vector<size_t> *>, 2> fresh;
            for (size_t v = 0; v < 2; ++v)
                for (const auto &g : groups[v]) {
                    if (is_foreign(v, g)) ++delta.foreign;
                    else fresh[v].push_back(&g);
                }
            std::vector<std::pair<Rigid, OpticalFit>> found;
            bool complete = true;  // a combination skipped for time could be a second constellation: no decision
            auto consider = [&](const std::array<std::vector<double>, 2> &xy) {
                if (std::chrono::steady_clock::now() >= deadline) { complete = false; return; }
                const auto gyro = tracking_recovery_enabled() ? episode->gyro->predict(stamp) : std::optional<Rigid>{};
                OpticalComputeBudget budget; budget.end = std::min(budget.end, deadline);
                const auto fit = solve_subset(api_, episode->model, *pair, xy, gyro ? gyro->data() : nullptr,
                    gyro ? double(stamp-episode->stamp)*1e-9 : 0,
                    gyro ? episode->gyro->consistency_bound_deg(stamp) : 180);
                if (!fit || !fit_bounds(*fit)) return;
                Rigid world; std::copy(std::begin(fit->pose), std::end(fit->pose), world.begin());
                if (gyro && !episode->gyro->consistent(world, stamp, gyro)) { ++delta.fit_drops; return; }
                if (primary_now && distance3(world, *primary_now) <= p_.primary_clear_m) { ++delta.primary_overlap; return; }
                for (const auto &t : tracks)
                    if (&t != episode && claims_blobs(t.state) && t.world && distance3(world, *t.world) <= p_.primary_clear_m) {
                        ++delta.primary_overlap;
                        return;
                    }
                for (const auto &f : found) if (distance3(f.first, world) <= p_.distinct_m) return;  // same constellation
                found.push_back({world, *fit});
            };
            for (const auto *a : fresh[0])
                for (const auto *b : fresh[1])
                    if (a->size() >= 4 || b->size() >= 4) consider({xy_of(mask->blobs[0], *a), xy_of(mask->blobs[1], *b)});
            if (found.empty())
                for (size_t v = 0; v < 2; ++v)
                    for (const auto *g : fresh[v])
                        if (g->size() >= 4) {
                            std::array<std::vector<double>, 2> xy;
                            xy[v] = xy_of(mask->blobs[v], *g);
                            consider(xy);
                        }
            if (found.size() > 1) ++delta.ambiguous;
            else if (!complete) ++delta.incomplete;
            else if (found.size() == 1 && episode->state == MultiChipState::Acquiring) {
                episode->state = MultiChipState::Provisional;
                episode->last.reset();
                ++delta.provisional;
                accept(*episode, found[0].first, found[0].second);
            } else if (found.size() == 1 && episode->world && distance3(found[0].first, *episode->world) <= p_.confirm_m) {
                episode->state = MultiChipState::Tracking;
                episode->last.reset();
                ++delta.bound;
                ++delta.fits;
                accept(*episode, found[0].first, found[0].second);
                newly_bound.push_back(episode->chip);
            } else if (found.size() == 1) ++delta.foreign;  // back somewhere else: not the provisional constellation
        }
    }
    std::lock_guard<std::mutex> lock(mutex_);
    counters_.contested += contested;
    counters_.fits += delta.fits; counters_.published += delta.published; counters_.fit_drops += delta.fit_drops;
    counters_.jump_drops += delta.jump_drops; counters_.provisional += delta.provisional; counters_.bound += delta.bound;
    counters_.ambiguous += delta.ambiguous; counters_.foreign += delta.foreign; counters_.no_before += delta.no_before;
    counters_.primary_overlap += delta.primary_overlap; counters_.incomplete += delta.incomplete;
    seen_.push_back({stamp, unclaimed_clusters});
    while (!seen_.empty() && seen_.back().stamp - seen_.front().stamp > 3000000000LL) seen_.pop_front();
    // Commit only tracks whose radio state did not change while this pair was solved.
    for (const auto &t : tracks) {
        auto *live = find(t.chip);
        if (!live || live->epoch != t.epoch) continue;
        if (live->state != t.state)
            log_multi_optics("chip " + hex(t.chip) + " " + multi_chip_state_name(live->state) + " -> " +
                             multi_chip_state_name(t.state));
        const auto epoch = live->epoch;
        *live = t;
        if (live->world && live->stamp == stamp) live->gyro->accepted(*live->world, stamp);
        live->epoch = epoch;
    }
    for (auto chip : newly_bound) {
        bound_.push_back(chip);
        log_multi_optics("chip " + hex(chip) + " bound: appeared after ON, vanished after OFF, back after ON");
    }
    for (auto &f : frames) frames_.push_back(f);
    mask_.reset();
}
} // namespace pico
