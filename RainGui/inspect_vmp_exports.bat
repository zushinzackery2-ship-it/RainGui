@echo off
call "D:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64

cd /d "%~dp0"

echo ==== EXPORTS ====
dumpbin /exports ".\bin\RainGui.vmp.dll"
if errorlevel 1 exit /b 1

echo.
echo ==== HEADERS ====
dumpbin /headers ".\bin\RainGui.vmp.dll"
if errorlevel 1 exit /b 1

exit /b 0
