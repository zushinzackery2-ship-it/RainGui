#include "raingui_dx11hook_internal.h"

namespace
{
    UINT NormalizeBufferCount(UINT bufferCount)
    {
        if (bufferCount == 0)
        {
            return 0;
        }

        return bufferCount > RainGuiDx11HookInternal::MaxBackBuffers
            ? RainGuiDx11HookInternal::MaxBackBuffers
            : bufferCount;
    }

    bool IsSwapChainStateChanged(IDXGISwapChain* swapChain)
    {
        using namespace RainGuiDx11HookInternal;

        if (!g_state.backendReady || !swapChain)
        {
            return false;
        }

        if (g_state.trackedSwapChain != swapChain)
        {
            return true;
        }

        DXGI_SWAP_CHAIN_DESC desc = {};
        if (FAILED(swapChain->GetDesc(&desc)))
        {
            return true;
        }

        DXGI_FORMAT format = desc.BufferDesc.Format;
        if (format == DXGI_FORMAT_UNKNOWN)
        {
            format = DXGI_FORMAT_R8G8B8A8_UNORM;
        }

        return NormalizeBufferCount(desc.BufferCount) != g_state.bufferCount ||
            desc.BufferDesc.Width != g_state.trackedWidth ||
            desc.BufferDesc.Height != g_state.trackedHeight ||
            format != g_state.backBufferFormat ||
            desc.OutputWindow != g_state.gameWindow;
    }
}

namespace RainGuiDx11HookInternal
{
    void RenderFrame(IDXGISwapChain* swapChain)
    {
        static LONG renderLogCount = 0;
        if (!g_state.backendReady || g_state.deviceLost || !g_state.context || !g_state.deviceContext)
        {
            return;
        }

        UINT bufferIndex = g_state.runtime.backBufferIndex;
        if (bufferIndex >= g_state.bufferCount)
        {
            return;
        }

        if (!EnsureRenderTargetView(swapChain, bufferIndex))
        {
            return;
        }

        ID3D11RenderTargetView* renderTargetView = g_state.renderTargetViews[bufferIndex];
        if (!renderTargetView)
        {
            return;
        }

        LONG currentRenderLog = InterlockedIncrement(&renderLogCount);
        if (currentRenderLog <= 5)
        {
            RAINGUI_DX11HOOK_LOG(
                "RenderFrame #%ld swapChain=%p bufferIndex=%u rtv=%p size=%.0fx%.0f",
                currentRenderLog,
                swapChain,
                static_cast<unsigned int>(bufferIndex),
                renderTargetView,
                g_state.runtime.width,
                g_state.runtime.height);
        }

        if (g_state.multithread)
        {
            g_state.multithread->Enter();
        }

        ID3D11RenderTargetView* oldRtvs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
        ID3D11DepthStencilView* oldDsv = nullptr;
        g_state.deviceContext->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, oldRtvs, &oldDsv);

        __try
        {
            g_state.deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            g_state.deviceLost = true;
        }

        AcquireSRWLockExclusive(&g_state.rainGuiLock);
        __try
        {
            RainGui::SetCurrentContext(g_state.context);
            RainGui_ImplDX11_NewFrame();
            RainGui_ImplWin32_NewFrame();
            RainGui::NewFrame();
            UpdateDefaultDebugState();
            RainGui::GetIO().MouseDrawCursor = IsInteractiveVisible();

            if (g_state.desc.onRender)
            {
                g_state.desc.onRender(&g_state.runtime, g_state.desc.userData);
            }

            RenderDebugUi();
            RainGui::Render();
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            g_state.deviceLost = true;
        }
        ReleaseSRWLockExclusive(&g_state.rainGuiLock);

        if (!g_state.deviceLost)
        {
            __try
            {
                RainGui_ImplDX11_RenderDrawData(RainGui::GetDrawData());
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                g_state.deviceLost = true;
            }
        }

        __try
        {
            g_state.deviceContext->OMSetRenderTargets(
                D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,
                oldRtvs,
                oldDsv);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
        }

        for (ID3D11RenderTargetView* oldRtv : oldRtvs)
        {
            if (oldRtv)
            {
                oldRtv->Release();
            }
        }

        if (oldDsv)
        {
            oldDsv->Release();
        }

