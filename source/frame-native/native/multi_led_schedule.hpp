#pragma once
// 2026-10-31 radio side of the optical identity for several trackers (multi_optics.hpp, docs/MULTI_TRACKER_RADIO_
// 2026-10-31.md "Optical identity for several trackers"). Radio thread, session lock held; installed only with
// multi-tracker optics, never on the single-tracker path.
//
// Per secondary tracker (LEDs dark from its configuration, 25 {1}):
//   Idle --(no other episode, first target and every bound tracker held)--> Sync: light sync (command 14 + 10 with the
//   module-timed phase reference, the same exposure plan as the first target, key = chip) --> On: 25 {2} -->
//   (ACK) lit(): window --> optics Provisional --> Off: 25 {1} --> (ACK) off() --> optics Darkened --> On: 25 {2}
//   --> (ACK) lit() --> optics Tracking: keep the light sync fresh. Any timeout, a window without a unique new
//   constellation, a constellation that did not follow OFF, or a lost track: 25 {1} and back to Idle with a back-off.
// A tracker is lit only by its own acknowledged command, so an unsynchronised tracker never flashes into the first
// target's exposures, and one chip at a time is in its episode.
#include "multi_optics.hpp"
#include "multi_peer_session.hpp"
#include "optical_timing.hpp"
#include <algorithm>
#include <deque>
#include <fstream>
#include <set>
#include <sstream>

namespace pico {
struct MultiLedParams {
    int64_t sync_timeout_ns = 2000000000LL;     // light sync (10 and 14 acknowledged)
    int64_t command_timeout_ns = 2000000000LL;  // one 25 command published and acknowledged
    int64_t retry_ns = 2000000000LL;            // back-off after a failed episode
    int64_t lost_retry_ns = 500000000LL;        // back-off after a lost track
};
class MultiLedSchedule {
  public:
    using Params = MultiLedParams;
    MultiLedSchedule(MultiChipOptics &optics, std::function<int64_t()> now, Params p = Params{})
        : optics_(optics), now_(std::move(now)), p_(p) {}
    MultiLedSchedule(const MultiLedSchedule &) = delete;
    MultiLedSchedule &operator=(const MultiLedSchedule &) = delete;
    // Exposure plan source of the first target's light sync; without one the episode lights unsynchronised (logged).
    void exposure_schedule(OpticalExposureSchedule *schedule) { schedule_ = schedule; }
    // Installs the mailbox/ACK hooks on every secondary peer with an optical model.
    void attach(SecondaryPeers &peers) {
        const auto chips = optics_.chips();
        for (size_t i = 0; i < peers.size(); ++i) {
            auto &sp = peers.at(i);
            if (std::find(chips.begin(), chips.end(), sp.entry.chip) == chips.end()) {
                log("peer " + std::to_string(sp.index) + " chip " + hex(sp.entry.chip) + " has no LED model: IMU only");
                continue;
            }
            auto entry = std::make_unique<Peer>();
            auto &peer = *entry;
            peer.p = &sp;
            sp.mailbox_hook = [this, &peer](int slot, PeerEngine &e, MailboxWriter &w) { mailbox(peer, slot, e, w); };
            sp.ack_hook = [this, &peer](std::optional<uint8_t> command, uint32_t, std::optional<uint16_t> tag) {
                acknowledge(peer, command, tag);
            };
            peers_.push_back(std::move(entry));
        }
    }
    void poll() {
        const auto now = now_();
        optics_.expire(now);
        for (auto chip : optics_.take_bound()) log("chip " + hex(chip) + " bound; publishing PTP2 with its chip");
        for (auto chip : optics_.take_lost())
            if (auto *peer = find(chip)) {
                const bool was_tracking = peer->step == Step::Tracking;
                darken(*peer, now, was_tracking ? p_.lost_retry_ns : p_.retry_ns, "optics closed the track/episode");
            }
        for (auto &entry : peers_) step(*entry, now);
    }
    uint64_t episodes_started() const { return started_; }
    uint64_t commands_acknowledged() const { return acked_; }
    uint64_t syncs_acknowledged() const { return synced_; }

