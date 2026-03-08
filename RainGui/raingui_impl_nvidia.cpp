// RainGui NVIDIA Overlay 后端实现
// 封装窗口劫持、D3D11 管线、输入绕过、反截图、任务栏兼容等

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include <d3d11.h>
#include <dxgi.h>
#include <dwmapi.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dwmapi.lib")

#define RAINGUI_DEFINE_MATH_OPERATORS
#include "raingui.h"
#include "raingui_impl_win32.h"
#include "raingui_impl_dx11.h"
#include "raingui_impl_nvidia.h"
#include "raingui_comm.h"

#ifndef WDA_EXCLUDEFROMCAPTURE
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#endif

// ============================================================
// 内部状态
// ============================================================

static ID3D11Device*            s_device = nullptr;
static ID3D11DeviceContext*     s_context = nullptr;
static IDXGISwapChain*          s_swapChain = nullptr;
static ID3D11RenderTargetView*  s_rtv = nullptr;
static HWND                     s_hwnd = nullptr;
static float                    s_screenW = 0.0f;
static float                    s_screenH = 0.0f;
static LONG_PTR                 s_origExStyle = 0;

// ============================================================
// 内部函数
// ============================================================

// 全局查找 NVIDIA 覆盖层窗口（FindWindowA 不依赖线程快照，手动映射注入下也能用）
static HWND FindNvidiaWindow()
{
    HWND hWnd = FindWindowA("CEF-OSC-WIDGET", "NVIDIA GeForce Overlay");
    if (hWnd)
        return hWnd;
    // 备选：无标题的 CEF-OSC-WIDGET
    return FindWindowA("CEF-OSC-WIDGET", nullptr);
}

static bool SetupWindow(HWND hWnd)
{
    if (!hWnd || !IsWindow(hWnd))
        return false;

    // 保存原始样式，添加穿透 + 不抢焦点
    s_origExStyle = GetWindowLongPtrA(hWnd, GWL_EXSTYLE);
    SetWindowLongPtrA(hWnd, GWL_EXSTYLE, s_origExStyle | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);

    // DWM 全透明玻璃
    MARGINS margins = { -1, -1, -1, -1 };
    DwmExtendFrameIntoClientArea(hWnd, &margins);

    // Layered window 完全不透明
    SetLayeredWindowAttributes(hWnd, 0x000000, 0xFF, LWA_ALPHA);

    // 高度减 1px 避免隐藏任务栏
    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, w, h - 1, 0);

    ShowWindow(hWnd, SW_SHOWNORMAL);
    UpdateWindow(hWnd);

    // 反截图
    SetWindowDisplayAffinity(hWnd, WDA_EXCLUDEFROMCAPTURE);

    return true;
}

static bool InitD3D11(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL featureLevel;
    D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        levels, 2, D3D11_SDK_VERSION,
        &sd, &s_swapChain, &s_device, &featureLevel, &s_context
    );
    if (FAILED(hr))
        return false;

    ID3D11Texture2D* backBuf = nullptr;
    s_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuf));
    if (backBuf)
    {
        s_device->CreateRenderTargetView(backBuf, nullptr, &s_rtv);
        backBuf->Release();
    }

    return s_rtv != nullptr;
}

static void CleanupD3D11()
{
    if (s_rtv) { s_rtv->Release(); s_rtv = nullptr; }
    if (s_swapChain) { s_swapChain->Release(); s_swapChain = nullptr; }
    if (s_context) { s_context->Release(); s_context = nullptr; }
    if (s_device) { s_device->Release(); s_device = nullptr; }
}

// ============================================================
// 公开 API
// ============================================================

