#include "native_test_runtime.hpp"
#include <string>
#include <cassert>
int main(int argc,char** argv) {
#ifdef _WIN32
    if(argc==2 && std::string(argv[1])=="--access-violation") {
        RaiseException(EXCEPTION_ACCESS_VIOLATION,0,0,nullptr);
        return 99;
    }
    if(argc==2 && std::string(argv[1])=="--abort") std::abort();
    if(argc==2 && std::string(argv[1])=="--assert") assert(false && "Native test assertion probe");
    if(argc<2){std::fputs("Usage: test_crash_runner executable [arguments]\n",stderr);return 2;}
    // Preserve Windows quoting exactly; reparsing argv and adding quotes corrupts
    // embedded quotes and trailing backslashes in fixture paths.
    const char* tail=GetCommandLineA();
    if(*tail=='"'){++tail;while(*tail && *tail!='"')++tail;if(*tail)++tail;}
    else {while(*tail && *tail!=' ' && *tail!='\t')++tail;}
    while(*tail==' ' || *tail=='\t')++tail;
    std::string command(tail);
    STARTUPINFOA startup{};startup.cb=sizeof(startup);PROCESS_INFORMATION process{};
    if(!CreateProcessA(nullptr,command.data(),nullptr,nullptr,TRUE,0,nullptr,nullptr,&startup,&process)){
        std::fprintf(stderr,"Native test child launch failed: Windows error %lu\n",GetLastError());return 2;
    }
    const auto waited=WaitForSingleObject(process.hProcess,INFINITE);
    DWORD code=0;
    const bool obtained=waited==WAIT_OBJECT_0 && GetExitCodeProcess(process.hProcess,&code);
    const auto error=GetLastError();
    CloseHandle(process.hThread);CloseHandle(process.hProcess);
    if(!obtained){std::fprintf(stderr,"Native test child wait failed: Windows error %lu\n",error);return 2;}
    if(code)std::fprintf(stderr,"Native test child failed: exit 0x%08lx\n",code);
    return static_cast<int>(code);
#else
    (void)argc;(void)argv;return 2;
#endif
}