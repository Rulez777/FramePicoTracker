#include "native_test_runtime.hpp"
#include "calibration_discovery.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
namespace {
int checks=0;
void check(bool ok){++checks;if(!ok)throw std::runtime_error("calibration discovery check="+std::to_string(checks));}
template<class F>std::string rejects(F f){try{f();}catch(const std::exception& e){++checks;return e.what();}throw std::runtime_error("Expected calibration rejection");}
void put(const std::filesystem::path& path){std::ofstream out(path);out<<"{}";if(!out)throw std::runtime_error("Fixture write failed");}
}
int main(){try{
    const auto base=std::filesystem::absolute("build/calibration-discovery-"+
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    const auto dataset=base/"this-run/Deckard_1.dataset";
    std::filesystem::create_directories(dataset);
    const auto old=base/"prior-attempt/preflight-camera-recordings/Deckard_0.dataset";
    std::filesystem::create_directories(old);put(old/"old_device_config.json");
    put(dataset/"device_config.json");
    std::filesystem::create_directory(dataset/"datasetConfig");
    put(dataset/"datasetConfig/nested_device_config.json");
    // Reproduce the former immediate-load failure, despite historical files.
    const auto missing=rejects([&]{pico::unique_active_device(dataset);});
    check(missing.find("candidates=0")!=std::string::npos);
    check(missing.find(dataset.string())!=std::string::npos);
    int64_t now=0;int polls=0,guards=0;
    auto clock=[&]{return now;};
    auto guard=[&]{++guards;};
    auto delayed=[&](int64_t ns){now+=ns;if(ns==100000000LL)++polls;
        if(now==300000000LL)put(dataset/"sensorRigCalibration.json");
        if(now==700000000LL)put(dataset/"live_device_config.json");};
    pico::await_active_calibration(dataset,clock,delayed,guard);
    check(now==900000000LL&&polls==7&&guards>=9);
    check(pico::unique_active_device(dataset)==dataset/"live_device_config.json");
    // A second candidate arriving during settling must fail, listing both.
    now=0;
    const auto ambiguous=rejects([&]{pico::await_active_calibration(dataset,clock,
        [&](int64_t ns){now+=ns;put(dataset/"second_device_config.json");},guard);});
    check(now==200000000LL);
    check(ambiguous.find("candidates=2")!=std::string::npos);
    check(ambiguous.find((dataset/"live_device_config.json").string())!=std::string::npos);
    check(ambiguous.find((dataset/"second_device_config.json").string())!=std::string::npos);
    const auto absent=base/"absent.dataset";std::filesystem::create_directory(absent);
    now=0;polls=0;
    const auto timeout=rejects([&]{pico::await_active_calibration(absent,clock,
        [&](int64_t ns){now+=ns;if(ns==100000000LL)++polls;},guard);});
    check(now==10200000000LL&&polls==100);
    check(timeout.find("candidates=0")!=std::string::npos);
    put(absent/"only_device_config.json");now=0;
    const auto no_rig=rejects([&]{pico::await_active_calibration(absent,clock,
        [&](int64_t ns){now+=ns;},guard);});
    check(now==10200000000LL);
    check(no_rig.find("Active calibration missing after wait")!=std::string::npos);
    check(no_rig.find((absent/"only_device_config.json").string())!=std::string::npos);
    now=0;
    check(rejects([&]{pico::await_active_calibration(absent,clock,[&](int64_t ns){now+=ns;},
        []{throw std::runtime_error("tracking/cancel guard");});})=="tracking/cancel guard");
    check(now==0);
    // Count nonregular matches; the existing loader rejects their type.
    std::filesystem::create_directory(absent/"directory_device_config.json");
    check(pico::active_device_candidates(absent).size()==2);
    const auto direct=base/"direct";const auto older=direct/"older.dataset",newer=direct/"newer.dataset";
    for(const auto& p:{older,newer}){std::filesystem::create_directories(p);
        put(p/"sensorRigCalibration.json");put(p/"active_device_config.json");}
    std::filesystem::last_write_time(older,std::filesystem::last_write_time(newer)-std::chrono::seconds(10));
    check(pico::direct_calibration_dataset(direct)==newer);
    check(pico::direct_calibration_dataset(older)==older); // explicit dataset/documentsRoot
    const auto incomplete=direct/"incomplete.dataset";std::filesystem::create_directory(incomplete);
    put(incomplete/"sensorRigCalibration.json");
    check(pico::direct_calibration_dataset(direct)==newer);
    put(newer/"ambiguous_device_config.json");
    check(pico::direct_calibration_dataset(direct)==older);
    (void)rejects([&]{pico::direct_calibration_dataset(incomplete);});
    std::cout<<checks<<" calibration discovery checks passed; evidence="<<base<<'\n';
    return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
