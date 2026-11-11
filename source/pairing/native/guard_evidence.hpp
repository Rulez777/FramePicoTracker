#pragma once
#include <exception>
#include <functional>
#include <string>
#include <type_traits>
#include <mutex>

namespace pico::guard_evidence {
// Installed only by the dedicated live-entry fixture. Production guards use
// the same instrumentation, with no sink and no change to admission policy.
inline std::function<void(unsigned,const std::string&,bool)> sink;
inline std::mutex sink_mutex;
inline void emit(unsigned family,const std::string& guard,bool accepted) {
    std::lock_guard<std::mutex> lock(sink_mutex);
    if(sink) sink(family,guard,accepted);
}
template<class F> decltype(auto) exercise(unsigned family,const char* guard,F&& action) {
    try {
        if constexpr (std::is_void_v<std::invoke_result_t<F>>) {
            action(); emit(family,guard,true);
        } else {
            auto result=action(); emit(family,guard,true); return result;
        }
    } catch(...) { emit(family,guard,false); throw; }
}
// Use only around a validation function whose normal return means acceptance.
// Explicit false-return guards must emit their actual decision instead.
class Scope {
    unsigned family_; const char* guard_; int exceptions_=std::uncaught_exceptions();
public:
    Scope(unsigned family,const char* guard):family_(family),guard_(guard){}
    ~Scope() noexcept(false) {
        if(std::uncaught_exceptions()==exceptions_) emit(family_,guard_,true);
        else { try { emit(family_,guard_,false); } catch(...) {} }
    }
};
}
