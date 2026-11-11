#pragma once
// Windows-only test adapter. Production builds never include this file.
// The HTTP, policy, JSON and capture code runs unchanged; subprocesses are
// injected by the test. Native POSIX subprocess behavior still needs Linux.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <fcntl.h>
#include <filesystem>
#include <io.h>
#include <map>
#include <mutex>
#include <set>
#include <sys/stat.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
using ssize_t = long long;
using socklen_t = int;
constexpr int MSG_NOSIGNAL = 0, SHUT_RDWR = SD_BOTH, O_CLOEXEC = 0,
              O_NONBLOCK = 0, F_SETFL = 0, F_SETFD = 1, FD_CLOEXEC = 1,
              X_OK = 0;
inline int host_fcntl(int, int, int) { return 0; }
inline tm *gmtime_r(const time_t *t, tm *out) {
  return gmtime_s(out, t) ? nullptr : out;
}
inline tm *localtime_r(const time_t *t, tm *out) {
  return localtime_s(out, t) ? nullptr : out;
}
namespace host {
inline std::mutex mutex;
inline std::map<int, SOCKET> sockets;
inline int next = 100000;
inline int add(SOCKET s) {
  if (s == INVALID_SOCKET)
    return -1;
  std::lock_guard<std::mutex> g(mutex);
  int n = next++;
  sockets[n] = s;
  return n;
}
inline SOCKET get(int n) {
  std::lock_guard<std::mutex> g(mutex);
  auto i = sockets.find(n);
  return i == sockets.end() ? INVALID_SOCKET : i->second;
}
inline int init() {
  WSADATA w;
  return WSAStartup(MAKEWORD(2, 2), &w);
}
inline const int initialized = init();
} // namespace host
inline int host_socket(int a, int b, int c) {
  return host::add(::socket(a, b, c));
}
inline int host_bind(int fd, const sockaddr *a, int n) {
  return ::bind(host::get(fd), a, n);
}
inline int host_connect(int fd, const sockaddr *a, int n) {
  return ::connect(host::get(fd), a, n);
}
inline int host_listen(int fd, int n) { return ::listen(host::get(fd), n); }
inline int host_accept(int fd, sockaddr *a, int *n) {
  return host::add(::accept(host::get(fd), a, n));
}
inline int host_shutdown(int fd, int n) { return ::shutdown(host::get(fd), n); }
inline int host_getsockname(int fd, sockaddr *a, int *n) {
  return ::getsockname(host::get(fd), a, n);
}
inline int host_setsockopt(int fd, int a, int b, const void *p, int n) {
  return ::setsockopt(host::get(fd), a, b, static_cast<const char *>(p), n);
}
inline ssize_t host_send(int fd, const void *p, size_t n, int f) {
  return ::send(host::get(fd), static_cast<const char *>(p), int(n), f);
}
inline ssize_t host_recv(int fd, void *p, size_t n, int f) {
  return ::recv(host::get(fd), static_cast<char *>(p), int(n), f);
}
struct host_pollfd {
  int fd;
  short events, revents;
};
inline int host_poll(host_pollfd *p, size_t n, int ms) {
  std::vector<WSAPOLLFD> v;
  for (size_t i = 0; i < n; ++i)
    v.push_back({host::get(p[i].fd), p[i].events, 0});
  int r = WSAPoll(v.data(), ULONG(n), ms);
  for (size_t i = 0; i < n; ++i)
    p[i].revents = v[i].revents;
  return r;
}
inline int host_close(int fd) {
  if (fd == 99999)
    return 0;
  auto s = host::get(fd);
  if (s != INVALID_SOCKET) {
    {
      std::lock_guard<std::mutex> g(host::mutex);
      host::sockets.erase(fd);
    }
    return closesocket(s);
  }
  return _close(fd);
}
inline int host_open(const wchar_t *p, int flags) {
  (void)flags;
  HANDLE h = CreateFileW(
      p, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
      nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (h == INVALID_HANDLE_VALUE) {
    errno = ENOENT;
    return -1;
  }
  return _open_osfhandle(reinterpret_cast<intptr_t>(h), _O_RDONLY | _O_BINARY);
}
inline int host_open(const char *p, int flags) {
  if (std::string(p) == "/dev/urandom")
    return 99999;
  return host_open(std::filesystem::path(p).c_str(), flags);
}
inline ssize_t host_read(int fd, void *p, size_t n) {
  if (fd == 99999) {
    using Random = BOOLEAN(WINAPI *)(PVOID, ULONG);
    auto f = reinterpret_cast<Random>(
        GetProcAddress(GetModuleHandleA("advapi32.dll"), "SystemFunction036"));
    if (!f) {
      LoadLibraryA("advapi32.dll");
      f = reinterpret_cast<Random>(GetProcAddress(
          GetModuleHandleA("advapi32.dll"), "SystemFunction036"));
    }
    return f && f(p, ULONG(n)) ? ssize_t(n) : -1;
  }
  return _read(fd, p, unsigned(n));
}
inline long long host_lseek(int fd, long long n, int whence) {
  return _lseeki64(fd, n, whence);
}
struct host_stat {
  unsigned st_mode = 0644, st_uid = 0, st_gid = 0;
  unsigned long long st_ino = 0;
  struct {
    long long tv_sec = 0;
    long tv_nsec = 0;
  } st_mtim;
};
inline int host_fstat(int fd, struct host_stat *s) {
  HANDLE h = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
  BY_HANDLE_FILE_INFORMATION i{};
  if (!GetFileInformationByHandle(h, &i))
    return -1;
  s->st_ino = (static_cast<unsigned long long>(i.nFileIndexHigh) << 32) |
              i.nFileIndexLow;
  unsigned long long t =
      (static_cast<unsigned long long>(i.ftLastWriteTime.dwHighDateTime)
       << 32) |
      i.ftLastWriteTime.dwLowDateTime;
  t -= 116444736000000000ULL;
  s->st_mtim.tv_sec = static_cast<long long>(t / 10000000);
  s->st_mtim.tv_nsec = long(t % 10000000) * 100;
  return 0;
}
inline int host_stat(const wchar_t *p, struct host_stat *s) {
  int fd = host_open(p, _O_RDONLY | _O_BINARY);
  if (fd < 0)
    return -1;
  int r = host_fstat(fd, s);
  _close(fd);
  return r;
}
struct passwd {
  char *pw_name;
};
struct group {
  char *gr_name;
};
inline int getpwuid_r(unsigned, passwd *, char *, size_t, passwd **p) {
  *p = nullptr;
  return 0;
}
inline int getgrgid_r(unsigned, group *, char *, size_t, group **p) {
  *p = nullptr;
  return 0;
}
#define socket host_socket
#define bind host_bind
#define connect host_connect
#define listen host_listen
#define accept host_accept
#define shutdown host_shutdown
#define getsockname host_getsockname
#define setsockopt host_setsockopt
#define send host_send
#define recv host_recv
#define pollfd host_pollfd
#define poll host_poll
#define fcntl host_fcntl
#define close host_close
#define open host_open
#define read host_read
#define lseek host_lseek
#define stat host_stat
#define fstat host_fstat
#define access _access
