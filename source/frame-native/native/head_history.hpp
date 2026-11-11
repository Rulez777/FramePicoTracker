#include "guard_evidence.hpp"
#pragma once
#include "native_identity.hpp"
#include "phase5/json.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>

namespace pico {
// Same sample selection as ControllerGuard.head_pose: healthy HMD only,
// inclusive brackets, <=30 ms bracket width, and NO extrapolation.
class HeadHistory {
    struct Sample { int64_t stamp; Rigid pose; std::optional<Rigid> origin; };
    std::vector<Sample> samples_;
    int64_t last_stamp_=-1;
    size_t capacity_=0;
public:
    explicit HeadHistory(size_t capacity=0):capacity_(capacity){}
    void observe(const phase5::Value& sample) {
        auto stamp=phase5::number(sample,"t_ns");
        if(stamp<=last_stamp_||stamp<=0||stamp>=INT64_MAX/8)
            throw std::runtime_error("Nonforward/invalid HMD history time");
        last_stamp_=stamp;
        const auto& devices=phase5::field(sample,"devices");
        if(devices.type!='[')throw std::runtime_error("HMD devices array required");
        for(const auto& d:devices.array) {
            if(phase5::number(d,"kind")!=1||!phase5::flag(d,"connected")||
               !phase5::flag(d,"valid")||phase5::number(d,"result")!=200)continue;
            const auto& m=phase5::field(d,"matrix");
            if(m.type!='['||m.array.size()!=12)throw std::runtime_error("HMD matrix must be row-major 3x4");
            auto pose=identity_rigid();
            for(size_t i=0;i<12;++i){
                if(m.array[i].type!='i')throw std::runtime_error("HMD matrix number required");
                pose[i]=std::stod(m.array[i].text);
            }
            if(!identity_rigid_valid(pose))throw std::runtime_error("Invalid HMD rigid transform");
            std::optional<Rigid> origin;
            auto raw=sample.object.find("raw_to_standing");
            if(raw!=sample.object.end()){
                const auto& a=raw->second;
                if(a.type!='['||a.array.size()!=12)throw std::runtime_error("Measured origin must be 3x4");
                auto r=identity_rigid();
                for(size_t i=0;i<12;++i){
                    if(a.array[i].type!='i')throw std::runtime_error("Measured origin number required");
                    r[i]=std::stod(a.array[i].text);
                    if(!std::isfinite(r[i]))throw std::runtime_error("Nonfinite measured origin");
                }
                for(int i=0;i<3;++i)for(int j=0;j<3;++j){
                    double dot=0;for(int k=0;k<3;++k)dot+=r[4*k+i]*r[4*k+j];
                    if(std::abs(dot-(i==j?1.:0.))>1e-5)throw std::runtime_error("Nonrigid measured origin");
                }
                const double det=r[0]*(r[5]*r[10]-r[6]*r[9])-r[1]*(r[4]*r[10]-r[6]*r[8])+r[2]*(r[4]*r[9]-r[5]*r[8]);
                if(std::abs(det-1)>1e-5)throw std::runtime_error("Reflected measured origin");
                origin=r;
            }
            samples_.push_back({stamp,pose,origin});
            if(capacity_&&samples_.size()>capacity_)samples_.erase(samples_.begin());
            break; // Python takes first healthy HMD
        }
    }
    Rigid raw_from_standing(int64_t stamp) const {
    pico::guard_evidence::Scope guard_evidence_scope(8,"origin");
        auto after=std::lower_bound(samples_.begin(),samples_.end(),stamp,
            [](const Sample& s,int64_t t){return s.stamp<t;});
        // Owner rule 2026-10-29: same fallback as head_pose - hold the latest origin (<100 ms) instead of dropping.
        if(after==samples_.end()){
            if(samples_.empty()||stamp-samples_.back().stamp>100000000)
                throw std::runtime_error("No recent measured driver origin");
            after=samples_.end()-1;
        }else if(after==samples_.begin()&&after->stamp!=stamp&&after->stamp-stamp>100000000)
            throw std::runtime_error("No recent measured driver origin");
        const auto& before=(after->stamp<=stamp||after==samples_.begin())?*after:*(after-1);
        if(!before.origin||!after->origin)
            throw std::runtime_error("Missing measured driver origin");
        for(size_t i=0;i<16;++i)if(std::abs((*before.origin)[i]-(*after->origin)[i])>1e-5)
            throw std::runtime_error("OpenVR origin changed across exposure");
        const auto& r=*before.origin;auto inverse=identity_rigid();
        for(int i=0;i<3;++i){
            inverse[4*i+3]=0;
            for(int j=0;j<3;++j){inverse[4*i+j]=r[4*j+i];inverse[4*i+3]-=r[4*j+i]*r[4*j+3];}
        }
        return inverse;
    }
    Rigid head_pose(int64_t stamp) const {
    pico::guard_evidence::Scope guard_evidence_scope(8,"head-bracket");
        auto after=std::lower_bound(samples_.begin(),samples_.end(),stamp,
            [](const Sample& s,int64_t t){return s.stamp<t;});
        // Owner rule 2026-10-29: optics now runs ~10 ms after exposure, often before the next HMD sample exists.
        // Without a later sample, hold the latest HMD pose if it is <100 ms old; with a gap >30 ms use the nearer
        // sample. Only an empty or far history still has no pose.
        if(after==samples_.end()){
            if(samples_.empty()||stamp-samples_.back().stamp>100000000)
                throw std::runtime_error("No recent valid HMD pose for exposure time");
            return samples_.back().pose;
        }
        if(after==samples_.begin()&&after->stamp!=stamp){
            if(after->stamp-stamp>100000000)throw std::runtime_error("No recent valid HMD pose for exposure time");
            return after->pose;
        }
        const auto& before=after->stamp==stamp?*after:*(after-1);
        if(after->stamp-before.stamp>30000000)
            return (stamp-before.stamp<=after->stamp-stamp)?before.pose:after->pose;
        double f=after->stamp==before.stamp?0:double(stamp-before.stamp)/double(after->stamp-before.stamp);
        auto out=identity_rigid();
        double r[3][3];
        for(int i=0;i<3;++i){
            out[4*i+3]=(1-f)*before.pose[4*i+3]+f*after->pose[4*i+3];
            for(int j=0;j<3;++j)r[i][j]=(1-f)*before.pose[4*i+j]+f*after->pose[4*i+j];
        }
        // Newton polar decomposition of the same linear rotation blend as
        // Python's U diag(1,1,det(U Vt)) Vt. Rigid endpoints give det>=0.
        // Singular 180-degree midpoint has no unique polar rotation: reject.
        bool converged=false;
        for(int iter=0;iter<64;++iter){
            double co[3][3];
            for(int i=0;i<3;++i)for(int j=0;j<3;++j)
                co[i][j]=r[(i+1)%3][(j+1)%3]*r[(i+2)%3][(j+2)%3]-r[(i+1)%3][(j+2)%3]*r[(i+2)%3][(j+1)%3];
            double det=r[0][0]*co[0][0]+r[0][1]*co[0][1]+r[0][2]*co[0][2];
            if(!std::isfinite(det)||det<1e-20)throw std::runtime_error("Ambiguous/nonrigid HMD rotation interpolation");
            double change=0;
            for(int i=0;i<3;++i)for(int j=0;j<3;++j){double next=.5*(r[i][j]+co[i][j]/det);change=std::max(change,std::abs(next-r[i][j]));r[i][j]=next;}
            if(change<1e-13){converged=true;break;}
        }
        if(!converged)throw std::runtime_error("HMD polar projection did not converge");
        for(int i=0;i<3;++i)for(int j=0;j<3;++j)out[4*i+j]=r[i][j];
        if(!identity_rigid_valid(out))throw std::runtime_error("Nonrigid interpolated HMD pose");
    return out;
    }
};
}

#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_head_history, "head-history");
