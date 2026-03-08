# RainGui 共享内存通信 - 使用指南

## 方案 3：混合方案（配置变量 + 绘制命令）

---

## 架构说明

```
OutOverlay.exe (外部进程)
├─ 创建共享内存
├─ 初始化配置变量
├─ 每帧写入绘制命令
└─ 读取 NVIDIA 端用户修改的配置

NVIDIA Overlay.exe (被注入)
├─ 打开共享内存
├─ 显示固定 UI（读写配置变量）
└─ 绘制共享内存中的命令
```

---

## 共享内存结构

```cpp
struct RainGuiCommData
{
    DWORD magic;              // 0x52474349
    DWORD version;            // 1
    DWORD frameId;            // 帧 ID

    // 配置变量（双向读写）
    RainGuiConfig config;
    {
        BYTE espEnabled;
        BYTE drawPlayerSkeleton;
        BYTE drawZombieSkeleton;
        BYTE drawNames;
        BYTE drawCollision;
        BYTE aimbotEnabled;
        BYTE drawFOV;
        float fovRadius;
        float smoothing;
        float deadZone;
        DWORD tickDelay;
        DWORD targetBone;
        DWORD aimKey;
        BYTE showUI;
    }

    // 绘制命令（单向写入）
    DWORD commandCount;
    RainGuiDrawCommand commands[4096];
};
```

---

## OutOverlay 端使用

### 1. 初始化

```cpp
#include "Communication/NvidiaComm.hpp"

int main()
{
    // 初始化共享内存
    if (!NvidiaComm::Initialize())
    {
        std::cerr << "NvidiaComm 初始化失败\n";
        return 1;
    }

    // 可选：设置初始配置
    RainGuiConfig* config = NvidiaComm::GetConfig();
    config->espEnabled = 1;
    config->fovRadius = 100.0f;
    config->showUI = 1;

    // ... 主循环 ...

    NvidiaComm::Shutdown();
    return 0;
}
```

### 2. 每帧绘制

```cpp
while (!g_Done)
{
    // 读取配置（用户可能在 NVIDIA UI 中修改了）
    RainGuiConfig* config = NvidiaComm::GetConfig();
    bool espEnabled = config->espEnabled;
    bool aimbotEnabled = config->aimbotEnabled;
    float fovRadius = config->fovRadius;

    // 开始新帧
    NvidiaComm::BeginFrame();

    // 根据配置绘制
    if (espEnabled)
    {
        // 绘制玩家骨骼（绿色）
        if (config->drawPlayerSkeleton)
        {
            for (auto& bone : playerBones)
            {
                NvidiaComm::AddPoint(
                    bone.screenX, bone.screenY,
                    3.0f,
                    IM_COL32(0, 255, 0, 255),
                    bone.visible
                );
            }

            // 绘制骨骼连线
            NvidiaComm::AddLine(
                bone1.screenX, bone1.screenY,
                bone2.screenX, bone2.screenY,
                IM_COL32(0, 255, 0, 255),
                2.0f,
                bone1.visible && bone2.visible
            );
        }

        // 绘制僵尸骨骼（红色）
        if (config->drawZombieSkeleton)
        {
            for (auto& actor : actors)
            {
                for (auto& bone : actor.bones)
                {
                    NvidiaComm::AddPoint(
                        bone.screenX, bone.screenY,
                        3.0f,
                        IM_COL32(255, 0, 0, 255),
                        bone.visible
                    );
                }
            }
        }

        // 绘制名称
        if (config->drawNames)
        {
            NvidiaComm::AddText(
                actor.screenX, actor.screenY - 20,
                actor.name.c_str(),
                IM_COL32(255, 255, 255, 255)
            );
        }
    }

    // 绘制 Aimbot FOV
    if (aimbotEnabled && config->drawFOV)
    {
        NvidiaComm::AddCircle(
            screenWidth / 2.0f,
            screenHeight / 2.0f,
            fovRadius,
            IM_COL32(255, 255, 255, 128),
            2.0f,
            true
        );
    }

    // 结束帧
    NvidiaComm::EndFrame();

    Sleep(16);  // 60 FPS
}
```

### 3. 响应配置变化

