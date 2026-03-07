@echo off
call "D:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64

cd /d "%~dp0"

if not exist ".\bin\RainGui.dll" exit /b 1

editbin /RELEASE ".\bin\RainGui.dll"
if errorlevel 1 exit /b 1

if exist ".\bin\RainGui.lib" editbin /RELEASE ".\bin\RainGui.lib" >nul 2>nul
if exist ".\bin\RainGui.exp" editbin /RELEASE ".\bin\RainGui.exp" >nul 2>nul

exit /b 0
