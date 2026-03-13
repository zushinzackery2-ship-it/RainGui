# RainGui

自用源码仓库。主体是 `RainGui/`，`DLL / LIB` 只是构建产物。

## 目录

- `RainGui/`
  - RainGui 核心源码
  - Win32 / DX9 / DX10 / DX11 / DX12 backend
  - NVIDIA overlay backend
  - 共享内存通信
  - DX12Hook 模块
- `RainGuiDx12HookTest/`
  - 独立测试载荷
  - `DllMain` 自动加载 `RainGui.dll` 并安装 DX12Hook

## 当前功能

- RainGui 品牌私有化
- 默认关闭 `raingui.ini`
- 默认日志名改为 `raingui_log.txt`
- `ImDrawVert` 使用私有布局
- 保留 NVIDIA overlay backend
- 保留共享内存通信模块
- 新增模块化 DX12Hook
  - VMT hook
  - `Present / ResizeBuffers / ResizeBuffers1 / ExecuteCommandLists`
  - `CommandQueue` 捕获
  - Fence 同步
  - Device lost / swapchain 重建
  - WndProc hook
- 新增统一默认外观入口
  - `RainGui_ApplyOverlayDefaults()`

## 构建

环境：

- Visual Studio 2022
- `D:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat`

构建库：

```powershell
cd .\RainGui
.\build.bat
```

产物：

```text
RainGui\bin\RainGui.dll
RainGui\bin\RainGui.lib
```

构建独立测试载荷：

```powershell
cd .\RainGuiDx12HookTest
.\build.bat
```

产物：

```text
RainGuiDx12HookTest\bin\RainGuiDx12HookTest.dll
```

## DX12Hook 测试

不要直接在 CE 里执行 `RainGui.dll` 导出。

直接注入：

- `RainGuiDx12HookTest.dll`

行为：

- 自动加载 `RainGui\bin\RainGui.dll`
- 自动安装 DX12Hook
- 成功后显示内置调试窗口
- `INSERT` 切换显示
- `END` 卸载

## 正式接入

正式接入直接链接 `RainGui.lib`，调用：

- `RainGui_DX12Hook_FillDefaultDesc`
- `RainGui_DX12Hook_Init`
- `RainGui_DX12Hook_Shutdown`
- `RainGui_DX12Hook_IsInstalled`
- `RainGui_DX12Hook_IsReady`
- `RainGui_DX12Hook_GetRuntime`

业务层只管：

- `onSetup`
- `onRender`
- `isVisible`
- `onShutdown`

## 许可

基于 ImGui 修改，遵循 MIT。
