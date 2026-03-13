#include <d3d12.h>

#include "raingui_exports.h"
#include "raingui_impl_dx11hook.h"
#include "raingui_impl_dx12hook.h"

RAINGUI_API RainGuiContext* RainGui_CreateContext()
{
    return RainGui::CreateContext();
}

RAINGUI_API void RainGui_DestroyContext(RainGuiContext* ctx)
{
    RainGui::DestroyContext(ctx);
}

RAINGUI_API void RainGui_SetCurrentContext(RainGuiContext* ctx)
{
    RainGui::SetCurrentContext(ctx);
}

RAINGUI_API RainGuiContext* RainGui_GetCurrentContext()
{
    return RainGui::GetCurrentContext();
}

RAINGUI_API RainGuiIO* RainGui_GetIO()
{
    return &RainGui::GetIO();
}

RAINGUI_API RainGuiStyle* RainGui_GetStyle()
{
    return &RainGui::GetStyle();
}

RAINGUI_API void RainGui_NewFrame()
{
    RainGui::NewFrame();
}

RAINGUI_API void RainGui_EndFrame()
{
    RainGui::EndFrame();
}

RAINGUI_API void RainGui_Render()
{
    RainGui::Render();
}

RAINGUI_API ImDrawData* RainGui_GetDrawData()
{
    return RainGui::GetDrawData();
}

// ---- 样式 ----

RAINGUI_API void RainGui_StyleColorsDark()
{
    RainGui::StyleColorsDark();
}

RAINGUI_API void RainGui_StyleColorsLight()
{
    RainGui::StyleColorsLight();
}

RAINGUI_API void RainGui_StyleColorsClassic()
{
    RainGui::StyleColorsClassic();
}

// ---- 窗口 ----

RAINGUI_API bool RainGui_Begin(const char* name, bool* pOpen, int flags)
{
    return RainGui::Begin(name, pOpen, (RainGuiWindowFlags)flags);
}

RAINGUI_API void RainGui_End()
{
    RainGui::End();
}

RAINGUI_API void RainGui_SetNextWindowPos(float x, float y, int cond)
{
    RainGui::SetNextWindowPos(ImVec2(x, y), (RainGuiCond)cond);
}

RAINGUI_API void RainGui_SetNextWindowSize(float w, float h, int cond)
{
    RainGui::SetNextWindowSize(ImVec2(w, h), (RainGuiCond)cond);
}

// ---- 控件 ----

RAINGUI_API void RainGui_Text(const char* text)
{
    RainGui::TextUnformatted(text);
}

RAINGUI_API bool RainGui_Button(const char* label, float sizeX, float sizeY)
{
    return RainGui::Button(label, ImVec2(sizeX, sizeY));
}

RAINGUI_API bool RainGui_Checkbox(const char* label, bool* v)
{
    return RainGui::Checkbox(label, v);
}

RAINGUI_API bool RainGui_SliderFloat(const char* label, float* v, float vMin, float vMax, const char* format)
{
    return RainGui::SliderFloat(label, v, vMin, vMax, format ? format : "%.3f");
}

RAINGUI_API bool RainGui_SliderInt(const char* label, int* v, int vMin, int vMax, const char* format)
{
    return RainGui::SliderInt(label, v, vMin, vMax, format ? format : "%d");
}

RAINGUI_API bool RainGui_ColorEdit4(const char* label, float col[4], int flags)
{
    return RainGui::ColorEdit4(label, col, (RainGuiColorEditFlags)flags);
}

RAINGUI_API bool RainGui_InputText(const char* label, char* buf, int bufSize, int flags)
{
    return RainGui::InputText(label, buf, (size_t)bufSize, (RainGuiInputTextFlags)flags);
}

RAINGUI_API void RainGui_Separator()
{
    RainGui::Separator();
}

RAINGUI_API void RainGui_SameLine(float offsetFromStartX, float spacing)
{
    RainGui::SameLine(offsetFromStartX, spacing);
}

