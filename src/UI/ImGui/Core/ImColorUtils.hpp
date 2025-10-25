#pragma once
#include "UI/ImGui/Lib/imgui.h"

namespace ImUtil::Colors {

    ImVec4 AdjustAlpha(const ImVec4& color, float alpha);
    ImVec4 AdjustLightness(const ImVec4& color, float factor);
    ImVec4 CalculateContrastColor(const ImVec4& background);
    ImVec4 RGBAToImVec4(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept;
    ImU32 fRGBToU32(std::array<float, 3> a_rgb);
    ImVec4 fRGBToImVec4(std::array<float, 3> a_rgb);

    std::array<float, 3> RGBToLinear(float r, float g, float b);
    std::array<float, 3> LinearToRGB(float r, float g, float b);
    std::array<float, 3> LinearToOKLab(float r, float g, float b);
    std::array<float, 3> OKLabToLinear(float L, float a, float b);
    ImVec4 CompensateForDarkColors(const ImVec4& c, float minL);
}
