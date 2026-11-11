#pragma once
#include "device_config.hpp"
#include "stock_ota_admission.hpp"
#include "radio_loop.hpp"
#include "image_admission.hpp"
#include "ota_trace.hpp"
#include <iostream>
namespace pico {
inline void require_ota_service_state(const Bytes &state) {
    if(state.size()!=roy_data_size || word(state,36)!=900 || word(state,40)!=4727 ||
       word(state,1888)!=1 || word(state,1880)!=0 || word(state,1884)!=0)
        throw std::runtime_error("OTA service38 requires 4727-us polls, 900-us slots, K1, no phase correction");
}
// Same-session qualification only. 64 MHz DWT stamps, unwrapped by differences.
// Occupancy rejects the measured ~50% Frame beat on the stock service schedule.
class StockCadence {
    std::optional<uint32_t> last_;
    double elapsed_=0, stock_error_=0, frame_error_=0, fit_xy_=0, fit_xx_=0;
    unsigned replies_=0;
  public:
    void observe(uint32_t cycles) {
        if(last_) {
            const double us=uint32_t(cycles-*last_)/64.0;
            if(us<=0 || us>1000000) { last_.reset(); elapsed_=stock_error_=frame_error_=fit_xy_=fit_xx_=0;replies_=0;return; }
            elapsed_+=us;
            const double slots=std::round(us/4727.5);
            fit_xy_+=slots*us;fit_xx_+=slots*slots;
            stock_error_+=std::pow(us-std::round(us/4727.5)*4727.5,2);
            frame_error_+=std::pow(us-std::round(us/3000)*3000,2);
            ++replies_;
        }
        last_=cycles;
    }
    bool qualified() const {
        if(elapsed_<5000000 || replies_<256)return false;
        const double occupancy=replies_/(elapsed_/4727.5);
        return fit_xx_>0 && std::abs(fit_xy_/fit_xx_-4727.5)<=3 &&
            occupancy>=.65 && occupancy<=1.05 &&
            std::sqrt(stock_error_/replies_)<120 && std::sqrt(frame_error_/replies_)>300;
    }
    std::string summary() const {
        return "elapsed_us="+std::to_string(elapsed_)+" replies="+std::to_string(replies_)+
            " occupancy="+std::to_string(elapsed_>0?replies_/(elapsed_/4727.5):0)+
            " fitted_us="+std::to_string(fit_xx_>0?fit_xy_/fit_xx_:0)+
            " stock_rms_us="+std::to_string(replies_?std::sqrt(stock_error_/replies_):0)+
            " frame_rms_us="+std::to_string(replies_?std::sqrt(frame_error_/replies_):0);
    }
};
class OtaRadioAdapter {
    DebugMemory &debug_;
    MailboxWriter writer_;
    TrackerOta &sender_;
    Bytes chip_, nonce_;
    std::function<double()> now_;
    double started_, query_started_=0;
    bool terminal_published_=false, began_=false, stock_=false, native_seen_=false, published_=false, delivered_=false;
    unsigned queries_=0;
    uint32_t generation_=2;
    uint8_t sequence_=0;
    Mailbox query_{2,2,{0}};
    std::optional<double> product_at_;
    std::string serial_;
    OtaNativeHistory *history_=nullptr;
    std::function<Bytes()> fresh_nonce_;
    std::set<Bytes> nonces_;
    StockCadence cadence_;
    bool ack_notification_=false;
    std::map<uint16_t,uint8_t> admission_tags_;
    std::set<uint16_t> query_tags_;
    void next_query() {
        if(queries_) nonce_=fresh_nonce_();
        if(nonce_.size()!=16 || !nonces_.insert(nonce_).second)
            throw std::runtime_error("OTA requires distinct fresh nonces");
        ++queries_; ++generation_; ++generation_;
        // FW189 954e..9556 requires the expected sequence, initially zero.
        // Increment only for a later query; skipping zero prevents dispatch.
        sequence_=uint8_t((queries_-1)&63);
        Bytes request{0x7e,2};request.insert(request.end(),nonce_.begin(),nonce_.end());
        query_={generation_,uint8_t(ack_notification_?3:2),TrackerOta::record(sequence_,request)};
        query_tags_.clear();
        query_started_=now_();published_=delivered_=false;
        ota_trace("query generation="+std::to_string(generation_)+" sequence="+std::to_string(sequence_)+" nonce="+ota_trace_hex(nonce_));
    }
    void begin_stock() {
        ota_trace("stock admission qualified "+cadence_.summary());
        sender_.generation_=generation_;sender_.sequence_=uint8_t((sequence_+1)&63);
        sender_.start_transfer();began_=!sender_.terminal();
    }
  public:
    static constexpr const char *module_sha =
        "44f4e52b07bbba4c459256c910228d440e25ff0d288238e76047fd21000c4c7c";
    // The fixed hash is the old-firmware build; on any other Roy firmware the module is the same source
    // rebuilt for the located addresses and named by the locator report (ROY_LAYOUT_REPORT).
    static bool ota_module_admitted(const Bytes &module) {
        const auto sha=phase5::sha256(std::string(module.begin(),module.end()));
        if(sha==module_sha)return true;
        const auto report=roy_layout_report();
        if(report.ok && sha==report.module_sha){std::cerr<<"OTA module admitted by Roy locator report "<<sha<<'\n';return true;}
        return false;
    }
    OtaRadioAdapter(DebugMemory &debug, TrackerOta &sender, Bytes chip, Bytes nonce,
                    std::function<double()> now, const Bytes &module,
                    const StockOtaApproval *approval=nullptr, OtaNativeHistory *history=nullptr,
                    std::function<Bytes()> fresh_nonce={},
                    std::string serial=pico::devices::serial())
        : debug_(debug), writer_(debug), sender_(sender), chip_(std::move(chip)),
          nonce_(std::move(nonce)), now_(std::move(now)), started_(now_()), serial_(std::move(serial)) {
        if (!ota_module_admitted(module))
            throw std::runtime_error("OTA requires the service38 module or its locator-built relocation");
        const auto parsed=elf_module(module);
        if(debug_.read(roy_code,parsed.code.size())!=parsed.code)
            throw std::runtime_error("Live station code differs from service38");
        if(chip_.size()!=4 || nonce_.size()!=16 || serial_.size()!=17)
            throw std::runtime_error("Invalid OTA identity");
        history_=history;fresh_nonce_=std::move(fresh_nonce);
        if(!fresh_nonce_)throw std::runtime_error("Fresh nonce source required");
        if(approval) {
            if(!approval->matches(chip_,sender_.image_sha()) || approval->serial()!=serial_ || !history_)
                throw std::runtime_error("Invalid stock OTA approval");
            if(history_->known(chip_)) throw std::runtime_error("Native tracker requires Service proof");
            stock_=true;
        }
        // Neutral product exchange first, as OTA39. No configuration or 7D.
    }
    void abort(const std::string &why) {ota_trace("abort "+why);sender_.abort(why);publish();}
    void publish() {
        if(!began_ && !sender_.terminal()) {
            if(now_()-started_>=35) sender_.abort("Admission deadline; no OTA start");
            else if(product_at_ && published_ && now_()-query_started_>=8) {
                if(stock_ && !native_seen_ && !delivered_) sender_.abort("Query delivery unproven; no OTA start");
                else if(queries_<3) next_query();
                else if(stock_ && !native_seen_ && cadence_.qualified() && now_()-*product_at_<=30) begin_stock();
                else {ota_trace("stock admission rejected "+cadence_.summary());
                    sender_.abort("Fresh Service proof or stock cadence unavailable; no OTA start");}
            }
        }
        sender_.tick();
        Mailbox box=query_;
        if(!began_ && !sender_.terminal()) {
            const uint8_t s1=ack_notification_?3:2;
            if(query_.s1!=s1) {
                generation_+=2;query_.generation=generation_;query_.s1=s1;
            }
            box=query_;
            admission_tags_[uint16_t(box.generation)]=box.s1;
            query_tags_.insert(uint16_t(box.generation));
        }
        if(began_)box=sender_.mailbox();
        if(sender_.terminal()) {
            // Terminal publication must follow every OTA generation, once only.
            if(!terminal_published_) {
                ota_trace("terminal state="+std::to_string(int(sender_.state))+" error="+sender_.error);
                generation_=std::max(generation_,sender_.generation_)+2;
                query_={generation_,2,{0}};terminal_published_=true;
            }
            box=query_;
        }
        writer_.publish(0,box.generation,box.s1,box.payload);
        published_=true;
    }
    void receive(const Record &r) {
        if(!r.hardware_crc || r.partial || r.channel!=2 || r.profile!=0 || r.chip!=word(chip_) ||
           r.dma.size()<3 || r.dma.size()!=size_t(r.dma[0])+2 || (r.dma[1]!=2 && r.dma[1]!=3)) {
            ota_trace("ignore adapter envelope crc/partial/channel/profile/chip/length/s1");return;
        }
        if(sender_.terminal()){ota_trace("ignore terminal");return;}
        const Bytes payload(r.dma.begin()+2,r.dma.end());
        std::vector<Command> records;
        try {records=decode_records(payload);}catch(...){ota_trace("ignore adapter decode");return;}
        if(began_) {sender_.receive(payload,true,r.dma[1],uint16_t(r.generation));return;}
        // S1 is a queue acknowledgement, not an echo. Match Python PeerEngine.
        const auto sent=admission_tags_.find(uint16_t(r.generation));
        if(sent!=admission_tags_.end() && (sent->second&1))ack_notification_=false;
        for(const auto &c:records)if(c.command!=2 && c.command!=10 && c.command!=11)ack_notification_=true;
        if(stock_)cadence_.observe(r.cycles);
        bool fe=false;
        for(const auto &c:records) if(c.command==1 && !c.body.empty() && c.body[0]==0xfe) {
            fe=native_seen_=true;
            if(history_)history_->remember(chip_);
        }
        if(!published_ || !query_tags_.count(uint16_t(r.generation))) {
            ota_trace("ignore unpublished/unmatched query generation="+std::to_string(r.generation));
            if(fe && stock_)sender_.abort("Unmatched FE requires fresh normal Service proof");
            return;
        }
        if(!product_at_) {
            for(const auto &c:records)if(c.command==1 && c.body.size()==35 && c.body[0]==0x82) {
                const auto &b=c.body;
                if((unsigned(b[1])|(unsigned(b[2])<<8))!=189 || std::string(b.begin()+8,b.begin()+25)!=serial_) {
                    ota_trace("reject product FW/serial");sender_.abort("Live product serial/FW189 differs from approval");return;
                }
                ota_trace("accept product; queue acknowledgement required");product_at_=now_();next_query();return;
            }
            ota_trace("ignore awaiting product-info");return;
        }
        delivered_=true; // canonical response attributed to published query slot
        for(const auto &c:records) {
            const auto &b=c.body;
            if(c.command!=1 || b.size()!=26 || b[0]!=0xfe || b[1]!=2 ||
               !std::equal(nonce_.begin(),nonce_.end(),b.begin()+2) ||
               !std::equal(chip_.begin(),chip_.end(),b.begin()+18)) {
                ota_trace("ignore command/schema/length/nonce/chip mismatch");continue;
            }
            // Any FE removes stock admission. A fresh matched FE02 uses normal rules.
            stock_=false;
            sender_.generation_=generation_;sender_.sequence_=uint8_t((sequence_+1)&63);
            try {sender_.begin(b,nonce_,chip_,now_()-query_started_);began_=!sender_.terminal();ota_trace("matched FE02 admission="+std::to_string(began_)+" error="+sender_.error);}
            catch(const std::exception &e){ota_trace(std::string("reject FE02 ")+e.what());sender_.abort(e.what());}
            return;
        }
        if(fe)sender_.abort("FE response requires fresh normal Service mode 0 proof");
        ota_trace(fe?"reject unmatched FE":"ignore no FE02 proof");
    }
    bool began() const {return began_;}
};
} // namespace pico
