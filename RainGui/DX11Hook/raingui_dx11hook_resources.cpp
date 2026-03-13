#include "raingui_dx11hook_internal.h"

namespace
{
    UINT GetCurrentBackBufferIndex(IDXGISwapChain* swapChain)
    {
        IDXGISwapChain3* swapChain3 = nullptr;
        if (swapChain && SUCCEEDED(swapChain->QueryInterface(IID_PPV_ARGS(&swapChain3))))
        {
            UINT index = swapChain3->GetCurrentBackBufferIndex();
            swapChain3->Release();
            return index;
        }

        return 0;
    }
}

namespace RainGuiDx11HookInternal
{
    void UpdateRuntimeSnapshot(IDXGISwapChain* swapChain)
    {
        DXGI_SWAP_CHAIN_DESC desc = {};
        if (swapChain && SUCCEEDED(swapChain->GetDesc(&desc)))
        {
            g_state.gameWindow = desc.OutputWindow;
            g_state.runtime.hwnd = desc.OutputWindow;
            g_state.runtime.width = static_cast<float>(desc.BufferDesc.Width);
            g_state.runtime.height = static_cast<float>(desc.BufferDesc.Height);
            g_state.runtime.bufferCount = desc.BufferCount;
            g_state.runtime.backBufferFormat = desc.BufferDesc.Format;
        }
        else
        {
            g_state.runtime.hwnd = g_state.gameWindow;
            g_state.runtime.bufferCount = g_state.bufferCount;
            g_state.runtime.backBufferFormat = g_state.backBufferFormat;
        }

        g_state.runtime.swapChain = swapChain;
        g_state.runtime.device = g_state.device;
        g_state.runtime.deviceContext = g_state.deviceContext;
        g_state.runtime.frameCount = g_state.frameCount;
        g_state.runtime.backBufferIndex = GetCurrentBackBufferIndex(swapChain);
    }

    bool CreateRenderResources(IDXGISwapChain* swapChain)
    {
        DXGI_SWAP_CHAIN_DESC desc = {};
        if (!swapChain || FAILED(swapChain->GetDesc(&desc)))
        {
            return false;
        }

        g_state.bufferCount = desc.BufferCount;
        if (g_state.bufferCount == 0)
        {
            return false;
        }

        if (g_state.bufferCount > MaxBackBuffers)
        {
            g_state.bufferCount = MaxBackBuffers;
        }

        g_state.backBufferFormat = desc.BufferDesc.Format;
        if (g_state.backBufferFormat == DXGI_FORMAT_UNKNOWN)
        {
            g_state.backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        }

        g_state.trackedWidth = desc.BufferDesc.Width;
        g_state.trackedHeight = desc.BufferDesc.Height;

        if (FAILED(swapChain->GetDevice(IID_PPV_ARGS(&g_state.device))))
        {
            return false;
        }

        g_state.device->GetImmediateContext(&g_state.deviceContext);
        if (!g_state.deviceContext)
        {
            return false;
        }

        if (SUCCEEDED(g_state.deviceContext->QueryInterface(IID_PPV_ARGS(&g_state.multithread))) &&
            g_state.multithread)
        {
            g_state.multithread->SetMultithreadProtected(TRUE);
        }

        for (UINT index = 0; index < g_state.bufferCount; ++index)
        {
            ID3D11Texture2D* backBuffer = nullptr;
            if (FAILED(swapChain->GetBuffer(index, IID_PPV_ARGS(&backBuffer))))
            {
                return false;
            }

            HRESULT hr = g_state.device->CreateRenderTargetView(backBuffer, nullptr, &g_state.renderTargetViews[index]);
            backBuffer->Release();
            if (FAILED(hr))
            {
                return false;
            }
        }

        return true;
    }

    void CleanupRenderResources()
    {
        if (g_state.deviceContext)
        {
            __try
            {
                g_state.deviceContext->ClearState();
                g_state.deviceContext->Flush();
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
            }
        }

        for (UINT index = 0; index < MaxBackBuffers; ++index)
        {
            if (g_state.renderTargetViews[index])
            {
                g_state.renderTargetViews[index]->Release();
                g_state.renderTargetViews[index] = nullptr;
            }
        }

        if (g_state.multithread)
        {
            g_state.multithread->Release();
            g_state.multithread = nullptr;
        }

        if (g_state.deviceContext)
        {
            g_state.deviceContext->Release();
            g_state.deviceContext = nullptr;
        }

        if (g_state.device)
        {
            g_state.device->Release();
            g_state.device = nullptr;
        }

        g_state.bufferCount = 0;
        g_state.backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        g_state.trackedWidth = 0;
        g_state.trackedHeight = 0;
    }
}
