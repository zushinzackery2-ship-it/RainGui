# RainGui

ImGui 魔改版，API 基本对齐，内置 NVIDIA Overlay 钩子后端。

- 全量重命名 `RainGui::`、`RainGui*`、`RAINGUI_*`
- 内置 `raingui_impl_nvidia`：自动 NVIDIA 窗口劫持 + D3D11 + 输入 + 反截图
- `RainGui_Nvidia_Init()` 一行初始化

## 项目结构

```
├── RainGui/            # RainGui 核心库 + 后端
│   ├── raingui.*               # 核心源码（ImGui 魔改）
│   ├── raingui_draw.cpp        # 绘制
│   ├── raingui_tables.cpp      # 表格
│   ├── raingui_widgets.cpp     # 控件
│   ├── raingui_internal.h      # 内部头文件
│   ├── raingui_impl_nvidia.*   # NVIDIA Overlay 钩子后端（自动安装）
│   ├── raingui_impl_win32.*    # Win32 后端
│   ├── raingui_impl_dx11.*     # DX11 后端
│   ├── raingui_impl_dx9/10/12  # 其他图形 API 后端
│   ├── raingui_exports.*       # DLL 导出层
│   ├── build.bat               # RainGui.dll 构建脚本
│   └── test/                   # 测试工程
├── NvOverlay/          # 注入 DLL 示例 + 注入器
│   ├── src/nv_render.cpp       # 注入 DLL（使用 NVIDIA 后端）
│   ├── src/injector.cpp        # 用户态注入器
│   └── build.bat               # 构建脚本
├── DCompOverlay/       # DirectComposition 覆盖层实验
├── NvHijack/           # NVIDIA 窗口劫持实验
└── OP_Plan.md          # 硬件 Overlay Plane (MPO) 研究计划
```

## 核心功能

- **截图不可见** — `SetWindowDisplayAffinity(WDA_EXCLUDEFROMCAPTURE)`
- **无新窗口/进程** — 劫持 NVIDIA 已有的 `CEF-OSC-WIDGET` 窗口
- **RainGui 渲染** — 群青色主题 + 微软雅黑字体 + DPI 感知
- **智能点击穿透** — 鼠标在 GUI 上可交互，移开自动穿透
- **任务栏兼容** — 窗口高度 -1px，不遮挡任务栏
- **Alt+Z 兼容** — 每帧强制维持窗口尺寸，NVIDIA 重置后自动恢复

## 依赖

- Visual Studio 2022 (MSVC x64)
- Windows 10/11
- NVIDIA GeForce Experience（覆盖层已开启）
- 内核注入器（用户自备）

## 构建

```bat
cd NvOverlay
build.bat
```

产出：
- `bin/nv_render.dll` — 注入 DLL
- `bin/injector.exe` — 用户态注入器（备用）

## 使用

1. 确保 NVIDIA GeForce Experience 运行且覆盖层已开启
2. 用内核注入器将 `nv_render.dll` 注入到 `NVIDIA Overlay.exe`（窗口类名 `CEF-OSC-WIDGET`）
3. 屏幕出现群青色 RainGui 窗口
4. 按 **END** 退出覆盖层

## RainGui NVIDIA 后端 API

```cpp
#include "raingui.h"
#include "raingui_impl_nvidia.h"

RainGui::CreateContext();
RainGui_Nvidia_Init();          // 一键初始化

while (running)
{
    RainGui_Nvidia_NewFrame();  // 维护窗口 + 输入
    RainGui::NewFrame();
    // 绘制 GUI...
    RainGui::Render();
    RainGui_Nvidia_Present();   // 渲染 + 呈现
}

RainGui_Nvidia_Shutdown();      // 清理 + 恢复窗口
```
