#pragma once
#include "archive.hpp"
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <map>
#ifndef FRAME_API_HOST_TEST
#include <netinet/in.h>
#include <sys/socket.h>
#endif
namespace frame {
inline constexpr const char *PAGE =
    R"PAGE(<!doctype html><meta charset="utf-8"><title>FramePicoTracker</title>
<body style="font:18px system-ui;background:#101820;color:#eef;padding:30px;max-width:900px">
<h1>FramePicoTracker</h1><p>Diagnostic capture: cameras, radio packets and Frame poses. Screen and audio recording disabled.</p>
<button id="capture">Record diagnostics (10 seconds)</button> <button id="refresh">Refresh status</button>
<p id="result"></p><pre id="status" style="white-space:pre-wrap;font-size:13px"></pre>
<script>
async function status(){document.getElementById('status').textContent=JSON.stringify(await(await fetch('/status')).json(),null,2)}
document.getElementById('refresh').onclick=status;
document.getElementById('capture').onclick=async()=>{
const button=document.getElementById('capture');button.disabled=true;
try{let r=await fetch('/capture',{method:'POST'});let job=await r.json();if(!r.ok)throw Error(job.error);
document.getElementById('result').textContent='Recording '+job.id;
while(true){await new Promise(r=>setTimeout(r,1000));let j=await(await fetch('/capture/'+job.id)).json();
if(j.state==='complete'){let a=document.createElement('a');a.href='/capture/'+job.id+'/bundle';a.textContent='Download diagnostic bundle';document.getElementById('result').replaceChildren(a);break}
if(j.state==='failed')throw Error(JSON.stringify(j));}await status();
}catch(e){document.getElementById('result').textContent=e.message}finally{button.disabled=false}}
status();</script>)PAGE";
struct Response {
  int code = 200;
  std::string content = "application/json", body;
  bool error = false;
  fs::path file;
  uintmax_t bytes = 0;
  bool capped = false;
  std::string message;
};
inline Response file_response(const fs::path &p, const char *content,
                              bool capped) {
  Response r;
  r.content = content;
  r.file = p;
  r.bytes = fs::file_size(p);
  r.capped = capped;
  return r;
}
inline std::pair<std::string, std::string> reason(int c) {
  switch (c) {
  case 200:
    return {"OK", "Request fulfilled, document follows"};
  case 202:
    return {"Accepted", "Request accepted, processing continues off-line"};
  case 400:
    return {"Bad Request", "Bad request syntax or unsupported method"};
  case 403:
    return {"Forbidden", "Request forbidden -- authorization will not help"};
  case 404:
    return {"Not Found", "Nothing matches the given URI"};
  case 409:
    return {"Conflict", "Request conflict"};
  case 501:
    return {"Not Implemented", "Server does not support this operation"};
  case 503:
    return {"Service Unavailable",
            "The server cannot process the request due to a high load"};
  default:
    return {"Gateway Timeout",
            "The gateway server did not receive a timely response"};
  }
}
inline std::string html(std::string s) {
  std::string o;
  for (char c : s) {
    if (c == '&')
      o += "&amp;";
    else if (c == '<')
      o += "&lt;";
    else if (c == '>')
      o += "&gt;";
    else
      o += c;
  }
  return o;
}
inline Response error(int code, std::string message = "") {
  auto r = reason(code);
  if (message.empty())
    message = r.first;
  std::string body =
      "<!DOCTYPE HTML>\n<html lang=\"en\">\n    <head>\n        <meta "
      "charset=\"utf-8\">\n        <title>Error response</title>\n    "
      "</head>\n    <body>\n        <h1>Error response</h1>\n        <p>Error "
      "code: " +
      std::to_string(code) + "</p>\n        <p>Message: " + html(message) +
      ".</p>\n        <p>Error code explanation: " + std::to_string(code) +
      " - " + r.second + ".</p>\n    </body>\n</html>\n";
  Response response;
  response.code = code;
  response.content = "text/html;charset=utf-8";
  response.body = body;
  response.error = true;
  response.message = message;
  return response;
}
inline Response json(J j, int code = 200) {
  Response response;
  response.code = code;
  response.body = dump(j);
  return response;
}
struct Agent {
  fs::path root, datasets;
  Runner runner;
  double started = mono();
  std::mutex lock, radio_lock;
  std::string active;
  std::vector<std::pair<std::string, J>> jobs;
  std::vector<std::thread> workers;
  Agent(fs::path r = "/home/steamos/pico-ram-work",
        fs::path d =
            "/home/steamos/.local/share/Steam/config/cv/xrservice/datasets",
        Runner fn = run)
      : root(std::move(r)), datasets(std::move(d)), runner(std::move(fn)) {}
  ~Agent() {
    for (auto &t : workers)
      if (t.joinable())
        t.join();
  }
  Control control() { return Control(root, runner); }
  J &job(const std::string &id) {
    for (auto &p : jobs)
      if (p.first == id)
        return p.second;
    throw std::runtime_error("Unknown capture");
  }
  bool capturing() {
    std::lock_guard<std::mutex> g(lock);
    return !active.empty();
  }
  J inventory() {
    J result = J::list();
    auto paths = recent(datasets, std::regex(".*\\.dataset"));
    if (paths.size() > 5)
      paths.resize(5);
    for (const auto &dir : paths) {
      J files = J::list();
      for (const auto &f : fs::directory_iterator(dir))
        if (f.is_regular_file())
          files.array.push_back(J::obj({{"name", f.path().filename().string()},
                                        {"bytes", J(f.file_size())},
                                        {"mtime", mtime(f.path())}}));
      result.array.push_back(
          J::obj({{"name", dir.filename().string()},
                  {"files", files},
                  {"scope", "Recorded dataset, not a live camera feed"}}));
    }
    return result;
  }
  J status() {
    J reports = J::list(), probes = J::list();
    for (const auto &p : recent(root, std::regex("ir-tx.*\\.json"))) {
      auto name = p.filename().string();
      if (name.size() >= 14 &&
          name.substr(name.size() - 14) == ".recovery.json")
        continue;
      try {
        auto d = load(p);
        if (!d["active_ir_tx_probe"].truth())
          continue;
        J report = J::obj();
        for (const auto *k :
             {"tx_diagnostics", "slots_granted", "session_closed_verified",
              "ram_restored_verified", "mpu_restored", "flash_written"})
          report[k] = d[k];
        probes.array.push_back(
            J::obj({{"source", name},
                    {"age_seconds", wall() - mtime(p)},
                    {"evidence_kind",
                     "Completed TX probe, not proof of command delivery"},
                    {"report", report}}));
        if (probes.array.size() == 3)
          break;
      } catch (const std::exception &) {
      }
    }
    auto paths = recent(root, std::regex(".*\\.summary\\.json"));
    if (paths.size() > 5)
      paths.resize(5);
    for (const auto &p : paths)
      try {
        reports.array.push_back(J::obj(
            {{"name", p.filename().string()},
             {"mtime", mtime(p)},
             {"age_seconds", wall() - mtime(p)},
             {"evidence_kind", "Completed capture; not live device presence"},
             {"report", load(p)}}));
      } catch (const std::exception &) {
        reports.array.push_back(
            J::obj({{"name", p.filename().string()},
                    {"error", "Report incomplete or unreadable"}}));
      }
    J camera;
    auto scan = root / "tracker-camera101-scan0.json";
    if (fs::exists(scan)) {
      auto d = load(scan);
      int lo = 0, hi = 0;
      bool first = true;
      for (const auto &f : d["frames"].array) {
        int n = int(f["blobs"].array.size());
        if (first)
          lo = n;
        first = false;
        lo = std::min(lo, n);
        hi = std::max(hi, n);
      }
      camera = J::obj({{"source", scan.filename().string()},
                       {"frames", int(d["frames"].array.size())},
                       {"min_blobs", lo},
                       {"max_blobs", hi},
                       {"identity_verified", false}});
    }
    auto stamp = utc();
    auto elapsed = mono() - started;
    auto c = control();
    auto radio = c.radio();
    auto app = c.application(c.radio());
    J current;
    {
      std::lock_guard<std::mutex> g(lock);
      if (!active.empty())
        current = active;
    }
    return J::obj({{"utc", stamp},
                   {"uptime_seconds", elapsed},
                   {"radio_service", radio},
                   {"application", app},
                   {"mode", "local_frame_tracker_api"},
                   {"recent_radio_runs", reports},
                   {"recent_active_probes", probes},
                   {"camera_scan", camera},
                   {"active_capture", current},
                   {"datasets", inventory()},
                   {"live_camera_available", false},
                   {"tracker_poses_available", false}});
  }
  void capture_worker(std::string id) {
    auto prefix = root / ("api-" + id);
    double start = wall();
    try {
      {
        std::lock_guard<std::mutex> g(lock);
        job(id)["state"] = "running";
      }
      auto c = control();
      if (!c.radio()["running"].truth())
        throw std::runtime_error(
            "Station is not running; diagnostics cannot start radio probes");
      auto log = c.stream().second;
      if (!fs::exists(log))
        throw std::runtime_error("Station record stream is unavailable");
      int fd = open(log.c_str(), O_RDONLY | O_CLOEXEC);
      if (fd < 0)
        throw std::runtime_error("Station record stream is unavailable");
      struct Close {
        int fd;
        ~Close() { close(fd); }
      } closer{fd};
      auto initial = lseek(fd, 0, SEEK_END);
      if (initial < 0)
        throw std::runtime_error("Station record stream seek failed");
      size_t received = 0;
      {
        std::ofstream out(prefix.string() + ".records.jsonl", std::ios::binary);
        if (!out)
          throw std::runtime_error("Capture output unavailable");
        double deadline = mono() + 10;
        while (mono() < deadline) {
          if (!c.radio()["running"].truth())
            throw std::runtime_error(
                "Station stopped or became stale during capture");
          char bytes[65536];
          ssize_t n = read(fd, bytes, sizeof bytes);
          if (n < 0) {
            if (errno == EINTR)
              continue;
            throw std::runtime_error("Station record stream read failed");
          }
          if (n) {
            received += size_t(n);
            if (received > 8 * 1024 * 1024)
              throw std::runtime_error("Diagnostic byte budget exceeded");
            out.write(bytes, n);
            if (!out)
              throw std::runtime_error("Capture output write failed");
          } else {
            struct stat named{}, opened{};
            if (stat(log.c_str(), &named) || fstat(fd, &opened))
              throw std::runtime_error("Station stream stat failed");
            if (named.st_ino != opened.st_ino)
              throw std::runtime_error(
                  "Station log rotated; repeat capture from current stream");
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
          }
        }
      }
      J manifest = J::obj({{"id", id},
                           {"started_unix", start},
                           {"finished_unix", wall()},
                           {"radio_bytes", J(received)},
                           {"history_bytes_skipped", J(initial)},
                           {"source", "existing station stream"},
                           {"opens_swd", false},
                           {"controls_camera_recording", false},
                           {"tracker_poses_available", false}});
      writefile(prefix.string() + ".manifest.json", dump(manifest, 2));
      bundle(prefix);
      std::lock_guard<std::mutex> g(lock);
      job(id)["state"] = "complete";
      job(id)["manifest"] = manifest;
    } catch (const std::exception &e) {
      std::lock_guard<std::mutex> g(lock);
      job(id)["state"] = "failed";
      job(id)["error"] = e.what();
    }
    {
      std::lock_guard<std::mutex> g(lock);
      active.clear();
    }
  }
  std::string start_capture() {
    std::lock_guard<std::mutex> g(lock);
    if (!active.empty())
      return {};
    auto id = uuid();
    active = id;
    jobs.emplace_back(id, J::obj({{"id", id},
                                  {"state", "queued"},
                                  {"kind", "existing_station_stream"}}));
    while (jobs.size() > 20)
      jobs.erase(jobs.begin());
    for (auto &worker : workers)
      if (worker.joinable())
        worker.join();
    workers.clear();
    workers.emplace_back([this, id] { capture_worker(id); });
    return id;
  }
  Response dispatch(const std::string &method, const std::string &target,
                    const std::map<std::string, std::string> &headers) {
    auto header = [&](const char *k, const char *fallback = "") {
      auto i = headers.find(k);
      return i == headers.end() ? std::string(fallback) : i->second;
    };
    if (method == "POST") {
      auto host = header("host"), origin = header("origin");
      bool app =
          target == "/profiles/game" || target == "/profiles/service" ||
          std::regex_match(target, std::regex("/trackers/[0-9a-f]{8}/update"));
      if ((target != "/capture" && target != "/radio/start" &&
           target != "/radio/stop" && !app) ||
          !std::regex_match(host,
                            std::regex("(127\\.0\\.0\\.1|localhost):[0-9]+")) ||
          (!origin.empty() && origin != "http://" + host) ||
          !header("transfer-encoding").empty())
        return error(403);
      if (header("content-length", "0") != "0")
        return error(400);
      if (app || target.rfind("/radio/", 0) == 0) {
        if (capturing())
          return json(
              J::obj({{"error",
                       app ? "Radio operation or capture already in progress"
                           : "Wait for capture cleanup"}}),
              409);
        std::unique_lock<std::mutex> g(radio_lock, std::try_to_lock);
        if (!g.owns_lock())
          return json(
              J::obj({{"error",
                       app ? "Radio operation or capture already in progress"
                           : "Radio operation already in progress"}}),
              409);
        try {
          auto c = control();
          J value;
          if (app)
            value = target.rfind("/profiles/", 0) == 0
                        ? c.select(target.substr(10))
                        : c.update(target.substr(10, 8), c.radio());
          else
            value = target == "/radio/start" ? c.select("game") : c.stop();
          return json(value, 202);
        } catch (const Timeout &e) {
          return json(J::obj({{"error", app ? e.what()
                                            : "Service operation timed out; "
                                              "check status before retrying"}}),
                      app ? 409 : 504);
        } catch (const std::exception &e) {
          return json(J::obj({{"error", e.what()}}), 409);
        }
      }
      if (!control().radio()["running"].truth())
        return json(J::obj({{"error", "Station is not running; diagnostics do "
                                      "not start RF probes"}}),
                    409);
      auto id = start_capture();
      return json(id.empty() ? J::obj({{"error", "Capture already running"}})
                             : J::obj({{"id", id}}),
                  id.empty() ? 409 : 202);
    }
    if (method != "GET")
      return error(501, "Unsupported method ('" + method + "')");
    std::string path = target.substr(0, target.find_first_of("?#"));
    if (path == "/") {
      Response response;
      response.content = "text/html; charset=utf-8";
      response.body = PAGE;
      return response;
    }
    std::smatch match;
    if (std::regex_match(
            path, match,
            std::regex(
                "/dataset/([A-Za-z0-9_.-]+\\.dataset)/([A-Za-z0-9_.-]+)"))) {
      auto dir = fs::weakly_canonical(datasets),
           file =
               fs::weakly_canonical(datasets / match[1].str() / match[2].str());
      auto rel = file.lexically_relative(dir);
      if (rel.empty() || *rel.begin() == ".." || !fs::is_regular_file(file))
        return error(404);
      return file_response(file, "application/octet-stream", true);
    }
    if (std::regex_match(path, match,
                         std::regex("/capture/([0-9a-f]{32})(/bundle)?"))) {
      J j;
      {
        std::lock_guard<std::mutex> g(lock);
        for (const auto &p : jobs)
          if (p.first == match[1].str())
            j = p.second;
      }
      if (!j.truth())
        return error(404);
      if (!match[2].matched)
        return json(j);
      auto p = root / ("api-" + match[1].str() + ".tar.gz");
      if ((!(j["state"] == "complete") && !(j["state"] == "failed")) ||
          !fs::is_regular_file(p))
        return error(409);
      return file_response(p, "application/gzip", false);
    }
    if (path != "/status")
      return error(404);
    try {
      return json(status());
    } catch (const std::exception &e) {
      return error(503, e.what());
    }
  }
};
} // namespace frame
