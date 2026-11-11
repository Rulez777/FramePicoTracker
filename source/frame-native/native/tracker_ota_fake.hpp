#pragma once
#include "tracker_ota.hpp"
#include "fake_openocd.hpp"
namespace pico {
// Deliberately separate fake tracker: validates wire length/index/padding/CRC,
// rather than acknowledging whatever the sender emits. No hardware adapter.
class FakeOtaTracker {
    Bytes received_;
    size_t size_ = 0;
    uint8_t crc_ = 0;
    unsigned next_ = 1;
    Bytes last_request_, last_reply_;
    Bytes retained_;
    bool data_phase_=false;
  public:
    bool finished = false;
    Bytes respond(const Mailbox &m) {
        if (m.payload == Bytes{0}) {
            if(!retained_.empty()) {
                auto reply=retained_;
                // service38: tag-0 fallback cannot retire a retained ACK.
                if(uint16_t(m.generation))retained_.clear();
                return reply;
            }
            if(data_phase_)return {0,1,1,0x99,0x9b}; // continuous unrelated telemetry
            return {0};
        }
        if(m.payload==last_request_) return last_reply_;
        const auto records=decode_records(m.payload);
        if (records.size()!=1 || records[0].command!=1) throw std::runtime_error("Fake OTA bad record");
        const auto &b=records[0].body;
        if (b.size()<2 || b[0]!=0x25) throw std::runtime_error("Fake OTA bad opcode");
        Bytes ack{0x25,b[1],1};
        if (b[1]==1) {
            data_phase_=false;retained_.clear();
            if (b.size()!=9 || b[7]!=0 || b[8]!=189) throw std::runtime_error("Fake OTA bad start");
            size_=(size_t(b[2])<<24)|(size_t(b[3])<<16)|(size_t(b[4])<<8)|b[5];
            crc_=b[6]; received_.clear(); next_=1;
        } else if (b[1]==2) {
            data_phase_=true;
            if (b.size()!=29 || unsigned(b[2])*256+b[3]!=next_ || b[4]>24 ||
                b[4]!=std::min(size_t(24),size_-received_.size())) throw std::runtime_error("Fake OTA bad data");
            for (size_t i=5+b[4];i<b.size();++i) if (b[i]!=255) throw std::runtime_error("Fake OTA bad padding");
            received_.insert(received_.end(),b.begin()+5,b.begin()+5+b[4]);
            ack={0x25,2,b[2],b[3],1}; ++next_;
        } else if (b[1]==3) {
            data_phase_=false;
            if (b.size()!=2 || received_.size()!=size_ || next_!=size_/24+2 || ota_crc(received_)!=crc_)
                throw std::runtime_error("Fake OTA finish integrity failure");
            finished=true;
        } else throw std::runtime_error("Fake OTA unexpected operation");
        Bytes out{0,1,uint8_t(ack.size())}; out.insert(out.end(),ack.begin(),ack.end());
        unsigned sum=0; for(auto v:out) sum+=v;
        out.push_back(uint8_t(sum)); last_request_=m.payload;last_reply_=out;
        if(b[1]==2) {retained_=out;return {0,1,1,0x99,0x9b};}
        return out;
    }
};
inline Bytes fake_service_proof(const Bytes &nonce, const Bytes &chip) {
    Bytes p{0xfe,2}; p.insert(p.end(),nonce.begin(),nonce.end()); p.insert(p.end(),chip.begin(),chip.end());
    p.insert(p.end(),{0,11,9,0}); return p;
}
inline void run_fake_ota(TrackerOta &sender, const std::function<void(unsigned,unsigned)> &progress) {
    FakeOpenOcd fake;
    DebugMemory debug([&](const std::string &s){return fake.command(s);});
    MailboxWriter writer(debug);
    FakeOtaTracker tracker;
    for (unsigned i=0;i<100000 && !sender.terminal();++i) {
        const auto box=sender.mailbox();
        writer.publish(0,box.generation,box.s1,box.payload);
        // Read back through the fake TCL parser, including mailbox seqlock.
        const auto words=debug.read(roy_data+1664,16);
        if (word(words,0)!=box.generation) throw std::runtime_error("Fake TCL mailbox mismatch");
        sender.receive(tracker.respond(box),true,3,uint16_t(box.generation));
        if (progress) progress(sender.acked_blocks,sender.blocks());
    }
    if (sender.state!=TrackerOta::State::Complete || !tracker.finished)
        throw std::runtime_error("Fake OTA did not finish: "+sender.error);
}
} // namespace pico
