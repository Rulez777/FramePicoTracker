#pragma once
#include <map>
#include <string>
namespace pico {
// Implementation availability only. Runtime proof is acquired by the guards.
class LiveImplementationRegistry {
    std::map<std::string, std::string> providers_;
public:
    bool provide(const std::string& capability, const std::string& source) {
        providers_.emplace(capability, source);
        return true;
    }
    bool has(const std::string& capability) const {
        return providers_.count(capability) != 0;
    }
};
inline LiveImplementationRegistry& live_implementations() {
    static LiveImplementationRegistry value;
    return value;
}
#define PICO_LIVE_IMPLEMENTATION(symbol, capability) \
    inline const bool symbol = ::pico::live_implementations().provide(capability, __FILE__)
}
