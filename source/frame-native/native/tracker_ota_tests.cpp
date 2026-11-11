#include "native_test_runtime.hpp"
#include "tracker_ota_fake.hpp"
#include <iostream>
using namespace pico;
namespace {
unsigned checks=0;
void check(bool b) {++checks;if(!b)throw std::runtime_error("OTA check "+std::to_string(checks));}
template<class F> void rejects(F f) {bool rejected=false;try{f();}catch(const std::exception&){rejected=true;}check(rejected);}
}
int main(int argc,char **argv) {
    try {
        if(argc!=2 && argc!=5)throw std::runtime_error("Supply temporary allowlist [ALPHA11 ROLLBACK RELEASE_ALLOWLIST]");
        const std::string allow=argv[1];
        const Bytes chip{1,2,3,4},nonce(16,7);
        const auto proof=fake_service_proof(nonce,chip);
        check(rejected_release_hash("a21d01f5f34e00d9ef6ce17cc43c64692aa39c79c110292a19ff0448c01b4ad0"));
        if(argc==5) {
            const char* hashes[]={"47d5aa2167ad574922259ee6c9ae19e3d227a54981203400c08a7d5fb0853a4a",
                "040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d"};
            for(unsigned i=0;i<2;++i) {
                std::ifstream in(argv[2+i],std::ios::binary);check(bool(in));
                Bytes image((std::istreambuf_iterator<char>(in)),{});
                check(phase5::sha256(std::string(image.begin(),image.end()))==hashes[i]);
                for(uint8_t revision:{uint8_t(7),uint8_t(12)}) {
                    TrackerOta sender(image,argv[4],[]{return 0.;});auto service=proof;service[23]=revision;
                    sender.begin(service,nonce,chip,0);run_fake_ota(sender,{});
                    check(sender.state==TrackerOta::State::Complete);
                }
            }
            std::cout<<"Alpha11 + diagnostic10 rollback OTA images PASS (fake transport, schema-2 rev7/rev12)\n";
        } else std::cout<<"NOT RUN: Alpha11 + diagnostic10 image OTA tests (supply image paths)\n";
        const auto ack=[](const Bytes &b){Bytes out{0,1,uint8_t(b.size())};
            out.insert(out.end(),b.begin(),b.end());unsigned sum=0;for(auto v:out)sum+=v;
            out.push_back(uint8_t(sum));return out;};
        check(rejected_release_hash("19dee5554df22cebbcf11f66924250072059a6cc772d08ad7239e9665ca62baf"));
        check(rejected_release_hash("e2bab6d21dbaf10ddc96d74ffd481722f65929438aa8487023614ee7824f2991"));
        check(!rejected_release_hash("c5a7860c6f7ec94518b9f546f6e51b26dfa3c3792ff7170ff2b6280c20ada064"));
        check(ota_crc(Bytes{'1','2','3','4','5','6','7','8','9'})==0xa1);
        for(size_t size : {size_t(24),size_t(25),size_t(48),size_t(99208)}) {
            Bytes image(size); for(size_t i=0;i<size;++i)image[i]=uint8_t(i*17);
            std::ofstream(allow)<<phase5::sha256(std::string(image.begin(),image.end()))<<'\n';
            double t=0;
            TrackerOta sender(image,allow,[&]{return t;});
            rejects([&]{sender.begin(proof,Bytes(16),chip,0);});
            auto bad=proof;bad[22]=1;
            rejects([&]{sender.begin(bad,nonce,chip,0);});
            rejects([&]{sender.begin(proof,nonce,chip,4);});
            rejects([&]{sender.begin(proof,nonce,Bytes{4,3,2,1},0);});
            sender.begin(proof,nonce,chip,0);
            rejects([&]{sender.begin(proof,nonce,chip,0);});
            auto box=sender.mailbox();
            check(!sender.receive({0},false,3,uint16_t(box.generation)));
            check(!sender.receive({0},true,1,uint16_t(box.generation)));
            check(!sender.receive({0},true,3,uint16_t(box.generation+2)));
            check(sender.state==TrackerOta::State::Prepare);
            run_fake_ota(sender,{});
            check(sender.state==TrackerOta::State::Complete && sender.acked_blocks==size/24+1);
            auto changed=image;changed[0]^=1;
            rejects([&]{admit_ota(changed,allow);});
            TrackerOta timeout(image,allow,[&]{return t;});
            timeout.begin(proof,nonce,chip,0);t=31;timeout.tick();
            check(timeout.state==TrackerOta::State::Failed && timeout.mailbox().payload==Bytes{0});
            TrackerOta exhausted(image,allow,[&]{return t;},65532);
            exhausted.begin(proof,nonce,chip,0);
            check(exhausted.state==TrackerOta::State::Failed);
            TrackerOta aborted(image,allow,[&]{return t;});
            aborted.begin(proof,nonce,chip,0);aborted.abort("user abort");
            check(aborted.mailbox().payload==Bytes{0});
            TrackerOta nak(image,allow,[&]{return t;});
            nak.begin(proof,nonce,chip,0);
            auto prepare=nak.mailbox();
            check(nak.receive({0},true,3,uint16_t(prepare.generation)));
            const auto start=nak.mailbox();
            check(!nak.receive({0},true,3,uint16_t(start.generation)));
            check(nak.state==TrackerOta::State::Start);
            check(!nak.receive(ack({0x25,1,1}),true,3,uint16_t(prepare.generation)));
            check(!nak.receive(ack({0x25,1,0}),true,3,uint16_t(start.generation)));
            check(nak.state==TrackerOta::State::Failed);
            TrackerOta poll(image,allow,[&]{return t;});
            poll.begin(proof,nonce,chip,0);auto m=poll.mailbox();
            poll.receive({0},true,3,uint16_t(m.generation));m=poll.mailbox();
            poll.receive(ack({0x25,1,1}),true,3,uint16_t(m.generation));m=poll.mailbox();
            check(poll.state==TrackerOta::State::Drain && m.payload==Bytes{0} && m.s1==3);
            check(!poll.receive(ack({0x25,1,1}),true,3,uint16_t(m.generation)));
            poll.receive({0},true,3,uint16_t(m.generation));m=poll.mailbox();
            check(poll.state==TrackerOta::State::Data);
            check(!poll.receive({0},true,3,uint16_t(m.generation)));
            const auto response_poll=poll.mailbox();
            check(response_poll.payload==Bytes{0} && response_poll.s1==2 && response_poll.generation!=m.generation);
            t+=.3;poll.tick();
            const auto retry=poll.mailbox();
            check(retry.payload==m.payload && retry.generation!=response_poll.generation);
            check(!poll.receive(ack({0x25,2,0,2,1}),true,3,uint16_t(retry.generation)));
            check(poll.acked_blocks==0);
            // Resume the same indexed operation after transient loss. Tag-zero
            // delivery must not retire the tracker ACK or advance the sender.
            FakeOtaTracker retained;
            TrackerOta flow(image,allow,[&]{return t;});flow.begin(proof,nonce,chip,0);
            auto exchange=[&] {auto box=flow.mailbox();return flow.receive(retained.respond(box),true,3,uint16_t(box.generation));};
            exchange();exchange();exchange(); // prepare, start, start drain
            auto data=flow.mailbox();
            flow.receive(retained.respond(data),true,3,uint16_t(data.generation));
            auto poll_box=flow.mailbox();
            const auto fallback=retained.respond({0,2,{0}});
            check(!flow.receive(fallback,true,3,0) && flow.acked_blocks==0);
            check(flow.receive(retained.respond(poll_box),true,3,uint16_t(poll_box.generation)));
            check(flow.acked_blocks==1 && flow.state==TrackerOta::State::Data);
            auto next=flow.mailbox();
            check(!flow.receive(fallback,true,3,uint16_t(next.generation))); // old index, current tag
        }
        // FW189 expected-sequence gate plus same-index duplicate ACK counter.
        // First loss is at wire block 0x093f, as in the 21:02 capture.
        for (bool accepted : {false,true}) {
            Bytes image(100800,0x5a);
            std::ofstream(allow)<<phase5::sha256(std::string(image.begin(),image.end()))<<'\n';
            double clock=0; TrackerOta flow(image,allow,[&]{return clock;},0,1);
            flow.begin(proof,nonce,chip,0);
            uint8_t expected=1; unsigned last=0, duplicates=0, writes=0, starts=0;
            bool lost=false; Bytes bytes;
            for(unsigned step=0;step<20000 && !flow.terminal();++step) {
                flow.tick(); auto m=flow.mailbox(); Bytes reply{0};
                if(m.payload!=Bytes{0}) {
                    const auto c=decode_records(m.payload).at(0); const auto &b=c.body;
                    if(c.sequence()==expected) {
                        if(b[1]==1) {++starts;reply=ack({0x25,1,1});}
                        else if(b[1]==2) {
                            const unsigned idx=unsigned(b[2])*256+b[3];
                            if(idx==0x093f && !lost) {
                                check(c.sequence()==0);lost=true;
                                if(!accepted) {clock+=.02;flow.receive(reply,true,3,uint16_t(m.generation));continue;}
                            }
                            if(idx!=last) {
                                check(idx==last+1);last=idx;++writes;duplicates=0;
                                bytes.insert(bytes.end(),b.begin()+5,b.begin()+5+b[4]);
                                if(idx!=0x093f)reply=ack({0x25,2,b[2],b[3],1});
                            } else if(++duplicates>=3) reply=ack({0x25,2,b[2],b[3],1,0xaa});
                        } else {check(bytes==image);reply=ack({0x25,3,1});}
                        expected=(expected+1)&63;
                    }
                }
                flow.receive(reply,true,3,uint16_t(m.generation));clock+=.02;
            }
            check(lost && flow.state==TrackerOta::State::Complete);
            check(starts==1 && writes==flow.blocks() && bytes==image);
        }        std::cout<<checks<<" OTA checks PASS (fake TCL and fake tracker)\n";
        return 0;
    }catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}
}
