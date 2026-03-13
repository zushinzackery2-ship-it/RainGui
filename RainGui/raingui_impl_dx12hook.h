#pragma once

#include "raingui_dx12hook_types.h"

namespace RainGuiDx12Hook
{
    void FillDefaultDesc(RainGuiDx12HookDesc* desc);
    bool Init(const RainGuiDx12HookDesc* desc);
    bool InitDefaultTest();
    void Shutdown();
    bool IsInstalled();
    bool IsReady();
    const RainGuiDx12HookRuntime* GetRuntime();
}
