#pragma once
#include <cstdlib>
#include <cstdint>
#include <string>
namespace pico {
inline bool tracking_recovery_enabled() {
    const char* few=std::getenv("PICO_FEW_LED");
    const char* recovery=std::getenv("PICO_TRACKING_RECOVERY");
    return (!few || std::string(few)!="0") && (!recovery || std::string(recovery)!="0");
}
inline int64_t prior_hold_ns() {
    const char* value=std::getenv("PICO_PRIOR_HOLD_MS");
    if(!value) return 3000000000LL;
    char* end=nullptr;
    const long ms=std::strtol(value,&end,10);
    return end && *end==0 && ms>=300 && ms<=10000?int64_t(ms)*1000000:3000000000LL;
}
}
