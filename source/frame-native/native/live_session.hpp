#pragma once
#include "phase5/tracking_guard.hpp"
#include "optical_timing.hpp"
#include "head_history.hpp"
#include "session_ownership.hpp"
#include "image_admission.hpp"
#include <functional>
#include <exception>
#include <iostream>
#include <stdexcept>

namespace pico {
inline void log_live_runtime_error(const std::exception& error) {
    const std::string why=error.what();
    if(why=="Live parent lost or cancellation requested" ||
       why=="Replay session wall bound reached" || why=="Live session 180s wall bound reached")
        throw std::runtime_error(why);
    std::cerr<<why<<"; dropping iteration, continuing session\n";
}
// Explicit offline-only clock policy; live construction has no adjustable bound.
struct ReplaySessionLimit { int seconds; };
// Runtime observations are reported without latching a session failure.
// This object never kills or releases an SRAM owner.
struct LiveSessionHooks {
    std::function<int64_t()> now;
    // Drain bounded observer input and ingest every row (never latest-only).
    std::function<void(phase5::BenchTrackingGuard&)> observer_poll;
    std::function<bool()> observer_alive;
    std::function<void()> worker_health;
    std::function<bool()> diagnostics_ready;
    std::function<void()> camera_poll;
    std::function<void(bool)> roy_poll;
    std::function<void()> publication_poll;
    std::function<bool()> parent_alive;
    std::function<bool()> cancelled;
    // Required adapters: immutable reviewed invocation revalidation and complete
    // same-owner OS evidence. Synthetic adapters never admit production live.
    std::function<void()> sealed_invocation;
    std::function<SessionOwnership()> ownership;
    std::function<void()> listener_attribution;
    std::function<void()> profile_exclusion;
};
class LiveSessionSafety {
    LiveSessionHooks h_;
    HeadHistory heads_{4096};
    phase5::BenchTrackingGuard tracking_;
    int64_t started_=0,last_now_=0;
    void lifetime();
    const OpticalPhaseVerification& phase_;
    bool admitted_=false;
    int64_t last_provenance_ns_=0;  // rate limit of the ownership/process scan (evidence only)
    int64_t wall_bound_=180000000000LL;
    bool replay_=false;
    SessionOwnership owner_;
    std::string required_lock_;
    SramOwner* sram_=nullptr;
    bool sram_entered_=false,sram_restoring_=false,sram_closed_=false;
    void provenance(bool seals);
    void tracking(bool require_now);
    void latch(const std::function<void()>&);
public:
    LiveSessionSafety(LiveSessionHooks,const OpticalPhaseVerification&,
        std::string required_lock="/home/steamos/.local/state/frame-pico/roy.lock");
    LiveSessionSafety(LiveSessionHooks,const OpticalPhaseVerification&,ReplaySessionLimit);
    Rigid head_pose(int64_t exposure); // same supervised rows, no extrapolation
      std::optional<Rigid> camera_head(int64_t exposure); // missing bracket drops this direct frame
      // Direct optics prepares observer input before solving. These cached
      // lookups perform no provenance/observer I/O; caller holds owner shared_.
      void camera_tracking();
      std::optional<Rigid> camera_head_cached(int64_t exposure);
      Rigid camera_origin_cached(int64_t exposure);
    Rigid driver_origin(int64_t exposure);
    LiveSessionSafety(const LiveSessionSafety&)=delete;
    LiveSessionSafety& operator=(const LiveSessionSafety&)=delete;
    LiveSessionSafety(LiveSessionSafety&&)=delete;
    LiveSessionSafety& operator=(LiveSessionSafety&&)=delete;
    void admit(); // before RF/SRAM entry; profile owner has its own admission
    bool prime_tracking(); // same guard/history, before admit only
    void before_sram(); // recheck seals/owner/listener + strict bench immediately before mutation
    void install_sram(SramOwner&,const Bytes& stock,const Bytes& elf,const Bytes& state);
    void enter_sram();
    // Recovery deliberately bypasses a failed tracking/profile latch. Never
    // release the owner until this returns all four literal-true barriers.
    CleanupBarriers restore_sram();
    void guard(); // before every radio iteration and identity radio tick
    void fresh_tracking(); // QueryHooks::fresh_guard, including command19
    void poll(); // RadioLoop::poll, after snapshot and before record dispatch
};
}
