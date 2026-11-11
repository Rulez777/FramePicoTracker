#pragma once
#include "peer_engine.hpp"
namespace pico {
using Page = std::map<std::string, uint32_t>;
Page parse_page(const Bytes &, unsigned, const Bytes &nonce, const Bytes &chip);
struct QueryHooks {
    std::function<BenchImuEvidence()> bench;
    std::function<void()> fresh_guard;
    // Returns whether native25's radio owner has an active ledger after tick.
    std::function<bool()> native_radio_tick;
    std::function<void(unsigned, const Bytes &, const Page &)> proof;
    std::function<void(uint8_t, const Bytes &)> action;
    std::function<void(const std::vector<std::pair<unsigned, Page>> &, const std::string &)>
        prerequisites;
    std::function<void(double)> producer_lifetime;
};
// Installed query_loader behavior, rather than a permissive base-engine pilot.
// Camera/native25 implementation is injected by later producer phases.
class QueryPeer final : public PeerEngine {
    QueryHooks hooks_;
    const uint8_t independent_mode_; // Only exact env "0" opts out; fixed for this peer's lifetime.
    LaterImuStart bench_gate_;
    BenchImuEvidence producer_;
    struct Active {
        unsigned schema;
        Bytes nonce;
        double sent;
    };
    std::optional<Active> diagnostic_active_;
    std::optional<double> settle_started_;
    unsigned settle_queries_ = 0;
    bool unsettled_ = false;
    Bytes breadcrumb_first_, breadcrumb_second_;
    uint8_t breadcrumb_source_ = 0;
    bool breadcrumb_coherent_ = true;
    unsigned breadcrumb_step_ = 0;
    bool breadcrumb_done_ = false;
    std::map<size_t, unsigned> page_failures_;
    uint64_t plan_disconnects_ = 0;
    bool plan_restart_pending_ = false;
    size_t plan_size() const;
    int plan_step(size_t) const;
    size_t step_position(int, bool last=false) const;
    bool release_revision() const { return (firmware_revision == 11 || firmware_revision == 12); }
    BenchImuEvidence bench();
    bool bench_admission();
    void prerequisites();
    void prerequisites_checked();

  public:
    size_t diagnostic_index = 0;
    double diagnostic_due = 0;
    // Historical name: completion of the selected independent-mode action (0 or 1).
    bool bootstrap_n1_requested = false, native_ir31_applied = false;
    // multi4: N of the bootstrap command 16 (2K-1, peer_table.hpp) instead of 1 / PICO_TEST_N; unset = unchanged.
    std::optional<uint8_t> bootstrap_n;
    // 2026-11-02 multi data plane: a link loss after the release plan (17/10/25/14/31/19) left the first target
    // reconnected but never told to stream again (session-215848: 3 s silence, product reply, then only empty
    // replies to the end). Set = run the release plan again once the link is back. Unset (single path) = unchanged.
    bool restart_plan_after_reconnect = false;
    uint64_t plan_restarts = 0;
    std::vector<std::pair<unsigned, Page>> observer_pages;
    std::optional<Page> breadcrumb;
    QueryPeer(Bytes, int, std::function<double()>, std::function<Bytes()>, QueryHooks);
    bool ack_tag(uint16_t) override;
    void advance() override;
    bool receive(const Bytes &, int, uint16_t, bool) override;
    bool active_query() const { return diagnostic_active_.has_value(); }
    bool diagnostics_ready() const;
};
} // namespace pico
