#include "raingui_dx12hook_internal.h"

namespace RainGuiDx12HookInternal
{
    void RenderFrame(IDXGISwapChain* swapChain, ID3D12CommandQueue* renderQueue)
    {
        if (!g_state.backendReady || g_state.deviceLost || !renderQueue || !g_state.context)
        {
            return;
        }

        UINT bufferIndex = g_state.runtime.backBufferIndex;
        if (bufferIndex >= g_state.bufferCount)
        {
            return;
        }

        if (!g_state.backBuffers[bufferIndex] || !g_state.commandAllocators[bufferIndex])
        {
            return;
        }

        if (g_state.fence && g_state.fenceValues[bufferIndex] > 0)
        {
            if (g_state.fence->GetCompletedValue() < g_state.fenceValues[bufferIndex])
            {
                g_state.fence->SetEventOnCompletion(
                    g_state.fenceValues[bufferIndex],
                    g_state.fenceEvent);
                if (WaitForSingleObject(g_state.fenceEvent, g_state.desc.fenceWaitTimeoutMs) == WAIT_TIMEOUT)
                {
                    return;
                }
            }
        }

        if (FAILED(g_state.commandAllocators[bufferIndex]->Reset()))
        {
            return;
        }

        if (FAILED(g_state.commandList->Reset(g_state.commandAllocators[bufferIndex], nullptr)))
        {
            return;
        }

        g_state.commandListRecording = true;

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = g_state.backBuffers[bufferIndex];
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        g_state.commandList->ResourceBarrier(1, &barrier);

        g_state.commandList->OMSetRenderTargets(1, &g_state.rtvHandles[bufferIndex], FALSE, nullptr);
        g_state.commandList->SetDescriptorHeaps(1, &g_state.srvHeap);

        AcquireSRWLockExclusive(&g_state.rainGuiLock);
        __try
        {
            RainGui::SetCurrentContext(g_state.context);
            RainGui_ImplDX12_NewFrame();
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

        __try
        {
            RainGui_ImplDX12_RenderDrawData(RainGui::GetDrawData(), g_state.commandList);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            g_state.deviceLost = true;
        }

        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        g_state.commandList->ResourceBarrier(1, &barrier);
        g_state.commandList->Close();
        g_state.commandListRecording = false;

        ID3D12CommandList* commandLists[] = { g_state.commandList };
        if (!CallOriginalExecuteSafe(renderQueue, 1, commandLists))
        {
            g_state.deviceLost = true;
            return;
        }

        UINT64 fenceValue = ++g_state.fenceCounter;
        g_state.fenceValues[bufferIndex] = fenceValue;
        renderQueue->Signal(g_state.fence, fenceValue);
    }

    HRESULT STDMETHODCALLTYPE HookPresent(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags)
    {
        if (g_state.unloading || g_state.deviceLost || g_state.suspendRendering)
        {
            DWORD exceptionCode = 0;
            HRESULT hr = CallOriginalPresentSafe(swapChain, syncInterval, flags, &exceptionCode);
            if (exceptionCode)
            {
                g_state.deviceLost = true;
            }

            return hr;
        }

        InterlockedIncrement(&g_state.presentInFlight);

        if (g_state.suspendRendering)
        {
            InterlockedDecrement(&g_state.presentInFlight);
            return CallOriginalPresentSafe(swapChain, syncInterval, flags, nullptr);
        }

        if ((flags & DXGI_PRESENT_TEST) || (g_state.gameWindow && IsIconic(g_state.gameWindow)))
        {
            InterlockedDecrement(&g_state.presentInFlight);
            return CallOriginalPresentSafe(swapChain, syncInterval, flags, nullptr);
        }

        if (g_state.renderCsReady)
        {
            EnterCriticalSection(&g_state.renderCs);
        }

        __try
        {
            auto* pendingQueue = reinterpret_cast<ID3D12CommandQueue*>(
                InterlockedExchangePointer(reinterpret_cast<volatile PVOID*>(&g_state.pendingQueue), nullptr));
            if (pendingQueue)
            {
                if (g_state.commandQueue != pendingQueue)
                {
                    if (g_state.commandQueue)
                    {
                        g_state.commandQueue->Release();
                    }

                    g_state.commandQueue = pendingQueue;
                }
                else
                {
                    pendingQueue->Release();
                }
            }

            if (g_state.backendReady && g_state.trackedSwapChain != swapChain)
            {
                ShutdownBackends(false);
                g_state.frameCount = 0;
            }

            UpdateRuntimeSnapshot(swapChain);

            if (!g_state.backendReady && g_state.commandQueue)
            {
                if (CreateRenderResources(swapChain) && InitializeBackends(swapChain))
                {
                    TryHookResizeBuffers1(swapChain);
                }
                else
                {
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
                        RenderFrame(swapChain, g_state.commandQueue);
                    }
                }
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            if (g_state.commandListRecording && g_state.commandList)
            {
                __try
                {
                    g_state.commandList->Close();
                }
                __except (EXCEPTION_EXECUTE_HANDLER)
                {
                }

                g_state.commandListRecording = false;
            }

            g_state.deviceLost = true;
        }

        if (g_state.renderCsReady)
        {
            LeaveCriticalSection(&g_state.renderCs);
        }

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
