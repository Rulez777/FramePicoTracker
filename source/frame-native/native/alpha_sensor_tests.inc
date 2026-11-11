// Synthetic fixtures follow prepare_imu byte order and temperature 0x2120.
// Ticks here are legal; the firmware verifier seeds out-of-range ticks.
// Actual revision-12 image execution using prepare_imu (only capture ticks made legal).
const Bytes firmwareVectors[]={
 {206,32,33,13,14,15,16,17,18,19,20,21,22,23,24,1,2,3,13,14,15,16,17,18,19,20,21,22,23,24,25,1,2,3,14,134,135,136,137,138,139},
 {203,32,33,13,14,15,16,17,18,19,20,21,22,23,24,1,2,3,13,14,15,16,17,18,19,20,21,22,23,24,25,1,2,3,14,15,16,17,18,19,20,21,22,23,24,25,26,1,2,3,15}
};
for(const auto& vector:firmwareVectors) {
    const auto b=decode_sensors(vector,SensorFormat::Alpha12);
    check(b.imu.size()==size_t(vector[0]&3));check(b.temperature==0x2120);
    check(b.imu.front().gyro[0]==0x0e0d && b.imu.back().counter==12+b.imu.size());
    if(b.mag)check((*b.mag)[0]==0x8786 && (*b.mag)[1]==0x8988 && (*b.mag)[2]==0x8b8a);
}
for(unsigned count=0;count<=3;++count) for(unsigned mag=0;mag<=1;++mag) {
    Bytes b{uint8_t(0xc8|count|(mag?4:0)),0x20,0x21};
    for(unsigned i=0;i<count;++i) {
        for(unsigned j=0;j<12;++j)b.push_back(uint8_t(13+i+j));
        b.insert(b.end(),{1,2,3,uint8_t(13+i)});
    }
    if(mag)b.insert(b.end(),{0x86,0x87,0x88,0x89,0x8a,0x8b});
    const auto decoded=decode_sensors(b,SensorFormat::Alpha12);
    check(decoded.imu.size()==count && bool(decoded.mag)==bool(mag));
    check(decoded.temperature==0x2120);
    for(unsigned i=0;i<count;++i) {check(decoded.imu[i].counter==13+i);check(decoded.imu[i].ticks==0x30201);check(decoded.imu[i].temperature==0x2120);}
    bool rejected=false;try{decode_imu(b);}catch(...){rejected=true;}check(rejected);
    Bytes dma{uint8_t(b.size()+4),2,0,2,uint8_t(b.size())};dma.insert(dma.end(),b.begin(),b.end());dma.push_back(0);
    check(pico_sensor_ack_revision(dma.data(),dma[0],12)==(count?(0x100u|(12+count)):0));
    check(pico_sensor_ack(dma.data(),dma[0])==(count?(0x100u|(12+count)):0));
    check(pico_sensor_ack_revision(dma.data(),dma[0],7)==0);
    for(unsigned peers:{2u,3u}) for(unsigned peer=1;peer<=peers;++peer) {
        PicoSensorRecord captured{};
        pico_sensor_record(&captured,dma.data(),dma[0],peer,1234,5,6,7,2);
        Bytes raw(reinterpret_cast<const uint8_t*>(&captured),reinterpret_cast<const uint8_t*>(&captured)+112);
        const auto record=parse_record(raw,true);
        check(record.chip==peer && record.dma==dma);
        check(decode_sensors(Bytes(record.dma.begin()+5,record.dma.end()-1),SensorFormat::Alpha12).mag==decoded.mag);
    }
    b.pop_back();rejected=false;try{decode_sensors(b,SensorFormat::Alpha12);}catch(...){rejected=true;}check(rejected);
}
for(unsigned marker=0;marker<256;++marker) for(unsigned n=0;n<66;++n) {
    Bytes b(n,uint8_t(0));if(n)b[0]=uint8_t(marker);
    bool ok=true;try{decode_sensors(b,SensorFormat::Alpha12);}catch(...){ok=false;}
    check(ok==((marker&0xf8)==0xc8 && n==3+16*(marker&3)+((marker&4)?6u:0u)));
    Bytes dma{uint8_t(n+4),2,0,2,uint8_t(n)};dma.insert(dma.end(),b.begin(),b.end());dma.push_back(0);
    check(pico_sensor_ack_revision(dma.data(),dma[0],12)==(ok&&(marker&3)?0x100u:0));
}
for(unsigned peers:{2u,3u}) {
    std::vector<SensorStatus> statuses(peers);Bytes nonce(16,4);
    for(unsigned peer=0;peer<peers;++peer) for(unsigned page=0;page<8;++page) {
        Bytes chip{uint8_t(peer+1),0,0,0},b{0xfe,3};b.insert(b.end(),nonce.begin(),nonce.end());b.insert(b.end(),chip.begin(),chip.end());
        b.insert(b.end(),{1,12,uint8_t(page),6});for(unsigned k=0;k<6;++k)b.push_back(uint8_t(page*6+k+peer));
        check(statuses[peer].accept(b,nonce,chip,page,10+page));
        auto bad=b;bad[2]^=1;check(!statuses[peer].accept(bad,nonce,chip,page,100));
        bad=b;bad[18]^=1;check(!statuses[peer].accept(bad,nonce,chip,page,100));
        bad=b;bad[23]=11;check(!statuses[peer].accept(bad,nonce,chip,page,100));
        check(!statuses[peer].accept(b,nonce,chip,(page+1)%8,100));
    }
    for(unsigned peer=0;peer<peers;++peer) {
        check(statuses[peer].mask==255);check(statuses[peer].raw[20]==20+peer);
        auto text=statuses[peer].text(peer+1,20);
        check(text.find("battery_mv=")!=std::string::npos);
        check(text.find("mag_device_state_raw="+std::to_string(40+peer)+"\n")!=std::string::npos);
        check(text.find("mag_callback_gate_raw="+std::to_string(41+peer)+"\n")!=std::string::npos);
        check(text.find("mag_timer_flags="+std::to_string(42+peer)+"\n")!=std::string::npos);
        check(text.find("mag_workmode_raw="+std::to_string(43+peer)+"\n")!=std::string::npos);
        check(text.find("link_state_raw="+std::to_string(44+peer)+"\n")!=std::string::npos);
        check(text.find("mag_product_check="+std::to_string(45+peer)+"\n")!=std::string::npos);
        check(text.find("mag_reconcile_attempts_mod256="+std::to_string(46+peer)+"\n")!=std::string::npos);
        check(text.find("mag_reconcile_successes_mod256="+std::to_string(47+peer)+"\n")!=std::string::npos);
    }
}
// Alpha13 keeps revision12, extends only optional pages; older builds lack ID.
for(unsigned peers:{1u,2u,3u}) for(unsigned peer=0;peer<peers;++peer) {
    SensorStatus status;Bytes nonce(16,7),chip{uint8_t(peer+1),0,0,0};
    auto page=[&](unsigned p,Bytes data) {
        Bytes b{0xfe,3};b.insert(b.end(),nonce.begin(),nonce.end());b.insert(b.end(),chip.begin(),chip.end());
        b.insert(b.end(),{1,12,uint8_t(p),6});b.insert(b.end(),data.begin(),data.end());
        return status.accept(b,nonce,chip,p,20+p);
    };
    check(page(3,{0,0,20,3,0x13,0}));
    check(page(8,{uint8_t(peer),1,0x92,0x1a,4,0}));
    check(page(9,{uint8_t(5+peer),0,0,0,0,0}));
    auto text=status.text(peer+1,40);
    check(text.find("build_id=19\n")!=std::string::npos);
    check(text.find("strap_flag="+std::to_string(peer)+"\n")!=std::string::npos);
    check(text.find("ir_enable=1\n")!=std::string::npos);
    check(text.find("ir_width=268946\n")!=std::string::npos);
    check(text.find("mag_seq="+std::to_string(5+peer)+"\n")!=std::string::npos);
    check(!page(10,{0,0,0,0,0,0}));
    check(page(3,{0,0,20,3,0,0}));
    text=status.text(peer+1,40);check(text.find("mag_seq=")==std::string::npos);
}
