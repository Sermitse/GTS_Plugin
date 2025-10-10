#include "UI/ImGui/Core/ImStyleManager.hpp"
#include "UI/ImGui/Lib/imgui.h"
#include "Config/Config.hpp"

namespace GTS {

    void ImStyleManager::InitializeDefaultStyle(ImGuiStyle& a_style) {
        // Rounding Settings
        a_style.WindowRounding = 2.5f;
        a_style.ChildRounding = 1.5f;
        a_style.FrameRounding = 1.5f;
        a_style.ScrollbarRounding = 1.5f;
        a_style.GrabRounding = 1.5f;
        a_style.TabRounding = 1.5f;

        a_style.GrabMinSize = 6.0f;

        // Anti-Aliasing
        a_style.AntiAliasedLines = true;
        a_style.AntiAliasedFill  = true;
        a_style.AntiAliasedLinesUseTex = true;
        a_style.IndentSpacing = 4.0f;

        // Border Sizes
        a_style.WindowBorderSize = 0.3f;
        a_style.ChildBorderSize = 0.0f;
        a_style.FrameBorderSize = 0.1f;
        a_style.ScrollbarSize = 7.0f;

        // Spacing & Padding
        a_style.ItemSpacing.y = 5.0f;
        a_style.FramePadding = { 4.0f, 4.0f };
        a_style.CellPadding = { 4.0f, 4.0f };


    }

    void ImStyleManager::ApplyAccentColor(ImGuiStyle& a_style) {
        auto& colors = a_style.Colors;
        const auto& accent = Config::UI.f3AccentColor;
        const ImVec4 accentColor{ accent[0], accent[1], accent[2], 1.0f };


        // Base color definitions
        constexpr ImVec4 textDisabled{ 0.5f, 0.5f, 0.5f, 1.0f };
        constexpr ImVec4 headerHovered{ 1.0f, 1.0f, 1.0f, 0.1f };
        constexpr ImVec4 tabHovered{ 0.2f, 0.2f, 0.2f, 1.0f };

        // Backgrounds
        
        colors[ImGuiCol_WindowBg] = { 0.0f, 0.0f, 0.0f, 0.85f };
        colors[ImGuiCol_ScrollbarBg] = ImVec4{};
        colors[ImGuiCol_ChildBg] = ImVec4{};
        colors[ImGuiCol_TableHeaderBg] = ImVec4{};
        colors[ImGuiCol_TableRowBg] = ImVec4{};
        colors[ImGuiCol_TableRowBgAlt] = ImVec4{};

        // Borders & Separators
        colors[ImGuiCol_Border] = { 0.50f, 0.50f, 0.50f, 0.7f };
        colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
        colors[ImGuiCol_ResizeGrip] = AdjustAlpha(accentColor, 0.4f);
        colors[ImGuiCol_ResizeGripHovered] = AdjustAlpha(accentColor, 0.6f);
        colors[ImGuiCol_ResizeGripActive] = AdjustAlpha(accentColor, 1.0f);

        // Text
        colors[ImGuiCol_Text] = { 1.0f, 1.0f, 1.0f, 1.0f };
        colors[ImGuiCol_TextDisabled] = textDisabled;

        // Frames & Buttons
        colors[ImGuiCol_FrameBg] = { 0.0f, 0.0f, 0.0f, 0.4f };
        colors[ImGuiCol_FrameBgHovered] = colors[ImGuiCol_FrameBg];
        colors[ImGuiCol_FrameBgActive] = colors[ImGuiCol_FrameBg];
        colors[ImGuiCol_Button] = { 0.0f, 0.0f, 0.0f, 0.4f };
        colors[ImGuiCol_ButtonHovered] = headerHovered;
        colors[ImGuiCol_ButtonActive] = ImVec4{};

        // Tabs
        colors[ImGuiCol_Tab] = ImVec4{};
        colors[ImGuiCol_TabHovered] = tabHovered;
        colors[ImGuiCol_TabSelected] = tabHovered;
        colors[ImGuiCol_TabDimmed] = colors[ImGuiCol_Tab];
        colors[ImGuiCol_TabDimmedSelected] = tabHovered;

        // Sliders
        colors[ImGuiCol_SliderGrab] = AdjustAlpha(accentColor, 0.45f);
        colors[ImGuiCol_SliderGrabActive] = AdjustAlpha(accentColor, 0.9f);

        // Navigation & Highlights
        colors[ImGuiCol_NavCursor] = AdjustAlpha(accentColor, 0.5f);

        // Accent-colored elements
        colors[ImGuiCol_CheckMark] = AdjustAlpha(accentColor, 1.0f);
        colors[ImGuiCol_PlotHistogram] = AdjustAlpha(accentColor, 0.7f);
        colors[ImGuiCol_Header] = AdjustAlpha(accentColor, 0.2f);
        colors[ImGuiCol_HeaderHovered] = AdjustAlpha(accentColor, 0.8f);
        colors[ImGuiCol_HeaderActive] = AdjustAlpha(accentColor, 0.7f);

        // Scrollbars
        colors[ImGuiCol_ScrollbarGrab] = AdjustAlpha(accentColor, 0.4f);
        colors[ImGuiCol_ScrollbarGrabHovered] = AdjustAlpha(accentColor, 0.6f);
        colors[ImGuiCol_ScrollbarGrabActive] = AdjustAlpha(accentColor, 1.0f);

        //Titlebar
        colors[ImGuiCol_TitleBg] = AdjustAlpha(accentColor, 0.8f);
        colors[ImGuiCol_TitleBgActive] = AdjustAlpha(accentColor, 1.0f);
        colors[ImGuiCol_TitleBgCollapsed] = AdjustAlpha(accentColor, 0.6f);
    }

    void ImStyleManager::SetupStyleImpl() {
        ImGuiStyle& currentStyle = ImGui::GetStyle();
        currentStyle = ImGuiStyle(); // Reset to default

        InitializeDefaultStyle(currentStyle);
        ApplyAccentColor(currentStyle);

        // Apply scaling
        currentStyle.ScaleAllSizes(exp2(Config::UI.fScale));
        currentStyle.FontScaleMain = Config::UI.fScale;
        currentStyle.MouseCursorScale = 1.0f;
    }

    ImVec4 ImStyleManager::CalculateContrastColor(const ImVec4& background) {
	    const float luminance = 0.2126f * background.x + 
		    0.7152f * background.y + 
		    0.0722f * background.z;
	    return (luminance > 0.5f) ? ImVec4(0.0f, 0.0f, 0.0f, 1.0f) 
		           : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    ImVec4 ImStyleManager::AdjustAlpha(const ImVec4& color, float alpha) {
	    return {color.x, color.y, color.z, alpha};
    }

    float ImStyleManager::GetScale() {
	    return Config::UI.fScale;
    }

    void ImStyleManager::ApplyStyle() {
	    SetupStyleImpl();
    }
}
