#pragma once

#include <d3d11.h>
#include <dxgi1_4.h>

struct RainGuiDx11HookRuntime;

typedef void (*RainGuiDx11HookSetupCallback)(
    const RainGuiDx11HookRuntime* runtime,
    void* userData);

typedef void (*RainGuiDx11HookRenderCallback)(
    const RainGuiDx11HookRuntime* runtime,
    void* userData);

typedef bool (*RainGuiDx11HookVisibleCallback)(
    void* userData);

typedef void (*RainGuiDx11HookShutdownCallback)(
    void* userData);

struct RainGuiDx11HookRuntime
{
    void* hwnd;
    IDXGISwapChain* swapChain;
    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
    UINT bufferCount;
    UINT backBufferIndex;
    DXGI_FORMAT backBufferFormat;
    float width;
    float height;
    UINT frameCount;
};

struct RainGuiDx11HookDesc
{
    RainGuiDx11HookSetupCallback onSetup;
    RainGuiDx11HookRenderCallback onRender;
    RainGuiDx11HookVisibleCallback isVisible;
    RainGuiDx11HookShutdownCallback onShutdown;
    void* userData;

    bool autoCreateContext;
    bool hookWndProc;
    bool blockInputWhenVisible;
    bool enableDefaultDebugWindow;

    UINT warmupFrames;
    UINT shutdownWaitTimeoutMs;
    UINT toggleVirtualKey;
    bool startVisible;
};
