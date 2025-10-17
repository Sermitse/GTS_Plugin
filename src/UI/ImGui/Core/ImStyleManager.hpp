#pragma once

#include "UI/ImGui/Lib/imgui.h"

namespace GTS {

    class ImStyleManager {

        private:
        static void InitializeDefaultStyle(ImGuiStyle& a_style);
        static void ApplyAccentColor(ImGuiStyle& a_style);
        static void SetupStyleImpl();
        static ImVec4 CalculateContrastColor(const ImVec4& background);
        static ImVec4 AdjustAlpha(const ImVec4& color, float alpha);
        static ImVec4 AdjustLightness(const ImVec4& color, float factor);

    public:
        [[nodiscard]] static float GetScale();
        static void ApplyStyle();
    };
}