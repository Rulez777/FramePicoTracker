#include "tcl_transport.hpp"
#include "imu_slot.hpp"
#define NOMINMAX
#include <algorithm>
#include <chrono>
#include <cctype>
#include <iostream>
#include <sstream>
#include <stdexcept>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
using Sock=SOCKET;
static void close_socket(Sock s){closesocket(s);}
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
using Sock=int;
static void close_socket(Sock s){close(s);}
#endif
namespace pico {
[[noreturn]] static void fail(const char* s){throw std::runtime_error(s);}
static std::string hx(uint32_t v){std::ostringstream o;o<<"0x"<<std::hex<<v;return o.str();}
namespace {
// owner only groups/supersedes entries; command is a copy, so a flush never touches a destroyed DebugMemory.
struct Queued{const void* owner;std::function<std::string(const std::string&)> command;
    std::vector<std::pair<uint32_t,size_t>> shape;std::vector<std::string> commands;size_t bytes;};
struct BatchState{
    int depth=0;
    std::vector<Queued> queue;
    std::vector<std::pair<std::pair<uint32_t,size_t>,Bytes>> prefetched;
    std::vector<std::pair<uint32_t,size_t>> requested;
};
thread_local BatchState batch_state;
thread_local TclStats tcl_stats;
bool overlaps(uint32_t a,size_t n,uint32_t b,size_t m){return uint64_t(a)<uint64_t(b)+m&&uint64_t(b)<uint64_t(a)+n;}
uint32_t at(const Bytes& b,size_t o){return uint32_t(b[o])|uint32_t(b[o+1])<<8|uint32_t(b[o+2])<<16|uint32_t(b[o+3])<<24;}
// Complete write_memory commands joined into one TCL request.
void send_writes(const std::function<std::string(const std::string&)>& command,const std::vector<std::string>& commands,size_t bytes){
    std::string script;for(const auto& c:commands){if(!script.empty())script+="; ";
    script+=c;}
    if(script.empty())return;
    ++tcl_stats.commands;tcl_stats.bytes_written+=bytes;
    command(script);
}
void flush_queue(){
    auto queue=std::move(batch_state.queue);batch_state.queue.clear();
    for(size_t i=0;i<queue.size();){
        const auto owner=queue[i].owner;const auto command=queue[i].command;std::vector<std::string> commands;size_t bytes=0;
        for(;i<queue.size()&&queue[i].owner==owner;++i){
            commands.insert(commands.end(),queue[i].commands.begin(),queue[i].commands.end());
            bytes+=queue[i].bytes;
        }
        send_writes(command,commands,bytes);
    }
}
}
TclStats& DebugMemory::stats(){return tcl_stats;}
TclSocket::TclSocket(uint16_t port){
#ifdef _WIN32
    static const bool ready=[]{WSADATA w{};return WSAStartup(MAKEWORD(2,2),&w)==0;}();
    if(!ready) fail("Winsock startup failed");
#endif
    Sock s=socket(AF_INET,SOCK_STREAM,0);
    if(s==Sock(-1)) fail("OpenOCD socket failed");
    int one=1;
    setsockopt(s,IPPROTO_TCP,TCP_NODELAY,reinterpret_cast<const char*>(&one),sizeof(one));
#ifdef _WIN32
    DWORD timeout=10000;
#else
    timeval timeout{10,0};
#endif
    setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,reinterpret_cast<const char*>(&timeout),sizeof(timeout));
    setsockopt(s,SOL_SOCKET,SO_SNDTIMEO,reinterpret_cast<const char*>(&timeout),sizeof(timeout));
    sockaddr_in a{};a.sin_family=AF_INET;a.sin_port=htons(port);a.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
    if(connect(s,reinterpret_cast<sockaddr*>(&a),sizeof(a))!=0){close_socket(s);fail("OpenOCD TCL connection failed");}
    fd_=intptr_t(s);
}
TclSocket::~TclSocket(){if(fd_!=-1)close_socket(Sock(fd_));}
std::string TclSocket::command(const std::string& text){
    if(poisoned_)fail("OpenOCD transport unresolved");
    if(text.size()>65535 || text.find('\x1a')!=std::string::npos || text.find('\0')!=std::string::npos)fail("Invalid TCL request");
    ++tcl_stats.wire;
    std::string request=text+'\x1a',result;result.reserve(65536);
    try{
        const auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(10);
        auto wait_ready=[&](bool writing){
            const auto left=std::chrono::duration_cast<std::chrono::microseconds>(deadline-std::chrono::steady_clock::now()).count();
            if(left<=0)fail("OpenOCD command timed out");
            timeval tv{long(left/1000000),long(left%1000000)};
            fd_set fds;FD_ZERO(&fds);FD_SET(Sock(fd_),&fds);
            if(select(int(fd_)+1,writing?nullptr:&fds,writing?&fds:nullptr,nullptr,&tv)<=0)
                fail("OpenOCD command timed out");
        };
        size_t offset=0;
        while(offset<request.size()){
#ifdef _WIN32
            int flags=0;
#else
            int flags=MSG_NOSIGNAL;
#endif
            wait_ready(true);
            int n=int(::send(Sock(fd_),request.data()+offset,int(request.size()-offset),flags));
            if(n<=0)fail("OpenOCD disconnected");
            offset+=size_t(n);
        }
        // Whole segments per recv; one select/recv pair per byte cost ~2 syscalls per response character.
        char buffer[4096];
        for(;;){
            const auto end=pending_.find('\x1a');
            if(end!=std::string::npos){
                if(result.size()+end>65536)fail("OpenOCD response exceeds bound");
                result.append(pending_,0,end);pending_.erase(0,end+1);break;
            }
            if(result.size()+pending_.size()>65536)fail("OpenOCD response exceeds bound");
            result+=pending_;pending_.clear();
            wait_ready(false);
            int n=int(recv(Sock(fd_),buffer,int(sizeof(buffer)),0));
            if(n<=0)fail("OpenOCD disconnected");
            pending_.assign(buffer,size_t(n));
        }
    }catch(...){poisoned_=true;throw;}
    auto first=result.find_first_not_of(" \t\r\n");
    result=first==std::string::npos?"":result.substr(first,result.find_last_not_of(" \t\r\n")-first+1);
    auto lower=result;std::transform(lower.begin(),lower.end(),lower.begin(),[](unsigned char c){return char(std::tolower(c));});
    for(auto error:{"error:","invalid command","not halted","couldn't","failed","timed out"})
        if(lower.find(error)!=std::string::npos)throw std::runtime_error(result);
    return result;
}
static std::vector<uint32_t> values(const std::string& text){
    std::istringstream in(text);std::string token;std::vector<uint32_t> out;
    while(in>>token){size_t used=0;unsigned long long v;
        try{if(token[0]=='-')fail("Invalid debugger word");
        v=std::stoull(token,&used,0);}catch(...){fail("Invalid debugger word");}
        if(used!=token.size()||v>0xffffffffULL)fail("Invalid debugger word");
        out.push_back(uint32_t(v));
    }
    return out;
}
static Bytes words(const std::string& text,size_t count,const char* error){
    std::istringstream in(text);std::string token;Bytes out;out.reserve(count*4);
    while(in>>token){size_t used=0;unsigned long long v;
        try{if(token[0]=='-')fail("Invalid debugger word");
        v=std::stoull(token,&used,0);}catch(...){fail("Invalid debugger word");}
        if(used!=token.size()||v>0xffffffffULL)fail("Invalid debugger word");
        if(out.size()/4>=count)fail(error);
        for(int j=0;j<4;++j)out.push_back(uint8_t(v>>(8*j)));
    }
    if(out.size()/4!=count)fail(error);
    return out;
}
std::string DebugMemory::issue(const std::string& text,size_t read,size_t written){
    ++tcl_stats.commands;tcl_stats.bytes_read+=read;tcl_stats.bytes_written+=written;
    return command_(text);
}
Bytes DebugMemory::read(uint32_t address,size_t count){
    if(count>65536 || uint64_t(address)+count>0x100000000ULL)fail("Invalid debugger read");
    if(batch_state.depth)flush_queue();
    const auto prefix=address&3u; address-=prefix;
    auto n=(count+prefix+3)/4;auto out=words(issue("read_memory "+hx(address)+" 32 "+std::to_string(n),n*4,0),n,"Incomplete debugger memory read");out.erase(out.begin(),out.begin()+prefix);out.resize(count);return out;
}
Bytes DebugMemory::read_many(const std::vector<std::pair<uint32_t,size_t>>& regions){
    std::string cmd="concat ";size_t count=0;
    for(auto r:regions){if((r.first&3)||(r.second&3)||!r.second||r.second>65536||uint64_t(r.first)+r.second>0x100000000ULL||count+r.second>65536)fail("Invalid batched memory read");
        if(count)cmd+=' ';
        cmd+="[read_memory "+hx(r.first)+" 32 "+std::to_string(r.second/4)+"]";count+=r.second;}
    if(batch_state.depth){
        auto& s=batch_state;
        for(auto r:regions)if(std::find(s.requested.begin(),s.requested.end(),r)==s.requested.end())s.requested.push_back(r);
        auto served=[&](std::pair<uint32_t,size_t> r){return std::find_if(s.prefetched.begin(),s.prefetched.end(),[&](const auto& p){return p.first==r;});};
        if(std::all_of(regions.begin(),regions.end(),[&](auto r){return served(r)!=s.prefetched.end();})){
            Bytes out;
            for(auto r:regions){auto p=served(r);out.insert(out.end(),p->second.begin(),p->second.end());s.prefetched.erase(p);}
            return out;
        }
        flush_queue();
    }
    return words(issue(cmd,count,0),count/4,"Incomplete batched debugger memory read");
}
std::vector<std::string> DebugMemory::write_commands(uint32_t a,const Bytes& b){
    uint64_t end=uint64_t(a)+b.size();
    if(!((a>=roy_data&&end<=roy_data+roy_env("ROY_DATA_SIZE",2048,10))||(a>=roy_code&&end<=roy_code+roy_env("ROY_CODE_SIZE",2045,10))||
        ((a==roy_env("ROY_HOOK",0x2000d8cc)||a==roy_env("ROY_STATIC",0x20010a24)||a==0xe0001000||a==0xe000edfc)&&b.size()==4)||(a==roy_rtt+16&&b.size()==8)))fail("Write is outside RAM module reservation");
    if((a&3) && a==roy_env("ROY_STATIC",0x20010a24) && b.size()==4) {
        std::string c="write_memory "+hx(a)+" 8 {";
        for(size_t i=0;i<b.size();++i) {if(i)c+=' ';c+=hx(b[i]);}
        return {c+"}"};
    }
    if((a&3)||(b.size()&3))fail("Unaligned RAM write");
    std::vector<std::string> out;
    for(size_t i=0;i<b.size();i+=64){std::string c="write_memory "+hx(a+uint32_t(i))+" 32 {";
        for(size_t j=i;j<std::min(i+64,b.size());j+=4){if(j!=i)c+=' ';
        uint32_t v=0;for(int k=0;k<4;++k)v|=uint32_t(b[j+k])<<(8*k);
        c+=hx(v);}out.push_back(c+"}");}
    return out;
}
void DebugMemory::write(uint32_t a,const Bytes& b){
    if(batch_state.depth){write_many({{a,b}});return;}
    const auto commands=write_commands(a,b);
    for(size_t i=0;i<commands.size();++i)issue(commands[i],0,std::min<size_t>(64,b.size()-i*64));
}
void DebugMemory::write_many(const std::vector<Region>& regions){
    std::vector<std::string> commands;Queued q{this,command_,{},{},0};
    for(const auto& r:regions){auto c=write_commands(r.first,r.second);commands.insert(commands.end(),c.begin(),c.end());
        q.shape.push_back({r.first,r.second.size()});q.bytes+=r.second.size();}
    if(commands.empty())return;
    if(batch_state.depth){
        auto& s=batch_state;
        // A later identical-shape write_many from the same owner overwrites every byte of the earlier one.
        s.queue.erase(std::remove_if(s.queue.begin(),s.queue.end(),[&](const Queued& e){return e.owner==this&&e.shape==q.shape;}),s.queue.end());
        s.prefetched.erase(std::remove_if(s.prefetched.begin(),s.prefetched.end(),[&](const auto& p){
            return std::any_of(q.shape.begin(),q.shape.end(),[&](auto w){return overlaps(w.first,w.second,p.first.first,p.first.second);});}),s.prefetched.end());
        q.commands=std::move(commands);s.queue.push_back(std::move(q));
        return;
    }
    send_writes(command_,commands,q.bytes);
}
DebugMemory::Batch::Batch(){++batch_state.depth;}
DebugMemory::Batch::~Batch(){
    if(batch_state.depth==1)try{flush_queue();}catch(const std::exception& e){std::cerr<<"Deferred debugger writes: "<<e.what()<<"; continuing\n";}
    if(--batch_state.depth==0){batch_state.prefetched.clear();batch_state.requested.clear();batch_state.queue.clear();}
}
void DebugMemory::Batch::flush(){flush_queue();}
std::vector<std::pair<uint32_t,size_t>> DebugMemory::Batch::requested() const{return batch_state.requested;}
void DebugMemory::Batch::prefetch(uint32_t address,Bytes bytes){
    if(!batch_state.depth)return;
    batch_state.prefetched.push_back({{address,bytes.size()},std::move(bytes)});
}
std::string DebugMemory::ring_script(const RingBatch& b){
    for(auto e:b.extra)if((e.first&3)||(e.second&3)||!e.second||e.second>4096||uint64_t(e.first)+e.second>0x100000000ULL)fail("Invalid batched memory read");
    const auto ring=hx(roy_data+380);
    // Jim Tcl evaluated by the OpenOCD TCL server; mirrors snapshot(): nothing on
    // rollback, newest ten after overflow, slot 9 wraps to slot 0. Header: words
    // 0..3 (magic, flag, heartbeat base; 2 rides along, cheaper than a split), 7
    // (repeat) and 28..29 (write/read index). Each record emits words 0..4, its
    // record_dma_words() DMA words and 25..27. A record's tail is read together
    // with the next slot's words 0..4 (adjacent unless wrapping); when at most 2
    // unused words separate DMA and tail they are read in one piece and dropped.
    std::string s="set pico_o {}";
    if(b.probe)s+="; set pico_o [read_memory 0xe0001004 32 1]";
    s+="; set pico_h [concat [read_memory "+hx(roy_data)+" 32 4] [read_memory "+hx(roy_data+28)+" 32 1] [read_memory "+hx(roy_data+112)+" 32 2]]"
       "; set pico_w [expr {[lindex $pico_h 5]}]; set pico_r [expr {[lindex $pico_h 6]}]";
    s+="; set pico_o [concat $pico_o $pico_h]";
    // enabled == an imu_ring flag: contiguous 48/80-byte slots, not a per-record DMA walk.
    // 0x494d5532 = 16 slots, 0x494d5533/0x494d5534/0x494d5535 (multi4) = 23 slots (imu_ring_depth_for in imu_slot.hpp).
    s+="; set pico_B 48; set pico_D 0; if {[lindex $pico_h 2] == 0x494d5532} {set pico_D 16}; if {[lindex $pico_h 2] == 0x494d5533 || [lindex $pico_h 2] == 0x494d5534 || [lindex $pico_h 2] == 0x494d5535} {set pico_D 23}; if {[lindex $pico_h 2] == 0x494d5536 || [lindex $pico_h 2] == 0x494d5537} {set pico_D 14; set pico_B 80}"
       "; if {$pico_D} {if {$pico_w > $pico_r} {set pico_s $pico_r; if {$pico_w - $pico_s > $pico_D} {set pico_s [expr {$pico_w - $pico_D}]}; "
       "set pico_n [expr {$pico_w - $pico_s}]; set pico_f [expr {$pico_s % $pico_D}]; set pico_k [expr {$pico_D - $pico_f}]; if {$pico_k > $pico_n} {set pico_k $pico_n}; "
       "set pico_o [concat $pico_o [read_memory [expr {"+ring+" + $pico_B * $pico_f}] 32 [expr {$pico_B / 4 * $pico_k}]]]; "
       "if {$pico_n > $pico_k} {set pico_o [concat $pico_o [read_memory [expr {"+ring+"}] 32 [expr {$pico_B / 4 * ($pico_n - $pico_k)}]]]}}} else {";
    s+="if {$pico_w > $pico_r} {set pico_s $pico_r; if {$pico_w - $pico_s > 10} {set pico_s [expr {$pico_w - 10}]}; set pico_t {}; "
       "for {set pico_i $pico_s} {$pico_i < $pico_w} {incr pico_i} {"
       "set pico_a [expr {"+ring+" + 112 * ($pico_i % 10)}]; "
       "if {[llength $pico_t] == 0} {set pico_t [read_memory $pico_a 32 5]}; "
       "set pico_d [expr {(([lindex $pico_t 4] & 0xffff) + 3) / 4}]; if {$pico_d > 20} {set pico_d 20}; "
       "set pico_o [concat $pico_o $pico_t]; "
       "set pico_k 3; if {$pico_i + 1 < $pico_w && ($pico_i + 1) % 10 != 0} {set pico_k 8}; "
       "if {$pico_d >= 18} {set pico_v [read_memory [expr {$pico_a + 20}] 32 [expr {20 + $pico_k}]]; "
       "set pico_o [concat $pico_o [lrange $pico_v 0 [expr {$pico_d - 1}]]]; set pico_v [lrange $pico_v 20 end]} else {"
       "if {$pico_d > 0} {set pico_o [concat $pico_o [read_memory [expr {$pico_a + 20}] 32 $pico_d]]}; "
       "set pico_v [read_memory [expr {$pico_a + 100}] 32 $pico_k]}; "
       "set pico_o [concat $pico_o [lrange $pico_v 0 2]]; set pico_t [lrange $pico_v 3 end]}}}";
    for(auto e:b.extra)s+="; set pico_o [concat $pico_o [read_memory "+hx(e.first)+" 32 "+std::to_string(e.second/4)+"]]";
    s+="; if {$pico_w != $pico_r} {write_memory "+hx(roy_data+116)+" 32 [list $pico_w]}";
    if(b.heartbeat)s+="; write_memory "+hx(roy_data+1608)+" 32 [list [expr {([lindex $pico_h 3] + 1000) & 0xffffffff}]]";
    return s+"; set pico_o";
}
RingBatchResult DebugMemory::ring_batch(const RingBatch& b){
    const auto script=ring_script(b);
    if(batch_state.depth)flush_queue();
    const auto v=values(issue(script,0,0));
    RingBatchResult out;size_t i=0;
    auto take=[&](size_t n){if(v.size()-i<n)fail("Incomplete radio batch");
    Bytes x;
        for(size_t k=0;k<n;++k){const auto w=v[i++];for(int j=0;j<4;++j)x.push_back(uint8_t(w>>(8*j)));}return x;};
    if(b.probe)out.cycles=at(take(1),0);
    out.header=take(7);
    out.write=at(out.header,20);out.read=at(out.header,24);
    out.ring.assign(1120,0);
    const auto w=out.write,r=out.read;
    size_t dropped=0;  // unused words a one-piece DMA+tail read fetched and the script dropped
    if(const uint32_t depth=imu_ring_depth_for(at(out.header,8))){
        if(w>r)
            for(uint32_t index=w-r>depth?w-depth:r;index!=w;++index){
                const auto slot=take(imu_ring_stride_for(at(out.header,8))/4);
                std::copy(slot.begin(),slot.end(),out.ring.begin()+(index%depth)*imu_ring_stride_for(at(out.header,8)));
            }
    }else if(w>r)
        for(uint32_t index=w-r>10?w-10:r;index!=w;++index){
            const auto slot=out.ring.begin()+(index%10)*112;
            const auto head=take(5);std::copy(head.begin(),head.end(),slot);
            const auto dma_words=record_dma_words(at(head,16));
            const auto dma=take(dma_words);std::copy(dma.begin(),dma.end(),slot+20);
            const auto tail=take(3);std::copy(tail.begin(),tail.end(),slot+100);
            if(dma_words>=18)dropped+=20-dma_words;
        }
    for(auto e:b.extra)out.extra.push_back(take(e.second/4));
    if(i!=v.size())fail("Incomplete radio batch");
    tcl_stats.bytes_read+=(v.size()+dropped)*4;
    tcl_stats.bytes_written+=(w!=r?4:0)+(b.heartbeat?4:0);
    return out;
}
}
