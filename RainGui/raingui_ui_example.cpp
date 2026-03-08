// RainGui NVIDIA 后端 - UI 示例
// 展示如何使用共享内存配置创建 UI

#include "raingui.h"
#include "raingui_comm.h"

// 在 nv_render.dll 的渲染循环中调用
void RainGui_Nvidia_RenderUI()
{
    if (!RainGui_Comm_IsValid())
        return;

    RainGuiConfig* config = RainGui_Comm_GetConfig();
    if (!config)
        return;

    // 只有在 showUI 为 true 时才显示 UI
    if (!config->showUI)
        return;

    // ESP 设置窗口
    RainGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
    RainGui::SetNextWindowSize(ImVec2(350, 400), ImGuiCond_FirstUseEver);

    if (RainGui::Begin("ESP & Aimbot Settings", nullptr, ImGuiWindowFlags_NoCollapse))
    {
        // ESP 设置
        if (RainGui::CollapsingHeader("ESP Settings", ImGuiTreeNodeFlags_DefaultOpen))
        {
            bool espEnabled = config->espEnabled != 0;
            if (RainGui::Checkbox("Enable ESP", &espEnabled))
            {
                config->espEnabled = espEnabled ? 1 : 0;
            }

            if (espEnabled)
            {
                RainGui::Indent();

                bool drawPlayerSkeleton = config->drawPlayerSkeleton != 0;
                if (RainGui::Checkbox("Draw Player Skeleton", &drawPlayerSkeleton))
                {
                    config->drawPlayerSkeleton = drawPlayerSkeleton ? 1 : 0;
                }

                bool drawZombieSkeleton = config->drawZombieSkeleton != 0;
                if (RainGui::Checkbox("Draw Zombie Skeleton", &drawZombieSkeleton))
                {
                    config->drawZombieSkeleton = drawZombieSkeleton ? 1 : 0;
                }

                bool drawNames = config->drawNames != 0;
                if (RainGui::Checkbox("Draw Names", &drawNames))
                {
                    config->drawNames = drawNames ? 1 : 0;
                }

                bool drawCollision = config->drawCollision != 0;
                if (RainGui::Checkbox("Draw Collision", &drawCollision))
                {
                    config->drawCollision = drawCollision ? 1 : 0;
                }

                RainGui::Unindent();
            }
        }

        RainGui::Separator();

        // Aimbot 设置
        if (RainGui::CollapsingHeader("Aimbot Settings", ImGuiTreeNodeFlags_DefaultOpen))
        {
            bool aimbotEnabled = config->aimbotEnabled != 0;
            if (RainGui::Checkbox("Enable Aimbot", &aimbotEnabled))
            {
                config->aimbotEnabled = aimbotEnabled ? 1 : 0;
            }

            if (aimbotEnabled)
            {
                RainGui::Indent();

                bool drawFOV = config->drawFOV != 0;
                if (RainGui::Checkbox("Draw FOV Circle", &drawFOV))
                {
                    config->drawFOV = drawFOV ? 1 : 0;
                }

                RainGui::SliderFloat("FOV Radius", &config->fovRadius, 20.0f, 500.0f, "%.0f");
                RainGui::SliderFloat("Smoothing", &config->smoothing, 1.0f, 30.0f, "%.1f");
                RainGui::SliderFloat("Dead Zone", &config->deadZone, 0.0f, 10.0f, "%.1f");

                int tickDelay = (int)config->tickDelay;
                if (RainGui::SliderInt("Tick Delay (ms)", &tickDelay, 0, 15))
                {
                    config->tickDelay = (DWORD)tickDelay;
                }

                const char* boneNames[] = { "Head", "Neck" };
                int targetBone = (int)config->targetBone;
                if (RainGui::Combo("Target Bone", &targetBone, boneNames, 2))
                {
                    config->targetBone = (DWORD)targetBone;
                }

                // 自瞄按键选择
                const char* keyNames[] =
                {
                    "Left Mouse", "Right Mouse", "Middle Mouse",
                    "Mouse 4", "Mouse 5", "Shift", "Ctrl"
                };
                DWORD keyValues[] =
                {
                    VK_LBUTTON, VK_RBUTTON, VK_MBUTTON,
                    VK_XBUTTON1, VK_XBUTTON2, VK_SHIFT, VK_CONTROL
                };

                int currentKeyIndex = 0;
                for (int i = 0; i < 7; ++i)
                {
                    if (keyValues[i] == config->aimKey)
                    {
                        currentKeyIndex = i;
                        break;
                    }
                }

                if (RainGui::Combo("Aim Key", &currentKeyIndex, keyNames, 7))
                {
                    config->aimKey = keyValues[currentKeyIndex];
                }

                RainGui::Unindent();
            }
        }

        RainGui::Separator();

        // 快捷键提示
        RainGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Hotkeys:");
        RainGui::BulletText("INS - Toggle UI");
        RainGui::BulletText("F2 - Quick Toggle Aimbot");
    }

    RainGui::End();
}
