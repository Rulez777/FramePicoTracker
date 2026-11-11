#pragma once
#include "device_config.hpp"
#include "native_identity.hpp"
#include "optical_timing.hpp"
#include "query_peer.hpp"
#include <deque>
#include <fstream>
namespace pico {
// Radio-thread policy. Only successfully published timed attempts can credit
// phase verification; bootstrap command10 and an unrelated tag cannot do so.
class LivePhaseRouting {
    OpticalExposureSchedule& schedule_;
    OpticalPhaseVerification& phase_;
    RoyClock& clock_;
    int ticks_;
    bool windows_;
    std::optional<ExposurePlan> plan_;
    struct Attempt {
        uint16_t tag;
        uint8_t command;
        ExposurePlan plan;
    };
    std::deque<Attempt> attempts_;
  public:
    LivePhaseRouting(OpticalExposureSchedule& schedule,OpticalPhaseVerification& phase,
                     RoyClock& clock,int ticks,bool windows=true)
        : schedule_(schedule),phase_(phase),clock_(clock),ticks_(ticks),windows_(windows) {}
    void reset() {
        schedule_.reset_peer(0);
        plan_.reset();
        attempts_.clear();
    }
    void poll(QueryPeer& peer,int64_t now) {
        if (!peer.diagnostics_ready() || peer.native25_exclusive || peer.queued.size() ||
            (peer.pending && peer.pending->command!=11) ||
            (peer.state!=PeerState::WaitSync && peer.state!=PeerState::Streaming)) return;
        std::optional<ExposurePlan> next;
        try { next=schedule_.prepare(0,1,now,0,0); }
        catch (const std::exception&) {
            log_optical_drop("Phase routing exposure plan unavailable; retrying next tick");
            return;
        }
        if (!next) return;
        plan_=next;
        peer.queue_exposure_sync(next->ticks,1,true,next->light_due);
    }
    template<class Publish>
    void mailbox(QueryPeer& peer,int64_t now,Publish publish) {
        auto pending=peer.pending;
        if (pending && plan_ && (pending->command==10 || pending->command==14)) {
            const auto target=pending->command==10?plan_->phase_target:plan_->light_target;
            if (windows_ && now>target+12000000) {
                std::optional<ExposurePlan> next;
                try { next=schedule_.prepare(0,1,now,0,0); }
                catch (const std::exception&) {
                    log_optical_drop("Phase routing retime plan unavailable; keeping current plan");
                }
                if (next) {
                    plan_=next;
                    peer.retime();
                    pending=peer.pending;
                }
            }
        }
        const auto box=peer.mailbox();
        std::optional<PhaseReference> reference;
        std::optional<CycleWindow> window;
        if (pending && plan_) {
            if (pending->command==10) {
                reference=PhaseReference{plan_->reference,uint32_t(plan_->ticks)};
                window=CycleWindow{plan_->phase_window[0],plan_->phase_window[1]};
            } else if (pending->command==14) {
                window=CycleWindow{plan_->light_window[0],plan_->light_window[1]};
            }
        }
        const bool timed=bool(reference) || bool(window);
        if (!windows_) window.reset();
        if (!publish(box,reference,window) || !pending || !plan_ || !timed) return;
        attempts_.push_back({uint16_t(box.generation),pending->command,*plan_});
        while (attempts_.size()>48) attempts_.pop_front();
    }
    void acknowledge(std::optional<uint8_t> command,uint32_t cycles,
                     std::optional<uint16_t> generation,int64_t now) {
        if (!command || !generation) return;
        for (const auto& attempt:attempts_) {
            if (attempt.tag!=*generation || attempt.command!=*command) continue;
            if (*command==14) schedule_.light_ack(attempt.plan.key,attempt.plan.count);
            if (*command==10) {
                schedule_.phase_ack(attempt.plan.key,now);
                // The converted stamp is unused; a missing Roy fit is logged and the
                // tag/command-matched ACK still credits phase.
                try { (void)clock_.convert(cycles,now); }
                catch (const std::exception&) {
                    log_optical_drop("Phase ACK without Roy clock conversion; crediting matched ACK");
                }
                // Native25 supplies causal identity independently; phase ACK
                // never grants LED or optical identity credit.
                phase_.ack(pico::devices::chip(),ticks_);
            }
            return;
        }
    }
};
}

#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_phase_routing, "phase-routing");
