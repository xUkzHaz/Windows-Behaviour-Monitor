#include "Theme.h"

namespace Theme
{
    void ApplyDarkTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        const ImVec4 bgDark = ImVec4(0.08f, 0.08f, 0.10f, 1.00f);
        const ImVec4 bgMid = ImVec4(0.11f, 0.11f, 0.14f, 1.00f);
        const ImVec4 bgLight = ImVec4(0.15f, 0.15f, 0.19f, 1.00f);
        const ImVec4 accent = ImVec4(0.00f, 0.75f, 0.75f, 1.00f);
        const ImVec4 accentHover = ImVec4(0.10f, 0.85f, 0.85f, 1.00f);
        const ImVec4 text = ImVec4(0.90f, 0.90f, 0.95f, 1.00f);
        const ImVec4 textDim = ImVec4(0.55f, 0.55f, 0.60f, 1.00f);
        const ImVec4 red = ImVec4(0.85f, 0.25f, 0.25f, 1.00f);
        const ImVec4 yellow = ImVec4(0.90f, 0.75f, 0.20f, 1.00f);

        colors[ImGuiCol_WindowBg] = bgDark;
        colors[ImGuiCol_ChildBg] = bgMid;
        colors[ImGuiCol_PopupBg] = bgMid;
        colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.25f, 0.60f);
        colors[ImGuiCol_FrameBg] = bgLight;
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.25f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.25f, 0.30f, 1.00f);
        colors[ImGuiCol_TitleBg] = bgDark;
        colors[ImGuiCol_TitleBgActive] = bgMid;
        colors[ImGuiCol_TitleBgCollapsed] = bgDark;
        colors[ImGuiCol_MenuBarBg] = bgMid;
        colors[ImGuiCol_ScrollbarBg] = bgDark;
        colors[ImGuiCol_ScrollbarGrab] = bgLight;
        colors[ImGuiCol_ScrollbarGrabHovered] = accent;
        colors[ImGuiCol_ScrollbarGrabActive] = accentHover;
        colors[ImGuiCol_CheckMark] = accent;
        colors[ImGuiCol_SliderGrab] = accent;
        colors[ImGuiCol_SliderGrabActive] = accentHover;
        colors[ImGuiCol_Button] = bgLight;
        colors[ImGuiCol_ButtonHovered] = accent;
        colors[ImGuiCol_ButtonActive] = accentHover;
        colors[ImGuiCol_Header] = bgLight;
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.20f, 0.25f, 1.00f);
        colors[ImGuiCol_HeaderActive] = accent;
        colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.20f, 0.25f, 0.60f);
        colors[ImGuiCol_SeparatorHovered] = accent;
        colors[ImGuiCol_SeparatorActive] = accentHover;
        colors[ImGuiCol_ResizeGrip] = accent;
        colors[ImGuiCol_ResizeGripHovered] = accentHover;
        colors[ImGuiCol_ResizeGripActive] = accentHover;
        colors[ImGuiCol_Tab] = bgMid;
        colors[ImGuiCol_TabHovered] = accent;
        colors[ImGuiCol_TabSelected] = bgLight;
        colors[ImGuiCol_TabDimmed] = bgDark;
        colors[ImGuiCol_TabDimmedSelected] = bgMid;
        colors[ImGuiCol_Text] = text;
        colors[ImGuiCol_TextDisabled] = textDim;
        colors[ImGuiCol_TextSelectedBg] = ImVec4(accent.x, accent.y, accent.z, 0.35f);
        colors[ImGuiCol_DragDropTarget] = yellow;
        colors[ImGuiCol_NavHighlight] = accent;
        colors[ImGuiCol_NavWindowingHighlight] = accent;
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.00f, 0.75f, 0.75f, 0.40f); 
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.06f, 0.06f, 0.08f, 1.00f); 
        colors[ImGuiCol_TabHovered] = ImVec4(0.20f, 0.20f, 0.25f, 1.00f); 
        colors[ImGuiCol_TableHeaderBg] = bgMid;
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.20f, 0.20f, 0.25f, 1.00f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.20f, 0.20f, 0.25f, 0.60f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.03f);

    }

    void ApplyProfessionalStyle()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowPadding = ImVec2(10.0f, 10.0f);
        style.FramePadding = ImVec2(8.0f, 4.0f);
        style.CellPadding = ImVec2(8.0f, 4.0f);
        style.ItemSpacing = ImVec2(8.0f, 6.0f);
        style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
        style.IndentSpacing = 20.0f;
        style.ScrollbarSize = 14.0f;
        style.GrabMinSize = 10.0f;
        style.WindowBorderSize = 1.0f;
        style.ChildBorderSize = 1.0f;
        style.PopupBorderSize = 1.0f;
        style.FrameBorderSize = 0.0f;
        style.TabBorderSize = 0.0f;
        style.WindowRounding = 0.0f;
        style.ChildRounding = 2.0f;
        style.FrameRounding = 2.0f;
        style.PopupRounding = 2.0f;
        style.ScrollbarRounding = 6.0f;
        style.GrabRounding = 4.0f;
        style.TabRounding = 2.0f;
        style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
        style.WindowMenuButtonPosition = ImGuiDir_None;
        style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
        style.SelectableTextAlign = ImVec2(0.0f, 0.5f);
        style.SeparatorTextBorderSize = 1.0f;
        style.SeparatorTextPadding = ImVec2(20.0f, 3.0f);
    }
}