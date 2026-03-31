<div align="center">

# RainGui

**基于 Dear ImGui 的即时模式 GUI 库**

*Immediate Mode UI | DX9 / DX10 / DX11 / DX12 渲染后端 | Hook 桥接层*

![C++](https://img.shields.io/badge/C%2B%2B-17-blue?style=flat-square)
![Platform](https://img.shields.io/badge/Platform-Windows%20x64-lightgrey?style=flat-square)
![UI](https://img.shields.io/badge/UI-Immediate%20Mode-green?style=flat-square)
![License](https://img.shields.io/badge/License-MIT-yellow?style=flat-square)

</div>

---

> [!NOTE]
> **仓库边界**  
> 本仓库负责 GUI 核心、Win32 / DX9 / DX10 / DX11 / DX12 渲染后端、可选 NVIDIA helper 及导出层。Hook 核心已下沉到 `Universal-Render-Hook`，Vulkan Layer 接入已下沉到 `VulkanHook`。

> [!IMPORTANT]
> **Hook 桥接层**  
> `raingui_impl_*hook.h` 及 `*_types.h` 现为桥接头，`RainGui_*Hook_*` 导出保留，实现收口到 `URH`，Vulkan 路径由 URH 转发到 `VulkanHook`。

## 特性

| 功能 | 说明 |
|:-----|:-----|
| **Immediate Mode UI** | 基于 Dear ImGui，每帧重建 UI，无需维护控件树 |
| **DX9 后端** | 固定管线渲染 |
| **DX10 后端** | 可编程管线渲染 |
| **DX11 后端** | `ID3D11Device` / `ID3D11DeviceContext` |
| **DX12 后端** | `ID3D12Device` / `ID3D12GraphicsCommandList` |
| **Win32 后端** | 窗口消息、鼠标键盘输入 |
| **NVIDIA 扩展** | 可选 NVIDIA Overlay helper |
| **Hook 桥接层** | 转发 `URH` 的接口和类型 |
| **DLL 导出** | 统一 `RainGui_*` C API |
| **通信层** | 可选共享内存通信辅助 |

---

## 架构

```
应用程序 / DLL
  RainGui::CreateContext()
  RainGui_ImplWin32_Init()
  RainGui_ImplDX11_Init() / RainGui_ImplDX12_Init()
    │
    ▼
RainGui Core
  ├─ raingui.cpp       (ImGui Core)
  ├─ raingui_draw.cpp  (Draw Lists)
  ├─ raingui_widgets.cpp
  └─ raingui_tables.cpp
    │
    ├───────────┬───────────┐
    ▼           ▼           ▼
Win32 Backend  DX11 Backend  DX12 Backend
    │           │           │
    └───────────┴───────────┘
                │
                ▼
       Hook 桥接层 / 导出层
```

---

## 渲染流程

```
帧开始
    ├─ RainGui_ImplWin32_NewFrame()
    ├─ RainGui_ImplDX11_NewFrame()
    ├─ RainGui::NewFrame()
    │
    ├─ 用户 UI 代码
    │   ├─ RainGui::Begin(...)
    │   ├─ RainGui::Text(...) / Button(...) / ...
    │   └─ RainGui::End()
    │
    └─ RainGui::Render()
        └─ RainGui_ImplDX11_RenderDrawData()
```

---

## DX11 后端原理

```
初始化 RainGui_ImplDX11_Init(device, deviceContext)
    ├─ 编译顶点/像素着色器
    ├─ 创建输入布局、常量缓冲
    ├─ 创建 Blend / Rasterizer / DepthStencil 状态
    └─ 创建字体纹理和采样器

每帧 RainGui_ImplDX11_RenderDrawData(drawData)
    ├─ 更新顶点/索引缓冲
    ├─ 设置正交投影
    ├─ 保存/恢复渲染状态
    └─ 遍历 DrawList 并 DrawIndexed()
```

## DX12 后端原理

```
初始化 RainGui_ImplDX12_Init(device, numFramesInFlight, ...)
    ├─ 创建 Root Signature / PSO
    ├─ 分配字体 SRV
    └─ 初始化每帧资源

每帧 RainGui_ImplDX12_RenderDrawData(drawData, commandList)
    ├─ 更新 Upload Heap 顶点/索引
    ├─ 绑定 Root Signature / PSO / Descriptor Heap
    └─ 遍历 DrawList 并 DrawIndexedInstanced()
```

---

## Hook 桥接层

| 文件 | 职责 |
|:-----|:-----|
| `raingui_impl_autohook.h` | 转发 `UrhAutoHook` |
| `raingui_impl_dx11hook.h` | 转发 `UrhDx11Hook` |
| `raingui_impl_dx12hook.h` | 转发 `UrhDx12Hook` |
| `raingui_impl_vulkanhook.h` | 转发 `UrhVulkanHook` |
| `raingui_*hook_types.h` | Runtime / Desc 类型别名 |

注意事项：
- 桥接层不自动完成 Hook 安装
- 本仓库不含 Vulkan 渲染后端，`raingui_impl_vulkanhook.h` 仅转发 Hook API
- Hook 回调中绘制 UI 需自行创建 Context 并初始化 Win32 + DX11/DX12 后端

---

## 快速开始

### 直接使用

```cpp
#include "raingui.h"
#include "raingui_impl_win32.h"
#include "raingui_impl_dx11.h"

RainGui::CreateContext();
RainGui_ImplWin32_Init(hwnd);
RainGui_ImplDX11_Init(device, deviceContext);

while (running)
{
    RainGui_ImplDX11_NewFrame();
    RainGui_ImplWin32_NewFrame();
    RainGui::NewFrame();

    RainGui::Begin("Demo");
    RainGui::Text("Hello, RainGui!");
    RainGui::End();

    RainGui::Render();
    RainGui_ImplDX11_RenderDrawData(RainGui::GetDrawData());
}

RainGui_ImplDX11_Shutdown();
RainGui_ImplWin32_Shutdown();
RainGui::DestroyContext();
```

### Hook 模式（DX11）

```cpp
#include "raingui.h"
#include "raingui_impl_win32.h"
#include "raingui_impl_dx11.h"
#include "raingui_impl_dx11hook.h"
#include "raingui_dx11hook_types.h"

static void OnSetup(const RainGuiDx11HookRuntime* runtime, void* userData)
{
    RainGui::CreateContext();
    RainGui_ImplWin32_Init(runtime->hwnd);
    RainGui_ImplDX11_Init(runtime->device, runtime->deviceContext);
}

static void OnRender(const RainGuiDx11HookRuntime* runtime, void* userData)
{
    RainGui_ImplDX11_NewFrame();
    RainGui_ImplWin32_NewFrame();
    RainGui::NewFrame();

    RainGui::Begin("Overlay");
    RainGui::Text("DX11 Hook");
    RainGui::End();

    RainGui::Render();
    RainGui_ImplDX11_RenderDrawData(RainGui::GetDrawData());
}

void Install()
{
    RainGuiDx11HookDesc desc = {};
    RainGuiDx11Hook::FillDefaultDesc(&desc);
    desc.onSetup = OnSetup;
    desc.onRender = OnRender;
    RainGuiDx11Hook::Init(&desc);
}
```

DX12 路径需提供 descriptor heap / command list 等资源。Vulkan 路径需上层自行提供渲染后端。

---

## 目录结构

```
RainGui/
└── RainGui/
    ├── raingui.cpp / raingui.h / raingui_internal.h    # Core
    ├── raingui_draw.cpp / raingui_widgets.cpp / raingui_tables.cpp
    ├── raingui_impl_win32.*                            # Win32 后端
    ├── raingui_impl_dx9.* / dx10.* / dx11.* / dx12.*   # 渲染后端
    ├── raingui_impl_nvidia.*                           # NVIDIA 扩展
    ├── raingui_impl_*hook.h / raingui_*hook_types.h    # Hook 桥接
    ├── raingui_defaults.* / raingui_comm.*             # 辅助
    └── raingui_exports.*                               # DLL 导出
```

---

## 集成

源码仓库，不含测试载荷或构建脚本。

- 直接加入 `RainGui/` 源码
- 如需 Hook 桥接层，额外加入 `URH/include` 与 `VulkanHook/include`

## 依赖方向

```
VulkanHook        Universal-Render-Hook
      ↑                   ↑
      └─────────┬─────────┘
                ↑
             RainGui
```

- 纯 GUI：无需 URH / VKH
- Hook 桥接层：统一依赖 URH
- Vulkan 桥接：需同时接入 VulkanHook

---

<div align="center">

**Platform:** Windows x64 | **License:** MIT

</div>
