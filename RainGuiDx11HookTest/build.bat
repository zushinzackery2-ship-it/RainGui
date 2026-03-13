@echo off
call "D:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64

cd /d "%~dp0"

set OBJ_DIR=obj
set BIN_DIR=bin

if exist "%OBJ_DIR%" rd /s /q "%OBJ_DIR%"
mkdir "%OBJ_DIR%"
if not exist "%BIN_DIR%" mkdir "%BIN_DIR%"

echo [1/2] Compiling RainGuiDx11HookTest...
cl /nologo /c /std:c++17 /EHsc /O2 /MT /W3 /I"." /I"..\RainGui" /Fo"%OBJ_DIR%\\" ^
    raingui_dx11hook_test.cpp
if errorlevel 1 goto :error

echo [2/2] Linking RainGuiDx11HookTest.dll...
link /nologo /DLL /OUT:"%BIN_DIR%\RainGuiDx11HookTest.dll" /MACHINE:X64 ^
    "%OBJ_DIR%\*.obj" ^
    user32.lib shlwapi.lib
if errorlevel 1 goto :error

echo [post] Finalizing test image...
editbin /RELEASE "%BIN_DIR%\RainGuiDx11HookTest.dll"
if errorlevel 1 goto :error

echo.
echo === Build OK: %BIN_DIR%\RainGuiDx11HookTest.dll ===
exit /b 0

:error
echo.
echo === Build FAILED ===
exit /b 1
