#pragma once
#include "live_guard_inventory.hpp"
#include <map>
#include "phase5/json.hpp"
#include <set>
#include <string>

namespace pico::live_fixture {
// Only completed harness assertions enroll guard events. This report describes
// synthetic enforcement, independently of the production validation hold.
class EnforcementReport {
    std::set<std::string> passed_;
    struct Evidence { std::string name,guard; bool accepted; };
    std::map<unsigned,std::vector<Evidence>> evidence_;
    static bool matching_fault(unsigned family,const std::string& name) {
        static const std::map<unsigned,std::set<std::string>> faults{
            {1,{"seal-target","seal-chip","seal-slot","seal-image","seal-argv","seal-file","seal-module","readiness-boot","readiness-age","readiness-future","readiness-candidate","readiness-incomplete"}},
            {3,{"listener-child"}}, {7,{"coexistence-frozen"}}, {8,{"optical-head-origin"}},
            {9,{"stock-anchor","stock-rtt","seal-module","checkpoint-failure"}},
            {10,{"radio-source-chip","radio-source-channel","radio-partial-crc","radio-payload-checksum"}},
            {11,{"radio-ring-loss","radio-heartbeat"}}, {12,{"radio-wrong-nonce","radio-lease-expired"}},
            {15,{"optical-led-restore"}}, {16,{"optical-image-size","optical-sensor","optical-identity-age"}},
            {18,{"optical-covariance"}}, {20,{"imu-axes","imu-time"}}, {21,{"camera-clock-boot"}},
            {22,{"optical-factory-model","optical-factory-rig"}},
            {23,{"optical-skew","optical-exposure","optical-source-age","optical-metadata","decoder-PTS"}},
            {24,{"camera-growth","decoder-failure"}}, {25,{"optical-clock-horizon","radio-clock-raw"}},
            {26,{"optical-publication-expiry"}}, {27,{"parent-signal","wall-bound"}}, {29,{"unresolved-owner-close"}}
        };
        const auto found=faults.find(family);return found!=faults.end()&&found->second.count(name)!=0;
    }
    static const std::map<unsigned,std::set<std::string>>& guards() {
        static const std::map<unsigned,std::set<std::string>> value{
            {1,{"seals","readiness"}}, {3,{"listener-child"}}, {7,{"coexistence-150ms"}},
            {8,{"head-bracket","origin"}}, {9,{"stock-rtt","module","checkpoint"}},
            {10,{"canonical","ack-tag"}}, {11,{"ring-capacity","heartbeat"}},
            {12,{"nonce-link","mode-lease"}}, {15,{"led-restore","two-episodes"}},
            {16,{"image-age-provenance"}}, {18,{"covariance"}}, {20,{"axes-time","sample-time"}},
            {21,{"boot-clock","acquisition"}}, {22,{"rig","factory-model"}},
            {23,{"PTS","stereo-time-exposure-age"}}, {24,{"source-growth-owner","decoder-health"}},
            {25,{"raw-fit","source-horizon","projection-horizon"}},
            {26,{"PTP2","PTI1","expiry"}}, {27,{"parent-wall","parent-signal"}},
            {29,{"retain-owner"}}
        }; return value;
    }
public:
    void passed(const std::string& name) { passed_.insert(name); }
    void observed(const std::string& name,const std::string& lines) {
        // Called only after the exit, refusal reason, cleanup and orphan checks.
        if(!passed_.count(name))throw std::runtime_error("Evidence requires completed case");
        std::istringstream input(lines);std::string line;size_t count=0;
        const auto inventory=live_guard_inventory(count);
        while(std::getline(input,line)) {
            if(line.empty())continue;
            const auto row=phase5::parse(line);
            if(phase5::field(row,"accepted").type!='b' ||
               phase5::field(row,"case").type!='s' || phase5::field(row,"id").type!='s' ||
               phase5::field(row,"guard").type!='s')
                throw std::runtime_error("Malformed guard evidence");
            const auto family=phase5::number(row,"family");
            if(family<1 || uint64_t(family)>count || phase5::field(row,"case").text!=name ||
               phase5::field(row,"id").text!=inventory[family-1].id)
                throw std::runtime_error("Mismatched guard evidence");
            evidence_[unsigned(family)].push_back({name,phase5::field(row,"guard").text,phase5::flag(row,"accepted")});
        }
    }
    std::string json() const {
        const std::map<std::string, std::set<std::string>> required{
            {"single-roy-owner", {"owner-process", "service-active", "service-query-error", "service-missing", "service-missing-inactive", "service-failed", "ok"}},
            {"profile-settings-power", {"ok", "write", "after-entry"}},
            {"tracking-observer-supervision", {"observer-eof", "ok"}},
            {"controllers-off-hmd-bench", {"controllers", "hmd", "optical-slow"}},
            {"native-N-pending-diagnostic-prerequisites", {"radio-native-n", "radio-native-pending", "radio-ok"}},
            {"startup-query-24s-three-attempts", {"radio-recovery", "radio-recovery-exhaust"}},
            {"phase-48-of-61-exhausted", {"optical-ok", "optical-phase-exhausted"}},
            {"imu-command19-optical-before-imu", {"optical-ok", "optical-producer-disabled"}},
            {"four-literal-true-cleanup-barriers", {"write", "after-entry", "radio-ring-loss", "optical-ok"}},
            {"worker-before-publisher-profile-release", {"worker", "camera-close", "optical-ok"}}
        };
        size_t count=0, satisfied=0;
        const auto rows=live_guard_inventory(count);
        std::ostringstream out;
        for(size_t i=0;i<count;++i) {
            const auto found=required.find(rows[i].id);
            bool complete=found!=required.end();
            if(complete)for(const auto& name:found->second)complete=complete&&passed_.count(name)!=0;
            std::set<std::string> observed_names;
            const auto measured=guards().find(unsigned(i+1));
            if(measured!=guards().end()) {
                std::set<std::string> accepted; bool rejected=false;
                const auto events=evidence_.find(unsigned(i+1));
                if(events!=evidence_.end())for(const auto& event:events->second) {
                    const bool positive=event.name=="ok" || event.name=="optical-ok" || event.name=="radio-ok";
                    if(event.accepted && positive)accepted.insert(event.guard);
                    if(!event.accepted && measured->second.count(event.guard) && matching_fault(unsigned(i+1),event.name))rejected=true;
                    observed_names.insert(event.name);
                }
                complete=rejected;
                for(const auto& guard:measured->second)complete=complete&&accepted.count(guard)!=0;
            }
            if(complete)++satisfied;
            out<<"{\"family\":"<<i+1<<",\"id\":\""<<rows[i].id
               <<"\",\"status\":\""<<(complete?"SATISFIED":"UNSATISFIED")<<"\",\"observed_cases\":[";
            bool comma=false;
            if(measured==guards().end() && found!=required.end())for(const auto& name:found->second)if(passed_.count(name))observed_names.insert(name);
            for(const auto& name:observed_names) {
                if(comma)out<<',';
                out<<'"'<<name<<'"';comma=true;
            }
            out<<"]}\n";
        }
        out<<"{\"scope\":\"synthetic-live-entry-enforcement\",\"families\":"<<count
           <<",\"satisfied\":"<<satisfied<<",\"unsatisfied\":"<<count-satisfied
           <<",\"runtime_admitted\":false}\n";
        return out.str();
    }
};
}
