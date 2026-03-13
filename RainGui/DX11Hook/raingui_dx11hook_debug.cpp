#include "raingui_dx11hook_internal.h"

namespace RainGuiDx11HookInternal
{
    static void RenderDefaultDebugWindow()
    {
        if (!g_state.desc.enableDefaultDebugWindow || !g_state.defaultDebugVisible)
        {
            return;
        }

        RainGui::SetNextWindowPos(ImVec2(60.0f, 60.0f), RainGuiCond_FirstUseEver);
        RainGui::SetNextWindowSize(ImVec2(430.0f, 240.0f), RainGuiCond_FirstUseEver);

        bool keepVisible = g_state.defaultDebugVisible;
        if (RainGui::Begin("RainGui DX11Hook", &keepVisible, 0))
        {
            RainGui::Text("DX11 hook is active");
            RainGui::Separator();
            RainGui::Text("Ready: %s", g_state.backendReady && !g_state.deviceLost ? "yes" : "no");
            RainGui::Text("Resolution: %.0f x %.0f", g_state.runtime.width, g_state.runtime.height);
            RainGui::Text("Buffers: %u", g_state.runtime.bufferCount);
            RainGui::Text("Frame: %u", g_state.runtime.frameCount);
            RainGui::Text("Format: %u", static_cast<unsigned int>(g_state.runtime.backBufferFormat));
            RainGui::Text("SwapChain: 0x%p", g_state.runtime.swapChain);
            RainGui::Text("Device: 0x%p", g_state.runtime.device);
            RainGui::Text("Context: 0x%p", g_state.runtime.deviceContext);
            RainGui::Spacing();
            RainGui::Text("Toggle key: VK_%u", g_state.desc.toggleVirtualKey);
        }
        RainGui::End();

        g_state.defaultDebugVisible = keepVisible;
    }

    void UpdateDefaultDebugState()
    {
        if (!g_state.desc.enableDefaultDebugWindow || g_state.desc.toggleVirtualKey == 0)
        {
            return;
        }

        if (GetAsyncKeyState(static_cast<int>(g_state.desc.toggleVirtualKey)) & 1)
        {
            g_state.defaultDebugVisible = !g_state.defaultDebugVisible;
        }
    }

    void RenderDebugUi()
    {
        RenderDefaultDebugWindow();
    }
}
