#include "UI/ImGui/Controls/ProgressBar.hpp"
#include "UI/ImGui/Lib/imgui_internal.h"

namespace ImGuiEx {

    void ProgressBar(float fraction, const ImVec2& size_arg, const char* overlay, const float heightmult,
        float borderThickness, bool useGradient,
        float gradientDarkFactor, float gradientLightFactor,
        bool useRounding,
        bool useCustomGradientColors,
        ImU32 gradientStartColor,
        ImU32 gradientEndColor,
        bool flipGradientDirection) {

        const ImGuiWindow* window = ImGui::GetCurrentWindow();

        if (window->SkipItems) {
            return;
        }
        const ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        // Calculate progress bar dimensions
        ImVec2 pos = window->DC.CursorPos;
        const auto TextSize = ImGui::CalcTextSize(overlay);
        const auto ItemWidth = ImGui::CalcItemWidth();
        const float Width = ItemWidth > TextSize.x ? ItemWidth : TextSize.x;
        const ImVec2 ResultSize = { Width, TextSize.y };
        const ImVec2 size = ImGui::CalcItemSize(size_arg, ResultSize.x, ResultSize.y + style.FramePadding.y * 2.0f * heightmult);
        const ImVec2 possize = { pos.x + size.x, pos.y + size.y };
        const ImRect bb(pos, possize);

        // Register the item and handle clipping
        ImGui::ItemSize(size, style.FramePadding.y);
        if (!ImGui::ItemAdd(bb, 0)) {
            return;
        }

        // Get the colors
        const ImU32 border_color = ImGui::GetColorU32(ImGuiCol_Border);
        const ImU32 bg_color = ImGui::GetColorU32(ImGuiCol_FrameBg);
        ImU32 fill_color = gradientStartColor;

        float frame_rounding = useRounding ? style.FrameRounding : 0.0f;

        // Create inner rectangle accounting for border thickness
        ImRect inner_bb(
            ImVec2(bb.Min.x + borderThickness, bb.Min.y + borderThickness),
            ImVec2(bb.Max.x - borderThickness, bb.Max.y - borderThickness)
        );

        // Render border
        if (borderThickness > 0.0f) {
            window->DrawList->AddRect(bb.Min, bb.Max, border_color, frame_rounding, ImDrawFlags_None, borderThickness);
        }

        // Render background (inner rectangle)
        window->DrawList->AddRectFilled(inner_bb.Min, inner_bb.Max, bg_color, frame_rounding);

        // Render filled portion with adjusted size for border
        if (fraction > 0.0f) {
            const float fill_width = ImMax((inner_bb.Max.x - inner_bb.Min.x) * fraction, 2.0f);
            ImRect fill_bb(
                inner_bb.Min,
                ImVec2(inner_bb.Min.x + fill_width, inner_bb.Max.y)
            );

            if (useGradient) {
                ImU32 startColor, endColor;

                if (useCustomGradientColors) {
                    // Apply global alpha to custom colors
                    ImVec4 startColorVec4 = ImGui::ColorConvertU32ToFloat4(gradientStartColor);
                    ImVec4 endColorVec4 = ImGui::ColorConvertU32ToFloat4(gradientEndColor);

                    // Modify the alpha component while preserving the original RGB
                    startColorVec4.w *= style.Alpha;
                    endColorVec4.w *= style.Alpha;

                    // Convert back to ImU32
                    startColor = ImGui::ColorConvertFloat4ToU32(startColorVec4);
                    endColor = ImGui::ColorConvertFloat4ToU32(endColorVec4);
                }
                else {
                    ImVec4 baseColor = ImGui::ColorConvertU32ToFloat4(fill_color);
                    baseColor.w *= style.Alpha;

                    ImVec4 darkColor = ImVec4(
                        ImClamp(baseColor.x * gradientDarkFactor, 0.0f, 1.0f),
                        ImClamp(baseColor.y * gradientDarkFactor, 0.0f, 1.0f),
                        ImClamp(baseColor.z * gradientDarkFactor, 0.0f, 1.0f),
                        baseColor.w
                    );

                    ImVec4 lightColor = ImVec4(
                        ImClamp(baseColor.x * gradientLightFactor, 0.0f, 1.0f),
                        ImClamp(baseColor.y * gradientLightFactor, 0.0f, 1.0f),
                        ImClamp(baseColor.z * gradientLightFactor, 0.0f, 1.0f),
                        baseColor.w
                    );

                    startColor = ImGui::ColorConvertFloat4ToU32(darkColor);
                    endColor = ImGui::ColorConvertFloat4ToU32(lightColor);
                }

                if (flipGradientDirection) {
                    const ImU32 temp = startColor;
                    startColor = endColor;
                    endColor = temp;
                }

                // Draw gradient fill
                window->DrawList->AddRectFilledMultiColor(
                    fill_bb.Min,
                    fill_bb.Max,
                    startColor,     // Left color
                    endColor,       // Right color
                    endColor,       // Bottom right color
                    startColor      // Bottom left color
                );
            }
            else {
                // Draw regular solid fill
                window->DrawList->AddRectFilled(fill_bb.Min, fill_bb.Max, fill_color, frame_rounding);
            }
        }

        // Render centered text
        if (overlay != nullptr) {
            ImVec2 overlay_size = ImGui::CalcTextSize(overlay);
            ImVec2 text_pos = ImVec2(
                bb.Min.x + (size.x - overlay_size.x) * 0.5f,
                bb.Min.y + (size.y - overlay_size.y) * 0.5f
            );

            // Draw text with contrasting shadow
            window->DrawList->AddText(
                ImVec2(text_pos.x + 1, text_pos.y + 1),
                IM_COL32(0, 0, 0, 128 * style.Alpha),
                overlay
            );
            window->DrawList->AddText(text_pos, ImGui::GetColorU32(ImGuiCol_Text), overlay);
        }
    }
}
