#pragma once
#include "phase5/orchestrator.hpp"
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

namespace pico {
// Canonical bounded records. Hashes detect corruption, not a malicious journal
// owner. Session identity must come from independent admission, never this file.
struct RecoveryRecord {
    uint64_t sequence = 0;
    std::string session;
    std::string kind;
    std::string previous;
    std::string payload;
};
inline bool recovery_digest(const std::string& value) {
    return value.size() == 64 && value.find_first_not_of("0123456789abcdef") == std::string::npos;
}
inline std::string recovery_encode(const RecoveryRecord& record) {
    if (record.sequence == 0 || record.sequence > 4096 || !recovery_digest(record.session) ||
        !recovery_digest(record.previous) || record.payload.empty() || record.payload.size() > 1024 * 1024 ||
        (record.kind != "checkpoint" && record.kind != "readback" && record.kind != "barriers" && record.kind != "unmutated"))
        throw std::runtime_error("Invalid recovery record");
    std::string bytes = "pico-recovery-v1\n";
    for (const auto& field : {std::to_string(record.sequence), record.session, record.kind,
                              record.previous, record.payload}) {
        bytes += std::to_string(field.size()) + ":" + field;
    }
    return bytes + phase5::sha256(bytes);
}
inline RecoveryRecord recovery_decode(const std::string& bytes) {
    const std::string magic = "pico-recovery-v1\n";
    if (bytes.size() > 1024 * 1024 + 512 || bytes.compare(0, magic.size(), magic) != 0)
        throw std::runtime_error("Invalid recovery envelope");
    size_t at = magic.size();
    auto field = [&]() {
        const auto colon = bytes.find(':', at);
        if (colon == std::string::npos || colon == at || colon - at > 7)
            throw std::runtime_error("Invalid recovery field length");
        size_t length = 0;
        for (size_t i = at; i < colon; ++i) {
            if (bytes[i] < '0' || bytes[i] > '9' || (i == at && bytes[i] == '0' && colon - at > 1))
                throw std::runtime_error("Noncanonical recovery field length");
            length = length * 10 + size_t(bytes[i] - '0');
        }
        at = colon + 1;
        if (length > bytes.size() - at)
            throw std::runtime_error("Truncated recovery field");
        auto result = bytes.substr(at, length);
        at += length;
        return result;
    };
    RecoveryRecord record;
    const auto sequence = field();
    if (sequence.empty() || sequence.size() > 4)
        throw std::runtime_error("Invalid recovery sequence");
    for (char c : sequence) {
        if (c < '0' || c > '9')
            throw std::runtime_error("Invalid recovery sequence digit");
        record.sequence = record.sequence * 10 + uint64_t(c - '0');
    }
    record.session = field();
    record.kind = field();
    record.previous = field();
    record.payload = field();
    // Re-encoding also rejects alternate sequence spelling and trailing bytes.
    if (recovery_encode(record) != bytes)
        throw std::runtime_error("Recovery digest/canonical encoding mismatch");
    return record;
}
}