```cpp
// 读取用户在 NVIDIA UI 中修改的配置
RainGuiConfig* config = NvidiaComm::GetConfig();

// 应用到本地逻辑
OverlayConfig::g_esp.enabled = config->espEnabled;
OverlayConfig::g_esp.drawPlayerSkeleton = config->drawPlayerSkeleton;
OverlayConfig::g_esp.drawZombieSkeleton = config->drawZombieSkeleton;
OverlayConfig::g_aimbot.enabled = config->aimbotEnabled;
OverlayConfig::g_aimbot.fovRadius = config->fovRadius;
OverlayConfig::g_aimbot.smoothing = config->smoothing;
```

---

## NVIDIA 端使用

### 1. 初始化（自动）

```cpp
// 在 nv_render.dll 中
RainGui::CreateContext();
RainGui_Nvidia_Init();  // 自动调用 RainGui_Comm_Init()
```

### 2. 渲染循环

```cpp
// 参考 raingui_ui_example.cpp
void RainGui_Nvidia_RenderUI();

while (running)
{
    RainGui_Nvidia_NewFrame();
    RainGui::NewFrame();

    // 渲染 UI（读写配置）
    RainGui_Nvidia_RenderUI();

    RainGui::Render();
    RainGui_Nvidia_Present();  // 自动绘制共享内存中的命令
}

RainGui_Nvidia_Shutdown();  // 自动调用 RainGui_Comm_Shutdown()
```

### 3. UI 示例（已实现）

```cpp
void RainGui_Nvidia_RenderUI()
{
    RainGuiConfig* config = RainGui_Comm_GetConfig();
    if (!config || !config->showUI)
        return;

    if (RainGui::Begin("ESP & Aimbot Settings"))
    {
        // ESP 设置
        bool espEnabled = config->espEnabled;
        if (RainGui::Checkbox("Enable ESP", &espEnabled))
        {
            config->espEnabled = espEnabled ? 1 : 0;
        }

        // Aimbot 设置
        bool aimbotEnabled = config->aimbotEnabled;
        if (RainGui::Checkbox("Enable Aimbot", &aimbotEnabled))
        {
            config->aimbotEnabled = aimbotEnabled ? 1 : 0;
        }

        RainGui::SliderFloat("FOV Radius", &config->fovRadius, 20.0f, 500.0f);
        RainGui::SliderFloat("Smoothing", &config->smoothing, 1.0f, 30.0f);
    }
    RainGui::End();
}
```

---

## 工作流程

### 1. 启动顺序

```
1. 启动 OutOverlay.exe
   └─ 创建共享内存 "Global\\RainGuiComm"
   └─ 初始化配置变量

2. 注入 nv_render.dll 到 NVIDIA Overlay.exe
   └─ 打开共享内存 "Global\\RainGuiComm"
   └─ 读取配置变量
   └─ 显示 UI
```

### 2. 运行时通信

```
OutOverlay.exe                    NVIDIA Overlay.exe
     ↓                                   ↓
BeginFrame()                       读取 commandCount
     ↓                                   ↓
AddPoint/AddLine/...               遍历 commands[]
     ↓                                   ↓
EndFrame()                         绘制到屏幕
     ↓                                   ↓
读取 config                        用户修改 UI
     ↓                                   ↓
应用配置到逻辑  ←─────────────  写入 config
```

### 3. 配置同步

```
用户在 NVIDIA UI 中修改 FOV → config.fovRadius = 200
                                      ↓
OutOverlay 读取 config.fovRadius → 应用到自瞄逻辑
```

---

## 优势

1. **UI 样式固定**：性能高，编译到 DLL 中
2. **配置动态**：用户可以在 NVIDIA UI 中实时修改
3. **绘制动态**：OutOverlay 可以动态控制绘制内容
4. **双向通信**：OutOverlay 读取配置，NVIDIA 写入配置

---

## 文件清单

### RainGui 端
- `raingui_comm.h` - 通信协议定义
- `raingui_comm.cpp` - 通信实现
- `raingui_impl_nvidia.cpp` - 集成通信模块
- `raingui_ui_example.cpp` - UI 示例

### OutOverlay 端
- `Communication/NvidiaComm.hpp` - 通信接口
- `Communication/NvidiaComm.cpp` - 通信实现

---

## 下一步

1. 编译 RainGui.dll（包含通信模块）
2. 修改 OutOverlay main.cpp（使用 NvidiaComm）
3. 测试通信和配置同步
