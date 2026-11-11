#include "guard_evidence.hpp"
#include <time.h>
#include <chrono>
#include "live_session.hpp"
#include <stdexcept>
#include <utility>
#include <iostream>
namespace pico {
LiveSessionSafety::LiveSessionSafety(LiveSessionHooks h,const OpticalPhaseVerification& p,std::string required_lock):h_(std::move(h)),tracking_([this](const phase5::Value& row){heads_.observe(row);}),phase_(p),required_lock_(std::move(required_lock)){
    if(!h_.now||!h_.observer_poll||!h_.observer_alive||!h_.worker_health||!h_.diagnostics_ready||!h_.camera_poll||!h_.roy_poll||!h_.publication_poll||!h_.parent_alive||!h_.cancelled||!h_.sealed_invocation||!h_.ownership||!h_.listener_attribution||!h_.profile_exclusion)
        throw std::invalid_argument("All live session safety dependencies required");
}
LiveSessionSafety::LiveSessionSafety(LiveSessionHooks h,const OpticalPhaseVerification& p,ReplaySessionLimit limit)
    :LiveSessionSafety(std::move(h),p){
    if(limit.seconds<1||limit.seconds>900)throw std::invalid_argument("Replay duration must be 1..900 seconds");
    replay_=true;wall_bound_=int64_t(limit.seconds)*1000000000LL;
}
void LiveSessionSafety::provenance(bool seals){
    // Owner rule 2026-10-29: the ownership/process scan walks all of /proc and took ~30% CPU and most of each
    // radio iteration when it ran on every guard() call. It is evidence only, so refresh it at most every 5 s.
    if(!seals&&admitted_){
#ifdef _WIN32
        const int64_t now=std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
#else
        timespec t{};clock_gettime(CLOCK_MONOTONIC,&t);
        const int64_t now=int64_t(t.tv_sec)*1000000000+t.tv_nsec;
#endif
        if(now-last_provenance_ns_<5000000000LL)return;
        last_provenance_ns_=now;
    }
    if(seals)latch([&]{h_.sealed_invocation();});
    latch([&]{
        auto current=h_.ownership();
        latch([&]{verify_session_ownership(current,required_lock_);});
        if(admitted_&&!same_session_owner(owner_,current))std::cerr<<"Live Roy owner identity changed; continuing with current evidence\n";
        owner_=std::move(current);
    });
    latch([&]{pico::guard_evidence::exercise(3,"listener-child",[&]{h_.listener_attribution();});});
    latch([&]{h_.profile_exclusion();});
}
void LiveSessionSafety::latch(const std::function<void()>& f){
    try{f();}catch(const std::exception& e){
        const std::string why=e.what();
        if(why=="Live parent lost or cancellation requested" ||
           why=="Replay session wall bound reached" || why=="Live session 180s wall bound reached" ||
           why=="Live session not admitted" || why=="Live session already admitted" ||
           why=="SRAM restoration started; dispatch forbidden" ||
           why=="SRAM session transaction already consumed" || why=="SRAM session not ready for entry")throw;
        std::cerr<<why<<"; dropping iteration, continuing session\n";
    }
}
void LiveSessionSafety::lifetime(){
    pico::guard_evidence::Scope guard_evidence_scope(27,"parent-wall");
    if(sram_restoring_||sram_closed_)throw std::runtime_error("SRAM restoration started; dispatch forbidden");
    const auto now=h_.now();
    if(!h_.parent_alive()||h_.cancelled())throw std::runtime_error("Live parent lost or cancellation requested");
    if(now<0||now<last_now_){std::cerr<<"Live session clock moved backwards; keeping last clock\n";return;}
    if(admitted_&&now-started_>=wall_bound_)throw std::runtime_error(replay_?"Replay session wall bound reached":"Live session 180s wall bound reached");
    last_now_=now;
}
Rigid LiveSessionSafety::head_pose(int64_t exposure){
    latch([&]{if(!admitted_)throw std::runtime_error("Live session not admitted");
    lifetime();provenance(false);tracking(true);});
    // The exposure caller drops an unavailable bracket. Never invent a pose.
    return heads_.head_pose(exposure);
}
std::optional<Rigid> LiveSessionSafety::camera_head(int64_t exposure){
    // Keep session health checks separate from an unavailable exposure bracket.
    latch([&]{if(!admitted_)throw std::runtime_error("Live session not admitted");
        lifetime();provenance(false);tracking(true);});
    return camera_head_cached(exposure);
}
void LiveSessionSafety::camera_tracking(){
    if(!admitted_)throw std::runtime_error("Live session not admitted");
    lifetime();tracking(true);
}
Rigid LiveSessionSafety::camera_origin_cached(int64_t exposure){
    return heads_.raw_from_standing(exposure);
}
std::optional<Rigid> LiveSessionSafety::camera_head_cached(int64_t exposure){
    try{(void)heads_.raw_from_standing(exposure);return heads_.head_pose(exposure);}
    catch(const std::runtime_error& e){std::cerr<<e.what()<<"; dropping camera sample\n";return {};}
}
void LiveSessionSafety::tracking(bool require_now){
    h_.observer_poll(tracking_);
    (void)tracking_.check(h_.now(),h_.observer_alive(),require_now);
}
Rigid LiveSessionSafety::driver_origin(int64_t exposure){
    latch([&]{if(!admitted_)throw std::runtime_error("Live session not admitted");
        lifetime();provenance(false);tracking(true);});
    return heads_.raw_from_standing(exposure);
}
void LiveSessionSafety::admit(){
    if(admitted_)throw std::runtime_error("Live session already admitted");
    lifetime();
    latch([&]{provenance(true);});
    latch([&]{tracking(true);});
    latch([&]{h_.worker_health();});
    lifetime();started_=last_now_;admitted_=true;
}
bool LiveSessionSafety::prime_tracking(){
    if(admitted_)throw std::runtime_error("Initial tracking already consumed");
    bool ready=false;
    latch([&]{
        lifetime();
        h_.observer_poll(tracking_);
        if(!h_.observer_alive())throw std::runtime_error("Initial observer terminated");
        if(tracking_.has_sample())ready=tracking_.check(h_.now(),true,true);
    });
    return ready;
}
void LiveSessionSafety::before_sram(){latch([&]{
    if(!admitted_)throw std::runtime_error("Live session not admitted");
    lifetime();provenance(true);tracking(true);h_.worker_health();lifetime();
});}
void LiveSessionSafety::install_sram(SramOwner& ram,const Bytes& stock,const Bytes& elf,const Bytes& state){latch([&]{
    if(sram_||sram_closed_)throw std::runtime_error("SRAM session transaction already consumed");
    before_sram();
    // Hash/ELF admission occurs before even binding a mutation-capable owner.
    auto module=admit_original210(stock,elf);
    sram_=&ram; // retain this owner even when install only partially succeeds
    ram.install(stock,module.code,module.entry,state);
});}
void LiveSessionSafety::enter_sram(){latch([&]{
    if(!sram_||sram_entered_||sram_closed_)throw std::runtime_error("SRAM session not ready for entry");
    sram_entered_=true;sram_->enter([&]{before_sram();});
});}
CleanupBarriers LiveSessionSafety::restore_sram(){
    if(!sram_||sram_closed_)throw std::runtime_error("No recoverable SRAM session");
    sram_restoring_=true;
    try{sram_->restore();}catch(const std::exception& e){std::cerr<<e.what()<<"; SRAM owner retained\n";}
    if(!sram_->barriers.complete()||!sram_->may_release_owner()){
        std::cerr<<"SRAM restoration barriers unresolved; retain owner\n";
        return sram_->barriers;
    }
    sram_closed_=true;return sram_->barriers;
}
void LiveSessionSafety::guard(){latch([&]{
    if(!admitted_)throw std::runtime_error("Live session not admitted");
    lifetime();provenance(false);tracking(false);h_.worker_health();
    h_.publication_poll(); // expiry runs even while diagnostics/clock acquire
});}
void LiveSessionSafety::fresh_tracking(){latch([&]{
    if(!admitted_)throw std::runtime_error("Live session not admitted");
    lifetime();provenance(false);tracking(true);
});}
void LiveSessionSafety::poll(){latch([&]{
    guard();
    if(!h_.diagnostics_ready())return;
    h_.camera_poll(); // OCC no-growth/decoder errors propagate, never auto-retry
    // A failed identity window is retried inside IdentityPhaseWindow (logged there).
    h_.roy_poll(true); // RoyClockPoll retains exact 8s acquisition/50ms probe policy
});}
}

#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_session_safety, "session-safety");