#ifndef _WIN32
#include "linux_owned_files.hpp"
#include <dirent.h>
#include <sys/file.h>
namespace pico {
// A caller-created, private, local filesystem directory is required. No
// replacement, truncation, automatic repair or deletion of recovery evidence.
// This sink is not a supervisor and never authorizes resource release itself.
class RecoveryJournal {
public:
    // Empty in normal use. Tests throw around actual syscalls; no hook can
    // replace fsync, exclusive creation, chain validation or readback.
    using FaultHook = std::function<void(const char*)>;
private:
    owned_files::Fd directory_;
    owned_files::Fd lock_;
    std::filesystem::path path_;
    struct stat identity_{};
    std::string session_;
    pid_t creator_ = getpid();
    bool poisoned_ = false;
    FaultHook fault_;
    static int open_directory(const std::filesystem::path& path) { try {
        owned_files::Fd fd(owned_files::parent(path / ".lock"));
        struct stat identity{};
        if (fstat(fd.value, &identity) || !S_ISDIR(identity.st_mode) ||
            identity.st_uid != geteuid() || (identity.st_mode & 0777) != 0700)
            throw std::runtime_error("Private recovery directory required before lock creation");
        const int retained = fd.value;
        fd.value = -1;
        return retained;
        } catch(const std::exception& e){std::cerr<<"Recovery journal: "<<e.what()<<"; continuing without durable directory\n";return -1;}
    }
    void stage(const char* value) const {
        if (fault_)
            fault_(value);
    }
    static std::string name(uint64_t sequence) {
        auto number = std::to_string(sequence);
        return std::string(4 - number.size(), '0') + number + ".record";
    }
    void verify_directory() const {
        if (getpid() != creator_)
            throw std::runtime_error("Inherited recovery journal authority refused");
        owned_files::Fd parent(owned_files::parent(path_));
        struct stat held{}, named{};
        if (fstat(directory_.value, &held) ||
            fstatat(parent.value, path_.filename().c_str(), &named, AT_SYMLINK_NOFOLLOW) ||
            held.st_dev != identity_.st_dev || held.st_ino != identity_.st_ino ||
            named.st_dev != held.st_dev || named.st_ino != held.st_ino ||
            !S_ISDIR(held.st_mode) || held.st_uid != geteuid() || (held.st_mode & 0777) != 0700)
            throw std::runtime_error("Recovery directory identity/authority changed");
        struct stat held_lock{}, named_lock{};
        if (fstat(lock_.value, &held_lock) ||
            fstatat(directory_.value, ".lock", &named_lock, AT_SYMLINK_NOFOLLOW) ||
            !owned_files::same(held_lock, named_lock) || held_lock.st_nlink != 1 ||
            !S_ISREG(held_lock.st_mode) || held_lock.st_uid != geteuid() ||
            (held_lock.st_mode & 0777) != 0600)
            throw std::runtime_error("Recovery lock replaced/changed");
    }
public:
    RecoveryJournal(const std::filesystem::path& path, std::string session, FaultHook fault = {})
        : directory_(open_directory(path)),
          lock_(openat(directory_.value, ".lock", O_RDWR | O_CREAT | O_CLOEXEC | O_NOFOLLOW | O_NONBLOCK, 0600)),
          path_(path), session_(std::move(session)), fault_(std::move(fault)) { try {
        if (!recovery_digest(session_) || lock_.value < 0 ||
            flock(lock_.value, LOCK_EX | LOCK_NB) || fstat(directory_.value, &identity_))
            throw std::runtime_error("Recovery journal exclusion unavailable");
        verify_directory();
        owned_files::Fd parent(owned_files::parent(path_));
        if (fsync(lock_.value) || fsync(directory_.value) || fsync(parent.value))
            throw std::runtime_error("Recovery journal lock durability failed");
        records();
        } catch(const std::exception& e){std::cerr<<"Recovery journal: "<<e.what()<<"; old/unavailable journal does not block run\n";}
    }
    std::vector<RecoveryRecord> records() const { try {
        verify_directory();
        // openat obtains an independent directory offset, unlike dup().
        const int scan_fd = openat(directory_.value, ".", O_RDONLY | O_DIRECTORY | O_CLOEXEC);
        if (scan_fd < 0)
            throw std::runtime_error("Recovery enumeration open failed");
        DIR* scan = fdopendir(scan_fd);
        if (!scan) {
            ::close(scan_fd);
            throw std::runtime_error("Recovery enumeration failed");
        }
        size_t count = 0;
        bool bad = false;
        errno = 0;
        while (auto* entry = readdir(scan)) {
            const std::string item = entry->d_name;
            if (item == "." || item == ".." || item == ".lock")
                continue;
            if (++count > 4096 || item.size() != 11 || item.substr(4) != ".record" ||
                item.substr(0, 4).find_first_not_of("0123456789") != std::string::npos) {
                bad = true;
                break;
            }
        }
        const int scan_error = errno;
        if (closedir(scan) || scan_error || bad)
            throw std::runtime_error("Unexpected/failed recovery directory enumeration");
        std::vector<RecoveryRecord> result;
        std::string previous(64, '0');
        for (size_t i = 1; i <= count; ++i) {
            owned_files::Fd fd(openat(directory_.value, name(i).c_str(), O_RDONLY | O_CLOEXEC | O_NOFOLLOW | O_NONBLOCK));
            struct stat before{}, after{}, named{};
            if (fd.value < 0 || fstat(fd.value, &before) || !S_ISREG(before.st_mode) ||
                before.st_nlink != 1 || before.st_uid != geteuid() || (before.st_mode & 0777) != 0400)
                throw std::runtime_error("Invalid/missing recovery generation");
            const auto bytes = owned_files::read(fd.value, 1024 * 1024 + 512);
            if (fstat(fd.value, &after) ||
                fstatat(directory_.value, name(i).c_str(), &named, AT_SYMLINK_NOFOLLOW) ||
                !owned_files::same(before, after) || !owned_files::same(before, named))
                throw std::runtime_error("Recovery generation changed during readback");
            auto record = recovery_decode(bytes);
            if (record.sequence != i || record.session != session_ || record.previous != previous ||
                (i == 1 && record.kind != "checkpoint" && record.kind != "unmutated") ||
                (record.kind == "unmutated" && (i != 1 || record.payload != "no-sram-write;resources-closed")) ||
                (!result.empty() && result.back().kind == "unmutated"))
                throw std::runtime_error("Recovery generation/session chain mismatch");
            previous = phase5::sha256(bytes);
            result.push_back(std::move(record));
        }
        verify_directory();
        return result;
        } catch(const std::exception& e){std::cerr<<"Recovery journal: "<<e.what()<<"; ignoring unavailable records\n";return {};}
    }
    std::string append(const std::string& kind, const std::string& payload) { try {
        if (poisoned_)
            throw std::runtime_error("Recovery journal retained after failed commit");
        const auto prior = records();
        if (prior.size() >= 4096 || (prior.empty() && kind != "checkpoint" && kind != "unmutated") ||
            (!prior.empty() && prior.back().kind == "unmutated") ||
            (kind == "unmutated" && (!prior.empty() || payload != "no-sram-write;resources-closed")))
            throw std::runtime_error("Recovery checkpoint first/generation cap required");
        RecoveryRecord record{prior.size() + 1, session_, kind,
            prior.empty() ? std::string(64, '0') : phase5::sha256(recovery_encode(prior.back())), payload};
        const auto bytes = recovery_encode(record);
        // Set before the first potentially committed syscall. No retry/resume
        // after an ambiguous write/fsync/readback in this process.
        poisoned_ = true;
        owned_files::Fd fd(openat(directory_.value, name(record.sequence).c_str(),
            O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC | O_NONBLOCK, 0600));
        if (fd.value < 0)
            throw std::runtime_error("Recovery generation exclusive create failed");
        stage("created");
        owned_files::write(fd.value, bytes);
        stage("written");
        if (fchmod(fd.value, 0400))
            throw std::runtime_error("Recovery generation permissions failed");
        stage("protected");
        if (fsync(fd.value))
            throw std::runtime_error("Recovery generation file durability failed");
        stage("file-synced");
        if (fsync(directory_.value))
            throw std::runtime_error("Recovery generation durability failed");
        stage("directory-synced");
        const auto actual = records();
        if (actual.size() != record.sequence || recovery_encode(actual.back()) != bytes)
            throw std::runtime_error("Recovery generation independent readback failed");
        stage("readback");
        poisoned_ = false;
        return phase5::sha256(bytes);
        } catch(const std::exception& e){std::cerr<<"Recovery journal: "<<e.what()<<"; append unavailable, continuing\n";return {};}
    }
};
}
#endif
