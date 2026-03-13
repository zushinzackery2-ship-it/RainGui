# RainGui

`RainGui` 是一个源码库仓库，核心内容是 `RainGui/` 下的源码与模块；`DLL / LIB` 只是构建产物。当前同时保留两条接入路线：

- `RainGui/`
  - 核心库，包含 RainGui 本体、Win32/DX9/DX10/DX11/DX12 backend、NVIDIA overlay backend、共享内存通信和新的 DX12Hook 模块
- `RainGuiDx12HookTest/`
  - 独立 DX12Hook 测试载荷，`DllMain` 内自动调用 `RainGui.dll` 的 DX12Hook API，适合 CE 或普通 LoadLibrary 注入做烟雾测试

## 当前重点

- 所有品牌标识已经统一为 `RainGui`
- `RainGui.dll` 默认不生成 `raingui.ini`
- 新增模块化 `DX12Hook`
  - VMT hook
  - `Present / ResizeBuffers / ResizeBuffers1 / ExecuteCommandLists`
  - 双阶段 `CommandQueue` 捕获
  - Fence 同步
  - Device lost / swapchain 热切换处理
- 新增统一默认外观入口
  - `RainGui_ApplyOverlayDefaults()`
- 新增独立测试载荷
  - `RainGuiDx12HookTest.dll`

## 目录结构

```text
RainGui-main/
├── RainGui/
│   ├── DX12Hook/                    # DX12Hook 内部实现
│   ├── raingui.*                    # RainGui 核心源码
│   ├── raingui_impl_dx9/10/11/12.*  # 图形 backend
│   ├── raingui_impl_nvidia.*        # NVIDIA overlay backend
│   ├── raingui_impl_win32.*         # Win32 backend
│   ├── raingui_comm.*               # 共享内存通信
│   ├── raingui_exports.*            # DLL 导出层
│   ├── raingui_defaults.*           # 统一默认外观配置
│   └── README.md
├── RainGuiDx12HookTest/
│   ├── raingui_dx12hook_test.cpp    # 独立测试载荷
│   └── build.bat
└── README.md
```

## 构建

环境要求：

- Visual Studio 2022
- `D:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat`

构建核心库：

```powershell
cd .\RainGui
.\build.bat
```

输出：

```text
RainGui\bin\RainGui.dll
RainGui\bin\RainGui.lib
```

构建独立测试载荷：

```powershell
cd .\RainGuiDx12HookTest
.\build.bat
```

输出：

```text
RainGuiDx12HookTest\bin\RainGuiDx12HookTest.dll
```

## DX12Hook 烟雾测试

最简单的测试方式不是直接执行 `RainGui.dll` 导出，而是注入独立测试载荷：

- 注入 `RainGuiDx12HookTest.dll`
- 载荷会自动加载同仓库下的 `RainGui\bin\RainGui.dll`
- 成功后会显示内置 `RainGui DX12Hook` 调试窗口
- `INSERT` 切换显示
- `END` 卸载

这个载荷只负责启动库，不承载正式业务逻辑，目的是把“能否稳定 hook 成功”与“业务 UI 是否写对”拆开验证。

## 正式接入方向

正式接入时，调用方直接链接 `RainGui.lib`，然后使用 `RainGui_DX12Hook_Init()`：

- `onSetup`
  - 初始化样式、字体或业务状态
- `onRender`
  - 每帧绘制自己的菜单和调试内容
- `isVisible`
  - 控制交互可见性
- `onShutdown`
  - 清理业务侧状态

独立测试载荷只保留给注入验证，不建议拿去承载正式功能。

## 许可

基于 ImGui 修改，遵循 MIT 许可证。
