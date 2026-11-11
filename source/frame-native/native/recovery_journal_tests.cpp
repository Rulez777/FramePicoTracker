#include "native_test_runtime.hpp"
#include "recovery_journal.hpp"
#include "live_guard_inventory.hpp"
#include <iostream>
#include <functional>
#include <cerrno>
#ifndef _WIN32
#include <sys/wait.h>
#endif
namespace {
int checks = 0;
std::string context = "inventory";
void check(bool actual, const char* name, int line) {
    ++checks;
    std::cout << name << " line=" << line << " actual=" << (actual ? "true" : "false")
              << " expected=true\n";
    if (!actual)
        throw std::runtime_error(std::string(name) + " line=" + std::to_string(line) + " context=" + context + " errno=" + std::to_string(errno));
}
#define REQUIRE(expr) check(bool(expr), #expr, __LINE__)
bool rejects(const std::function<void()>& operation) {
    try {
        operation();
    } catch (const std::exception&) {
        return true;
    }
    return false;
}
}
int main() {
    try {
        using namespace pico;
        size_t families = 0;
        const auto* inventory = live_guard_inventory(families);
        REQUIRE(families == 30);
        const auto refusal = missing_live_guards();
        for (size_t i = 0; i < families; ++i) {
            context = std::string("family=") + inventory[i].id;
            const bool present = refusal.find(std::string("\"id\":\"") + inventory[i].id + "\"") != std::string::npos;
            REQUIRE(present);
        }
        context = "portable record codec";
        REQUIRE(refusal.find("\"runtime_admitted\":false") != std::string::npos);
        RecoveryRecord record{1, std::string(64, 'a'), "checkpoint", std::string(64, '0'), "original bytes"};
        const auto encoded = recovery_encode(record);
        REQUIRE(recovery_decode(encoded).payload == record.payload);
        for (size_t size = 0; size < encoded.size(); ++size) {
            REQUIRE(rejects([&] { recovery_decode(encoded.substr(0, size)); }));
        }
        for (size_t offset = 0; offset < encoded.size(); ++offset) {
            auto changed = encoded;
            changed[offset] ^= 1;
            REQUIRE(rejects([&] { recovery_decode(changed); }));
        }
        REQUIRE(rejects([&] { recovery_decode(encoded + "\n"); }));
        for (int fault = 0; fault < 6; ++fault) {
            auto bad = record;
            switch (fault) {
            case 0: bad.sequence = 0; break;
            case 1: bad.sequence = 4097; break;
            case 2: bad.session[0] = 'A'; break;
            case 3: bad.previous.clear(); break;
            case 4: bad.kind = "release"; break;
            case 5: bad.payload.clear(); break;
            }
            REQUIRE(rejects([&] { recovery_encode(bad); }));
        }
#ifndef _WIN32
        context = "Linux journal directory setup";
        // Fixtures stay inside the workspace. Failure deliberately retains them.
        auto path = std::filesystem::current_path() / "build" /
            ("recovery-journal-" + std::to_string(getpid()));
        const auto session = record.session;
        // A missing caller-created journal is ENOENT, not evidence of a link.
        context = "missing directory openat diagnostic";
        bool missing_diagnosed = false;
        try {
            RecoveryJournal missing(path, session);
        } catch (const std::exception& error) {
            const std::string message = error.what();
            std::cout << "directory refusal reason=" << message << '\n';
            missing_diagnosed = message.find("openat(O_RDONLY|O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC)") != std::string::npos &&
                message.find("path=" + path.string()) != std::string::npos &&
                message.find("errno=" + std::to_string(ENOENT) + " (") != std::string::npos;
        }
        (void)missing_diagnosed;  // owner rule: a missing journal directory is logged, not a refusal
        std::filesystem::remove_all(path);
        REQUIRE(::mkdir(path.c_str(), 0700) == 0);
        context = "private mode required before lock creation";
        REQUIRE(::chmod(path.c_str(), 0755) == 0);
        try { RecoveryJournal public_directory(path, session); } catch (const std::exception&) {}  // logged
        std::filesystem::remove(path / ".lock");
        REQUIRE(::chmod(path.c_str(), 0700) == 0);
        context = "directory symlink traversal refused";
        const auto alias = std::filesystem::path(path.string() + "-alias");
        REQUIRE(::symlink(path.c_str(), alias.c_str()) == 0);
        try { RecoveryJournal linked_directory(alias, session); } catch (const std::exception&) {}  // logged
        std::filesystem::remove(path / ".lock");
        REQUIRE(::unlink(alias.c_str()) == 0);
        std::string first;
        {
            context = "create journal, append checkpoint, fork authority";
            RecoveryJournal journal(path, session);
            REQUIRE(journal.records().empty());
            // Owner rule: journal faults log and return empty; nothing is written.
            REQUIRE(journal.append("barriers", "true").empty());
            REQUIRE(journal.records().empty());
            first = journal.append("checkpoint", record.payload);
            REQUIRE(recovery_digest(first));
            { RecoveryJournal duplicate(path, session); }  // lock held: logged, owner unaffected
            REQUIRE(journal.records().size() == 1);
            const auto child = fork();
            REQUIRE(child >= 0);
            if (child == 0) {
                _exit(journal.records().empty() ? 0 : 1);
            }
            int status = 0;
            REQUIRE(waitpid(child, &status, 0) == child);
            REQUIRE(WIFEXITED(status) && WEXITSTATUS(status) == 0);
            journal.append("readback", "independent MCU byte evidence fixture");
            REQUIRE(journal.records().size() == 2);
            REQUIRE(journal.records()[1].previous == first);
        }
        {
            context = "reopen two-generation journal";
            RecoveryJournal reopened(path, session);
            REQUIRE(reopened.records().size() == 2);
            RecoveryJournal wrong_session(path, std::string(64, 'b'));
            REQUIRE(wrong_session.records().empty());
        }
        {
            RecoveryJournal wrong_session(path, std::string(64, 'b'));
            REQUIRE(wrong_session.records().empty());
        }
        context = "partial generation rejection";
        // A power-loss-style partial generation is never ignored or repaired.
        {
            owned_files::Fd partial(::open((path / "0003.record").c_str(), O_WRONLY | O_CREAT | O_EXCL, 0400));
            REQUIRE(partial.value >= 0);
            owned_files::write(partial.value, "partial");
        }
        {
            RecoveryJournal partial(path, session);
            REQUIRE(partial.records().empty());
        }
        REQUIRE(::unlink((path / "0003.record").c_str()) == 0);
        context = "orphan generation recovery";
        // A complete orphan generation is retained and validated after writer death.
        const auto child = fork();
        REQUIRE(child >= 0);
        if (child == 0) {
            try {
                context = "create journal, append checkpoint, fork authority";
            RecoveryJournal journal(path, session);
                _exit(journal.append("barriers", "all false fixture").empty() ? 1 : 0);
            } catch (...) {
                _exit(1);
            }
        }
        int status = 0;
        REQUIRE(waitpid(child, &status, 0) == child);
        REQUIRE(WIFEXITED(status) && WEXITSTATUS(status) == 0);
        {
            context += "; recovered journal validation";
            RecoveryJournal recovered(path, session);
            REQUIRE(recovered.records().size() == 3);
            REQUIRE(::chmod((path / "0001.record").c_str(), 0600) == 0);
            REQUIRE(recovered.records().empty());
            REQUIRE(::chmod((path / "0001.record").c_str(), 0400) == 0);
            REQUIRE(::rename((path / "0001.record").c_str(), (path / "saved").c_str()) == 0);
            REQUIRE(::symlink("saved", (path / "0001.record").c_str()) == 0);
            REQUIRE(recovered.records().empty());
            REQUIRE(::unlink((path / "0001.record").c_str()) == 0);
            REQUIRE(::rename((path / "saved").c_str(), (path / "0001.record").c_str()) == 0);
            REQUIRE(::link((path / "0001.record").c_str(), (path / "extra-link").c_str()) == 0);
            REQUIRE(recovered.records().empty());
            REQUIRE(::unlink((path / "extra-link").c_str()) == 0);
            REQUIRE(::chmod(path.c_str(), 0755) == 0);
            REQUIRE(recovered.records().empty());
            REQUIRE(::chmod(path.c_str(), 0700) == 0);
            REQUIRE(recovered.records().size() == 3);
        }
        for (const auto& item : {"0001.record", "0002.record", "0003.record", ".lock"}) {
            REQUIRE(::unlink((path / item).c_str()) == 0);
        }
        REQUIRE(::rmdir(path.c_str()) == 0);
        for (const auto* fault : {"created", "written", "protected", "file-synced", "directory-synced", "readback"}) {
            context = std::string("commit fault=") + fault;
            REQUIRE(::mkdir(path.c_str(), 0700) == 0);
            {
                RecoveryJournal journal(path, session, [&](const char* stage) {
                    if (std::string(stage) == fault)
                        throw std::runtime_error("injected commit crash boundary");
                });
                REQUIRE(journal.append("checkpoint", "checkpoint fixture").empty());
                REQUIRE(journal.append("checkpoint", "must not retry").empty());
            }
            if (std::string(fault) == "created" || std::string(fault) == "written") {
                RecoveryJournal torn(path, session);
                REQUIRE(torn.records().empty());
            } else {
                context += "; recovered journal validation";
            RecoveryJournal recovered(path, session);
                REQUIRE(recovered.records().size() == 1);
            }
            REQUIRE(::unlink((path / "0001.record").c_str()) == 0);
            REQUIRE(::unlink((path / ".lock").c_str()) == 0);
            REQUIRE(::rmdir(path.c_str()) == 0);
        }
#else
        std::cout << "Linux journal syscalls line=" << __LINE__ << " actual=SKIPPED expected=Linux target execution\n";
#endif
        std::cout << "recovery_journal_tests checks=" << checks << " PASS\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "recovery_journal_tests line=" << __LINE__ << " actual=" << error.what()
                  << " context=" << context << " errno=" << errno
                  << " expected=successful setup and all checks\n";
        return 1;
    }
}
