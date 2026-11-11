#pragma once
#include "recovery_journal.hpp"
#include "roy_owner.hpp"

namespace pico {
// Parent retains this state independently of the child's filesystem view.
// Only the exact generation consumed over the private channel is acknowledged.
class SupervisorReportConsumer {
    std::string session_;
    std::string checkpoint_;
    std::string report_;
    uint64_t sequence_ = 0;
public:
    explicit SupervisorReportConsumer(std::string session) : session_(std::move(session)) {
        if (!recovery_digest(session_))
            throw std::runtime_error("Invalid supervisor session");
    }
    std::string consume(const std::string& bytes) {
        const auto record = recovery_decode(bytes);
        if (record.session != session_ || !report_.empty())
            throw std::runtime_error("Supervisor report session/replay mismatch");
        if (record.sequence != sequence_ + 1 ||
            record.previous != (sequence_ ? checkpoint_ : std::string(64, '0')))
            throw std::runtime_error("Supervisor generation/head mismatch");
        if (record.kind == "checkpoint") {
            checkpoint_ = phase5::sha256(bytes);
            sequence_ = record.sequence;
            return checkpoint_;
        }
        if (!sequence_ && record.kind == "unmutated" &&
            record.payload == "no-sram-write;resources-closed") {
            report_ = phase5::sha256(bytes);
            return report_;
        }
        if (checkpoint_.empty() || record.kind != "barriers" ||
            record.payload != CleanupBarriers{true, true, true, true}.json())
            throw std::runtime_error("Supervisor barriers not independently bound/literal true");
        report_ = phase5::sha256(bytes);
        return report_;
    }
    bool complete() const { return !report_.empty(); }
    const std::string& checkpoint_head() const { return checkpoint_; }
};
}
