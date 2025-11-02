#pragma once
#include "UI/Lib/imgui.h"

namespace ImGuiEx {

    enum imGuiEx_ProgressBarFlags : uint32_t {
        ImGuiExProgresbarFlag_None            = 0,
        ImGuiExProgresbarFlag_Gradient        = 1 << 0,
        ImGuiExProgresbarFlag_Rounding        = 1 << 1,
        ImGuiExProgresbarFlag_DualColor       = 1 << 2,
        ImGuiExProgresbarFlag_SwapGradientDir = 1 << 3
    };

    void ProgressBar(
        float a_fraction, 
        const ImVec2& a_size, 
        const char* a_ovlText,
        ImU32 a_flags = ImGuiExProgresbarFlag_None,
        float a_heightMult = 1.0f,
        float a_borderThickness = 1.0f,
        float a_roundingScale = 5.0f,
        float a_gradDarkFactor = 0.7f,
        float a_gradLightFactor = 1.3f,
        ImU32 a_gradCol1 = IM_COL32(255, 0, 0, 255),
        ImU32 a_gradCol2 = IM_COL32(0, 255, 0, 255),
		ImU32 a_borderCol = IM_COL32(128, 128, 128, 192)
        );

}
