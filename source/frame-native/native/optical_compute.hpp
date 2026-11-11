#pragma once
#include "native_identity.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>

namespace pico {
// One deadline shared by upper stereo/mono and lower fallback on the optics thread.
struct OpticalComputeBudget {
    using Clock=std::chrono::steady_clock;
    inline static thread_local OpticalComputeBudget* current=nullptr;
    OpticalComputeBudget* previous=current;
    unsigned attempts=0;
    Clock::time_point end=Clock::now()+std::chrono::milliseconds(25);
    OpticalComputeBudget(){current=this;}
    ~OpticalComputeBudget(){current=previous;}
    OpticalComputeBudget(const OpticalComputeBudget&)=delete;
    OpticalComputeBudget& operator=(const OpticalComputeBudget&)=delete;
    static bool start_solve(){
        if(exhausted() || (current && current->attempts>=2)){log_optical_drop("Pair solver budget exhausted");return false;}
        if(current){ ++current->attempts; }
        return true;
    }
    static bool exhausted(){return current && Clock::now()>=current->end;}
};

// detector.c uint8 ABI: intensity-weighted x,y, sqrt(area/pi), peak, area.
// Use area (slot 4), never the radius (slot 2), for size similarity.
inline std::vector<double> compact_led_cluster(const std::vector<double>& blobs) {
    struct Cluster { std::vector<size_t> ids; double score=0; } best;
    const size_t n=blobs.size()/5;
    // The solver tolerates a few extra blobs (2026-10-30 recording: selection at any count lost every fit).
    if(n<=48) { std::vector<double> all; for(size_t j=0;j<n;++j){all.push_back(blobs[5*j]);all.push_back(blobs[5*j+1]);} return all; }
    for(size_t seed=0;seed<n;++seed) {
        const double area=blobs[5*seed+4];
        if(!std::isfinite(area)||area<=0||area>400)continue;
        std::vector<std::pair<double,size_t>> neighbors;
        for(size_t j=0;j<n;++j) {
            const double a=blobs[5*j+4];
            const double dx=blobs[5*j]-blobs[5*seed],dy=blobs[5*j+1]-blobs[5*seed+1];
            const double d=dx*dx+dy*dy;
            if(std::isfinite(d)&&std::isfinite(a)&&a>0&&a<=400&&a>=area/3&&a<=area*3&&d<=200*200)
                neighbors.emplace_back(d,j);
        }
        std::sort(neighbors.begin(),neighbors.end());
        if(neighbors.size()>8)neighbors.resize(8);
        if(neighbors.size()<4)continue;
        double spread=0,variation=0;
        for(const auto& item:neighbors){spread+=item.first;variation+=std::abs(std::log(blobs[5*item.second+4]/area));}
        // Prefer a full eight-LED group, then compactness and uniform small area.
        const double score=10000*neighbors.size()-spread/neighbors.size()-100*variation-area;
        if(best.ids.empty()||score>best.score){best.score=score;best.ids.clear();for(auto item:neighbors)best.ids.push_back(item.second);}
    }
    std::sort(best.ids.begin(),best.ids.end()); // preserve detector order within the selected group
    std::vector<double> pixels;
    for(auto j:best.ids){pixels.push_back(blobs[5*j]);pixels.push_back(blobs[5*j+1]);}
    if(best.ids.size()!=n)log_optical_drop("Candidate cluster cap dropped blobs");
    return pixels;
}
}
