#include "native_test_runtime.hpp"
#include "tcl_transport.hpp"
#include "imu_slot.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <thread>
#include <exception>
#ifdef _WIN32
#include <winsock2.h>
using Sock=SOCKET;
static void shut(Sock s){closesocket(s);}
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
using Sock=int;
static void shut(Sock s){close(s);}
#endif
using namespace pico;
static int checks=0;
static void check(bool b){if(!b)throw std::runtime_error("assertion "+std::to_string(checks));
++checks;}
static uint32_t w32(const Bytes& b,size_t o=0){return uint32_t(b[o])|uint32_t(b[o+1])<<8|uint32_t(b[o+2])<<16|uint32_t(b[o+3])<<24;}
template<class F>static void rejects(F f,const std::string& reason){try{f();}catch(const std::exception& e){check(std::string(e.what())==reason);return;}throw std::runtime_error("missing rejection");}
// Fake TCL socket server: exact command -> memory-word fixture. All requests retained.
struct Fake {
    Sock listener;uint16_t port;std::thread worker;std::exception_ptr error;
    std::vector<std::string> recorded;
    std::vector<std::pair<std::string,std::string>> fixtures;
    explicit Fake(std::vector<std::pair<std::string,std::string>> f):fixtures(std::move(f)){
#ifdef _WIN32
        WSADATA w{};if(WSAStartup(MAKEWORD(2,2),&w))throw std::runtime_error("startup");
#endif
        listener=socket(AF_INET,SOCK_STREAM,0);check(listener!=Sock(-1));
        sockaddr_in a{};a.sin_family=AF_INET;a.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
        check(bind(listener,reinterpret_cast<sockaddr*>(&a),sizeof(a))==0);
#ifdef _WIN32
        int n=sizeof(a);
#else
        socklen_t n=sizeof(a);
#endif
        check(getsockname(listener,reinterpret_cast<sockaddr*>(&a),&n)==0);port=ntohs(a.sin_port);check(listen(listener,1)==0);
        worker=std::thread([this]{Sock s=accept(listener,nullptr,nullptr);try{
            for(const auto& fixture:fixtures){std::string request;char c;
                while(true){if(recv(s,&c,1,0)!=1)throw std::runtime_error("fake EOF");
                if(c=='\x1a')break;
                request+=c;}
                recorded.push_back(request);if(request!=fixture.first)throw std::runtime_error("unexpected command: "+request);
                // Deliberately fragment every response and its terminator.
                for(char b:fixture.second+'\x1a')if(::send(s,&b,1,0)!=1)throw std::runtime_error("fake send");
            }
        }catch(...){error=std::current_exception();}shut(s);});
    }
    void finish(){worker.join();if(error)std::rethrow_exception(error);}
    ~Fake(){if(worker.joinable())worker.join();
    shut(listener);}
};
// Memory words for the ring_batch plan below (unset words read as their own
// address) and the reply ring_script must produce from them. Slots 8,9 then 0,1
// (w=12,r=8) with DMA lengths 4 (1 word, own read), 0 (no DMA read, wraps),
// 72 (18 words, read through the tail) and 81 (clamped to 20 words).
struct RingModel {
    std::map<uint32_t,uint32_t> mem;
    RingModel(){
        mem[roy_data]=0x314d5250;mem[roy_data+12]=40;mem[roy_data+28]=3;mem[roy_data+112]=12;mem[roy_data+116]=8;
        const uint32_t lengths[]={4,0,72,81};
        for(uint32_t i=8;i!=12;++i)mem[roy_data+380+(i%10)*112+16]=lengths[i-8]|0x00040000;
    }
    uint32_t at(uint32_t a) const{auto p=mem.find(a);return p==mem.end()?a:p->second;}
    std::vector<uint32_t> reply(const RingBatch& plan) const{
        std::vector<uint32_t> v;auto words=[&](uint32_t a,size_t n){for(size_t i=0;i<n;++i)v.push_back(at(a+uint32_t(4*i)));};
        if(plan.probe)words(0xe0001004,1);
        words(roy_data,4);words(roy_data+28,1);words(roy_data+112,2);
        for(uint32_t i=8;i!=12;++i){const uint32_t slot=roy_data+380+(i%10)*112;
            words(slot,5);words(slot+20,record_dma_words(at(slot+16)));words(slot+100,3);}
        for(auto e:plan.extra)words(e.first,e.second/4);
        return v;
    }
    // Jim Tcl file for the Frame: `openocd -f <file> 2>&1 | grep RING` runs the real
    // script under stub read_memory/write_memory and prints RING SCRIPT OK.
    std::string jim(const RingBatch& plan) const{
        std::ostringstream o;
        o<<"proc read_memory {a w n} {global pico_mem; set o {}; for {set i 0} {$i < $n} {incr i} {"
           "set k [expr {($a + 4 * $i) & 0xffffffff}]; if {[dict exists $pico_mem $k]} {lappend o [format 0x%08x [dict get $pico_mem $k]]} "
           "else {lappend o [format 0x%08x $k]}}; return $o}\n";
        o<<"proc write_memory {a w l} {global pico_writes; lappend pico_writes [expr {$a}] [expr {[lindex $l 0]}]}\n";
        o<<"set pico_writes {}\nset pico_mem [dict create]\n";
        for(auto& m:mem)o<<"dict set pico_mem "<<m.first<<" "<<m.second<<"\n";
        o<<"set pico_got [eval {"<<DebugMemory::ring_script(plan)<<"}]\n";
        o<<"set pico_want {";for(auto x:reply(plan))o<<x<<" ";
        o<<"}\n";
        o<<"set pico_ok [expr {[llength $pico_got] == [llength $pico_want]}]\n";
        o<<"foreach g $pico_got x $pico_want {if {$g eq {} || $x eq {} || $g != $x} {set pico_ok 0}}\n";
        o<<"if {$pico_writes ne {"<<(roy_data+116)<<" 12"<<(plan.heartbeat?" "+std::to_string(roy_data+1608)+" 1040":"")<<"}} {set pico_ok 0}\n";
        o<<"if {$pico_ok} {puts \"RING SCRIPT OK\"} else {puts \"RING SCRIPT MISMATCH got=$pico_got writes=$pico_writes\"}\nshutdown\n";
        return o.str();
    }
};
int main(int argc,char** argv){try{
    if(argc==3&&std::string(argv[1])=="--jim-ring-check"){
        RingBatch plan;plan.probe=true;plan.heartbeat=true;plan.extra={{roy_data+1516,16}};
        std::ofstream(argv[2])<<RingModel().jim(plan);
        std::cout<<"wrote "<<argv[2]<<"\n";return 0;
    }
    std::vector<std::string> sent;std::string response="0x44332211";
    DebugMemory m([&](const std::string& c){sent.push_back(c);return response;});
    for(size_t n=1;n<=4;++n){Bytes expected{0x11,0x22,0x33,0x44};expected.resize(n);check(m.read(roy_data,n)==expected);check(sent.back()=="read_memory 0x20010c18 32 1");}
    for(auto bad:{"", "1 2", "-1", "0x100000000", "nonsense", "1x"}){
        response=bad;
        rejects([&]{m.read(roy_data,4);},(response.empty()||response=="1 2")?"Incomplete debugger memory read":"Invalid debugger word");
    }
    auto before=sent.size();
    rejects([&]{m.write_many({{roy_data,Bytes(4)},{0,Bytes(4)}});},"Write is outside RAM module reservation");check(sent.size()==before);
    rejects([&]{m.read_many({{roy_data,4},{roy_data+1,4}});},"Invalid batched memory read");check(sent.size()==before);
    rejects([&]{m.read_many({{roy_data,0}});},"Invalid batched memory read");
    for(uint32_t a:{roy_data-4,roy_data+2048,roy_code-4,roy_code+2044,roy_rtt+12,roy_rtt+20,0xe0001004u})
        rejects([&]{m.write(a,Bytes(4));},"Write is outside RAM module reservation");
    rejects([&]{m.write(roy_data+1,Bytes(4));},"Unaligned RAM write");
    rejects([&]{m.write(roy_data,Bytes(3));},"Unaligned RAM write");
    for(uint32_t a:{0x2000d8ccu,0x20010a24u,0xe0001000u,0xe000edfcu})check(DebugMemory::write_commands(a,Bytes(4)).size()==1);
    check(DebugMemory::write_commands(roy_rtt+16,Bytes(8)).size()==1);
    auto chunks=DebugMemory::write_commands(roy_data,Bytes(68));check(chunks.size()==2);check(chunks[1]=="write_memory 0x20010c58 32 {0x0}");
    m.write_many({{roy_data,Bytes{1,2,3,4}},{roy_data+116,Bytes{7,0,0,0}}});
    check(sent.back()=="write_memory 0x20010c18 32 {0x4030201}; write_memory 0x20010c8c 32 {0x7}");
    before=sent.size();m.write_many({});check(sent.size()==before);
    {   // Batch: queued writes, superseded identical-shape write_many, prefetch, accounting.
        std::vector<std::string> log;
        DebugMemory b([&](const std::string& c){log.push_back(c);return std::string(c.rfind("concat ",0)==0?"0x5":"");});
        const auto start=DebugMemory::stats();
        {
            DebugMemory::Batch batch;
            DebugMemory::Batch::prefetch(roy_data+1516,Bytes(16,7));
            DebugMemory::Batch::prefetch(roy_data+1600,Bytes(4,9));
            b.write_many({{roy_data+1664,Bytes(4,1)},{roy_data+1668,Bytes(4)},{roy_data+1664,Bytes(4)}});
            b.write_many({{roy_data+1664,Bytes(4,3)},{roy_data+1668,Bytes(4)},{roy_data+1664,Bytes(4,2)}});
            check(log.empty());
            check(b.read_many({{roy_data+1516,16}})==Bytes(16,7)&&log.empty());
            b.write(roy_data+1600,Bytes(4)); // overlapping queued write drops that prefetch
            check(batch.requested()==std::vector<std::pair<uint32_t,size_t>>({{roy_data+1516,16}}));
            check(b.read_many({{roy_data+1600,4}})==Bytes({5,0,0,0}));
            check(log.size()==2);
            check(log[0]=="write_memory 0x20011298 32 {0x3030303}; write_memory 0x2001129c 32 {0x0}; "
                          "write_memory 0x20011298 32 {0x2020202}; write_memory 0x20011258 32 {0x0}");
            check(log[1]=="concat [read_memory 0x20011258 32 1]");
            b.write_many({{roy_data+1664,Bytes(4)}});
            check(log.size()==2);
        }
        check(log.size()==3&&log[2]=="write_memory 0x20011298 32 {0x0}");
        const auto& end=DebugMemory::stats();
        check(end.commands-start.commands==3&&end.bytes_written-start.bytes_written==20&&end.bytes_read-start.bytes_read==4);
        // Outside a batch writes leave immediately again.
        b.write(roy_data,Bytes(4));check(log.size()==4);
    }
    {   // ring_batch: probe, used header words, wrapped slots (w=12,r=8 -> slots 8,9 then 0,1)
        // read only as far as parse_record looks, extra read.
        RingBatch plan;plan.probe=true;plan.heartbeat=true;plan.extra={{roy_data+1516,16}};
        const auto script=DebugMemory::ring_script(plan);
        check(script.rfind("set pico_o {}; set pico_o [read_memory 0xe0001004 32 1]",0)==0);
        check(script.find("[read_memory 0x20010c18 32 4] [read_memory 0x20010c34 32 1] [read_memory 0x20010c88 32 2]")!=std::string::npos);
        check(script.find(" 32 30]")==std::string::npos&&script.find("28 *")==std::string::npos);  // no full header, no full slots
        check(script.find("write_memory 0x20010c8c 32 [list $pico_w]")!=std::string::npos);
        check(script.find("write_memory 0x20011260 32")!=std::string::npos);
        check(script.find("[read_memory 0x20011204 32 4]")<script.find("write_memory"));
        const RingModel model;
        std::string reply;for(auto x:model.reply(plan))reply+=std::to_string(x)+" ";
        DebugMemory b([&](const std::string& c){check(c==script);return reply;});
        const auto start=DebugMemory::stats();
        const auto got=b.ring_batch(plan);
        check(got.cycles==0xe0001004u&&got.write==12&&got.read==8&&got.header.size()==28);
        check(w32(got.header)==0x314d5250&&w32(got.header,12)==40&&w32(got.header,16)==3);
        // Each copied slot holds exactly words 0..4, its DMA words and 25..27; the rest is zero.
        const uint32_t dma[]={1,0,18,20};
        for(uint32_t i=8;i!=12;++i){
            const size_t o=(i%10)*112;const uint32_t slot=roy_data+380+uint32_t(o);
            for(size_t k=0;k<28;++k){
                const bool used=k<5+dma[i-8]||k>=25;
                check(w32(got.ring,o+4*k)==(used?model.at(slot+uint32_t(4*k)):0u));
            }
        }
        check(std::all_of(got.ring.begin()+2*112,got.ring.begin()+8*112,[](uint8_t x){return x==0;}));
        check(got.extra.size()==1&&w32(got.extra[0])==roy_data+1516&&w32(got.extra[0],12)==roy_data+1528);
        const auto& end=DebugMemory::stats();
        // 1 probe + 7 header + (8+1)+(8+0)+(8+18+2 dropped)+(8+20) record + 4 extra words.
        check(end.commands-start.commands==1&&end.bytes_read-start.bytes_read==(1+7+73+4)*4&&end.bytes_written-start.bytes_written==8);
        const auto full=reply;
        reply=full+" 1";rejects([&]{b.ring_batch(plan);},"Incomplete radio batch");
        reply=full.substr(0,full.rfind(' ',full.size()-2));rejects([&]{b.ring_batch(plan);},"Incomplete radio batch");
        reply="77 0x314d5250 0 0 40 3 12 8";rejects([&]{b.ring_batch(plan);},"Incomplete radio batch");
        reply="can't read \"pico_w\": no such variable";rejects([&]{b.ring_batch(plan);},"Invalid debugger word");
        plan.extra={{roy_data+1,4}};rejects([&]{DebugMemory::ring_script(plan);},"Invalid batched memory read");
        check(script.find("0x494d5532")!=std::string::npos&&script.find("12 *")!=std::string::npos);
        std::vector<uint32_t> imu(7,0);imu[0]=0x314d5250;imu[2]=0x494d5532;imu[5]=2;
        for(uint32_t k=0;k<24;++k)imu.push_back(1000+k);
        std::string imu_text;for(auto x:imu)imu_text+=std::to_string(x)+" ";
        DebugMemory imu_io([&](const std::string&){return imu_text;});
        const auto imu_got=imu_io.ring_batch(RingBatch{});
        check(imu_got.write==2&&imu_got.read==0&&w32(imu_got.ring,0)==1000&&w32(imu_got.ring,44)==1011);
        check(w32(imu_got.ring,48)==1012&&w32(imu_got.ring,92)==1023);
        // ackring5: flag 0x494d5533 = 23 slots; w=25, r=0 keeps the newest 23 (indices 2..24), wrapping 22 -> 0.
        check(script.find("0x494d5533")!=std::string::npos&&script.find("pico_D")!=std::string::npos);
        std::vector<uint32_t> d23(7,0);d23[0]=0x314d5250;d23[2]=0x494d5533;d23[5]=25;
        for(uint32_t k=0;k<23*12;++k)d23.push_back(2000+k);
        std::string d23_text;for(auto x:d23)d23_text+=std::to_string(x)+" ";
        DebugMemory d23_io([&](const std::string&){return d23_text;});
        const auto d23_got=d23_io.ring_batch(RingBatch{});
        check(d23_got.write==25&&d23_got.ring.size()>=23*48);
        check(w32(d23_got.ring,2*48)==2000&&w32(d23_got.ring,22*48+44)==2000+21*12-1);  // index 2..22
        check(w32(d23_got.ring,0)==2000+21*12&&w32(d23_got.ring,48+44)==2000+23*12-1);  // index 23,24 -> 0,1
        check(pico::imu_ring_depth_for(0x494d5532)==16&&pico::imu_ring_depth_for(0x494d5533)==23&&pico::imu_ring_depth_for(0x494d5534)==23&&
              pico::imu_ring_depth_for(1)==0);
    }
    Fake fake({{"read_memory 0x20010c18 32 1"," \n0x44332211\t"},
               {"concat [read_memory 0x200111f8 32 24] [read_memory 0x20011370 32 10]",std::string("0 ")+[] {std::string s;for(int i=1;i<34;++i)s+="0 ";
               return s;}()},
               {"write_memory 0x20010c18 32 {0x4030201}",""}});
    {TclSocket socket(fake.port);DebugMemory io([&](const std::string& c){return socket.command(c);});
        check(io.read(roy_data,3)==Bytes({0x11,0x22,0x33}));
        check(io.read_many({{roy_data+1504,96},{roy_data+1880,40}})==Bytes(136));
        io.write(roy_data,Bytes{1,2,3,4});}
    fake.finish();check(fake.recorded.size()==3);
    for(auto error:{"ERROR: fixture","Invalid command fixture","not halted","couldn't","FAILED","timed out"}){
        Fake f({{"poll",error},{"poll","running"}});{TclSocket socket(f.port);rejects([&]{socket.command("poll");},error);
            check(socket.command("poll")=="running");}f.finish();check(f.recorded.size()==2);
    }
    Fake eof({});{TclSocket socket(eof.port);rejects([&]{socket.command("poll");},"OpenOCD disconnected");rejects([&]{socket.command("poll");},"OpenOCD transport unresolved");}eof.finish();
    std::cout<<"PASS "<<checks<<" TCL checks\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}