  private:
    enum class Step { Wait, Idle, Sync, On, Off, Window, Tracking };
    struct Attempt { uint16_t tag; uint8_t command; ExposurePlan plan; };
    struct Peer {
        SecondaryPeer *p = nullptr;
        Step step = Step::Wait;
        std::optional<ExposurePlan> plan;
        std::deque<Attempt> attempts;
        Bytes led;                    // body of the 25 command in flight
        std::set<uint16_t> led_tags;  // its published mailbox generations
        int64_t led_published = 0, started = 0, retry_after = 0;
        bool sync_acked = false;
    };
    MultiChipOptics &optics_;
    std::function<int64_t()> now_;
    Params p_;
    OpticalExposureSchedule *schedule_ = nullptr;
    std::vector<std::unique_ptr<Peer>> peers_;
    uint64_t started_ = 0, acked_ = 0, synced_ = 0;

    static std::string hex(uint32_t chip) { std::ostringstream o; o << std::hex << chip; return o.str(); }
    static void log(const std::string &line) {
        std::ofstream("/tmp/pico-diag.log", std::ios::app) << "multi_led: " << line << std::endl;
    }
    Peer *find(uint32_t chip) {
        for (auto &p : peers_) if (p->p->entry.chip == chip) return p.get();
        return nullptr;
    }
    static bool idle(const PeerEngine &e) { return !e.pending && e.queued.empty(); }
    static bool enrolled(const SecondaryPeer &sp) {
        const auto &e = *sp.engine;
        return sp.plane->enrolled && (e.state == PeerState::WaitSync || e.state == PeerState::Streaming) && e.mode_verified();
    }
    void led(Peer &peer, uint8_t mode, int64_t now) {
        auto &e = *peer.p->engine;
        e.queued.erase(std::remove_if(e.queued.begin(), e.queued.end(), [](const auto &c) { return c.first == 25; }),
                       e.queued.end());
        e.queued.push_back({uint8_t(25), Bytes{mode}});
        peer.led = Bytes{mode};
        peer.led_tags.clear();
        peer.led_published = 0;
        peer.started = now;
        e.advance();
    }
    void darken(Peer &peer, int64_t now, int64_t backoff, const std::string &why) {
        optics_.dark(peer.p->entry.chip);
        if (enrolled(*peer.p)) led(peer, 1, now);
        peer.step = Step::Idle;
        peer.retry_after = now + backoff;
        peer.plan.reset();
        log("chip " + hex(peer.p->entry.chip) + " dark (25 {1}): " + why);
    }
    bool sync(Peer &peer, int64_t now) {
        if (!schedule_) return false;
        std::optional<ExposurePlan> next;
        try { next = schedule_->prepare(0, 1, now, 0, peer.p->entry.chip); }
        catch (const std::exception &) { log_optical_drop("Secondary light sync plan unavailable; retrying next tick"); return false; }
        if (!next) return false;
        peer.plan = next;
        peer.p->engine->queue_exposure_sync(next->ticks, 1, true, next->light_due);
        return true;
    }
    void step(Peer &peer, int64_t now) {
        auto &sp = *peer.p;
        auto &e = *sp.engine;
        const auto chip = sp.entry.chip;
        if (!enrolled(sp)) {
            if (peer.step != Step::Wait) {
                optics_.dark(chip);
                log("chip " + hex(chip) + " link/enrollment lost; optics closed, waiting");
            }
            peer.step = Step::Wait;
            return;
        }
        auto timed_out = [&](int64_t limit) { return now - peer.started > limit; };
        switch (peer.step) {
        case Step::Wait:
            peer.step = Step::Idle;
            peer.retry_after = now;
            log("chip " + hex(chip) + " enrolled; LEDs dark until its identity episode");
            break;
        case Step::Idle: {
            if (now < peer.retry_after || !idle(e) || optics_.acquiring() || !optics_.all_bound_tracked(now)) break;
            for (const auto &o : peers_)
                if (o.get() != &peer && o->step != Step::Idle && o->step != Step::Wait && o->step != Step::Tracking) return;
            ++started_;
            peer.started = now;
            peer.sync_acked = false;
            if (sync(peer, now)) { peer.step = Step::Sync; log("chip " + hex(chip) + " episode: light sync"); }
            else {
                log("chip " + hex(chip) + " episode: no exposure plan; lighting without light sync");
                led(peer, 2, now);
                peer.step = Step::On;
            }
            break;
        }
        case Step::Sync:
            if (peer.sync_acked) { led(peer, 2, now); peer.step = Step::On; log("chip " + hex(chip) + " synced; ON"); }
            else if (timed_out(p_.sync_timeout_ns)) darken(peer, now, p_.retry_ns, "light sync not acknowledged");
            break;
        case Step::On:
        case Step::Off:
            if (timed_out(p_.command_timeout_ns)) darken(peer, now, p_.retry_ns, "LED command not acknowledged");
            break;
        case Step::Window: {
            const auto s = optics_.state(chip);
            if (s == MultiChipState::Provisional) { led(peer, 1, now); peer.step = Step::Off; log("chip " + hex(chip) + " provisional; OFF"); }
            else if (s == MultiChipState::Darkened) { led(peer, 2, now); peer.step = Step::On; log("chip " + hex(chip) + " darkened; ON"); }
            else if (s == MultiChipState::Tracking) { peer.step = Step::Tracking; peer.retry_after = now; }
            else if (s == MultiChipState::Dark) darken(peer, now, p_.retry_ns, "episode closed");
            break;
        }
        case Step::Tracking:
            // Keep the light sync fresh (the plan only returns when due, as for the first target).
            if (idle(e) || (e.pending && e.pending->command == 11 && e.queued.empty())) (void)sync(peer, now);
            break;
        }
    }
    void mailbox(Peer &peer, int slot, PeerEngine &e, MailboxWriter &w) {
        const auto pending = e.pending;
        const auto box = e.mailbox();
        std::optional<PhaseReference> reference;
        if (pending && pending->command == 10 && peer.plan)
            reference = PhaseReference{peer.plan->reference, uint32_t(peer.plan->ticks)};
        // No cycle window: the multi4 module admits only the phase reference (bit 16), roy_ram_station_v4_multi4.c:146.
        if (!w.publish(slot, box.generation, box.s1, box.payload, reference) || !pending) return;
        if (pending->command == 25 && pending->body == peer.led) {
            peer.led_tags.insert(uint16_t(box.generation));
            if (!peer.led_published) peer.led_published = now_();
        }
        if (peer.plan && ((pending->command == 10 && reference) || pending->command == 14)) {
            peer.attempts.push_back({uint16_t(box.generation), pending->command, *peer.plan});
            while (peer.attempts.size() > 48) peer.attempts.pop_front();
        }
    }
    void acknowledge(Peer &peer, std::optional<uint8_t> command, std::optional<uint16_t> tag) {
        if (!command || !tag) return;
        const auto now = now_();
        const auto chip = peer.p->entry.chip;
        if (*command == 25 && peer.led_tags.count(*tag)) {
            ++acked_;
            peer.led_tags.clear();
            if (peer.led == Bytes{2} && peer.step == Step::On) {
                optics_.lit(chip, peer.led_published, now);
                peer.step = Step::Window;
            } else if (peer.led == Bytes{1} && peer.step == Step::Off) {
                optics_.off(chip, peer.led_published, now);
                peer.step = Step::Window;
            }
            return;
        }
        if (!schedule_) return;
        for (const auto &a : peer.attempts) {
            if (a.tag != *tag || a.command != *command) continue;
            if (*command == 14) schedule_->light_ack(chip, a.plan.count);
            if (*command == 10) { schedule_->phase_ack(chip, now); peer.sync_acked = true; ++synced_; }
            return;
        }
    }
};
} // namespace pico
