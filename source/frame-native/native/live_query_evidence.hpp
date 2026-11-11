#pragma once
#include "query_peer.hpp"
#include "phase5/json.hpp"
#include <cmath>
#include <memory>
namespace pico {
using LiveEvent = std::function<void(const std::string&,const std::string&)>;
inline QueryHooks measured_query_hooks(std::function<BenchImuEvidence()> bench,
                                      std::function<void()> fresh,LiveEvent event) {
    if(!bench || !fresh || !event)
        throw std::runtime_error("Measured query sources required");
    QueryHooks hooks;
    auto upstream=std::make_shared<Page>();
    hooks.bench=std::move(bench);
    hooks.fresh_guard=std::move(fresh);
    // The shared owner replaces this with its independent native25 radio tick.
    hooks.native_radio_tick=[]()->bool {
        throw std::runtime_error("Query radio owner not installed");
    };
    hooks.proof=[event,upstream](unsigned schema,const Bytes& body,const Page& fields){
        if(schema==4)*upstream=fields;
        phase5::Value row;
        row.type='{';
        row.object["schema"]=phase5::parse(std::to_string(schema));
        row.object["body_hex"].type='s';
        row.object["body_hex"].text=hex(body.data(),body.size());
        auto& values=row.object["fields"];
        values.type='{';
        for(const auto& field:fields)
            values.object[field.first]=phase5::parse(std::to_string(field.second));
        event("diagnostic-proof",phase5::encode(row));
    };
    hooks.action=[event,upstream](uint8_t command,const Bytes& body){
        if(command==10)upstream->clear();
        event("diagnostic-action",std::to_string(command)+":"+hex(body.data(),body.size()));
    };
    hooks.prerequisites=[event](const std::vector<std::pair<unsigned,Page>>& pages,
                                const std::string& inference){
        // QueryPeer performs the exact prerequisite predicates before delivery.
        phase5::Value row;
        row.type='{';
        row.object["running_timer_evidence"].type='s';
        row.object["running_timer_evidence"].text=inference;
        auto& values=row.object["pages"];
        values.type='[';
        for(const auto& page:pages){
            phase5::Value item;
            item.type='{';
            item.object["schema"]=phase5::parse(std::to_string(page.first));
            auto& fields=item.object["fields"];
            fields.type='{';
            for(const auto& field:page.second)
                fields.object[field.first]=phase5::parse(std::to_string(field.second));
            values.array.push_back(std::move(item));
        }
        event("native10-prerequisites",phase5::encode(row));
    };
    hooks.producer_lifetime=[event,upstream](double age){
        if(!std::isfinite(age) || age<0 || age>3)  // owner rule: logged, not fatal
            event("producer-evidence-age-logged",std::to_string(age));
        phase5::Value row;
        row.type='{';
        row.object["bootstrap_producer_age_s"]=phase5::parse(std::to_string(age));
        row.object["post_transition_counter_fresh"]=phase5::parse("true");
        auto& gates=row.object["current_upstream_gates"];
        gates.type='{';
        for(const auto& gate:*upstream)
            gates.object[gate.first]=phase5::parse(std::to_string(gate.second));
        row.object["interpretation"].type='s';
        row.object["interpretation"].text="Two fresh mode7 advancing producer snapshots after native10/IR initialization; strict controllers-off guard, identity and fixed phase verified. No joint coexistence claim; decoded IMU decides success";
        event("producer-evidence-lifetime",phase5::encode(row));
    };
    return hooks;
}
}

#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_measured_query, "measured-query");
