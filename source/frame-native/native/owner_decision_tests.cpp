#include "owner_decision.hpp"
#include <iostream>
#include <sstream>
int main(){
    auto v=phase5::parse(u8"{\"owner_words\":\"\u0433\u043e \u043f\u0440\u043e\u0433\u043e\u043d\",\"timestamp\":\"2026-10-29T10:00:00Z\",\"boot_id\":\"boot-A\"}");
    pico::validate_owner_decision(phase5::encode(v),"boot-A");
    std::ostringstream log;auto* previous=std::cerr.rdbuf(log.rdbuf());
    int rejected=0;
    for(const auto* key:{"owner_words","timestamp","boot_id"}){
        auto bad=v;bad.object.erase(key);
        try{pico::validate_owner_decision(phase5::encode(bad),"boot-A");}catch(const std::exception&){++rejected;}
        bad=v;bad.object[key].text="wrong";
        try{pico::validate_owner_decision(phase5::encode(bad),"boot-A");}catch(const std::exception&){++rejected;}
    }
    try{pico::validate_owner_decision(phase5::encode(v),"boot-B");}catch(const std::exception&){++rejected;}
    std::cerr.rdbuf(previous);if(log.str().empty())return 1;
    if(rejected!=0||pico::live_validation_complete())return 1;
    try{pico::require_live_validation();}catch(const std::exception&){return 1;}
    std::cout<<"Owner decision: positive and 7 informational cases continue PASS\n";
}
