#include "device_config.hpp"
#include "guard_evidence.hpp"
#include "live_policy.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include <mutex>
#include <stdexcept>
#include <utility>
namespace pico {
namespace {
void require(bool b,const char* s){if(!b)throw std::runtime_error(s);}
std::mutex drops_mutex;
std::map<std::string,uint64_t> drops;
// Owner rule 2026-10-29: measured-data and observer checks log and drop, never end the run.
void log_policy(const char* reason,const std::string& detail={}){
    std::lock_guard<std::mutex> lock(drops_mutex);
    const auto count=++drops[reason];
    if(count==1||count%100==0){
        std::cerr<<"live_policy_drop reason=\""<<reason<<'"';
        if(!detail.empty())std::cerr<<" detail=\""<<detail<<'"';
        std::cerr<<" count="<<count<<'\n';
    }
}
}
uint64_t live_policy_drops(const std::string& reason){
    std::lock_guard<std::mutex> lock(drops_mutex);
    const auto it=drops.find(reason);return it==drops.end()?0:it->second;
}
TrackingObserver::TrackingObserver(ObserverHooks h):h_(std::move(h)){
    require(h_.read&&h_.child_alive&&h_.parent_alive&&h_.cancelled&&h_.now,"Incomplete observer supervision");
    started_=last_poll_=h_.now();
}
void TrackingObserver::poll(phase5::BenchTrackingGuard& guard){
    const auto now=h_.now();
    if(now<last_poll_||now<started_){log_policy("Observer clock regressed; skipping poll");return;}
    last_poll_=now;
    if(now-last_row_.value_or(started_)>=3000000000LL)log_policy("Observer startup/silence reached 3s; still waiting");
    if(!h_.parent_alive()||h_.cancelled()||!h_.child_alive())log_policy("Observer/parent exited or cancelled; draining remaining rows");
    if(eof_)return;
    // Hard per-call byte budget, including a child flooding complete rows.
    for(size_t budget=0;budget<262144;){
        size_t limit=4096;ObserverRead r;
        try{r=h_.read(limit);}
        catch(const std::exception& e){log_policy("Observer read failed; retrying next poll",e.what());return;}
        if(r.bytes.size()>limit)log_policy("Observer read exceeded requested bound; processing bytes");
        budget+=r.bytes.size();
        for(char c:r.bytes){
            if(c=='\n'){
                if(skip_line_){skip_line_=false;pending_.clear();continue;}
                if(pending_.empty()){log_policy("Empty observer row; dropped");continue;}
                try{guard.ingest(pending_);(void)guard.check(h_.now(),true,false);}
                catch(const std::exception& e){log_policy("Observer row rejected; dropped",e.what());}
                last_row_=h_.now();pending_.clear();
            }
            else if(skip_line_)continue;
            else if(pending_.size()>=65536){log_policy("Observer line exceeds 65536; dropping line");pending_.clear();skip_line_=true;}
            else pending_+=c;
        }
        if(r.eof){
            if(!pending_.empty()||skip_line_)log_policy("Observer partial final row; dropped");
            pending_.clear();skip_line_=false;eof_=true;
            log_policy("Observer EOF; no further rows");return;
        }
        if(r.bytes.empty())break;
        if(budget>=262144)log_policy("Observer output flood; continuing next poll");
    }
    if(!h_.child_alive()||!h_.parent_alive()||h_.cancelled())log_policy("Observer supervision changed during read");
}
void ImuAdmission::configure(const ImuCalibration& c){
    // Rejected reconfiguration revokes old calibration rather than retaining it.
    configured_=false;time_=false;
    const char* why=nullptr;
    if(!(c.axes_verified&&c.time_verified&&std::isfinite(c.gyro_scale)&&c.gyro_scale>0&&c.gyro_scale<.01
      &&std::isfinite(c.accel_scale)&&c.accel_scale>0&&c.accel_scale<.1&&c.offset>=-25000000&&c.offset<=25000000))why="Unmeasured IMU calibration";
    for(auto x:c.axes)if(!why&&!std::isfinite(x))why="Nonfinite IMU axes";
    for(int i=0;i<3&&!why;++i)for(int j=0;j<3&&!why;++j){double dot=0;for(int k=0;k<3;++k)dot+=c.axes[k*3+i]*c.axes[k*3+j];
        if(!(std::abs(dot-(i==j?1.:0.))<=1e-4))why="Nonorthogonal IMU axes";}
    const auto& a=c.axes;
    double det=a[0]*(a[4]*a[8]-a[5]*a[7])-a[1]*(a[3]*a[8]-a[5]*a[6])+a[2]*(a[3]*a[7]-a[4]*a[6]);
    if(!why&&!(det>=.999))why="Reflected IMU axes";
    double bias=0;for(auto x:c.bias){if(!why&&!std::isfinite(x))why="Nonfinite IMU bias";
    bias+=x*x;}
    if(!why&&!(std::sqrt(bias)*c.gyro_scale<.05))why="IMU gyro bias exceeds bound";
    pico::guard_evidence::emit(20,"axes-time",!why);
    if(why){log_policy(why,"IMU left unconfigured; samples are not admitted");return;}
    c_=c;configured_=true;time_=true;
}
void ImuAdmission::reconnect(){time_=false;invalid_=true;anchor_=state_=0;}
void ImuAdmission::set_time(bool measured){time_=measured&&configured_;}
void ImuAdmission::optical_anchor(int64_t stamp,int64_t now){
    if(!(stamp>0&&now>=stamp&&now-stamp<=250000000&&stamp>anchor_)){log_policy("Invalid IMU optical anchor; dropped");return;}
    anchor_=state_=stamp;invalid_=diverged_=false;
}
bool ImuAdmission::sample(int64_t stamp,int64_t now,const std::array<double,3>& gyro,const std::array<double,3>& accel,double sigma){
    if(!configured_||!time_||invalid_||diverged_||anchor_<=0){pico::guard_evidence::emit(20,"sample-time",false);return false;}
    if((c_.offset>0&&stamp>std::numeric_limits<int64_t>::max()-c_.offset)
       ||(c_.offset<0&&stamp<std::numeric_limits<int64_t>::min()-c_.offset))return false;
    stamp+=c_.offset;
    if(now<anchor_||now-anchor_>250000000||stamp<=anchor_||stamp>now||stamp<=state_||stamp-state_>25000000)return false;
    double g2=0,a2=0;
    for(int i=0;i<3;++i){double g=0,a=0;for(int j=0;j<3;++j){
        if(!std::isfinite(gyro[j])||!std::isfinite(accel[j]))return false;
        g+=c_.axes[i*3+j]*(gyro[j]-c_.bias[j])*c_.gyro_scale;a+=c_.axes[i*3+j]*accel[j]*c_.accel_scale;
    }g2+=g*g;a2+=a*a;}
    if(!std::isfinite(g2)||!std::isfinite(a2)||g2>35.*35.||a2>320.*320.)return false;
    if(!std::isfinite(sigma)||sigma<0||sigma>.03){diverged_=true;return false;}
    state_=stamp;pico::guard_evidence::emit(20,"sample-time",true);return true;
}
CameraPreflight::CameraPreflight(std::string boot,std::string dataset,std::string calibration,int64_t start)
 :boot_(std::move(boot)),dataset_(std::move(dataset)),calibration_(std::move(calibration)),start_(start){
    require(!boot_.empty()&&!dataset_.empty()&&!calibration_.empty()&&start_>0,"Camera provenance missing");
}
void CameraPreflight::clock(const ClockRead& r,const std::string& boot){
    if(!(boot==boot_&&r.source>0&&r.raw>0&&r.mono>0&&r.boottime>0
      &&r.observed>=start_&&r.observed-start_<=4000000000LL&&r.observed>last_read_)){
        log_policy("Camera clock boot/order/window invalid; read dropped");return;
    }
    last_read_=r.observed;
    if(r.observed-start_<1000000000)return; // original one-second backlog settle
    if(reads_.size()>=4096){log_policy("Camera clock read cap; read dropped");return;}
    reads_.push_back(r);
}
void CameraPreflight::exposure(const std::string& sensor,double raw,double seconds,int64_t now){
    if(!(now>=start_&&now-start_<=12000000000LL)){log_policy("Camera preflight deadline; exposure dropped");return;}
    if(sensor!=pico::devices::upper(0)&&sensor!=pico::devices::upper(1))return;
    if(!(std::isfinite(raw)&&raw>0&&std::isfinite(seconds)&&seconds>=.000750&&seconds<=.000850)){
        log_policy("Wrong upper-camera exposure; dropped");return;
    }
    auto& v=exposures_[sensor];
    // An omission restarts the cadence from this exposure instead of dropping every later one.
    if(!v.empty()&&!(raw-v.back()>.032&&raw-v.back()<.035))log_policy("Camera cadence omission/change; continuing from this exposure");
    if(v.size()>=512){log_policy("Camera preflight exposure cap; dropped");return;}
    v.push_back(raw);
}
CameraPreflightResult CameraPreflight::finish(int64_t now,const std::string& boot,bool healthy,bool cancelled){
    CameraPreflightResult out;out.boot=boot_;out.dataset=dataset_;out.calibration=calibration_;
    if(!(healthy&&!cancelled&&boot==boot_&&now>=start_&&now-start_>=4000000000LL
      &&now-start_<=12000000000LL&&reads_.size()>=40&&exposures_.size()==2)){
        log_policy("Camera preflight incomplete","reads="+std::to_string(reads_.size())+" sensors="+std::to_string(exposures_.size()));
        out.complete=false;
    }
    for(const auto& item:exposures_)if(item.second.size()<6){
        log_policy("Six upper exposures required per sensor; incomplete",item.first);out.complete=false;
    }
    for(const auto& r:reads_){std::array<int64_t,3> clocks{r.raw,r.mono,r.boottime};
        for(size_t i=0;i<3;++i)if(clocks[i]>=r.source&&clocks[i]-r.source<=250000000)out.causal_fraction[i]+=1.;}
    if(!reads_.empty())for(auto& f:out.causal_fraction)f/=double(reads_.size());
    if(!(out.causal_fraction[0]>=.9)){
        log_policy("Current boot RAW causal fraction below .9",std::to_string(out.causal_fraction[0]));out.complete=false;
    }
    out.ambiguous=out.causal_fraction[1]>=.9||out.causal_fraction[2]>=.9;
    pico::guard_evidence::emit(21,"clock-preflight",out.complete);
    return out;
}
PublicationExpiry::PublicationExpiry(std::function<bool(const OpticalPoseFrame&,int64_t)> f):send_(std::move(f)){require(bool(send_),"Publication sender missing");}
bool PublicationExpiry::send(OpticalPoseFrame frame,int64_t now){
    require(sequence_<std::numeric_limits<uint64_t>::max(),"Publication sequence exhausted");
    if(now<=0||now<now_){log_policy("Publication clock regressed; frame dropped");return false;}
    try{
        frame.sequence=++sequence_;(void)frame.encode(now);
        if(!send_(frame,now)){log_policy("Publication send failed; retrying with next frame");return false;}
    }catch(const std::exception& e){log_policy("Publication send failed; retrying with next frame",e.what());return false;}
    now_=now;return true;
}
void PublicationExpiry::optical(OpticalPoseFrame frame,int64_t now){
    if(!(frame.valid&&frame.connected&&frame.stamp>anchor_&&(!last_||last_->chip==frame.chip))){
        log_policy("Invalid publication anchor/peer; pose dropped");return;
    }
    if(!send(frame,now))return;
    anchor_=frame.stamp;last_=frame;invalid_=false;
}
void PublicationExpiry::poll(int64_t now,bool connected,bool allowed){
    if(now<now_){log_policy("Publication clock regressed; poll skipped");return;}
    if(!last_){now_=now;return;}
    if(now<anchor_){log_policy("Publication clock precedes optical anchor; poll skipped");return;}
    if(connected&&allowed&&!invalid_&&now-anchor_<250000000){pico::guard_evidence::emit(26,"expiry",true);now_=now;return;}
    if(invalid_&&last_connected_==connected&&now-status_<100000000){now_=now;return;}
    auto frame=*last_;frame.stamp=now;frame.connected=connected;frame.valid=false;frame.quality=0;
    frame.velocity={};frame.omega={};
    pico::guard_evidence::emit(26,"expiry",false);
    if(!send(frame,now))return; // invalid status is retried on the next poll
    invalid_=true;last_connected_=connected;status_=now;
}
}
