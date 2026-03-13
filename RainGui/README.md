# RainGui

RainGui 核心库目录。

## 当前状态

- 品牌标识已统一为 `RainGui`
- 默认不再自动生成 `raingui.ini`
- 默认日志名为 `raingui_log.txt`
- 默认禁用 Demo 和 Metrics 窗口
- `ImDrawVert` 已改为非官方默认布局
- DX9 / DX10 / DX11 / DX12 backend 已同步 RainGui 品牌名
- 新增模块化 `DX11Hook`
- 保留 NVIDIA Overlay 后端
- 保留共享内存通信模块
- 新增模块化 `DX12Hook`
- 新增统一默认外观入口 `RainGui_ApplyOverlayDefaults()`

## 目录内重要文件

- `DX11Hook/`
  - DX11Hook 内部实现
- `DX12Hook/`
  - DX12Hook 内部实现
- `raingui.*`
  - 核心源码
- `raingui_impl_win32.*`
  - Win32 backend
- `raingui_impl_dx9/10/11/12.*`
  - 图形 backend
- `raingui_impl_nvidia.*`
  - NVIDIA Overlay 后端
- `raingui_comm.*`
  - 共享内存通信
- `raingui_defaults.*`
  - 默认 overlay 外观
- `raingui_dx11hook_types.h`
  - DX11Hook 对外结构
- `raingui_dx12hook_types.h`
  - DX12Hook 对外结构
- `raingui_impl_dx11hook.h`
  - DX11Hook 对外 C++ 接口
- `raingui_impl_dx12hook.h`
  - DX12Hook 对外 C++ 接口
- `raingui_exports.*`
  - DLL 导出层

## DX11Hook

### 安装层负责

- VMT hook
- 临时设备探测 vtable
- `Present`
- `Present` 内的资源初始化 / 重建
- WndProc hook
- Device lost
- 安全卸载

### 业务层负责

- `onSetup`
- `onRender`
- `isVisible`
- `onShutdown`

## DX12Hook

### 安装层负责

- VMT hook
- 临时设备探测 vtable
- `Present / ResizeBuffers / ResizeBuffers1 / ExecuteCommandLists`
- `CommandQueue` 捕获
- per-frame Fence 同步
- Device lost / swapchain 热切换
- WndProc hook
- 安全卸载

### 业务层负责

- `onSetup`
- `onRender`
- `isVisible`
- `onShutdown`

## Hook 默认导出

- `RainGui_DX11Hook_FillDefaultDesc`
- `RainGui_DX11Hook_Init`
- `RainGui_DX11Hook_Shutdown`
- `RainGui_DX11Hook_IsInstalled`
- `RainGui_DX11Hook_IsReady`
- `RainGui_DX11Hook_GetRuntime`
- `RainGui_DX12Hook_FillDefaultDesc`
- `RainGui_DX12Hook_Init`
- `RainGui_DX12Hook_Shutdown`
- `RainGui_DX12Hook_IsInstalled`
- `RainGui_DX12Hook_IsReady`
- `RainGui_DX12Hook_GetRuntime`
- `RainGui_ApplyOverlayDefaults`

## Hook 最小接入示例（DX12）

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

`build.bat` 会同时编译 DX11Hook / DX12Hook 实现，并产出统一的 `RainGui.dll`。

构建成功后生成：

```text
bin\RainGui.dll
bin\RainGui.lib
bin\RainGui.exp
```

## 测试载荷

根目录里同时提供两个独立测试 DLL：

- `RainGuiDx11HookTest/`
- `RainGuiDx12HookTest/`

共同特征：

- 注入后自动加载 `RainGui.dll`
- 自动安装对应的 Hook
- 成功后显示调试窗口
- 只用于验证 hook 是否正常
