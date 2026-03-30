# RainGui

RainGui 核心库目录。

## 当前状态

- 品牌标识已统一为 `RainGui`
- 默认不再自动生成 `raingui.ini`
- 默认日志名为 `raingui_log.txt`
- 默认禁用 Demo 和 Metrics 窗口
- `ImDrawVert` 已改为非官方默认布局
- DX9 / DX10 / DX11 / DX12 backend 已同步 RainGui 品牌名
- `DX11 / DX12 / AutoHook` 实现已下沉到 `Universal-Render-Hook`
- `VulkanHook` 实现已下沉到独立 `VulkanHook`
- 保留 NVIDIA Overlay 后端
- 保留共享内存通信模块
- 新增统一默认外观入口 `RainGui_ApplyOverlayDefaults()`
- `RainGui_DX11Hook_* / RainGui_DX12Hook_* / RainGui_AutoHook_* / RainGui_VulkanHook_*` 继续作为桥接导出保留

## 目录内重要文件

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
  - DX11Hook 对外结构转发层
- `raingui_dx12hook_types.h`
  - DX12Hook 对外结构转发层
- `raingui_impl_dx11hook.h`
  - DX11Hook 对外 C++ 接口转发层
- `raingui_impl_dx12hook.h`
  - DX12Hook 对外 C++ 接口转发层
- `raingui_autohook_types.h`
  - AutoHook 对外结构转发层
- `raingui_impl_autohook.h`
  - AutoHook 对外 C++ 接口转发层
- `raingui_vulkanhook_types.h`
  - VulkanHook 对外结构转发层
- `raingui_impl_vulkanhook.h`
  - VulkanHook 对外 C++ 接口转发层
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

## 集成

`RainGui` 当前作为源码层分发，不再在仓库里跟踪本地 `build.bat`、验证载荷或其它一次性测试脚本。

- 直接将本目录源码加入你的工程
- 如果使用 Hook 桥接头，同时加入 `Universal-Render-Hook/URH/include` 与 `VulkanHook/VulkanHook/include`
- 统一 DLL、打包和验证工程由上层使用方自行维护

## 命名兼容

- `Universal-Render-Hook` 原生类型以 `Urh*` 为规范名
- `VulkanHook` 原生类型以 `Vkh*` 为规范名
- `RainGuiDx11HookDesc / RainGuiDx12HookDesc / RainGuiAutoHookDesc / RainGuiVulkanHookDesc` 只在 `RainGui` 转发头里提供
