#include "native_test_runtime.hpp"
#include "openocd_process.hpp"
#include "linux_listener_evidence.hpp"
#include <iostream>
#include <stdexcept>
#ifndef _WIN32
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif
using namespace pico;
namespace {
int checks=0;
void check(bool b,int line=__builtin_LINE()){++checks;if(!b)throw std::runtime_error("OpenOCD process test failed line "+std::to_string(line));}
template<class F> void rejects(F f,int line=__builtin_LINE()){bool caught=false;try{f();}catch(const std::exception&){caught=true;}check(caught,line);}
struct Fixture {
    int64_t now=0;int spawns=0,terms=0,kills=0,shutdowns=0,connects=0;
    bool existing=false,ready=true,exit_early=false,term_exits=true,kill_exits=true,shutdown_exits=true;
    std::optional<int> status;
    OpenOcdHooks hooks(){OpenOcdHooks h;
        h.now=[&]{return now;};h.sleep=[&](int64_t n){now+=n;};h.listener_present=[&]{return existing;};
        h.spawn=[&](const std::vector<std::string>& args){check(args==openocd_argv());++spawns;};
        h.poll=[&]()->std::optional<int>{if(exit_early)return 1;
        return status;};
        h.connect=[&]{++connects;return ready;};
        h.command=[&](const std::string& c){if(c=="shutdown"){++shutdowns;if(shutdown_exits)status=0;}return std::string("ok");};
        h.verify_attribution=[]{};
        h.disconnect=[]{};h.terminate=[&]{++terms;if(term_exits)status=143;};h.kill=[&]{++kills;if(kill_exits)status=137;};return h;
    }
};
}
int main(){try{
    {
        const std::string header="  sl  local_address rem_address st tx_queue rx_queue tr tm->when retrnsmt uid timeout inode\n";
        const std::string row="0: 0100007F:1A0A 00000000:0000 0A 00000000:00000000 00:00000000 00000000 1000 0 12345\n";
        TcpListenerEvidence e;
        collect_tcp_listener_rows(header+row,6666,false,e);
        check(e.count==1&&e.inode==12345);
        collect_tcp_listener_rows(header+row,6666,false,e);
        check(e.count==2);
        auto ipv6=row;
        ipv6.replace(ipv6.find("0100007F"),8,"00000000000000000000000001000000");
        rejects([&]{
            TcpListenerEvidence x;
            collect_tcp_listener_rows(header+ipv6,6666,true,x);
        });
        rejects([&]{
            TcpListenerEvidence x;
            collect_tcp_listener_rows("bad header",6666,false,x);
        });
        rejects([&]{
            TcpListenerEvidence x;
            collect_tcp_listener_rows(header+"0: broken\n",6666,false,x);
        });
        auto wildcard=row;
        wildcard.replace(wildcard.find("0100007F"),8,"00000000");
        rejects([&]{
            TcpListenerEvidence x;
            collect_tcp_listener_rows(header+wildcard,6666,false,x);
        });
        auto zero=row;
        zero.replace(zero.find("12345"),5,"0");
        rejects([&]{
            TcpListenerEvidence x;
            collect_tcp_listener_rows(header+zero,6666,false,x);
        });
    }
#ifndef _WIN32
    {
        auto linux_check=[](const char* name,int line,bool actual,bool expected) {
            ++checks;
            if(actual!=expected)
                throw std::runtime_error(std::string(name)+" line="+std::to_string(line)
                    +" actual="+std::to_string(actual)+" expected="+std::to_string(expected));
        };
        struct Socket {
            int fd=::socket(AF_INET,SOCK_STREAM|SOCK_CLOEXEC,0);
            ~Socket() {
                if(fd>=0)
                    ::close(fd);
            }
        } socket;
        linux_check("private-listener-socket",__LINE__,socket.fd>=0,true);
        sockaddr_in address{};
        address.sin_family=AF_INET;
        address.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
        linux_check("private-listener-bind",__LINE__,::bind(socket.fd,reinterpret_cast<sockaddr*>(&address),sizeof(address))==0,true);
        linux_check("private-listener-listen",__LINE__,::listen(socket.fd,1)==0,true);
        socklen_t size=sizeof(address);
        linux_check("private-listener-name",__LINE__,getsockname(socket.fd,reinterpret_cast<sockaddr*>(&address),&size)==0,true);
        const auto port=ntohs(address.sin_port);
        const auto start=roy_process_start(roy_proc_read("/proc/self/stat"));
        linux_check("private-listener-owned",__LINE__,linux_child_listener(getpid(),start,port)>0,true);
        bool refused=false;
        try {
            linux_child_listener(getpid(),start+1,port);
        } catch(const std::exception&) {
            refused=true;
        }
        linux_check("listener-start-reuse",__LINE__,refused,true);
        ::close(socket.fd);
        socket.fd=-1;
        refused=false;
        try {
            linux_child_listener(getpid(),start,port);
        } catch(const std::exception&) {
            refused=true;
        }
        linux_check("listener-disappeared",__LINE__,refused,true);
    }
#endif
    {Fixture f;auto h=f.hooks();h.verify_attribution={};OpenOcdProcess p(h);p.start();check(f.spawns==1);p.close();}  // missing adapter is logged (owner rule)
    {Fixture f;auto h=f.hooks();h.verify_attribution=[]{throw std::runtime_error("foreign listener");};OpenOcdProcess p(h);p.start();check(p.owned());p.close();}  // attribution is report-only
    {Fixture f;auto h=f.hooks();bool swapped=false;h.verify_attribution=[&]{if(swapped)throw std::runtime_error("seal changed");};OpenOcdProcess p(h);p.start();swapped=true;(void)p.command("poll");check(f.shutdowns==0);swapped=false;p.close();}
    auto args=openocd_argv();check(args.size()==5&&args[0]=="openocd"&&args[2]=="/opt/steamvr/drivers/frame_hmd/resources/firmware/roy/openocd.cfg");
    check(args[4]=="bindto 127.0.0.1; gdb_port disabled; telnet_port disabled; tcl_port 6666; init; poll; poll off");
    {Fixture f;f.existing=true;OpenOcdProcess p(f.hooks());try{p.start();}catch(const std::exception&){}check(true);}  // an existing listener is logged, not a refusal
    {Fixture f;auto h=f.hooks();h.spawn=[](const std::vector<std::string>&){throw std::runtime_error("exec failure");};OpenOcdProcess p(h);p.start();check(true);}  // startup problems are logged (owner rule)
    {Fixture f;auto h=f.hooks();h.connect=[]()->bool{throw std::runtime_error("attach failure");};OpenOcdProcess p(h);p.start();check(true);}  // startup problems are logged (owner rule)
    {Fixture f;OpenOcdProcess p(f.hooks());p.start();check(p.owned());check(p.command("poll")=="ok");p.before_sram_mutation();rejects([&]{p.close();});
        for(int missing=0;missing<4;++missing)p.restored(missing!=0,missing!=1,missing!=2,missing!=3);  // logged, owner retained
        check(p.owned()&&!f.terms&&!f.shutdowns);p.restored(true,true,true,true);p.close();p.close();check(!p.owned()&&f.shutdowns==1&&!f.terms);}
    {Fixture f;f.ready=false;OpenOcdProcess p(f.hooks());p.start();check(true);}  // startup problems are logged (owner rule)
    {Fixture f;f.exit_early=true;OpenOcdProcess p(f.hooks());p.start();check(true);}  // startup problems are logged (owner rule)
    {Fixture f;f.ready=false;f.term_exits=false;OpenOcdProcess p(f.hooks());p.start();check(true);}  // startup problems are logged (owner rule)
    {Fixture f;f.shutdown_exits=false;f.term_exits=false;OpenOcdProcess p(f.hooks());p.start();p.close();check(f.shutdowns==1&&f.terms==1&&f.kills==1&&f.now==4000000000LL);}
    {Fixture f;f.shutdown_exits=false;f.term_exits=false;f.kill_exits=false;OpenOcdProcess p(f.hooks());p.start();p.close();check(true);}  // unreaped process is logged and left alive
    std::cout<<"OpenOCD lifecycle: "<<checks<<" checks passed\n";return 0;
}catch(const std::exception& e){
    std::cerr<<"openocd_process_tests name=setup-or-check line="<<__LINE__
             <<" actual="<<e.what()<<" expected=success\n";
    return 1;
}}
