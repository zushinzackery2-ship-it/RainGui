#include "raingui_dx11hook_internal.h"
#include "../raingui_defaults.h"

namespace RainGuiDx11HookInternal
{
    bool InitializeBackends(IDXGISwapChain* swapChain)
    {
        UpdateRuntimeSnapshot(swapChain);
        RAINGUI_DX11HOOK_LOG(
            "InitializeBackends start. swapChain=%p hwnd=%p device=%p context=%p size=%.0fx%.0f",
            swapChain,
            g_state.gameWindow,
            g_state.device,
            g_state.deviceContext,
            g_state.runtime.width,
            g_state.runtime.height);

        if (!g_state.context)
        {
            RainGuiContext* context = RainGui::GetCurrentContext();
            if (!context)
            {
                if (!g_state.desc.autoCreateContext)
                {
                    RAINGUI_DX11HOOK_LOG("InitializeBackends failed: autoCreateContext disabled and no current context.");
                    return false;
                }

                RAINGUI_CHECKVERSION();
                context = RainGui::CreateContext();
                if (!context)
                {
                    RAINGUI_DX11HOOK_LOG("InitializeBackends failed: RainGui::CreateContext returned null.");
                    return false;
                }

                g_state.ownsContext = true;
            }

            g_state.context = context;
        }

        RainGui::SetCurrentContext(g_state.context);

        if (!g_state.setupCalled)
        {
            RainGui::GetIO().IniFilename = nullptr;
            if (g_state.ownsContext)
            {
                RainGuiDefaults::ApplyOverlayDefaults();
            }

            if (g_state.desc.onSetup)
            {
                g_state.desc.onSetup(&g_state.runtime, g_state.desc.userData);
            }

            g_state.setupCalled = true;
        }

        if (!RainGui_ImplWin32_Init(g_state.gameWindow))
        {
            RAINGUI_DX11HOOK_LOG("InitializeBackends failed: RainGui_ImplWin32_Init hwnd=%p", g_state.gameWindow);
            return false;
        }

        if (!RainGui_ImplDX11_Init(g_state.device, g_state.deviceContext))
        {
            RAINGUI_DX11HOOK_LOG(
                "InitializeBackends failed: RainGui_ImplDX11_Init device=%p context=%p",
                g_state.device,
                g_state.deviceContext);
            RainGui_ImplWin32_Shutdown();
            return false;
        }

        g_state.rainGuiLockReady = true;

        if (g_state.desc.hookWndProc)
        {
            WNDPROC currentProc = reinterpret_cast<WNDPROC>(
                GetWindowLongPtrW(g_state.gameWindow, GWLP_WNDPROC));
            if (currentProc != HookedWndProc)
            {
                g_state.originalWndProc = reinterpret_cast<WNDPROC>(
                    SetWindowLongPtrW(
                        g_state.gameWindow,
                        GWLP_WNDPROC,
                        reinterpret_cast<LONG_PTR>(HookedWndProc)));
            }
        }

        g_state.trackedSwapChain = swapChain;
        g_state.backendReady = true;
        g_state.deviceLost = false;
        RAINGUI_DX11HOOK_LOG(
            "InitializeBackends succeeded. trackedSwapChain=%p hwnd=%p originalWndProc=%p",
            g_state.trackedSwapChain,
            g_state.gameWindow,
            reinterpret_cast<void*>(g_state.originalWndProc));
        return true;
    }

    void ShutdownBackends(bool finalShutdown)
    {
        RAINGUI_DX11HOOK_LOG(
            "ShutdownBackends called. finalShutdown=%d backendReady=%d context=%p hwnd=%p",
            finalShutdown ? 1 : 0,
            g_state.backendReady ? 1 : 0,
            g_state.context,
            g_state.gameWindow);
        g_state.rainGuiLockReady = false;

        if (g_state.originalWndProc && g_state.gameWindow && IsWindow(g_state.gameWindow))
        {
            SetWindowLongPtrW(
                g_state.gameWindow,
                GWLP_WNDPROC,
                reinterpret_cast<LONG_PTR>(g_state.originalWndProc));
            g_state.originalWndProc = nullptr;
        }

        if (g_state.backendReady && g_state.context)
        {
            RainGui::SetCurrentContext(g_state.context);
            __try
            {
                RainGui_ImplDX11_Shutdown();
                RainGui_ImplWin32_Shutdown();
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
            }
        }

        CleanupRenderResources();

        g_state.backendReady = false;
        g_state.trackedSwapChain = nullptr;
        g_state.runtime.swapChain = nullptr;
        g_state.runtime.device = nullptr;
        g_state.runtime.deviceContext = nullptr;
        g_state.runtime.bufferCount = 0;

        if (!finalShutdown)
        {
            return;
        }

        if (g_state.desc.onShutdown)
        {
            g_state.desc.onShutdown(g_state.desc.userData);
        }

        if (g_state.ownsContext && g_state.context)
        {
            RainGui::DestroyContext(g_state.context);
        }

        g_state.context = nullptr;
        g_state.ownsContext = false;
        g_state.setupCalled = false;
        g_state.gameWindow = nullptr;
        ResetRuntime();
    }
}
