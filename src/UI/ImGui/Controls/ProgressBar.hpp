#pragma once
#include "UI/ImGui/Lib/imgui.h"

namespace ImGuiEx {

    void ProgressBar(float fraction, const ImVec2& size_arg, const char* overlay, const float heightmult,
        float borderThickness = 1.0f, bool useGradient = false,
        float gradientDarkFactor = 0.7f, float gradientLightFactor = 1.3f,
        bool useRounding = true,
        bool useCustomGradientColors = false,
        ImU32 gradientStartColor = IM_COL32(255, 0, 0, 255),
        ImU32 gradientEndColor = IM_COL32(0, 255, 0, 255),
        bool flipGradientDirection = false);

}
