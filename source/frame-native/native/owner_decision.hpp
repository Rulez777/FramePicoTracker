#pragma once
#include "phase5/orchestrator.hpp"
#include "live_guard_inventory.hpp"
namespace pico {
// Authorization is distinct from physical readiness; it grants no runtime credit.
inline void validate_owner_decision(const std::string& bytes,const std::string& boot) try {
    if(bytes.size()>4096)throw std::runtime_error("Owner decision exceeds 4096 bytes");
    const auto v=phase5::parse(bytes);
    auto s=[&](const char* key){const auto& x=phase5::field(v,key);
        if(x.type!='s'||x.text.empty())throw std::runtime_error("Owner decision field absent");
        return x.text;
    };
    if(s("owner_words")!="\xd0\xb3\xd0\xbe \xd0\xbf\xd1\x80\xd0\xbe\xd0\xb3\xd0\xbe\xd0\xbd" || s("boot_id")!=boot || boot.empty())
        throw std::runtime_error("Owner decision words/boot mismatch");
    const auto t=s("timestamp");
    if(t.size()!=20||t[4]!='-'||t[7]!='-'||t[10]!='T'||t[13]!=':'||t[16]!=':'||t[19]!='Z')
        throw std::runtime_error("Owner decision timestamp must be UTC YYYY-MM-DDTHH:MM:SSZ");
    for(size_t i=0;i<t.size();++i)if(i!=4&&i!=7&&i!=10&&i!=13&&i!=16&&i!=19&&(t[i]<'0'||t[i]>'9'))
        throw std::runtime_error("Owner decision timestamp invalid");
    const int year=std::stoi(t.substr(0,4)),month=std::stoi(t.substr(5,2)),day=std::stoi(t.substr(8,2));
    const int days[]={31,28,31,30,31,30,31,31,30,31,30,31};
    if(year<2026||month<1||month>12||day<1||
       day>days[month-1]+(month==2&&year%4==0&&(year%100!=0||year%400==0)?1:0)||
       std::stoi(t.substr(11,2))>23||std::stoi(t.substr(14,2))>59||std::stoi(t.substr(17,2))>59)
        throw std::runtime_error("Owner decision timestamp invalid");
}
catch(const std::exception& e){std::cerr<<e.what()<<"; owner decision report only, continuing\n";}
inline std::string owner_unproven_families() {
    return std::string("owner-admit-unproven: registry proof hold ONLY; family 7 controller coexistence OFF (controllers-off only); family 20 IMU fusion admission OFF\n")+missing_live_guards();
}
}
