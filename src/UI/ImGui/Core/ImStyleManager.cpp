#include "UI/ImGui/Core/ImStyleManager.hpp"

#include "ImColorUtils.hpp"

#include "UI/ImGui/Lib/imgui.h"
#include "Config/Config.hpp"


namespace GTS {

    void ImStyleManager::InitializeDefaultStyle(ImGuiStyle& a_style) {
        // Rounding Settings
        a_style.WindowRounding =          2.5f;
        a_style.ChildRounding =           1.5f;
        a_style.FrameRounding =           1.5f;
        a_style.ScrollbarRounding =       1.5f;
        a_style.GrabRounding =            1.5f;
        a_style.TabRounding =             1.5f;

        a_style.GrabMinSize =             6.0f;

        // Anti-Aliasing
        a_style.AntiAliasedLines =        true;
        a_style.AntiAliasedFill  =        true;
        a_style.AntiAliasedLinesUseTex =  true;
        a_style.IndentSpacing =           4.0f;

        // Border Sizes
        a_style.WindowBorderSize =        0.3f;
        a_style.ChildBorderSize =         0.0f;
        a_style.FrameBorderSize =         0.1f;
        a_style.ScrollbarSize =           7.0f;

        // Spacing & Padding
        a_style.ItemSpacing.y =           5.0f;
        a_style.FramePadding =            { 4.0f, 4.0f };
        a_style.CellPadding =             { 4.0f, 4.0f };


    }

    void ImStyleManager::ApplyAccentColor(ImGuiStyle& a_style) {
        auto& colors = a_style.Colors;      
        const auto& accent = Config::UI.f3AccentColor;
        const ImVec4 accentColor{ accent[0], accent[1], accent[2], 1.0f };

        const ImVec4 iElemBase =                ImUtil::Colors::RGBAToImVec4(0, 0, 0, 132);
        const ImVec4 iElemHover =               ImUtil::Colors::RGBAToImVec4(48, 48, 48, 132);
        const ImVec4 iElemSelect =              ImUtil::Colors::RGBAToImVec4(24, 24, 24, 132);

        ImVec4 iCheckbox =                      ImUtil::Colors::CompensateForDarkColors(accentColor, 0.5f);
        ImVec4 iSliderGrab =                    ImUtil::Colors::CompensateForDarkColors(accentColor, 0.5f);

        // Backgrounds
        colors[ImGuiCol_WindowBg] =             { 0.0f, 0.0f, 0.0f, 0.85f };
        colors[ImGuiCol_ScrollbarBg] =          {};
        colors[ImGuiCol_ChildBg] =              {};
        colors[ImGuiCol_TableHeaderBg] =        {};
        colors[ImGuiCol_TableRowBg] =           {};
        colors[ImGuiCol_TableRowBgAlt] =        {};

        // Borders & Separators
        colors[ImGuiCol_Border] =               { 0.50f, 0.50f, 0.50f, 0.7f };
        colors[ImGuiCol_Separator] =            { 0.50f, 0.50f, 0.50f, 0.7f };
        colors[ImGuiCol_ResizeGrip] =           ImUtil::Colors::AdjustAlpha(accentColor, 1.0f);
        colors[ImGuiCol_ResizeGripHovered] =    ImUtil::Colors::AdjustLightness(colors[ImGuiCol_ResizeGrip], 1.2f);
        colors[ImGuiCol_ResizeGripActive] =     ImUtil::Colors::AdjustLightness(colors[ImGuiCol_ResizeGrip], 1.5f);

        // Text
        colors[ImGuiCol_Text] =                 { 1.0f, 1.0f, 1.0f, 1.0f };
        colors[ImGuiCol_TextDisabled] =         { 0.4f, 0.4f, 0.4f, 1.0f };

        // Frames & Buttons
        colors[ImGuiCol_FrameBg] =              iElemBase;
        colors[ImGuiCol_FrameBgHovered] =       iElemHover;
        colors[ImGuiCol_FrameBgActive] =        iElemSelect;
        colors[ImGuiCol_Button] =               iElemBase;
        colors[ImGuiCol_ButtonHovered] =        iElemHover;
        colors[ImGuiCol_ButtonActive] =         ImUtil::Colors::AdjustAlpha(accentColor, 0.55f);;

        // Tabs
        colors[ImGuiCol_Tab] =                  iElemBase;
        colors[ImGuiCol_TabHovered] =           iElemHover;
        colors[ImGuiCol_TabSelected] =          ImUtil::Colors::AdjustAlpha(iSliderGrab, 0.45f);
        colors[ImGuiCol_TabDimmed] =            iElemBase;
        colors[ImGuiCol_TabDimmedSelected] =    iElemBase;

        // Sliders
        colors[ImGuiCol_SliderGrab] =           ImUtil::Colors::AdjustAlpha(iSliderGrab, 0.45f);
        colors[ImGuiCol_SliderGrabActive] =     ImUtil::Colors::AdjustAlpha(iSliderGrab, 0.9f);

        // Navigation & Highlights
        colors[ImGuiCol_NavCursor] =            ImUtil::Colors::AdjustAlpha(accentColor, 0.5f);

        // Accent-colored elements
        colors[ImGuiCol_CheckMark] =            iCheckbox;
        colors[ImGuiCol_PlotHistogram] =        ImUtil::Colors::AdjustAlpha(accentColor, 0.7f);
        colors[ImGuiCol_Header] =               ImUtil::Colors::AdjustLightness(ImUtil::Colors::AdjustAlpha(accentColor, 0.4f), 0.8f);
        colors[ImGuiCol_HeaderHovered] =        ImUtil::Colors::AdjustLightness(colors[ImGuiCol_Header], 1.0f);
        colors[ImGuiCol_HeaderActive] =         ImUtil::Colors::AdjustLightness(colors[ImGuiCol_Header], 1.2f);

        // Scrollbars
        colors[ImGuiCol_ScrollbarGrab] =         ImUtil::Colors::AdjustAlpha(ImUtil::Colors::AdjustLightness(accentColor, 0.8f), 0.5f);
        colors[ImGuiCol_ScrollbarGrabHovered] =  ImUtil::Colors::AdjustAlpha(ImUtil::Colors::AdjustLightness(accentColor, 1.0f), 0.5f);
        colors[ImGuiCol_ScrollbarGrabActive] =   ImUtil::Colors::AdjustAlpha(ImUtil::Colors::AdjustLightness(accentColor, 0.8f), 0.5f);

        //Titlebar
        colors[ImGuiCol_TitleBg] =               ImUtil::Colors::AdjustLightness(accentColor, 0.3f);
        colors[ImGuiCol_TitleBgActive] =         ImUtil::Colors::AdjustLightness(accentColor, 0.6f);
        colors[ImGuiCol_TitleBgCollapsed] =      ImUtil::Colors::AdjustLightness(accentColor, 0.3f);
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

    float ImStyleManager::GetScale() {
	    return Config::UI.fScale;
    }

    void ImStyleManager::ApplyStyle() {
	    SetupStyleImpl();
    }
}
