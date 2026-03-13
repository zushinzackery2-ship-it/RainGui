@echo off
call "D:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64

cd /d "%~dp0"

set OBJ_DIR=obj
set BIN_DIR=bin

if exist "%OBJ_DIR%" rd /s /q "%OBJ_DIR%"
mkdir "%OBJ_DIR%"
if not exist "%BIN_DIR%" mkdir "%BIN_DIR%"

echo [1/2] Compiling RainGui DLL...
cl /nologo /c /std:c++17 /EHsc /O2 /MT /W3 /DRAINGUI_BUILD_DLL /I"." /Fo"%OBJ_DIR%\\" ^
    raingui_exports.cpp ^
    raingui_exports_defaults.cpp ^
    raingui_exports_dx12hook_test.cpp ^
    raingui_defaults.cpp ^
    raingui.cpp ^
    raingui_draw.cpp ^
    raingui_widgets.cpp ^
    raingui_tables.cpp ^
    raingui_impl_dx9.cpp ^
    raingui_impl_dx10.cpp ^
    raingui_impl_dx12.cpp ^
    DX12Hook\raingui_dx12hook_probe.cpp ^
    DX12Hook\raingui_dx12hook_bootstrap.cpp ^
    DX12Hook\raingui_dx12hook_debug.cpp ^
    DX12Hook\raingui_dx12hook_resources.cpp ^
    DX12Hook\raingui_dx12hook_context.cpp ^
    DX12Hook\raingui_dx12hook_hooks_common.cpp ^
    DX12Hook\raingui_dx12hook_hooks_present.cpp ^
    DX12Hook\raingui_dx12hook_hooks_aux.cpp ^
    raingui_impl_win32.cpp ^
    raingui_impl_dx11.cpp
if errorlevel 1 goto :error

echo [2/2] Linking RainGui.dll...
link /nologo /DLL /OUT:"%BIN_DIR%\RainGui.dll" /IMPLIB:"%BIN_DIR%\RainGui.lib" /MACHINE:X64 ^
    "%OBJ_DIR%\*.obj" ^
    user32.lib gdi32.lib dwmapi.lib d3d9.lib d3d10.lib d3d11.lib d3d12.lib dxgi.lib d3dcompiler.lib
if errorlevel 1 goto :error

echo [post] Finalizing release image...
editbin /RELEASE "%BIN_DIR%\RainGui.dll"
if errorlevel 1 goto :error

echo.
echo === Build OK: %BIN_DIR%\RainGui.dll ===
exit /b 0

:error
echo.
echo === Build FAILED ===
exit /b 1
