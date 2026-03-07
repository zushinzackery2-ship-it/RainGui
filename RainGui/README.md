# RainGui

RainGui 是基于当前 `Imgui` 目录私有化改造后的 UI 分支，目标是尽量降低标准 RainGui 的可见特征，并以 DLL 形式产出供外部模块使用。

## 当前状态

- 品牌标识已统一为 `RainGui`
- 默认不再自动生成 `raingui.ini`
- 默认日志名为 `raingui_log.txt`
- 默认禁用 Demo 和 Metrics 窗口
- `ImDrawVert` 已改为非官方默认布局
- DX9 / DX10 / DX11 / DX12 backend 已同步 RainGui 品牌名
- 当前默认产物为 `RainGui.dll`

## 目录内重要文件

- `build.bat`
  - 使用 Visual Studio 2022 Developer Command Prompt 构建 DLL
- `imconfig.h`
  - DLL 导出宏、demo/metrics 配置、顶点布局配置
- `raingui.cpp`
  - 默认 ini / log 行为
- `raingui_draw.cpp`
  - 默认字体入口与字体可见标识
- `raingui_impl_dx9.cpp`
- `raingui_impl_dx10.cpp`
- `raingui_impl_dx11.cpp`
- `raingui_impl_dx12.cpp`
  - 各后端 RainGui 品牌与渲染接入实现

## 构建方式

### 环境要求

- Visual Studio 2022
- VS 开发者命令提示符：
  - `D:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat`

### 构建命令

在 PowerShell 中进入 `e:\lapian\Imgui` 后运行：

```powershell
.\build.bat
```

### 构建产物

构建成功后会生成：

```text
bin\RainGui.dll
bin\RainGui.lib
bin\RainGui.exp
```

## DLL 导出说明

`imconfig.h` 中通过 `RAINGUI_BUILD_DLL` 控制导出方向：

- 构建 DLL 时定义：`RAINGUI_BUILD_DLL`
- 使用 DLL 的调用方不要定义该宏

当前 DLL 使用 `__declspec(dllexport)` / `__declspec(dllimport)` 方式导出与导入。

## 接入方式

### 调用方编译时

- 包含当前目录下的头文件
- 链接：`RainGui.lib`
- 运行时放置：`RainGui.dll`

### 运行时注意事项

由于这是 DLL 形态，调用方需要注意 RainGui 原本的跨模块约束：

- 必要时显式调用 `RainGui::SetCurrentContext()`
- 必要时显式调用 `RainGui::SetAllocatorFunctions()`

尤其在多模块、注入场景或自定义分配器场景下，这一点必须自测确认。

## 当前已做的主要改造

### 品牌与运行时标识

- `RainGui_Win32`
- `RainGui_DX9`
- `RainGui_DX10`
- `RainGui_DX11`
- `RainGui_DX12`
- `RainGuiDefault.ttf`
- `raingui_log.txt`

### 默认配置

- 默认关闭 `raingui.ini` 自动落盘
- 默认关闭 Demo / Metrics 窗口

### 顶点布局

当前 `ImDrawVert` 已改为：

```cpp
struct ImDrawVert
{
    ImVec2 pos;
    ImU32 col;
    ImVec2 uv;
    ImU32 meta;
};
```

这会改变默认 stride 与 backend 输入布局特征。

## VMP 使用建议

RainGui 当前设计为先完成源码层去特征，再对最终 DLL 进行保护。

### 建议流程

1. 先构建出 `RainGui.dll`
2. 确认调用方能够正常加载和渲染
3. 再对 `RainGui.dll` 做 VMP 保护

### 注意事项

- 一般情况下，VMP 不会自动破坏导出表
- 但如果对导出边界、入口点、初始化路径保护过重，可能影响 DLL 加载或导出调用
- 建议优先保护内部逻辑，不要一开始就重度处理导出边界

### 最少自测项

做完 VMP 后，至少验证以下几点：

- `LoadLibrary` 是否正常
- 导出函数是否可调用
- `CreateContext / NewFrame / Render` 是否正常
- 字体上传是否正常
- 首帧渲染是否正常

## 当前仍然可能存在的源码层痕迹

当前仍可能存在以下仅源码层或注释层残留：

- `Dear RainGui`
- `dear raingui`
- `raingui_*` 文件名
- `imstb_*` 中的注释标记

这类痕迹主要影响源码层识别，不影响当前 DLL 的构建与基本运行。

## 后续可继续增强的方向

如果还要继续抬强度，可继续考虑：

- 替换默认字体数据本体
- 继续减少会进入二进制的固定字面量
- 进一步私有化头文件与文件名
- 为调用方补一层更薄的 RainGui 专用导出封装

## 当前状态总结

- RainGui DLL 已可构建
- 全部主要后端已同步品牌与布局
- 当前版本可直接用于接入与后续 VMP 测试
