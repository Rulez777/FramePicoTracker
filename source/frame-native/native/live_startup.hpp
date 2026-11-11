#pragma once
#include <cstdint>
#include <exception>
#include <functional>
#include <stdexcept>
#include <iostream>

namespace pico {
inline constexpr int64_t camera_readiness_ns = 12000000000LL;

// Shared production ordering boundary. A failed camera proof or source close
// cannot reach checkpoint/install/entry, including partially started sources.
inline void preflight_then_enter(const std::function<void()>& preflight,
    const std::function<void()>& install,const std::function<void()>& enter) {
    try{preflight();}catch(const std::exception& e){std::cerr<<e.what()<<"; camera preflight report only, continuing\n";}
    install();
    enter();
}

// The deadline starts after source.start(), as in ProfileOwner.preflight.
inline void await_camera_ready(const std::function<int64_t()>& now,
    const std::function<void()>& guard,const std::function<bool()>& poll,
    const std::function<void()>& sleep) {
    const auto start = now();
    auto previous = start;
    for (;;) {
        guard();
        const auto current = now();
        if (current < previous || current - start >= camera_readiness_ns)
            {std::cerr<<"Camera readiness regressed/exceeded 12s; continuing with available camera state\n";return;}
        previous = current;
        if (poll()) return;
        sleep();
    }
}

// Attempt both joins and log each failure. Never release a profile or
// LED ledger here; the supervisor alone authorizes those after barriers.
inline void stop_camera_worker(const std::function<void()>& worker,
    const std::function<void()>& camera) {
    try { worker(); } catch (const std::exception& e) {std::cerr<<e.what()<<"; worker cleanup continues\n";}
    try { camera(); } catch (const std::exception& e) {std::cerr<<e.what()<<"; camera cleanup continues\n";}
}

inline void stop_factory_dispatch(bool& stopping,const std::function<void()>& dispatch) {
    stopping = true;
    // Camera/recording closure must be registered with the supervisor's
    // post-barrier finalizer. This phase only revokes and joins dispatch.
    dispatch();
}
}
