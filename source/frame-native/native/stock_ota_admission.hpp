#pragma once
#include "device_config.hpp"
#include "tracker_ota.hpp"
#include <ctime>
namespace pico {
inline bool ota_readiness_matches(const std::string &answer,const std::string &challenge,double age) {
    return !challenge.empty() && answer=="READY "+challenge && std::isfinite(age) && age>=0 && age<=120;
}
inline std::string ota_hex(const Bytes &bytes) {
    std::string s;for(auto b:bytes) {s+="0123456789abcdef"[b>>4];s+="0123456789abcdef"[b&15];}return s;
}
// Reader only. Production never creates or rewrites owner approval files.
class StockOtaApproval {
    Bytes chip_;
    std::string sha_;
    std::string serial_;
  public:
    StockOtaApproval(const std::string &path,const Bytes &chip,const std::string &sha,
                     const std::vector<std::string> &argv, std::string serial=pico::devices::serial(),
                     int64_t unix_now=std::time(nullptr)):chip_(chip),sha_(sha),serial_(std::move(serial)) {
        std::ifstream in(path,std::ios::binary);
        if(!in) throw std::runtime_error("Owner stock OTA approval file required");
        std::string header, timestamp;
        std::getline(in,header);std::getline(in,timestamp);
        if(header!="fw189-ota-approval-v2" || timestamp.rfind("unix=",0)!=0)
            throw std::runtime_error("Fresh v2 owner approval required");
        size_t used=0;const auto issued=std::stoll(timestamp.substr(5),&used);
        if(used!=timestamp.size()-5 || issued<0 || issued>unix_now || unix_now-issued>120)
            throw std::runtime_error("Owner approval expired or future dated");
        if(serial_.size()!=17 || serial_.rfind("PC",0)!=0 ||
           serial_.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789")!=std::string::npos)
            throw std::runtime_error("Invalid approved serial");
        std::string expected="chip="+ota_hex(chip)+"\nserial="+serial_+"\nfirmware=189\nsha256="+sha+"\n";
        for(const auto &arg:argv) expected+="argv="+ota_hex(Bytes(arg.begin(),arg.end()))+"\n";
        std::string actual((std::istreambuf_iterator<char>(in)),{});
        if(actual!=expected) throw std::runtime_error("Owner approval chip/image/argv mismatch");
    }
    bool matches(const Bytes &chip,const std::string &sha) const {return chip==chip_ && sha==sha_;}
    const std::string &serial() const {return serial_;}
};
// Persistent negative evidence, shared by ordinary and stock OTA invocations.
// Known installed diagnostic target is denied even in a fresh history file.
class OtaNativeHistory {
    std::string path_;
    std::set<std::string> chips_;
  public:
    explicit OtaNativeHistory(std::string path):path_(std::move(path)) {
        std::ifstream in(path_);
        if(!in) throw std::runtime_error("Prepared native-reply history required");
        std::string line;
        while(std::getline(in,line)) {
            if(line.empty())continue;
            if(line.size()!=8 || line.find_first_not_of("0123456789abcdef")!=std::string::npos)
                throw std::runtime_error("Invalid native-reply history");
            chips_.insert(line);
        }
        chips_.insert(pico::devices::chip_le_hex()); // the primary tracker runs our firmware (devices.conf first line)
    }
    bool known(const Bytes &chip) const {return chips_.count(ota_hex(chip))!=0;}
    void remember(const Bytes &chip) {
        if(known(chip))return;
        std::ofstream out(path_,std::ios::app);
        out<<ota_hex(chip)<<'\n';out.flush();
        if(!out)throw std::runtime_error("Cannot persist native reply; refusing OTA");
        chips_.insert(ota_hex(chip));
    }
};
}
