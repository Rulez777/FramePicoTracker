#pragma once
#include "json.hpp"
#include <algorithm>
#include <array>
#include <cerrno>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <functional>
#include <mutex>
#include <regex>
#include <sstream>
#include <thread>
#ifdef FRAME_API_HOST_TEST
#include "host_test.hpp"
#else
#include <fcntl.h>
#include <poll.h>
#include <spawn.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
extern char **environ;
#endif

namespace phase5 {
std::string sha256(const std::string &);
}
namespace frame {
namespace fs = std::filesystem;
inline double wall() {
  return std::chrono::duration<double>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}
inline double mono() {
  return std::chrono::duration<double>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}
inline std::string readfile(const fs::path &p) {
  std::ifstream in(p, std::ios::binary);
  if (!in)
    throw std::runtime_error("[Errno " + std::to_string(errno) + "] " +
                             std::strerror(errno) + ": '" + p.string() + "'");
  std::ostringstream s;
  s << in.rdbuf();
  if (in.bad())
    throw std::runtime_error("Read failed: " + p.string());
  return s.str();
}
inline void writefile(const fs::path &p, const std::string &s) {
  std::ofstream out(p, std::ios::binary);
  if (!out)
    throw std::runtime_error("Cannot write: " + p.string());
  out.write(s.data(), std::streamsize(s.size()));
  if (!out)
    throw std::runtime_error("Cannot write: " + p.string());
}
inline J load(const fs::path &p) { return Parser(readfile(p)).parse(); }
inline std::string trim(std::string s) {
  auto ws = [](unsigned char c) { return std::isspace(c); };
  while (!s.empty() && ws(static_cast<unsigned char>(s.back())))
    s.pop_back();
  size_t n = 0;
  while (n < s.size() && ws(static_cast<unsigned char>(s[n])))
    ++n;
  return s.substr(n);
}
struct Timeout : std::runtime_error {
  using std::runtime_error::runtime_error;
};
struct Result {
  int code;
  std::string out, err;
};
using Runner = std::function<Result(const std::vector<std::string> &, double)>;
inline Result run(const std::vector<std::string> &args, double timeout) {
#ifdef FRAME_API_HOST_TEST
  (void)args;
  (void)timeout;
  throw std::runtime_error("POSIX subprocess test requires Linux");
#else
  static std::mutex spawn_mutex;
  std::unique_lock<std::mutex> spawning(spawn_mutex);
  int op[2], ep[2];
  if (pipe(op))
    throw std::runtime_error("pipe failed");
  if (pipe(ep)) {
    close(op[0]);
    close(op[1]);
    throw std::runtime_error("pipe failed");
  }
  for (int fd : {op[0], op[1], ep[0], ep[1]})
    fcntl(fd, F_SETFD, FD_CLOEXEC);
  std::vector<char *> av;
  for (const auto &a : args)
    av.push_back(const_cast<char *>(a.c_str()));
  av.push_back(nullptr);
  posix_spawn_file_actions_t actions;
  posix_spawn_file_actions_init(&actions);
  posix_spawn_file_actions_adddup2(&actions, op[1], 1);
  posix_spawn_file_actions_adddup2(&actions, ep[1], 2);
  for (int fd : {op[0], op[1], ep[0], ep[1]})
    posix_spawn_file_actions_addclose(&actions, fd);
  pid_t pid = -1;
  int spawn_error =
      posix_spawnp(&pid, av[0], &actions, nullptr, av.data(), environ);
  posix_spawn_file_actions_destroy(&actions);
  close(op[1]);
  close(ep[1]);
  spawning.unlock();
  if (spawn_error) {
    close(op[0]);
    close(ep[0]);
    throw std::runtime_error("[Errno " + std::to_string(spawn_error) + "] " +
                             std::strerror(spawn_error) + ": '" + args[0] +
                             "'");
  }
  fcntl(op[0], F_SETFL, O_NONBLOCK);
  fcntl(ep[0], F_SETFL, O_NONBLOCK);
  Result r{};
  double deadline = mono() + timeout;
  int status = 0;
  bool exited = false;
  bool opened[2] = {true, true};
  int fds[2] = {op[0], ep[0]};
  while (!exited || opened[0] || opened[1]) {
    if (mono() >= deadline) {
      if (!exited) {
        kill(pid, SIGKILL);
        while (waitpid(pid, &status, 0) < 0 && errno == EINTR) {
        }
      }
      for (int i = 0; i < 2; ++i)
        if (opened[i])
          close(fds[i]);
      std::string cmd = "[";
      for (size_t i = 0; i < args.size(); ++i) {
        if (i)
          cmd += ", ";
        cmd += "'" + args[i] + "'";
      }
      cmd += "]";
      throw Timeout("Command '" + cmd + "' timed out after " +
                    std::to_string(int(timeout)) + " seconds");
    }
    pollfd p[2] = {{opened[0] ? fds[0] : -1, short(opened[0] ? POLLIN : 0), 0},
                   {opened[1] ? fds[1] : -1, short(opened[1] ? POLLIN : 0), 0}};
    poll(p, 2, 10);
    for (int i = 0; i < 2; ++i)
      if (opened[i]) {
        char b[65536];
        for (;;) {
          ssize_t n = read(fds[i], b, sizeof b);
          if (n > 0)
            (i == 0 ? r.out : r.err).append(b, size_t(n));
          else {
            if (n == 0) {
              close(fds[i]);
              opened[i] = false;
            } else if (errno == EINTR)
              continue;
            break;
          }
        }
      }
    if (!exited) {
      pid_t w = waitpid(pid, &status, WNOHANG);
      if (w == pid)
        exited = true;
    }
  }
  r.code = WIFEXITED(status) ? WEXITSTATUS(status) : -WTERMSIG(status);
  auto universal = [](const std::string &s) {
    std::string out;
    for (size_t i = 0; i < s.size(); ++i) {
      if (s[i] == '\r') {
        out += '\n';
        if (i + 1 < s.size() && s[i + 1] == '\n')
          ++i;
      } else
        out += s[i];
    }
    return out;
  };
  r.out = universal(r.out);
  r.err = universal(r.err);
  return r;
#endif
}
inline double mtime(const fs::path &p) {
  struct stat s{};
  if (stat(p.c_str(), &s))
    throw std::runtime_error("stat failed: " + p.string());
  return double(s.st_mtim.tv_sec) + double(s.st_mtim.tv_nsec) * 1e-9;
}
inline std::string camera_command(bool start, Runner runner = run) {
  auto result = runner(
      {"/opt/steamvr/bin/linuxarm64/vrcmd", "--cmd", "0",
       start ? "start_tracking_recording 1 0 0" : "stop_tracking_recording"},
      5);
  if (result.code || result.out.find("response: ok") == std::string::npos)
    throw std::runtime_error("Camera recording command failed: " + result.out +
                             result.err);
  return trim(result.out);
}
inline std::vector<fs::path> recent(const fs::path &root,
                                    const std::regex &pattern) {
  std::vector<fs::path> v;
  if (fs::exists(root))
    for (const auto &e : fs::directory_iterator(root))
      if (std::regex_match(e.path().filename().string(), pattern))
        v.push_back(e.path());
  std::stable_sort(v.begin(), v.end(), [](const auto &a, const auto &b) {
    return mtime(a) > mtime(b);
  });
  return v;
}
inline std::string uuid() {
  std::array<unsigned char, 16> b{};
  int fd = open("/dev/urandom", O_RDONLY | O_CLOEXEC);
  if (fd < 0)
    throw std::runtime_error("urandom unavailable");
  size_t n = 0;
  while (n < b.size()) {
    ssize_t r = read(fd, b.data() + n, b.size() - n);
    if (r < 0 && errno == EINTR)
      continue;
    if (r <= 0) {
      close(fd);
      throw std::runtime_error("urandom read failed");
    }
    n += size_t(r);
  }
  close(fd);
  b[6] = (b[6] & 15) | 64;
  b[8] = (b[8] & 63) | 128;
  const char *h = "0123456789abcdef";
  std::string s;
  for (auto x : b) {
    s += h[x >> 4];
    s += h[x & 15];
  }
  return s;
}
inline std::string utc() {
  auto now = std::chrono::system_clock::now();
  auto us = std::chrono::duration_cast<std::chrono::microseconds>(
                now.time_since_epoch())
                .count();
  time_t t = time_t(us / 1000000);
  tm v{};
  gmtime_r(&t, &v);
  char b[80];
  strftime(b, sizeof b, "%Y-%m-%dT%H:%M:%S", &v);
  std::string s = b;
  if (us % 1000000) {
    std::snprintf(b, sizeof b, ".%06d", int(us % 1000000));
    s += b;
  }
  return s + "+00:00";
}

struct Control {
  fs::path root;
  Runner runner;
  explicit Control(fs::path p, Runner r = run)
      : root(std::move(p)), runner(std::move(r)) {}
  static std::string unitname(const std::string &p) {
    return "frame-pico-" + p + ".service";
  }
  std::string command(std::initializer_list<std::string> tail) {
    std::vector<std::string> a = {"systemctl", "--user"};
    a.insert(a.end(), tail);
    auto r = runner(a, 30);
    if (r.code)
      throw std::runtime_error(
          trim(r.err).empty() ? "Radio service operation failed" : trim(r.err));
    return r.out;
  }
  J unit(const std::string &name) {
    std::istringstream in(
        command({"show", name,
                 "--property=ActiveState,InvocationID,MainPID,FragmentPath"}));
    J j = J::obj();
    std::string line;
    while (std::getline(in, line)) {
      auto pos = line.find('=');
      if (pos != std::string::npos)
        j[line.substr(0, pos)] = line.substr(pos + 1);
    }
    return j;
  }
  J saved() {
    auto p = root / "app-owner-last.json";
    if (!fs::exists(p))
      return {};
    J j = load(p);
    if (!(j["profile"] == "game" || j["profile"] == "service") ||
        !std::regex_match(j["invocation"].text, std::regex("[0-9a-f]{32}")))
      throw std::runtime_error("Saved radio ownership malformed");
    return j;
  }
  void restored(const std::string &p, const std::string &id) {
    if (!std::regex_match(id, std::regex("[0-9a-f]{32}")))
      throw std::runtime_error("Previous owner invocation unknown");
    auto j = load(root / "app-cleanup" / p / (id + ".json"));
    bool good = j["owner_invocation_id"].text == id && j["profile"].text == p;
    for (const auto *k : {"session_closed_verified", "ram_restored_verified",
                          "mpu_restored", "trace_controls_restored_verified"})
      good = good && j[k].yes();
    if (!good)
      throw std::runtime_error("Previous owner MCU restoration not verified");
  }
  J validate(const std::string &p) {
    auto fail = [](const std::string &s) { throw std::runtime_error(s); };
    if (p != "game" && p != "service")
      fail("Unknown radio profile");
    J catalog;
    try {
      catalog = load(root / "app-profiles.json");
    } catch (const std::exception &) {
      fail("Installed profile catalog unavailable");
    }
    auto item = catalog[p];
    if (!item["admitted"].yes())
      fail(item["reason"].type == 'n' ? "Profile not admitted"
                                      : item["reason"].text);
    if (item["mode"].type != 'd' ||
        item["mode"].number() != (p == "game" ? 1 : 0))
      fail("Wrong applied-mode requirement");
    if (item["unit"].text != unitname(p))
      fail("Unexpected radio unit");
    auto files = item["files"];
    if (files.type != 'o' || files.object.empty())
      fail("Missing artifact seals");
    for (const auto &f : files.object) {
      if (!fs::path(f.first).is_absolute() ||
          !std::regex_match(f.second.text, std::regex("[0-9a-f]{64}")))
        fail("Invalid artifact seal");
      if (phase5::sha256(readfile(f.first)) != f.second.text)
        fail("Radio artifact seal mismatch: " + f.first);
    }
    auto has = [&](const std::string &s) {
      for (const auto &f : files.object)
        if (f.first == s)
          return true;
      return false;
    };
    auto u = unit(unitname(p));
    if (!has(u["FragmentPath"].text))
      fail("Installed radio unit is not sealed");
    if (!(item["owner_contract"] == "fresh-admission-mode-lease-cleanup-v1"))
      fail("Missing runtime owner contract");
    std::string config = item["station_config"].text;
    if (!has(config))
      fail("Station mode configuration is not sealed");
    auto c = load(config);
    if (c["tracker_desired_mode"].text != (p == "game" ? "frame" : "stock"))
      fail("Runtime does not maintain requested tracker mode");
    if (!c["tracker_dual_mode_ids"].truth() ||
        c["tracker_mode_query_schema"].number() != 2)
      fail("Fresh applied-mode queries required");
    if (p == "service" && !c["optics_enabled"].no())
      fail("Service mode cannot publish optical poses");
    auto cmd = item["command"];
    if (cmd.type != 'a' || cmd.array.empty() ||
        std::any_of(cmd.array.begin(), cmd.array.end(),
                    [](const J &a) { return a.type != 's'; }))
      fail("Missing reviewed runtime command");
    if (!has(cmd.array[0].text))
      fail("Runtime executable is not sealed");
    for (size_t i = 1; i < cmd.array.size(); ++i) {
      auto a = cmd.array[i].text;
      if (a.size() >= 3 && a.substr(a.size() - 3) == ".py" && !has(a))
        fail("Runtime entry script is not sealed");
    }
    auto arg = [&](const char *k, const char *error, bool optional = false) {
      size_t n = 0, index = 0;
      for (size_t i = 0; i < cmd.array.size(); ++i)
        if (cmd.array[i] == k) {
          ++n;
          index = i;
        }
      if (optional && n == 0)
        return std::string{};
      if (n != 1 || index + 1 >= cmd.array.size())
        fail(error);
      return cmd.array[index + 1].text;
    };
    auto watcher =
        arg("--tracking-watch", "Invalid tracking watcher argument", true);
    if (!watcher.empty() && (!has(watcher) || access(watcher.c_str(), X_OK)))
      fail("Runtime tracking watcher is not executable or sealed");
    if (arg("--live-state", "Current owner live stream required") !=
        "{attempt}/radio-live.json")
      fail("Current owner live stream required");
    if (arg("--station-config",
            "Runtime command uses a different mode configuration") != config)
      fail("Runtime command uses a different mode configuration");
    auto module = arg("--module", "Explicit radio module required");
    if (files[module].text != (p == "game" ? "d8328717a5f6b757fd002ad6622e4fa5f"
                                             "07c45a1de935f6631267792deb286a3"
                                           : "44f4e52b07bbba4c459256c910228d440"
                                             "e25ff0d288238e76047fd21000c4c7c"))
      fail("Radio module differs from retained game/service profile");
    return item;
  }
  std::pair<fs::path, fs::path> stream() {
    auto s = saved();
    auto game = unit(unitname("game")), service = unit(unitname("service"));
    std::vector<std::string> active;
    for (const auto &p : {std::string("game"), std::string("service")}) {
      auto &u = p == "game" ? game : service;
      if (u["ActiveState"] == "active")
        active.push_back(p);
      if (!(u["ActiveState"] == "active" || u["ActiveState"] == "inactive" ||
            u["ActiveState"] == "failed"))
        throw std::runtime_error("Radio profile is transitioning");
    }
    if (active.size() > 1)
      throw std::runtime_error("Multiple radio owners active");
    if (s.truth() || !active.empty()) {
      if (!s.truth() || active != std::vector<std::string>{s["profile"].text})
        throw std::runtime_error("Current radio owner is inactive or unbound");
      auto u = s["profile"] == "game" ? game : service;
      if (u["InvocationID"].text != s["invocation"].text ||
          !std::regex_match(u["MainPID"].text, std::regex("[1-9][0-9]*")))
        throw std::runtime_error(
            "Current radio invocation does not match its owner");
      auto a = root / "app-attempts" / s["profile"].text / s["invocation"].text;
      return {a / "radio-live.json", a / "radio-live-records.jsonl"};
    }
    if (!(unit("frame-pico-radio.service")["ActiveState"] == "active"))
      throw std::runtime_error("Radio service is inactive");
    return {root / "radio-live.json", root / "radio-live-records.jsonl"};
  }
  J radio() {
    try {
      auto s = load(stream().first);
      double age = wall() - s["updated_unix"].number();
      if (!s["running"].yes() || s["updated_unix"].type != 'd' ||
          !(age >= 0 && age < 2))
        throw std::runtime_error("Radio stream is stale or stopped");
      return s;
    } catch (const std::exception &e) {
      return J::obj({{"running", false},
                     {"trackers", J::list()},
                     {"poses_valid", false},
                     {"error", e.what()}});
    }
  }
  J trackers(const J &state) {
    J list = J::list();
    double age = wall() - state["updated_unix"].number();
    bool fresh = state["running"].yes() && state["updated_unix"].type == 'd' &&
                 age >= 0 && age < 2;
    for (const auto &peer : state["trackers"].array) {
      if (!fresh || !peer["connected"].yes())
        continue;
      J mode = peer["radio_mode_verified"].yes() ? peer["radio_mode"] : J{};
      bool stock = peer["custom_mode_supported"].no() &&
                   peer["firmware_identity_verified"].yes(),
           tracked = peer["pose_valid"].yes() && mode == "frame";
      list.array.push_back(
          J::obj({{"id_le", peer["id_le"]},
                  {"serial", peer["serial"]},
                  {"mode", mode == "frame"   ? J(1)
                           : mode == "stock" ? J(0)
                                             : J{}},
                  {"mode_verified", mode == "frame" || mode == "stock"},
                  {"status", tracked ? "Tracked"
                                     : "RF connected; awaiting verified 6DoF"},
                  {"pose_valid", tracked},
                  {"update_required", stock},
                  {"update_available", false},
                  {"mode_status", stock ? "Custom firmware update required"
                                  : mode.type == 'n' ? "Checking mode"
                                  : mode == "frame"  ? "Game mode"
                                                     : "Service mode"}}));
    }
    return list;
  }
  J application(const J &state) {
    J profiles = J::obj();
    for (const auto &p : {"game", "service"})
      try {
        validate(p);
        profiles[p] = J::obj({{"available", true}, {"reason", ""}});
      } catch (const std::exception &e) {
        profiles[p] = J::obj({{"available", false}, {"reason", e.what()}});
      }
    auto peers = trackers(state);
    bool tracking =
        std::any_of(peers.array.begin(), peers.array.end(),
                    [](const auto &p) { return p["pose_valid"].yes(); });
    return J::obj({{"profiles", profiles},
                   {"trackers", peers},
                   {"tracking_available", tracking},
                   {"desired_game_mode", 1},
                   {"update_mode", 0}});
  }
  J select(const std::string &p) {
    validate(p);
    auto legacy = unit("frame-pico-radio.service");
    if (!(legacy["ActiveState"] == "inactive" ||
          legacy["ActiveState"] == "failed"))
      throw std::runtime_error(
          "Legacy radio owner must be closed and restored first");
    J states = J::obj({{"game", unit(unitname("game"))},
                       {"service", unit(unitname("service"))}});
    if (states["game"]["ActiveState"] == "active" &&
        states["service"]["ActiveState"] == "active")
      throw std::runtime_error(
          "Multiple radio owners active; restoration required");
    auto s = saved();
    if (s.truth() && !(states[s["profile"].text]["ActiveState"] == "active" &&
                       states[s["profile"].text]["InvocationID"].text ==
                           s["invocation"].text))
      restored(s["profile"].text, s["invocation"].text);
    for (const auto &current : {std::string("game"), std::string("service")}) {
      auto state = states[current];
      if (!(state["ActiveState"] == "inactive" ||
            state["ActiveState"] == "failed" ||
            state["ActiveState"] == "active"))
        throw std::runtime_error("Radio owner still transitioning");
      if (current == p && state["ActiveState"] == "active")
        return J::obj({{"requested_profile", p},
                       {"state", "already_running"},
                       {"tracking_verified", false}});
      auto id = state["InvocationID"].text;
      if (state["ActiveState"] == "active") {
        command({"stop", unitname(current)});
        auto stopped = unit(unitname(current));
        if (!(stopped["ActiveState"] == "inactive" &&
              stopped["MainPID"] == "0"))
          throw std::runtime_error(
              "Previous radio process still owns hardware");
        restored(current, id);
      } else if (!id.empty())
        restored(current, id);
    }
    validate(p);
    command({"start", unitname(p)});
    return J::obj({{"requested_profile", p},
                   {"state", "start_requested"},
                   {"desired_mode", p == "game" ? 1 : 0},
                   {"tracking_verified", false}});
  }
  J stop() {
    for (const auto &p : {std::string("game"), std::string("service")}) {
      auto s = unit(unitname(p));
      if (s["ActiveState"] == "active" || s["ActiveState"] == "activating" ||
          s["ActiveState"] == "deactivating") {
        command({"stop", unitname(p)});
        auto after = unit(unitname(p));
        if (!(after["ActiveState"] == "inactive" && after["MainPID"] == "0"))
          throw std::runtime_error("Radio owner remains alive");
        restored(p, s["InvocationID"].text);
      }
    }
    auto s = saved();
    if (s.truth())
      restored(s["profile"].text, s["invocation"].text);
    auto legacy = unit("frame-pico-radio.service");
    if (!(legacy["ActiveState"] == "inactive" ||
          legacy["ActiveState"] == "failed")) {
      command({"stop", "frame-pico-radio.service"});
      throw std::runtime_error("Legacy radio stop requested; its restoration "
                               "requires separate verification");
    }
    return J::obj({{"state", "stopped"},
                   {"owned_profiles_restored", true},
                   {"tracking_verified", false}});
  }
  J update(const std::string &chip, const J &state) {
    auto peers = trackers(state);
    int n = 0;
    bool required = false;
    for (const auto &p : peers.array)
      if (p["id_le"].text == chip) {
        ++n;
        required = p["update_required"].yes();
      }
    if (n != 1 || !required)
      throw std::runtime_error(
          "Fresh stock firmware capability proof required");
    throw std::runtime_error("OTA application owner integration pending; no "
                             "firmware transfer started");
  }
};
} // namespace frame
