// FramePicoTracker 2026-10-31: native session + body (two feet, or pelvis + two feet) panel in the SteamVR dashboard.
// 2026-11-02 (body-calib): two-step body calibration (stand + look ahead, then look down at the trackers), Auto 2 / Auto 3,
// step prompts and per-tracker state from the driver status (/tmp/pico-body-status.txt lines step/result/tracker/note).
// Previous versions: main.cpp.pre-multi-20261031 (body panel), main.cpp.pre-body-20261031 (API/radio-profile panel).
// Interfaces used (no radio access of its own):
//   session  native-port/live-visual.sh (1 tracker) or native-port-multi-20261031/live-visual-multi.sh (PICO_PEERS from
//            frame-app-peers.txt; disabled until the owner approves multi-tracker sessions), started detached (own session,
//            clean SSH-like environment), never signalled; progress from its stdout (/tmp/frame-app-session.out),
//            /tmp/live-run.out and /tmp/pico-diag.log.
//   radio    station_counters lines in /tmp/pico-diag.log (s12 slots, s16 tracker packets, s24 blocked; every 5 s) and,
//            in multi sessions, the per-chip radio dump <root>/radio.bin ([i64 host_ns][112-byte record], chip at 100).
//   tracker  driver lines "pico_fusion <CHIP>: imu pkts ... optical ..." in vrserver.txt (every 2 s per chip).
//   mode     verified radio mode per chip from the session's attempt/live-events.jsonl (diagnostic-proof schema 2).
//   body     driver ack13 files: steamvr-driver/resources/body_roles.txt, /tmp/pico-body-calibrate, /tmp/pico-body-status.txt.
//   trackers SteamVR devices PicoSwift<CHIP> (pose result, battery) and resources/imu_calibration.txt (IMU fusion).
#include "vendor/vr_overlay.h"
#include "openvr.h"
#include <cairo/cairo.h>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <vector>

