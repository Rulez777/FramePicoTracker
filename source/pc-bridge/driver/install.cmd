@echo off
setlocal
set "VRPATH=%~1"
if not defined VRPATH set "VRPATH=C:\Program Files (x86)\Steam\steamapps\common\SteamVR\bin\win64\vrpathreg.exe"
if not exist "%VRPATH%" (echo Pass full path to vrpathreg.exe as first argument & exit /b 1)
if not exist "%~dp0bin\win64\driver_frametrackers.dll" (echo Build the bridge first & exit /b 1)
"%VRPATH%" adddriver "%~dp0."
exit /b %errorlevel%
