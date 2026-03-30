#pragma once

#include "raingui.h"
#include "raingui_impl_win32.h"
#include "raingui_impl_dx9.h"
#include "raingui_impl_dx10.h"
#include "raingui_impl_dx11.h"
#include "raingui_impl_dx12.h"
#include "raingui_autohook_types.h"
#include "raingui_dx11hook_types.h"
#include "raingui_dx12hook_types.h"
#include "raingui_vulkanhook_types.h"

#undef RAINGUI_API
#ifdef RAINGUI_BUILD_DLL
#define RAINGUI_API extern "C" __declspec(dllexport)
#else
#define RAINGUI_API extern "C" __declspec(dllimport)
#endif

// ---- 核心生命周期 ----
RAINGUI_API RainGuiContext* RainGui_CreateContext();
RAINGUI_API void RainGui_DestroyContext(RainGuiContext* ctx);
RAINGUI_API void RainGui_SetCurrentContext(RainGuiContext* ctx);
RAINGUI_API RainGuiContext* RainGui_GetCurrentContext();
RAINGUI_API RainGuiIO* RainGui_GetIO();
RAINGUI_API RainGuiStyle* RainGui_GetStyle();
RAINGUI_API void RainGui_NewFrame();
RAINGUI_API void RainGui_EndFrame();
RAINGUI_API void RainGui_Render();
RAINGUI_API ImDrawData* RainGui_GetDrawData();

// ---- 样式 ----
RAINGUI_API void RainGui_StyleColorsDark();
RAINGUI_API void RainGui_StyleColorsLight();
RAINGUI_API void RainGui_StyleColorsClassic();
RAINGUI_API void RainGui_ApplyOverlayDefaults();

// ---- 窗口 ----
RAINGUI_API bool RainGui_Begin(const char* name, bool* pOpen, int flags);
RAINGUI_API void RainGui_End();
RAINGUI_API void RainGui_SetNextWindowPos(float x, float y, int cond);
RAINGUI_API void RainGui_SetNextWindowSize(float w, float h, int cond);

// ---- 控件 ----
RAINGUI_API void RainGui_Text(const char* text);
RAINGUI_API bool RainGui_Button(const char* label, float sizeX, float sizeY);
RAINGUI_API bool RainGui_Checkbox(const char* label, bool* v);
RAINGUI_API bool RainGui_SliderFloat(const char* label, float* v, float vMin, float vMax, const char* format);
RAINGUI_API bool RainGui_SliderInt(const char* label, int* v, int vMin, int vMax, const char* format);
RAINGUI_API bool RainGui_ColorEdit4(const char* label, float col[4], int flags);
RAINGUI_API bool RainGui_InputText(const char* label, char* buf, int bufSize, int flags);
RAINGUI_API void RainGui_Separator();
RAINGUI_API void RainGui_SameLine(float offsetFromStartX, float spacing);
RAINGUI_API void RainGui_Spacing();
RAINGUI_API bool RainGui_CollapsingHeader(const char* label, int flags);
RAINGUI_API bool RainGui_TreeNode(const char* label);
RAINGUI_API void RainGui_TreePop();

// ---- Win32 Backend ----
RAINGUI_API bool RainGui_Win32_Init(void* hwnd);
RAINGUI_API void RainGui_Win32_Shutdown();
RAINGUI_API void RainGui_Win32_NewFrame();
RAINGUI_API void RainGui_Win32_EnableDpiAwareness();
RAINGUI_API float RainGui_Win32_GetDpiScaleForHwnd(void* hwnd);
RAINGUI_API float RainGui_Win32_GetDpiScaleForMonitor(void* monitor);
RAINGUI_API void RainGui_Win32_EnableAlphaCompositing(void* hwnd);
RAINGUI_API __int64 RainGui_Win32_WndProcHandler(void* hwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);

RAINGUI_API bool RainGui_DX9_Init(IDirect3DDevice9* device);
RAINGUI_API void RainGui_DX9_Shutdown();
RAINGUI_API void RainGui_DX9_NewFrame();
RAINGUI_API void RainGui_DX9_RenderDrawData(ImDrawData* drawData);
RAINGUI_API bool RainGui_DX9_CreateDeviceObjects();
RAINGUI_API void RainGui_DX9_InvalidateDeviceObjects();