namespace {
const std::string W = "/home/steamos/pico-ram-work";
const std::string ROLES = W + "/steamvr-driver/resources/body_roles.txt";
const std::string IMU_CAL = W + "/steamvr-driver/resources/imu_calibration.txt";
const std::string SCRIPT = W + "/native-port/live-visual.sh";
// 2026-11-02: 2-3 Alpha 1.3 trackers paired to the Frame: multi-alpha tree (run-multi.sh prepares the sealed root and
// execs live-visual-multi-alpha.sh). Previous: native-port-multi-20261031/live-visual-multi.sh (stock multi4).
const std::string MULTI_DIR = W + "/native-port-mrec-20261103";  // 2026-11-03: IMU-loss fix + per-chip tracking recovery (was native-port-ma-20261102)
const std::string MULTI_SCRIPT = MULTI_DIR + "/run-multi.sh";
const std::string PEERS = W + "/frame-app-peers.txt";
const std::string SESSION_PID = W + "/frame-app-session.pid";
const char *STATUS = "/tmp/pico-body-status.txt";
const char *CAL_REQUEST = "/tmp/pico-body-calibrate";
const char *SESSION_OUT = "/tmp/frame-app-session.out";
// Primary tracker = first "tracker" line of devices.conf (written by pair-tracker.sh); no IDs are compiled in.
std::string firstTarget() {
    const char *env = getenv("PICO_DEVICES");
    std::ifstream f(env ? env : "/home/steamos/pico-ram-work/devices.conf");
    std::string kind, chip, rest;
    while (f >> kind) { if (kind == "tracker" && (f >> chip)) return chip; std::getline(f, rest); }
    return "-";
}
// Owner 2026-11-02: "the app does not let me connect 3 trackers" - multi-tracker sessions enabled.
const bool MULTI_APPROVED = true;
const int PW = 1200, PH = 900;

volatile sig_atomic_t stop = 0, show = 0;
void signalHandler(int s) { if (s == SIGUSR1) show = 1; else stop = 1; }

// Test overrides only (fixtures instead of the live files); unset in normal use.
std::string envOr(const char *k, const std::string &d) { const char *v = std::getenv(k); return v && *v ? v : d; }
std::string diagPath() { return envOr("FRAME_PICO_DIAG", "/tmp/pico-diag.log"); }
std::string vrserverPath() { return envOr("FRAME_PICO_VRSERVER", envOr("HOME", "/home/steamos") + "/.local/share/Steam/logs/vrserver.txt"); }

std::string readFile(const std::string &p, size_t limit = 1 << 20) {
    std::ifstream f(p, std::ios::binary); if (!f) return {};
    std::string s(limit, '\0'); f.read(&s[0], limit); s.resize(size_t(f.gcount())); return s;
}
std::string readTail(const std::string &p, size_t n) {
    std::ifstream f(p, std::ios::binary | std::ios::ate); if (!f) return {};
    const long long size = f.tellg(), from = size > (long long)n ? size - (long long)n : 0;
    std::string s(size_t(size - from), '\0'); f.seekg(from); f.read(&s[0], s.size()); return s;
}
std::vector<std::string> splitLines(const std::string &s) {
    std::vector<std::string> v; std::istringstream in(s); std::string l;
    while (std::getline(in, l)) { if (!l.empty() && l.back() == '\r') l.pop_back(); v.push_back(l); }
    return v;
}
bool mtimeOf(const std::string &p, time_t &t) { struct stat st{}; if (stat(p.c_str(), &st)) return false; t = st.st_mtime; return true; }
std::string upper(std::string s) { for (auto &c : s) c = char(std::toupper((unsigned char)c)); return s; }
std::string hhmmss(time_t t) { char b[16]; std::strftime(b, sizeof b, "%H:%M:%S", std::localtime(&t)); return b; }
bool startsWith(const std::string &s, const char *p) { return s.rfind(p, 0) == 0; }
bool endsWith(const std::string &s, const std::string &e) { return s.size() >= e.size() && s.compare(s.size() - e.size(), e.size(), e) == 0; }
std::string baseName(const std::string &p) { return p.substr(p.find_last_of('/') + 1); }
long long monoNs() { struct timespec ts{}; clock_gettime(CLOCK_MONOTONIC, &ts); return ts.tv_sec * 1000000000LL + ts.tv_nsec; }
double realNow() { struct timespec ts{}; clock_gettime(CLOCK_REALTIME, &ts); return ts.tv_sec + ts.tv_nsec * 1e-9; }
std::string ago(double s) {
    char b[32];
    if (s < 90) std::snprintf(b, sizeof b, "%.0f s", s < 0 ? 0 : s); else if (s < 5400) std::snprintf(b, sizeof b, "%.0f min", s / 60);
    else std::snprintf(b, sizeof b, "%.0f h", s / 3600);
    return b;
}
std::string fmt(const char *f, double v) { char b[32]; std::snprintf(b, sizeof b, f, v); return b; }

// Whole new lines of a growing log since the last call; a partial last line is read again next time. A replaced or
// truncated file (SteamVR restart, new session log) starts over.
struct LineTail {
    std::string path; std::streamoff offset = 0; ino_t ino = 0;
    template <class R, class F> void update(const std::string &p, R &&onReset, F &&each) {
        struct stat st{}; if (stat(p.c_str(), &st)) return;
        if (p != path || st.st_ino != ino || st.st_size < offset) { path = p; ino = st.st_ino; offset = 0; onReset(); }
        std::ifstream f(p, std::ios::binary); if (!f) return;
        f.seekg(offset); std::string line; std::streamoff budget = 16 << 20;
        while (budget > 0) {
            const std::streamoff at = f.tellg();
            if (!std::getline(f, line) || f.eof()) { offset = at; break; }
            offset = f.tellg(); budget -= offset - at; each(line);
        }
    }
};

// ---- body model files (driver ack13) ----
struct RolesFile { bool exists = false; int autoN = 0; std::map<std::string, std::string> roles; };
RolesFile readRoles() {
    RolesFile r; std::ifstream f(ROLES); if (!f) return r;
    r.exists = true; std::string l;
    while (std::getline(f, l)) {  // same grammar as LoadBodyRoles in driver_pico_tracker.cpp
        if (l.empty() || l[0] == '#') continue;
        unsigned chip; char role[32]; int n;
        if (std::sscanf(l.c_str(), "auto %d", &n) == 1) { r.autoN = (n >= 2 && n <= 4) ? n : 0; continue; }
        if (std::sscanf(l.c_str(), "%x %31s", &chip, role) == 2) { char c[16]; std::snprintf(c, sizeof c, "%08X", chip); r.roles[c] = role; }
    }
    return r;
}
struct BodyTracker { std::string role, position, mount; int imu = 0, still = 0, optics = 0, seen = 0; };
struct BodyStatus {
    bool present = false; long age = 0; int active = 0, autoN = 0, calibrated = 0; std::map<std::string, std::string> roles;
    // driver 2026-11-02 (two-step calibration); empty with an older driver
    std::string step, result, note; double stepS = 0, left = 0; int guessed = 0, expected = 0;
    std::map<std::string, BodyTracker> trackers;
};
BodyStatus readStatus() {
    BodyStatus b; time_t t; if (!mtimeOf(STATUS, t)) return b;
    const auto lines = splitLines(readFile(STATUS, 16384)); if (lines.empty()) return b;
    std::istringstream in(lines[0]); std::string w;  // line 1: ack13 grammar
    if (!(in >> w >> b.active >> w >> b.autoN >> w >> b.calibrated >> w)) return b;
    b.present = true; b.age = long(std::time(nullptr) - t);
    while (in >> w) { auto eq = w.find('='); if (eq != std::string::npos) b.roles[upper(w.substr(0, eq))] = w.substr(eq + 1); }
    for (size_t i = 1; i < lines.size(); ++i) {
        const auto &l = lines[i]; std::istringstream ls(l); std::string k; ls >> k;
        if (k == "step") ls >> b.step >> b.stepS >> w >> b.left;
        else if (k == "result") ls >> b.result >> w >> b.guessed >> w >> b.expected;
        else if (k == "note") b.note = l.size() > 5 ? l.substr(5) : "";
        else if (k == "tracker") {
            std::string chip; BodyTracker bt;
            ls >> chip >> w >> bt.role >> w >> bt.imu >> w >> bt.still >> w >> bt.optics >> w >> bt.seen >> w >> bt.position >> w >> bt.mount;
            b.trackers[upper(chip)] = bt;
        }
    }
    return b;
}
// The driver reloads body_roles.txt when its mtime (seconds) changes, so two writes in one second must still differ.
std::string writeRoles(const std::string &content) {
    const std::string tmp = ROLES + ".app-tmp";
    { std::ofstream f(tmp, std::ios::trunc); if (!f) return "Cannot write " + tmp; f << content; if (!f) return "Cannot write " + tmp; }
    time_t old = 0; mtimeOf(ROLES, old);
    const time_t t = std::max(std::time(nullptr), old + 1);
    struct timespec ts[2] = {{t, 0}, {t, 0}}; utimensat(AT_FDCWD, tmp.c_str(), ts, 0);
    if (std::rename(tmp.c_str(), ROLES.c_str())) return "Cannot replace " + ROLES;
    return {};
}
std::string stamp() { time_t t = std::time(nullptr); char b[32]; std::strftime(b, sizeof b, "%Y-%m-%d %H:%M:%S", std::localtime(&t)); return b; }
std::string bodyAction(const std::string &a) {
    if (a == "off") { auto e = writeRoles("# body model off (FramePicoTracker " + stamp() + ")\n"); return e.empty() ? "Body model off: every tracker is published on its own." : e; }
    if (a == "auto" || a == "auto3") { auto e = writeRoles("auto 3\n"); return e.empty() ? "Body auto: pelvis + two feet. Now press Calibrate (2 steps)." : e; }
    if (a == "auto2") { auto e = writeRoles("auto 2\n"); return e.empty() ? "Body auto: two feet. Now press Calibrate (2 steps)." : e; }
    const auto rf = readRoles();
    if (a == "calibrate") {
        if (rf.autoN == 0 && rf.roles.empty()) return "Body model is off: choose Auto 2 (feet) or Auto 3 (pelvis + feet) first.";
        int fd = open(CAL_REQUEST, O_CREAT | O_WRONLY, 0644); if (fd < 0) return std::string("Cannot create ") + CAL_REQUEST; close(fd);
        return "Step 1 of 2: stand straight, look ahead, hold still; then look down at your trackers.";
    }
    if (a == "swap") {
        auto roles = readStatus().roles; if (roles.empty()) roles = rf.roles;
        std::string left, right;
        for (auto &kv : roles) { if (kv.second == "left_foot") left = kv.first; if (kv.second == "right_foot") right = kv.first; }
        if (left.empty() || right.empty()) return "Swap needs both feet assigned: calibrate first.";
        roles[left] = "right_foot"; roles[right] = "left_foot";
        std::string c = "# roles fixed by FramePicoTracker (left/right swapped) " + stamp() + "\n";
        for (auto &kv : roles) c += kv.first + " " + kv.second + "\n";
        auto e = writeRoles(c);
        return e.empty() ? "Left/right swapped. Press Calibrate (2 steps) again: stand straight looking ahead, then look down." : e;
    }
    return "Unknown body action " + a;
}

// ---- multi-tracker peer table (owned by the app, editable) ----
// One line per module peer index (0..3, list order = PICO_PEERS order): CHIP SLOT SERIAL; CHIP "-" = not known yet
// (that index stays a "-" placeholder, the tracker cannot join). Grammar of PICO_PEERS: native/peer_table.hpp.
struct Peer { std::string chip, slot, serial; };
const char *DEFAULT_PEERS =
    "# FramePicoTracker multi-tracker peers (2026-10-31). One line per module peer index 0..3, in order:\n"
    "#   CHIP SLOT SERIAL     CHIP = 8 hex digits, or - if not known yet (that index is sent as \"-\").\n"
    "# PICO_PEERS for live-visual-multi.sh is built from these lines (CHIP:SLOT:SERIAL,...).\n"
    "# (empty until pair-tracker.sh pairs a tracker)\n";
std::vector<Peer> readPeers() {
    if (access(PEERS.c_str(), F_OK)) { std::ofstream f(PEERS); f << DEFAULT_PEERS; }
    std::vector<Peer> v;
    for (auto &l : splitLines(readFile(PEERS, 8192))) {
        if (l.empty() || l[0] == '#') continue;
        std::istringstream in(l); Peer p; if (!(in >> p.chip)) continue; in >> p.slot >> p.serial;
        p.chip = p.chip == "-" ? "" : upper(p.chip);
        if (v.size() < 4) v.push_back(p);
    }
    return v;
}
std::string peersEnv(const std::vector<Peer> &peers) {
    std::string s;
    for (auto &p : peers) {
        s += s.empty() ? "" : ",";
        if (p.chip.empty()) { s += "-"; continue; }
        s += p.chip; if (!p.slot.empty()) { s += ":" + p.slot; if (!p.serial.empty()) s += ":" + p.serial; }
    }
    return s;
}

// ---- processes ----
struct Procs { int script = 0, producer = 0, openocd = 0; bool multi = false; };
Procs scanProcs() {
    Procs p; DIR *d = opendir("/proc"); if (!d) return p;
    while (auto *e = readdir(d)) {
        if (!std::isdigit((unsigned char)e->d_name[0])) continue;
        const int pid = std::atoi(e->d_name);
        const std::string base = std::string("/proc/") + e->d_name;
        const auto cmd = readFile(base + "/cmdline", 8192);
        std::vector<std::string> argv; size_t s = 0;
        for (size_t i = 0; i < cmd.size(); ++i) if (!cmd[i]) { argv.push_back(cmd.substr(s, i - s)); s = i + 1; }
        for (auto &a : argv) {
            if (endsWith(a, "live-visual.sh")) p.script = pid;
            if (endsWith(a, "live-visual-multi.sh") || endsWith(a, "live-visual-multi-alpha.sh") || endsWith(a, "run-multi.sh")) { p.script = pid; p.multi = true; }
        }
        if (!argv.empty() && endsWith(argv[0], "/producer") && std::find(argv.begin(), argv.end(), "--live") != argv.end()) p.producer = pid;
        auto comm = readFile(base + "/comm", 64); if (!comm.empty() && comm.back() == '\n') comm.pop_back();
        if (comm == "openocd") p.openocd = pid;
    }
    closedir(d); return p;
}

// Starts the session script exactly as over SSH: own session (double fork, survives this app), stdout/stderr to
// SESSION_OUT, no inherited descriptors (the UI lock!) and no inherited dashboard environment (SteamVR children carry
// LD_PRELOAD=libxrtap.so). The app never signals the session; it ends after its own duration and cleanup.
// The one addition to the SSH environment is PICO_STATION_COUNTERS=1 (one station_counters line every 5 s, which the
// multi script always sets): the panel's radio rate and its "tracker went silent" detector read those lines.
std::string startSession(bool multi) {
    const auto p = scanProcs();
    if (p.script || p.producer || p.openocd)
        return "A radio session is already running (script " + std::to_string(p.script) + ", producer " + std::to_string(p.producer) +
               ", openocd " + std::to_string(p.openocd) + "). Wait for live exit.";
    if (multi && !MULTI_APPROVED) return "3-tracker sessions are off until the owner approves multi-tracker sessions.";
    const std::string peers = multi ? peersEnv(readPeers()) : "";
    if (multi && peers.find_first_not_of("-,") == std::string::npos) return "No known chip in " + PEERS;
    const char *over = std::getenv("FRAME_PICO_SESSION_SCRIPT");  // tests only: a dummy script instead of the radio session
    const std::string script = over && *over ? over : multi ? MULTI_SCRIPT : SCRIPT;
    if (access(script.c_str(), R_OK)) return "Cannot read " + script;
    const char *home = std::getenv("HOME");
    std::vector<std::string> env = {std::string("HOME=") + (home ? home : "/home/steamos"), "USER=steamos", "LOGNAME=steamos",
                                    "SHELL=/bin/bash", "PATH=/usr/local/sbin:/usr/local/bin:/usr/bin", "PICO_STATION_COUNTERS=1"};
    if (multi) env.push_back("PICO_PEERS=" + peers);
    std::vector<char *> envp; for (auto &e : env) envp.push_back(&e[0]); envp.push_back(nullptr);
    std::string sh = "sh", sc = script; char *argv[] = {&sh[0], &sc[0], nullptr};
    const pid_t c = fork();
    if (c < 0) return "fork failed";
    if (c == 0) {
        setsid();
        const pid_t g = fork();
        if (g != 0) _exit(g < 0 ? 1 : 0);
        int out = open(SESSION_OUT, O_WRONLY | O_CREAT | O_TRUNC, 0644); if (out >= 0) { dup2(out, 1); dup2(out, 2); }
        int in = open("/dev/null", O_RDONLY); if (in >= 0) dup2(in, 0);
        int pf = open(SESSION_PID.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (pf >= 0) {  // "pid start_epoch\n", formatted without stdio after fork
            char b[48]; int n = 0; long long v[2] = {getpid(), (long long)std::time(nullptr)};
            for (int k = 0; k < 2; ++k) { char t[24]; int m = 0; long long x = v[k]; do { t[m++] = char('0' + x % 10); x /= 10; } while (x); while (m) b[n++] = t[--m]; b[n++] = k ? '\n' : ' '; }
            if (write(pf, b, n) < 0) {}
            close(pf);
        }
        closefrom(3);
        if (chdir(W.c_str())) {}
        execve("/bin/sh", argv, envp.data());
        _exit(127);
    }
    int status = 0; waitpid(c, &status, 0);
    if (!WIFEXITED(status) || WEXITSTATUS(status)) return "Session launch failed";
    return "Session started (" + baseName(script) + (multi ? ", PICO_PEERS=" + peers : "") + "). It ends by itself after its duration and restores Roy SRAM.";
}

// ---- radio and driver logs ----
struct Counter { long long t = 0, slots = 0, packets = 0, blocked = 0; };
struct CounterScanner {  // station_counters t=<mono ns> s12=<slots> s16=<tracker packets> s20=.. s24=<blocked>
    LineTail tail; std::vector<Counter> all;
    void update() {
        tail.update(diagPath(), [&] { all.clear(); }, [&](const std::string &l) {
            if (!startsWith(l, "station_counters ")) return;
            Counter c; long long s20;
            if (std::sscanf(l.c_str(), "station_counters t=%lld s12=%lld s16=%lld s20=%lld s24=%lld", &c.t, &c.slots, &c.packets, &s20, &c.blocked) == 5) all.push_back(c);
        });
    }
};
struct Fusion { double at = 0, prevAt = 0, optChangedAt = 0, imuAgeMs = -1; unsigned long long imu = 0, prevImu = 0, opt = 0, prevOpt = 0; int lines = 0; };
struct FusionScanner {  // driver: "<date> [Info] - pico_tracker: pico_fusion A1B2C3D4: imu pkts .. | fusion imu N .. optical N .. imuAge ms"
    LineTail tail; std::map<std::string, Fusion> chips;
    void update() {
        tail.update(vrserverPath(), [&] { chips.clear(); }, [&](const std::string &l) {
            const auto p = l.find("pico_fusion "); if (p == std::string::npos) return;
            char chip[9] = {}; unsigned long long pk, st, cr, imu, late, reinit, opt;
            if (std::sscanf(l.c_str() + p, "pico_fusion %8[0-9A-Fa-f]: imu pkts %llu stale %llu clockRej %llu | fusion imu %llu late %llu reinit %llu optical %llu",
                            chip, &pk, &st, &cr, &imu, &late, &reinit, &opt) != 8) return;
            struct tm tm{}; const char *rest = strptime(l.c_str(), "%a %b %d %Y %H:%M:%S", &tm); if (!rest) return;
            tm.tm_isdst = -1; const double t = double(mktime(&tm)) + (*rest == '.' ? std::atof(rest) : 0);
            auto &f = chips[upper(chip)];
            if (f.lines && opt < f.opt) f = Fusion{};  // driver restarted inside the same log
            if (opt != f.opt || (!f.lines && opt)) f.optChangedAt = t;
            f.prevAt = f.at; f.prevImu = f.imu; f.prevOpt = f.opt; f.at = t; f.imu = imu; f.opt = opt; ++f.lines;
            const auto a = l.find("imuAge "); f.imuAgeMs = a == std::string::npos ? -1 : std::atof(l.c_str() + a + 7);
        });
    }
};
struct DumpChip { long long first = 0, last = 0, count = 0; std::vector<long long> recent; };
struct DumpScanner {  // <root>/radio.bin of multi sessions: [i64 host_ns][112-byte record], record chip word at 100
    std::string path; std::streamoff offset = 0; long long last = 0; std::map<std::string, DumpChip> chips;
    void update(const std::string &root) {
        const std::string p = root.empty() ? "" : root + "/radio.bin";
        if (p != path) { path = p; offset = 0; last = 0; chips.clear(); }
        std::ifstream f(p, std::ios::binary); if (!f) return;
        f.seekg(0, std::ios::end); if (f.tellg() < offset) { offset = 0; last = 0; chips.clear(); }
        f.seekg(offset); char b[120]; long long budget = 64 << 20;
        while (budget > 0 && f.read(b, 120)) {
            offset += 120; budget -= 120;
            long long t; uint32_t chip; std::memcpy(&t, b, 8); std::memcpy(&chip, b + 8 + 100, 4);
            if (!chip) continue;
            char c[16]; std::snprintf(c, sizeof c, "%08X", chip);
            auto &d = chips[c]; if (!d.first) d.first = t; d.last = t; ++d.count; d.recent.push_back(t); last = std::max(last, t);
        }
        for (auto &kv : chips) { auto &r = kv.second.recent; r.erase(r.begin(), std::lower_bound(r.begin(), r.end(), last - 5000000000LL)); }
    }
};

// ---- session progress ----
struct SessionView {
    bool running = false, byApp = false, multi = false; Procs procs; time_t start = 0; int duration = 150; long long startNs = 0;
    std::string root; std::vector<std::string> lines;
    std::vector<Counter> counters;  // this session's station_counters lines
    bool countersLive = false; double slotsPerS = -1, packetsPerS = -1;
};
std::string newestRoot() {
    std::string best; time_t bt = 0;
    for (const std::string &dir : {W + "/native-port", MULTI_DIR}) {
        DIR *d = opendir(dir.c_str()); if (!d) continue;
        while (auto *e = readdir(d)) {
            if (!startsWith(e->d_name, "native-live-")) continue;
            time_t t; if (mtimeOf(dir + "/" + e->d_name, t) && t >= bt) { bt = t; best = dir + "/" + e->d_name; }
        }
        closedir(d);
    }
    return best;
}
std::string diagSummary(const std::string &l) {  // optical_worker.cpp optical_diag line -> short TRACKING summary
    auto get = [&](const char *k) { auto p = l.find(std::string(" ") + k + "="); if (p == std::string::npos) return std::string("?"); p += std::strlen(k) + 2; return l.substr(p, l.find(' ', p) - p); };
    return "fits " + get("fits") + " stereo " + get("stereo_fits") + " lower " + get("lower_fits") + " id_ok " + get("identity_ok") + " phase " + get("phase_state");
}
long long sessionStartNs(const std::string &root) {  // first live event (invocation), same monotonic clock as the counters
    const auto s = readFile(root + "/attempt/live-events.jsonl", 4096); const auto p = s.find("\"t_ns\":");
    return p == std::string::npos ? 0 : std::atoll(s.c_str() + p + 7);
}
SessionView readSession(const CounterScanner &cs) {
    SessionView s; s.procs = scanProcs(); s.running = s.procs.script || s.procs.producer;
    {
        std::istringstream in(readFile(SESSION_PID, 256)); long long pid = 0, t = 0;
        if (in >> pid >> t) { s.byApp = s.running && pid == s.procs.script; if (s.byApp) s.start = time_t(t); }
    }
    if (s.running && !s.byApp) { time_t t; if (mtimeOf("/proc/" + std::to_string(s.procs.script ? s.procs.script : s.procs.producer), t)) s.start = t; }
    auto duration = [&](bool multi) {
        const auto sc = readFile(multi ? MULTI_SCRIPT : SCRIPT, 1 << 16); auto p = sc.find("--duration ");
        if (p != std::string::npos) s.duration = std::atoi(sc.c_str() + p + 11);
    };
    duration(s.procs.multi);
    const time_t now = std::time(nullptr);
    if (s.running) {
        char b[160]; std::snprintf(b, sizeof b, "RUNNING  %ld / %d s  (%s%s)", long(now - s.start), s.duration, s.procs.multi ? "multi, " : "",
                                   s.byApp ? "started here" : "started outside the app, e.g. over SSH");
        s.lines.push_back(b);
    } else if (s.procs.openocd) {
        s.lines.push_back("openocd " + std::to_string(s.procs.openocd) + " runs without a session script: Roy owner present.");
    }
    // The app's last session output, unless a newer session (e.g. over SSH) replaced it.
    time_t outT = 0; const bool haveOut = mtimeOf(SESSION_OUT, outT);
    const std::string newest = envOr("FRAME_PICO_ROOT", newestRoot());
    std::vector<std::string> outLines; std::string outRoot;
    if (haveOut && (s.byApp || !s.running))
        for (auto &l : splitLines(readFile(SESSION_OUT, 1 << 16))) {
            if (startsWith(l, "root=")) outRoot = l.substr(5);
            else if (startsWith(l, "controller ") || startsWith(l, "WARNING") || startsWith(l, "prep failed") || startsWith(l, "SESSION_FAILED") ||
                     startsWith(l, "live exit") || startsWith(l, "multi observables")) outLines.push_back(l);
        }
    time_t outRootT = 0, newestT = 0;
    const bool outCurrent = haveOut && (s.byApp || outRoot.empty() || !mtimeOf(newest, newestT) || (mtimeOf(outRoot, outRootT) && outRootT + 2 >= newestT));
    if (outCurrent && haveOut && (s.byApp || !s.running)) {
        if (!s.running) s.lines.push_back("Last session started here: output " + hhmmss(outT));
        if (!outRoot.empty()) { s.root = outRoot; s.lines.push_back("root " + baseName(s.root)); }
        s.lines.insert(s.lines.end(), outLines.begin(), outLines.end());
    }
    if (s.root.empty()) { s.root = newest; if (!s.root.empty() && !s.running) s.lines.push_back("Last session " + baseName(s.root) + " (started outside the app)"); }
    s.multi = startsWith(s.root, (MULTI_DIR + "/").c_str());
    if (!s.running && s.multi) duration(true);
    // Station counters of this session: from its first event on, until a gap of more than 30 s (a later session).
    s.startNs = s.root.empty() ? 0 : sessionStartNs(s.root);
    if (s.startNs)
        for (auto &c : cs.all) {
            if (c.t < s.startNs) continue;
            if (!s.counters.empty() && c.t - s.counters.back().t > 30000000000LL) break;
            s.counters.push_back(c);
        }
    const size_t n = s.counters.size();
    if (n >= 2) {
        const auto &a = s.counters[n - 2], &b = s.counters[n - 1]; const double dt = (b.t - a.t) * 1e-9;
        s.countersLive = monoNs() - b.t < 12000000000LL && dt > 0;
        if (s.countersLive) { s.slotsPerS = (b.slots - a.slots) / dt; s.packetsPerS = (b.packets - a.packets) / dt; }
    }
    time_t dt = 0;
    if (mtimeOf(diagPath(), dt) && (!s.running || dt + 2 >= s.start)) {
        std::string last;
        for (auto &l : splitLines(readTail(diagPath(), 4096))) if (startsWith(l, "optical_diag")) last = l;
        if (!last.empty()) s.lines.push_back(std::string(s.running ? "TRACKING " : "last optics ") + hhmmss(dt) + "  " + diagSummary(last));
    }
    time_t rt = 0;
    if (s.running && mtimeOf("/tmp/live-run.out", rt) && rt + 2 >= s.start) {
        std::string last, tracking;
        for (auto &l : splitLines(readTail("/tmp/live-run.out", 65536))) {
            if (startsWith(l, "TRACKING")) tracking = l;
            if (l.empty() || l[0] == '{' || l.find("Valve controller connected") != std::string::npos || startsWith(l, "filesystem error")) continue;
            last = l;
        }
        if (!tracking.empty()) s.lines.push_back(tracking);
        if (!last.empty()) s.lines.push_back("producer: " + last);
    }
    return s;
}

// Hang detector (docs/TRACKER_HANG_DIAGNOSTIC10_2026-10-30.md): the tracker went silent when station_counters s16
// (tracker packets) stops while s12 (slots) keeps growing, for two 5-s lines in a row or up to the session's last line.
// A hung tracker still adds a stray packet now and then (T1 224749: +7 in 100 s against ~270 per 5 s before), so a
// 5-s line with at most 2 new packets counts as silent. Times are seconds after radio start (first counter line).
// Multi sessions also check each chip in radio.bin (one tracker can stop while the others keep s16 growing).
std::vector<std::string> hangLines(const SessionView &s, const DumpScanner &ds) {
    std::vector<std::string> out;
    if (s.root.empty()) return out;
    const long long mono = monoNs(); const double real = realNow();
    const auto &c = s.counters;
    const long long radioStart = c.empty() ? s.startNs : c.front().t;
    auto rel = [&](long long t) { return (t - radioStart) * 1e-9; };
    auto wall = [&](long long t) { return hhmmss(time_t(real - (mono - t) * 1e-9)); };
    auto quiet = [&](size_t k) { return c[k].packets - c[k - 1].packets <= 2 && c[k].slots - c[k - 1].slots > 50; };
    if (c.empty()) out.push_back(s.running ? "station counters: waiting for the first line (every 5 s)"
                                           : "no station counters in this session (started without PICO_STATION_COUNTERS): hang detector off");
    for (size_t i = 1; i < c.size(); ++i) {
        if (!quiet(i) || c[i - 1].packets == 0) continue;
        size_t j = i; while (j + 1 < c.size() && quiet(j + 1)) ++j;
        if (j == i && j + 1 < c.size()) { i = j; continue; }  // one quiet 5-s line in the middle: not a hang
        // The last replies fall into the line before the first quiet one: onset between c[i-2] and c[i-1].
        char b[220]; const long long from = c[i >= 2 ? i - 2 : 0].t, to = c[i - 1].t;
        std::snprintf(b, sizeof b, "SILENT: tracker went silent at +%.0f..%.0f s of radio (by %s)", rel(from), rel(to), wall(to).c_str());
        std::string l = b;
        if (j + 1 < c.size()) l += ", answered again by +" + fmt("%.0f", rel(c[j + 1].t)) + " s";
        else if (s.running) l += " (still silent)";
        out.push_back(l);
        out.push_back("SILENT: tracker stopped answering (hang?): report it with the TX journal");
        break;
    }
    if (s.multi && !ds.chips.empty() && !c.empty())
        for (auto &kv : ds.chips) {
            if (c.back().t - kv.second.last <= 10000000000LL) continue;  // replies until within 10 s of the last counter line
            out.push_back("SILENT: " + kv.first + " last reply at +" + fmt("%.0f", rel(kv.second.last)) + " s of radio (" + wall(kv.second.last) + "), slots continued");
        }
    const std::string tx = "/tmp/pico-tx-" + baseName(s.root) + ".log"; struct stat st{};
    out.push_back(stat(tx.c_str(), &st) ? "TX journal: none for this session" : "TX journal: " + tx + " (" + std::to_string(st.st_size / 1024) + " KB)");
    return out;
}

// Verified radio mode per chip from the session events (diagnostic-proof, schema 2: body[22] = actual radio mode).
struct ModeScanner {
    std::string path, rootName; std::streamoff offset = 0; std::map<std::string, int> modes;
    void update(const std::string &root) {
        if (root.empty()) return;
        const std::string p = root + "/attempt/live-events.jsonl";
        if (p != path) { path = p; offset = 0; modes.clear(); rootName = root.substr(root.find_last_of('/') + 1); }
        std::ifstream f(p, std::ios::binary); if (!f) return;
        f.seekg(0, std::ios::end); const std::streamoff size = f.tellg();
        if (size < offset) { offset = 0; modes.clear(); }
        f.seekg(offset); std::string line; std::streamoff budget = 8 << 20;
        while (budget > 0) {
            const std::streamoff at = f.tellg();
            if (!std::getline(f, line) || f.eof()) { offset = at; break; }  // a partial last line is read again next time
            offset = f.tellg(); budget -= offset - at;
            if (line.find("diagnostic-proof") == std::string::npos) continue;
            const auto m = line.find("actual_radio_mode\\\":"), h = line.find("body_hex\\\":\\\"fe02");
            if (m == std::string::npos || h == std::string::npos) continue;
            const std::string hex = line.substr(h + 13, 52);
            if (hex.size() < 46) continue;
            const std::string le = hex.substr(36, 8);  // little-endian chip bytes, SteamVR serial uses the reversed order
            modes[upper(le.substr(6, 2) + le.substr(4, 2) + le.substr(2, 2) + le.substr(0, 2))] = std::atoi(line.c_str() + m + 20);
        }
    }
    std::string sessionTime() const {  // native-live-YYYYMMDD-HHMMSS
        return rootName.size() >= 27 ? rootName.substr(21, 2) + ":" + rootName.substr(23, 2) : rootName;
    }
};

// ---- trackers ----
struct Tracker {
    std::string chip, serial, role = "-"; bool inVr = false, connected = false, valid = false, imuCal = false;
    int result = 0, battery = -1, mode = -1; double radio = -1;  // replies/s while a session's counters are live
};
struct Scanners { ModeScanner modes; CounterScanner counters; FusionScanner fusion; DumpScanner dump; };
std::vector<Tracker> readTrackers(bool vr, const BodyStatus &body, const RolesFile &rf, const SessionView &s, const std::vector<Peer> &peers, const Scanners &sc) {
    std::map<std::string, Tracker> m;
    auto at = [&](const std::string &chip) -> Tracker & { auto &t = m[chip]; t.chip = chip; return t; };
    for (auto &l : splitLines(readFile(IMU_CAL, 65536))) {
        if (l.empty() || l[0] == '#') continue;
        unsigned chip; double r0; if (std::sscanf(l.c_str(), "%x %lf", &chip, &r0) != 2) continue;
        char c[16]; std::snprintf(c, sizeof c, "%08X", chip); at(c).imuCal = true;
    }
    for (auto &p : peers) { if (p.chip.empty()) at("?" + p.serial).serial = p.serial; else at(p.chip).serial = p.serial; }
    for (auto &kv : rf.roles) at(kv.first).role = kv.second;
    for (auto &kv : body.roles) at(kv.first).role = kv.second;  // the driver's current assignment wins
    for (auto &kv : sc.modes.modes) at(kv.first).mode = kv.second;
    for (auto &kv : sc.fusion.chips) at(kv.first);
    if (s.countersLive) {
        if (s.multi) { for (auto &kv : sc.dump.chips) at(kv.first).radio = kv.second.recent.size() / 5.0; }
        else at(firstTarget()).radio = s.packetsPerS;
    }
    if (vr && vr::VRSystem()) {
        auto *sys = vr::VRSystem();
        vr::TrackedDevicePose_t poses[vr::k_unMaxTrackedDeviceCount];
        sys->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0, poses, vr::k_unMaxTrackedDeviceCount);
        for (unsigned i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i) {
            if (sys->GetTrackedDeviceClass(i) != vr::TrackedDeviceClass_GenericTracker) continue;
            char serial[64] = {}; sys->GetStringTrackedDeviceProperty(i, vr::Prop_SerialNumber_String, serial, sizeof serial);
            std::string s = serial; if (!startsWith(s, "PicoSwift") || s.size() < 17) continue;
            auto &t = at(upper(s.substr(9)));
            t.inVr = true; t.connected = poses[i].bDeviceIsConnected; t.valid = poses[i].bPoseIsValid; t.result = poses[i].eTrackingResult;
            if (sys->GetBoolTrackedDeviceProperty(i, vr::Prop_DeviceProvidesBatteryStatus_Bool))
                t.battery = int(sys->GetFloatTrackedDeviceProperty(i, vr::Prop_DeviceBatteryPercentage_Float) * 100 + .5f);
        }
    }
    std::vector<Tracker> v; for (auto &kv : m) v.push_back(kv.second); return v;
}

struct Model {
    bool vr = false; std::vector<Tracker> trackers; BodyStatus body; RolesFile roles; SessionView session; std::vector<Peer> peers;
    const Scanners *sc = nullptr;
};
Model gather(bool vr, Scanners &sc) {
    Model m; m.vr = vr; m.body = readStatus(); m.roles = readRoles(); m.peers = readPeers();
    sc.counters.update(); sc.fusion.update();
    m.session = readSession(sc.counters);
    sc.modes.update(m.session.root); sc.dump.update(m.session.multi ? m.session.root : "");
    const auto h = hangLines(m.session, sc.dump); m.session.lines.insert(m.session.lines.end(), h.begin(), h.end());
    m.sc = &sc; m.trackers = readTrackers(vr, m.body, m.roles, m.session, m.peers, sc); return m;
}

// Table cells of one tracker (panel and --status print the same text). good: 1 green, 0 normal, -1 orange.
struct Cell { std::string text; int good = 0; };
const char *HEAD[] = {"chip", "role", "SteamVR", "optics (driver)", "radio", "IMU", "calibration", "radio mode", "battery"};
const double COL[] = {40, 165, 275, 455, 600, 680, 795, 885, 1040}, COLW[] = {120, 105, 175, 140, 75, 110, 85, 150, 120};
std::vector<Cell> cells(const Tracker &t, const Model &m) {
    std::vector<Cell> v;
    v.push_back({t.chip[0] == '?' ? "unknown" : t.chip, 0});
    v.push_back({t.role, 0});
    std::string vrs = !m.vr ? "(no SteamVR here)" : !t.inVr ? "not in SteamVR yet" : !t.connected ? "disconnected" :
                      t.valid && t.result == vr::TrackingResult_Running_OK ? "tracking" :
                      t.result == vr::TrackingResult_Fallback_RotationOnly ? "rotation only (IMU)" : "not tracked (" + std::to_string(t.result) + ")";
    v.push_back({vrs, t.inVr && t.valid && t.result == vr::TrackingResult_Running_OK ? 1 : 0});
    // Driver counters (vrserver.txt, one line per chip every 2 s): optical fixes and IMU samples fused.
    const auto fi = m.sc->fusion.chips.find(t.chip);
    const double now = realNow();
    if (fi == m.sc->fusion.chips.end()) { v.push_back({"-", 0}); v.push_back({}); v.push_back({"-", 0}); }
    else {
        const auto &f = fi->second; const double span = f.at - f.prevAt;
        const bool fresh = f.lines >= 2 && now - f.at < 6 && span > 0 && span < 10;
        const double optRate = fresh ? (f.opt - f.prevOpt) / span : 0, imuRate = fresh ? (f.imu - f.prevImu) / span : 0;
        if (optRate > 0) v.push_back({"fresh " + fmt("%.0f", optRate) + "/s", 1});
        else if (f.opt) v.push_back({"fix " + ago(now - f.optChangedAt) + " ago", 0});
        else v.push_back({"no fix yet", 0});
        v.push_back({});
        if (imuRate > 0) v.push_back({fmt("%.0f", imuRate) + "/s", 1});
        else if (!fresh) v.push_back({"- (no log)", 0});
        else v.push_back({f.imuAgeMs >= 0 ? "none " + ago(f.imuAgeMs / 1000) : "none", 0});
        // The driver keeps the last pose with result 200 after the data stops (seen 2026-10-31: "tracking" 17 min after
        // the session), so a pose without IMU or optics in the last driver line is shown as held, not as tracking.
        if (fresh && optRate <= 0 && imuRate <= 0 && t.inVr && t.valid) v[2] = {"held pose (no data)", -1};
    }
    // Radio: tracker replies per second from the session's station counters (multi: per chip from radio.bin).
    v[4] = !m.session.countersLive ? Cell{"-", 0} : t.radio < 0 ? Cell{"-", 0} : t.radio < 0.5 ? Cell{"silent", -1} : Cell{fmt("%.0f", t.radio) + "/s", 1};
    v.push_back(t.imuCal ? Cell{"own", 1} : Cell{"shared", -1});  // driver ack14+: no own line = the first line's calibration
    v.push_back(t.mode < 0 ? Cell{"not queried yet", 0} :
                Cell{(t.mode == 1 ? "Game 1" : t.mode == 0 ? "Service 0" : "mode " + std::to_string(t.mode)) + " " + m.sc->modes.sessionTime(), t.mode == 1 ? 1 : 0});
    v.push_back({t.battery >= 0 ? std::to_string(t.battery) + " %" : "unknown", 0});
    return v;
}
std::string stationLine(const SessionView &s) {
    if (!s.countersLive) return s.counters.empty() ? "Station: no live counters" : "Station: counters stopped (session over)";
    const auto &b = s.counters.back();
    return "Station: " + fmt("%.0f", s.slotsPerS) + " slots/s, " + fmt("%.0f", s.packetsPerS) + " tracker packets/s, blocked " + std::to_string(b.blocked) +
           " (counters " + ago((monoNs() - b.t) * 1e-9) + " ago)";
}

// ---- drawing ----
enum Id { BodyOff, BodyAuto2, BodyAuto, Calibrate, Swap, ModeGame, ModeService, One, Three, Start, Close };
struct Button { double x, y, w, h; Id id; bool enabled; };
void text(cairo_t *c, double x, double y, double size, const std::string &value, double maxw = 0) {
    cairo_set_font_size(c, size); std::string v = value;
    if (maxw > 0) { cairo_text_extents_t e; cairo_text_extents(c, v.c_str(), &e);
        while (e.x_advance > maxw && v.size() > 6) { v.resize(v.size() - 4); v += "..."; cairo_text_extents(c, v.c_str(), &e); } }
    cairo_move_to(c, x, y); cairo_show_text(c, v.c_str());
}
void grey(cairo_t *c, double g) { cairo_set_source_rgb(c, g, g, g); }
void tone(cairo_t *c, int good) { if (good > 0) cairo_set_source_rgb(c, .5, .95, .6); else if (good < 0) cairo_set_source_rgb(c, 1, .7, .35); else grey(c, .85); }

std::vector<Button> draw(cairo_t *c, const Model &m, const std::string &message, bool confirmStart, bool multiSel, double mx, double my) {
    std::vector<Button> buttons;
    auto hovered = [&](double x, double y, double w, double h) { return mx >= x && mx <= x + w && my >= y && my <= y + h; };
    auto button = [&](double x, double y, double w, double h, Id id, const std::string &label, bool enabled = true, bool selected = false, bool close = false) {
        const bool hover = enabled && hovered(x, y, w, h);
        if (!enabled) cairo_set_source_rgb(c, .13, .15, .17);
        else if (close) cairo_set_source_rgb(c, hover ? .55 : .30, .15, .17);
        else if (selected) cairo_set_source_rgb(c, hover ? .16 : .10, hover ? .62 : .48, hover ? .36 : .28);
        else cairo_set_source_rgb(c, hover ? .18 : .10, hover ? .62 : .42, hover ? .82 : .60);
        cairo_rectangle(c, x, y, w, h); cairo_fill(c);
        if (hover || selected) { cairo_set_source_rgb(c, .75, .95, 1); cairo_set_line_width(c, selected ? 3 : 2); cairo_rectangle(c, x + 1.5, y + 1.5, w - 3, h - 3); cairo_stroke(c); }
        grey(c, enabled ? 1 : .45); text(c, x + 18, y + h / 2 + 8, 21, label, w - 30);
        buttons.push_back({x, y, w, h, id, enabled});
    };
    cairo_set_source_rgb(c, .04, .07, .10); cairo_paint(c);
    cairo_select_font_face(c, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_source_rgb(c, .9, .95, 1); text(c, 40, 56, 38, "FramePicoTracker");
    grey(c, .6); text(c, 420, 56, 18, "native session, Game mode 1, body: two feet or pelvis + two feet");

    // Trackers
    cairo_set_source_rgb(c, .9, .95, 1); text(c, 40, 100, 22, "Trackers");
    grey(c, .55); for (int i = 0; i < 9; ++i) text(c, COL[i], 128, 15, HEAD[i]);
    int imuCount = 0, row = 0;
    for (auto &t : m.trackers) {
        if (t.imuCal) ++imuCount;
        if (row >= 4) continue;
        const double y = 156 + row * 40; ++row;
        const auto cs = cells(t, m);
        for (size_t i = 0; i < cs.size(); ++i) { if (i == 0) cairo_set_source_rgb(c, .9, .95, 1); else tone(c, cs[i].good); text(c, COL[i], y, 17, cs[i].text, COLW[i]); }
        if (!t.serial.empty()) { grey(c, .55); text(c, COL[0], y + 16, 11, t.serial); }
    }
    if (m.trackers.empty()) { grey(c, .75); text(c, 40, 156, 19, "No tracker known yet (none in SteamVR, imu_calibration.txt, peers or body roles)."); }
    grey(c, .85); text(c, 40, 330, 16, stationLine(m.session), 700);
    grey(c, .55); text(c, 40, 352, 14, "Battery unknown: FW189 reports battery only in its discovery reply, which the native session does not use.", 1120);

    // Body
    const double by = 395;
    cairo_set_source_rgb(c, .9, .95, 1); text(c, 40, by, 22, "Body");
    const bool on = m.roles.autoN > 0 || !m.roles.roles.empty();
    button(40, by + 15, 120, 54, BodyOff, "Off", true, !on);
    button(172, by + 15, 205, 54, BodyAuto2, "Auto 2 (feet)", true, m.roles.autoN == 2);
    button(389, by + 15, 211, 54, BodyAuto, "Auto 3 (+pelvis)", true, m.roles.autoN == 3);
    if (!m.roles.roles.empty() && m.roles.autoN == 0) { grey(c, .75); text(c, 40, by + 80, 14, "fixed roles (body_roles.txt)", 300); }
    button(40, by + 82, 560, 58, Calibrate, "Calibrate  (2 steps)", on);
    std::string st, st2, roles;
    const bool live = m.body.present && m.body.age <= 6;
    if (!m.body.present) st = "Driver status missing: SteamVR driver not running?";
    else if (m.body.age > 6) st = "Driver status " + std::to_string(m.body.age) + " s old: SteamVR driver not running?";
    else if (!m.body.active) st = "Driver: body model off";
    else if (m.body.step == "stand") { st = "Step 1 of 2: stand straight, look straight ahead, hold still"; st2 = m.body.note; }
    else if (m.body.step == "look_down") { st = "Step 2 of 2: look down at your trackers, keep standing still"; st2 = m.body.note; }
    else if (m.body.calibrated) {
        st = m.body.result == "optical" ? "Calibrated (all trackers seen by the cameras)" :
             m.body.result == "partial" ? "Calibrated (some trackers seen, the rest by IMU)" :
             m.body.result == "imu_only" ? "Calibrated IMU-only: positions estimated from head height" : "Calibrated";
        st2 = m.body.guessed ? "roles guessed (cameras saw no tracker): Swap if left/right is wrong" : m.body.note;
    }
    else st = "Older driver: all trackers visible, stand still 1 s";
    for (auto &kv : m.body.roles) {
        roles += (roles.empty() ? "" : "   ") + kv.second + " " + kv.first;
        const auto bt = m.body.trackers.find(kv.first);
        if (bt != m.body.trackers.end() && m.body.calibrated) roles += " (" + bt->second.position + ")";
    }
    if (m.body.calibrated && live && (m.body.result == "optical" || m.body.result.empty())) cairo_set_source_rgb(c, .5, .95, .6);
    else if (m.body.calibrated && live) cairo_set_source_rgb(c, 1, .85, .45);
    else grey(c, .85);
    text(c, 40, by + 165, 18, st, 560);
    grey(c, .7); text(c, 40, by + 185, 14, st2, 560);
    grey(c, .85); text(c, 40, by + 203, 15, roles.empty() ? "roles: assigned at calibration (pelvis = highest, feet left/right)" : roles, 560);
    bool feet = false; { int n = 0; for (auto &kv : m.body.roles) n += kv.second == "left_foot" || kv.second == "right_foot"; feet = n == 2; }
    button(40, by + 210, 260, 50, Swap, "Swap left / right", feet);
    const int want = m.roles.autoN ? m.roles.autoN : std::max(2, int(m.roles.roles.size()));
    if (imuCount < want) { grey(c, .75); text(c, 40, by + 290, 16, std::to_string(want - imuCount) + " of " + std::to_string(want) + " use the shared IMU calibration (gyro bias may differ slightly)", 560); }

    // Session
    const double sx = 630;
    cairo_set_source_rgb(c, .9, .95, 1); text(c, sx, by, 22, "Session (native, " + std::to_string(m.session.duration) + " s)");
    button(sx, by + 15, 250, 46, ModeGame, "Game  (mode 1)", true, true);
    button(sx + 265, by + 15, 265, 46, ModeService, "Service  (mode 0)", false);
    button(sx, by + 70, 250, 46, One, "1 tracker", true, !multiSel);
    button(sx + 265, by + 70, 265, 46, Three, "3 trackers (body)", MULTI_APPROVED, multiSel);
    grey(c, .6);
    text(c, sx, by + 136, 14, MULTI_APPROVED ? "3 trackers: run-multi.sh (Alpha 1.3) with" : "3 trackers: OFF until the owner approves multi-tracker sessions.", 530);
    text(c, sx, by + 153, 12, "PICO_PEERS=" + peersEnv(m.peers), 530);
    text(c, sx, by + 170, 14, "peers file (editable): " + PEERS, 530);
    const bool busy = m.session.running || m.session.procs.openocd;
    button(sx, by + 182, 530, 54, Start, busy ? "Session running..." : confirmStart ? (multiSel ? "Confirm: all 3 trackers ON, off charger" : "Confirm: trackers ON, off charger") :
                                                           multiSel ? "Start session (3 trackers)" : "Start session (1 tracker)", !busy, confirmStart);
    double ly = by + 262;
    for (auto &l : m.session.lines) { if (ly > by + 420) break;
        if (startsWith(l, "WARNING") || startsWith(l, "SESSION_FAILED") || startsWith(l, "prep failed") || startsWith(l, "SILENT")) cairo_set_source_rgb(c, 1, .7, .35);
        else if (startsWith(l, "RUNNING") || startsWith(l, "TRACKING") || l == "live exit 0") cairo_set_source_rgb(c, .5, .95, .6); else grey(c, .85);
        text(c, sx, ly, 15, l, 540); ly += 20; }
    if (m.session.lines.empty()) { grey(c, .75); text(c, sx, ly, 16, "No session yet. The session sets and verifies Game mode 1 itself.", 530); }

    // Footer
    grey(c, .95); if (!message.empty()) text(c, 40, 845, 18, message, 900);
    grey(c, .6); text(c, 40, 878, 15, "Closing this panel never stops a session; a session ends after its duration and restores Roy SRAM itself.", 900);
    button(980, 832, 180, 52, Close, "Close", true, false, true);
    return buttons;
}
std::string modeMessage(Id id) {
    return id == ModeGame ? "Game mode 1 is the only native target: the session queries each tracker and switches it to mode 1 if needed."
                          : "Service mode 0 (firmware update) is not in the native path yet.";
}
void printStatus(const Model &m) {  // --status: the same model as text (verification over SSH)
    for (auto *h : HEAD) std::printf("%-20s", h);
    std::printf("\n");
    for (auto &t : m.trackers) { for (auto &c : cells(t, m)) std::printf("%-20s", c.text.c_str()); std::printf("  %s\n", t.serial.c_str()); }
    std::printf("%s\nPICO_PEERS=%s (multi approved: %s)\nsession root %s start_ns %lld counters %zu\n", stationLine(m.session).c_str(), peersEnv(m.peers).c_str(),
                MULTI_APPROVED ? "yes" : "no", m.session.root.c_str(), m.session.startNs, m.session.counters.size());
    for (auto &l : m.session.lines) std::printf("  %s\n", l.c_str());
}
}  // namespace

int main(int argc, char **argv) {
    const std::string arg = argc > 1 ? argv[1] : "";
    if (arg == "--probe") return VrOverlay::probe();
    if (arg == "--body" && argc > 2) {  // same code as the panel buttons: off | auto2 | auto3 (auto) | calibrate | swap
        const auto before = readStatus();
        std::printf("%s\nstatus before: active %d auto %d calibrated %d roles %zu\n", bodyAction(argv[2]).c_str(), before.active, before.autoN, before.calibrated, before.roles.size());
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::printf("status after 5 s:  %s", readFile(STATUS, 4096).c_str());
        return 0;
    }
    if (arg == "--start-session") { std::printf("%s\n", startSession(argc > 2 && std::string(argv[2]) == "multi").c_str()); return 0; }
    if (arg == "--status" || (arg == "--render" && argc > 2)) {  // offline text / preview without SteamVR
        Scanners sc; auto m = gather(false, sc);
        if (arg == "--status") { printStatus(m); return 0; }
        auto *s = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, PW, PH); auto *c = cairo_create(s);
        draw(c, m, argc > 3 ? argv[3] : "", false, false, -1, -1); cairo_surface_write_to_png(s, argv[2]);
        cairo_destroy(c); cairo_surface_destroy(s); return 0;
    }
    int lock = open((W + "/frame-pico-ui.lock").c_str(), O_CREAT | O_RDWR, 0600);
    if (lock < 0) return 1;
    if (flock(lock, LOCK_EX | LOCK_NB) != 0) {
        FILE *f = fopen((W + "/frame-pico-ui.pid").c_str(), "r"); int pid = 0;
        if (f) { if (fscanf(f, "%d", &pid) == 1 && pid > 1) kill(pid, SIGUSR1); fclose(f); }
        close(lock); return 0;
    }
    if (FILE *pf = fopen((W + "/frame-pico-ui.pid").c_str(), "w")) { fprintf(pf, "%d\n", getpid()); fclose(pf); }
    signal(SIGINT, signalHandler); signal(SIGTERM, signalHandler); signal(SIGUSR1, signalHandler);
    VrOverlay overlay; std::string error;
    if (overlay.connect(PW, PH, error) != VrOverlay::ConnectResult::Ok) { fprintf(stderr, "%s\n", error.c_str()); return 1; }
    std::vector<unsigned char> rgba(PW * PH * 4), thumb(128 * 128 * 4, 255);
    overlay.submitThumbnail(thumb.data(), 128); overlay.showPanel();
    auto *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, PW, PH); auto *c = cairo_create(surface);
    Scanners scanners; Model model = gather(true, scanners);
    auto refresh = std::chrono::steady_clock::now(), confirmUntil = refresh;
    bool savedPreview = false, multiSel = false; double mouseX = -1, mouseY = -1; std::string message; std::vector<Button> buttons;
    while (!stop && overlay.steamVrAlive()) {
        auto events = overlay.pollEvents(); if (events.quit || events.closeRequested) break;
        if (show) { show = 0; overlay.showPanel(); }
        const auto now = std::chrono::steady_clock::now();
        for (auto p : events.pointer) {
            if (p.type == PointerInput::Type::Leave) { mouseX = mouseY = -1; continue; }
            mouseX = p.x; mouseY = p.y;
            if (p.type != PointerInput::Type::Up) continue;
            for (auto &b : buttons) {
                if (!b.enabled || mouseX < b.x || mouseX > b.x + b.w || mouseY < b.y || mouseY > b.y + b.h) continue;
                switch (b.id) {
                    case BodyOff: message = bodyAction("off"); break;
                    case BodyAuto2: message = bodyAction("auto2"); break;
                    case BodyAuto: message = bodyAction("auto3"); break;
                    case Calibrate: message = bodyAction("calibrate"); break;
                    case Swap: message = bodyAction("swap"); break;
                    case ModeGame: case ModeService: message = modeMessage(b.id); break;
                    case One: case Three: multiSel = b.id == Three && MULTI_APPROVED; confirmUntil = now; break;
                    case Start:  // two presses: the second one is the owner's fresh readiness answer
                        if (now < confirmUntil) { confirmUntil = now; message = startSession(multiSel); }
                        else { confirmUntil = now + std::chrono::seconds(6); message = multiSel ? "All 3 trackers ON and off the charger, controllers awake? Press again to start."
                                                                                                 : "Trackers ON and off the charger, controllers awake? Press again to start."; }
                        break;
                    case Close: stop = 1; break;
                }
                refresh = now - std::chrono::seconds(5);
            }
        }
        if (now - refresh > std::chrono::seconds(1)) { model = gather(true, scanners); refresh = now; }
        buttons = draw(c, model, message, now < confirmUntil, multiSel, mouseX, mouseY);
        cairo_surface_flush(surface); auto *pixels = cairo_image_surface_get_data(surface);
        if (!savedPreview) { cairo_surface_write_to_png(surface, (W + "/frame-pico-ui-preview.png").c_str()); savedPreview = true; }
        // Verification without a person in the headset: the exact submitted panel pixels on request.
        if (access("/tmp/frame-pico-ui-snapshot", F_OK) == 0) { unlink("/tmp/frame-pico-ui-snapshot"); cairo_surface_write_to_png(surface, "/tmp/frame-pico-ui-snapshot.png"); }
        for (size_t i = 0; i < rgba.size(); i += 4) { rgba[i] = pixels[i + 2]; rgba[i + 1] = pixels[i + 1]; rgba[i + 2] = pixels[i]; rgba[i + 3] = pixels[i + 3]; }
        overlay.submitPanel(rgba.data());
        if (overlay.ensureOverlay() == OverlayRepair::Repaired) { overlay.submitThumbnail(thumb.data(), 128); overlay.submitPanel(rgba.data()); }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    cairo_destroy(c); cairo_surface_destroy(surface); overlay.shutdown(); close(lock); return 0;
}