bool RainGui_Nvidia_Init()
{
    fprintf(stderr, "[NvBackend] Init 开始\n"); fflush(stderr);

    // DPI 感知
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    fprintf(stderr, "[NvBackend] DPI 设置完成\n"); fflush(stderr);

    // 已注入进程内部，直接枚举当前进程窗口
    s_hwnd = FindNvidiaWindow();
    fprintf(stderr, "[NvBackend] EnumWindow: HWND=%p\n", (void*)s_hwnd); fflush(stderr);
    if (!s_hwnd)
        return false;

    // 获取物理屏幕分辨率（DPI 感知后的真实像素）
    s_screenW = (float)GetSystemMetrics(SM_CXSCREEN);
    s_screenH = (float)GetSystemMetrics(SM_CYSCREEN);
    fprintf(stderr, "[NvBackend] 物理屏幕: %.0fx%.0f\n", s_screenW, s_screenH); fflush(stderr);

    fprintf(stderr, "[NvBackend] SetupWindow...\n"); fflush(stderr);
    if (!SetupWindow(s_hwnd))
    {
        fprintf(stderr, "[NvBackend] SetupWindow 失败!\n"); fflush(stderr);
        return false;
    }
    fprintf(stderr, "[NvBackend] SetupWindow 成功\n"); fflush(stderr);

    fprintf(stderr, "[NvBackend] InitD3D11...\n"); fflush(stderr);
    if (!InitD3D11(s_hwnd))
    {
        fprintf(stderr, "[NvBackend] InitD3D11 失败!\n"); fflush(stderr);
        return false;
    }
    fprintf(stderr, "[NvBackend] InitD3D11 成功\n"); fflush(stderr);

    // 初始化 RainGui 后端
    fprintf(stderr, "[NvBackend] RainGui_ImplWin32_Init...\n"); fflush(stderr);
    RainGui_ImplWin32_Init(s_hwnd);
    fprintf(stderr, "[NvBackend] RainGui_ImplDX11_Init...\n"); fflush(stderr);
    RainGui_ImplDX11_Init(s_device, s_context);

    // 设置 DisplayFramebufferScale 为 (1, 1)，因为我们使用物理像素坐标
    RainGuiIO& io = RainGui::GetIO();
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    fprintf(stderr, "[NvBackend] DisplayFramebufferScale: %.2f, %.2f\n",
        io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    fflush(stderr);

    fprintf(stderr, "[NvBackend] Init 完成!\n"); fflush(stderr);

    // 初始化共享内存通信（非致命错误）
    fprintf(stderr, "[NvBackend] RainGui_Comm_Init...\n"); fflush(stderr);
    if (!RainGui_Comm_Init())
    {
        fprintf(stderr, "[NvBackend] 共享内存通信初始化失败（非致命）\n"); fflush(stderr);
    }
    else
    {
        fprintf(stderr, "[NvBackend] 共享内存通信初始化成功\n"); fflush(stderr);
    }

    return true;
}

void RainGui_Nvidia_Shutdown()
{
    RainGui_Comm_Shutdown();
    RainGui_ImplDX11_Shutdown();
    RainGui_ImplWin32_Shutdown();
    CleanupD3D11();

    // 恢复 NVIDIA 窗口原始状态
    if (s_hwnd && IsWindow(s_hwnd))
    {
        SetWindowLongPtrA(s_hwnd, GWL_EXSTYLE, s_origExStyle);
        SetWindowDisplayAffinity(s_hwnd, WDA_NONE);
        ShowWindow(s_hwnd, SW_HIDE);
    }

    s_hwnd = nullptr;
}

void RainGui_Nvidia_NewFrame()
{
    if (!s_hwnd)
        return;

    // 强制维持窗口置顶 + 尺寸（NVIDIA Alt+Z 会重置）
    SetWindowPos(s_hwnd, HWND_TOPMOST, 0, 0,
        (int)s_screenW, (int)s_screenH - 1,
        SWP_NOACTIVATE | SWP_NOREDRAW);

    // 动态切换点击穿透
    RainGuiIO& io = RainGui::GetIO();
    LONG_PTR style = GetWindowLongPtrA(s_hwnd, GWL_EXSTYLE);
    if (io.WantCaptureMouse)
    {
        if (style & WS_EX_TRANSPARENT)
            SetWindowLongPtrA(s_hwnd, GWL_EXSTYLE, style & ~WS_EX_TRANSPARENT);
    }
    else
    {
        if (!(style & WS_EX_TRANSPARENT))
            SetWindowLongPtrA(s_hwnd, GWL_EXSTYLE, style | WS_EX_TRANSPARENT);
    }

    // RainGui 后端 NewFrame
    RainGui_ImplDX11_NewFrame();
    RainGui_ImplWin32_NewFrame();

    // 强制覆盖焦点检测（RainGui_ImplWin32_NewFrame 会因非前台窗口置空鼠标位置）
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    io.MousePos = ImVec2((float)cursorPos.x, (float)cursorPos.y);
    io.MouseDown[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    io.MouseDown[1] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
}

void RainGui_Nvidia_Present()
{
    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    s_context->OMSetRenderTargets(1, &s_rtv, nullptr);
    s_context->ClearRenderTargetView(s_rtv, clearColor);

    // 先渲染 RainGui 自身的 UI
    RainGui_ImplDX11_RenderDrawData(RainGui::GetDrawData());

    s_swapChain->Present(1, 0);
}

void* RainGui_Nvidia_GetHwnd()
{
    return (void*)s_hwnd;
}

ID3D11Device* RainGui_Nvidia_GetDevice()
{
    return s_device;
}

ID3D11DeviceContext* RainGui_Nvidia_GetDeviceContext()
{
    return s_context;
}

float RainGui_Nvidia_GetScreenWidth()
{
    return s_screenW;
}

float RainGui_Nvidia_GetScreenHeight()
{
    return s_screenH;
}
