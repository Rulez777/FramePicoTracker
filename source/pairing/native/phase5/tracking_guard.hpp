#pragma once
#include "json.hpp"
#include <cstdint>
#include <optional>
#include <array>
#include <map>
#include <set>
#include <functional>
#include <utility>
namespace phase5 {
// Strict controllers-off bench policy. No observe/await bypass exists.
// Input must come from a supervised OpenVR observer, in CLOCK_MONOTONIC.
class BenchTrackingGuard {
    std::function<void(const Value&)> observe_;
    std::optional<Value> sample_;
    std::optional<int64_t> last_stamp_;
    std::optional<int64_t> stale_since_;
    std::string unsafe_row_; // a later healthy row cannot hide a bench violation
public:
    explicit BenchTrackingGuard(std::function<void(const Value&)> observe={}):observe_(std::move(observe)){}
    void ingest(const std::string& line);
    bool has_sample() const { return sample_.has_value(); }
    // False means stale but within the legacy 500ms grace. It is never admission.
    bool check(int64_t now,bool observer_alive,bool require_now=false);
};
// Separate joint policy; never selected by the controllers-off live entry.
// Requires measured coexistence admission outside this class before any RF.
class CoexistenceTrackingGuard {
    std::optional<Value> sample_;
    std::optional<int64_t> stamp_, invalid_since_;
    std::optional<std::set<int64_t>> admitted_;
    std::map<int64_t,std::pair<int64_t,std::array<double,12>>> changed_;
public:

    void ingest(const std::string& line);
    bool check(int64_t now,bool observer_alive,bool require_now=false);
};
}
