#pragma once
#include <functional>
#include <string>
namespace pico {class LiveSessionOwner;}
struct ProducerReplayResult { size_t publications=0; std::string barriers; };
// All doubles are explicit. Work runs while the SAME fake SRAM/TCL owner is live.
ProducerReplayResult unified_synthetic_replay(
    const std::function<void(const std::function<void()>&)>& work,
    const std::function<void()>& guard,
    const std::function<void(const std::string&)>& restored,
    const std::string& safety_fault = {},
    const std::function<void()>& verify_invocation = {},
    const std::function<void()>& verify_profile = {},
    pico::LiveSessionOwner* session_owner = nullptr,
    int replay_seconds = 180);
size_t synthetic_optical_replay();
