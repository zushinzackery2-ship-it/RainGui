<div align="center">

# RainGui

**基于 Dear ImGui 的即时模式 GUI 库与图形后端封装**

*Immediate Mode UI | DX9 / DX10 / DX11 / DX12 后端 | Hook 转发层*

![C++](https://img.shields.io/badge/C%2B%2B-17-blue?style=flat-square)
![Platform](https://img.shields.io/badge/Platform-Windows%20x64-lightgrey?style=flat-square)
![UI](https://img.shields.io/badge/UI-Immediate%20Mode-green?style=flat-square)
![License](https://img.shields.io/badge/License-MIT-yellow?style=flat-square)

</div>

---

> [!NOTE]
> **仓库边界**  
> `RainGui` 负责 GUI 核心、Win32 / DX9 / DX10 / DX11 / DX12 backend、可选 NVIDIA helper，以及导出层。  
> `DX11 / DX12 / AutoHook` 的 Hook 核心已经下沉到 `Universal-Render-Hook`，`Vulkan` 的底层跟踪与 Layer 接入已经下沉到 `VulkanHook`。

> [!IMPORTANT]
> **Hook 转发层说明**  
> `raingui_impl_autohook.h`、`raingui_impl_dx11hook.h`、`raingui_impl_dx12hook.h`、`raingui_impl_vulkanhook.h` 以及对应的 `*_types.h` 现在只是桥接头。  
> `RainGui_*Hook_*` 导出仍然保留，但实现已经统一收口到 `URH`；其中 Vulkan 路径再由 `URH` 的 facade 转发到 `VulkanHook`。

## 特性

| 功能 | 说明 |
|:-----|:-----|
| **Immediate Mode UI** | 基于 Dear ImGui，每帧重建 UI 状态，无需维护控件树 |
| **DX9 后端** | `raingui_impl_dx9.cpp`：固定管线渲染 backend |
| **DX10 后端** | `raingui_impl_dx10.cpp`：DX10 programmable pipeline backend |
| **DX11 后端** | `raingui_impl_dx11.cpp`：基于 `ID3D11Device` / `ID3D11DeviceContext` 的渲染 backend |
| **DX12 后端** | `raingui_impl_dx12.cpp`：基于 `ID3D12Device` / `ID3D12GraphicsCommandList` 的渲染 backend |
| **Win32 后端** | `raingui_impl_win32.cpp`：处理窗口消息、鼠标和键盘输入 |
| **NVIDIA 扩展** | `raingui_impl_nvidia.cpp`：可选 NVIDIA Overlay helper |
| **Hook 转发层** | `raingui_impl_*hook.h` / `raingui_*hook_types.h`：把 `URH` 的 C++ 接口和类型转发给 `RainGui` 使用者 |
| **DLL 导出** | `raingui_exports.cpp`：统一导出 `RainGui_*` C API |
| **通信层** | `raingui_comm.cpp`：可选共享内存通信辅助 |

---

## 后端架构

```text
应用程序 / DLL
  RainGui::CreateContext()
  RainGui_ImplWin32_Init()
  RainGui_ImplDX11_Init() / RainGui_ImplDX12_Init()
    |
    v
RainGui Core
  +-- raingui.cpp          (ImGui Core)
  +-- raingui_draw.cpp     (Draw Lists)
  +-- raingui_widgets.cpp  (UI Widgets)
  +-- raingui_tables.cpp   (Tables)
    |
    +-------------+-------------+
    |             |             |
    v             v             v
Win32 Backend   DX11 Backend   DX12 Backend
    |             |             |
    +-------------+-------------+
                  |
                  v
         Hook 转发层 / 导出层
      +-- raingui_impl_autohook.h
      +-- raingui_impl_dx11hook.h
      +-- raingui_impl_dx12hook.h
      +-- raingui_impl_vulkanhook.h
      +-- raingui_exports.cpp
```

---

## ImGui 渲染流程

```text
帧开始
    │
    ├─ RainGui_ImplWin32_NewFrame()
    ├─ RainGui_ImplDX11_NewFrame() / RainGui_ImplDX12_NewFrame()
    ├─ RainGui::NewFrame()
    │
    ├─ 用户代码
    │   ├─ RainGui::Begin(...)
    │   ├─ RainGui::Text(...)
    │   ├─ RainGui::Button(...)
    │   └─ RainGui::End()
    │
    └─ RainGui::Render()
        └─ RainGui_ImplDX11_RenderDrawData() / RainGui_ImplDX12_RenderDrawData()
```

---

## DX11 后端原理

```text
初始化
  RainGui_ImplDX11_Init(device, deviceContext)
    ├─ 编译顶点 / 像素着色器
    ├─ 创建输入布局
    ├─ 创建常量缓冲
    ├─ 创建 Blend / Rasterizer / DepthStencil 状态
    └─ 创建字体纹理和采样器

每帧渲染
  RainGui_ImplDX11_RenderDrawData(drawData)
    ├─ 更新顶点 / 索引缓冲
    ├─ 设置正交投影
    ├─ 保存当前渲染状态
    ├─ 绑定 ImGui 所需状态
    ├─ 遍历 DrawList 并 DrawIndexed()
    └─ 恢复调用方状态
```

## DX12 后端原理

```text
初始化
  RainGui_ImplDX12_Init(device, numFramesInFlight, rtvFormat, srvHeap, ...)
    ├─ 创建 Root Signature
    ├─ 创建 Pipeline State Object
    ├─ 分配字体 SRV
    └─ 初始化每帧资源

每帧渲染
  RainGui_ImplDX12_RenderDrawData(drawData, commandList)
    ├─ 更新 Upload Heap 中的顶点 / 索引数据
    ├─ 绑定 Root Signature / PSO / Descriptor Heap
    ├─ 设置视口和正交投影
    └─ 遍历 DrawList 并 DrawIndexedInstanced()
```

---

## Hook 转发层现状

| 文件 | 当前职责 |
|:-----|:---------|
| `raingui_impl_autohook.h` | `UrhAutoHook` 命名空间转发 |
| `raingui_impl_dx11hook.h` | `UrhDx11Hook` 命名空间转发 |
| `raingui_impl_dx12hook.h` | `UrhDx12Hook` 命名空间转发 |
| `raingui_impl_vulkanhook.h` | `UrhVulkanHook` 命名空间转发 |
| `raingui_dx11hook_types.h` | `UrhDx11HookRuntime / Desc` 类型别名 |
| `raingui_dx12hook_types.h` | `UrhDx12HookRuntime / Desc` 类型别名 |
| `raingui_autohook_types.h` | `UrhAutoHookRuntime / Desc` 类型别名 |
| `raingui_vulkanhook_types.h` | `UrhVulkanHookRuntime / Desc` 类型别名 |

需要注意：

- 这些文件不会在 `RainGui` 仓库内部自动完成 Hook 安装逻辑。
- `RainGui` 仓库本身不提供 Vulkan 渲染 backend；`raingui_impl_vulkanhook.h` 只负责 Vulkan Hook / Runtime API 转发。
- 如果你在 Hook 回调里绘制 UI，仍然需要在业务代码里自己创建 `RainGui` context，并按命中的后端初始化 `Win32 + DX11` 或 `Win32 + DX12` backend。

---

## 快速开始

### 直接使用（非 Hook 模式）

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

### Hook 模式（DX11 示例）

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
    RainGui::Text("DX11 hook path");
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

DX12 路径同理，但需要你在业务代码里提供 descriptor heap、command list 等 DX12 渲染资源。  
Vulkan 路径若要绘制 UI，需要由上层自行提供 Vulkan renderer backend。

---

## 目录结构

```text
RainGui/
├── RainGui/
│   ├── raingui.cpp / raingui.h / raingui_internal.h
│   ├── raingui_draw.cpp / raingui_widgets.cpp / raingui_tables.cpp
│   ├── raingui_impl_win32.*
│   ├── raingui_impl_dx9.*
│   ├── raingui_impl_dx10.*
│   ├── raingui_impl_dx11.*
│   ├── raingui_impl_dx12.*
│   ├── raingui_impl_nvidia.*
│   ├── raingui_impl_*hook.h
│   ├── raingui_*hook_types.h
│   ├── raingui_defaults.*
│   ├── raingui_comm.*
│   └── raingui_exports.*
├── LICENSE
└── README.md
```

## 依赖方向

```text
VulkanHook        Universal-Render-Hook
      ↑                 ↑
      └────────┬────────┘
               ↑
             RainGui
```

- 纯 GUI 使用路径不需要 `URH` / `VKH`
- `RainGui` 代码层的 Hook 转发现在统一只看 `URH`
- 但 `URH` 的 Vulkan facade 仍以下层 `VulkanHook` 为实现基础，所以使用 Vulkan 相关转发时仍需要把 `VulkanHook` 一并放进工程

## 集成

当前仓库按源码层分发：

- 直接把 `RainGui/` 目录源码加入你的工程
- 如需 Hook 转发层，额外加入 `Universal-Render-Hook/URH/include` 与 `VulkanHook/VulkanHook/include`
- 当前仓库不再跟踪本地测试载荷、一次性验证脚本或仓库内构建脚本

---

<div align="center">

**Platform:** Windows x64 | **License:** MIT

</div>
