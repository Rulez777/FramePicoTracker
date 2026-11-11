#pragma once
#include "tracker_ota_fake.hpp"
#include "ota_service_state.hpp"
#include <filesystem>
#include <iostream>
namespace pico {
// Offline only: independent FW189 expected-sequence dispatcher and flash sink.
// No socket, child, observer, physical clock, READY answer or owner file.
struct StockFakeResult { unsigned starts=0,writes=0,queries=0,duplicates=0; bool lost=false,finished=false; };
inline StockFakeResult run_stock_ota_fake(const Bytes &image,const std::string &allow,
    const Bytes &module,const Bytes &chip,const std::string &serial,const std::string &scratch,
    bool inject_fe=false,bool bad_product=false,bool drop_query=false,
    bool bad_serial=false,unsigned product_delay=0) {
    const auto state=make_ota_service_state(nullptr,chip);
    require_ota_target_state(state,chip);
    const auto sha=phase5::sha256(std::string(image.begin(),image.end()));
    // These files are explicitly synthetic fixtures, never live approvals.
    std::filesystem::create_directories(scratch);
    const std::string ap=scratch+"/synthetic-approval.fixture",hp=scratch+"/synthetic-history.fixture";
    const std::vector<std::string> av{"fake-only","--stock-approval",ap,"--chip",ota_hex(chip),"--serial",serial};
    {std::ofstream out(ap,std::ios::binary);out<<"fw189-ota-approval-v2\nunix=1700000000\nchip="<<ota_hex(chip)
        <<"\nserial="<<serial<<"\nfirmware=189\nsha256="<<sha<<'\n';
     for(const auto &a:av)out<<"argv="<<ota_hex(Bytes(a.begin(),a.end()))<<'\n';}
    StockOtaApproval approval(ap,chip,sha,av,serial,1700000001);
    std::ofstream(hp,std::ios::trunc).close();OtaNativeHistory history(hp);
    FakeOpenOcd fake;DebugMemory debug([&](const std::string &s){return fake.command(s);});
    fake.put(roy_code,elf_module(module).code);
    double clock=0;TrackerOta sender(image,allow,[&]{return clock;});uint8_t nonce=1;
    OtaRadioAdapter adapter(debug,sender,chip,Bytes(16,nonce),[&]{return clock;},module,
        &approval,&history,[&]{return Bytes(16,++nonce);},serial);
    StockFakeResult result;uint8_t expected=0;Bytes retained,bytes;unsigned last=0;bool retired=false;
    auto rec=[](const Bytes &body) {Bytes r{0,1,uint8_t(body.size())};r.insert(r.end(),body.begin(),body.end());
        unsigned sum=0;for(auto b:r)sum+=b;
        r.push_back(uint8_t(sum));return r;};
    Bytes product(35);product[0]=0x82;product[1]=bad_product?188:189;
    std::string product_serial=serial;
    if(bad_serial)product_serial.back()='H';
    std::copy(product_serial.begin(),product_serial.end(),product.begin()+8);retained=rec(product);
    for(unsigned step=0;step<100000 && !sender.terminal();++step) {
        adapter.publish();
        if(step<product_delay){clock+=.004727;continue;}
        const auto h=fake.read(roy_data+1664,8);const auto meta=word(h,4);
        Mailbox m{word(h),uint8_t(meta>>8),fake.read(roy_data+1672,meta&255)};
        Bytes reply{0};
        // Queue ACK retires product/notifications before query processing.
        if(m.s1&1){retained.clear();retired=true;}
        if(!retained.empty())reply=retained;
        const auto cmds=decode_records(m.payload);
        if(!cmds.empty() && cmds[0].sequence()==expected) {
            const auto &b=cmds[0].body;
            if(drop_query && b[0]==0x7e && expected==0) {
                // A response whose request did not dispatch is deliberately
                // outside stock assumptions; exercise no destructive start.
                clock+=.004727;continue;
            }
            expected=(expected+1)&63;
            if(b[0]==0x7e) {
                if(!retired)throw std::runtime_error("Query sent before S1=3 product ACK");
                ++result.queries; // Stock has no handler/reply for 7E.
                if(inject_fe)reply=rec(Bytes{0xfe,1});
            } else if(b[0]==0x25) {
                if(result.queries!=3)throw std::runtime_error("Start without three dispatched stock queries");
                if(b[1]==1) {
                    ++result.starts;
                    const size_t n=(size_t(b[2])<<24)|(size_t(b[3])<<16)|(size_t(b[4])<<8)|b[5];
                    if(b.size()!=9 || n!=image.size() || b[6]!=ota_crc(image) || b[7]!=0 || b[8]!=189)
                        throw std::runtime_error("Bad stock OTA start");
                    reply=rec({0x25,1,1});
                } else if(b[1]==2) {
                    const unsigned index=unsigned(b[2])*256+b[3];
                    if(index==last+1) {
                        ++result.writes;last=index;
                        if(b.size()!=29 || b[4]!=std::min(size_t(24),image.size()-bytes.size()))
                            throw std::runtime_error("Bad stock block length");
                        bytes.insert(bytes.end(),b.begin()+5,b.begin()+5+b[4]);
                        for(size_t i=5+b[4];i<b.size();++i)if(b[i]!=255)throw std::runtime_error("Bad padding");
                        if(index==7 && !result.lost){result.lost=true;reply={0};}
                        else reply=rec({0x25,2,b[2],b[3],1});
                    } else if(index==last) {
                        ++result.duplicates;
                        // FW189 duplicate counter: requeue after three dispatches.
                        if(result.duplicates>=3)reply=rec({0x25,2,b[2],b[3],1,0xaa});
                    } else throw std::runtime_error("Stock index skipped");
                } else if(b[1]==3) {
                    if(bytes!=image || last!=sender.blocks())throw std::runtime_error("Stock image mismatch");
                    result.finished=true;reply=rec({0x25,3,1});
                } else throw std::runtime_error("Unexpected OTA subcommand");
            } else throw std::runtime_error("Unexpected stock command");
        }
        // 75% occupancy at the actual configured integer poll 4727 us.
        if(step%4!=0) {
            if(retired)retained.clear(); // service38 retires captured application ACKs
            Record r;r.hardware_crc=true;r.channel=2;r.profile=0;r.chip=word(chip);
            r.generation=m.generation;r.cycles=uint32_t(uint64_t(step)*4727*64);
            r.dma={uint8_t(reply.size()),3};r.dma.insert(r.dma.end(),reply.begin(),reply.end());
            // Loss occurs after application dispatch/capture, not before write.
            // Product and application ACKs retained for the next captured slot.
            adapter.receive(r);
        } else if(reply!=Bytes{0})retained=reply;
        clock+=.004727;
    }
    OtaNativeHistory reloaded(hp);
    if(inject_fe) {
        if(!sender.terminal() || result.starts || !reloaded.known(chip))throw std::runtime_error("FE history regression");
    } else if(bad_product || drop_query || bad_serial || product_delay>2400) {
        if(!sender.terminal() || result.starts || reloaded.known(chip))throw std::runtime_error("Stock refusal regression");
    } else {
        if(sender.state!=TrackerOta::State::Complete || !result.finished || result.starts!=1 ||
           result.writes!=sender.blocks() || !result.lost || result.duplicates<3 || reloaded.known(chip))
            throw std::runtime_error("Stock fake failed: "+sender.error);
        std::cout<<"stock fake wire="<<ota_hex(chip)<<" serial="<<serial<<" blocks="<<sender.acked_blocks
            <<" queries="<<result.queries<<" starts="<<result.starts<<" lost_ACK_recovered=1 history_native=0"
            <<" finish_verified fake-only activation_verified=false\n";
    }
    return result;
}
}