RAINGUI_API bool RainGui_DX10_Init(ID3D10Device* device);
RAINGUI_API void RainGui_DX10_Shutdown();
RAINGUI_API void RainGui_DX10_NewFrame();
RAINGUI_API void RainGui_DX10_RenderDrawData(ImDrawData* drawData);
RAINGUI_API bool RainGui_DX10_CreateDeviceObjects();
RAINGUI_API void RainGui_DX10_InvalidateDeviceObjects();

RAINGUI_API bool RainGui_DX11_Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
RAINGUI_API void RainGui_DX11_Shutdown();
RAINGUI_API void RainGui_DX11_NewFrame();
RAINGUI_API void RainGui_DX11_RenderDrawData(ImDrawData* drawData);
RAINGUI_API bool RainGui_DX11_CreateDeviceObjects();
RAINGUI_API void RainGui_DX11_InvalidateDeviceObjects();

RAINGUI_API void RainGui_DX11Hook_FillDefaultDesc(RainGuiDx11HookDesc* desc);
RAINGUI_API bool RainGui_DX11Hook_Init(const RainGuiDx11HookDesc* desc);
RAINGUI_API void RainGui_DX11Hook_Shutdown();
RAINGUI_API bool RainGui_DX11Hook_IsInstalled();
RAINGUI_API bool RainGui_DX11Hook_IsReady();
RAINGUI_API const RainGuiDx11HookRuntime* RainGui_DX11Hook_GetRuntime();
RAINGUI_API void RainGui_AutoHook_FillDefaultDesc(RainGuiAutoHookDesc* desc);
RAINGUI_API bool RainGui_AutoHook_Init(const RainGuiAutoHookDesc* desc);
RAINGUI_API void RainGui_AutoHook_Shutdown();
RAINGUI_API bool RainGui_AutoHook_IsInstalled();
RAINGUI_API bool RainGui_AutoHook_IsReady();
RAINGUI_API const RainGuiAutoHookRuntime* RainGui_AutoHook_GetRuntime();
RAINGUI_API void RainGui_AutoHook_GetDiagnostics(RainGuiAutoHookDiagnostics* diagnostics);
RAINGUI_API void RainGui_VulkanHook_FillDefaultDesc(RainGuiVulkanHookDesc* desc);
RAINGUI_API bool RainGui_VulkanHook_Init(const RainGuiVulkanHookDesc* desc);
RAINGUI_API void RainGui_VulkanHook_Shutdown();
RAINGUI_API bool RainGui_VulkanHook_IsInstalled();
RAINGUI_API bool RainGui_VulkanHook_IsReady();
RAINGUI_API const RainGuiVulkanHookRuntime* RainGui_VulkanHook_GetRuntime();

RAINGUI_API bool RainGui_DX12_Init(ID3D12Device* device, int numFramesInFlight, DXGI_FORMAT rtvFormat, ID3D12DescriptorHeap* cbvSrvHeap,
    D3D12_CPU_DESCRIPTOR_HANDLE fontSrvCpuDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE fontSrvGpuDescHandle);
RAINGUI_API void RainGui_DX12_Shutdown();
RAINGUI_API void RainGui_DX12_NewFrame();
RAINGUI_API void RainGui_DX12_RenderDrawData(ImDrawData* drawData, ID3D12GraphicsCommandList* graphicsCommandList);
RAINGUI_API bool RainGui_DX12_CreateDeviceObjects();
RAINGUI_API void RainGui_DX12_InvalidateDeviceObjects();

RAINGUI_API void RainGui_DX12Hook_FillDefaultDesc(RainGuiDx12HookDesc* desc);
RAINGUI_API bool RainGui_DX12Hook_Init(const RainGuiDx12HookDesc* desc);
RAINGUI_API bool RainGui_DX12Hook_InitDefaultTest();
RAINGUI_API void RainGui_DX12Hook_Shutdown();
RAINGUI_API bool RainGui_DX12Hook_IsInstalled();
RAINGUI_API bool RainGui_DX12Hook_IsReady();
RAINGUI_API const RainGuiDx12HookRuntime* RainGui_DX12Hook_GetRuntime();
