#pragma once
#include "live_session_owner.hpp"

namespace pico {
class LiveRecordingSession {
  public:
    virtual ~LiveRecordingSession() = default;
    virtual const OpticalPhaseVerification &phase() const = 0;
    virtual LedModel model() const = 0;
    virtual NativeOpticalApi api() const = 0;
    virtual QueryHooks query_hooks() = 0;
    // Must complete and close the preflight source before any MCU mutation.
    virtual void preflight(const std::function<void()> &guard) = 0;
    virtual void bind_evidence(LiveSessionOwner &,LiveSessionSafety &) = 0;
    virtual void start(LiveSessionOwner &, LiveSessionSafety &, QueryPeer &) = 0;
    virtual void poll() = 0;
    virtual void health() = 0;
    virtual void stop_join() = 0;
    virtual void event(const std::string &, const std::string &) = 0;
};
}
