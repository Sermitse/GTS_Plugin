#include "UI/ImGui/Core/ImStyleManager.hpp"
#include "UI/ImGui/Lib/imgui.h"
#include "Config/Config.hpp"



namespace {

    constexpr ImVec4 ToImVec4(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept {
        constexpr float scale = 1.0f / 255.0f;
        return ImVec4(r * scale, g * scale, b * scale, a * scale);
    }

    auto EnforceMinValueHSV = [](ImVec4 c, float minV) -> ImVec4 {
        float r = c.x, g = c.y, b = c.z;

        float maxC = std::max({ r, g, b });
        float minC = std::min({ r, g, b });
        float delta = maxC - minC;

        float h = 0.0f, s = 0.0f, v = maxC;

        if (delta > 1e-6f) {
            s = delta / maxC;
            if (maxC == r) h = (g - b) / delta;
            else if (maxC == g) h = 2.0f + (b - r) / delta;
            else h = 4.0f + (r - g) / delta;
            h = std::fmod((h * 60.0f + 360.0f), 360.0f);
        }

        v = std::max(v, minV);

        // HSV to RGB
        if (s < 1e-6f) return { v, v, v, c.w };

        float hh = h / 60.0f;
        int i = static_cast<int>(hh);
        float f = hh - i;
        float p = v * (1.0f - s);
        float q = v * (1.0f - s * f);
        float t = v * (1.0f - s * (1.0f - f));

        switch (i % 6) {
            case 0: return { v, t, p, c.w };
            case 1: return { q, v, p, c.w };
            case 2: return { p, v, t, c.w };
            case 3: return { p, q, v, c.w };
            case 4: return { t, p, v, c.w };
            case 5: return { v, p, q, c.w };
			default: break;
        }
        return { v, v, v, c.w };
    };


}

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

        constexpr ImVec4 iElemBase = ToImVec4(0, 0, 0, 132);
        constexpr ImVec4 iElemHover = ToImVec4(48, 48, 48, 132);
        //constexpr ImVec4 iElemSelect = ToImVec4(24, 24, 24, 132);

        ImVec4 iCheckbox = EnforceMinValueHSV(accentColor, 0.2f);
        ImVec4 iSliderGrab = EnforceMinValueHSV(accentColor, 0.3f);

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
        colors[ImGuiCol_ResizeGrip] = AdjustAlpha(accentColor, 1.0f);
        colors[ImGuiCol_ResizeGripHovered] = AdjustLightness(colors[ImGuiCol_ResizeGrip], 1.2);
        colors[ImGuiCol_ResizeGripActive] = AdjustLightness(colors[ImGuiCol_ResizeGrip], 1.5);

        // Text
        colors[ImGuiCol_Text] = { 1.0f, 1.0f, 1.0f, 1.0f };
        colors[ImGuiCol_TextDisabled] = textDisabled;

        // Frames & Buttons
        colors[ImGuiCol_FrameBg] = iElemBase;
        colors[ImGuiCol_FrameBgHovered] = iElemHover;
        colors[ImGuiCol_FrameBgActive] = AdjustAlpha(iSliderGrab, 0.45f);
        colors[ImGuiCol_Button] = iElemBase;
        colors[ImGuiCol_ButtonHovered] = iElemHover;
        colors[ImGuiCol_ButtonActive] = AdjustAlpha(iSliderGrab, 0.45f);;

        // Tabs
        colors[ImGuiCol_Tab] = iElemBase;
        colors[ImGuiCol_TabHovered] = iElemHover;
        colors[ImGuiCol_TabSelected] = AdjustAlpha(iSliderGrab, 0.45f);
        colors[ImGuiCol_TabDimmed] = colors[ImGuiCol_Tab];
        colors[ImGuiCol_TabDimmedSelected] = colors[ImGuiCol_Tab];

        // Sliders
        colors[ImGuiCol_SliderGrab] = AdjustAlpha(iSliderGrab, 0.45f);
        colors[ImGuiCol_SliderGrabActive] = AdjustAlpha(iSliderGrab, 0.9f);

        // Navigation & Highlights
        colors[ImGuiCol_NavCursor] = AdjustAlpha(accentColor, 0.5f);

        // Accent-colored elements
        colors[ImGuiCol_CheckMark] = iCheckbox;
        colors[ImGuiCol_PlotHistogram] = AdjustAlpha(accentColor, 0.7f);
        colors[ImGuiCol_Header] = AdjustAlpha(accentColor, 0.2f);
        colors[ImGuiCol_HeaderHovered] = AdjustLightness(colors[ImGuiCol_Header], 1.2f);
        colors[ImGuiCol_HeaderActive] = AdjustLightness(colors[ImGuiCol_Header], 1.1f);

        // Scrollbars
        colors[ImGuiCol_ScrollbarGrab] = AdjustAlpha(AdjustLightness(accentColor, 0.8f), 0.5f);
        colors[ImGuiCol_ScrollbarGrabHovered] = AdjustAlpha(AdjustLightness(accentColor, 1.0f), 0.5f);
        colors[ImGuiCol_ScrollbarGrabActive] = AdjustAlpha(AdjustLightness(accentColor, 0.8f), 0.5f);

        //Titlebar
        colors[ImGuiCol_TitleBg] = AdjustLightness(accentColor, 0.3f);
        colors[ImGuiCol_TitleBgActive] = AdjustLightness(accentColor, 0.6f);
        colors[ImGuiCol_TitleBgCollapsed] = AdjustLightness(accentColor, 0.3f);
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

    ImVec4 ImStyleManager::AdjustLightness(const ImVec4& color, float factor) {
        // Convert RGB to HSL
        float r = color.x, g = color.y, b = color.z;
        float maxC = std::max({ r, g, b });
        float minC = std::min({ r, g, b });
        float l = (maxC + minC) / 2.0f;

        if (maxC == minC) {
            // Achromatic (gray)
            float newL = std::clamp(l * factor, 0.0f, 1.0f);
            return { newL, newL, newL, color.w };  // Preserve original alpha
        }

        float delta = maxC - minC;
        float s = (l > 0.5f) ? delta / (2.0f - maxC - minC) : delta / (maxC + minC);

        float h;
        if (maxC == r) {
            h = (g - b) / delta + (g < b ? 6.0f : 0.0f);
        }
        else if (maxC == g) {
            h = (b - r) / delta + 2.0f;
        }
        else {
            h = (r - g) / delta + 4.0f;
        }
        h /= 6.0f;

        // Adjust lightness
        l = std::clamp(l * factor, 0.0f, 1.0f);

        // Convert HSL back to RGB
        auto hue2rgb = [](float p, float q, float t) {
            if (t < 0.0f) t += 1.0f;
            if (t > 1.0f) t -= 1.0f;
            if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
            if (t < 1.0f / 2.0f) return q;
            if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
            return p;
            };

        float q = (l < 0.5f) ? l * (1.0f + s) : l + s - l * s;
        float p = 2.0f * l - q;

        return {
            hue2rgb(p, q, h + 1.0f / 3.0f),
            hue2rgb(p, q, h),
            hue2rgb(p, q, h - 1.0f / 3.0f),
            color.w
        };
    }



    float ImStyleManager::GetScale() {
	    return Config::UI.fScale;
    }

    void ImStyleManager::ApplyStyle() {
	    SetupStyleImpl();
    }
}
