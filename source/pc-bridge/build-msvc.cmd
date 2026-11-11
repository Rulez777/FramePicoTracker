@echo off
setlocal
cd /d "%~dp0"
set "BUILD=build-msvc"
set "FLAGS=/std:c++17 /EHsc /W4 /WX /O2 /Zi /D_CRT_SECURE_NO_WARNINGS /DNOMINMAX"
if "%~1"=="--sanitize" (set "BUILD=build-asan" & set "FLAGS=%FLAGS% /fsanitize=address")
if not exist "%BUILD%" mkdir "%BUILD%"
pushd "%BUILD%"
cl %FLAGS% /I..\..\third_party\openvr ..\tests.cpp /Fe:pc_bridge_tests.exe /link /INCREMENTAL:NO
if errorlevel 1 exit /b 1
.\pc_bridge_tests.exe
if errorlevel 1 exit /b 1
cl %FLAGS% /I..\..\third_party\openvr ..\send.cpp /Fe:pico_pc_bridge_send.exe /link ws2_32.lib /INCREMENTAL:NO
if errorlevel 1 exit /b 1
cl %FLAGS% ..\receive.cpp /Fe:pc_bridge_receive.exe /link ws2_32.lib /INCREMENTAL:NO
if errorlevel 1 exit /b 1
cl %FLAGS% /I..\..\third_party\openvr /LD ..\driver.cpp /Fe:driver_frametrackers.dll /link ws2_32.lib /INCREMENTAL:NO
if errorlevel 1 exit /b 1
xcopy /E /I /Y ..\driver frametrackers >nul
if errorlevel 1 exit /b 1
if not exist frametrackers\bin\win64 mkdir frametrackers\bin\win64
copy /Y driver_frametrackers.dll frametrackers\bin\win64\driver_frametrackers.dll >nul
if errorlevel 1 exit /b 1
popd
exit /b 0
