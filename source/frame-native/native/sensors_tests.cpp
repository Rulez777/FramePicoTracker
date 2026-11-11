#include "sensors.hpp"
#include "sensor_status.hpp"
#include "imu_slot.hpp"
#include "station_sensors/sensor_capture.h"
#include "../driver/pico_fusion.h"
#include "../driver/sensor_config.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#ifdef SENSOR_BASELINE
#include "../outputs/sensors_baseline.h"
#endif
using namespace pico;
using namespace pico_fusion;
static unsigned checks=0;
static void check(bool b) { ++checks;if(!b){std::fprintf(stderr,"FAIL %u\n",checks);std::exit(1);} }
int main() {
    { // Synthetic raw fields only: no calibration or device data.
        SensorStats a,b;
        check(a.magInterval.log().find("mag_raw=NA")!=std::string::npos);
        check(a.mag({32768,32768,32768}));
        check(!a.mag({32768,32768,32768}));
        check(a.magInterval.count==1 && a.magInterval.maxStep==0);
        check(a.mag({33792,32768,32768}));
        check(a.mag({32768,30720,32768}));
        const auto &m=a.magInterval;
        check(m.count==3 && a.magFresh==3 && a.magRepeat==1);
        check(m.raw==std::array<uint16_t,3>{32768,30720,32768});
        check(m.last==std::array<double,3>{0,-200,0});
        check(m.minimum==std::array<double,3>{0,-200,0});
        check(m.maximum==std::array<double,3>{100,0,0});
        check(m.normMin==0 && m.normSum/m.count==100 && m.normMax==200);
        check(std::abs(m.maxStep-std::sqrt(50000.0))<1e-9);
        check(m.log().find("mag_norm_uT=0.000,100.000,200.000")!=std::string::npos);
        check(b.magInterval.count==0 && !b.previous);
        a.magInterval=MagInterval{};
        check(!a.mag({32768,30720,32768}) && a.magInterval.count==0);
        check(a.magInterval.log().find("mag_uT=NA")!=std::string::npos);
        check(a.mag({32768,32768,32768}));
        check(a.magInterval.count==1 && a.magInterval.maxStep==200);
        check(a.magInterval.normMax==0 && a.magInterval.minimum[1]==0);
        check(b.mag({0,65535,32768}));
        check(b.magInterval.last[0]==-3200 && b.magInterval.last[1]==3199.90234375);
        check(a.magInterval.count==1);
    }
#include "alpha_sensor_tests.inc"
    for(unsigned peers:{2u,3u}) for(unsigned peer=1;peer<=peers;++peer) {
        unsigned decoded=0;std::vector<pico_sensors::Wire> telemetry;
        Hooks hooks;hooks.now=[] {return 10000000000LL;};
        hooks.decoded=[&](const auto&,const auto&){++decoded;};
        hooks.sensor=[&](const auto&v){telemetry.push_back(v);return true;};
        DataPlane plane(hooks);plane.chip=peer;plane.enrolled=true;plane.sensor_format=SensorFormat::Alpha12;
        for(unsigned count=0;count<=3;++count) {
            Bytes body{uint8_t(0xcc|count),0x80,0xff};
            for(unsigned i=0;i<count;++i){Bytes sample(16);sample[15]=uint8_t(peer+i);body.insert(body.end(),sample.begin(),sample.end());}
            body.insert(body.end(),{0,128,0,128,0,128});
            Record r;r.chip=peer;r.hardware_crc=true;r.channel=2;r.profile=0;
            r.dma={uint8_t(body.size()+4),2,0,2,uint8_t(body.size())};r.dma.insert(r.dma.end(),body.begin(),body.end());
            unsigned sum=0;for(size_t k=2;k<r.dma.size();++k)sum+=r.dma[k];
            r.dma.push_back(uint8_t(sum));
            const auto old=decoded;plane.record(r);check(decoded==old+count);
            check(telemetry.back().chip==peer && (telemetry.back().flags&pico_sensors::Mag));
            if(count==0)check(telemetry.front().flags==pico_sensors::TemperatureSnapshot);
        }
    }
    for(auto fmt:{SensorFormat::Stock189,SensorFormat::Diagnostic7,SensorFormat::Release11})
    for(size_t n:{6u,18u,24u,36u,42u}) {
        Bytes b(n); if(n%18==6) { b[n-6]=0x34;b[n-5]=0x92;b[n-4]=0x78;b[n-3]=0x56;b[n-2]=0xff;b[n-1]=0xff; }
        auto x=decode_sensors(b,fmt);check(x.imu.size()==n/18);check(bool(x.mag)==(n%18==6));
        if(x.mag) check((*x.mag)[0]==0x9234 && (*x.mag)[1]==0x5678 && (*x.mag)[2]==65535);
    }
    for(size_t n=0;n<80;++n) if(n!=6&&n!=18&&n!=24&&n!=36&&n!=42) {
        bool threw=false;try{decode_sensors(Bytes(n));}catch(...){threw=true;}check(threw);
    }
    bool threw=false;try{decode_sensors(Bytes(24),SensorFormat::Unknown);}catch(...){threw=true;}check(threw);
    check(pico_sensors::AccelLsb(0x44)==1024);check(pico_sensors::AccelLsb(0x47)==2048);
    check(pico_sensors::AccelLsb(0)==0);check(pico_sensors::GyroScale(0x44)==2*pico_sensors::GyroScale(0x47));
    check(pico_sensors::Temperature(0)==25);check(pico_sensors::Microtesla(32768)==0);
    check(pico_sensors::Microtesla(33792)==100);
    SensorStats a,b;check(a.mag({1,2,3}));check(!a.mag({1,2,3}));check(b.mag({1,2,3}));check(a.mag({1,2,4}));
    a.temperature(-13248);a.temperature(13248);check(std::fabs(a.tempMin+75)<1e-10&&std::fabs(a.tempMax-125)<1e-10);
    pico_sensors::Wire w;w.chip=1;w.hostNs=1;w.flags=pico_sensors::Mag|pico_sensors::Fresh;
    pico_sensors::Wire decoded;check(pico_sensors::Decode(&w,sizeof(w),decoded));
    check(!pico_sensors::Decode(&w,32,decoded));w.flags=0x8000;check(!pico_sensors::Decode(&w,sizeof(w),decoded));
    Record disc;disc.chip=1;disc.dma={13,0,2,0,10,0,0,1,0,0,0,0,0x26,0xbd,0};
    check(decode_discovery_power(disc,w));check(w.batteryLevel==6&&w.chargerRaw==2&&w.millivolts==0);
    disc.dma.pop_back();check(!decode_discovery_power(disc,w));
    // Admission and per-chip transport: real canonical walker, additive PTS1, unchanged PTI1.
    std::vector<pico_sensors::Wire> sent;
    Hooks h;h.now=[] {return 10000000000LL;};h.sensor=[&](const auto&v){sent.push_back(v);return true;};
    DataPlane dp(h);dp.chip=1;dp.enrolled=true;dp.imu_id=0x47;dp.sensor_id_loaded=true;
    Record r;r.hardware_crc=true;r.chip=1;r.channel=2;r.profile=0;
    Bytes body(24);body[18]=1;body[19]=0x80;body[21]=0x80;body[23]=0x80;
    auto packet=[&] {r.dma={uint8_t(body.size()+4),2,0,2,uint8_t(body.size())};r.dma.insert(r.dma.end(),body.begin(),body.end());unsigned sum=0;for(size_t i=2;i<r.dma.size();++i)sum+=r.dma[i];
    r.dma.push_back(uint8_t(sum));};
    packet();dp.record(r);check(sent.size()==2);check(sent[0].flags&pico_sensors::Imu);check(sent[1].flags&pico_sensors::Fresh);
    dp.record(r);check(sent.size()==4&&!(sent.back().flags&pico_sensors::Fresh));
    r.chip=2;dp.record(r);check(sent.size()==4);r.chip=1;r.hardware_crc=false;dp.record(r);check(sent.size()==4);
    r.hardware_crc=true;body=Bytes{1,128,0,128,0,128};packet();dp.record(r);check(sent.size()==5);
    check(!(sent.back().flags&pico_sensors::Imu));
    Record power;power.hardware_crc=true;power.channel=37;power.chip=99;
    power.dma={13,0,2,0,10,0,0,1,0,0,0,0,0x26,0xbd,0};
    dp.record(power);check(sent.size()==6&&sent.back().chip==1&&(sent.back().flags&pico_sensors::Power));
    power.dma[7]=2;dp.record(power);check(sent.size()==6); // broadcast from another chip
    // Regression: old compact module can only transport the 18 IMU bytes, not its magnetic tail.
    std::array<uint8_t,48> compact{};compact[10]=2;compact[11]=18;
    auto truncated=expand_imu_slot(compact.data());
    auto oldRecords=decode_records(Bytes(truncated.dma.begin()+2,truncated.dma.end()));
    check(!decode_sensors(oldRecords[0].body).mag);
    // The full ABI4 record retains all 42 body bytes, attributed to the peer's actual chip.
    Bytes full(112);full[12]=2;full[100]=1;Bytes original(42);original[36]=1;original[37]=128;original[39]=128;original[41]=128;
    Bytes dma{46,2,0,2,42};dma.insert(dma.end(),original.begin(),original.end());unsigned checksum=0;for(size_t j=2;j<dma.size();++j)checksum+=dma[j];
    dma.push_back(uint8_t(checksum));
    full[16]=uint8_t(dma.size());std::copy(dma.begin(),dma.end(),full.begin()+20);
    auto preserved=parse_record(full,true);auto preservedCommands=decode_records(Bytes(preserved.dma.begin()+2,preserved.dma.end()));
    check(decode_sensors(preservedCommands[0].body).mag==std::optional<std::array<uint16_t,3>>({32769,32768,32768}));
    static_assert(sizeof(PicoSensorRecord)==112,"full ABI4 sensor ring");
    for(unsigned peer=1;peer<=3;++peer)for(size_t n:{6u,18u,24u,36u,42u}) {
        Bytes payload(n);for(size_t j=0;j<n;++j)payload[j]=uint8_t(j);
        Bytes d{uint8_t(n+4),2,0,2,uint8_t(n)};d.insert(d.end(),payload.begin(),payload.end());d.push_back(0);
        PicoSensorRecord captured{};pico_sensor_record(&captured,d.data(),d[0],peer,1234,5,6,7,2);
        Bytes raw(reinterpret_cast<const uint8_t*>(&captured),reinterpret_cast<const uint8_t*>(&captured)+112);
        auto rr=parse_record(raw,true);check(rr.dma==d&&rr.chip==peer&&rr.cycles==1234&&rr.generation==6);
        const auto ack=pico_sensor_ack(d.data(),d[0]);
        check(ack==(n==6?0u:0x100u|unsigned(payload[n>=36?35:17])));
    }
    for(unsigned n=0;n<256;++n) {
        uint8_t bad[80]={8,2,0,2,uint8_t(n)};
        check(pico_sensor_ack(bad,8)==0); // bounded even for 0xFD; no out-of-record reads
    }
    // Legacy decoder alone proves the previous dropped-tail failure.
    check(decode_imu(body).empty());check(decode_sensors(body).mag.has_value());
    MagCalibration mc;mc.valid=true;
    MagneticState ms;Quat q;
    for(int i=1;i<=200;++i)check(ms.Update(i*10000000.,{30,15,5},q,true,mc));
    const auto saved=q;check(!ms.Update(201e7,{300,150,50},q,false,mc));check(q.w==saved.w&&q.y==saved.y);
    // Heading sign, tilt compensation and slow convergence during loss.
    MagneticState drift;Quat estimate;
    check(drift.Update(1e9,{30,15,5},estimate,true,mc));
    estimate=Exp({0,.2,0});const double before=Norm(Log(estimate));
    for(int i=1;i<=6000;++i)drift.Update(1e9+i*1e7,{30,15,5},estimate,false,mc);
    check(Norm(Log(estimate))<before*.1);check(drift.corrections>5000);
    MagneticState rotate;Quat truth;
    for(int i=1;i<=500;++i) {
        truth=Exp({.2*std::sin(i*.01),i*.002,.1});
        auto raw=Rotate(Conj(truth),{30,15,5});Quat est=truth;
        check(rotate.Update(i*1e7,raw,est,true,mc));check(Norm(Log(Mul(Conj(truth),est)))<1e-10);
    }
    ThermalBias thermal;for(int i=0;i<500;++i){double t=20+i*.02;thermal.Observe(t,{2*t,-t,.5*t});}
    auto term=thermal.Term(30,25);check(std::fabs(term.x-10)<1e-8&&std::fabs(term.y+5)<1e-8);
    ThermalBias flat;for(int i=0;i<100;++i)flat.Observe(25,{1,2,3});
    check(Norm(flat.Term(30,25))==0);
    {
        const char*path="build/sensors-test-calibration.txt";
        {std::ofstream f(path);f<<"1 1 0 0 0 1 0 0 0 1 1 2 3 1 0 0 1 0 0 0 1 0 0 0 1\n";}
        MagCalibration loaded;check(LoadMagCalibration(path,1,loaded));check(loaded.offset.y==2&&loaded.valid);
        check(!LoadMagCalibration(path,2,loaded));
        {std::ofstream f(path);f<<"1 1 0 0 0 1 0 0 0 1 0 0 0 0 0 0\n";}
        check(!LoadMagCalibration(path,1,loaded)); // no fabricated magnetic mounting
        std::remove(path);
    }
#if PICO_MAG_FUSION
    {
        Calibration c;c.valid=true;c.gyroRadPerLsb=.001;
        Tracker corrected(c),uncorrected(c);corrected.SetMagCalibration(mc);
        for(int i=1;i<=4000;++i) {
            const uint64_t t=1000000000ull+i*10000000ull;int16_t g[3]={0,10,0},acc[3]={0,1024,0};
            corrected.SensorImu(0,0x44);corrected.Imu(t,g,acc);uncorrected.Imu(t,g,acc);
            if(i<100&&i%3==0) {corrected.Optical(t,{0,0,0},{});uncorrected.Optical(t,{0,0,0},{});}
            corrected.Magnetometer(t+1,{30,15,5});
        }
        auto x=corrected.Pose(41000000001ull),y=uncorrected.Pose(41000000001ull);
        check(x.valid&&y.valid);check(std::fabs(Log(x.orientation).y)<std::fabs(Log(y.orientation).y));
        auto diag=corrected.MagneticDiagnostics();check(diag.accepted>3000&&diag.corrections>3000);
        // Deferred optical delivery at the same exposure must rebuild magnetic state.
        Tracker delayed(c);delayed.SetMagCalibration(mc);
        for(int i=1;i<150;++i) {
            const uint64_t t=1000000000ull+i*10000000ull;
            int16_t g[3]={0,0,0},acc[3]={0,1024,0};delayed.Imu(t,g,acc);
            delayed.Magnetometer(t+1,{30,15,5});
            if(i>10) delayed.Optical(t-100000000,{0,0,0},{});
        }
        check(delayed.MagneticDiagnostics().corrections==0);
    }
#endif
    // Default OFF must preserve actual fusion output exactly across delayed fixes and a gap.
#ifdef SENSOR_BASELINE
    Calibration c;c.valid=true;c.gyroRadPerLsb=.002138;
    pico_baseline::Calibration bc;bc.valid=true;bc.gyroRadPerLsb=c.gyroRadPerLsb;
    Tracker tr(c);pico_baseline::Tracker old(bc);
    for(int i=1;i<2000;++i) {
        uint64_t t=1000000000ull+i*5000000ull;int16_t g[3]={int16_t(i%19),-4,3},acc[3]={0,1024,0};
        tr.SensorImu(int16_t(i),0x47);tr.Imu(t,g,acc);old.Imu(t,g,acc);tr.Magnetometer(t,{30,15,5});
        if(i%7==0 && (i<1000||i>1300)) {tr.Optical(t-20000000,{.1,.2,.3},{});old.Optical(t-20000000,{.1,.2,.3},{});}
        auto x=tr.Pose(t+10000000);auto y=old.Pose(t+10000000);
        check(x.orientation.w==y.orientation.w&&x.orientation.x==y.orientation.x&&x.orientation.y==y.orientation.y&&x.orientation.z==y.orientation.z);
        check(x.position.x==y.position.x&&x.position.y==y.position.y&&x.position.z==y.position.z&&x.valid==y.valid);
    }
#endif
    std::printf("sensors_tests PASS %u\n",checks);
}
