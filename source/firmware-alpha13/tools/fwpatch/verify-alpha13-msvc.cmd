@call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
@cl /nologo /std:c++17 /EHsc /O2 /W4 /WX tools\fwpatch\main.cpp /Fo:outputs\alpha-main-msvc.obj /Fe:outputs\alpha-fwpatch-msvc.exe
@if errorlevel 1 exit /b 1
@cl /nologo /std:c++17 /EHsc /O2 /W4 /WX tools\fwpatch\verify.cpp /Fo:outputs\alpha-verify-msvc.obj /Fe:outputs\alpha-fwverify-msvc.exe
@if errorlevel 1 exit /b 1
@cl /nologo /std:c++17 /EHsc /O2 /W4 /WX tools\fwpatch\alpha_verify.cpp /Fo:outputs\alpha-cases-msvc.obj /Fe:outputs\alpha-cases-msvc.exe
@if errorlevel 1 exit /b 1
@outputs\alpha-fwpatch-msvc.exe release outputs/frame-alpha13.elf outputs/msvc-alpha13 >outputs/accounting-alpha13-msvc.txt
@if errorlevel 1 exit /b 1
@outputs\alpha-fwverify-msvc.exe >outputs/verification-previous-alpha13-msvc.txt
@if errorlevel 1 exit /b 1
@outputs\alpha-cases-msvc.exe outputs/frame-alpha13/image.bin outputs/frame-alpha13.elf >outputs/verification-alpha13-msvc.txt
@if errorlevel 1 exit /b 1
