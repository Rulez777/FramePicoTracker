#include "http.hpp"
#include <cstdlib>
namespace {
using namespace frame;
void check(bool ok, const std::string &what) {
  if (!ok)
    throw std::runtime_error(what);
}
struct Reply {
  int code;
  std::string body, raw;
  J value() const { return Parser(body).parse(); }
};
Reply request(unsigned short port, const std::string &method,
              const std::string &path, const std::string &extra = "",
              bool fragmented = false) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  check(fd >= 0, "client socket");
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = htons(port);
  check(connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof addr) == 0,
        "client connect");
  std::string wire = method + " " + path +
                     " HTTP/1.1\r\nHost: localhost:" + std::to_string(port) +
                     "\r\n" + extra + "\r\n";
  if (fragmented) {
    for (char c : wire)
      check(sendall(fd, std::string(1, c)), "fragment write");
  } else
    check(sendall(fd, wire), "client write");
  std::string response;
  char b[4096];
  for (;;) {
    ssize_t n = recv(fd, b, sizeof b, 0);
    if (n < 0 && errno == EINTR)
      continue;
    if (n <= 0)
      break;
    response.append(b, size_t(n));
  }
  close(fd);
  auto split = response.find("\r\n\r\n");
  check(split != std::string::npos, "HTTP response headers: " + response);
  int code = 0;
  check(std::sscanf(response.c_str(), "HTTP/1.0 %d", &code) == 1,
        "HTTP status");
  auto length = response.find("Content-Length: ");
  check(length != std::string::npos, "Content-Length");
  check(std::stoull(response.substr(length + 16)) ==
            response.size() - split - 4,
        "response byte length");
  return {code, response.substr(split + 4), response};
}
struct Fake {
  std::mutex mutex;
  std::vector<std::vector<std::string>> calls;
  std::atomic<bool> legacy{false}, timeout{false};
  Result operator()(const std::vector<std::string> &args, double seconds) {
    std::lock_guard<std::mutex> g(mutex);
    check(seconds == 30, "systemctl timeout");
    check(args.size() >= 4 && args[0] == "systemctl" && args[1] == "--user",
          "exact subprocess prefix");
    calls.push_back(args);
    if (timeout)
      throw Timeout("fixture timeout");
    if (args[2] == "show") {
      check(args.size() == 5 &&
                args[4] ==
                    "--property=ActiveState,InvocationID,MainPID,FragmentPath",
            "show argv");
      return {0,
              std::string("ActiveState=") +
                  (legacy && args[3] == "frame-pico-radio.service"
                       ? "active"
                       : "inactive") +
                  "\nInvocationID=\nMainPID=0\nFragmentPath=/fixture/unit\n",
              ""};
    }
    if (args[2] == "stop") {
      legacy = false;
      return {0, "", ""};
    }
    throw std::runtime_error("Unexpected command");
  }
};
} // namespace
int main() {
  using namespace frame;
  fs::path root;
  try {
    root = fs::temp_directory_path() / ("pico-api-test-" + uuid());
    fs::create_directories(root / "datasets" / "sample.dataset");
    writefile(root / "datasets" / "sample.dataset" / "blob.bin",
              std::string("a\0b", 3));
    writefile(root / "a.summary.json", "{\"value\": 7}");
    writefile(root / "bad.summary.json", "{");
    writefile(root / "ir-tx1.json",
              "{\"active_ir_tx_probe\": true, \"slots_granted\": 2}");
    writefile(root / "ir-tx2.recovery.json", "{\"active_ir_tx_probe\": true}");
    writefile(root / "tracker-camera101-scan0.json",
              "{\"frames\":[{\"blobs\":[]},{\"blobs\":[1,2,3]}]}");
    Fake fake;
    Agent agent(root, root / "datasets",
                [&](const auto &a, double s) { return fake(a, s); });
    Server server(agent, 0);
    std::thread serving([&] { server.serve(); });
    struct Stop {
      Server &s;
      std::thread &t;
      ~Stop() {
        s.stop();
        if (t.joinable())
          t.join();
      }
    } stop{server, serving};
    auto req = [&](const std::string &m, const std::string &p,
                   const std::string &h = "") {
      return request(server.port, m, p, h);
    };
    auto page = request(server.port, "GET", "/?query=yes", "", true);
    check(page.code == 200 && page.body == PAGE,
          "root page exact bytes, partial reads, query");
    auto s = req("GET", "/status");
    check(s.code == 200, "status");
    auto state = s.value();
    check(state["mode"] == "local_frame_tracker_api" &&
              !state["radio_service"]["running"].yes(),
          "status inactive radio");
    check(state["radio_service"]["error"] == "Radio service is inactive",
          "radio reason");
    check(state["application"]["profiles"]["game"]["reason"] ==
              "Installed profile catalog unavailable",
          "catalog error");
    check(state["camera_scan"]["frames"].number() == 2 &&
              state["camera_scan"]["max_blobs"].number() == 3,
          "camera counts");
    check(state["recent_radio_runs"].array.size() == 2 &&
              state["recent_active_probes"].array.size() == 1,
          "report filtering");
    check(state["datasets"].array.size() == 1, "dataset inventory");
    check(std::regex_match(state["utc"].text,
                           std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}:[0-"
                                      "9]{2}:[0-9]{2}(\\.[0-9]{6})?\\+00:00")),
          "UTC format");
    check(req("GET", "/dataset/sample.dataset/blob.bin?download=1").body ==
              std::string("a\0b", 3),
          "binary dataset");
    for (const auto &p :
         {"/missing", "/dataset/sample.dataset/missing", "/dataset/../secret",
          "/capture/00000000000000000000000000000000",
          "/capture/00000000000000000000000000000000/bundle"})
      check(req("GET", p).code == 404, "404 route");
    check(req("PUT", "/status").code == 501, "unsupported method");
    check(req("POST", "/capture").code == 409, "inactive capture");
    check(req("POST", "/capture", "Origin: http://evil\r\n").code == 403,
          "origin rejection");
    check(req("POST", "/capture", "Transfer-Encoding: chunked\r\n").code == 403,
          "transfer encoding");
    check(req("POST", "/capture", "Content-Length: 00\r\n").code == 400,
          "exact empty body requirement");
    check(req("POST", "/capture", "Content-Length: 1\r\n").code == 400,
          "nonempty body rejected without waiting");
    check(req("GET", "/", "Content-Length: 1\r\n").code == 200,
          "GET does not wait for an unused body");
    check(agent.dispatch("POST", "/capture", {{"host", "example.com:18741"}})
                  .code == 403,
          "nonlocal Host");
    check(
        req("POST", "/capture",
            "Origin: http://localhost:" + std::to_string(server.port) + "\r\n")
                .code == 409,
        "matching origin reaches handler");
    int slow = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in slow_addr{};
    slow_addr.sin_family = AF_INET;
    slow_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    slow_addr.sin_port = htons(server.port);
    check(connect(slow, reinterpret_cast<sockaddr *>(&slow_addr),
                  sizeof slow_addr) == 0,
          "slow client connect");
    sendall(slow, "GET / HTTP/1.1\r\nHost:");
    check(req("GET", "/status").code == 200, "independent connection threads");
    close(slow);
#ifndef FRAME_API_HOST_TEST
    writefile(root / "secret", "private");
    fs::create_symlink(root / "secret",
                       root / "datasets" / "sample.dataset" / "escape");
    check(req("GET", "/dataset/sample.dataset/escape").code == 404,
          "dataset symlink escape");
#endif
    check(req("POST", "/capture?x=1").code == 403, "POST raw target");
    for (const auto &p :
         {"/profiles/game", "/profiles/service", "/radio/start"}) {
      auto r = req("POST", p);
      check(r.code == 409 &&
                r.value()["error"] == "Installed profile catalog unavailable",
            "profile endpoint catalog rejection");
    }
    auto updated = req("POST", "/trackers/1234abcd/update");
    check(updated.code == 409 &&
              updated.value()["error"] ==
                  "Fresh stock firmware capability proof required",
          "update endpoint");
    check(req("POST", "/trackers/1234ABCD/update").code == 403,
          "lowercase tracker route");
    check(req("POST", "/radio/stop").code == 202, "stop endpoint");
    fake.timeout = true;
    check(req("POST", "/radio/stop").code == 504, "stop timeout");
    fake.timeout = false;
    writefile(root / "tracker-camera101-scan0.json", "{");
    check(req("GET", "/status").code == 503, "invalid scan status");
    fs::remove(root / "tracker-camera101-scan0.json");
    fake.legacy = true;
    writefile(root / "radio-live-records.jsonl", "history\n");
    std::atomic<bool> publishing{true};
    std::thread publisher([&] {
      while (publishing) {
        writefile(root / "radio-live.next.json",
                  dump(J::obj({{"running", true},
                               {"updated_unix", wall()},
                               {"trackers", J::list()}})));
#ifdef FRAME_API_HOST_TEST
        check(MoveFileExW((root / "radio-live.next.json").c_str(),
                          (root / "radio-live.json").c_str(),
                          MOVEFILE_REPLACE_EXISTING) != 0,
              "atomic live fixture");
#else
        fs::rename(root / "radio-live.next.json", root / "radio-live.json");
#endif
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }
    });
    struct PublisherStop {
      std::atomic<bool> &p;
      std::thread &t;
      ~PublisherStop() {
        p = false;
        if (t.joinable())
          t.join();
      }
    } ps{publishing, publisher};
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto capture = req("POST", "/capture");
    check(capture.code == 202, "start capture");
    auto id = capture.value()["id"].text;
    check(std::regex_match(
              id, std::regex("[0-9a-f]{12}4[0-9a-f]{3}[89ab][0-9a-f]{15}")),
          "UUID4 bits");
    check(req("POST", "/capture").code == 409, "one active capture");
    check(req("POST", "/radio/stop").value()["error"] ==
              "Wait for capture cleanup",
          "capture exclusion");
    check(req("POST", "/profiles/game").value()["error"] ==
              "Radio operation or capture already in progress",
          "profile exclusion");
    check(req("GET", "/capture/" + id + "/bundle").code == 409,
          "pending bundle");
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    {
      std::ofstream out(root / "radio-live-records.jsonl",
                        std::ios::app | std::ios::binary);
      out << "new record\n";
    }
    double deadline = mono() + 13;
    J job;
    while (mono() < deadline) {
      job = req("GET", "/capture/" + id).value();
      if (job["state"] == "complete" || job["state"] == "failed")
        break;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    check(job["state"] == "complete", "capture completion: " + dump(job));
    check(job["manifest"]["radio_bytes"].number() == 11 &&
              job["manifest"]["history_bytes_skipped"].number() == 8,
          "capture skips history");
    check(readfile(root / ("api-" + id + ".records.jsonl")) == "new record\n",
          "capture record bytes");
    auto archive = req("GET", "/capture/" + id + "/bundle");
    check(archive.code == 200 &&
              archive.body.substr(0, 3) == std::string("\x1f\x8b\x08", 3),
          "gzip bundle route");
    if (const char *exported = std::getenv("FRAME_API_TEST_BUNDLE"))
      writefile(exported, archive.body);
#ifndef FRAME_API_HOST_TEST
    auto listing =
        run({"tar", "-tzf", (root / ("api-" + id + ".tar.gz")).string()}, 5);
    check(listing.code == 0 && listing.out == "api-" + id +
                                                  ".records.jsonl\napi-" + id +
                                                  ".manifest.json\n",
          "GNU tar reads both members");
    auto processes = run(
        {"/bin/sh", "-c", "printf 'out\\r\\n'; printf 'err\\r' >&2; exit 7"},
        5);
    check(processes.code == 7 && processes.out == "out\n" &&
              processes.err == "err\n",
          "POSIX stdout/stderr/status/text conversion");
    bool timed = false;
    try {
      run({"/bin/sleep", "1"}, .02);
    } catch (const Timeout &) {
      timed = true;
    }
    check(timed, "POSIX subprocess timeout");
#endif
    publishing = false;
    publisher.join();
    writefile(root / "radio-live.json",
              dump(J::obj({{"running", true}, {"updated_unix", wall() - 3}})));
    check(req("POST", "/capture").code == 409, "stale stream");
    writefile(root / "radio-live.json",
              "{\"running\":true,\"updated_unix\":NaN}");
    check(req("POST", "/capture").code == 409, "nonfinite freshness rejected");
    // Failure path: capture accepted on a fresh state but its record file is
    // absent.
    fs::remove(root / "radio-live-records.jsonl");
    writefile(root / "radio-live.json",
              dump(J::obj({{"running", true}, {"updated_unix", wall()}})));
    auto failure = req("POST", "/capture");
    check(failure.code == 202, "failure capture accepted");
    auto fid = failure.value()["id"].text;
    for (int i = 0; i < 100; ++i) {
      job = req("GET", "/capture/" + fid).value();
      if (job["state"] == "failed")
        break;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    check(job["error"] == "Station record stream is unavailable",
          "capture failure reason");
    check(req("GET", "/capture/" + fid + "/bundle").code == 409,
          "failed missing bundle");
    auto wait_failed = [&](const std::string &cid, const char *expected) {
      J last;
      for (int i = 0; i < 200; ++i) {
        last = req("GET", "/capture/" + cid).value();
        if (last["state"] == "failed")
          break;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
      check(last["error"] == expected, "capture failure: " + dump(last));
    };
    auto fresh = [&] {
      while (agent.capturing())
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      writefile(root / "radio-live.json",
                dump(J::obj({{"running", true}, {"updated_unix", wall()}})));
      writefile(root / "radio-live-records.jsonl", "");
    };
#ifndef FRAME_API_HOST_TEST
    fresh();
    auto rotated = req("POST", "/capture").value()["id"].text;
    std::this_thread::sleep_for(std::chrono::milliseconds(80));
    writefile(root / "new-records", "");
    fs::rename(root / "new-records", root / "radio-live-records.jsonl");
    wait_failed(rotated,
                "Station log rotated; repeat capture from current stream");
#endif
    fresh();
    auto stopped = req("POST", "/capture").value()["id"].text;
    std::this_thread::sleep_for(std::chrono::milliseconds(80));
    writefile(root / "radio-live.json",
              dump(J::obj({{"running", false}, {"updated_unix", wall()}})));
    wait_failed(stopped, "Station stopped or became stale during capture");
    fresh();
    auto over = req("POST", "/capture").value()["id"].text;
    std::this_thread::sleep_for(std::chrono::milliseconds(80));
    writefile(root / "radio-live-records.jsonl",
              std::string(8 * 1024 * 1024 + 1, 'x'));
    wait_failed(over, "Diagnostic byte budget exceeded");
    auto unicode =
        Parser("{\"s\":\"\\ud83d\\ude80\",\"a\":[true,null,-1.25e2]}").parse();
    check(dump(unicode) ==
              "{\"s\": \"\\ud83d\\ude80\", \"a\": [true, null, -125.0]}",
          "JSON Unicode and number preservation");
    check(J(1e-4).text == "0.0001" && J(1e-5).text == "1e-05" &&
              J(1e15).text == "1000000000000000.0" && J(1e16).text == "1e+16" &&
              J(-0.0).text == "-0.0",
          "Python float repr thresholds");
    for (bool start : {false, true})
      check(camera_command(
                start,
                [&](const std::vector<std::string> &args, double seconds) {
                  check(seconds == 5 &&
                            args ==
                                std::vector<std::string>{
                                    "/opt/steamvr/bin/linuxarm64/vrcmd",
                                    "--cmd", "0",
                                    start ? "start_tracking_recording 1 0 0"
                                          : "stop_tracking_recording"},
                        "camera helper argv");
                  return Result{0, "response: ok\n", ""};
                }) == "response: ok",
            "camera helper output");
    server.stop();
    serving.join();
    fs::remove_all(root);
    std::cout << "frame_debug_agent tests passed\n";
    return 0;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}
