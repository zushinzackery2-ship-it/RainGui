# RainGui

RainGui 是基于 ImGui 整理出来的定制版本，API 尽量保持原来的使用习惯，
但命名、默认外观和工程组织都已经统一到了 `RainGui`。

这份仓库现在同时保留两条路线：

- 源码级的 `NVIDIA Overlay + 共享内存通信` 方案
- 当前默认在维护和产出的 `DX12Hook + RainGui.dll / RainGui.lib` 方案

如果你只是想先跑通当前仓库，优先看 `RainGui/build.bat`、
`RainGuiDx12HookTest/build.bat` 和下面的 DX12Hook 接入说明即可。

---

## 当前状态

- 品牌标识已统一为 `RainGui::`、`RainGui_*`、`RAINGUI_*`
- 默认不再自动生成 `raingui.ini`
- 默认日志名为 `raingui_log.txt`
- 默认禁用 Demo 和 Metrics 窗口
- `DX9 / DX10 / DX11 / DX12` backend 已同步 RainGui 命名
- 新增模块化 `DX12Hook`
- 新增统一默认外观入口 `RainGui_ApplyOverlayDefaults()`
- `raingui_impl_nvidia.*` 和 `raingui_comm.*` 源码仍然保留
- 但当前默认 `RainGui/build.bat` 的构建重点是 `RainGui.dll`、
  `RainGui.lib` 以及 DX12Hook 导出接口

这点需要单独说明一下：

- `RainGui.dll` 当前导出的主要是核心 API、Win32/DX backend、DX12Hook 接口
- NVIDIA Overlay 和共享内存模块现在更适合源码级接入
- 默认构建脚本不会把 `raingui_impl_nvidia.cpp`、`raingui_comm.cpp`
  编进 `RainGui.dll`

---

## 核心能力

### 1. RainGui 核心库

- 保留 ImGui 的主要使用方式，迁移成本低
- 对外提供 `RainGui_*` C 导出接口，方便 DLL 方式接入
- 内置 Win32、DX9、DX10、DX11、DX12 backend
- 可以通过 `RainGui_ApplyOverlayDefaults()` 直接套用默认外观

### 2. DX12Hook

- 只做 VMT hook，不引入 inline hook
- 覆盖 `Present / ResizeBuffers / ResizeBuffers1 / ExecuteCommandLists`
- 通过 `ExecuteCommandLists` 捕获队列，在 `Present` 阶段消费 direct queue
- 每帧做 Fence 同步，避免 allocator 和命令列表抢用
- 处理 WndProc hook、swapchain 热切换、device lost 和安全卸载
- 安装层和业务层分离，业务代码只需要关心回调

### 3. NVIDIA Overlay 源码方案

- 保留 `raingui_impl_nvidia.*`
- 仍然支持劫持 NVIDIA 的 `CEF-OSC-WIDGET` 窗口
- 仍然保留 D3D11 渲染、消息转发、点击穿透和反截图逻辑
- 这条路线现在不是默认 DLL 导出能力，而是源码级保留模块

### 4. 共享内存通信

- 保留 `raingui_comm.*`
- 共享内存中既有绘制命令，也有双向配置区
- 支持点、线、圆、矩形、文字等基础图元
- 更适合继续沿用原有外部工具 + 渲染端 DLL 的工作流

---

## 项目结构

```text
RainGui-main/
├── RainGui/                           # RainGui 核心库与 DX12Hook
│   ├── DX12Hook/                      # DX12Hook 内部实现
│   ├── raingui.*                      # 核心源码
│   ├── raingui_draw.cpp               # 绘制相关
│   ├── raingui_tables.cpp             # 表格
│   ├── raingui_widgets.cpp            # 控件
│   ├── raingui_internal.h             # 内部头文件
│   ├── raingui_exports.*              # DLL 导出层
│   ├── raingui_defaults.*             # 默认外观
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
├── README原版参考.md                   # 原版参考说明
└── README.md                          # 本文件
```

---

## 依赖

- Windows 10/11
- Visual Studio 2022
- x64 Release / MT 构建链
- 如果走 NVIDIA Overlay 路线，还需要可用的 NVIDIA Overlay 环境

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

