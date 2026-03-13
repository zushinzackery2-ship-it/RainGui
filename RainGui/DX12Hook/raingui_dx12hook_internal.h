#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>

#include "../raingui.h"
#include "../raingui_dx12hook_types.h"
#include "../raingui_impl_dx12.h"
#include "../raingui_impl_dx12hook.h"
#include "../raingui_impl_win32.h"

namespace RainGuiDx12HookInternal
{
    static constexpr UINT MaxBackBuffers = 8;

    typedef HRESULT(STDMETHODCALLTYPE* PresentFn)(
        IDXGISwapChain* swapChain,
        UINT syncInterval,
        UINT flags);

    typedef HRESULT(STDMETHODCALLTYPE* ResizeBuffersFn)(
        IDXGISwapChain* swapChain,
        UINT bufferCount,
        UINT width,
        UINT height,
        DXGI_FORMAT newFormat,
        UINT swapChainFlags);

    typedef HRESULT(STDMETHODCALLTYPE* ResizeBuffers1Fn)(
        IDXGISwapChain3* swapChain,
        UINT bufferCount,
        UINT width,
        UINT height,
        DXGI_FORMAT newFormat,
        UINT swapChainFlags,
        const UINT* creationNodeMask,
        IUnknown* const* presentQueue);

    typedef void(STDMETHODCALLTYPE* ExecuteCommandListsFn)(
        ID3D12CommandQueue* queue,
        UINT numCommandLists,
        ID3D12CommandList* const* commandLists);

    struct Dx12HookProbeData
    {
        void** commandQueueVtable;
        void** swapChainVtable;
    };

    struct ModuleState
    {
        RainGuiDx12HookDesc desc;
        RainGuiDx12HookRuntime runtime;
        Dx12HookProbeData probe;

        PresentFn originalPresent;
        ResizeBuffersFn originalResizeBuffers;
        ResizeBuffers1Fn originalResizeBuffers1;
        ExecuteCommandListsFn originalExecuteCommandLists;

        ID3D12Device* device;
        ID3D12CommandQueue* commandQueue;
        ID3D12CommandQueue* pendingQueue;
        ID3D12DescriptorHeap* rtvHeap;
        ID3D12DescriptorHeap* srvHeap;
        ID3D12CommandAllocator* commandAllocators[MaxBackBuffers];
        ID3D12GraphicsCommandList* commandList;
        ID3D12Resource* backBuffers[MaxBackBuffers];
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[MaxBackBuffers];
        UINT bufferCount;
        UINT rtvDescriptorSize;

        ID3D12Fence* fence;
        HANDLE fenceEvent;
        UINT64 fenceValues[MaxBackBuffers];
        UINT64 fenceCounter;

        RainGuiContext* context;
        HWND gameWindow;
        WNDPROC originalWndProc;
        IDXGISwapChain* trackedSwapChain;
        void** trackedSwapChainVtable;
        DXGI_FORMAT backBufferFormat;

        bool ownsContext;
        bool setupCalled;
        bool installed;
        bool backendReady;
        bool deviceLost;
        bool defaultDebugVisible;
        volatile LONG bootstrapRequested;
        volatile bool unloading;
        volatile bool suspendRendering;
        volatile bool commandListRecording;
        volatile LONG presentInFlight;

        CRITICAL_SECTION renderCs;
        bool renderCsReady;
        SRWLOCK rainGuiLock;
        bool rainGuiLockReady;

        UINT frameCount;
    };

    extern ModuleState g_state;

    void ResetRuntime();
    void FillDefaultDesc(RainGuiDx12HookDesc& desc);

    bool ProbeVtables(Dx12HookProbeData& probeData);

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
    void CleanupRenderResources();
    bool InitializeBackends(IDXGISwapChain* swapChain);
    void ShutdownBackends(bool finalShutdown);

    bool IsInteractiveVisible();
    void UpdateDefaultDebugState();
    void RenderDebugUi();
    void TryHookResizeBuffers1(IDXGISwapChain* swapChain);

    PresentFn ResolvePresentFn(IDXGISwapChain* swapChain);
    ExecuteCommandListsFn ResolveExecuteFn(ID3D12CommandQueue* queue);

    HRESULT CallOriginalPresentSafe(
        IDXGISwapChain* swapChain,
        UINT syncInterval,
        UINT flags,
        DWORD* exceptionCode = nullptr);

    bool CallOriginalExecuteSafe(
        ID3D12CommandQueue* queue,
        UINT numCommandLists,
        ID3D12CommandList* const* commandLists);

    void RenderFrame(
        IDXGISwapChain* swapChain,
        ID3D12CommandQueue* renderQueue);

    LRESULT CALLBACK HookedWndProc(
        HWND hwnd,
        UINT msg,
        WPARAM wParam,
        LPARAM lParam);

    HRESULT STDMETHODCALLTYPE HookPresent(
        IDXGISwapChain* swapChain,
        UINT syncInterval,
        UINT flags);

    HRESULT STDMETHODCALLTYPE HookResizeBuffers(
        IDXGISwapChain* swapChain,
        UINT bufferCount,
        UINT width,
        UINT height,
        DXGI_FORMAT newFormat,
        UINT swapChainFlags);

    HRESULT STDMETHODCALLTYPE HookResizeBuffers1(
        IDXGISwapChain3* swapChain,
        UINT bufferCount,
        UINT width,
        UINT height,
        DXGI_FORMAT newFormat,
        UINT swapChainFlags,
        const UINT* creationNodeMask,
        IUnknown* const* presentQueue);

    void STDMETHODCALLTYPE HookExecuteCommandLists(
        ID3D12CommandQueue* queue,
        UINT numCommandLists,
        ID3D12CommandList* const* commandLists);
}
