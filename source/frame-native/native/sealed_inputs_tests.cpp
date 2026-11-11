#include "native_test_runtime.hpp"
#include "sealed_inputs.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#ifndef _WIN32
#include <sys/mman.h>
#endif
namespace {
int checks=0;
void check(bool ok,const char* name,int line,const std::string& actual,const std::string& expected){
    ++checks;
    if(!ok)throw std::runtime_error(std::string(name)+" line="+std::to_string(line)+" actual="+actual+" expected="+expected);
}
#define REQUIRE(expr) do{const bool actual_=bool(expr);check(actual_,#expr,__LINE__,actual_?"true":"false","true");}while(false)
template<class F>void rejects(F f,const char* name,int line){
    bool caught=false;
    try{f();}catch(const std::exception&){caught=true;}
    check(caught,name,line,caught?"exception":"no exception","exception");
}
#define REJECT(...) rejects(__VA_ARGS__,#__VA_ARGS__,__LINE__)
}
int main(){try{
    using namespace pico;
    const auto sha=phase5::sha256("config");
    std::vector<ReviewedInput> manifest{{"/entry",sha,true,{"/config"}},{"/config",sha,false,{}}};
    validate_input_graph(manifest,"/entry");
    REQUIRE(manifest.size()==2);
    for(int fault=0;fault<10;++fault){
        auto bad=manifest;
        switch(fault){
        case 0:bad.clear();break;
        case 1:bad.push_back(bad[0]);break;
        case 2:bad[0].executable=false;break;
        case 3:bad[0].sha256=std::string(64,'A');break;
        case 4:bad[0].dependencies={"/absent"};break;
        case 5:bad[0].dependencies.clear();break;
        case 6:bad[0].dependencies={"/config","/config"};break;
        case 7:bad[0].path="relative";break;
        case 8:bad[1].path+=std::string(1,'\0');break;
        case 9:bad.resize(257);break;
        }
        REJECT([&]{validate_input_graph(bad,"/entry");});
    }
    // Cyclic shared-library graphs are legal; every referenced node must exist.
    manifest[1].dependencies={"/entry"};
    validate_input_graph(manifest,"/entry");
    REQUIRE(manifest.size()==2);
    const auto anchor=input_graph_sha256(manifest,"/entry");
    verify_input_graph(manifest,"/entry",anchor);
    auto reordered=manifest;
    std::reverse(reordered.begin(),reordered.end());
    REQUIRE(input_graph_sha256(reordered,"/entry")==anchor);
    REJECT([&]{verify_input_graph(manifest,"/entry","");});
    REJECT([&]{verify_input_graph(manifest,"/entry",std::string(64,'A'));});
    for(int fault=0;fault<4;++fault){
        auto changed=manifest;
        switch(fault){
        case 0:changed[1].sha256=phase5::sha256("other");break;
        case 1:changed[1].executable=true;break;
        case 2:changed[1].dependencies.clear();break;
        case 3:changed[0].path="/new-entry";break;
        }
        REJECT([&]{verify_input_graph(changed,"/entry",anchor);});
    }
    for(const auto& path:{"/a/../entry","/a/./entry","/entry/.","/entry/..","//entry","/entry/"}){
        auto bad=manifest;
        bad[0].path=path;
        REJECT([&]{validate_input_graph(bad,path);});
    }
#ifndef _WIN32
    const auto root=std::filesystem::canonical("build")/("sealed-inputs-"+std::to_string(getpid()));
    struct Cleanup {
        std::filesystem::path root;
        ~Cleanup(){std::error_code ec;std::filesystem::remove_all(root,ec);}
    } cleanup{root};
    REQUIRE(std::filesystem::create_directory(root));
    const auto source=root/"input";
    {std::ofstream out(source,std::ios::binary);out<<"config";}
    // Archive-restored old mtime is legal. Reading may update atime under
    // relatime; neither atime nor equality with the new snapshot is required.
    const struct timespec old_times[2]={{1,0},{1,0}};
    REQUIRE(utimensat(AT_FDCWD,source.c_str(),old_times,0)==0);
    SealedInput sealed(source,sha,false);
    REQUIRE(owned_files::read(sealed.descriptor(),64)=="config");
    REQUIRE(fcntl(sealed.descriptor(),F_GETFD)&FD_CLOEXEC);
    const auto fd=sealed.descriptor();
    const int seals=fcntl(fd,F_GET_SEALS);
    REQUIRE((seals&(F_SEAL_WRITE|F_SEAL_GROW|F_SEAL_SHRINK|F_SEAL_SEAL))==(F_SEAL_WRITE|F_SEAL_GROW|F_SEAL_SHRINK|F_SEAL_SEAL));
    REQUIRE(pwrite(fd,"x",1,0)==-1);
    sealed.verify();
    REQUIRE(ftruncate(fd,0)==-1);
    sealed.verify();
    REQUIRE(ftruncate(fd,7)==-1);
    sealed.verify();
    REQUIRE(fcntl(fd,F_ADD_SEALS,F_SEAL_WRITE)==-1);
    // Deterministic regression: content seals deliberately do not seal times.
    // The old full-stat comparator rejects this unchanged immutable snapshot.
    struct stat snapshot_before{},snapshot_after{};
    REQUIRE(fstat(fd,&snapshot_before)==0);
    REQUIRE(futimens(fd,old_times)==0);
    REQUIRE(fstat(fd,&snapshot_after)==0);
    REQUIRE(!owned_files::same(snapshot_before,snapshot_after));
    REQUIRE(sealed_identity_changes(snapshot_before,snapshot_after).empty());
    REQUIRE(sealed_identity_changes(snapshot_before,snapshot_after,true).find("st_mtim.tv_sec before=")!=std::string::npos);
    sealed.verify();
    {owned_files::Fd reopened(open(sealed.exec_path().c_str(),O_RDONLY|O_CLOEXEC));
     REQUIRE(reopened.value>=0);
     REQUIRE(owned_files::read(reopened.value,64)=="config");
     struct stat reopened_identity{};
     REQUIRE(fstat(reopened.value,&reopened_identity)==0);
     REQUIRE(sealed_identity_changes(snapshot_after,reopened_identity).empty());}
    void* mapping=mmap(nullptr,6,PROT_READ,MAP_SHARED,fd,0);
    REQUIRE(mapping!=MAP_FAILED);
    const bool mapped_content=std::string(static_cast<const char*>(mapping),6)=="config";
    REQUIRE(munmap(mapping,6)==0);
    REQUIRE(mapped_content);
    REQUIRE(mmap(nullptr,6,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0)==MAP_FAILED);
    sealed.verify();
    // Metadata policy remains strict and reports the actual field and values.
    REQUIRE(fchmod(fd,0600)==0);
    bool mode_diagnostic=false;
    try{sealed.verify();}catch(const std::exception& e){
        mode_diagnostic=std::string(e.what()).find("st_mode before="+
            std::to_string(S_IFREG|0400)+" after="+std::to_string(S_IFREG|0600))!=std::string::npos;
    }
    REQUIRE(mode_diagnostic);
    REQUIRE(fchmod(fd,0400)==0);
    sealed.verify();
    for(int field=0;field<7;++field){
        auto changed=snapshot_after;
        const char* name="";
        switch(field){
        case 0:++changed.st_dev;name="st_dev";break;
        case 1:++changed.st_ino;name="st_ino";break;
        case 2:++changed.st_size;name="st_size";break;
        case 3:++changed.st_mode;name="st_mode";break;
        case 4:++changed.st_nlink;name="st_nlink";break;
        case 5:++changed.st_uid;name="st_uid";break;
        case 6:++changed.st_gid;name="st_gid";break;
        }
        const auto diagnostic=sealed_identity_changes(snapshot_after,changed);
        REQUIRE(diagnostic.find(std::string(name)+" before=")==0);
        REQUIRE(diagnostic.find(" after=")!=std::string::npos);
        REQUIRE(diagnostic.find(';')==std::string::npos);
    }
    {std::ofstream out(source,std::ios::binary|std::ios::trunc);out<<"changed";}
    sealed.verify();
    REQUIRE(owned_files::read(fd,64)=="config");
    REJECT([&]{SealedInput bad(source,sha,false);});
    REJECT([&]{SealedInput bad(source,phase5::sha256("changed"),true);});
    std::filesystem::remove(source);
    sealed.verify();
    REQUIRE(owned_files::read(fd,64)=="config");
    std::filesystem::create_symlink("absent",source);
    REJECT([&]{SealedInput bad(source,sha,false);});
    std::filesystem::remove(source);
    REQUIRE(mkfifo(source.c_str(),0600)==0);
    REJECT([&]{SealedInput bad(source,sha,false);});
    std::filesystem::remove(source);
    {std::ofstream out(source,std::ios::binary);out<<"config";}
    std::filesystem::create_hard_link(source,root/"hardlink");
    REJECT([&]{SealedInput bad(source,sha,false);});
    std::filesystem::remove(root/"hardlink");
    std::filesystem::create_directory_symlink(root,root/"alias");
    REJECT([&]{SealedInput bad(root/"alias"/"input",sha,false);});
    REJECT([&]{SealedInput bad(root/"alias"/".."/"input",sha,false);});
    REJECT([&]{sealed.verify_child(getpid());});
    // Native image identity must be the immutable snapshot, not the source.
    auto executable=std::filesystem::canonical("/proc/self/exe");
    std::ifstream in(executable,std::ios::binary);
    std::string image{std::istreambuf_iterator<char>(in),std::istreambuf_iterator<char>()};
    const auto copy=root/"executable";
    std::filesystem::copy_file(executable,copy);
    const std::vector<ReviewedInput> bundle_manifest{{copy.string(),phase5::sha256(image),true,{source.string()}},
                                                    {source.string(),sha,false,{}}};
    // Test-only anchor: production must load this from independent review.
    SealedInputs bundle(bundle_manifest,copy.string(),input_graph_sha256(bundle_manifest,copy.string()));
    REQUIRE(bundle.at(copy.string()).size()==image.size());
    REQUIRE(owned_files::read(bundle.at(source.string()).descriptor(),64)=="config");
    REJECT([&]{bundle.at("/unlisted");});
    {owned_files::Fd oversized(openat(AT_FDCWD,(root/"large").c_str(),O_RDWR|O_CREAT|O_EXCL|O_CLOEXEC,0600));
     REQUIRE(oversized.value>=0);
     REQUIRE(ftruncate(oversized.value,64*1024*1024+1)==0);}
    REJECT([&]{SealedInput bad(root/"large",sha,false);});
#endif
    std::cout<<"sealed inputs "<<checks<<" checks PASS\n";
    return 0;
}catch(const std::exception& e){
    std::cerr<<"sealed inputs suite line="<<__LINE__<<" actual="<<e.what()<<" expected=all checks pass\n";
    return 1;
}}
