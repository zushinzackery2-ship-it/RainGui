# RainGui

轻量级即时模式 GUI 与后端辅助封装库。

## 定位

- GUI 层，不是底层图形 Hook 核心
- 提供 Win32 / D3D 后端接线
- 提供默认 overlay 风格和导出封装
- 保留 `NVIDIA Overlay` 和共享内存通信相关模块

## 目录

```text
RainGui/
├── RainGui/                 核心源码
├── RainGuiDx11HookTest/     DX11 测试工程
└── RainGuiDx12HookTest/     DX12 测试工程
```

`RainGui/` 目录主要包含：

- `raingui.*`
- `raingui_impl_win32.*`
- `raingui_impl_dx9/10/11/12.*`
- `raingui_defaults.*`
- `raingui_impl_nvidia.*`
- `raingui_comm.*`
- `raingui_exports.*`

## 依赖关系

- `RainGui` 现在不再承载 DX11 / DX12 / Vulkan Hook 核心实现
- Hook/runtime 探测已经下沉到 `Universal-Render-Hook` 和 `VulkanHook`
- 上层业务可以同时依赖它们，但不该再把 `RainGui` 当底层 Hook 库

## 当前构建依赖

如果要构建 `RainGui.dll` 的统一导出版本，默认会按当前同级目录查找：

- `../Universal-Render-Hook/URH`
- `../VulkanHook/VulkanHook`

也可以通过环境变量覆盖：

```powershell
$env:URH_ROOT = 'F:\Deps\Universal-Render-Hook\URH'
$env:VKH_ROOT = 'F:\Deps\VulkanHook\VulkanHook'
.\RainGui\build.bat
```

底层原生类型已经改为 `Urh* / Vkh*` 作为规范名。
`RainGui*` 命名只保留在 `RainGui` 自己的转发头和导出层里。

## 使用方式

当前更推荐源码级接入，按需把需要的 `.cpp/.h` 编进业务工程。

## 许可

MIT，见根目录 `LICENSE`。  
本仓库基于 Dear ImGui 整理维护，公开发布时建议保留上游许可证注释。
