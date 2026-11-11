#include "native_test_runtime.hpp"
#include "live_startup.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace {
int checks = 0;
void check(bool actual,const char* name,int line) {
    ++checks;
    std::cout << name << " line=" << line << " actual=" << actual << " expected=1\n";
    if (!actual)throw std::runtime_error("Startup regression failed");
}
#define REQUIRE(value) check(bool(value),#value,__LINE__)
template<class F> bool rejects(F operation) {
    try { operation(); } catch (const std::exception&) { return true; }
    return false;
}
}
int main() {
    try {
        using namespace pico;
        REQUIRE(camera_readiness_ns == 12000000000LL);
        for (int fault = 0; fault < 4; ++fault) {
            int64_t now = 1;
            int polls = 0;
            int guards = 0;
            auto run = [&] {
                await_camera_ready([&] { return now; },[&] {
                    ++guards;
                    if (fault == 1 && guards == 2)throw std::runtime_error("Cancelled");
                },[&] {
                    ++polls;
                    if (fault == 2)throw std::runtime_error("Recorder failed");
                    return false;
                },[&] { now = fault == 3 ? 0 : now + camera_readiness_ns; });
            };
            REQUIRE(rejects(run) == (fault == 1 || fault == 2));
            REQUIRE(polls == 1);
        }
        { int64_t now = 1;
          int polls = 0;
          await_camera_ready([&] { return now; },[] {},[&] { return ++polls == 2; },[&] { now += camera_readiness_ns - 1; });
          REQUIRE(polls == 2); }
        for (int fault = 0; fault < 4; ++fault) {
            std::vector<std::string> order;
            auto stop = [&] { stop_camera_worker([&] {
                order.push_back("worker");
                if (fault & 1)throw std::runtime_error("Join failed");
            },[&] {
                order.push_back("camera");
                if (fault & 2)throw std::runtime_error("Camera close failed");
            }); };
            REQUIRE(!rejects(stop));
            REQUIRE((order == std::vector<std::string>{"worker","camera"}));
        }
        for (int fault = 0; fault < 2; ++fault) {
            bool stopping = false;
            int calls = 0;
            REQUIRE(rejects([&] { stop_factory_dispatch(stopping,[&] {
                REQUIRE(stopping);
                ++calls;
                if (fault)throw std::runtime_error("Dispatch stop failed");
            }); }) == (fault != 0));
            REQUIRE(stopping && calls == 1);
        }
        std::cout << "live_startup_tests checks=" << checks << " PASS\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "live_startup_tests line=" << __LINE__ << " actual=" << error.what() << " expected=all checks pass\n";
        return 1;
    }
}
