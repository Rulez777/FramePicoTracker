#pragma once
#include "agent.hpp"
#include <set>
namespace frame {
inline std::string http_date() {
  time_t now = time(nullptr);
  tm t{};
  gmtime_r(&now, &t);
  char b[80];
  strftime(b, sizeof b, "%a, %d %b %Y %H:%M:%S GMT", &t);
  return b;
}
inline bool sendall(int fd, const std::string &s) {
  size_t n = 0;
  while (n < s.size()) {
    ssize_t k = send(fd, s.data() + n, s.size() - n, MSG_NOSIGNAL);
    if (k < 0 && errno == EINTR)
      continue;
    if (k <= 0)
      return false;
    n += size_t(k);
  }
  return true;
}
struct Server {
  Agent &agent;
  int listener = -1;
  unsigned short port = 0;
  std::atomic<bool> stopping{false};
  std::mutex connections_lock, log_lock;
  std::condition_variable connections_done;
  std::set<int> connections;
  explicit Server(Agent &a, unsigned short requested) : agent(a) {
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0)
      throw std::runtime_error("socket failed");
    int yes = 1;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
    fcntl(listener, F_SETFD, FD_CLOEXEC);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(requested);
    if (bind(listener, reinterpret_cast<sockaddr *>(&addr), sizeof addr) ||
        listen(listener, 5)) {
      close(listener);
      listener = -1;
      throw std::runtime_error("bind/listen failed");
    }
    socklen_t len = sizeof addr;
    getsockname(listener, reinterpret_cast<sockaddr *>(&addr), &len);
    port = ntohs(addr.sin_port);
  }
  ~Server() {
    stop();
    if (listener >= 0)
      close(listener);
  }
  void stop() {
    stopping = true;
    if (listener >= 0)
      shutdown(listener, SHUT_RDWR);
    std::unique_lock<std::mutex> g(connections_lock);
    for (int fd : connections)
      shutdown(fd, SHUT_RDWR);
    connections_done.wait(g, [&] { return connections.empty(); });
  }
  void log(const std::string &s) {
    std::lock_guard<std::mutex> g(log_lock);
    time_t now = time(nullptr);
    tm t{};
    localtime_r(&now, &t);
    char date[80];
    strftime(date, sizeof date, "%d/%b/%Y %H:%M:%S", &t);
    std::cerr << "127.0.0.1 - - [" << date << "] " << s << '\n';
  }
  void connection(int fd) {
    std::string input;
    auto line = [&](std::string &out) {
      for (;;) {
        auto n = input.find('\n');
        if (n != std::string::npos) {
          if (n + 1 > 65536)
            return false;
          out = input.substr(0, n);
          input.erase(0, n + 1);
          if (!out.empty() && out.back() == '\r')
            out.pop_back();
          return true;
        }
        if (input.size() > 65536)
          return false;
        char bytes[4096];
        ssize_t r = recv(fd, bytes, sizeof bytes, 0);
        if (r < 0 && errno == EINTR)
          continue;
        if (r <= 0)
          return false;
        input.append(bytes, size_t(r));
      }
    };
    // BaseHTTPRequestHandler defaults to HTTP/1.0 replies and closes each
    // connection even when the request uses HTTP/1.1. Parse HTTP/1.1 fully,
    // but preserve that observable connection lifetime.
    std::string request;
    if (!line(request) || request.empty())
      return;
    std::istringstream in(request);
    std::string method, target, version, extra;
    in >> method >> target >> version;
    Response response;
    bool head = method == "HEAD";
    if (method.empty() || target.empty() ||
        (version != "HTTP/1.1" && version != "HTTP/1.0") || (in >> extra))
      response = error(400, "Bad request syntax ('" + request + "')");
    else {
      std::map<std::string, std::string> headers;
      std::string h;
      bool valid = true;
      unsigned count = 0;
      for (;;) {
        if (!line(h)) {
          valid = false;
          break;
        }
        if (h.empty())
          break;
        if (++count > 100) {
          valid = false;
          break;
        }
        auto colon = h.find(':');
        if (colon == std::string::npos) {
          valid = false;
          break;
        }
        auto key = h.substr(0, colon);
        for (auto &c : key)
          c = char(std::tolower(static_cast<unsigned char>(c)));
        if (headers.find(key) == headers.end())
          headers.emplace(key, trim(h.substr(colon + 1)));
      }
      if (!valid)
        response = error(400);
      else {
        // All authorized POST routes require an exactly zero length
        // body. Reject disallowed framing without waiting for attacker
        // supplied bodies, matching the source's immediate response.
        if (!valid)
          response = error(400);
        else
          try {
            response = agent.dispatch(method, target, headers);
          } catch (const std::exception &e) {
            log(e.what());
            return;
          }
      }
    }
    if (response.error)
      log("code " + std::to_string(response.code) + ", message " +
          response.message);
    log("\"" + request + "\" " + std::to_string(response.code) + " -");
    std::string wire =
        "HTTP/1.0 " + std::to_string(response.code) + " " +
        (response.error ? response.message : reason(response.code).first) +
        "\r\nServer: BaseHTTP/0.6 C++17\r\nDate: " + http_date() + "\r\n";
    if (response.error)
      wire += "Connection: close\r\n";
    wire += "Content-Type: " + response.content + "\r\nContent-Length: " +
            std::to_string(response.file.empty() ? response.body.size()
                                                 : response.bytes) +
            "\r\n\r\n";
    if (!head)
      wire += response.body;
    if (!sendall(fd, wire))
      return;
    if (!head && !response.file.empty()) {
      std::ifstream file(response.file, std::ios::binary);
      uintmax_t remaining = response.bytes;
      char buffer[65536];
      while (file && (!response.capped || remaining)) {
        size_t count =
            response.capped
                ? size_t(std::min(remaining, uintmax_t(sizeof buffer)))
                : sizeof buffer;
        size_t n = size_t(file.rdbuf()->sgetn(buffer, std::streamsize(count)));
        if (!n)
          break;
        if (!sendall(fd, std::string(buffer, n)))
          break;
        if (response.capped)
          remaining -= n;
      }
    }
  }
  void serve() {
    while (!stopping) {
      pollfd p{listener, POLLIN, 0};
      int r = poll(&p, 1, 500);
      if (r < 0 && errno == EINTR)
        continue;
      if (stopping)
        break;
      if (r <= 0)
        continue;
      int fd = accept(listener, nullptr, nullptr);
      if (fd < 0)
        continue;
      fcntl(fd, F_SETFD, FD_CLOEXEC);
      {
        std::lock_guard<std::mutex> g(connections_lock);
        if (stopping) {
          close(fd);
          break;
        }
        connections.insert(fd);
      }
      try {
        std::thread([this, fd] {
          try {
            connection(fd);
          } catch (const std::exception &e) {
            log(e.what());
          }
          close(fd);
          std::lock_guard<std::mutex> g(connections_lock);
          connections.erase(fd);
          connections_done.notify_all();
        }).detach();
      } catch (...) {
        close(fd);
        std::lock_guard<std::mutex> g(connections_lock);
        connections.erase(fd);
        connections_done.notify_all();
      }
    }
  }
};
} // namespace frame