RAINGUI_API void RainGui_Spacing()
{
    RainGui::Spacing();
}

RAINGUI_API bool RainGui_CollapsingHeader(const char* label, int flags)
{
    return RainGui::CollapsingHeader(label, (RainGuiTreeNodeFlags)flags);
}

RAINGUI_API bool RainGui_TreeNode(const char* label)
{
    return RainGui::TreeNode(label);
}

RAINGUI_API void RainGui_TreePop()
{
    RainGui::TreePop();
}

// ---- Win32 Backend ----

extern LRESULT RainGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

RAINGUI_API __int64 RainGui_Win32_WndProcHandler(void* hwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam)
{
    return (__int64)RainGui_ImplWin32_WndProcHandler((HWND)hwnd, (UINT)msg, (WPARAM)wParam, (LPARAM)lParam);
}

RAINGUI_API bool RainGui_Win32_Init(void* hwnd)
{
    return RainGui_ImplWin32_Init(hwnd);
}

RAINGUI_API void RainGui_Win32_Shutdown()
{
    RainGui_ImplWin32_Shutdown();
}

RAINGUI_API void RainGui_Win32_NewFrame()
{
    RainGui_ImplWin32_NewFrame();
}

RAINGUI_API void RainGui_Win32_EnableDpiAwareness()
{
    RainGui_ImplWin32_EnableDpiAwareness();
}

RAINGUI_API float RainGui_Win32_GetDpiScaleForHwnd(void* hwnd)
{
    return RainGui_ImplWin32_GetDpiScaleForHwnd(hwnd);
}

RAINGUI_API float RainGui_Win32_GetDpiScaleForMonitor(void* monitor)
{
    return RainGui_ImplWin32_GetDpiScaleForMonitor(monitor);
}

RAINGUI_API void RainGui_Win32_EnableAlphaCompositing(void* hwnd)
{
    RainGui_ImplWin32_EnableAlphaCompositing(hwnd);
}

RAINGUI_API bool RainGui_DX9_Init(IDirect3DDevice9* device)
{
    return RainGui_ImplDX9_Init(device);
}

RAINGUI_API void RainGui_DX9_Shutdown()
{
    RainGui_ImplDX9_Shutdown();
}

RAINGUI_API void RainGui_DX9_NewFrame()
{
    RainGui_ImplDX9_NewFrame();
}

RAINGUI_API void RainGui_DX9_RenderDrawData(ImDrawData* drawData)
{
    RainGui_ImplDX9_RenderDrawData(drawData);
}

RAINGUI_API bool RainGui_DX9_CreateDeviceObjects()
{
    return RainGui_ImplDX9_CreateDeviceObjects();
}

RAINGUI_API void RainGui_DX9_InvalidateDeviceObjects()
{
    RainGui_ImplDX9_InvalidateDeviceObjects();
}

RAINGUI_API bool RainGui_DX10_Init(ID3D10Device* device)
{
    return RainGui_ImplDX10_Init(device);
}

RAINGUI_API void RainGui_DX10_Shutdown()
{
    RainGui_ImplDX10_Shutdown();
}

RAINGUI_API void RainGui_DX10_NewFrame()
{
    RainGui_ImplDX10_NewFrame();
}

RAINGUI_API void RainGui_DX10_RenderDrawData(ImDrawData* drawData)
{
    RainGui_ImplDX10_RenderDrawData(drawData);
}

RAINGUI_API bool RainGui_DX10_CreateDeviceObjects()
{
    return RainGui_ImplDX10_CreateDeviceObjects();
}

RAINGUI_API void RainGui_DX10_InvalidateDeviceObjects()
{
    RainGui_ImplDX10_InvalidateDeviceObjects();
}

RAINGUI_API bool RainGui_DX11_Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    return RainGui_ImplDX11_Init(device, deviceContext);
}

RAINGUI_API void RainGui_DX11_Shutdown()
{
    RainGui_ImplDX11_Shutdown();
}

