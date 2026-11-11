#include "native_test_runtime.hpp"
#include "live_policy.hpp"
#include "listener_attribution.hpp"
#include "openocd_process.hpp"
#include "observer_pipe.hpp"
#include "observer_process.hpp"
#ifndef _WIN32
#include <sys/wait.h>
#include <signal.h>
#include <cstdlib>
#include <fstream>
#endif
#include <iostream>
#include <limits>
#include <sstream>
#include <iomanip>
#include <type_traits>
using namespace pico;
namespace {
int checks=0;
std::string context;
struct Context {
    std::string previous=context;
    explicit Context(std::string current){context=std::move(current);}
    ~Context(){context=previous;}
};
template<class T>std::string value(const T& v){std::ostringstream out;out<<std::boolalpha<<std::setprecision(17)<<v;return out.str();}
std::string value(char v){return std::to_string(static_cast<int>(v));}
template<class T>std::string value(const std::vector<T>& v){std::string out="[";for(const auto& x:v){if(out.size()>1)out+=",";
out+=value(x);}return out+"]";}
std::string one_line(std::string s){for(auto& c:s)if(c=='\n'||c=='\r')c=' ';
return s;}
struct CheckFailure:std::runtime_error{using std::runtime_error::runtime_error;};
void result(bool ok,const char* name,int line,const std::string& actual,const std::string& expected){
    ++checks;if(!ok)throw CheckFailure("live policy check "+std::to_string(checks)+" ["+one_line(name)+"]"+context+" line="+std::to_string(line)+" actual="+one_line(actual)+" expected="+one_line(expected));
}
template<class A,class B>bool same(const A& a,const B& b){
    // Avoid signed/unsigned warnings without truncating diagnostic comparisons.
    if constexpr(std::is_integral_v<A>&&std::is_integral_v<B>&&std::is_signed_v<A>!=std::is_signed_v<B>){
        if constexpr(std::is_signed_v<A>)return a>=0&&static_cast<std::make_unsigned_t<A>>(a)==b;
        else return b>=0&&a==static_cast<std::make_unsigned_t<B>>(b);
    }else return a==b;
}
template<class A,class B>void check_equal(const A& actual,const B& expected,const char* name,int line){result(same(actual,expected),name,line,value(actual),value(expected));}
template<class F>void rejection(F f,const char* name,int line){
    std::string actual="no exception";bool rejected=false;
    try{f();}catch(const std::exception& e){rejected=true;actual=std::string("std::exception: ")+e.what();}
    result(rejected,name,line,actual,"std::exception");
}
template<class F>void checked(F f,const char* name,int line,const char* expected){
    try{f();}catch(const CheckFailure&){throw;}
    catch(const std::exception& e){result(false,name,line,std::string("exception: ")+e.what(),expected);}
}
#define CHECK_EQ(actual, expected, name) checked([&]{check_equal((actual),(expected),name,__LINE__);},name,__LINE__,#expected)
#define CHECK_GE(actual, expected, name) checked([&]{const auto a_=(actual);const auto b_=(expected);result(a_>=b_,name,__LINE__,value(a_),">= "+value(b_));},name,__LINE__,">= " #expected)
#define CHECK_GT(actual, expected, name) checked([&]{const auto a_=(actual);const auto b_=(expected);result(a_>b_,name,__LINE__,value(a_),"> "+value(b_));},name,__LINE__,"> " #expected)
#define CHECK_BOOL(expr) checked([&]{check_equal(bool(expr),true,#expr,__LINE__);},#expr,__LINE__,"true")
#define rejects(...) rejection(__VA_ARGS__,#__VA_ARGS__,__LINE__)
// Log-and-continue: f must not throw; with a reason its drop counter must grow,
// without one some stderr line must be written (phase5 guard logs every time).
template<class F>bool reports(const char* reason,F f){
    const auto before=reason?live_policy_drops(reason):0;
    std::ostringstream out;auto* old=std::cerr.rdbuf(out.rdbuf());
    try{f();}catch(...){std::cerr.rdbuf(old);throw;}
    std::cerr.rdbuf(old);
    return reason?live_policy_drops(reason)>before:!out.str().empty();
}
const char* label(const char* reason){return reason?reason:"stderr line";}
#define REPORTS(reason,...) checked([&]{result(reports(reason,__VA_ARGS__),#__VA_ARGS__,__LINE__,"no log/drop",label(reason));},#__VA_ARGS__,__LINE__,"logged, no exception")
const char* const silence="Observer startup/silence reached 3s; still waiting";
const char* const exited="Observer/parent exited or cancelled; draining remaining rows";
const char* const observer_eof="Observer EOF; no further rows";
const char* const any_log=nullptr;
ImuCalibration calibration(){ImuCalibration c;c.axes={1,0,0,0,1,0,0,0,1};c.gyro_scale=.001;c.accel_scale=.01;c.axes_verified=c.time_verified=true;return c;}
std::string row(){return "{\"t_ns\":1000000000,\"devices\":[{\"index\":0,\"kind\":1,\"connected\":true,\"valid\":true,\"result\":200,\"matrix\":[1,0,0,0,0,1,0,0,0,0,1,0]}]}\n";}
struct ObserverFixture {
    std::string bytes=row();bool eof=false,alive=true,parent=true,cancel=false,throw_read=false,flood=false;
    TrackingObserver observer{{[&](size_t n){if(throw_read)throw std::runtime_error("pipe read");
        if(flood)return ObserverRead{std::string(n,'x'),false};
        auto s=bytes.substr(0,n);bytes.erase(0,s.size());return ObserverRead{s,eof};},[&]{return alive;},[&]{return parent;},[&]{return cancel;},[]{return 1000000000;}}};
    phase5::BenchTrackingGuard guard;
};
void fill(CameraPreflight& p,int bad_raw=0){
    for(int i=0;i<40;++i){int64_t t=2000000000LL+i*70000000LL;
        p.clock({t,t+(i<bad_raw?250000001:100000000),t+100000000,t+100000000,t},"boot");}
    for(const auto& sensor:{"CAMUPPERLEFT","CAMUPPERRIGHT"})for(int i=0;i<6;++i)p.exposure(sensor,2.+i/30.,.0008,3000000000LL);
}
OpticalPoseFrame pose(){OpticalPoseFrame f;f.chip=7;f.stamp=1000000000;f.connected=f.valid=true;f.quality=.8;return f;}
}
int main(){try{
    {
        const std::string elf="\177ELFsynthetic bytes for seal policy only";
        const auto sha=phase5::sha256(elf);
        CHECK_EQ(sha.size(),size_t(64),"observer seal hash size");
        verify_observer_bytes(elf,sha);
        rejects([&]{verify_observer_bytes(elf,"");});
        rejects([&]{verify_observer_bytes(elf,std::string(64,'A'));});
        rejects([&]{verify_observer_bytes(elf,std::string(64,'0'));});
        rejects([&]{verify_observer_bytes("#!/bin/sh",phase5::sha256("#!/bin/sh"));});
        rejects([&]{verify_observer_bytes("",phase5::sha256(""));});
    }
#ifndef _WIN32
    // This test executable is also a private no-argument native observer child.
    // No tracking_watch/OpenVR/hardware or scripting runtime is executed.
    if(const auto mode=std::getenv("PICO_PRIVATE_OBSERVER_FIXTURE")){
        if(std::string(mode)=="eof")return 0;
        for(;;){
            auto line=row();const auto begin=line.find("1000000000");
            timespec t{};if(clock_gettime(CLOCK_MONOTONIC,&t))return 2;
            line.replace(begin,10,std::to_string(int64_t(t.tv_sec)*1000000000+t.tv_nsec));
            std::cout<<line<<std::flush;
            timespec pause{0,10000000};nanosleep(&pause,nullptr);
        }
    }
    checked([&]{
        const auto executable=std::filesystem::canonical("/proc/self/exe");
        std::ifstream image(executable,std::ios::binary);
        const std::string image_bytes{std::istreambuf_iterator<char>(image),std::istreambuf_iterator<char>()};
        const auto reviewed_sha=phase5::sha256(image_bytes);
        {
            Context scope(" observer reviewed files");
            const auto root=std::filesystem::canonical("build")/("observer-seal-"+std::to_string(getpid()));
            struct RemoveFiles {
                std::filesystem::path root;
                ~RemoveFiles(){std::error_code ec;std::filesystem::remove_all(root,ec);}
            } remove{root};
            CHECK_EQ(std::filesystem::create_directory(root),true,"create exclusive seal test directory");
            const auto copy=root/"watch";
            const auto output=root/"proof";
            std::filesystem::copy_file(executable,copy);
            {
                ObserverLaunchFiles files(copy,output,reviewed_sha);
                files.verify();
                CHECK_GE(files.proof(),0,"retained proof descriptor");
                rejects([&]{files.verify_child(getpid());}); // copied inode is not this running image
                std::filesystem::rename(output,root/"saved-proof");
                {
                    std::ofstream replacement(output);
                    replacement<<"replacement";
                }
                rejects([&]{files.verify();});
            }
            std::filesystem::remove(output);
            {
                ObserverLaunchFiles files(copy,output,reviewed_sha);
                {
                    std::ofstream changed(copy,std::ios::app|std::ios::binary);
                    changed<<'x';
                }
                rejects([&]{files.verify();});
            }
            std::filesystem::remove(output);
            std::filesystem::remove(copy);
            std::filesystem::create_symlink(executable,copy);
            rejects([&]{ObserverLaunchFiles files(copy,output,reviewed_sha);});
            CHECK_EQ(std::filesystem::exists(output),false,"symlink executable creates no proof");
            std::filesystem::create_directory(root/"real");
            std::filesystem::create_directory_symlink(root/"real",root/"alias");
            rejects([&]{ObserverLaunchFiles files(executable,root/"alias"/"proof",reviewed_sha);});
            rejects([&]{ObserverLaunchFiles files(executable,root/"real"/".."/"proof",reviewed_sha);});
            CHECK_EQ(std::filesystem::exists(root/"real"/"proof"),false,"symlink parent creates no proof");
        }
        const auto proof=std::filesystem::absolute("build/observer-launch-"+std::to_string(getpid())+".jsonl");
        struct Cleanup {
            std::shared_ptr<ObserverProcess> child;
            std::filesystem::path proof;
            ~Cleanup(){if(child)try{child->close();}catch(...){}unsetenv("PICO_PRIVATE_OBSERVER_FIXTURE");
                std::error_code ec;std::filesystem::remove(proof,ec);}
        } cleanup{{},proof};
        CHECK_EQ(setenv("PICO_PRIVATE_OBSERVER_FIXTURE","rows",1),0,"setenv(\"PICO_PRIVATE_OBSERVER_FIXTURE\",\"rows\",1)==0");
        rejects([&]{ObserverProcess bad_seal(executable,proof,std::string(64,'0'));});
        CHECK_EQ(std::filesystem::exists(proof),false,"bad observer seal creates no proof or child");
        cleanup.child=std::make_shared<ObserverProcess>(executable,proof,reviewed_sha);
        CHECK_EQ(cleanup.child->argv(),std::vector<std::string>{executable.string()},"cleanup.child->argv()==std::vector<std::string>{executable.string()}");
        auto hooks=cleanup.child->hooks([]{return false;});
        TrackingObserver observer(hooks);phase5::BenchTrackingGuard guard;
        bool ready=false;
        for(int i=0;i<100&&!ready;++i){observer.poll(guard);
            // guard.check only logs now (owner rule), so wait for the observer's first proof row instead.
            guard.check(hooks.now(),true,true);
            {std::ifstream probe(proof);std::string row;ready=bool(std::getline(probe,row));}
            if(!ready){timespec pause{0,10000000};nanosleep(&pause,nullptr);}
        }
        CHECK_BOOL(ready);std::ifstream in(proof);std::string line;CHECK_BOOL(bool(std::getline(in,line)));
        CHECK_GT(phase5::number(phase5::Json(line).parse(),"t_ns"),0,"observer proof timestamp");
        rejects([&]{ObserverProcess duplicate(executable,proof,reviewed_sha);});
        cleanup.child->close();CHECK_EQ((hooks.child_alive()),false,"!hooks.child_alive()");
        REPORTS(exited,[&]{observer.poll(guard);});CHECK_EQ(observer.alive(),false,"closed child reported, not thrown");
        cleanup.child->close(); // idempotent reaping, no PID reuse signal.
        int status=0;const auto reaped=waitpid(-1,&status,WNOHANG);const auto reap_errno=errno;
        CHECK_EQ(reaped,-1,"no remaining child: waitpid");CHECK_EQ(reap_errno,ECHILD,"no remaining child: errno");
    },"Linux sealed observer launch",__LINE__,"reviewed direct child, retained proof, clean reap");
#endif
    {int64_t now=1;std::string bytes;
     TrackingObserver o({[&](size_t){auto b=bytes;bytes.clear();return ObserverRead{b,false};},[]{return true;},[]{return true;},[]{return false;},[&]{return now;}});
     phase5::BenchTrackingGuard g;o.poll(g);now=3000000000LL;
     const auto quiet=live_policy_drops(silence);o.poll(g);CHECK_EQ(live_policy_drops(silence),quiet,"silence below 3s not logged");++now;
     // 3s silence is logged and the observer keeps waiting for rows.
     REPORTS(silence,[&]{o.poll(g);});bytes=row();REPORTS(silence,[&]{o.poll(g);});
     CHECK_BOOL(g.has_sample());now=3000000002LL;CHECK_EQ(live_policy_drops(silence)-quiet,uint64_t(2),"row ends the silence episode");
     const auto after=live_policy_drops(silence);o.poll(g);CHECK_EQ(live_policy_drops(silence),after,"fresh row resets silence");}
    {int64_t now=1000000000;std::string bytes=row();
     TrackingObserver o({[&](size_t){auto b=bytes;bytes.clear();return ObserverRead{b,false};},[]{return true;},[]{return true;},[]{return false;},[&]{return now;}});
     phase5::BenchTrackingGuard g;o.poll(g);now+=3000000000LL;REPORTS(silence,[&]{o.poll(g);});
     now-=1;REPORTS("Observer clock regressed; skipping poll",[&]{o.poll(g);});}
#ifndef _WIN32
    // Private real pipe and pidfd children only. No live observer is launched.
    for(int fault=0;fault<5;++fault){Context test_context(" fault="+std::to_string(fault));
        struct PrivateChild {
            int fds[2]{-1,-1},ready[2]{-1,-1};pid_t pid=-1;int child=-1,parent=-1;
            ~PrivateChild(){if(pid>0){kill(pid,SIGKILL);while(waitpid(pid,nullptr,0)<0&&errno==EINTR){}}
                for(auto fd:{fds[0],fds[1],ready[0],ready[1],child,parent})if(fd>=0)::close(fd);}
        } c;
        CHECK_EQ(pipe2(c.fds,O_NONBLOCK|O_CLOEXEC),0,"pipe2(c.fds,O_NONBLOCK|O_CLOEXEC)==0");
        CHECK_EQ(pipe2(c.ready,O_CLOEXEC),0,"fixture readiness pipe");
        c.pid=fork();CHECK_GE(c.pid,0,"fixture fork pid");if(c.pid==0){::close(c.fds[0]);::close(c.fds[1]);::close(c.ready[0]);
            const char ready='R';if(write(c.ready[1],&ready,1)!=1)_exit(2);
            ::close(c.ready[1]);for(;;)pause();}
        // fork inherits the writer even with O_CLOEXEC (there is no exec).
        // EOF is valid only once the child confirms its inherited writer closed.
        ::close(c.ready[1]);c.ready[1]=-1;char ready=0;ssize_t received;
        pollfd readiness{c.ready[0],POLLIN,0};int signalled;
        do{signalled=::poll(&readiness,1,2000);}while(signalled<0&&errno==EINTR);
        CHECK_EQ(signalled,1,"child pipe closure handshake within 2s");
        do{received=read(c.ready[0],&ready,1);}while(received<0&&errno==EINTR);
        CHECK_EQ(received,1,"child closed inherited pipe ends: readiness byte count");
        CHECK_EQ(ready,'R',"child closed inherited pipe ends: readiness token");
        ::close(c.ready[0]);c.ready[0]=-1;
        c.child=int(syscall(SYS_pidfd_open,c.pid,0));c.parent=int(syscall(SYS_pidfd_open,getpid(),0));
        CHECK_GE(c.child,0,"child pidfd");CHECK_GE(c.parent,0,"parent pidfd");
        if(fault==4){rejects([&]{ObserverPipe bad(c.fds[0],c.fds[0],c.parent);});continue;}
        auto hooks=ObserverPipe::hooks(c.fds[0],c.child,c.parent,[]{return 1000000000;},[]{return false;});
        TrackingObserver o(hooks);phase5::BenchTrackingGuard g;
        // The adapter owns independent nonblocking flags, not dup-shared flags.
        CHECK_EQ(fcntl(c.fds[0],F_SETFL,0),0,"fcntl(c.fds[0],F_SETFL,0)==0");o.poll(g);
        if(fault==0){auto s=row();CHECK_EQ(write(c.fds[1],s.data(),20),20,"write(c.fds[1],s.data(),20)==20");o.poll(g);
            CHECK_EQ(write(c.fds[1],s.data()+20,s.size()-20),ssize_t(s.size()-20),"write(c.fds[1],s.data()+20,s.size()-20)==ssize_t(s.size()-20)");o.poll(g);CHECK_BOOL(g.check(1000000000,true,true));}
        // Original Linux check #24: EOF after the final writer closes.
        // EOF after the final writer closes is logged specifically as EOF, never thrown.
        if(fault==1){::close(c.fds[1]);c.fds[1]=-1;REPORTS(observer_eof,[&]{o.poll(g);});}
        if(fault==2||fault==3){kill(c.pid,SIGKILL);while(waitpid(c.pid,nullptr,0)<0&&errno==EINTR){}c.pid=-1;
            if(fault==3)hooks.parent_alive=hooks.child_alive;
            TrackingObserver exited_child(hooks);REPORTS(exited,[&]{exited_child.poll(g);});
            CHECK_EQ(exited_child.alive(),false,"exited child is reported by alive()");}
    }
#endif
    // Loader Debugger argv/listener refusal, extended with causal child identity.
    ListenerIdentity launch;launch.child=launch.owner=42;launch.child_start=launch.observed_start=123;
    launch.socket_inode=9;launch.address="127.0.0.1";launch.port=6666;launch.matching_listeners=1;
    launch.argv=openocd_argv();launch.executable_seal="exe-sha";launch.dependency_seals={"config-sha","included-tcl-sha"};
    verify_listener_identity(launch,launch);CHECK_BOOL(true);
    for(int fault=0;fault<12;++fault){Context test_context(" fault="+std::to_string(fault));auto e=launch;
        switch(fault){case 0:e.owner=99;break;case 1:e.observed_start++;break;case 2:e.socket_inode=0;break;
        case 3:e.matching_listeners=2;break;case 4:e.address="0.0.0.0";break;case 5:e.port=6667;break;
        case 6:e.argv[4]+="; halt";break;case 7:e.executable_seal="changed";break;case 8:e.dependency_seals.pop_back();break;
        case 9:e.child=43;break;case 10:e.child_start++;break;case 11:e.dependency_seals[1]="changed";break;}
        rejects([&]{verify_listener_identity(e,launch);});}
    {auto l=launch;l.dependency_seals.clear();rejects([&]{verify_listener_identity(launch,l);});}
    // controller_guard._read: bounded lines, every sample, no ignored EOF.
    {ObserverFixture f;f.observer.poll(f.guard);CHECK_BOOL(f.guard.check(1000000000,true,true));}
    {ObserverFixture f;auto complete=f.bytes;f.bytes=complete.substr(0,20);f.observer.poll(f.guard);
      CHECK_EQ(f.guard.check(1000000000,true,true),false,"partial row is not a sample");f.bytes=complete.substr(20);f.observer.poll(f.guard);CHECK_BOOL(f.guard.check(1000000000,true,true));}
    // Every observer fault is logged and the next poll keeps using rows.
    for(int fault=0;fault<8;++fault){Context test_context(" fault="+std::to_string(fault));ObserverFixture f;
        const char* reason=any_log;
        switch(fault){case 0:f.eof=true;reason=observer_eof;break;case 1:f.alive=false;reason=exited;break;case 2:f.parent=false;reason=exited;break;
        case 3:f.cancel=true;reason=exited;break;case 4:f.throw_read=true;reason="Observer read failed; retrying next poll";break;
        case 5:f.bytes="{}\n";break;case 6:f.flood=true;reason="Observer output flood; continuing next poll";break;case 7:f.bytes=row()+row();break;}
        REPORTS(reason,[&]{f.observer.poll(f.guard);});f.eof=false;f.alive=f.parent=true;f.cancel=f.throw_read=f.flood=false;
        f.bytes=fault==6?"\n"+row():row(); // a flooded line is skipped up to its newline
        f.observer.poll(f.guard);CHECK_BOOL(f.guard.check(1000000000,true,true));CHECK_BOOL(f.observer.alive());}
    {ObserverFixture f;f.bytes=std::string(65537,'x');REPORTS("Observer line exceeds 65536; dropping line",[&]{f.observer.poll(f.guard);});
      CHECK_EQ(f.guard.has_sample(),false,"overlong line is not a sample");
      f.bytes="\n"+row();f.observer.poll(f.guard);CHECK_BOOL(f.guard.check(1000000000,true,true));}
    {ObserverFixture f;f.bytes="partial";f.observer.poll(f.guard);f.eof=true;
      REPORTS("Observer partial final row; dropped",[&]{f.observer.poll(f.guard);});CHECK_EQ(f.guard.has_sample(),false,"partial final row dropped");}
    {ObserverFixture f;auto bad=row();auto at=bad.find("\"kind\":1");bad.replace(at,8,"\"kind\":2");
      f.bytes=bad+row();REPORTS(any_log,[&]{f.observer.poll(f.guard);});
      CHECK_EQ(f.guard.check(1000000000,true,true),false,"unhealthy row cannot be hidden by latest-only ingestion");}
    {ObserverFixture f;auto h=ObserverHooks{[&](size_t){f.parent=false;return ObserverRead{};},[]{return true;},[&]{return f.parent;},[]{return false;},[]{return 1000000000;}};
      TrackingObserver o(h);REPORTS("Observer supervision changed during read",[&]{o.poll(f.guard);});}
    // measured_pipeline.configure_imu/_propagate exact boundaries and no commit.
    for(int fault=0;fault<11;++fault){Context test_context(" fault="+std::to_string(fault));auto c=calibration();switch(fault){case 0:c.axes_verified=false;break;case 1:c.time_verified=false;break;
        case 2:c.axes[0]=-1;break;case 3:c.axes[0]=1.001;break;case 4:c.axes[0]=std::numeric_limits<double>::quiet_NaN();break;
        case 5:c.gyro_scale=.01;break;case 6:c.accel_scale=.1;break;case 7:c.bias={50,0,0};break;case 8:c.offset=25000001;break;
        case 9:c.offset=-25000001;break;case 10:c.bias[0]=std::numeric_limits<double>::infinity();break;}
        // Rejected calibration is logged; samples stay unadmitted, the run continues.
        const char* const reasons[11]={"Unmeasured IMU calibration","Unmeasured IMU calibration","Reflected IMU axes",
            "Nonorthogonal IMU axes","Nonfinite IMU axes","Unmeasured IMU calibration","Unmeasured IMU calibration",
            "IMU gyro bias exceeds bound","Unmeasured IMU calibration","Unmeasured IMU calibration","Nonfinite IMU bias"};
        ImuAdmission a;a.configure(calibration());REPORTS(reasons[fault],[&]{a.configure(c);});a.optical_anchor(1000000000,1000000000);
        CHECK_EQ((a.sample(1020000000,1020000000,{0,0,0},{0,0,0},.01)),false,"rejected calibration revokes admission");}
    {ImuAdmission a;a.configure(calibration());
      REPORTS("Invalid IMU optical anchor; dropped",[&]{a.optical_anchor(1000000000,1250000001);});
      CHECK_EQ((a.sample(1020000000,1020000000,{0,0,0},{0,0,0},.01)),false,"dropped anchor admits nothing");
      a.optical_anchor(1000000000,1000000000);CHECK_BOOL(a.sample(1020000000,1020000000,{0,0,0},{0,0,0},.01));
      REPORTS("Invalid IMU optical anchor; dropped",[&]{a.optical_anchor(1000000000,1000000000);});
      CHECK_EQ(a.state(),1020000000,"non-advancing anchor keeps state");}
    {ImuAdmission a;a.configure(calibration());a.optical_anchor(1000000000,1000000000);
      CHECK_BOOL(a.sample(1025000000,1025000000,{35000,0,0},{32000,0,0},.03));CHECK_EQ(a.state(),1025000000,"a.state()==1025000000");
      CHECK_EQ((a.sample(1025000000,1025000000,{0,0,0},{0,0,0},.01)),false,"!a.sample(1025000000,1025000000,{0,0,0},{0,0,0},.01)");
      CHECK_EQ((a.sample(1050000001,1050000001,{0,0,0},{0,0,0},.01)),false,"!a.sample(1050000001,1050000001,{0,0,0},{0,0,0},.01)");CHECK_EQ(a.state(),1025000000,"a.state()==1025000000");
      CHECK_EQ((a.sample(1050000000,1050000000,{0,0,0},{0,0,0},.030001)),false,"!a.sample(1050000000,1050000000,{0,0,0},{0,0,0},.030001)");CHECK_EQ(a.state(),1025000000,"a.state()==1025000000");
      CHECK_EQ((a.sample(1050000000,1050000000,{0,0,0},{0,0,0},.01)),false,"!a.sample(1050000000,1050000000,{0,0,0},{0,0,0},.01)");
      a.optical_anchor(1100000000,1100000000);CHECK_BOOL(a.sample(1120000000,1120000000,{0,0,0},{0,0,0},.01));
      a.reconnect();CHECK_EQ((a.sample(1125000000,1125000000,{0,0,0},{0,0,0},.01)),false,"!a.sample(1125000000,1125000000,{0,0,0},{0,0,0},.01)");
      a.optical_anchor(1200000000,1200000000);CHECK_EQ((a.sample(1220000000,1220000000,{0,0,0},{0,0,0},.01)),false,"!a.sample(1220000000,1220000000,{0,0,0},{0,0,0},.01)");
      a.set_time(true);CHECK_BOOL(a.sample(1220000000,1220000000,{0,0,0},{0,0,0},.01));a.invalidate();
      CHECK_EQ((a.sample(1240000000,1240000000,{0,0,0},{0,0,0},.01)),false,"!a.sample(1240000000,1240000000,{0,0,0},{0,0,0},.01)");}
    for(int fault=0;fault<7;++fault){Context test_context(" fault="+std::to_string(fault));ImuAdmission a;a.configure(calibration());a.optical_anchor(1000000000,1000000000);
      int64_t stamp=1020000000,now=1020000000;std::array<double,3> g{},acc{};double sigma=.01;
      switch(fault){case 0:stamp=1000000000;break;case 1:now=1019999999;break;case 2:now=1250000001;break;
      case 3:g[0]=35001;break;case 4:acc[0]=32001;break;case 5:g[0]=std::numeric_limits<double>::quiet_NaN();break;
      case 6:sigma=std::numeric_limits<double>::quiet_NaN();break;}
      CHECK_EQ((a.sample(stamp,now,g,acc,sigma)),false,"!a.sample(stamp,now,g,acc,sigma)");CHECK_EQ(a.state(),1000000000,"a.state()==1000000000");}
    // station_optics._measure_clock and profile_owner.preflight from actual rows.
    {CameraPreflight p("boot","dataset","rig/factory-seals",1000000000);fill(p,4);auto r=p.finish(5000000000,"boot",true,false);
      CHECK_EQ(r.causal_fraction[0],.9,"r.causal_fraction[0]==.9");CHECK_BOOL(r.ambiguous);CHECK_EQ(r.boot,"boot","r.boot==\"boot\"");
      CHECK_BOOL(r.complete);}
    // Incomplete/failed preflight measurements are logged and returned as incomplete, never thrown.
    {CameraPreflight p("boot","dataset","rig",1000000000);fill(p,5);CameraPreflightResult r;
      REPORTS("Current boot RAW causal fraction below .9",[&]{r=p.finish(5000000000,"boot",true,false);});
      CHECK_EQ(r.complete,false,"low causal fraction incomplete");CHECK_EQ(r.causal_fraction[0],.875,"best available fraction");}
    for(int fault=0;fault<4;++fault){Context test_context(" fault="+std::to_string(fault));CameraPreflight p("boot","dataset","rig",1000000000);fill(p);
      CameraPreflightResult r;
      REPORTS("Camera preflight incomplete",[&]{r=p.finish(fault==0?13000000001LL:5000000000,fault==1?"other":"boot",fault!=2,fault==3);});
      CHECK_EQ(r.complete,false,"faulted preflight incomplete");CHECK_EQ(r.causal_fraction[0],1.,"measured fraction kept");}
    {CameraPreflight p("boot","dataset","rig",1000000000);CameraPreflightResult r;
      REPORTS("Camera preflight incomplete",[&]{r=p.finish(5000000000,"boot",true,false);});
      CHECK_EQ(r.complete,false,"empty preflight incomplete");CHECK_EQ(r.causal_fraction[0],0.,"no reads, no division");}
    {CameraPreflight p("boot","dataset","rig",1000000000);
      REPORTS("Camera clock boot/order/window invalid; read dropped",[&]{p.clock({1,2,3,4,2000000000},"other");});
      fill(p);CHECK_BOOL(p.finish(5000000000,"boot",true,false).complete);}
    for(double cadence:{.031999,.035001,.066666}){Context test_context(" cadence="+value(cadence));CameraPreflight p("boot","dataset","rig",1000000000);
      p.exposure("CAMUPPERLEFT",1.,.0008,2000000000);
      REPORTS("Camera cadence omission/change; continuing from this exposure",[&]{p.exposure("CAMUPPERLEFT",1.+cadence,.0008,2000000000);});
      const auto quiet=live_policy_drops("Camera cadence omission/change; continuing from this exposure");
      p.exposure("CAMUPPERLEFT",1.+cadence+1./30,.0008,2000000000);
      CHECK_EQ(live_policy_drops("Camera cadence omission/change; continuing from this exposure"),quiet,"cadence resumes from the omitted exposure");}
    for(double exposure:{.000749,.000851,std::numeric_limits<double>::quiet_NaN()}){Context test_context(" exposure="+value(exposure));CameraPreflight p("boot","dataset","rig",1000000000);
      REPORTS("Wrong upper-camera exposure; dropped",[&]{p.exposure("CAMUPPERRIGHT",1.,exposure,2000000000);});}
    // measured_pipeline.expire: >=250ms, 100ms repeat and connection changes.
    {std::vector<OpticalPoseFrame> sent;PublicationExpiry p([&](const OpticalPoseFrame& f,int64_t){sent.push_back(f);return true;});
      p.optical(pose(),1000000000);p.poll(1249999999,true,true);CHECK_EQ(sent.size(),1,"sent.size()==1");
      p.poll(1250000000,true,true);CHECK_EQ(sent.size(),2,"sent.size()==2");CHECK_EQ((sent.back().valid),false,"!sent.back().valid");CHECK_EQ(sent.back().stamp,1250000000,"sent.back().stamp==1250000000");
      p.poll(1349999999,true,true);CHECK_EQ(sent.size(),2,"sent.size()==2");p.poll(1350000000,true,true);CHECK_EQ(sent.size(),3,"sent.size()==3");
      p.poll(1350000001,false,true);CHECK_EQ(sent.size(),4,"sent.size()==4");CHECK_EQ((sent.back().connected),false,"!sent.back().connected");
      p.poll(1350000002,true,true);CHECK_EQ(sent.size(),5,"sent.size()==5");CHECK_EQ((sent.back().valid),false,"!sent.back().valid");CHECK_EQ(sent.back().sequence,5,"sent.back().sequence==5");}
    // A failed send is logged, never latched: the invalid status is retried by the next poll.
    {int n=0;bool accept_invalid=false;
      PublicationExpiry p([&](const OpticalPoseFrame& f,int64_t){++n;return f.valid||accept_invalid;});p.optical(pose(),1000000000);
      REPORTS("Publication send failed; retrying with next frame",[&]{p.poll(1000000001,true,false);});CHECK_EQ(n,2,"invalid status attempted");
      p.poll(1000000002,true,true);CHECK_EQ(n,2,"fresh allowed pose needs no status");
      accept_invalid=true;p.poll(1000000003,true,false);CHECK_EQ(n,3,"invalid status retried");
      p.poll(1000000004,true,false);CHECK_EQ(n,3,"sent status repeats only after 100ms");}
    {int n=0;std::vector<OpticalPoseFrame> sent;
      PublicationExpiry p([&](const OpticalPoseFrame& f,int64_t)->bool{if(++n==1)throw std::runtime_error("UDP fault");
      sent.push_back(f);return true;});
      REPORTS("Publication send failed; retrying with next frame",[&]{p.optical(pose(),1000000000);});
      p.poll(1000000001,true,true);CHECK_EQ(sent.size(),size_t(0),"unsent pose arms no expiry");
      auto next=pose();next.stamp=1033333333;p.optical(next,1033333334);
      CHECK_EQ(sent.size(),size_t(1),"next pose published after a UDP fault");CHECK_EQ(sent.back().sequence,uint64_t(2),"sequence continues");}
    {std::vector<OpticalPoseFrame> sent;PublicationExpiry p([&](const OpticalPoseFrame& f,int64_t){sent.push_back(f);return true;});
      p.optical(pose(),1000000000);
      REPORTS("Invalid publication anchor/peer; pose dropped",[&]{p.optical(pose(),1000000001);}); // non-advancing stamp
      auto other=pose();other.chip=8;other.stamp=1033333333;
      REPORTS("Invalid publication anchor/peer; pose dropped",[&]{p.optical(other,1033333334);});
      REPORTS("Publication clock regressed; poll skipped",[&]{p.poll(999999999,true,true);});
      CHECK_EQ(sent.size(),size_t(1),"only the admitted pose was sent");}
    std::cout<<"live policy "<<checks<<" checks PASS\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}

