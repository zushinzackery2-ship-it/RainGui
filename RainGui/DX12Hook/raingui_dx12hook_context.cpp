#include "raingui_dx12hook_internal.h"
#include "../raingui_defaults.h"

namespace RainGuiDx12HookInternal
{
    bool InitializeBackends(IDXGISwapChain* swapChain)
    {
        UpdateRuntimeSnapshot(swapChain);
        RAINGUI_DX12HOOK_LOG(
            "InitializeBackends begin: swapChain=%p hwnd=%p device=%p queue=%p buffers=%u format=%u",
            swapChain,
            g_state.gameWindow,
            g_state.device,
            g_state.commandQueue,
            g_state.bufferCount,
            static_cast<unsigned int>(g_state.backBufferFormat));

        if (!g_state.context)
        {
            RainGuiContext* context = RainGui::GetCurrentContext();
            if (!context)
            {
                if (!g_state.desc.autoCreateContext)
                {
                    RAINGUI_DX12HOOK_LOG("InitializeBackends failed: autoCreateContext disabled");
                    return false;
                }

                RAINGUI_CHECKVERSION();
                context = RainGui::CreateContext();
                if (!context)
                {
                    RAINGUI_DX12HOOK_LOG("InitializeBackends failed: CreateContext returned null");
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
            RAINGUI_DX12HOOK_LOG("InitializeBackends failed: RainGui_ImplWin32_Init hwnd=%p", g_state.gameWindow);
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
            RAINGUI_DX12HOOK_LOG(
                "InitializeBackends failed: RainGui_ImplDX12_Init device=%p buffers=%u format=%u",
                g_state.device,
                g_state.bufferCount,
                static_cast<unsigned int>(g_state.backBufferFormat));
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
        RAINGUI_DX12HOOK_LOG(
            "InitializeBackends success: trackedSwapChain=%p hwnd=%p backendReady=%d",
            g_state.trackedSwapChain,
            g_state.gameWindow,
            g_state.backendReady ? 1 : 0);
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