RAINGUI_API void RainGui_DX11_NewFrame()
{
    RainGui_ImplDX11_NewFrame();
}

RAINGUI_API void RainGui_DX11_RenderDrawData(ImDrawData* drawData)
{
    RainGui_ImplDX11_RenderDrawData(drawData);
}

RAINGUI_API bool RainGui_DX11_CreateDeviceObjects()
{
    return RainGui_ImplDX11_CreateDeviceObjects();
}

RAINGUI_API void RainGui_DX11_InvalidateDeviceObjects()
{
    RainGui_ImplDX11_InvalidateDeviceObjects();
}

RAINGUI_API void RainGui_DX11Hook_FillDefaultDesc(RainGuiDx11HookDesc* desc)
{
    RainGuiDx11Hook::FillDefaultDesc(desc);
}

RAINGUI_API bool RainGui_DX11Hook_Init(const RainGuiDx11HookDesc* desc)
{
    return RainGuiDx11Hook::Init(desc);
}

RAINGUI_API void RainGui_DX11Hook_Shutdown()
{
    RainGuiDx11Hook::Shutdown();
}

RAINGUI_API bool RainGui_DX11Hook_IsInstalled()
{
    return RainGuiDx11Hook::IsInstalled();
}

RAINGUI_API bool RainGui_DX11Hook_IsReady()
{
    return RainGuiDx11Hook::IsReady();
}

RAINGUI_API const RainGuiDx11HookRuntime* RainGui_DX11Hook_GetRuntime()
{
    return RainGuiDx11Hook::GetRuntime();
}

RAINGUI_API bool RainGui_DX12_Init(ID3D12Device* device, int numFramesInFlight, DXGI_FORMAT rtvFormat, ID3D12DescriptorHeap* cbvSrvHeap,
    D3D12_CPU_DESCRIPTOR_HANDLE fontSrvCpuDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE fontSrvGpuDescHandle)
{
    return RainGui_ImplDX12_Init(device, numFramesInFlight, rtvFormat, cbvSrvHeap, fontSrvCpuDescHandle, fontSrvGpuDescHandle);
}

RAINGUI_API void RainGui_DX12_Shutdown()
{
    RainGui_ImplDX12_Shutdown();
}

RAINGUI_API void RainGui_DX12_NewFrame()
{
    RainGui_ImplDX12_NewFrame();
}

RAINGUI_API void RainGui_DX12_RenderDrawData(ImDrawData* drawData, ID3D12GraphicsCommandList* graphicsCommandList)
{
    RainGui_ImplDX12_RenderDrawData(drawData, graphicsCommandList);
}

RAINGUI_API bool RainGui_DX12_CreateDeviceObjects()
{
    return RainGui_ImplDX12_CreateDeviceObjects();
}

RAINGUI_API void RainGui_DX12_InvalidateDeviceObjects()
{
    RainGui_ImplDX12_InvalidateDeviceObjects();
}

RAINGUI_API void RainGui_DX12Hook_FillDefaultDesc(RainGuiDx12HookDesc* desc)
{
    RainGuiDx12Hook::FillDefaultDesc(desc);
}

RAINGUI_API bool RainGui_DX12Hook_Init(const RainGuiDx12HookDesc* desc)
{
    return RainGuiDx12Hook::Init(desc);
}

RAINGUI_API void RainGui_DX12Hook_Shutdown()
{
    RainGuiDx12Hook::Shutdown();
}

RAINGUI_API bool RainGui_DX12Hook_IsInstalled()
{
    return RainGuiDx12Hook::IsInstalled();
}

RAINGUI_API bool RainGui_DX12Hook_IsReady()
{
    return RainGuiDx12Hook::IsReady();
}

RAINGUI_API const RainGuiDx12HookRuntime* RainGui_DX12Hook_GetRuntime()
{
    return RainGuiDx12Hook::GetRuntime();
}
