<div align="center">

# RainGui

基于 ImGui 的即插即用 Overlay 支持库

DX11 / DX12 纯 VMT Hook，自动探测 vtable、自动管理资源生命周期、自动处理 Device Lost 和热卸载

NVIDIA Overlay 路线自动发现窗口并接管渲染

> **支持库** — 源码直接编入工程使用，DLL 为可选分发形态

<!-- 在这里放一张效果截图或 GIF -->
<!-- ![RainGui Demo](docs/demo.gif) -->

</div>

## 功能

### DX11 / DX12 Hook

- 纯 VMT hook，不引入 inline hook
- DX12 覆盖 Present / ResizeBuffers / ResizeBuffers1 / ExecuteCommandLists
- **全自动**：资源初始化、GPU Fence 同步、WndProc 接管、Device Lost 重建、Swapchain 热切换、安全卸载 — 全部内部处理，业务代码只需要写 `onRender` 回调
- DX11 同等完成度，接口同构

### NVIDIA Overlay

- 劫持 NVIDIA `CEF-OSC-WIDGET` 窗口
- **全自动**：窗口发现、D3D11 渲染上下文创建、消息转发、点击穿透、反截图
- 源码级接入：将 `raingui_impl_nvidia.cpp` 编进目标工程即可使用

### 共享内存通信

- 外部进程通过共享内存发送绘制命令（点、线、圆、矩形、文字）
- 源码级接入：将 `raingui_comm.cpp` 编进目标工程即可使用

### 通用

- ImGui API 兼容，保留原生使用习惯
- `RainGui_ApplyOverlayDefaults()` 一调即用的默认外观
- DLL / LIB 双形态

## 快速开始

### 1. 构建

```powershell
cd .\RainGui
.\build.bat
```

产物：`RainGui\bin\RainGui.dll` + `RainGui.lib`

### 2. 最小接入（DX12 示例，DX11 同构）

```cpp
#include "raingui_exports.h"

static void OnRender(const RainGuiDx12HookRuntime* runtime, void* userData)
{
    RainGui_Begin("Hello", nullptr, 0);
    RainGui_Text("RainGui is running");
    RainGui_End();
}

void Install()
{
    RainGuiDx12HookDesc desc = {};
    RainGui_DX12Hook_FillDefaultDesc(&desc);
    desc.onRender = OnRender;
    desc.toggleVirtualKey = VK_INSERT;
    desc.startVisible = true;
    RainGui_DX12Hook_Init(&desc);
}
```

### 3. 测试

```powershell
cd .\RainGuiDx12HookTest
.\build.bat
```

注入到任意 DX12 程序，`INSERT` 切换显示，`END` 卸载退出。

## 导出接口

**DX11Hook**：`RainGui_DX11Hook_Init` / `Shutdown` / `IsInstalled` / `IsReady` / `GetRuntime` / `FillDefaultDesc`

**DX12Hook**：`RainGui_DX12Hook_Init` / `Shutdown` / `IsInstalled` / `IsReady` / `GetRuntime` / `FillDefaultDesc`

**通用**：`RainGui_ApplyOverlayDefaults`

## 项目结构

```
RainGui/                     核心库 + Hook 实现
├── DX11Hook/                DX11 VMT Hook
├── DX12Hook/                DX12 VMT Hook
├── raingui.*                核心源码
├── raingui_exports.*        DLL 导出层
├── raingui_defaults.*       默认外观
├── raingui_impl_nvidia.*    NVIDIA Overlay
├── raingui_comm.*           共享内存通信
└── build.bat                构建脚本

RainGuiDx11HookTest/         DX11 测试载荷
RainGuiDx12HookTest/         DX12 测试载荷
```

## 依赖

- Windows 10 / 11
- Visual Studio 2022
- x64 Release / MT

## 许可

基于 ImGui 修改，遵循 MIT 许可证。
