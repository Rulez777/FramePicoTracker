#pragma once
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <optional>
namespace pico {
// Match calibration_snapshot.py: direct children of the selected OCC's dataset,
// not dataset_root, archives, datasetConfig subdirectories or other attempts.
inline std::vector<std::filesystem::path> active_device_candidates(const std::filesystem::path& dataset){
    std::vector<std::filesystem::path> out;
    const std::string suffix="_device_config.json";
    for(const auto& entry:std::filesystem::directory_iterator(dataset)){
        const auto name=entry.path().filename().string();
        if(name.size()>=suffix.size()&&name.compare(name.size()-suffix.size(),suffix.size(),suffix)==0)
            out.push_back(entry.path());
    }
    std::sort(out.begin(),out.end());return out;
}
inline std::string active_calibration_diagnostic(const std::filesystem::path& dataset){
    const auto candidates=active_device_candidates(dataset);
    std::ostringstream message;
    message<<"dataset="<<dataset.string()
               <<" pattern=*_device_config.json candidates="<<candidates.size()<<" [";
    for(const auto& path:candidates)message<<path.string()<<' ';
    message<<"] calibration="<<(dataset/"sensorRigCalibration.json").string()
               <<" exists="<<std::filesystem::exists(dataset/"sensorRigCalibration.json");
    return message.str();
}
inline std::filesystem::path unique_active_device(const std::filesystem::path& dataset){
    const auto candidates=active_device_candidates(dataset);
    if(candidates.size()!=1)
        throw std::runtime_error("Active device configuration missing or ambiguous: "+active_calibration_diagnostic(dataset));
    return candidates.front();
}
// Direct capture does not create a dataset. Accept an explicit snapshot or
// documentsRoot, otherwise use the newest complete dataset directly below root.
inline std::filesystem::path direct_calibration_dataset(const std::filesystem::path& root){
    auto complete=[](const std::filesystem::path& p){
        return std::filesystem::is_directory(p)&&
            std::filesystem::is_regular_file(p/"sensorRigCalibration.json")&&
            active_device_candidates(p).size()==1;
    };
    if(complete(root))return root;
    std::optional<std::filesystem::path> newest;
    if(std::filesystem::is_directory(root))for(const auto& entry:std::filesystem::directory_iterator(root)){
        if(entry.path().extension()!=".dataset"||!complete(entry.path()))continue;
        if(!newest||std::filesystem::last_write_time(entry.path())>std::filesystem::last_write_time(*newest)||
            (std::filesystem::last_write_time(entry.path())==std::filesystem::last_write_time(*newest)&&entry.path()>*newest))newest=entry.path();
    }
    if(newest)return *newest;
    throw std::runtime_error("No complete XRService calibration snapshot under "+root.string()+"; set calibration_dataset to a dataset or documentsRoot");
}
// station_optics.py start(): <=10s presence wait, 100ms poll, then 200ms
// settle. Ambiguity is checked after settling; never select newest config.
template<class Now,class Sleep,class Guard>
void await_active_calibration(const std::filesystem::path& dataset,Now now,Sleep sleep,Guard guard){
    const auto until=now()+10000000000LL;
    while(now()<until){
        guard();
        if(std::filesystem::exists(dataset/"sensorRigCalibration.json")&&!active_device_candidates(dataset).empty())break;
        sleep(100000000LL);
    }
    guard();sleep(200000000LL);guard();
    (void)unique_active_device(dataset);
    if(!std::filesystem::exists(dataset/"sensorRigCalibration.json"))
        throw std::runtime_error("Active calibration missing after wait: "+active_calibration_diagnostic(dataset));
}
}
