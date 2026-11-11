#pragma once
#include "camera_preflight.hpp"
#include <limits>

template<class Check,class Reject> void camera_preflight_tests(Check check_result,Reject reject_result){
#define check(...) check_result(bool(__VA_ARGS__),#__VA_ARGS__,__LINE__)
#define reject(...) reject_result(__VA_ARGS__,#__VA_ARGS__,__LINE__)
    using namespace pico;
    const int64_t start=10000000000LL;
    auto row=[](int i,int sensor){camera::AccessUnit a;a.key={38,0,0};
        a.timestamp=11.+i/30.;a.exposure=.0008;
        a.sensor=sensor?"CAMUPPERRIGHT":"CAMUPPERLEFT";return a;};
    auto feed=[&](CameraAcquisition& p,int bad=0,bool ambiguous=false){
        for(int i=0;i<20;++i)for(int s=0;s<2;++s){auto a=row(i,s);auto stamp=nearest_even(*a.timestamp*1e9);
            p.observe(a,stamp,{stamp+(i*2+s<bad?250000001:1000000),
                stamp+(ambiguous?1000000:900000000),stamp+(ambiguous?1000000:900000000)});}
    };
    {CameraAcquisition p(start,"boot","recording","calibration");reject([&]{p.proof();});
     feed(p,4);check(!p.poll(start+3999999999LL,"boot"));check(p.poll(start+4000000000LL,"boot"));
     const auto& proof=p.proof();check(proof.samples==40&&proof.causal[0]==36&&!proof.ambiguous);
     check(proof.exposures[0]==20&&proof.exposures[1]==20&&proof.clock.boot=="boot"&&proof.dataset=="recording");
     check(proof.difference_bounds[0][0]==1000000&&proof.difference_bounds[0][1]==250000001);}
    {CameraAcquisition p(start,"boot","recording","calibration");feed(p,0,true);check(p.poll(start+4000000000LL,"boot")&&p.proof().ambiguous);}
    {CameraAcquisition p(start,"boot","recording","calibration");feed(p,5);reject([&]{p.poll(start+4000000000LL,"boot");});reject([&]{p.poll(start+4000000001LL,"boot");});}
    {CameraAcquisition p(start,"boot","recording","calibration");reject([&]{p.poll(start+4000000000LL,"boot");});}
    {CameraAcquisition p(start,"boot","recording","calibration");feed(p);reject([&]{p.poll(start+4000000000LL,"other");});reject([&]{p.proof();});}
    {CameraAcquisition p(start,"boot","recording","calibration");reject([&]{p.poll(start-1,"boot");});}
    {CameraAcquisition p(start,"boot","recording","calibration");reject([&]{p.poll(start+12000000000LL,"boot");});}
    for(int fault=0;fault<9;++fault){CameraAcquisition p(start,"boot","recording","calibration");auto a=row(0,0);
        if(fault==0)a.timestamp.reset();
        if(fault==1)a.timestamp=std::numeric_limits<double>::quiet_NaN();
        if(fault==2)a.exposure.reset();
        if(fault==3)a.exposure=.000749;
        if(fault==4)a.exposure=.000851;
        if(fault==5)a.exposure=std::numeric_limits<double>::quiet_NaN();
        if(fault>=6){p.observe(a,11000000000LL,{11000000000LL,11000000000LL,11000000000LL});
            a.timestamp=11.+(fault==6?0.:fault==7?.031:.036);}
        reject([&]{p.observe(a,11100000000LL,{11100000000LL,11100000000LL,11100000000LL});});
        reject([&]{p.poll(11200000000LL,"boot");});
    }
    {CameraAcquisition p(start,"boot","recording","calibration");auto a=row(0,0);
     reject([&]{p.observe(a,11000000000LL,{0,11000000000LL,11000000000LL});});}
    // Backlog contributes exposure proof but never clock-domain samples.
    {CameraAcquisition p(start,"boot","recording","calibration");auto a=row(0,0);a.timestamp=10.1;
     p.observe(a,start+100000000,{0,0,0});reject([&]{p.poll(start+4000000000LL,"boot");});}
    // Forty causal reads from one camera cannot substitute for two upper views.
    {CameraAcquisition p(start,"boot","recording","calibration");for(int i=0;i<40;++i){auto a=row(i,0);auto stamp=nearest_even(*a.timestamp*1e9);p.observe(a,stamp,{stamp,stamp,stamp});}
     check(!p.poll(start+4000000000LL,"boot"));reject([&]{p.poll(start+12000000000LL,"boot");});}
}

#undef check
#undef reject
