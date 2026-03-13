#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>

struct RainGuiDx12HookRuntime;

typedef void (*RainGuiDx12HookSetupCallback)(
    const RainGuiDx12HookRuntime* runtime,
    void* userData);

typedef void (*RainGuiDx12HookRenderCallback)(
    const RainGuiDx12HookRuntime* runtime,
    void* userData);

typedef bool (*RainGuiDx12HookVisibleCallback)(
    void* userData);

typedef void (*RainGuiDx12HookShutdownCallback)(
    void* userData);

struct RainGuiDx12HookRuntime
{
    void* hwnd;
    IDXGISwapChain* swapChain;
    ID3D12Device* device;
    ID3D12CommandQueue* commandQueue;
    UINT bufferCount;
    UINT backBufferIndex;
    DXGI_FORMAT backBufferFormat;
    float width;
    float height;
    UINT frameCount;
};

struct RainGuiDx12HookDesc
{
    RainGuiDx12HookSetupCallback onSetup;
    RainGuiDx12HookRenderCallback onRender;
    RainGuiDx12HookVisibleCallback isVisible;
    RainGuiDx12HookShutdownCallback onShutdown;
    void* userData;

    bool autoCreateContext;
    bool hookWndProc;
    bool blockInputWhenVisible;
    bool enableDefaultDebugWindow;

    UINT warmupFrames;
    UINT fenceWaitTimeoutMs;
    UINT shutdownWaitTimeoutMs;
    UINT toggleVirtualKey;
    bool startVisible;
};
