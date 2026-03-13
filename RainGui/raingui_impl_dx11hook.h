#pragma once

#include "raingui_dx11hook_types.h"

namespace RainGuiDx11Hook
{
    void FillDefaultDesc(RainGuiDx11HookDesc* desc);
    bool Init(const RainGuiDx11HookDesc* desc);
    void Shutdown();
    bool IsInstalled();
    bool IsReady();
    const RainGuiDx11HookRuntime* GetRuntime();
}
