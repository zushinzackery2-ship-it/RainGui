#include "raingui_defaults.h"

#include "raingui.h"

namespace RainGuiDefaults
{
    static ImVec4 LerpColor(const ImVec4& left, const ImVec4& right, float factor)
    {
        return ImVec4(
            left.x + (right.x - left.x) * factor,
            left.y + (right.y - left.y) * factor,
            left.z + (right.z - left.z) * factor,
            left.w + (right.w - left.w) * factor);
    }

    static void ApplyOverlayColors(RainGuiStyle& style)
    {
        ImVec4* colors = style.Colors;

        colors[RainGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
        colors[RainGuiCol_TextDisabled] = ImVec4(0.50f, 0.55f, 0.60f, 1.00f);
        colors[RainGuiCol_WindowBg] = ImVec4(0.08f, 0.09f, 0.12f, 0.95f);
        colors[RainGuiCol_ChildBg] = ImVec4(0.08f, 0.09f, 0.12f, 0.75f);
        colors[RainGuiCol_PopupBg] = ImVec4(0.10f, 0.11f, 0.15f, 0.98f);
        colors[RainGuiCol_Border] = ImVec4(0.18f, 0.22f, 0.30f, 1.00f);
        colors[RainGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

        colors[RainGuiCol_FrameBg] = ImVec4(0.18f, 0.20f, 0.26f, 1.00f);
        colors[RainGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.28f, 0.38f, 1.00f);
        colors[RainGuiCol_FrameBgActive] = ImVec4(0.18f, 0.33f, 0.56f, 1.00f);

        colors[RainGuiCol_TitleBg] = ImVec4(0.10f, 0.11f, 0.15f, 1.00f);
        colors[RainGuiCol_TitleBgActive] = ImVec4(0.14f, 0.15f, 0.20f, 1.00f);
        colors[RainGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.11f, 0.15f, 1.00f);
        colors[RainGuiCol_MenuBarBg] = ImVec4(0.09f, 0.10f, 0.14f, 1.00f);

        colors[RainGuiCol_ScrollbarBg] = ImVec4(0.07f, 0.08f, 0.11f, 0.85f);
        colors[RainGuiCol_ScrollbarGrab] = ImVec4(0.18f, 0.22f, 0.30f, 1.00f);
        colors[RainGuiCol_ScrollbarGrabHovered] = ImVec4(0.22f, 0.28f, 0.38f, 1.00f);
        colors[RainGuiCol_ScrollbarGrabActive] = ImVec4(0.20f, 0.50f, 0.90f, 0.90f);

        colors[RainGuiCol_CheckMark] = ImVec4(0.20f, 0.80f, 0.60f, 1.00f);
        colors[RainGuiCol_SliderGrab] = ImVec4(0.20f, 0.50f, 0.90f, 0.80f);
        colors[RainGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.60f, 0.98f, 0.90f);

        colors[RainGuiCol_Button] = ImVec4(0.20f, 0.50f, 0.90f, 0.80f);
        colors[RainGuiCol_ButtonHovered] = ImVec4(0.26f, 0.60f, 0.98f, 0.90f);
        colors[RainGuiCol_ButtonActive] = ImVec4(0.16f, 0.48f, 0.85f, 1.00f);

        colors[RainGuiCol_Header] = ImVec4(0.16f, 0.36f, 0.63f, 0.85f);
        colors[RainGuiCol_HeaderHovered] = ImVec4(0.22f, 0.46f, 0.76f, 0.95f);
        colors[RainGuiCol_HeaderActive] = ImVec4(0.18f, 0.40f, 0.69f, 1.00f);

        colors[RainGuiCol_Separator] = ImVec4(0.23f, 0.27f, 0.36f, 1.00f);
        colors[RainGuiCol_SeparatorHovered] = ImVec4(0.26f, 0.60f, 0.98f, 0.90f);
        colors[RainGuiCol_SeparatorActive] = ImVec4(0.16f, 0.48f, 0.85f, 1.00f);

        colors[RainGuiCol_ResizeGrip] = ImVec4(0.20f, 0.50f, 0.90f, 0.35f);
        colors[RainGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.60f, 0.98f, 0.80f);
        colors[RainGuiCol_ResizeGripActive] = ImVec4(0.16f, 0.48f, 0.85f, 1.00f);

        colors[RainGuiCol_Tab] = LerpColor(colors[RainGuiCol_Header], colors[RainGuiCol_TitleBgActive], 0.55f);
        colors[RainGuiCol_TabHovered] = colors[RainGuiCol_HeaderHovered];
        colors[RainGuiCol_TabActive] =
            LerpColor(colors[RainGuiCol_HeaderActive], colors[RainGuiCol_TitleBgActive], 0.35f);
        colors[RainGuiCol_TabUnfocused] =
            LerpColor(colors[RainGuiCol_Tab], colors[RainGuiCol_TitleBg], 0.35f);
        colors[RainGuiCol_TabUnfocusedActive] =
            LerpColor(colors[RainGuiCol_TabActive], colors[RainGuiCol_TitleBg], 0.20f);
    }

    void ApplyOverlayDefaults()
    {
        RainGuiStyle& style = RainGui::GetStyle();
        RainGui::StyleColorsDark(&style);

        // 这套默认值直接服务于注入式 overlay 场景，目的是让不同 hook 后端落出来的 UI 尺寸一致。
        style.WindowRounding = 6.0f;
        style.ChildRounding = 4.0f;
        style.FrameRounding = 4.0f;
        style.PopupRounding = 4.0f;
        style.ScrollbarRounding = 4.0f;
        style.GrabRounding = 4.0f;
        style.TabRounding = 4.0f;
        style.WindowPadding = ImVec2(18.0f, 16.0f);
        style.FramePadding = ImVec2(10.0f, 6.0f);
        style.ItemSpacing = ImVec2(12.0f, 10.0f);
        style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);
        style.IndentSpacing = 24.0f;
        style.WindowBorderSize = 1.0f;
        style.PopupBorderSize = 1.0f;

        RainGuiIO& io = RainGui::GetIO();
        io.FontGlobalScale = 1.0f;
        io.FontAllowUserScaling = false;

        ApplyOverlayColors(style);
    }
}
