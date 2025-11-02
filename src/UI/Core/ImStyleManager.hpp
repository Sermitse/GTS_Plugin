#pragma once

#include "UI/Lib/imgui.h"

namespace GTS {

    class ImStyleManager {

        private:
        static void InitializeDefaultStyle(ImGuiStyle& a_style);
        static void ApplyAccentColor(ImGuiStyle& a_style);
        static void SetupStyleImpl();

    public:
        [[nodiscard]] static float GetScale();
        static void ApplyStyle();
    };
}