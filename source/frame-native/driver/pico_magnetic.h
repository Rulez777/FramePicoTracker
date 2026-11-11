#pragma once
// Included after the fusion vector/quaternion helpers, inside pico_fusion.
#ifndef PICO_MAG_FUSION
#define PICO_MAG_FUSION 0
#endif
#ifndef PICO_MAG_LEARN
#define PICO_MAG_LEARN 1
#endif
#ifndef PICO_TEMP_BIAS
#define PICO_TEMP_BIAS 0
#endif
#ifndef PICO_SENSOR_SCALES
#define PICO_SENSOR_SCALES 0
#endif
// Heading gates and correction (2026-11-02, outputs/mag-map-claude2/REPORT.md: the field near the desk is
// disturbed, gradients 50-77 uT/m, |B| in hand 49.3+-5.5 uT, MAG ~10 ms older than the packet's newest IMU).
#ifndef PICO_MAG_NORM_TOL
#define PICO_MAG_NORM_TOL 0.10   // | |B| - calibrated radius | below this fraction
#endif
#ifndef PICO_MAG_GYRO_UT
#define PICO_MAG_GYRO_UT 3.0     // field change over ~100 ms vs the fused-orientation prediction (uT)
#endif
#ifndef PICO_MAG_DIP_DEG
#define PICO_MAG_DIP_DEG 8.0     // angle of the field to world up vs its learned value (selected on train half)
#endif
#ifndef PICO_MAG_TAU_S
#define PICO_MAG_TAU_S 5.0       // yaw correction time constant (selected on train half)
#endif
#ifndef PICO_MAG_MAX_RATE
#define PICO_MAG_MAX_RATE 0.01   // yaw correction rate limit (rad/s)
#endif
#ifndef PICO_MAG_LATENCY_NS
#define PICO_MAG_LATENCY_NS 10e6 // MAG stamp = newest IMU sample stamp of its packet minus this
#endif
struct MagHeadingParams {
    double normTol=PICO_MAG_NORM_TOL,gyroUt=PICO_MAG_GYRO_UT,dipRad=PICO_MAG_DIP_DEG*3.141592653589793/180;
    double tauS=PICO_MAG_TAU_S,maxRate=PICO_MAG_MAX_RATE;
};
// Process-wide; the driver uses the compiled defaults, the replay tool sweeps them.
inline MagHeadingParams &MagHeading() { static MagHeadingParams p; return p; }
struct MagCalibration {
    double soft[9]={1,0,0,0,1,0,0,0,1};
    Vec3 offset;
    double radius=0; // sphere radius of the field fit (uT); 0 = unknown, the gate then uses the learned norm
    int preCoeff=0; double scalar[2]={}; // vendor scalars retained, semantics unknown
    double mount[9]={1,0,0,0,1,0,0,0,1};
    bool valid=false; // set only with an explicitly measured magnetic mounting
    static Vec3 Matrix(const double *m,Vec3 v) {
        return {m[0]*v.x+m[1]*v.y+m[2]*v.z,m[3]*v.x+m[4]*v.y+m[5]*v.z,m[6]*v.x+m[7]*v.y+m[8]*v.z};
    }
};
struct MagneticState {
    double last=0,localNorm=0,lastNorm=0,reference=0;
    double dip=0; bool haveDip=false;
    bool anchored=false,haveBounds=false;
    Vec3 lo,hi,onlineOffset; Quat firstQ;
    uint64_t accepted=0,rejected=0,corrections=0;
    uint64_t rejectNorm=0,rejectHistory=0,rejectGyro=0,rejectHorizontal=0,rejectDip=0;
    // Recent body-frame fields with the fused orientation at their time (gyro-consistency test).
    struct Past { double t=0; Vec3 body; Quat q; };
    static constexpr unsigned kPast=24;
    Past past[kPast]; unsigned pastHead=0,pastCount=0;
    static double Wrap(double x) { return std::atan2(std::sin(x),std::cos(x)); }
    // Called once per source-time event, state is replayed with the optical history.
    // A sample steers yaw only if (1) |B| is within normTol of the calibrated radius, (2) the field seen ~100 ms
    // earlier, carried to now by the fused orientation, matches within gyroUt and (3) its angle to world up matches
    // the learned dip within dipRad. Reference bearing and dip are learned only from such samples while optics is
    // visible; without optics the correction is yaw-only and rate-limited.
    bool Update(double t,Vec3 raw,Quat &q,bool visible,const MagCalibration &c,bool learn=false) {
        if(!c.valid || !std::isfinite(Norm(raw)) || t<=last) return false;
        const double dt=last?std::fmin((t-last)/1e9,.1):0; last=t;
        const MagHeadingParams &prm=MagHeading();
        Vec3 off=Add(c.offset,onlineOffset);
        const Vec3 body=MagCalibration::Matrix(c.mount,MagCalibration::Matrix(c.soft,Sub(raw,off)));
        const double norm=Norm(body);
        double change=-1,bestAge=1e18;
        for(unsigned k=0;k<pastCount;++k) {
            const Past &p=past[k];
            const double age=std::fabs(t-p.t-100e6);
            if(age<=40e6 && age<bestAge) { bestAge=age; change=Norm(Sub(body,Rotate(Conj(q),Rotate(p.q,p.body)))); }
        }
        past[pastHead]={t,body,q}; pastHead=(pastHead+1)%kPast;
        if(pastCount<kPast) ++pastCount;
        lastNorm=norm;
        if(visible && norm>10 && norm<100) localNorm=localNorm>0?localNorm+.005*(norm-localNorm):norm;
        const double radius=c.radius>0?c.radius:localNorm;
        if(norm<10 || norm>100 || !(radius>0) || std::fabs(norm-radius)>prm.normTol*radius) { ++rejected; ++rejectNorm; return false; }
        if(change<0) { ++rejected; ++rejectHistory; return false; }
        if(change>prm.gyroUt) { ++rejected; ++rejectGyro; return false; }
        const Vec3 world=Rotate(q,body);
        if(std::hypot(world.x,world.z)<.15*norm) { ++rejected; ++rejectHorizontal; return false; }
        const double currentDip=std::asin(std::fmax(-1.,std::fmin(1.,world.y/norm)));
        if((haveDip && std::fabs(currentDip-dip)>prm.dipRad) || (!haveDip && !visible)) { ++rejected; ++rejectDip; return false; }
        const double bearing=std::atan2(world.z,world.x);
        ++accepted;
        if(visible) {
            dip=haveDip?dip+.02*(currentDip-dip):currentDip;haveDip=true;
            reference=anchored?reference+.02*Wrap(bearing-reference):bearing; anchored=true;
            (void)learn; // Calibration learning is isolated in MagLearner; never replay-learn a pose.
            return true;
        }
        if(!anchored || dt<=0) return true;
        // World Y is up. Positive Y rotation decreases atan2(z,x). Yaw only: roll and pitch are untouched.
        const double error=Wrap(bearing-reference);
        const double limit=prm.maxRate*dt;
        const double step=std::fmax(-limit,std::fmin(limit,error*(1-std::exp(-dt/prm.tauS))));
        q=Normalize(Mul(Exp({0,step,0}),q)); ++corrections;
        return true;
    }
};
struct ThermalBias {
    double n=0,meanT=0,sTT=0,minT=1e9,maxT=-1e9;
    Vec3 meanG,cov;
    void Observe(double t,Vec3 g) {
        if(!std::isfinite(t)||t< -40||t>85) return;
        const double d=t-meanT; ++n;meanT+=d/n;
        const Vec3 dg=Sub(g,meanG);meanG=Add(meanG,Scale(dg,1/n));
        sTT+=d*(t-meanT); cov=Add(cov,Scale(Sub(g,meanG),d));
        minT=std::fmin(minT,t);maxT=std::fmax(maxT,t);
    }
    Vec3 Term(double t,double reference) const {
        if(n<50||maxT-minT<2||sTT<1||!std::isfinite(t)||std::fabs(t-reference)>20) return {};
        Vec3 slope=Scale(cov,1/sTT);
        if(std::fabs(slope.x)>5||std::fabs(slope.y)>5||std::fabs(slope.z)>5) return {};
        return Scale(slope,t-reference);
    }
};
