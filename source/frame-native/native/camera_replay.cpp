#include "native_test_runtime.hpp"
#include "recording_replay.hpp"
#include <chrono>
#include <iostream>
int main(int argc,char** argv){try{
    if(argc!=3)throw std::runtime_error("Usage: camera_replay <recording-directory> <absolute-ffmpeg>");
    auto now=std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    pico::camera::replay_recording(argv[1],argv[2],now+900000000000LL);
    return 0;
}catch(const std::exception& e){std::cerr<<"camera_replay: "<<e.what()<<'\n';return 1;}}
