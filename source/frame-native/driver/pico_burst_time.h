#pragma once
#include <algorithm>
#include <cstdint>
#include <deque>
#include <cmath>
#include <cstddef>
#include <utility>
namespace pico_fusion {
// PTI1 has a Roy-derived receive stamp, not a hardware sample timestamp.
// Only a negative host-minus-Roy offset is identifiable: positive residual
// contains unknown transport delay and must not be silently removed.
class BurstTime {
public:
    double Stamp(uint64_t host, uint64_t roy, size_t later, double period) {
        double residual=double(host)-double(roy);
        if (roy!=lastRoy_) {
            observations_.push_back({double(host),residual}); lastRoy_=roy;
            while(!observations_.empty() && double(host)-observations_.front().first>2e9)
                observations_.pop_front();
        }
        double offset=0;
        for(auto &v:observations_) offset=std::fmin(offset,v.second);
        double newest=double(roy)+offset;
        // Log/drop policy: a broken stamp falls back to arrival, never ends a run.
        if(double(host)-newest>=500e6) newest=double(host);
        return std::fmax(1.,newest-double(later)*period);
    }
private:
    uint64_t lastRoy_=0;
    std::deque<std::pair<double,double>> observations_;
};
}
