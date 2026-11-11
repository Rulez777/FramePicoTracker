#include "file_digest.hpp"
#include "../sealed_inputs.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <algorithm>
static void check(bool ok){if(!ok)throw std::runtime_error("streaming digest regression");}
template<class F> static void rejects(F f){bool caught=false;try{f();}catch(...){caught=true;}check(caught);}
int main(){try{
    check(phase5::sha256("")=="e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    check(phase5::sha256("abc")=="ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
    check(phase5::sha256(std::string(1000000,'a'))=="cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0");
    const auto path=std::filesystem::absolute("build/large-executable-regression.fixture");
    std::string chunk(65536,'a');
    {std::ofstream out(path,std::ios::binary);out.write("\177ELF",4);
     for(int i=0;i<272;++i){out.write(chunk.data(),std::streamsize(chunk.size()));}
     check(bool(out));}
    std::ifstream input(path,std::ios::binary);
    const auto expected=phase5::sha256_stream([&](char* b,size_t n){input.read(b,std::streamsize(std::min(n,size_t(997))));return size_t(input.gcount());});
    // Independent System.Security.Cryptography.SHA256 fixture digest.
    check(expected=="d337afbefa16da6207cd93fd082c25e86f5427049c210abc7cc34e8083c34160");
    check(std::filesystem::file_size(path)>16*1024*1024);
#ifndef _WIN32
    check(chmod(path.c_str(),0700)==0);
    const auto digest=phase5::file_digest(path.string(),true);
    check(digest.sha==expected&&digest.size==17*1024*1024+4);
    rejects([&]{phase5::file_digest(path.string(),false);});
    {pico::SealedInput sealed(path,expected,true);check(sealed.size()==digest.size);sealed.verify();}
    rejects([&]{pico::SealedInput sealed(path,std::string(64,'0'),true);});
    bool changed=false;
    rejects([&]{phase5::file_digest_capture(path.string(),true,[&](const char*,size_t){
        if(!changed){changed=true;std::ofstream out(path,std::ios::binary|std::ios::app);out.put('b');}
    });});
    const auto link=path.string()+".link";
    check(symlink(path.c_str(),link.c_str())==0);
    rejects([&]{phase5::file_digest(link,true);});
    std::filesystem::remove(link);
    {std::ofstream out(path,std::ios::binary);out<<"#!/bin/sh\n";}
    rejects([&]{phase5::file_digest(path.string(),true);});
    {std::ofstream out(path,std::ios::binary);out<<"\177ELF";}
    std::filesystem::resize_file(path,256ULL*1024*1024+1);
    rejects([&]{phase5::file_digest(path.string(),true);});
#endif
    input.close();std::filesystem::remove(path);
    std::cout<<"streaming SHA256 and >16 MiB fixture PASS\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
