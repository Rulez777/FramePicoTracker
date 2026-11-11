#include <iostream>
#pragma once
#include "live_implementation_registry.hpp"
#include <sstream>
#include <stdexcept>
#include <vector>
namespace pico {
struct LiveGuardInventoryRow {
    const char* id;
    const char* required;
};
inline const LiveGuardInventoryRow* live_guard_inventory(size_t& count) {
    static const LiveGuardInventoryRow rows[] = {
        {"sealed-target-image-argv-boot-readiness", "seals native-entry"},
        {"single-roy-owner", "roy-lock process-scan parent-authority linux-factory"},
        {"openocd-listener-child-attribution", "openocd"},
        {"profile-settings-power", "profile"},
        {"tracking-observer-supervision", "observer linux-factory"},
        {"controllers-off-hmd-bench", "session-safety linux-factory"},
        {"controller-coexistence-150ms", "bench-only"},
        {"head-history-origin", "head-history pixels linux-recording"},
        {"protected-stock-module-rtt-checkpoint", "sram image-admission prepared linux-factory"},
        {"canonical-crc-chip-slot-tag", "data-plane station linux-factory"},
        {"ring-pending-capacity-heartbeat", "radio-loop linux-factory"},
        {"mode-lease-nonce-link", "query linux-factory"},
        {"native-N-pending-diagnostic-prerequisites", "query measured-query linux-recording"},
        {"startup-query-24s-three-attempts", "query event-sink linux-factory"},
        {"native25-exclusive-two-episodes-led-restore", "native25 identity led-recovery linux-factory"},
        {"identity-images-link-index-age", "identity optical-worker pixels linux-recording"},
        {"phase-48-of-61-exhausted", "phase phase-routing linux-recording"},
        {"stereo-covariance-ambiguity-ray-capacity-speed", "identity optical-worker covariance linux-recording"},
        {"imu-command19-optical-before-imu", "query measured-query imu-routing linux-recording"},
        {"imu-axes-time-admission", "fusion-disabled"},
        {"camera-clock-boot-preflight", "camera-preflight camera-worker linux-recording"},
        {"factory-model-rig-enrollment", "calibration prepared linux-recording"},
        {"camera-PTS-skew-exposure-source-age", "pixels camera-worker linux-recording"},
        {"camera-staleness-decoder-growth-ownership", "camera camera-worker linux-recording"},
        {"roy-clock-fit-raw-horizons", "data-plane optical-timing linux-factory"},
        {"PTP2-PTI1-publication-expiry", "publication imu-routing linux-factory"},
        {"parent-signals-wall-bound", "supervisor parent-authority native-entry linux-factory"},
        {"four-literal-true-cleanup-barriers", "sram supervisor linux-factory"},
        {"never-kill-SRAM-owner", "supervisor linux-factory"},
        {"worker-before-publisher-profile-release", "led-recovery supervisor linux-factory"},
    };
    count = sizeof(rows) / sizeof(rows[0]);
    return rows;
}
inline std::vector<std::string> missing_implementations(const LiveGuardInventoryRow& row,
    const LiveImplementationRegistry& registry) {
    std::istringstream required(row.required);
    std::vector<std::string> missing;
    std::string capability;
    while (required >> capability) {
        if (!registry.has(capability)) missing.push_back(capability);
    }
    return missing;
}
inline bool live_parity_complete() {
    size_t count = 0;
    const auto rows = live_guard_inventory(count);
    for (size_t i = 0; i < count; ++i) {
        if (!missing_implementations(rows[i], live_implementations()).empty()) return false;
    }
    return true;
}
// Release hold: new Linux continuation validation and independent audit pending.
// Clear only in a reviewed source change with passing Linux suite/audit evidence.
inline bool live_validation_complete() { return false; }
inline void require_live_validation() {
    if (!live_validation_complete())
        std::cerr<<"Live validation report: Linux suite and independent parity audit unverified; continuing\n";
}
// No family yet has reviewed passing end-to-end proof through --live -> parent
// -> supervisor -> production session. The preceding startup harness passed on
// aarch64 per owner report. New radio/optical cases remain unexecuted on Linux;
// positive command19-gated PTI1 publication is still missing from this matrix.
// See LIVE_ENTRY_FIXTURE_2026-10-29.md. No automatic/provider-only enrollment.
inline bool live_family_end_to_end_verified(const LiveGuardInventoryRow&) { return false; }
// SATISFIED means enforced on the reachable live path AND end-to-end tested.
// Provider availability is reported separately and never grants admission.
inline std::string missing_live_guards() {
    size_t count = 0;
    size_t gaps = 0;
    size_t implementation_gaps = 0;
    const auto rows = live_guard_inventory(count);
    std::ostringstream out;
    for (size_t i = 0; i < count; ++i) {
        const auto missing = missing_implementations(rows[i], live_implementations());
        if (!missing.empty()) ++implementation_gaps;
        const bool tested = live_family_end_to_end_verified(rows[i]);
        const bool satisfied = missing.empty() && tested && live_validation_complete();
        if (!satisfied) ++gaps;
        out << "{\"family\":" << i + 1 << ",\"id\":\"" << rows[i].id
            << "\",\"status\":\"" << (satisfied ? "SATISFIED" : "UNSATISFIED")
            << "\",\"implementation_available\":" << (missing.empty() ? "true" : "false")
            << ",\"end_to_end_verified\":" << (tested ? "true" : "false")
            << ",\"missing\":[";
        bool comma = false;
        for (size_t j = 0; j < missing.size(); ++j) {
            if (comma) out << ',';
            out << "\"implementation:" << missing[j] << "\"";
            comma = true;
        }
        if (!tested) { if (comma) out << ',';
        out << "\"end-to-end-live-path-proof\""; comma = true; }
        if (!live_validation_complete()) { if (comma) out << ',';
        out << "\"live-validation-hold\""; }
        out << "]}\n";
    }
    out << "{\"families\":" << count << ",\"unsatisfied\":" << gaps
        << ",\"implementation_complete\":" << (implementation_gaps == 0 ? "true" : "false")
        << ",\"validation_complete\":" << (live_validation_complete() ? "true" : "false")
        << ",\"runtime_admitted\":false}";
    return out.str();
}
}
