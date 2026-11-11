#pragma once
#include "native_identity.hpp"
#include "live_camera.hpp"
#include <ostream>
#include <iomanip>
#include <cmath>
namespace pico {
// Production replay supplies original MONOTONIC exposure conversions. The null
// argument preserves explicit fixed-HEAD fixtures/legacy interchange only.
inline void write_replay_pose(std::ostream& out,const Rigid& pose,
 const std::array<camera::GrayFrame,2>& pair,uint64_t file,uint64_t stereo,uint64_t file_pair,
 const std::array<int64_t,2>* world_stamps=nullptr){
 auto stamp=[](const camera::GrayFrame& f){if(!f.metadata.timestamp||!std::isfinite(*f.metadata.timestamp)||*f.metadata.timestamp<0||*f.metadata.timestamp>=double(INT64_MAX)/1e9)throw std::runtime_error("Invalid export timestamp");
    return int64_t(std::llround(*f.metadata.timestamp*1e9));};
 auto left=stamp(pair[0]),right=stamp(pair[1]);
 if(world_stamps){left=(*world_stamps)[0];right=(*world_stamps)[1];if(left<=0||right<=0)throw std::runtime_error("Invalid world export time");}
 out<<std::setprecision(17)<<"{\"source_time_ns\":"<<std::min(left,right)
    <<",\"time_domain\":\""<<(world_stamps?"CLOCK_MONOTONIC":"recorded_camera_RAW")<<"\",\"world_reference\":\""<<(world_stamps?"measured_head_world":"fixed_replay_HEAD")<<"\",\"file_id\":"<<file<<",\"stereo_id\":"<<stereo<<",\"file_pair_id\":"<<file_pair<<",\"frame_ids\":[";
 for(int i=0;i<2;++i){if(i)out<<',';
 const auto& a=pair[i].metadata;
   out<<"{\"source_time_ns\":"<<(i?right:left)<<",\"camera_id\":";
   if(a.camera_id)out<<*a.camera_id;else out<<"null";
   out<<",\"stream\":["<<a.key.kind<<','<<unsigned(a.key.selector)<<','<<a.key.ordinal<<"]}";}
 out<<"],\"position\":["<<pose[3]<<','<<pose[7]<<','<<pose[11]<<"],\"rotation\":[";
 for(int i=0;i<3;++i){if(i)out<<',';
 out<<'['<<pose[4*i]<<','<<pose[4*i+1]<<','<<pose[4*i+2]<<']';}
 out<<"]}\n";
 if(!out)throw std::runtime_error("Pose output write failed");
}
}
