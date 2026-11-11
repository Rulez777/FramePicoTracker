#include "native_test_runtime.hpp"
#include "tracker_ota_fake.hpp"
#include "tracker_ota_live.hpp"
#include <iostream>
#include <filesystem>
using namespace pico;
namespace {
unsigned checks=0;
void check(bool b) {++checks;if(!b)throw std::runtime_error("OTA radio check "+std::to_string(checks));}
template<class F> void rejects(F f) {bool bad=false;try{f();}catch(const std::exception&){bad=true;}check(bad);}
Bytes record(const Bytes &body) {
    Bytes p{0,1,uint8_t(body.size())};p.insert(p.end(),body.begin(),body.end());
    unsigned sum=0;for(auto b:p)sum+=b;
    p.push_back(uint8_t(sum));return p;
}
struct Fixture {
    FakeOpenOcd fake;
    DebugMemory debug;
    Bytes module,chip{1,2,3,4},nonce=Bytes(16,0x5a);
    TrackerOta sender;
    OtaRadioAdapter adapter;
    DataPlane plane;
    RadioLoop loop;
    FakeOtaTracker tracker;
    unsigned write=0, ota_starts=0;
    Bytes last_dma;
    bool wrong_mode=false, silent=false, wrong_nonce=false, stock_fake=false, bad_serial=false, bad_fw=false;
    bool retained_product=false, product_retired=false, pending=false, wrong_chip=false;
    uint8_t fresh=100; bool cadence_fixture=false; uint32_t rf_cycles=0;
    Fixture(const Bytes &image,const std::string &allow,const Bytes &elf,const StockOtaApproval *approval=nullptr,OtaNativeHistory *history=nullptr,
            Bytes target={1,2,3,4},Bytes query_nonce=Bytes(16,0x5a))
        : debug([&](const std::string &s){return fake.command(s);}),module(elf),chip(std::move(target)),nonce(std::move(query_nonce)),
          sender(image,allow,[&]{return double(fake.ns)/1e9;},4,1),
          adapter(prepare(),sender,chip,nonce,[&]{return double(fake.ns)/1e9;},module,approval,history,[&]{return Bytes(16,++fresh);}),
          plane(Hooks{}),loop(fake.io(debug),plane,{[]{},[&](const Bytes&){adapter.publish();},
              [](uint32_t,bool){},[]{},[](const IterationLatency&){}},true,false) {
        plane.chip=word(chip);plane.exclusive=true;
        plane.hooks.now=[&]{return fake.ns;};
        plane.hooks.canonical=[&](const Record &r,const std::vector<Command>&,const std::optional<AckContext>&){adapter.receive(r);};
        fake.set(roy_data,0x314d5250);
    }
    DebugMemory &prepare() {fake.put(roy_code,elf_module(module).code);return debug;}
    Mailbox box() {
        const auto head=fake.read(roy_data+1664,8);const auto meta=word(head,4);
        return {word(head),uint8_t(meta>>8),fake.read(roy_data+1672,meta&255)};
    }
    void enqueue(const Mailbox &m,const Bytes &payload) {
        Bytes slot(112);auto put=[&](size_t at,uint32_t v){auto b=le32(v);std::copy(b.begin(),b.end(),slot.begin()+at);};
        put(0,write);put(4,m.generation<<16);put(104,cadence_fixture?rf_cycles:uint32_t(fake.ns*64/1000));put(12,2);put(16,uint32_t(payload.size()+2));
        slot[20]=uint8_t(payload.size());slot[21]=3;
        std::copy(payload.begin(),payload.end(),slot.begin()+22);put(100,word(chip));
        last_dma=Bytes(slot.begin()+20,slot.begin()+22+payload.size());
        fake.put(roy_data+380+(write%10)*112,slot);fake.set(roy_data+112,++write);
    }
    void step(bool drop=false) {
        adapter.publish();const auto m=box();Bytes reply;
        const auto commands=decode_records(m.payload);
        if(!commands.empty() && commands[0].body[0]==0x7e) {
            auto n=Bytes(commands[0].body.begin()+2,commands[0].body.end());
            auto proof=fake_service_proof(n,chip);
            if(wrong_mode)proof[22]=1;
            if(wrong_nonce)proof[2]^=1;
            if(pending)proof[25]=1;
            if(wrong_chip)proof[18]^=1;
            if(retained_product) {
                // Independent phase189 FE02 framing/rev7 (not fake_service_proof rev11).
                proof=unhex("fe02d1d7d0e50150687d20daaa75f72cec9ed4c3b2a100070000");
                std::copy(n.begin(),n.end(),proof.begin()+2);
                std::copy(chip.begin(),chip.end(),proof.begin()+18);
                if(m.s1&1)product_retired=true;
            }
            reply=stock_fake ? Bytes{0} : record(proof);
            if(retained_product){reply[0]=1;++reply.back();}
            if(retained_product && !product_retired) {
                Bytes product(35);product[0]=0x82;product[1]=189;
                const std::string serial="PCTESTTRACKER0001";
                std::copy(serial.begin(),serial.end(),product.begin()+8);reply=record(product);
            }
        } else if(commands.empty() && sender.state==TrackerOta::State::Ready) {
            Bytes product(35);product[0]=0x82;product[1]=bad_fw?188:189;
            const std::string serial=bad_serial?"PCTESTTRACKER0009":"PCTESTTRACKER0001";
            std::copy(serial.begin(),serial.end(),product.begin()+8);reply=record(product);
        } else {
            if(!commands.empty() && commands[0].body[0]==0x25 && commands[0].body[1]==1)++ota_starts;
            reply=tracker.respond(m);
        }
        if(!drop && !silent)enqueue(m,reply);
        loop.step();
    }
    void finish() {
        for(unsigned i=0;i<60000 && !sender.terminal();++i)step();
        check(sender.state==TrackerOta::State::Complete && tracker.finished);
        check(sender.acked_blocks==sender.blocks() && ota_starts==1);
        check(plane.counters.crc==0 && plane.counters.malformed==0);
    }
};
}
int main(int argc,char **argv) {try {
    const bool smoke=argc==5 && std::string(argv[4])=="--admission-smoke";
    if(argc!=4 && argc!=6 && !smoke)throw std::runtime_error("usage: ota_radio_tests MODULE CANDIDATE_A ALLOWLIST [ALPHA_IMAGE ROLLBACK | --admission-smoke]");
    // Private release artifacts are optional test inputs, never synthesized.
    const auto directory=std::filesystem::absolute(argv[2]).parent_path();
    bool missing=false;
    for(const auto& path : {std::filesystem::path(argv[1]),std::filesystem::path(argv[2]),
                           directory/"frame-release-b.bin",directory/"frame-release-c.bin"}) {
        if(!std::filesystem::is_regular_file(path)) {
            std::cout<<"SKIP OTA radio artifact suite: missing "<<path.string()<<"\n";
            missing=true;
        }
    }
    if(missing) {std::cout<<"SKIP: 0 OTA radio assertions run; supply service38.elf and frame-release-{a,b,c}.bin.\n";return 0;}
    const auto module=ota_file(argv[1]),image=ota_file(argv[2]);const std::string allow=argv[3];
    if(smoke) {
        Fixture x(image,allow,module);x.retained_product=true;x.step();x.step();
        check(x.adapter.began() && x.product_retired && x.ota_starts==0);
        Fixture bad(image,allow,module);bad.wrong_mode=true;bad.step();bad.step();
        check(!bad.adapter.began() && bad.ota_starts==0);
        std::cout<<checks<<" admission trace smoke checks PASS (fake only)\n";return 0;
    }
    if(argc==6) {
        const char* rollbackHash="040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d";
        for(unsigned i=0;i<2;++i) {
            const auto extra=ota_file(argv[4+i]);
            // The image under test is admitted by TrackerOta's supplied allowlist.
            if(i==1) check(phase5::sha256(std::string(extra.begin(),extra.end()))==rollbackHash);
            Fixture x(extra,allow,module);x.finish();
        }
        std::cout<<"Alpha image + diagnostic10 rollback OTA radio images PASS (fake TCL/tracker)\n";
    } else std::cout<<"NOT RUN: Alpha image + diagnostic10 OTA radio image tests\n";
    // Both POSIX scripts run from native/, MSVC scripts from the repository root.
    const auto fixtures=std::filesystem::absolute(argv[2]).parent_path();
    auto fixture=[&](const char *name){return (fixtures/name).string();};
    check(ota_readiness_matches("READY 123","123",2));
    check(!ota_readiness_matches("READY 123","456",2));
    check(!ota_readiness_matches("READY 123","123",121));
    check(!ota_readiness_matches("yes","123",2));
    {Bytes state(2048);auto put=[&](size_t off,uint32_t v){auto b=le32(v);std::copy(b.begin(),b.end(),state.begin()+off);};
     put(36,900);put(40,4727);put(1888,1);require_ota_service_state(state);check(true);
     put(40,6000);rejects([&]{require_ota_service_state(state);});}
    check(phase5::sha256(std::string(image.begin(),image.end()))=="c5a7860c6f7ec94518b9f546f6e51b26dfa3c3792ff7170ff2b6280c20ada064");
    const auto rejected_b=ota_file(fixture("frame-release-b.bin"));
    const auto rejected_c=ota_file(fixture("frame-release-c.bin"));
    rejects([&]{admit_ota(rejected_b,allow);});
    rejects([&]{admit_ota(rejected_c,allow);});
    const std::string rejected_allow=fixture("rejected-release.fixture");
    std::ofstream(rejected_allow)<<"19dee5554df22cebbcf11f66924250072059a6cc772d08ad7239e9665ca62baf\n"
        <<"e2bab6d21dbaf10ddc96d74ffd481722f65929438aa8487023614ee7824f2991\n";
    rejects([&]{admit_ota(rejected_b,rejected_allow);});
    rejects([&]{admit_ota(rejected_c,rejected_allow);});
    // Real 17:55 wire payloads; FW189 9540..957c expects a sequence,
    // increments only after dispatch, and ignores out-of-order command 1.
    {unsigned expected=0;
     auto dispatch=[&](const Bytes &payload) {
         const auto commands=decode_records(payload);
         if(commands.empty() || commands[0].sequence()!=expected)return unhex("010300");
         expected=(expected+1)&63;
         // diagnostic10 rev7 Service/N0/pending0, with this trace nonce.
         auto reply=record(unhex("fe02c72c67d4d8b41a70a83fa8bb3508ac7bd4c3b2a100070000"));
         reply[0]=1;++reply.back(); // product occupied outbound command-1 sequence zero
         return reply;
     };
     unsigned replay_index=0;
     const unsigned repeats[]={1691,1690,1692}; // gen4+6, gen8, gen10 (exact trace counts)
     for(const auto *hex:{"0101127e02c72c67d4d8b41a70a83fa8bb3508ac7b86",
                          "0201127e0281a888ade3262726060c07a9d39e0a27ad",
                          "0301127e0244c29b7d3642f621052cd273a8ebb4e3e3"})
         { for(unsigned i=0;i<repeats[replay_index];++i)check(dispatch(unhex(hex))==unhex("010300"));
         ++replay_index; }
     // Counted runs above retain the original order of the three requests.
     check(expected==0);
     Fixture x(image,allow,module,nullptr,nullptr,unhex("d4c3b2a1"),unhex("c72c67d4d8b41a70a83fa8bb3508ac7b"));
     const auto product=unhex("270300012382bd0031323333315043323331304d4c4b43313130383635472b290401000000002000d2");
     x.adapter.publish();x.enqueue(x.box(),Bytes(product.begin()+2,product.end()));x.loop.step();
     x.adapter.publish();const auto query=x.box();
     check(query.s1==3);
     check(query.payload==unhex("0001127e02c72c67d4d8b41a70a83fa8bb3508ac7b85"));
     const auto response=dispatch(query.payload);
     check(response!=unhex("010300") && expected==1);
     x.enqueue(query,response);x.loop.step();check(x.adapter.began() && x.ota_starts==0);
     // Repeating an accepted query does not execute its handler again.
     check(dispatch(query.payload)==unhex("010300"));
    }
    {Fixture x(image,allow,module);x.finish();}
    {Fixture x(image,allow,module,nullptr,nullptr,unhex("d4c3b2a1"),unhex("d1d7d0e50150687d20daaa75f72cec9e"));
     x.retained_product=true;x.step();x.adapter.publish();
     check(x.box().s1==3);x.step();check(x.product_retired && x.adapter.began());
     check(x.last_dma==unhex("1e0301011afe02d1d7d0e50150687d20daaa75f72cec9ed4c3b2a10007000039"));x.finish();}
    {Fixture x(image,allow,module);x.pending=true;x.step();x.step();check(x.sender.terminal() && !x.adapter.began());}
    {Fixture x(image,allow,module);x.wrong_chip=true;x.step();x.step();check(!x.adapter.began() && x.ota_starts==0);}
    {Fixture x(image,allow,module);x.step();x.adapter.publish();const auto earlier=x.box();
     x.enqueue(earlier,Bytes{0});x.loop.step();x.adapter.publish();
     check(x.box().generation!=earlier.generation && x.box().s1==2);
     x.enqueue(earlier,record(fake_service_proof(x.nonce,x.chip)));x.loop.step();
     check(x.adapter.began());}
    {Fixture x(image,allow,module);x.step();x.adapter.publish();auto unpublished=x.box();unpublished.generation+=100;
     x.enqueue(unpublished,record(fake_service_proof(x.nonce,x.chip)));x.loop.step();
     check(!x.adapter.began() && x.ota_starts==0);}
    {Fixture x(image,allow,module);x.step();x.adapter.publish();Record r;
     r.hardware_crc=true;r.channel=2;r.chip=word(x.chip)^1;r.generation=x.box().generation;
     const auto payload=record(fake_service_proof(x.nonce,x.chip));r.dma={uint8_t(payload.size()),3};
     r.dma.insert(r.dma.end(),payload.begin(),payload.end());x.adapter.receive(r);
     check(!x.adapter.began() && x.ota_starts==0);}
    {Fixture x(image,allow,module);x.wrong_mode=true;x.step();x.step();check(x.sender.state==TrackerOta::State::Failed && x.ota_starts==0);x.adapter.publish();check(x.box().payload==Bytes{0});}
    {Fixture x(image,allow,module);x.wrong_nonce=true;for(unsigned i=0;i<4000 && !x.sender.terminal();++i){x.fake.ns+=10000000;x.step();}check(x.sender.state==TrackerOta::State::Failed && x.ota_starts==0);}
    {auto bad=image;bad[0]^=1;rejects([&]{Fixture x(bad,allow,module);});}
    {auto bad=module;bad[0]^=1;rejects([&]{Fixture x(image,allow,bad);});}
    {Fixture x(image,allow,module);x.fake.memory[roy_code]^=1;
     rejects([&]{OtaRadioAdapter bad(x.debug,x.sender,x.chip,x.nonce,[]{return 0.;},module);});}
    {Fixture x(image,allow,module);
     for(unsigned i=0;i<40;++i)x.step();
     check(x.sender.acked_blocks>0);x.adapter.abort("operator abort");
     const auto m=x.box();check(m.payload==Bytes{0} && x.sender.state==TrackerOta::State::Failed);
     const auto terminal=x.box();x.adapter.publish();check(x.box().generation==terminal.generation);
     Fixture fresh(image,allow,module);fresh.finish();
    }    {Fixture x(image,allow,module);
     while(x.sender.state!=TrackerOta::State::Data)x.step();
     x.step(); // telemetry; sender switches to a short response poll
     const auto before=x.sender.acked_blocks;
     x.step(true); // retained indexed ACK lost after module capture
     x.fake.ns+=300000000;x.sender.tick();x.step(); // duplicate block request returns cached ACK
     check(x.sender.acked_blocks==before+1 && x.sender.retries>0);x.finish();}
    {Fixture x(image,allow,module);x.bad_serial=true;x.step();check(x.sender.terminal() && !x.adapter.began());}
    {Fixture x(image,allow,module);x.bad_fw=true;x.step();check(x.sender.terminal() && !x.adapter.began());}
    {Fixture x(image,allow,module);x.step();x.adapter.publish();const auto m=x.box();
     check(decode_records(m.payload)[0].sequence()==0);
     x.stock_fake=true;x.step(); // delivery but no FE
     x.fake.ns+=8000000001LL;x.adapter.publish();
     check(decode_records(x.box().payload)[0].sequence()==1);x.stock_fake=false;x.step();
     check(x.adapter.began());x.finish();}
    const std::vector<std::string> av{"tool","--stock-approval","fixture"};
    const Bytes target{1,2,3,4};
    const auto sha=phase5::sha256(std::string(image.begin(),image.end()));
    const std::string approval_path=fixture("stock-approval.fixture");
    auto approval_text=[&](int64_t stamp) {
        std::string text="fw189-ota-approval-v2\nunix="+std::to_string(stamp)+
            "\nchip="+ota_hex(target)+"\nserial=PCTESTTRACKER0001\nfirmware=189\nsha256="+sha+"\n";
        for(const auto &a:av)text+="argv="+ota_hex(Bytes(a.begin(),a.end()))+"\n";
        return text;
    };
    rejects([&]{StockOtaApproval a(fixture("no-such-owner-approval"),target,sha,av);});
    const auto unix_now=std::time(nullptr);
    for(auto stamp:{unix_now-121,unix_now+1}) {
        std::ofstream(approval_path,std::ios::binary)<<approval_text(stamp);
        rejects([&]{StockOtaApproval a(approval_path,target,sha,av);});
    }
    std::ofstream(approval_path,std::ios::binary)<<approval_text(unix_now);
    rejects([&]{StockOtaApproval a(approval_path,target,sha,{"different argv"});});
    rejects([&]{StockOtaApproval a(approval_path,Bytes{4,3,2,1},sha,av);});
    rejects([&]{StockOtaApproval a(approval_path,target,sha,av,"PCTESTTRACKER0009");});
    StockOtaApproval approval(approval_path,target,sha,av);
    const std::string history_path=fixture("native-history.fixture");
    for(unsigned scenario=0;scenario<8;++scenario) {
        std::ofstream(history_path,std::ios::trunc).close();OtaNativeHistory history(history_path);
        Fixture x(image,allow,module,&approval,&history);x.stock_fake=true;x.cadence_fixture=true;
        x.step(); // matched product
        for(unsigned query=0;query<6 && !x.sender.terminal() && !x.adapter.began();++query) {
            // 8 seconds of replies, with one slot missing in four (75% occupancy).
            for(unsigned j=0;j<1700 && !x.sender.terminal();++j) {
                x.fake.ns+=scenario==4?3000000:scenario==5?9455000:4727500;
                x.rf_cycles+=scenario==4?192000:scenario==5?605120:302560;
                if(scenario!=1 && j%4!=0) {
                    if(scenario==2)x.wrong_mode=true,x.stock_fake=false;
                    if(scenario==3)x.wrong_nonce=true,x.stock_fake=false;
                    if(scenario==6)x.stock_fake=false; // FE mode 0 takes the normal rule
                    if(scenario==7)x.wrong_nonce=true,x.stock_fake=false;
                    x.step();
                } else x.adapter.publish();
            }
            x.adapter.publish();
        }
        if(scenario==0 || scenario==6) {
            check(x.adapter.began());x.finish();
        } else check(!x.adapter.began() && x.sender.terminal() && x.ota_starts==0);
        if(scenario==2 || scenario==3 || scenario==6 || scenario==7) {
            OtaNativeHistory reloaded(history_path);check(reloaded.known(target));
            rejects([&]{Fixture denied(image,allow,module,&approval,&reloaded);});
        }
    }
    // Cadence negatives: short evidence, Frame 3000, stock-poll 50% beat,
    // sparse and duplicate stamps cannot qualify.
    for(unsigned scenario=0;scenario<5;++scenario) {
        StockCadence cadence;uint32_t cycles=0;
        const unsigned n=scenario==0?500:2000;
        for(unsigned i=0;i<n;++i) {
            cadence.observe(cycles);
            cycles+=scenario==1?192000:scenario==2?605120:scenario==3?3025600:scenario==4?0:302560;
        }
        check(!cadence.qualified());
    }
    {StockCadence c;uint32_t stamp=0xffff0000;
     for(unsigned i=0;i<1600;++i){c.observe(stamp);stamp+=302560;}
     check(c.qualified());}
    std::cout<<checks<<" OTA radio checks PASS (candidate A, real RadioLoop, fake Tcl/tracker)\n";
    return 0;
} catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}}
