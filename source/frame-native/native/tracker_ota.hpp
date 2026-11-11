#pragma once
#include "peer_engine.hpp"
#include "ota_trace.hpp"
#include <cmath>
#include <fstream>
#include <set>
namespace phase5 { std::string sha256(const std::string &); }
namespace pico {
// FW189 command-1 OTA protocol. The caller owns the exclusive radio session.
// No reconnect, activation command, mode setter, or automatic resume is provided.
inline uint8_t ota_crc(const Bytes &b) {
    uint8_t crc = 0;
    for (auto v : b) for (unsigned i = 0; i < 8; ++i) {
        const bool mix = (crc ^ v) & 1;
        crc >>= 1; if (mix) crc ^= 0x8c;
        v >>= 1;
    }
    return crc;
}
inline bool rejected_release_hash(const std::string &hash) {
    return hash=="a21d01f5f34e00d9ef6ce17cc43c64692aa39c79c110292a19ff0448c01b4ad0" ||
           hash=="19dee5554df22cebbcf11f66924250072059a6cc772d08ad7239e9665ca62baf" ||
           hash=="e2bab6d21dbaf10ddc96d74ffd481722f65929438aa8487023614ee7824f2991";
}
inline void admit_ota(const Bytes &image, const std::string &allowlist) {
    if (image.size() < 24 || image.size() > 0xffffff || image.size()/24 + 1 > 65535)
        throw std::runtime_error("OTA image size outside FW189 fields");
    std::ifstream in(allowlist);
    if (!in) throw std::runtime_error("Explicit OTA allowlist required");
    const auto hash = phase5::sha256(std::string(image.begin(), image.end()));
    if (rejected_release_hash(hash)) throw std::runtime_error("Rejected release candidate");
    bool admitted = false;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || line[0] == '#') continue;
        if (line.size() != 64 || line.find_first_not_of("0123456789abcdef") != std::string::npos)
            throw std::runtime_error("Allowlist requires one exact lowercase SHA256 per line");
        admitted |= line == hash;
    }
    if (!admitted) throw std::runtime_error("OTA image SHA256 not admitted");
}
class TrackerOta {
  public:
    enum class State { Ready, Prepare, Start, Data, Finish, Drain, Complete, Failed };
  private:
    Bytes image_;
    std::function<double()> now_;
    State next_ = State::Ready;
    double started_ = 0, operation_ = 0, attempt_ = 0;
    uint32_t generation_;
    uint8_t sequence_, s1_ = 3;
    bool request_ = true;
    bool recovering_ = false;
    std::set<uint16_t> tags_, published_;
    void generation() {
        if ((generation_ & 65535) >= 65534) { abort("OTA tag exhaustion"); return; }
        generation_ += 2;
        tags_.insert(uint16_t(generation_));
    }
    void operation(State s) {
        recovering_ = false; state = s; operation_ = attempt_ = now_(); request_ = true;
        tags_.clear(); published_.clear(); generation();
    }
    void publication(bool request) { generation(); request_ = request; }
    static Bytes record(uint8_t seq, const Bytes &body) {
        Bytes p{seq, 1, uint8_t(body.size())}; p.insert(p.end(), body.begin(), body.end());
        unsigned sum = 0; for (auto b : p) sum += b;
        p.push_back(uint8_t(sum)); return p;
    }
  public:
    State state = State::Ready;
    unsigned index = 0, acked_blocks = 0, retries = 0, rejected = 0;
    std::string error;
    TrackerOta(Bytes image, const std::string &allowlist, std::function<double()> now,
               uint32_t generation = 0, uint8_t sequence = 0)
        : image_(std::move(image)), now_(std::move(now)), generation_(generation), sequence_(sequence) {
        admit_ota(image_, allowlist);
        if (!now_ || (generation & 1) || sequence >= 64) throw std::runtime_error("Invalid OTA initial state");
    }
    unsigned blocks() const { return unsigned(image_.size()/24 + 1); }
    bool terminal() const { return state == State::Complete || state == State::Failed; }
    void abort(const std::string &why) { state = State::Failed; error = why; }
    std::string image_sha() const { return phase5::sha256(std::string(image_.begin(),image_.end())); }
  private:
    friend class OtaRadioAdapter;
    void start_transfer() {
        if (state != State::Ready) throw std::runtime_error("OTA cannot restart");
        if ((65534-(generation_&65535))/2 < 3*blocks()+5) { abort("Insufficient OTA tags before start"); return; }
        started_=now_(); operation(State::Prepare);
    }
  public:
    // Fresh chip/nonce/CRC Service-mode proof must be validated by the adapter.
    void begin(const Bytes &proof, const Bytes &nonce, const Bytes &chip, double proof_age) {
        if (state != State::Ready) throw std::runtime_error("OTA cannot restart");
        if (nonce.size()!=16 || chip.size()!=4 || proof.size()!=26 || proof[0]!=0xfe || proof[1]!=2 ||
            !std::equal(nonce.begin(),nonce.end(),proof.begin()+2) ||
            !std::equal(chip.begin(),chip.end(),proof.begin()+18) || proof[22]!=0 ||
            !supported_wire_revision(proof[23]) || proof[25]!=0 ||
            !std::isfinite(proof_age) || proof_age<0 || proof_age>3)
            throw std::runtime_error("Fresh target-bound Service mode 0 proof required");
        start_transfer();
    }
    void tick() {
        if (terminal() || state==State::Ready) return;
        const double t=now_();
        if (t-started_>=1800 || t-operation_ >= ((state==State::Data || state==State::Drain)?10:30)) {
            abort("OTA application ACK deadline expired"); return;
        }
        if ((state==State::Data || state==State::Finish) && t-operation_>=1.0) {
            if (!recovering_) {
                recovering_=true;
                ota_trace("data recovery begin block="+std::to_string(index));
            }
            // Sweep all expected sequences, retaining exactly the same block.
            // FW189 ignores non-expected sequences; duplicate indices requeue
            // an ACK without writing again (0xfc72/0xfcca). Never restart OTA.
            if (t-attempt_>=.05) {
                attempt_=t; ++retries; sequence_=(sequence_+1)&63;
                publication(true);
                ota_trace("data recovery probe block="+std::to_string(index)+
                    " sequence="+std::to_string(sequence_)+" generation="+std::to_string(generation_));
            }
            return;
        }
        if (t-attempt_>=.25) { attempt_=t; ++retries;
            if (state==State::Data && !request_) publication(true);
        }
    }
    Mailbox mailbox() {
        if (state==State::Ready) throw std::runtime_error("Begin OTA explicitly");
        if (!terminal()) published_.insert(uint16_t(generation_));
        if (terminal() || state==State::Prepare || state==State::Drain) return {generation_,s1_,{0}};
        if (state==State::Data && !request_) return {generation_,2,{0}};
        Bytes body{0x25};
        if (state==State::Start) {
            body.push_back(1);
            for (int shift : {24,16,8,0}) body.push_back(uint8_t(image_.size() >> shift));
            body.insert(body.end(),{ota_crc(image_),0,189});
        } else if (state==State::Finish) body.push_back(3);
        else {
            body.insert(body.end(),{2,uint8_t(index>>8),uint8_t(index),0});
            const size_t off=(index-1)*24, n=std::min(size_t(24),image_.size()-off);
            body[4]=uint8_t(n); body.insert(body.end(),image_.begin()+off,image_.begin()+off+n);
            body.resize(29,255);
        }
        return {generation_,s1_,record(sequence_,body)};
    }
    bool receive(const Bytes &payload, bool crc, int s1, uint16_t tag) {
        tick(); if (terminal() || state==State::Ready) return false;
        if (!crc || (s1!=2 && s1!=3) || !tags_.count(tag) || !published_.count(tag)) { ++rejected; return false; }
        std::vector<Command> commands;
        try { commands=decode_records(payload); } catch (...) { ++rejected; return false; }
        if (state==State::Prepare || state==State::Drain) {
            if (payload!=Bytes{0}) { ++rejected; return false; }
            s1_=2;
            if (state==State::Prepare) operation(State::Start);
            else if (next_==State::Complete) state=State::Complete;
            else operation(next_);
            return true;
        }
        const unsigned expected=state==State::Start?1:state==State::Data?2:3;
        for (const auto &c : commands) {
            const auto &b=c.body;
            if (c.command!=1 || b.size()<3 || b[0]!=0x25 || b[1]!=expected) continue;
            bool success=false;
            if (expected==2) {
                if ((b.size()!=5 && b.size()!=6) || (b.size()==6 && b[5]!=0xaa) || b[4]>1 ||
                    (unsigned(b[2])*256+b[3])!=index) { ++rejected; continue; }
                success=b[4]!=0;
            } else { if (b.size()!=3 || b[2]>1) { ++rejected; continue; } success=b[2]!=0; }
            if (!success) { abort("Negative OTA application ACK"); return false; }
            // The ACK carries an index, not the dispatched request sequence.
            // A delayed ACK during a sweep cannot identify the expected next
            // sequence. The next block can sweep too if needed.
            if (recovering_) ota_trace("data recovery ACK block="+std::to_string(index));
            sequence_=(sequence_+1)&63; s1_=3;
            if (expected==1) { index=1; next_=State::Data; }
            else if (expected==2) { ++acked_blocks; next_=index==blocks()?State::Finish:State::Data;
                if (next_==State::Data) ++index;
            } else next_=State::Complete;
            // service38 owns retained ACK retirement. OTA39 removed the data
            // drain that stalled OTA38 at block 549. Keep start/final barriers.
            if (expected==2) { s1_=2; operation(next_); }
            else operation(State::Drain);
            return true;
        }
        if (state==State::Data && request_ && tag==uint16_t(generation_)) publication(false);
        return false;
    }
};
} // namespace pico
