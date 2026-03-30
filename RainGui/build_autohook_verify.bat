@echo off
call "D:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64

cd /d "%~dp0"
call .\build.bat
exit /b %errorlevel%
