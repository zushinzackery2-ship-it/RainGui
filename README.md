# RainGui

ImGui 魔改版，API 基本对齐，内置 NVIDIA Overlay 钩子后端 + 跨进程共享内存通信。

- 全量重命名 `RainGui::`、`RainGui*`、`RAINGUI_*`
- 内置 `raingui_impl_nvidia`：自动 NVIDIA 窗口劫持 + D3D11 + 输入消息转发 + 反截图
- 内置 `raingui_comm`：跨进程共享内存通信，支持外部进程发送绘制命令
- `RainGui_Nvidia_Init()` 一行初始化

---

## 核心功能

### NVIDIA Overlay 后端
- **截图不可见** — `SetWindowDisplayAffinity(WDA_EXCLUDEFROMCAPTURE)`
- **无新窗口/进程** — 劫持 NVIDIA 已有的 `CEF-OSC-WIDGET` 窗口
- **RainGui 渲染** — 群青色主题 + 微软雅黑字体 + DPI 感知
- **智能点击穿透** — 鼠标在 GUI 上可交互，移开自动穿透
- **消息转发** — 转发 Win32 WndProc，滚轮、`InputText`、快捷键输入更稳定
- **任务栏兼容** — 窗口高度 -1px，不遮挡任务栏
- **Alt+Z 兼容** — 每帧强制维持窗口尺寸，NVIDIA 重置后自动恢复

### 共享内存通信
- **跨进程绘制** — 外部进程通过共享内存发送绘制命令
- **零拷贝** — 纯内存操作，延迟 < 1μs
- **支持图元** — 点、线、圆、矩形、文字
- **配置同步** — 双向读写配置变量（ESP、Aimbot 等）
- **顺序无关** — DLL 和外部进程启动顺序任意

---

## 项目结构

```
RainGui-main/
├── RainGui/                      # RainGui 核心库
│   ├── raingui.*                 # 核心源码（ImGui 魔改）
│   ├── raingui_draw.cpp          # 绘制
│   ├── raingui_tables.cpp        # 表格
│   ├── raingui_widgets.cpp       # 控件
│   ├── raingui_internal.h        # 内部头文件
│   ├── raingui_impl_nvidia.*     # NVIDIA Overlay 钩子后端
│   ├── raingui_impl_nvidia_wndproc.hpp # NVIDIA 窗口消息转发辅助
│   ├── raingui_impl_win32.*      # Win32 后端
│   ├── raingui_impl_dx11.*       # DX11 后端
│   ├── raingui_impl_dx9/10/12.*  # 其他图形 API 后端
│   ├── raingui_comm.*            # 共享内存通信模块
│   ├── raingui_ui_example.cpp    # UI 示例代码
│   ├── README.md                 # RainGui 子目录补充说明
│   └── pch.*                     # 预编译头
└── README.md                     # 本文件
```

---

## 依赖

- Visual Studio 2022 (MSVC x64)
- Windows 10/11
- NVIDIA GeForce Experience（覆盖层已开启）
- 内核注入器（用于手动映射注入）

---

## 快速开始

### 方式 1：单独使用 NVIDIA 后端

```cpp
#include "raingui.h"
#include "raingui_impl_nvidia.h"

// 初始化
RainGui::CreateContext();
RainGui_Nvidia_Init();

// 渲染循环
while (running)
{
    RainGui_Nvidia_NewFrame();
    RainGui::NewFrame();

    // 绘制 GUI
    RainGui::Begin("My Window");
    RainGui::Text("Hello, RainGui!");
    RainGui::End();

    RainGui::Render();
    RainGui_Nvidia_Present();
}

// 清理
RainGui_Nvidia_Shutdown();
RainGui::DestroyContext();
```

### 方式 2：使用共享内存通信

**注入端（nv_render.dll）：**
```cpp
#include "raingui.h"
#include "raingui_impl_nvidia.h"
#include "raingui_comm.h"

RainGui::CreateContext();
RainGui_Nvidia_Init();  // 自动初始化共享内存通信

while (running)
{
    RainGui_Nvidia_NewFrame();
    RainGui::NewFrame();

    // 显示通信状态
    if (RainGui_Comm_IsValid())
    {
        const RainGuiCommData* data = RainGui_Comm_GetData();
        RainGui::Text("帧 ID: %u", data->frameId);
        RainGui::Text("绘制命令: %u", data->commandCount);
    }

    RainGui::Render();
    RainGui_Nvidia_Present();  // 自动绘制共享内存中的命令
}

RainGui_Nvidia_Shutdown();
```

