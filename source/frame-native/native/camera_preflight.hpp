#include "guard_evidence.hpp"
#pragma once
#include "device_config.hpp"
#include "live_camera.hpp"
#include "optical_timing.hpp"
#include "live_startup.hpp"
#include <algorithm>
#include <cmath>

namespace pico {
struct CameraReadClocks { int64_t raw,mono,boottime; };
struct CameraAcquisitionProof {
    CameraClockEvidence clock;
    std::string dataset;
    std::string calibration;
    size_t samples=0;
    std::array<size_t,3> causal{};
    std::array<std::array<int64_t,2>,3> difference_bounds{};
    std::array<size_t,2> exposures{};
    bool ambiguous=false;
};
// Current recording only: one second backlog settling, three seconds of actual
// read clocks. Historical replay evidence must never be passed as acquisition.
class CameraAcquisition {
    int64_t start_,last_;
    CameraAcquisitionProof proof_;
    std::array<std::optional<double>,2> previous_{};
    bool failed_=false,ready_=false;
    void require(bool b,const char* message){if(!b){pico::guard_evidence::emit(21,"acquisition",false);failed_=true;throw std::runtime_error(message);}}
    void tick(int64_t now){
        require(!failed_,"Camera preflight latched");
        require(now>=last_&&now<INT64_MAX/8,"Camera preflight clock regressed/invalid");last_=now;
        require(ready_||now-start_<camera_readiness_ns,"Camera preflight exceeded 12s");
    }
public:
    CameraAcquisition(int64_t now,std::string boot,std::string dataset,std::string calibration):start_(now),last_(now){
        require(now>0&&now<INT64_MAX/8&&!boot.empty()&&!dataset.empty()&&!calibration.empty(),"Camera acquisition provenance missing");
        proof_.clock={1,std::move(boot),"CLOCK_MONOTONIC_RAW","Current recording RAW acquisition; bridge per conversion",true,0,500000};
        proof_.dataset=std::move(dataset);
        proof_.calibration=std::move(calibration);
    }
    void observe(const camera::AccessUnit& row,int64_t now,CameraReadClocks read){
        tick(now);if(ready_)return;
        if(row.key.kind!=38)return;
        require(row.timestamp&&std::isfinite(*row.timestamp)&&*row.timestamp>0&&*row.timestamp<double(INT64_MAX/8)/1e9,
                "Camera preflight timestamp missing/invalid");
        const auto stamp=nearest_even(*row.timestamp*1e9);
        if(now-start_>=1000000000LL&&now-start_<4000000000LL){
            std::array<int64_t,3> clocks{read.raw,read.mono,read.boottime};
            for(size_t i=0;i<3;++i){
                require(clocks[i]>0&&clocks[i]<INT64_MAX/8,"Camera read clock invalid");
                const auto age=clocks[i]-stamp;
                if(age>=0&&age<=250000000)++proof_.causal[i];
                auto& bounds=proof_.difference_bounds[i];
                if(!proof_.samples)bounds={age,age};
                else {bounds[0]=(std::min)(bounds[0],age);bounds[1]=(std::max)(bounds[1],age);}
            }
            require(proof_.samples<4096,"Camera acquisition sample cap exceeded");++proof_.samples;
        }
        if(!row.sensor)return;
        size_t sensor=2;
        if(*row.sensor==pico::devices::upper(0))sensor=0;
        if(*row.sensor==pico::devices::upper(1))sensor=1;
        if(sensor==2)return;
        require(row.exposure&&std::isfinite(*row.exposure)&&*row.exposure>=.000750&&*row.exposure<=.000850,
                "Wrong actual upper-camera IR exposure");
        if(previous_[sensor]){
            const auto delta=*row.timestamp-*previous_[sensor];
            require(delta>.032&&delta<.035,"Camera cadence omission/change");
        }
        require(proof_.exposures[sensor]<512,"Camera preflight exposure cap exceeded");
        previous_[sensor]=*row.timestamp;++proof_.exposures[sensor];
    }
    bool poll(int64_t now,const std::string& current_boot){
        tick(now);require(current_boot==proof_.clock.boot,"Camera acquisition boot changed");
        if(!ready_&&now-start_>=4000000000LL){
            require(proof_.samples>=40,"Too few live camera clock samples");
            require(proof_.causal[0]*10>=proof_.samples*9,"Camera clock RAW causal fraction below 0.9");
            if(proof_.exposures[0]>=6&&proof_.exposures[1]>=6){
                // Python records ambiguity against its retained XRService RAW
                // domain basis; it does not invent unique-domain evidence.
                proof_.ambiguous=proof_.causal[1]*10>=proof_.samples*9||proof_.causal[2]*10>=proof_.samples*9;
                ready_=true;
            }
        }
        return ready_;
    }
    const CameraAcquisitionProof& proof()const{
    pico::guard_evidence::Scope guard_evidence_scope(21,"acquisition");
        if(!ready_||failed_)throw std::runtime_error("Camera preflight not admitted");
    return proof_;
    }
};
}

#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_camera_preflight, "camera-preflight");
