#pragma once
// Include in every native test translation unit that defines main.
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <initializer_list>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <crtdbg.h>
namespace pico_test {
inline LONG WINAPI crash(EXCEPTION_POINTERS* info) {
    std::fprintf(stderr,"Native test crashed: Windows exception 0x%08lx at %p\n",
        info->ExceptionRecord->ExceptionCode,info->ExceptionRecord->ExceptionAddress);
    std::fflush(stderr);
    TerminateProcess(GetCurrentProcess(),info->ExceptionRecord->ExceptionCode);
    return EXCEPTION_EXECUTE_HANDLER;
}
inline void aborted(int) {
    std::fputs("Native test crashed: SIGABRT\n",stderr);
    std::fflush(stderr);
    std::_Exit(3);
}
struct Runtime {
    Runtime() {
        SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX|SEM_NOOPENFILEERRORBOX);
        _set_error_mode(_OUT_TO_STDERR);
        _set_abort_behavior(0,_WRITE_ABORT_MSG|_CALL_REPORTFAULT);
        for(int type : {_CRT_WARN,_CRT_ERROR,_CRT_ASSERT}) { (void)type;
            _CrtSetReportMode(type,_CRTDBG_MODE_FILE);
            _CrtSetReportFile(type,_CRTDBG_FILE_STDERR);
        }
        SetUnhandledExceptionFilter(crash);
        std::signal(SIGABRT,aborted);
    }
};
inline Runtime runtime;
}
#endif