**外部进程（发送绘制命令）：**
```cpp
#include "NvidiaComm.hpp"

// 初始化
NvidiaComm::Initialize();

// 每帧发送绘制命令
while (running)
{
    NvidiaComm::BeginFrame();

    // 绘制点
    NvidiaComm::AddPoint(100, 100, 5.0f, IM_COL32(255, 0, 0, 255));

    // 绘制线
    NvidiaComm::AddLine(100, 100, 200, 200, IM_COL32(0, 255, 0, 255), 2.0f);

    // 绘制圆
    NvidiaComm::AddCircle(960, 540, 100.0f, IM_COL32(255, 255, 0, 255), 2.0f);

    // 绘制文字
    NvidiaComm::AddText(100, 50, "Hello from external process!", IM_COL32(255, 255, 255, 255));

    NvidiaComm::EndFrame();
    Sleep(16);  // 60 FPS
}

NvidiaComm::Shutdown();
```

---

## 编译

参考外部项目示例（如 NvOverlay）的 `build.bat`：

```bat
@echo off
call "D:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64

set RAINGUI_DIR=path\to\RainGui-main\RainGui

cl /nologo /O2 /MT /EHsc /W3 /LD /I%RAINGUI_DIR% ^
   /Fe:nv_render.dll ^
   nv_render.cpp ^
   %RAINGUI_DIR%\raingui_impl_nvidia.cpp ^
   %RAINGUI_DIR%\raingui_comm.cpp ^
   %RAINGUI_DIR%\raingui.cpp ^
   %RAINGUI_DIR%\raingui_draw.cpp ^
   %RAINGUI_DIR%\raingui_tables.cpp ^
   %RAINGUI_DIR%\raingui_widgets.cpp ^
   %RAINGUI_DIR%\raingui_impl_dx11.cpp ^
   %RAINGUI_DIR%\raingui_impl_win32.cpp ^
   /link /DLL d3d11.lib dxgi.lib dwmapi.lib user32.lib gdi32.lib advapi32.lib
```

---

## 使用场景

### 1. 游戏 Overlay（单进程）
直接注入 DLL，使用 NVIDIA 后端绘制 UI。

### 2. ESP / Aimbot（跨进程）
- 外部进程读取游戏内存，计算骨骼坐标
- 通过共享内存发送绘制命令
- 注入的 DLL 接收命令并绘制到屏幕

### 3. 调试工具
实时显示游戏内部状态、性能指标等。

---

## 共享内存通信协议

### 架构说明

```
外部进程 (OutOverlay.exe)
├─ 创建共享内存
├─ 初始化配置变量
├─ 每帧写入绘制命令
└─ 读取 NVIDIA 端用户修改的配置

注入进程 (nv_render.dll)
├─ 打开共享内存
├─ 显示固定 UI（读写配置变量）
└─ 绘制共享内存中的命令
```

### 共享内存结构

```cpp
struct RainGuiCommData
{
    DWORD magic;              // 0x52474349
    DWORD version;            // 3
    DWORD frameId;            // 帧 ID
    ULONGLONG heartbeatTick;  // 发送端心跳
    char processName[64];     // 通信进程名

    // 配置变量（双向读写）
    RainGuiConfig config;
    {
        BYTE espEnabled;
        BYTE drawPlayerSkeleton;
        BYTE drawZombieSkeleton;
        BYTE drawNames;
        BYTE drawCollision;
        BYTE actorFilterEnabled;
        BYTE aimbotEnabled;
        BYTE drawFOV;
        float fovRadius;
        float smoothing;
        float deadZone;
        DWORD tickDelay;
        DWORD targetBone;
        DWORD aimKey;
        BYTE showUI;
        BYTE reserved[3];
        char actorClassFilter[64];
    }

    // 绘制命令（单向写入）
    DWORD commandCount;
    RainGuiDrawCommand commands[32768];
};
```

### 支持的绘制命令

- `RGUI_CMD_POINT` - 圆点
- `RGUI_CMD_LINE` - 线段
- `RGUI_CMD_CIRCLE` - 圆圈（空心/填充）
- `RGUI_CMD_RECT` - 矩形（空心/填充）
- `RGUI_CMD_TEXT` - 文字

### 配置同步

- ESP 配置（骨骼、名称、碰撞体）
- Actor 过滤配置（过滤开关、类名关键字）
- Aimbot 配置（FOV、平滑度、目标骨骼）
- UI 状态
- 心跳与通信进程名（用于离线检测和状态显示）

### 工作流程

```
外部进程                          注入进程
    ↓                                ↓
BeginFrame()                    读取 commandCount
    ↓                                ↓
AddPoint/AddLine/...            遍历 commands[]
    ↓                                ↓
EndFrame()                      绘制到屏幕
    ↓                                ↓
读取 config                     用户修改 UI
    ↓                                ↓
应用配置到逻辑  ←──────────  写入 config
```

---

## 注意事项

1. **注入方式**：推荐使用内核手动映射注入，避免被检测
2. **权限**：共享内存已配置安全描述符，允许跨进程访问
3. **启动顺序**：DLL 和外部进程启动顺序任意，自动创建/打开共享内存
4. **性能**：共享内存通信延迟 < 1μs，32768 条命令缓冲适合复杂场景

---

## 许可

基于 ImGui 修改，遵循 MIT 许可证。
