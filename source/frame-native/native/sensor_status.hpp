#pragma once
#include "data_plane.hpp"
#include <sstream>
#include <algorithm>
namespace pico {
// Eight independent query-time snapshots, never an atomic sensor measurement.
struct SensorStatus {
    std::array<uint8_t,60> raw{};
    std::array<double,10> observed{};
    unsigned mask=0;
    unsigned mag_observations=0, mag_delta=0;
    double mag_interval=0;
    bool accept(const Bytes& b,const Bytes& nonce,const Bytes& chip,unsigned page,double now) {
        if(b.size()!=32 || nonce.size()!=16 || chip.size()!=4 || page>9 ||
           b[0]!=0xfe || b[1]!=3 || b[22]!=1 || b[23]!=12 || b[24]!=page || b[25]!=6 ||
           !std::equal(nonce.begin(),nonce.end(),b.begin()+2) ||
           !std::equal(chip.begin(),chip.end(),b.begin()+18)) return false;
        if(page==3) {
            if(mag_observations){mag_delta=unsigned(uint8_t(b[28]-raw[20]));mag_interval=now-observed[3];}
            ++mag_observations;
        }
        std::copy(b.begin()+26,b.end(),raw.begin()+6*page);
        observed[page]=now;mask|=1u<<page;return true;
    }
    std::string text(uint32_t chip,double now) const {
        std::ostringstream s;s<<"version=1\nchip="<<std::hex<<chip<<std::dec<<"\npage_mask="<<mask<<"\nobserved_now="<<now<<"\n";
        for(unsigned p=0;p<10;++p) if(mask&(1u<<p)) s<<"page"<<p<<"_time="<<observed[p]<<"\n";
        auto value=[&](const char*name,unsigned o,unsigned n=1) {
            unsigned needed=0;for(unsigned k=0;k<n;++k)needed|=1u<<((o+k)/6);
            if((mask&needed)!=needed)return;
            uint32_t v=0;for(unsigned k=0;k<n;++k)v|=uint32_t(raw[o+k])<<(8*k);
            s<<name<<"="<<v<<"\n";
        };
        value("imu_variant",0);value("imu_id",1);value("imu_mode",2);value("sampling_active",3);
        value("ordinary_config_valid",4);value("gyro_config_code",5);value("accel_config_code",6);value("acquisition_profile",7);
        value("filtered_battery_mv",8,2);value("battery_level",10);value("battery_percent",11);
        value("battery_category",12);value("battery_debounce",13);value("battery_mv",14,2);
        for(unsigned i=0;i<4;++i)value(("charger_raw"+std::to_string(i)).c_str(),16+i);
        value("mag_counter",20);value("board_variant",21);value("attachment_class_raw",23);
        if(mag_observations) s<<"mag_counter_observations="<<mag_observations<<"\nmag_counter_delta_mod256="<<mag_delta
            <<"\nmag_counter_interval_s="<<mag_interval<<"\nmag_counter_changed="<<(mag_observations>1&&mag_delta!=0)<<"\n";
        value("radio_health_raw",24,4);
        for(unsigned i=0;i<4;++i)value(("radio_adjacent_raw"+std::to_string(i)).c_str(),28+i);
        value("power_marker",32);value("halt_failure",33);value("halt_failure_count",34,4);
        value("pending_mode",38);value("N",39);
        // Alpha 1.2 uses formerly-zero spare bytes; revision/layout stay 12.
        // Older revision-12 binaries report zero here: zero is not proof of
        // a diagnosed failure. Query-time pages can have different ages.
        value("mag_device_state_raw",40);value("mag_callback_gate_raw",41);
        value("mag_timer_flags",42);value("mag_workmode_raw",43);
        value("link_state_raw",44);value("mag_product_check",45);
        value("mag_reconcile_attempts_mod256",46);value("mag_reconcile_successes_mod256",47);
        // 0 means no explicit identity in older builds, not Alpha 1.0.
        value("build_id",22);
        if((mask&(1u<<3)) && raw[22]==0x13) {
            value("strap_flag",48);value("ir_enable",49);value("ir_width",50,4);
            value("mag_seq",54,4);
        }
        return s.str();
    }
};
}
