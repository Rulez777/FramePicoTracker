@echo off
setlocal
chcp 65001 >nul
powershell.exe -NoProfile -STA -ExecutionPolicy Bypass -File "%~dp0src\FwBuilder.ps1" %*
exit /b %ERRORLEVEL%
