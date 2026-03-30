<div align="center">

# RainGui

**轻量级即时模式 GUI 与后端辅助封装库**

*Immediate Mode UI | Win32 / D3D Backends | Forwarding API*

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

## 目录

```text
RainGui/
├── RainGui/                 核心源码
├── RainGuiDx11HookTest/     DX11 测试工程
└── RainGuiDx12HookTest/     DX12 测试工程
```

## 依赖关系

| 方向 | 说明 |
|:-----|:-----|
| `RainGui -> Universal-Render-Hook` | DX11 / DX12 / AutoHook 转发层 |
| `RainGui -> VulkanHook` | Vulkan 转发层 |
| `RainGui <- InterRec` | 可选上层业务使用方 |

## 构建

统一导出版 `RainGui.dll` 默认按兄弟目录查找依赖：

- `../Universal-Render-Hook/URH`
- `../VulkanHook/VulkanHook`

也可以通过环境变量覆盖：

```powershell
$env:URH_ROOT = 'F:\Deps\Universal-Render-Hook\URH'
$env:VKH_ROOT = 'F:\Deps\VulkanHook\VulkanHook'
.\RainGui\build.bat
```

## 命名策略

- 底层原生类型用 `Urh* / Vkh*`
- `RainGui*` 命名只保留在本仓的转发头和导出层
- 上层如果只需要 UI，可以不直接接触底层命名

## 许可

MIT，见根目录 `LICENSE`
