# RainGui

RainGui 是基于 ImGui 整理出来的定制版本，API 尽量保持原来的使用习惯，
但命名、默认外观和工程组织都已经统一到了 `RainGui`。

这份仓库现在同时保留两条路线：

- 源码级的 `NVIDIA Overlay + 共享内存通信` 方案
- 当前主线的 `DX11Hook / DX12Hook + RainGui.dll / RainGui.lib` 方案

---

## 当前状态

- 品牌标识已统一为 `RainGui::`、`RainGui_*`、`RAINGUI_*`
- 默认不再自动生成 `raingui.ini`
- 默认日志名为 `raingui_log.txt`
- 默认禁用 Demo 和 Metrics 窗口
- `DX9 / DX10 / DX11 / DX12` backend 已同步 RainGui 命名
- 新增模块化 `DX11Hook`
- 新增模块化 `DX12Hook`
- 新增统一默认外观入口 `RainGui_ApplyOverlayDefaults()`
- `raingui_impl_nvidia.*` 和 `raingui_comm.*` 源码仍然保留
- 但当前 `RainGui/build.bat` 的构建重点是 `RainGui.dll`、
  `RainGui.lib` 以及 DX11Hook / DX12Hook 导出接口
- `RainGui.dll` 当前导出的主要是核心 API、Win32/DX backend、
  DX11Hook / DX12Hook 接口
- NVIDIA Overlay 和共享内存模块按源码级接入处理
- 默认构建脚本不会把 `raingui_impl_nvidia.cpp`、`raingui_comm.cpp`
  编进 `RainGui.dll`

---

## 核心能力

### 1. RainGui 核心库

- 保留 ImGui 的主要使用方式，迁移成本低
- 对外提供 `RainGui_*` C 导出接口，方便 DLL 方式接入
- 内置 Win32、DX9、DX10、DX11、DX12 backend
- 可以通过 `RainGui_ApplyOverlayDefaults()` 直接套用默认外观

### 2. DX11Hook

- 只做 VMT hook，不引入 inline hook
- 主入口只挂 `IDXGISwapChain::Present` 的 `vtable[8]`
- `Present` 内部负责 RTV 重建、WndProc hook、swapchain 热切换和安全卸载
- 业务层仍然只需要关心 `onSetup / onRender / isVisible / onShutdown`

### 3. DX12Hook

- 只做 VMT hook，不引入 inline hook
- 覆盖 `Present / ResizeBuffers / ResizeBuffers1 / ExecuteCommandLists`
- 通过 `ExecuteCommandLists` 捕获队列，在 `Present` 阶段消费 direct queue
- 每帧做 Fence 同步，避免 allocator 和命令列表抢用
- 处理 WndProc hook、swapchain 热切换、device lost 和安全卸载
- 安装层和业务层分离，业务代码只需要关心回调

### 4. NVIDIA Overlay 源码方案

- 保留 `raingui_impl_nvidia.*`
- 仍然支持劫持 NVIDIA 的 `CEF-OSC-WIDGET` 窗口
- 仍然保留 D3D11 渲染、消息转发、点击穿透和反截图逻辑
- 当前不作为默认 DLL 导出能力，只保留源码模块

### 5. 共享内存通信

- 保留 `raingui_comm.*`
- 共享内存中既有绘制命令，也有双向配置区
- 支持点、线、圆、矩形、文字等基础图元
- 保留给旧的外部工具 + 渲染端 DLL 工作流

---

## 项目结构

```text
RainGui-main/
├── RainGui/                           # RainGui 核心库与 DX11Hook / DX12Hook
│   ├── DX11Hook/                      # DX11Hook 内部实现
│   ├── DX12Hook/                      # DX12Hook 内部实现
│   ├── raingui.*                      # 核心源码
│   ├── raingui_draw.cpp               # 绘制相关
│   ├── raingui_tables.cpp             # 表格
│   ├── raingui_widgets.cpp            # 控件
│   ├── raingui_internal.h             # 内部头文件
│   ├── raingui_exports.*              # DLL 导出层
│   ├── raingui_defaults.*             # 默认外观
│   ├── raingui_dx11hook_types.h       # DX11Hook 对外结构
│   ├── raingui_impl_dx11hook.h        # DX11Hook C++ 接口
│   ├── raingui_dx12hook_types.h       # DX12Hook 对外结构
│   ├── raingui_impl_dx12hook.h        # DX12Hook C++ 接口
│   ├── raingui_impl_nvidia.*          # NVIDIA Overlay 源码方案
│   ├── raingui_impl_nvidia_wndproc.hpp# NVIDIA 窗口消息转发辅助
│   ├── raingui_comm.*                 # 共享内存通信协议
│   ├── raingui_ui_example.cpp         # NVIDIA 路线的示例 UI
│   ├── build.bat                      # 构建 RainGui.dll / RainGui.lib
│   └── README.md                      # 子目录补充说明
├── RainGuiDx12HookTest/               # DX12Hook 独立测试载荷
│   ├── raingui_dx12hook_test.cpp
│   └── build.bat
├── RainGuiDx11HookTest/               # DX11Hook 独立测试载荷
│   ├── raingui_dx11hook_test.cpp
│   └── build.bat
├── README原版参考.md                   # 原版参考说明
└── README.md                          # 本文件
```

---

## 依赖

- Windows 10/11
- Visual Studio 2022
- x64 Release / MT 构建链
- NVIDIA Overlay 路线额外需要可用的 NVIDIA Overlay 环境

仓库里的构建脚本已经会先调用 VS 开发者命令提示符，
直接在 PowerShell 里执行 `.\build.bat` 即可，不要在普通终端手敲 `cl` 或 `link`。

---

## 构建

### 构建 `RainGui.dll`

