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
> `RainGui` 是 GUI 层，不再承载底层图形 Hook 核心。  
> `DX11 / DX12 / AutoHook` 已下沉到 `Universal-Render-Hook`，`Vulkan` 已下沉到 `VulkanHook`。  
> `RainGui_*Hook_*` 导出仍然保留，但实现是桥接转发，不再在本仓库内重复维护一份 Hook 核心。

> [!IMPORTANT]
> **ImGui 集成说明**  
> 本库内嵌完整 Dear ImGui 源码（`raingui.cpp` / `raingui.h` / `raingui_internal.h`），提供开箱即用的 UI 能力。  
> 多个图形后端实现（DX9 / DX10 / DX11 / DX12）可根据目标环境选择。

---

## 特性

| 功能 | 说明 |
|:-----|:-----|
| **Immediate Mode UI** | 基于 Dear ImGui，每帧重建 UI 状态，无需维护控件树，代码即界面 |
| **DX9 后端** | `raingui_impl_dx9.cpp`：使用 `IDirect3DDevice9` 创建顶点缓冲和纹理，支持固定管线渲染 |
| **DX10 后端** | `raingui_impl_dx10.cpp`：使用 `ID3D10Device` 创建着色器和常量缓冲，支持可编程管线 |
| **DX11 后端** | `raingui_impl_dx11.cpp`：使用 `ID3D11Device` / `ID3D11DeviceContext`，支持多线程渲染 |
| **DX12 后端** | `raingui_impl_dx12.cpp`：使用 `ID3D12Device` / `ID3D12GraphicsCommandList`，管理 Descriptor Heap 和 Root Signature |
| **Win32 后端** | `raingui_impl_win32.cpp`：处理 `WndProc` 消息，转换鼠标 / 键盘输入到 ImGui |
| **Hook 转发层** | `raingui_impl_autohook.h` / `raingui_impl_dx11hook.h` / `raingui_impl_dx12hook.h` / `raingui_impl_vulkanhook.h`：将 `Universal-Render-Hook` / `VulkanHook` 的回调桥接到 ImGui 后端 |
| **NVIDIA 扩展** | `raingui_impl_nvidia.cpp`：可选 NVIDIA Overlay 支持 |
| **DLL 导出** | `raingui_exports.cpp`：统一导出 `RainGui_*` 系列 C API，便于跨模块调用 |
| **通信层** | `raingui_comm.cpp`：可选的进程间通信辅助（与控制器交互） |

---

## 后端架构

```
应用程序 / DLL
  RainGui_Init() / RainGui_NewFrame() / RainGui_Render()
    |
    v
RainGui Core
  +-- raingui.cpp (ImGui Core)
  +-- raingui_draw.cpp (Draw Lists)
  +-- raingui_widgets.cpp (UI Widgets)
    |
    +-------------+-------------+
    |             |             |
    v             v             v
raingui_impl_   raingui_impl_   raingui_impl_
  dx11.cpp        dx12.cpp        win32.cpp
  (渲染后端)       (渲染后端)       (输入后端)
    |             |
    +-------------+
          |
          v
Hook 转发层
  raingui_impl_autohook.h → Universal-Render-Hook (URH)
  raingui_impl_vulkanhook.h → VulkanHook (VHK)
```

---

## ImGui 渲染流程

```
帧开始
    │
    ├─ RainGui_NewFrame()
    │   ├─ ImGui_ImplWin32_NewFrame()    # 更新输入状态
    │   └─ ImGui::NewFrame()             # 开始新帧
    │
    ├─ 用户代码
    │   ├─ ImGui::Begin("窗口")
    │   ├─ ImGui::Text("文本")
    │   ├─ ImGui::Button("按钮")
    │   └─ ImGui::End()
    │
    └─ RainGui_Render()
        ├─ ImGui::Render()               # 生成 DrawList
        └─ ImGui_ImplDX11_RenderDrawData()  # 提交 GPU 命令
            │
            ├─ 更新顶点 / 索引缓冲
            ├─ 设置视口 / 裁剪矩形
            ├─ 绑定着色器 / 纹理
            └─ DrawIndexed()
```

---

## DX11 后端原理

```cpp
// 初始化：创建着色器和资源
ImGui_ImplDX11_Init(device, deviceContext);
    │
    ├─ 编译顶点着色器 (vs_4_0)
    ├─ 编译像素着色器 (ps_4_0)
    ├─ 创建输入布局 (POSITION / TEXCOORD / COLOR)
    ├─ 创建常量缓冲 (投影矩阵)
    ├─ 创建混合状态 (Alpha Blending)
    ├─ 创建光栅化状态 (裁剪 + 无背面剔除)
    ├─ 创建深度模板状态 (禁用深度测试)
    └─ 创建采样器状态 (线性过滤)

// 每帧渲染
ImGui_ImplDX11_RenderDrawData(drawData);
    │
    ├─ 更新 / 创建顶点缓冲 (ImDrawVert[])
    ├─ 更新 / 创建索引缓冲 (ImDrawIdx[])
    ├─ 设置投影矩阵 (正交投影)
    ├─ 保存当前渲染状态
    ├─ 设置 ImGui 渲染状态
    ├─ 遍历 DrawList
    │   ├─ 设置裁剪矩形
    │   ├─ 绑定纹理
    │   └─ DrawIndexed()
    └─ 恢复渲染状态
```

---

## DX12 后端原理

