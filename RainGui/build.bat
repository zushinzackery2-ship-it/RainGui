@echo off
call "D:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64

cd /d "%~dp0"

set "SCRIPT_DIR=%~dp0"
if "%URH_ROOT%"=="" set "URH_ROOT=%SCRIPT_DIR%..\..\Universal-Render-Hook\URH"
if "%VKH_ROOT%"=="" set "VKH_ROOT=%SCRIPT_DIR%..\..\VulkanHook\VulkanHook"
set "URH_INCLUDE=%URH_ROOT%\include"
set "URH_SRC=%URH_ROOT%\src"
set "VKH_INCLUDE=%VKH_ROOT%\include"
set "VKH_SRC=%VKH_ROOT%\src"

set OBJ_DIR=obj
set BIN_DIR=bin
set TMP_DIR=%OBJ_DIR%\temp

if exist "%OBJ_DIR%" rd /s /q "%OBJ_DIR%"
mkdir "%OBJ_DIR%"
mkdir "%TMP_DIR%"
if not exist "%BIN_DIR%" mkdir "%BIN_DIR%"

set TMP=%CD%\%TMP_DIR%
set TEMP=%CD%\%TMP_DIR%

echo [1/2] Compiling RainGui DLL...
cl /nologo /c /std:c++17 /EHsc /O2 /MT /W3 /DRAINGUI_BUILD_DLL /I"." /I"%URH_INCLUDE%" /I"%URH_SRC%" /I"%VKH_INCLUDE%" /I"%VKH_SRC%" /Fo"%OBJ_DIR%\\" ^
    raingui_exports.cpp ^
    raingui_exports_defaults.cpp ^
    raingui_exports_dx12hook_test.cpp ^
    "%URH_SRC%\urh_autohook.cpp" ^
    "%URH_SRC%\urh_autohook_helpers.cpp" ^
    "%URH_SRC%\urh_autohook_dispatch.cpp" ^
    raingui_defaults.cpp ^
    raingui.cpp ^
    raingui_draw.cpp ^
    raingui_widgets.cpp ^
    raingui_tables.cpp ^
    raingui_impl_dx9.cpp ^
    raingui_impl_dx10.cpp ^
    "%URH_SRC%\urh_dx11_probe.cpp" ^
    "%URH_SRC%\urh_dx11_bootstrap.cpp" ^
    "%URH_SRC%\urh_dx11_debug.cpp" ^
    "%URH_SRC%\urh_dx11_resources.cpp" ^
    "%URH_SRC%\urh_dx11_context.cpp" ^
    "%URH_SRC%\urh_dx11_hooks_common.cpp" ^
    "%URH_SRC%\urh_dx11_hooks_present.cpp" ^
    raingui_impl_dx12.cpp ^
    "%URH_SRC%\urh_dx12_probe.cpp" ^
    "%URH_SRC%\urh_dx12_bootstrap.cpp" ^
    "%URH_SRC%\urh_dx12_debug.cpp" ^
    "%URH_SRC%\urh_dx12_resources.cpp" ^
    "%URH_SRC%\urh_dx12_context.cpp" ^
    "%URH_SRC%\urh_dx12_hooks_common.cpp" ^
    "%URH_SRC%\urh_dx12_hooks_present.cpp" ^
    "%URH_SRC%\urh_dx12_hooks_aux.cpp" ^
    "%VKH_SRC%\vkh_bootstrap.cpp" ^
    "%VKH_SRC%\vkh_dispatch.cpp" ^
    "%VKH_SRC%\vkh_imports.cpp" ^
    "%VKH_SRC%\vkh_hooks.cpp" ^
    "%VKH_SRC%\vkh_layer_state.cpp" ^
    "%VKH_SRC%\vkh_layer_instance.cpp" ^
    "%VKH_SRC%\vkh_layer_runtime.cpp" ^
    "%VKH_SRC%\vkh_layer_exports.cpp" ^
    "%VKH_SRC%\vkh_loader.cpp" ^
    "%VKH_SRC%\vkh_runtime_probe.cpp" ^
    "%VKH_SRC%\vkh_tracking.cpp" ^
    "%VKH_SRC%\vkh_proc.cpp" ^
    raingui_impl_win32.cpp ^
    raingui_impl_dx11.cpp
if errorlevel 1 goto :error

echo [2/2] Linking RainGui.dll...
link /nologo /DLL /OUT:"%BIN_DIR%\RainGui.dll" /IMPLIB:"%BIN_DIR%\RainGui.lib" /MACHINE:X64 ^
    "%OBJ_DIR%\*.obj" ^
    user32.lib gdi32.lib dwmapi.lib shlwapi.lib d3d9.lib d3d10.lib d3d11.lib d3d12.lib dxgi.lib d3dcompiler.lib
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
