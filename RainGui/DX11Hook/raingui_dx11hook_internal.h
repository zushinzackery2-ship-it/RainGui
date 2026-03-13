#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <d3d11_4.h>
#include <dxgi1_4.h>

#include "../raingui.h"
#include "../raingui_dx11hook_types.h"
#include "../raingui_impl_dx11.h"
#include "../raingui_impl_dx11hook.h"
#include "../raingui_impl_win32.h"

#if __has_include("ConsoleLogger.h")
#include "ConsoleLogger.h"
#define RAINGUI_DX11HOOK_LOG(...) ConsoleLogger::Log(__VA_ARGS__)
#else
#define RAINGUI_DX11HOOK_LOG(...) do { } while (0)
#endif

namespace RainGuiDx11HookInternal
{
    static constexpr UINT MaxBackBuffers = 8;

    typedef HRESULT(STDMETHODCALLTYPE* PresentFn)(
        IDXGISwapChain* swapChain,
        UINT syncInterval,
        UINT flags);

    struct Dx11HookProbeData
    {
        void** swapChainVtable;
    };

    struct ModuleState
    {
        RainGuiDx11HookDesc desc;
        RainGuiDx11HookRuntime runtime;
        Dx11HookProbeData probe;

        PresentFn originalPresent;

        ID3D11Device* device;
        ID3D11DeviceContext* deviceContext;
        ID3D11RenderTargetView* renderTargetViews[MaxBackBuffers];
        ID3D11Multithread* multithread;
        UINT bufferCount;
        DXGI_FORMAT backBufferFormat;
        UINT trackedWidth;
        UINT trackedHeight;

        RainGuiContext* context;
        HWND gameWindow;
        WNDPROC originalWndProc;
        IDXGISwapChain* trackedSwapChain;

        bool ownsContext;
        bool setupCalled;
        bool installed;
        bool backendReady;
        bool deviceLost;
        bool defaultDebugVisible;
        volatile bool unloading;
        volatile bool suspendRendering;
        volatile LONG presentInFlight;

        CRITICAL_SECTION renderCs;
        bool renderCsReady;
        SRWLOCK rainGuiLock;
        bool rainGuiLockReady;

        UINT frameCount;
    };

    extern ModuleState g_state;

    void ResetRuntime();
    void FillDefaultDesc(RainGuiDx11HookDesc& desc);

    bool ProbeVtables(Dx11HookProbeData& probeData);

    bool PatchVtable(
        void** vtable,
        int index,
        void* hookFn,
        void** originalFn);

    bool RestoreVtable(
        void** vtable,
        int index,
        void* originalFn);

    bool InstallHooks();
    void UninstallHooks();

    void UpdateRuntimeSnapshot(IDXGISwapChain* swapChain);
    bool CreateRenderResources(IDXGISwapChain* swapChain);
    bool EnsureRenderTargetView(IDXGISwapChain* swapChain, UINT bufferIndex);
    void CleanupRenderResources();
    bool InitializeBackends(IDXGISwapChain* swapChain);
    void ShutdownBackends(bool finalShutdown);

    bool IsInteractiveVisible();
    void UpdateDefaultDebugState();
    void RenderDebugUi();

    PresentFn ResolvePresentFn(IDXGISwapChain* swapChain);

    HRESULT CallOriginalPresentSafe(
        IDXGISwapChain* swapChain,
        UINT syncInterval,
        UINT flags,
        DWORD* exceptionCode = nullptr);

    void RenderFrame(IDXGISwapChain* swapChain);
    void ProcessPresentFrame(IDXGISwapChain* swapChain);

    LRESULT CALLBACK HookedWndProc(
        HWND hwnd,
        UINT msg,
        WPARAM wParam,
        LPARAM lParam);

    HRESULT STDMETHODCALLTYPE HookPresent(
        IDXGISwapChain* swapChain,
        UINT syncInterval,
        UINT flags);
}
