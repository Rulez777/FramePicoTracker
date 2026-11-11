#pragma once
#include "phase5/tracking_guard.hpp"
#include "pose_publication.hpp"
#include <exception>
#include <functional>
#include <map>
namespace pico {
// Every dropped sample/skipped iteration below is counted per reason and logged
// to stderr (first and every 100th time). Nothing here ends a hardware run.
uint64_t live_policy_drops(const std::string& reason);
// Nonblocking read callback returns at most requested bytes. EOF is distinct
// from EAGAIN. EOF, overflow and malformed rows are logged, never hidden.
struct ObserverRead { std::string bytes; bool eof=false; };
struct ObserverHooks {
    std::function<ObserverRead(size_t)> read;
    std::function<bool()> child_alive, parent_alive, cancelled;
    std::function<int64_t()> now;
};
class TrackingObserver {
    ObserverHooks h_;
    std::string pending_;
    bool eof_=false,skip_line_=false;
    int64_t started_=0,last_poll_=0;
    std::optional<int64_t> last_row_;
public:
    explicit TrackingObserver(ObserverHooks);
    void poll(phase5::BenchTrackingGuard&);
    bool alive() const {
        return h_.child_alive()&&h_.parent_alive()&&!h_.cancelled();
    }
};
struct ImuCalibration {
    std::array<double,9> axes{};
    std::array<double,3> bias{};
    double gyro_scale=0,accel_scale=0;
    int64_t offset=0;
    bool axes_verified=false,time_verified=false;
};
// Admission and bounded propagation policy, NOT an EKF/calibration estimator.
// Covariance is checked before commit; reconnect revokes time until remeasured.
// A rejected calibration/anchor is logged and leaves samples unadmitted.
class ImuAdmission {
    ImuCalibration c_;
    bool configured_=false,time_=false,invalid_=true,diverged_=false;
    int64_t anchor_=0,state_=0;
public:
    void configure(const ImuCalibration&);
    void reconnect();
    void set_time(bool measured);
    void optical_anchor(int64_t stamp,int64_t now);
    void invalidate(){invalid_=true;}
    bool sample(int64_t stamp,int64_t now,const std::array<double,3>& gyro,
                const std::array<double,3>& accel,double position_sigma);
    int64_t state()const{return state_;}
};
struct ClockRead { int64_t source=0,raw=0,mono=0,boottime=0,observed=0; };
struct CameraPreflightResult {
    std::array<double,3> causal_fraction{};
    bool ambiguous=false;
    bool complete=true; // false: logged why; caller keeps the best available result
    std::string boot,dataset,calibration;
};
class CameraPreflight {
    std::string boot_,dataset_,calibration_;
    int64_t start_,last_read_=0;
    std::vector<ClockRead> reads_;
    std::map<std::string,std::vector<double>> exposures_;
public:
    CameraPreflight(std::string boot,std::string dataset,std::string calibration,int64_t start);
    void clock(const ClockRead&,const std::string& boot);
    void exposure(const std::string& sensor,double raw,double seconds,int64_t now);
    CameraPreflightResult finish(int64_t now,const std::string& boot,bool source_healthy,bool cancelled);
};
// Single enrolled peer publication owner. A failed send is logged and the next
// frame is tried; expiry emits invalid every 100ms, including identity loss.
class PublicationExpiry {
    std::function<bool(const OpticalPoseFrame&,int64_t)> send_;
    std::optional<OpticalPoseFrame> last_;
    uint64_t sequence_=0;
    int64_t anchor_=0,status_=0,now_=0;
    bool invalid_=false,last_connected_=false;
    bool send(OpticalPoseFrame,int64_t);
public:
    explicit PublicationExpiry(std::function<bool(const OpticalPoseFrame&,int64_t)>);
    void optical(OpticalPoseFrame,int64_t now);
    void poll(int64_t now,bool connected,bool identity_allowed);
};
}
