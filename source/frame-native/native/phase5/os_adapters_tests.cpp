#include "os_adapters.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>
using namespace phase5;
namespace {
void check(bool value,const char* why){if(!value)throw std::runtime_error(why);}
template<class F> void reports(F work,const char* message){
    std::ostringstream log;auto* previous=std::cerr.rdbuf(log.rdbuf());
    try{work();}catch(...){std::cerr.rdbuf(previous);throw;}
    std::cerr.rdbuf(previous);
    check(log.str().find(message)!=std::string::npos,"Expected diagnostic and continuation");
}
}
int main(){try{
    char directory[]="/tmp/pico-adapter-tests-XXXXXX";
    check(mkdtemp(directory)!=nullptr,"Private temporary fixture");
    const std::filesystem::path root(directory);
    OsAdapters os((root/"owner.lock").string());
    auto files=os.files();
    const auto settings=(root/"settings.json").string();
    files.exclusive(settings,"original");
    check(chmod(settings.c_str(),0640)==0,"Set original mode");
    files.compare_replace(settings,"original","profile");
    check(files.read(settings)==std::optional<std::string>("profile"),"Apply settings");
    struct stat st{};check(stat(settings.c_str(),&st)==0&&(st.st_mode&0777)==0640,"Preserve mode");
    reports([&]{files.compare_replace(settings,"foreign","overwrite");},"Concurrent settings change");
    check(files.read(settings)==std::optional<std::string>("profile"),"Skip conflicting mutation");
    files.compare_replace(settings,"profile","original");
    files.compare_replace(settings,"original",{});
    check(!files.read(settings),"Restore original absence");
    files.exclusive(settings,"same");
    OsAdapters::FaultHooks faults;
    faults.before_settings_mutation=[&]{
        files.exclusive(settings+".other","same");
        std::filesystem::rename(settings+".other",settings);
    };
    OsAdapters raced((root/"raced.lock").string(),faults);
    reports([&]{raced.files().compare_replace(settings,"same","changed");},"Settings inode/metadata changed");
    check(files.read(settings)==std::optional<std::string>("same"),"Skip raced settings");
    auto runtime=os.runtime("/bin/true");
    reports([&]{runtime.terminate_native(getpid());},"Unbound/replaced");
    reports([&]{check(runtime.power("/absent-power-tool",{},1)=="{}","Unavailable power report");},"Power cap");
    OsAdapters::ScanHooks hooks;hooks.include_process=[](int64_t){return false;};
    auto empty=os.runtime("/bin/true","/proc",hooks);
    check(empty.managed().empty(),"Empty process scan allowed");
    reports([&]{restart_native(empty);},"Native XRService count 0");
    {
        // Regression 2026-10-30: an XRService that ignores SIGTERM stayed next to its replacement.
        Runtime hung;int64_t now=0;std::vector<ManagedProcess> ps{{100,60},{101,60}};int killed=0,termed=0;
        hung.clock.now=[&]{return now;};hung.clock.sleep=[&](int64_t ns){now+=ns;};
        hung.managed=[&]{return ps;};
        hung.terminate_native=[&](int64_t pid){++termed;if(pid==101)ps.erase(ps.begin()+1);};
        hung.kill_native=[&](int64_t pid){++killed;check(pid==100,"Only the hung XRService is killed");ps={{102,1}};};
        restart_native(hung);
        check(termed==2&&killed==1&&ps.size()==1&&ps.front().pid==102,"Hung and duplicate XRService retired");
    }
    os.files_hardened().writer_exclusion();
    auto child=os.spawn({"/bin/true"});
    for(int i=0;i<1000&&!child.poll();++i)usleep(1000);
    check(child.poll()==std::optional<int>(0),"Real child exits normally");
    std::cout<<"OS adapter continuation tests PASS\n";
    return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 1;}}