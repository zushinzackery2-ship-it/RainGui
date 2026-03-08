// RainGui NVIDIA 后端 - UI 实现
// 完整的 ESP & Aimbot 配置界面（群青色主题 + Tab 布局）

#include "raingui.h"
#include "raingui_comm.h"

static void RenderESPTab(RainGuiConfig* config);
static void RenderAimbotTab(RainGuiConfig* config);

// 在 nv_render.dll 的渲染循环中调用
void RainGui_Nvidia_RenderUI()
{
    if (!RainGui_Comm_IsValid())
        return;

    RainGuiConfig* config = RainGui_Comm_GetConfig();
    if (!config)
        return;

    if (!config->showUI)
        return;

    RainGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
    RainGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);

    if (RainGui::Begin("ESP & Aimbot [INS 开关菜单]", nullptr, 0))
    {
        RainGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "按 INSERT 键开关此菜单");
        RainGui::Separator();
        RainGui::Spacing();

        if (RainGui::BeginTabBar("MainTabs"))
        {
            if (RainGui::BeginTabItem("ESP 透视"))
            {
                RenderESPTab(config);
                RainGui::EndTabItem();
            }

            if (RainGui::BeginTabItem("Aimbot 自瞄"))
            {
                RenderAimbotTab(config);
                RainGui::EndTabItem();
            }

            RainGui::EndTabBar();
        }
    }
    RainGui::End();
}

static void RenderESPTab(RainGuiConfig* config)
{
    RainGui::TextColored(ImVec4(0.0f, 1.0f, 0.5f, 1.0f), "ESP 透视设置");
    RainGui::Separator();
    RainGui::Spacing();

    bool espEnabled = config->espEnabled != 0;
    if (RainGui::Checkbox("启用 ESP", &espEnabled))
    {
        config->espEnabled = espEnabled ? 1 : 0;
    }
    RainGui::Spacing();

    RainGui::Text("骨骼绘制选项：");
    bool drawPlayerSkeleton = config->drawPlayerSkeleton != 0;
    if (RainGui::Checkbox("绘制玩家骨骼 (绿色)", &drawPlayerSkeleton))
    {
        config->drawPlayerSkeleton = drawPlayerSkeleton ? 1 : 0;
    }

    bool drawZombieSkeleton = config->drawZombieSkeleton != 0;
    if (RainGui::Checkbox("绘制僵尸骨骼 (红色)", &drawZombieSkeleton))
    {
        config->drawZombieSkeleton = drawZombieSkeleton ? 1 : 0;
    }

    RainGui::Spacing();
    RainGui::Separator();
    RainGui::Spacing();

    RainGui::Text("Collision:");
    bool drawCollision = config->drawCollision != 0;
    if (RainGui::Checkbox("Collision Wireframe", &drawCollision))
    {
        config->drawCollision = drawCollision ? 1 : 0;
    }

    RainGui::Spacing();
    RainGui::Separator();
    RainGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "提示: 骨骼ESP会实时显示角色姿态");
}

static void RenderAimbotTab(RainGuiConfig* config)
{
    RainGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "自瞄设置");
    RainGui::Spacing();
    RainGui::TextColored(ImVec4(0.9f, 0.7f, 0.2f, 1.0f), "快捷键: F2 快速开关自瞄");
    RainGui::Spacing();

    bool aimbotEnabled = config->aimbotEnabled != 0;
    if (RainGui::Checkbox("启用自瞄", &aimbotEnabled))
    {
        config->aimbotEnabled = aimbotEnabled ? 1 : 0;
    }
    if (aimbotEnabled)
    {
        RainGui::SameLine();
        RainGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[已启用]");
    }

    bool drawFOV = config->drawFOV != 0;
    if (RainGui::Checkbox("显示 FOV 圆圈", &drawFOV))
    {
        config->drawFOV = drawFOV ? 1 : 0;
    }

    RainGui::Spacing();
    RainGui::Separator();
    RainGui::TextColored(ImVec4(0.3f, 0.8f, 1.0f, 1.0f), "FOV 视野设置");
    RainGui::SliderFloat("视野半径 (像素)", &config->fovRadius, 20.0f, 500.0f, "%.0f");

    RainGui::Spacing();
    RainGui::Separator();
    RainGui::TextColored(ImVec4(0.3f, 0.8f, 1.0f, 1.0f), "自瞄参数");
    RainGui::SliderFloat("平滑度 (1-30)", &config->smoothing, 1.0f, 30.0f, "%.1f");
    RainGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "数值越大越平滑，建议 5-15");

    RainGui::SliderFloat("死区 (像素)", &config->deadZone, 0.0f, 10.0f, "%.1f");
    RainGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "中心死区，避免微小抖动");

    int tickDelay = (int)config->tickDelay;
    if (RainGui::SliderInt("自瞄频率 (毫秒)", &tickDelay, 1, 15))
    {
        config->tickDelay = (DWORD)tickDelay;
    }
    RainGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "每次移动的延迟，1=最快");

    RainGui::Spacing();
    RainGui::Separator();
    RainGui::Text("自瞄激活按键:");

    const char* keyNames[] =
    {
        "鼠标左键", "鼠标右键", "鼠标中键",
        "鼠标侧键4", "鼠标侧键5", "Shift", "Ctrl"
    };
    DWORD keyValues[] =
    {
        VK_LBUTTON, VK_RBUTTON, VK_MBUTTON,
        VK_XBUTTON1, VK_XBUTTON2, VK_SHIFT, VK_CONTROL
    };

    int keyIndex = 0;
    for (int i = 0; i < 7; ++i)
    {
        if (keyValues[i] == config->aimKey)
        {
            keyIndex = i;
            break;
        }
    }

    if (RainGui::RadioButton(keyNames[0], &keyIndex, 0)) config->aimKey = keyValues[0];
    RainGui::SameLine();
    if (RainGui::RadioButton(keyNames[1], &keyIndex, 1)) config->aimKey = keyValues[1];
    RainGui::SameLine();
    if (RainGui::RadioButton(keyNames[2], &keyIndex, 2)) config->aimKey = keyValues[2];

    if (RainGui::RadioButton(keyNames[3], &keyIndex, 3)) config->aimKey = keyValues[3];
    RainGui::SameLine();
    if (RainGui::RadioButton(keyNames[4], &keyIndex, 4)) config->aimKey = keyValues[4];

    if (RainGui::RadioButton(keyNames[5], &keyIndex, 5)) config->aimKey = keyValues[5];
    RainGui::SameLine();
    if (RainGui::RadioButton(keyNames[6], &keyIndex, 6)) config->aimKey = keyValues[6];

    RainGui::Spacing();
    RainGui::Separator();
    RainGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "提示: 按住激活键将自动瞄准 FOV 内的目标");
}