```powershell
cd .\RainGui
.\build.bat
```

产物位置：

```text
RainGui\bin\RainGui.dll
RainGui\bin\RainGui.lib
RainGui\bin\RainGui.exp
RainGui\obj\*.obj
```

### 构建 `RainGuiDx12HookTest.dll`

```powershell
cd .\RainGuiDx12HookTest
.\build.bat
```

产物位置：

```text
RainGuiDx12HookTest\bin\RainGuiDx12HookTest.dll
RainGuiDx12HookTest\obj\*.obj
```

### 构建 `RainGuiDx11HookTest.dll`

```powershell
cd .\RainGuiDx11HookTest
.\build.bat
```

产物位置：

```text
RainGuiDx11HookTest\bin\RainGuiDx11HookTest.dll
RainGuiDx11HookTest\obj\*.obj
```

---

## 当前主线接法

DX11Hook 和 DX12Hook 的业务层接法保持一致，切换时主要替换：

- `RainGuiDx11HookDesc` / `RainGuiDx12HookDesc`
- `RainGui_DX11Hook_*` / `RainGui_DX12Hook_*`
- runtime 中 DX11 提供 `device + deviceContext`，DX12 提供 `device + commandQueue`

### DX11Hook 最小接入

```cpp
#include "raingui_exports.h"

static bool g_showMenu = true;

static void OnSetup(const RainGuiDx11HookRuntime* runtime, void* userData)
{
    RainGui_ApplyOverlayDefaults();
}

static void OnRender(const RainGuiDx11HookRuntime* runtime, void* userData)
{
    if (!g_showMenu)
    {
        return;
    }

    RainGui_Begin("RainGui DX11 Hook", nullptr, 0);
    RainGui_Text("Hook ready");
    RainGui_End();
}

static bool IsVisible(void* userData)
{
    return g_showMenu;
}

void InstallRainGui()
{
    RainGuiDx11HookDesc desc = {};

    RainGui_DX11Hook_FillDefaultDesc(&desc);
    desc.onSetup = OnSetup;
    desc.onRender = OnRender;
    desc.isVisible = IsVisible;
    desc.toggleVirtualKey = VK_INSERT;
    desc.startVisible = true;

    RainGui_DX11Hook_Init(&desc);
}
```

### DX12Hook 最小接入

最小示例：

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

    RainGui_Begin("RainGui DX12 Hook", nullptr, 0);
    RainGui_Text("Hook ready");
    RainGui_End();
}

static bool IsVisible(void* userData)
{
    return g_showMenu;
}

void InstallRainGui()
{
    RainGuiDx12HookDesc desc = {};

    RainGui_DX12Hook_FillDefaultDesc(&desc);
    desc.onSetup = OnSetup;
    desc.onRender = OnRender;
    desc.isVisible = IsVisible;
    desc.toggleVirtualKey = VK_INSERT;
    desc.startVisible = true;

    RainGui_DX12Hook_Init(&desc);
}
```

常用导出接口在 `RainGui/raingui_exports.h`：

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

---

## 烟雾测试

`RainGuiDx12HookTest/` 和 `RainGuiDx11HookTest/` 是独立测试载荷。

行为：

- 等待当前进程成为前台窗口
- 从相对路径加载 `..\..\RainGui\bin\RainGui.dll`
- 解析对应 Hook 导出
- 重试安装 hook，成功后显示默认调试窗口
- `INSERT` 切换显示
- 按 `END` 键卸载并退出

---

## NVIDIA Overlay 路线

保留源码，不属于当前默认产物。

相关文件：

- `RainGui/raingui_impl_nvidia.h`
- `RainGui/raingui_impl_nvidia.cpp`
- `RainGui/raingui_comm.h`
- `RainGui/raingui_comm.cpp`
- `RainGui/raingui_ui_example.cpp`

源码级最小用法：

```cpp
#include "raingui.h"
#include "raingui_impl_nvidia.h"

RainGui::CreateContext();
RainGui_Nvidia_Init();

while (running)
{
    RainGui_Nvidia_NewFrame();
    RainGui::NewFrame();

    RainGui::Begin("My Window");
    RainGui::Text("Hello, RainGui!");
    RainGui::End();

    RainGui::Render();
    RainGui_Nvidia_Present();
}

RainGui_Nvidia_Shutdown();
RainGui::DestroyContext();
```

- 默认 `RainGui.dll` 不导出 `RainGui_Nvidia_*` 和 `RainGui_Comm_*`
- 继续走这条路径时，需要把相关源码直接编进目标工程，
  或者自行调整构建脚本

---

## 共享内存通信协议

共享内存协议定义在 `RainGui/raingui_comm.h`，
这部分仍然保留，主要服务 NVIDIA Overlay 路线。

关键常量：

- 共享内存名：`Global\\RainGuiComm`
- 协议版本：`3`
- 最大绘制命令数：`32768`

核心结构大致如下：

```cpp
struct RainGuiCommData
{
    DWORD magic;
    DWORD version;
    DWORD frameId;
    ULONGLONG heartbeatTick;
    char processName[64];

    RainGuiConfig config;

    DWORD commandCount;
    RainGuiDrawCommand commands[32768];
};
```

支持的绘制命令：

- `RGUI_CMD_POINT`
- `RGUI_CMD_LINE`
- `RGUI_CMD_CIRCLE`
- `RGUI_CMD_RECT`
- `RGUI_CMD_TEXT`

配置区里保留了原来那套双向读写思路，
主要是 ESP、Aimbot、UI 显示状态之类的字段。

仓库里保留的是协议定义和渲染端代码，没有附带通用发送端封装库。
外部进程发命令时，需要按 `RainGuiCommData`
的布局写入共享内存。

---

## 许可

基于 ImGui 修改，遵循 MIT 许可证。