        if (g_state.multithread)
        {
            g_state.multithread->Leave();
        }
    }

    void ProcessPresentFrame(IDXGISwapChain* swapChain)
    {
        if (g_state.renderCsReady)
        {
            EnterCriticalSection(&g_state.renderCs);
        }

        __try
        {
            // 只挂 vtable[8] 时，资源重建必须在 Present 内自己兜住。
            if (g_state.deviceLost && g_state.backendReady)
            {
                RAINGUI_DX11HOOK_LOG("ProcessPresentFrame detected deviceLost with backendReady=1, forcing rebuild.");
                ShutdownBackends(false);
                g_state.frameCount = 0;
            }

            if (IsSwapChainStateChanged(swapChain))
            {
                RAINGUI_DX11HOOK_LOG(
                    "ProcessPresentFrame detected swapchain change. tracked=%p current=%p hwnd=%p",
                    g_state.trackedSwapChain,
                    swapChain,
                    g_state.gameWindow);
                ShutdownBackends(false);
                g_state.frameCount = 0;
            }

            UpdateRuntimeSnapshot(swapChain);

            if (g_state.deviceLost && !g_state.backendReady)
            {
                g_state.deviceLost = false;
            }

            if (!g_state.backendReady)
            {
                if (CreateRenderResources(swapChain) && InitializeBackends(swapChain))
                {
                    RAINGUI_DX11HOOK_LOG("ProcessPresentFrame initialized backends successfully.");
                    UpdateRuntimeSnapshot(swapChain);
                }
                else
                {
                    RAINGUI_DX11HOOK_LOG("ProcessPresentFrame failed to initialize backends, forcing shutdown.");
                    ShutdownBackends(false);
                }
            }

            if (g_state.backendReady && !g_state.deviceLost)
            {
                ++g_state.frameCount;
                UpdateRuntimeSnapshot(swapChain);

                if (g_state.frameCount > g_state.desc.warmupFrames)
                {
                    if (g_state.device && g_state.device->GetDeviceRemovedReason() != S_OK)
                    {
                        g_state.deviceLost = true;
                    }

                    if (!g_state.deviceLost)
                    {
                        RenderFrame(swapChain);
                    }
                }
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            g_state.deviceLost = true;
        }

        if (g_state.renderCsReady)
        {
            LeaveCriticalSection(&g_state.renderCs);
        }
    }

    HRESULT STDMETHODCALLTYPE HookPresent(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags)
    {
        static LONG presentLogCount = 0;
        LONG currentPresentLog = InterlockedIncrement(&presentLogCount);
        if (currentPresentLog <= 8)
        {
            RAINGUI_DX11HOOK_LOG(
                "HookPresent #%ld swapChain=%p sync=%u flags=0x%X backendReady=%d deviceLost=%d",
                currentPresentLog,
                swapChain,
                static_cast<unsigned int>(syncInterval),
                static_cast<unsigned int>(flags),
                g_state.backendReady ? 1 : 0,
                g_state.deviceLost ? 1 : 0);
        }

        if (g_state.unloading || g_state.suspendRendering)
        {
            DWORD exceptionCode = 0;
            HRESULT hr = CallOriginalPresentSafe(swapChain, syncInterval, flags, &exceptionCode);
            if (exceptionCode || hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
            {
                g_state.deviceLost = true;
            }

            return hr;
        }

        InterlockedIncrement(&g_state.presentInFlight);

        if (g_state.suspendRendering)
        {
            InterlockedDecrement(&g_state.presentInFlight);

            DWORD exceptionCode = 0;
            HRESULT hr = CallOriginalPresentSafe(swapChain, syncInterval, flags, &exceptionCode);
            if (exceptionCode || hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
            {
                g_state.deviceLost = true;
            }

            return hr;
        }

        if ((flags & DXGI_PRESENT_TEST) || (g_state.gameWindow && IsIconic(g_state.gameWindow)))
        {
            InterlockedDecrement(&g_state.presentInFlight);

            DWORD exceptionCode = 0;
            HRESULT hr = CallOriginalPresentSafe(swapChain, syncInterval, flags, &exceptionCode);
            if (exceptionCode || hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
            {
                g_state.deviceLost = true;
            }

            return hr;
        }

        ProcessPresentFrame(swapChain);

        DWORD exceptionCode = 0;
        HRESULT hr = CallOriginalPresentSafe(swapChain, syncInterval, flags, &exceptionCode);
        if (exceptionCode || hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            g_state.deviceLost = true;
        }

        InterlockedDecrement(&g_state.presentInFlight);
        return hr;
    }
}
