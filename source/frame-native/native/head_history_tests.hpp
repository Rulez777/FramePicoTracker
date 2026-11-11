#pragma once
#include "recording_head_history.hpp"
#include "replay_pose_text.hpp"
#include <sstream>
#include <iostream>
#include <chrono>
namespace head_history_tests {
using namespace pico;
inline Rigid rotation(double angle,double x=0){auto m=identity_rigid();m[0]=m[5]=std::cos(angle);m[1]=-std::sin(angle);m[4]=std::sin(angle);m[3]=x;return m;}
inline std::string sample(int64_t stamp,const Rigid& pose,bool healthy=true){
    std::ostringstream s;s.precision(17);s<<"{\"t_ns\":"<<stamp<<",\"devices\":[{\"kind\":1,\"connected\":true,\"valid\":"<<(healthy?"true":"false")<<",\"result\":200,\"matrix\":[";
    for(int i=0;i<12;++i){if(i)s<<',';
    s<<pose[size_t(i)];}s<<"]}]}";return s.str();
}
inline void run(){
    // Exercise a native-only test package: no repository-level ../samples.
    // Restore cwd even if a check throws; retain the sandbox as failure evidence.
    struct RestoreDirectory {
        std::filesystem::path saved=std::filesystem::current_path();
        ~RestoreDirectory(){std::error_code ec;std::filesystem::current_path(saved,ec);}
    } restore;
    const auto sandbox=restore.saved/"build"/("head-history-package-"+
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directories(sandbox/"fixtures");
    for(const auto* name:{"r13b-driver-proof-and-camera-clock.sample.txt","r13b-frame-timing.sample.jsonl"}){
        const auto from=restore.saved/"fixtures"/name;
        if(!std::filesystem::is_regular_file(from))
            throw std::runtime_error("Missing head-history test fixture: "+from.string());
        std::filesystem::copy_file(from,sandbox/"fixtures"/name);
    }
    std::filesystem::current_path(sandbox);
    int checks=0;
    auto check=[&](bool v,int line=__builtin_LINE()){++checks;if(!v)throw std::runtime_error("head history check "+std::to_string(checks)+" line "+std::to_string(line));};
    auto rejects=[&](auto action){bool bad=false;try{action();}catch(const std::exception&){bad=true;}check(bad);};
    auto observe=[](HeadHistory& h,int64_t t,Rigid pose,bool healthy=true){auto s=sample(t,pose,healthy);h.observe(phase5::Json(s).parse());};
    constexpr int64_t start=10000000000LL;
    HeadHistory h;observe(h,start,rotation(0,1));observe(h,start+15000000,rotation(1,99),false);observe(h,start+30000000,rotation(2.09439510239319549,3));
    check(h.head_pose(start)==rotation(0,1));check(std::abs(h.head_pose(start+30000000)[0]-std::cos(2.09439510239319549))<1e-12);
    auto p=h.head_pose(start+7500000);
    double expected=std::atan2(.25*std::sin(2.09439510239319549),.75+.25*std::cos(2.09439510239319549));
    check(std::abs(p[0]-std::cos(expected))<1e-12&&std::abs(p[4]-std::sin(expected))<1e-12&&std::abs(p[3]-1.5)<1e-12);
    check(std::abs(expected-2.09439510239319549*.25)>.1); // catches accidental SLERP
    check(identity_rigid_valid(p));
    (void)h.head_pose(start-1);(void)h.head_pose(start+30000001);  // 2026-10-29: nearest/latest pose held (<100 ms), not a drop
    rejects([&]{h.head_pose(start-100000001);});
    rejects([&]{observe(h,start+30000000,identity_rigid());});
    HeadHistory gap;observe(gap,start,identity_rigid());observe(gap,start+30000001,identity_rigid());check(gap.head_pose(start+1)==identity_rigid());  // gap >30 ms: nearer sample
    HeadHistory invalid;auto nan=identity_rigid();nan[0]=2;rejects([&]{observe(invalid,start,nan);});
    HeadHistory singular;observe(singular,start,identity_rigid());observe(singular,start+20000000,rotation(3.14159265358979323846));rejects([&]{singular.head_pose(start+10000000);});
    // Noncommuting left basis: polar projection commutes with left rotation.
    auto basis=identity_rigid();basis[5]=basis[10]=0;basis[6]=-1;basis[9]=1;
    auto left=[&](Rigid m){auto n=identity_rigid();for(int i=0;i<3;++i)for(int j=0;j<4;++j){n[4*i+j]=0;for(int k=0;k<3;++k)n[4*i+j]+=basis[4*i+k]*m[4*k+j];}
    return n;};
    HeadHistory arbitrary;observe(arbitrary,start,left(rotation(0,1)));observe(arbitrary,start+30000000,left(rotation(2.09439510239319549,3)));
    auto transformed=arbitrary.head_pose(start+7500000),expected_pose=left(p);for(int i=0;i<16;++i)check(std::abs(transformed[size_t(i)]-expected_pose[size_t(i)])<1e-12);
    const auto dir=std::filesystem::current_path()/"build/head_history.synthetic.dataset";
    std::filesystem::create_directories(dir);
    {std::ofstream out(dir/"tracking_watch.jsonl");out<<sample(start,rotation(0,1))<<'\n'<<sample(start+30000000,rotation(.2,3))<<'\n';}
    auto write_clock=[&](int64_t raw,int64_t stamp,int64_t now,int64_t uncertainty=1000){std::ofstream out(dir/"camera_exposure_clock.jsonl");out<<"{\"camera_raw_ns\":"<<raw<<",\"source_time_ns\":"<<stamp<<",\"conversion_now_ns\":"<<now<<",\"uncertainty_ns\":"<<uncertainty<<",\"verified\":true,\"time_domain\":\"CLOCK_MONOTONIC\"}\n";};
    std::array<camera::GrayFrame,2> pair;for(auto& f:pair){f.metadata.timestamp=9.;f.metadata.exposure=.0008;}pair[1].metadata.timestamp=9.000001;
    write_clock(9000000000LL,start+15000000,start+20000000);
    RecordingHeadHistory recording(dir);auto result=recording.lookup(pair);
    check(result.stamps[0]==start+15000000&&result.stamps[1]==start+15001000);
    check(std::abs(result.world_from_head[0][3]-2)<1e-12&&result.world_from_head[1][3]>2);
    std::ostringstream exported;write_replay_pose(exported,rotation(.4,2),pair,1,1,1,&result.stamps);
    const auto row=phase5::Json(exported.str()).parse();
    check(phase5::field(row,"world_reference").text=="measured_head_world"&&phase5::field(row,"time_domain").text=="CLOCK_MONOTONIC");
    check(phase5::number(row,"source_time_ns")==start+15000000);
    pair[0].metadata.timestamp=8.;rejects([&]{recording.lookup(pair);});pair[0].metadata.timestamp=9.;
    pair[1].metadata.timestamp=9.000003;rejects([&]{recording.lookup(pair);});pair[1].metadata.timestamp=9.000001;
    write_clock(9000000000LL,start,start+250000001);rejects([&]{RecordingHeadHistory bad(dir);});
    write_clock(9000000000LL,start,start-1);rejects([&]{RecordingHeadHistory bad(dir);});
    write_clock(9000000000LL,start,start+1,500001);rejects([&]{RecordingHeadHistory bad(dir);});
    write_clock(9000000000LL,start+30000000,start+30001000);RecordingHeadHistory edge(dir);try{(void)edge.lookup(pair);}catch(const std::exception&){}  // edge pose now held (<100 ms), not required to reject
    rejects([&]{RecordingHeadHistory missing(dir/"absent");});
    // Actual source schema: raw_to_standing must not be applied a second time.
    const auto source_dir=dir/("r13b-format.synthetic-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directories(source_dir);
    auto driver_row=sample(start,rotation(0,1));
    driver_row.insert(1,"\"raw_to_standing\":[1,0,0,0,0,1,0,1.6,0,0,1,0],");
    {std::ofstream out(source_dir/"driver-proof.jsonl");out<<driver_row<<'\n'<<sample(start+30000000,rotation(.2,3))<<'\n';}
    const auto clock_path=source_dir/"camera-clock.json";
    auto clock_json=[](const std::string& domain,int64_t offset=0){return "{\"version\":1,\"boot_id\":\"original-run-boot\",\"verified\":true,\"source_domain\":\""+domain+"\",\"offset_ns\":"+std::to_string(offset)+",\"uncertainty_ns\":500000,\"method\":\"synthetic test evidence\"}";};
    auto save_evidence=[&](const std::string& bytes){std::ofstream out(clock_path);out<<bytes;};
    save_evidence(clock_json("CLOCK_MONOTONIC_RAW"));
    auto evidence=read_recording_camera_clock(clock_path);
    check(evidence.domain=="CLOCK_MONOTONIC_RAW"&&evidence.offset==0&&evidence.boot=="original-run-boot");
    // Startup RAW evidence alone CANNOT reconstruct historical bridges.
    rejects([&]{RecordingHeadHistory bad(source_dir,source_dir/"driver-proof.jsonl",clock_path);});
    rejects([&]{RecordingHeadHistory bad(source_dir,source_dir/"driver-proof.jsonl");});
    {std::ofstream out(source_dir/"camera_exposure_clock.jsonl");out<<"{\"camera_raw_ns\":9000000000,\"source_time_ns\":10015000000,\"conversion_now_ns\":10020000000,\"uncertainty_ns\":1000,\"verified\":true,\"time_domain\":\"CLOCK_MONOTONIC\"}\n";}
    RecordingHeadHistory source(source_dir,source_dir/"driver-proof.jsonl",clock_path);
    auto source_result=source.lookup(pair);
    check(source_result.stamps[0]==start+15000000&&source_result.world_from_head[0][7]==0);
    check(std::abs(source_result.world_from_head[0][3]-2)<1e-12);
    save_evidence(clock_json("CLOCK_MONOTONIC",1015000000));
    RecordingHeadHistory mono(source_dir,source_dir/"driver-proof.jsonl",clock_path);
    check(mono.lookup(pair).stamps==source_result.stamps);
    save_evidence(clock_json("CLOCK_MONOTONIC",1015000001));
    rejects([&]{RecordingHeadHistory bad(source_dir,source_dir/"driver-proof.jsonl",clock_path);});
    save_evidence(clock_json("CLOCK_BOOTTIME"));rejects([&]{read_recording_camera_clock(clock_path);});
    save_evidence("{}");rejects([&]{read_recording_camera_clock(clock_path);});
    save_evidence(clock_json("CLOCK_MONOTONIC_RAW")+"trailing");rejects([&]{read_recording_camera_clock(clock_path);});
    // Supplied format sample is intentionally truncated after three complete
    // watcher rows. Parse those rows and the separate pretty-printed clock JSON.
    std::ifstream format_sample("fixtures/r13b-driver-proof-and-camera-clock.sample.txt");
    if(!format_sample)throw std::runtime_error("Cannot open head-history fixture: fixtures/r13b-driver-proof-and-camera-clock.sample.txt");
    check(bool(format_sample)); // historical check #49: fixture availability, not FP
    std::string line,clock_bytes;HeadHistory actual_format;int rows=0;bool clock_section=false;
    while(std::getline(format_sample,line)){
        if(line=="===="||line=="====\r"){clock_section=true;continue;}
        if(clock_section){clock_bytes+=line+'\n';continue;}
        if(rows<3){actual_format.observe(phase5::Json(line).parse());++rows;}
        else if(!line.empty()&&line!="\r")rejects([&]{phase5::Json(line).parse();});
    }
    check(rows==3);
    auto actual_pose=actual_format.head_pose(29340449519507LL);
    check(std::abs(actual_pose[7]-1.57258344)<1e-12); // not +1.60000002
    save_evidence(clock_bytes);auto actual_clock=read_recording_camera_clock(clock_path);
    check(actual_clock.domain=="CLOCK_MONOTONIC_RAW"&&actual_clock.uncertainty==500000&&actual_clock.offset==0);
    // Retained frame_timing maps must override legacy conversions, preserve
    // exact per-view integers, and reject missing/ambiguous pairs.
    const auto map_path=source_dir/"optical-live.jsonl";
    auto map_row=[](double left,double right){std::ostringstream s;s.precision(17);
        s<<"{\"frame_timing\":true,\"selected_source_raw\":["<<left<<','<<right
         <<"],\"exposure_stamps_ns\":[10010000001,10010001003],\"decoded_monotonic_ns\":[10020000000,10020000001]}\n";return s.str();};
    auto save_map=[&](const std::string& bytes){std::ofstream out(map_path);out<<bytes;};
    save_evidence(clock_json("CLOCK_MONOTONIC_RAW"));
    const std::string missing_rows=
        "{\"frame_timing\":true}\n"
        "{\"frame_timing\":true,\"selected_source_raw\":[8,8.000001],\"decoded_monotonic_ns\":[10020000000,10020000001]}\n"
        "{\"frame_timing\":true,\"exposure_stamps_ns\":[10010000001,10010001003]}\n";
    save_map("{\"other_log_row\":true}\n"+missing_rows+map_row(9.,9.000001)+missing_rows);
    RecordingHeadHistory mapped(source_dir/"no-legacy-clock",source_dir/"driver-proof.jsonl",clock_path,map_path);
    check(mapped.skipped_mapping_rows()==6&&mapped.mapped_rows()==1);
    pair[0].metadata.timestamp=8.;pair[1].metadata.timestamp=8.000001;
    rejects([&]{mapped.lookup(pair);}); // skipped rows never create mappings
    pair[0].metadata.timestamp=9.;pair[1].metadata.timestamp=9.000001;
    auto mapped_result=mapped.lookup(pair);
    check(mapped_result.stamps[0]==10010000001LL&&mapped_result.stamps[1]==10010001003LL);
    check(mapped_result.world_from_head[0][3]>1.66&&mapped_result.world_from_head[0][3]<1.67);
    std::ostringstream mapped_export;write_replay_pose(mapped_export,rotation(0),pair,1,1,1,&mapped_result.stamps);
    auto mapped_json=phase5::Json(mapped_export.str()).parse();
    check(phase5::number(mapped_json,"source_time_ns")==10010000001LL);
    check(phase5::number(phase5::field(mapped_json,"frame_ids").array[1],"source_time_ns")==10010001003LL);
    pair[0].metadata.timestamp=9.0000005;check(mapped.lookup(pair).stamps==mapped_result.stamps);
    pair[0].metadata.timestamp=9.0000011;rejects([&]{mapped.lookup(pair);});pair[0].metadata.timestamp=9.;
    pair[1].metadata.timestamp=9.0000021;rejects([&]{mapped.lookup(pair);});pair[1].metadata.timestamp=9.000001;
    save_map(map_row(9.,9.000001)+map_row(9.0000001,9.0000011));
    RecordingHeadHistory ambiguous(source_dir,source_dir/"driver-proof.jsonl",clock_path,map_path);
    rejects([&]{ambiguous.lookup(pair);});
    save_map("{\"frame_timing\":true}");rejects([&]{RecordingHeadHistory bad(source_dir,source_dir/"driver-proof.jsonl",clock_path,map_path);});
    save_map("{\"other_log_row\":true}");rejects([&]{RecordingHeadHistory bad(source_dir,source_dir/"driver-proof.jsonl",clock_path,map_path);});
    // Missing one field must not hide malformed values in another present field.
    for(const auto* fields:{
        "\"selected_source_raw\":null", "\"selected_source_raw\":[9]",
        "\"selected_source_raw\":[\"9\",9]", "\"selected_source_raw\":[1e999,9]",
        "\"exposure_stamps_ns\":false", "\"exposure_stamps_ns\":[1,2,3]",
        "\"exposure_stamps_ns\":[1.5,2]", "\"exposure_stamps_ns\":[1e999,2]",
        "\"decoded_monotonic_ns\":{}", "\"decoded_monotonic_ns\":[1,null]"}) {
        save_map(map_row(9.,9.000001)+"{\"frame_timing\":true,"+fields+"}\n");
        rejects([&]{RecordingHeadHistory bad(source_dir,source_dir/"driver-proof.jsonl",clock_path,map_path);});
    }
    auto no_decoded=map_row(9.,9.000001);
    no_decoded.erase(no_decoded.find(",\"decoded_monotonic_ns\""));no_decoded+="}\n";
    save_map(no_decoded);rejects([&]{RecordingHeadHistory bad(source_dir,source_dir/"driver-proof.jsonl",clock_path,map_path);});
    auto stale=map_row(9.,9.000001);auto pos=stale.find("10020000000");stale.replace(pos,11,"10300000000");
    save_map(stale);rejects([&]{RecordingHeadHistory bad(source_dir,source_dir/"driver-proof.jsonl",clock_path,map_path);});
    // Actual supplied frame timing sample, with explicitly synthetic HMD coverage.
    {std::ofstream out(source_dir/"driver-proof.jsonl");out<<sample(29365573582466LL,rotation(0,1))<<'\n'<<sample(29365593582466LL,rotation(.2,3))<<'\n';}
    RecordingHeadHistory sample_map(source_dir,source_dir/"driver-proof.jsonl",clock_path,"fixtures/r13b-frame-timing.sample.jsonl");
    rejects([&]{sample_map.lookup(pair);});
    for(auto& f:pair)f.metadata.timestamp=29366.606507817323;
    auto actual_stamps=sample_map.lookup(pair).stamps;
    check(actual_stamps[0]==29365573582466LL&&actual_stamps[1]==29365573582466LL);
    std::cout<<checks<<" synthetic head history checks passed\n";
}
}
