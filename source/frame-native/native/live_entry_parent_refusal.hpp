#pragma once
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace pico::live_fixture {
inline constexpr const char* competing_owner_reason =
    "pid=424242 exe=[/fixture/openocd] argv=[openocd --fixture-owner]";
inline std::string competing_owner_failure() {
    return std::string("Existing or unknown radio owner: ") + competing_owner_reason + " comm=[]";
}
// Compare the complete terminal diagnostic, never a substring in another error
// or in earlier progress output. A successful case must have no error line.
inline void verify_fixture_result(const std::string& output, int status,
                                  const std::string& expected_reason, bool supervised = false) {
    const std::string prefix = "pico_producer: ";
    std::istringstream lines(output);
    std::string line, actual;
    unsigned errors = 0;
    while (std::getline(lines, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.compare(0, prefix.size(), prefix) == 0) {
            ++errors;
            actual = line.substr(prefix.size());
        }
    }
    const auto expected = (supervised ?
        "Supervised runtime failed; restoration and release verified: " : "") + expected_reason;
    const bool success = expected_reason.empty();
    if (success ? status != 0 || errors != 0 : status != 1 || errors != 1 || actual != expected)
        throw std::runtime_error("Fixture result mismatch: actual exit=" + std::to_string(status) +
            " errors=" + std::to_string(errors) + " reason=[" + actual + "] expected exit=" +
            (success ? "0" : "1") + " reason=[" + (success ? "" : expected) + "]");
}

// A parent admission refusal has no restoration transaction to report. Keep
// this exception narrower than the supervised failure/cleanup contract.
inline void verify_parent_owner_refusal(const std::filesystem::path& root, int status) {
    const auto require = [](bool value) {
        if (!value) throw std::runtime_error("Unverified parent owner refusal");
    };
    const auto read = [&](const std::filesystem::path& path) {
        std::ifstream input(path, std::ios::binary);
        require(bool(input));
        return std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
    };
    verify_fixture_result(read(root / "result.log"), status, competing_owner_failure());
    require(!std::filesystem::exists(root / "supervisor-pid"));
    require(std::filesystem::is_empty(root / "attempt/recovery-journal"));
    require(read(root / "settings.json") == "{}\n");
    require(read(root / "power") == "120\n");
    require(!std::filesystem::exists(root / "attempt/openocd.log"));
    // Only the admission views are permitted: no profile, camera, debugger,
    // checkpoint, SRAM, radio, release or forbidden signal action may occur.
    if (std::filesystem::exists(root / "events")) {
        std::istringstream events(read(root / "events"));
        std::string line;
        while (std::getline(events, line))
            require(line == "fixture-process-view" || line == "fixture-service-view");
    }
}
}