---

## 当前默认推荐接法：DX12Hook

如果你的目标是给 DX12 游戏做 Hook，这条是当前仓库最直接、
也是和默认构建产物最一致的接法。

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

这条路线的分工很清楚：

- 安装层负责 hook、设备探测、队列捕获、状态恢复、WndProc 和卸载
- 业务层只实现 `onSetup / onRender / isVisible / onShutdown`

常用导出接口在 `RainGui/raingui_exports.h`：

- `RainGui_DX12Hook_FillDefaultDesc`
- `RainGui_DX12Hook_Init`
- `RainGui_DX12Hook_Shutdown`
- `RainGui_DX12Hook_IsInstalled`
- `RainGui_DX12Hook_IsReady`
- `RainGui_DX12Hook_GetRuntime`
- `RainGui_ApplyOverlayDefaults`

---

## 烟雾测试

根目录里的 `RainGuiDx12HookTest/` 是一个独立测试载荷，
用途很单纯，就是验证 `RainGui.dll + DX12Hook` 这条链路是否能正常装上。

它的行为大致是：

- 等待当前进程成为前台窗口
- 从相对路径加载 `..\..\RainGui\bin\RainGui.dll`
- 解析 DX12Hook 相关导出
- 重试安装 hook，成功后显示默认调试窗口
- `INSERT` 切换显示
- 按 `END` 键卸载并退出

适合做的事：

- 快速验证 hook 是否能装上
- 快速验证 `RainGui.dll` 是否能正常被测试载荷加载

不适合做的事：

- 当成正式接入方案长期使用

正式项目里更建议直接链接 `RainGui.lib`，
或者把需要的源码编进你自己的模块里。

---

## NVIDIA Overlay 路线

这条路线依然在仓库里，只是现在默认不走它产出 DLL。

相关文件：

- `RainGui/raingui_impl_nvidia.h`
- `RainGui/raingui_impl_nvidia.cpp`
- `RainGui/raingui_comm.h`
- `RainGui/raingui_comm.cpp`
- `RainGui/raingui_ui_example.cpp`

源码级最小用法仍然是这套：

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

这条路线目前要注意两件事：

- 默认 `RainGui.dll` 不导出 `RainGui_Nvidia_*` 和 `RainGui_Comm_*`
- 如果你要继续走这条路径，需要把相关源码直接编进自己的项目，
  或者自己调整构建脚本

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

要注意的是，仓库里现在只保留了协议定义和渲染端代码，
并没有附带一个通用的发送端封装库。
如果你要从外部进程发命令，需要自己按 `RainGuiCommData`
的布局写入共享内存。

---

## 适合的使用场景

### 1. DX12 游戏 Hook

- 链接 `RainGui.lib`
- 通过 `RainGui_DX12Hook_*` 完成安装和渲染回调接入

### 2. Hook 烟雾测试

- 构建 `RainGui.dll`
- 构建 `RainGuiDx12HookTest.dll`
- 注入测试 DLL，快速看 hook 是否正常

### 3. 继续沿用旧的 NVIDIA Overlay 工作流

- 直接复用 `raingui_impl_nvidia.*` 和 `raingui_comm.*`
- 由你自己的渲染端 DLL 去编译这些源码

### 4. 外部工具 + 渲染端分离

- 外部进程负责内存读取和数据计算
- 渲染端负责显示
- 双方通过共享内存交换绘制命令和配置

---

## 注意事项

1. 现在仓库默认主线是 `RainGui.dll / RainGui.lib + DX12Hook`，
   不要把 NVIDIA 源码模块和 DLL 导出能力当成同一件事
2. `RainGuiDx12HookTest.dll` 只适合验证 hook，
   正式接入还是建议直接链接 `RainGui.lib`
3. 测试载荷默认按相对路径查找 `RainGui.dll`，
   如果你调整了目录结构，记得一起改加载路径
4. 更多 RainGui 子目录内部说明可以看 `RainGui/README.md`

---

## 许可

基于 ImGui 修改，遵循 MIT 许可证。