```cpp
// 初始化
ImGui_ImplDX12_Init(device, numFramesInFlight, rtvFormat, srvDescriptorHeap, ...);
    │
    ├─ 创建 Root Signature (1 个 Descriptor Table + 1 个常量)
    ├─ 编译顶点 / 像素着色器
    ├─ 创建 Pipeline State Object (PSO)
    └─ 为字体纹理分配 SRV Descriptor

// 每帧渲染
ImGui_ImplDX12_RenderDrawData(drawData, commandList);
    │
    ├─ 更新 / 创建顶点缓冲 (Upload Heap)
    ├─ 更新 / 创建索引缓冲 (Upload Heap)
    ├─ 设置 Root Signature / PSO
    ├─ 设置 Descriptor Heap
    ├─ 设置视口 / 投影矩阵
    ├─ 设置顶点 / 索引缓冲视图
    ├─ 遍历 DrawList
    │   ├─ 设置裁剪矩形
    │   ├─ 设置纹理 Descriptor
    │   └─ DrawIndexedInstanced()
    └─ (由调用方提交 CommandList)
```

---

## 目录结构

```
RainGui/
├── RainGui/
│   ├── raingui.cpp              # Dear ImGui 核心（内嵌）
│   ├── raingui.h                # 公开头
│   ├── raingui_internal.h       # 内部结构
│   ├── raingui_draw.cpp         # DrawList 渲染
│   ├── raingui_widgets.cpp      # UI 控件实现
│   ├── raingui_tables.cpp       # 表格控件
│   ├── raingui_demo.cpp         # Demo 窗口
│   │
│   ├── raingui_impl_win32.cpp   # Win32 输入后端
│   ├── raingui_impl_win32.h
│   ├── raingui_impl_dx9.cpp     # DX9 渲染后端
│   ├── raingui_impl_dx9.h
│   ├── raingui_impl_dx10.cpp    # DX10 渲染后端
│   ├── raingui_impl_dx10.h
│   ├── raingui_impl_dx11.cpp    # DX11 渲染后端
│   ├── raingui_impl_dx11.h
│   ├── raingui_impl_dx12.cpp    # DX12 渲染后端
│   ├── raingui_impl_dx12.h
│   │
│   ├── raingui_impl_autohook.h  # URH AutoHook 转发
│   ├── raingui_impl_dx11hook.h  # URH DX11 Hook 转发
│   ├── raingui_impl_dx12hook.h  # URH DX12 Hook 转发
│   ├── raingui_impl_vulkanhook.h # VHK Vulkan Hook 转发
│   │
│   ├── raingui_impl_nvidia.cpp  # NVIDIA Overlay 支持
│   ├── raingui_impl_nvidia.h
│   │
│   ├── raingui_exports.cpp      # DLL 导出入口
│   ├── raingui_exports.h
│   ├── raingui_comm.cpp         # 进程间通信
│   ├── raingui_comm.h
│   ├── raingui_defaults.cpp     # 默认配置
│   │
│   ├── imconfig.h               # ImGui 编译配置
│   ├── imstb_rectpack.h         # stb 字体打包
│   ├── imstb_textedit.h         # stb 文本编辑
│   └── imstb_truetype.h         # stb TrueType 解析
│
```

---

## 快速开始

### 直接使用（非 Hook 模式）

```cpp
#include "raingui.h"
#include "raingui_impl_win32.h"
#include "raingui_impl_dx11.h"

// 初始化
ImGui::CreateContext();
ImGui_ImplWin32_Init(hwnd);
ImGui_ImplDX11_Init(device, deviceContext);

// 主循环
while (running)
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // UI 代码
    ImGui::Begin("Demo");
    ImGui::Text("Hello, RainGui!");
    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

// 清理
ImGui_ImplDX11_Shutdown();
ImGui_ImplWin32_Shutdown();
ImGui::DestroyContext();
```

### Hook 模式（配合 Universal-Render-Hook）

```cpp
#include "raingui_impl_autohook.h"

void OnSetup(const UrhAutoHookRuntime* runtime, void* userData)
{
    ImGui::CreateContext();
    // 根据 runtime->backend 初始化对应后端
}

void OnRender(const UrhAutoHookRuntime* runtime, void* userData)
{
    ImGui::NewFrame();
    // UI 代码
    ImGui::Render();
    // 提交渲染
}

int main()
{
    UrhAutoHookDesc desc;
    URH::FillDefaultDesc(&desc);
    desc.onSetup = OnSetup;
    desc.onRender = OnRender;
    URH::Init(&desc);
}
```

---

## 依赖方向

```
VulkanHook
    ↑
Universal-Render-Hook
    ↑
RainGui (本仓库)
    ↑
InterRec (可选上层)
```

| 方向 | 说明 |
|:-----|:-----|
| `RainGui -> Universal-Render-Hook` | DX11 / DX12 / AutoHook 转发层 |
| `RainGui -> VulkanHook` | Vulkan 转发层 |
| `RainGui <- InterRec` | 可选上层业务使用方 |

---

## 集成

`RainGui` 目前按源码库使用，不再跟踪仓库内测试载荷和本地构建脚本。

- 将 `RainGui/` 目录加入你的工程或作为子模块引用
- 如果使用 Hook 转发头，额外加入 `Universal-Render-Hook/URH/include` 与 `VulkanHook/VulkanHook/include`
- 如需导出统一 DLL，请在你自己的工程里编译 `RainGui/*.cpp` 并按需链接 `URH` / `VulkanHook`

---

<div align="center">

**Platform:** Windows x64 | **License:** MIT

</div>
