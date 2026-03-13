# RainGui

这个目录是库本体。

## 包含内容

- `raingui.*`
  - RainGui 核心源码
- `raingui_impl_win32.*`
  - Win32 backend
- `raingui_impl_dx9/10/11/12.*`
  - 图形 backend
- `raingui_impl_nvidia.*`
  - NVIDIA overlay backend
- `raingui_comm.*`
  - 共享内存通信
- `DX12Hook/`
  - DX12Hook 内部实现
- `raingui_exports.*`
  - DLL 导出层
- `raingui_defaults.*`
  - 默认 overlay 外观

## DX12Hook

当前 DX12Hook 负责：

- VMT hook
- 临时设备探测 vtable
- `Present / ResizeBuffers / ResizeBuffers1 / ExecuteCommandLists`
- `CommandQueue` 捕获
- per-frame Fence 同步
- Device lost / swapchain 热切换
- WndProc hook
- 安全卸载

对外导出：

- `RainGui_DX12Hook_FillDefaultDesc`
- `RainGui_DX12Hook_Init`
- `RainGui_DX12Hook_Shutdown`
- `RainGui_DX12Hook_IsInstalled`
- `RainGui_DX12Hook_IsReady`
- `RainGui_DX12Hook_GetRuntime`
- `RainGui_ApplyOverlayDefaults`

`RainGuiDx12HookDesc` 默认行为：

- 自动创建 context
- hook WndProc
- 菜单可见时拦截输入
- 默认热键 `VK_INSERT`

## 最小接入

```cpp
#include "raingui_exports.h"

static bool g_showMenu = true;

static void OnSetup(const RainGuiDx12HookRuntime* runtime, void* userData)
{
    RainGui_ApplyOverlayDefaults();
}

static void OnRender(const RainGuiDx12HookRuntime* runtime, void* userData)
{
    if (GetAsyncKeyState(VK_INSERT) & 1)
    {
        g_showMenu = !g_showMenu;
    }

    if (!g_showMenu)
    {
        return;
    }

    RainGui_Begin("DX12 Hook", nullptr, 0);
    RainGui_Text("RainGui DX12Hook OK");
    RainGui_End();
}

static bool IsVisible(void* userData)
{
    return g_showMenu;
}

void InstallHook()
{
    RainGuiDx12HookDesc desc;
    RainGui_DX12Hook_FillDefaultDesc(&desc);
    desc.onSetup = OnSetup;
    desc.onRender = OnRender;
    desc.isVisible = IsVisible;
    RainGui_DX12Hook_Init(&desc);
}
```

## 构建

```powershell
.\build.bat
```

输出：

```text
bin\RainGui.dll
bin\RainGui.lib
bin\RainGui.exp
```

## 测试载荷

仓库根目录的 `RainGuiDx12HookTest/` 是独立测试 DLL：

- 注入后自动加载 `RainGui.dll`
- 自动安装 DX12Hook
- 只用于验证 hook 是否正常

正式功能不要写进这个测试载荷。
