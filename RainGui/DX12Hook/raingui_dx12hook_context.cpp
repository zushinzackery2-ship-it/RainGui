#include "raingui_dx12hook_internal.h"
#include "../raingui_defaults.h"

namespace RainGuiDx12HookInternal
{
    bool InitializeBackends(IDXGISwapChain* swapChain)
    {
        UpdateRuntimeSnapshot(swapChain);

        if (!g_state.context)
        {
            RainGuiContext* context = RainGui::GetCurrentContext();
            if (!context)
            {
                if (!g_state.desc.autoCreateContext)
                {
                    return false;
                }

                RAINGUI_CHECKVERSION();
                context = RainGui::CreateContext();
                if (!context)
                {
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
            return false;
        }

        if (!RainGui_ImplDX12_Init(
                g_state.device,
                static_cast<int>(g_state.bufferCount),
                g_state.backBufferFormat,
                g_state.srvHeap,
                g_state.srvHeap->GetCPUDescriptorHandleForHeapStart(),
                g_state.srvHeap->GetGPUDescriptorHandleForHeapStart()))
        {
            RainGui_ImplWin32_Shutdown();
            return false;
        }

        RainGui_ImplDX12_CreateDeviceObjects();
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
        return true;
    }

    void ShutdownBackends(bool finalShutdown)
    {
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
                RainGui_ImplDX12_Shutdown();
                RainGui_ImplWin32_Shutdown();
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
            }
        }

        CleanupRenderResources();

        if (g_state.device)
        {
            g_state.device->Release();
            g_state.device = nullptr;
        }

        g_state.backendReady = false;
        g_state.trackedSwapChain = nullptr;
        g_state.runtime.swapChain = nullptr;
        g_state.runtime.device = nullptr;
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

        if (g_state.commandQueue)
        {
            g_state.commandQueue->Release();
            g_state.commandQueue = nullptr;
        }

        auto* pendingQueue = reinterpret_cast<ID3D12CommandQueue*>(
            InterlockedExchangePointer(reinterpret_cast<volatile PVOID*>(&g_state.pendingQueue), nullptr));
        if (pendingQueue)
        {
            pendingQueue->Release();
        }

        g_state.context = nullptr;
        g_state.ownsContext = false;
        g_state.setupCalled = false;
        g_state.gameWindow = nullptr;
        ResetRuntime();
    }
}
