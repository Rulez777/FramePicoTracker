#include "guard_evidence.hpp"
#include "native25.hpp"
#include <algorithm>
#include <limits>
#include <fstream>
#include <iostream>
namespace pico {
namespace {
[[noreturn]] void reject(const char *s) { throw std::runtime_error(s); }
// Owner rule 2026-10-29: LED episode problems are logged; the episode is restored (ON) and the run goes on.
void soft(const std::string &s) {
    std::cerr << "native25_soft: " << s << std::endl;
    std::ofstream("/tmp/pico-diag.log", std::ios::app) << "native25_soft: " << s << std::endl;
}
bool body_is(const Bytes &b, uint8_t v) { return b == Bytes{v}; }
bool fresh_mode(PeerEngine &e) { return e.mode_profile == "frame" && e.mode_verified(); }
// Bound source times away from int64 overflow in all temporal predicates below.
// This exceeds any admitted session and rejects corrupt input, never broadens it.
bool time_ok(int64_t t) { return t > 0 && t <= std::numeric_limits<int64_t>::max() / 4; }
}
LedModeEpisodes::LedModeEpisodes(uint32_t chip) : chip_(chip) {
    if (!chip) reject("Exact enrolled chip required");
}
void LedModeEpisodes::begin(uint32_t chip, bool mode, bool controllers, bool geometry) {
    if (chip != chip_ || !mode || !controllers || !geometry)
        reject("Fresh target/mode/controller/optical admission required");
    // No episode cap and no failure latch: the tracker can be re-identified any number of times.
    if (active) reject("Episode not admitted");
    active = LedEpisode{};
    active->token = static_cast<unsigned>(episodes.size() + 1);
    active->chip = chip_;
    restore_required = true;
}
void LedModeEpisodes::published(const Bytes &b, uint32_t g, int64_t t) {
    if (!active || (!body_is(b, 1) && !body_is(b, 2))) reject("Unknown native25 command");
    if ((g & 1) || !(g & 65535)) reject("Invalid protected publication tag");
    if (!time_ok(t)) reject("Source clock timestamp required");
    auto &e = *active;
    if (body_is(b, 1)) {
        if (e.on_published) reject("Cannot re-arm OFF after restoration publication");
        e.off_tags.insert(static_cast<uint16_t>(g));
        if (!e.off_published) e.off_published = t;
    } else {
        e.on_tags.insert(static_cast<uint16_t>(g));
        if (!e.on_published) e.on_published = t;
    }
}
bool LedModeEpisodes::can_ack(uint32_t chip, const Bytes &b, uint32_t g, int64_t t,
                              bool crc, int64_t error) const {
    // Attribution is the exact publication tag; clock uncertainty only widens the time window, never rejects.
    if (!active || chip != chip_ || !crc || !time_ok(t) || g == 0 || g > 65535) return false;
    const auto &e = *active;
    const auto pub = body_is(b, 1) ? e.off_published : e.on_published;
    const auto &tags = body_is(b, 1) ? e.off_tags : e.on_tags;
    return (body_is(b, 1) || body_is(b, 2)) && tags.count(static_cast<uint16_t>(g)) &&
           pub && t + std::max<int64_t>(error, 0) >= *pub;
}
bool LedModeEpisodes::acknowledge(uint32_t chip, const Bytes &b, uint32_t g, int64_t t,
                                  bool crc, int64_t error) {
    if (!can_ack(chip, b, g, t, crc, error)) return false;
    auto &e = *active;
    auto &ack = body_is(b, 1) ? e.off_ack : e.on_ack;
    if (!ack) {
        ack = t;
        (body_is(b, 1) ? e.off_error : e.on_error) = error;
    }
    return true;
}
bool LedModeEpisodes::restoration_due(int64_t now) const {
    return active && active->off_published && now >= *active->off_published &&
           now - *active->off_published >= hold_ns;
}
void LedModeEpisodes::deadline(int64_t now) {
    if (active && active->off_published && now > *active->off_published &&
        now - *active->off_published > maximum_off_ns && !failed) {
        failed = true;  // overdue: tick() sends ON now; logged once per episode
        soft("OFF exceeded 800 ms; restoring LEDs");
    }
}
void LedModeEpisodes::restored_application(bool mode, bool measured) {
    pico::guard_evidence::Scope guard_evidence_scope(15,"led-restore");
    if (!active) return;
    if (!measured) return;  // keep waiting for the lit return
    if (!active->on_ack || !mode)
        soft("Optical lit return without ON ACK/fresh mode; accepting the visible LEDs");
    episodes.push_back(*active);
    active.reset();
    restore_required = false;
    failed = false;
}
NativeLedRadioOwner::NativeLedRadioOwner(uint32_t chip, std::string link)
    : chip_(chip), link_(std::move(link)), ledger_(chip) {
    if (link_.empty()) reject("Actual live-link token required");
}
void NativeLedRadioOwner::engine(PeerEngine &e, const std::string &link) {
    if (link != link_ || e.chip != le32(chip_) || e.slot != 0)
        soft("Target/link differs from the enrolled episode link; continuing");
}
bool NativeLedRadioOwner::begin(PeerEngine &e, const std::string &link, int64_t now,
                                bool controllers, bool geometry) {
    engine(e, link);
    if (!time_ok(now)) reject("Actual monotonic timestamp required");
    // Not ready yet is not an error: the identity session simply tries again on a later tick.
    if (e.pending || !e.queued.empty()) return false;
    if (!e.last_reply || !(0 <= e.now() - *e.last_reply && e.now() - *e.last_reply < .25)) return false;
    if (!fresh_mode(e)) return false;
    if (!controllers || !geometry) soft("Episode with controller/geometry admission unproven");
    {
        std::lock_guard<std::mutex> lock(evidence_);
        if (ledger_.active) return false;  // previous episode still restoring
        ledger_.begin(chip_, true, true, true);
    }
    e.native25_exclusive = true;
    try { e.begin(25, {1}); }
    catch (...) {
        abort_requested = true;
        failure = "OFF allocation failed; no identity credit";
        throw;
    }
    restore_started = false;
    return true;
}
void NativeLedRadioOwner::tick(PeerEngine &e, const std::string &link, int64_t now) {
    engine(e, link);
    std::lock_guard<std::mutex> lock(evidence_);
    if (!ledger_.active) return;
    ledger_.deadline(now);  // logs and marks the episode overdue; never throws
    // Hardware 2026-10-29: an episode whose identity window never confirmed stayed active forever, so no second
    // episode could start and identity never bound. LEDs are already back ON here, so close it and allow another.
    if (ledger_.active->on_ack && now > *ledger_.active->on_ack && now - *ledger_.active->on_ack > 2000000000LL) {
        soft("Episode not confirmed 2 s after ON ACK; closing it so a new episode can start");
        ledger_.active.reset();
        ledger_.restore_required = false;
        ledger_.failed = false;
        e.native25_exclusive = false;
        return;
    }
    const bool overdue = ledger_.failed;
    if ((ledger_.restoration_due(now) || overdue) && !ledger_.active->on_published) {
        // ON goes out once the OFF is acknowledged, or unconditionally once the episode is overdue,
        // so the LEDs are never left off.
        // Hardware 2026-10-29: when the OFF's ACK is never attributed, the OFF stays pending forever and the ON
        // never goes out, leaving the LEDs dark for the rest of the session. Once overdue, drop that OFF.
        if (overdue && !restore_started && e.pending && e.pending->command == 25 && body_is(e.pending->body, 1)) {
            soft("OFF still pending after 800 ms; replacing it with ON");
            e.pending.reset();
        }
        if (!e.pending && (e.queued.empty() || overdue) && !restore_started && (ledger_.active->off_ack || overdue)) {
            e.begin(25, {2});
            restore_started = true;
            relight = true;
            soft(std::string("ON sent; off_ack=") + (ledger_.active->off_ack ? "yes" : "no") + " overdue=" + (overdue ? "yes" : "no"));
        } else if (e.pending && (e.pending->command != 25 ||
                   (!body_is(e.pending->body, 1) && !body_is(e.pending->body, 2)))) {
            soft("Another command is pending inside the LED episode; ON follows when it retires");
        }
    }
}
void NativeLedRadioOwner::published(PeerEngine &e, uint32_t g, const Bytes &b, int64_t t) {
    std::lock_guard<std::mutex> lock(evidence_);
    if (!ledger_.active) return;
    if (!e.pending || e.pending->command != 25 || e.pending->body != b ||
        std::find(e.pending->attempts.begin(), e.pending->attempts.end(), g) == e.pending->attempts.end()) {
        soft("Publication is not the allocated pending native25 command; ignored");
        return;
    }
    ledger_.published(b, g, t);
}
void NativeLedRadioOwner::optical_return(PeerEngine &e, const std::string &link, bool measured) {
    engine(e, link);
    std::lock_guard<std::mutex> lock(evidence_);
    ledger_.restored_application(fresh_mode(e), measured);
    e.native25_exclusive = false;
}
std::optional<LedEpisode> NativeLedRadioOwner::snapshot() const {
    std::lock_guard<std::mutex> lock(evidence_);
    return ledger_.active;
}
bool NativeLedRadioOwner::restore_required() const {
    std::lock_guard<std::mutex> lock(evidence_);
    return ledger_.restore_required;
}
size_t NativeLedRadioOwner::completed() const {
    std::lock_guard<std::mutex> lock(evidence_);
    return ledger_.episodes.size();
}
Native25AckBridge::Native25AckBridge(NativeLedRadioOwner &owner, PeerEngine &e, RoyClock &clock,
    const std::string &link, std::function<int64_t()> now)
    : owner_(owner), engine_(e), clock_(clock), now_(std::move(now)) {
    owner_.engine(e, link);
    if (e.native25_ack_bridge || !now_) reject("Bridge already installed or missing clock");
    e.native25_ack_bridge = [this](uint16_t tag, const std::function<bool()> &previous) {
        return acknowledge(tag, previous);
    };
}
Native25AckBridge::~Native25AckBridge() {
    if (owner_.restore_required()) {
        owner_.abort_requested = true;
        owner_.failure = "ACK bridge removed with restoration unresolved";
        // Never leave a live OFF eligible for the unattributed MCU shortcut.
        // No captured object survives bridge destruction.
        engine_.native25_ack_bridge = [](uint16_t, const std::function<bool()> &) { return false; };
    } else engine_.native25_ack_bridge = {};
}
bool Native25AckBridge::acknowledge(uint16_t tag, const std::function<bool()> &previous) {
    const auto episode = owner_.snapshot();
    if (!episode || !engine_.pending || engine_.pending->command != 25) return previous();
    const Bytes body = engine_.pending->body;
    if (!context_ || context_->generation != tag) return false;
    {
        std::lock_guard<std::mutex> lock(owner_.evidence_);
        if (!owner_.ledger_.can_ack(owner_.chip_, body, tag, context_->source_ns, true,
                                    context_->uncertainty_ns)) return false;
    }
    if (engine_.native25_ack_retiring) reject("Nested native25 ACK retirement");
    engine_.native25_ack_retiring = true;
    bool accepted;
    try { accepted = previous(); }
    catch (...) { engine_.native25_ack_retiring = false; throw; }
    engine_.native25_ack_retiring = false;
    if (accepted) {
        std::lock_guard<std::mutex> lock(owner_.evidence_);
        if (!owner_.ledger_.acknowledge(owner_.chip_, body, tag, context_->source_ns, true,
                                        context_->uncertainty_ns))
            reject("Canonical native25 ACK retired without episode attribution");
    }
    return accepted;
}
void Native25AckBridge::record(const Record &r, const std::function<void()> &previous) {
    if (recording_) reject("Nested canonical record ownership");
    std::optional<AckContext> context;
    if (r.hardware_crc && !r.partial && r.channel == 2 && r.profile == 0 &&
        r.chip == owner_.chip_ && r.dma.size() >= 3 && r.dma.size() == size_t(r.dma[0]) + 2 &&
        (r.dma[1] == 2 || r.dma[1] == 3) && r.generation > 0 && r.generation < 65536) {
        try {
            decode_records(Bytes(r.dma.begin() + 2, r.dma.end()));
            context = ack_context(r, clock_, now_());
        } catch (const std::exception &) {} // invalid attribution, retain diagnostics
    }
    recording_ = true;
    context_ = context;
    try { previous(); }
    catch (...) { context_.reset(); recording_ = false; throw; }
    context_.reset();
    recording_ = false;
}
bool Native25AckBridge::publish(MailboxWriter &writer, int slot, const Mailbox &m,
    std::optional<PhaseReference> phase, std::optional<CycleWindow> window) {
    const auto body = engine_.pending && engine_.pending->command == 25
                          ? engine_.pending->body : Bytes{};
    if (slot == 0 && owner_.snapshot() && !body.empty()) {
        const auto commands = decode_records(m.payload);
        if (!engine_.pending || commands.size() != 1 || commands[0].command != 25 ||
            commands[0].body != body || commands[0].tag != engine_.pending->sequence ||
            std::find(engine_.pending->attempts.begin(), engine_.pending->attempts.end(),
                      m.generation) == engine_.pending->attempts.end())
            reject("Mailbox is not the actual pending native25 command");
    }
    const auto stamp = now_();
    const bool changed = writer.publish(slot, m.generation, m.s1, m.payload, phase, window);
    if (changed && slot == 0 && (body_is(body, 1) || body_is(body, 2)) && owner_.snapshot())
        owner_.published(engine_, m.generation, body, stamp);
    return changed;
}
std::optional<IdentityWindow> identity_window(const LedEpisode &e, uint32_t chip,
    const std::vector<IdentityFrameTime> &frames, int64_t p) {
    if (!chip || e.chip != chip || !e.token || !e.off_ack || !e.on_ack ||
        !e.off_published || !e.on_published || e.off_tags.empty() || e.on_tags.empty() ||
        // No upper bound on ACK/frame uncertainty (owner rule 2026-10-29): it only widens the windows below.
        p != 33333333 || e.off_error < 0 || e.on_error < 0 || !time_ok(*e.off_ack) ||
        !time_ok(*e.on_ack) || !time_ok(*e.off_published) || !time_ok(*e.on_published)) return {};
    const auto duration = *e.on_ack - *e.off_ack;
    if (!(duration - e.off_error - e.on_error > 100000000 &&
          duration + e.off_error + e.on_error < 2000000000)) return {};  // was 500 ms; native radio adds latency (2026-10-29)
    IdentityWindow w;
    w.off_bound = std::min(*e.off_ack - e.off_error, *e.off_published);
    w.on_bound = std::min(*e.on_ack - e.on_error, *e.on_published);
    for (size_t i = 0; i < frames.size(); ++i) {
        const auto &f = frames[i];
        if (!time_ok(f.stamp) || f.index < 0 || f.error < 0 ||
            (i && f.stamp <= frames[i - 1].stamp)) return {};
        if (f.stamp + f.error < w.off_bound) w.before.push_back(i);
        if (f.stamp - f.error > *e.on_ack + e.on_error + p && w.after.size() < 3)
            w.after.push_back(i);
        if (*e.off_ack + e.off_error + 2 * p < f.stamp - f.error &&
            f.stamp + f.error < w.on_bound - p) w.dark.push_back(i);
    }
    if (w.before.size() < 3 || w.after.size() != 3 || w.dark.size() < 2) return {};
    w.before.erase(w.before.begin(), w.before.end() - 3);
    for (auto i : w.before) if (!frames[i].measured_world) return {};
    for (auto i : w.after) if (!frames[i].measured_world) return {};
    for (auto i : w.dark) if (frames[i].measured_world) return {};
    for (size_t i = w.before.front() + 1; i <= w.after.back(); ++i) {
        const auto &a = frames[i - 1];
        const auto &b = frames[i];
        const auto dt = b.stamp - a.stamp;
        // Integer products exactly express strict .8/.1.2 cadence boundaries.
        if (b.index <= a.index || b.index - a.index != 1 || dt > 2 * p ||
            !(5 * dt > 4 * p && 5 * dt < 6 * p))
            return {};
    }
    return w;
}
} // namespace pico

#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_native25, "native25");
