#include "UI/Controls/ProgressBar.hpp"
#include "UI/Lib/imgui_internal.h"


namespace {

    void DrawRoundedGradientRect(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max,
        ImU32 col_left, ImU32 col_right, float rounding) {

        if (rounding <= 0.0f) {
            draw_list->AddRectFilledMultiColor(p_min, p_max, col_left, col_right, col_right, col_left);
            return;
        }

        const float width = p_max.x - p_min.x;
        const float height = p_max.y - p_min.y;

        // Clamp rounding to half the smallest dimension
        rounding = ImMin(rounding, ImMin(width, height) * 0.5f);

        // Number of segments per corner arc
        constexpr int segments_per_corner = 16;

        auto lerpColor = [](ImU32 col1, ImU32 col2, float t) -> ImU32 {
            ImVec4 c1 = ImGui::ColorConvertU32ToFloat4(col1);
            ImVec4 c2 = ImGui::ColorConvertU32ToFloat4(col2);
            ImVec4 result = {
                c1.x + (c2.x - c1.x) * t,
                c1.y + (c2.y - c1.y) * t,
                c1.z + (c2.z - c1.z) * t,
                c1.w + (c2.w - c1.w) * t
            };
            return ImGui::ColorConvertFloat4ToU32(result);
        };

        const ImVec2 uv = draw_list->_Data->TexUvWhitePixel;
        const float AA_SIZE = draw_list->_FringeScale;

        // Build a list of all perimeter points
        ImVector<ImVec2> points;
        ImVector<ImVec2> normals;
        points.reserve(segments_per_corner * 4 + 4);
        normals.reserve(segments_per_corner * 4 + 4);

        auto addPoint = [&](const ImVec2& p, const ImVec2& n) {
            points.push_back(p);
            normals.push_back(n);
        };

        // Top edge, left to right
        addPoint({ p_min.x + rounding, p_min.y }, { 0.0f, -1.0f });

        // Top-right corner arc
        for (int i = 1; i <= segments_per_corner; ++i) {
            float t = static_cast<float>(i) / (segments_per_corner + 1);
            float angle = -std::numbers::pi * 0.5f + t * std::numbers::pi * 0.5f;
            ImVec2 pos = {
                p_max.x - rounding + ImCos(angle) * rounding,
                p_min.y + rounding + ImSin(angle) * rounding
            };
            addPoint(pos, { ImCos(angle), ImSin(angle) });
        }

        // Right edge, top to bottom
        addPoint({ p_max.x, p_min.y + rounding }, { 1.0f, 0.0f });
        addPoint({ p_max.x, p_max.y - rounding }, { 1.0f, 0.0f });

        // Bottom-right corner arc
        for (int i = 1; i <= segments_per_corner; ++i) {
            float t = static_cast<float>(i) / (segments_per_corner + 1);
            float angle = 0.0f + t * std::numbers::pi * 0.5f;
            ImVec2 pos = {
                p_max.x - rounding + ImCos(angle) * rounding,
                p_max.y - rounding + ImSin(angle) * rounding
            };
            addPoint(pos, { ImCos(angle), ImSin(angle) });
        }

        // Bottom edge, right to left
        addPoint({ p_max.x - rounding, p_max.y }, { 0.0f, 1.0f });
        addPoint({ p_min.x + rounding, p_max.y }, { 0.0f, 1.0f });

        // Bottom-left corner arc
        for (int i = 1; i <= segments_per_corner; ++i) {
            float t = static_cast<float>(i) / (segments_per_corner + 1);
            float angle = std::numbers::pi * 0.5f + t * std::numbers::pi * 0.5f;
            ImVec2 pos = {
                p_min.x + rounding + ImCos(angle) * rounding,
                p_max.y - rounding + ImSin(angle) * rounding
            };
            addPoint(pos, { ImCos(angle), ImSin(angle) });
        }

        // Left edge, bottom to top
        addPoint({ p_min.x, p_max.y - rounding }, { -1.0f, 0.0f });
        addPoint({ p_min.x, p_min.y + rounding }, { -1.0f, 0.0f });

        // Top-left corner arc
        for (int i = 1; i <= segments_per_corner; ++i) {
            float t = static_cast<float>(i) / (segments_per_corner + 1);
            float angle = std::numbers::pi + t * std::numbers::pi * 0.5f;
            ImVec2 pos = {
                p_min.x + rounding + ImCos(angle) * rounding,
                p_min.y + rounding + ImSin(angle) * rounding
            };
            addPoint(pos, { ImCos(angle), ImSin(angle) });
        }

        const int num_points = points.Size;
        const ImDrawIdx vtx_base = static_cast<ImDrawIdx>(draw_list->_VtxCurrentIdx);

        // We need: center + inner points + outer AA points
        const int vtx_count = 1 + num_points * 2;
        const int idx_count = num_points * 3 + num_points * 6; // inner triangles + AA fringe
        draw_list->PrimReserve(idx_count, vtx_count);

        // Center vertex
        ImVec2 center = { (p_min.x + p_max.x) * 0.5f, (p_min.y + p_max.y) * 0.5f };
        float center_t = 0.5f;
        ImU32 center_color = lerpColor(col_left, col_right, center_t);
        draw_list->PrimWriteVtx(center, uv, center_color);

        // Inner perimeter vertices
        for (int i = 0; i < num_points; ++i) {
            float t = ImClamp((points[i].x - p_min.x) / width, 0.0f, 1.0f);
            ImU32 color = lerpColor(col_left, col_right, t);
            draw_list->PrimWriteVtx(points[i], uv, color);
        }

        // Outer AA fringe vertices
        for (int i = 0; i < num_points; ++i) {
            float t = ImClamp((points[i].x - p_min.x) / width, 0.0f, 1.0f);
            ImVec4 c = ImGui::ColorConvertU32ToFloat4(lerpColor(col_left, col_right, t));
            c.w *= 0.5f;
            ImU32 color_outer = ImGui::ColorConvertFloat4ToU32(c);
            ImVec2 outer_pos{
                points[i].x + normals[i].x * AA_SIZE,
                points[i].y + normals[i].y * AA_SIZE
            };
            draw_list->PrimWriteVtx(outer_pos, uv, color_outer);
        }

        // Inner triangle fan indices
        for (int i = 0; i < num_points; ++i) {
            draw_list->PrimWriteIdx(vtx_base);
            draw_list->PrimWriteIdx(vtx_base + 1 + i);
            draw_list->PrimWriteIdx(vtx_base + 1 + ((i + 1) % num_points));
        }

        // AA fringe indices
        const ImDrawIdx inner_base = vtx_base + 1;
        const ImDrawIdx outer_base = vtx_base + 1 + num_points;
        for (int i = 0; i < num_points; ++i) {
            int next_i = (i + 1) % num_points;
            draw_list->PrimWriteIdx(inner_base + i);
            draw_list->PrimWriteIdx(outer_base + i);
            draw_list->PrimWriteIdx(outer_base + next_i);

            draw_list->PrimWriteIdx(inner_base + i);
            draw_list->PrimWriteIdx(outer_base + next_i);
            draw_list->PrimWriteIdx(inner_base + next_i);
        }
    }
}

namespace ImGuiEx {

    void ProgressBar(float a_fraction, const ImVec2& a_size, const char* a_ovlText,
        ImU32 a_flags, float a_heightMult, float a_borderThickness, float a_roundingScale,
        float a_gradDarkFactor, float a_gradLightFactor,
        ImU32 a_gradCol1, ImU32 a_gradCol2, ImU32 a_borderCol) {

        const ImGuiWindow* window = ImGui::GetCurrentWindow();

        if (window->SkipItems) {
            return;
        }
        const ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        // Calculate progress bar dimensions
        ImVec2 pos = window->DC.CursorPos;
        const auto TextSize = ImGui::CalcTextSize(a_ovlText);
        const auto ItemWidth = ImGui::CalcItemWidth();
        const float Width = ItemWidth > TextSize.x ? ItemWidth : TextSize.x;
        const ImVec2 ResultSize = { Width, TextSize.y };
        const ImVec2 size = ImGui::CalcItemSize(a_size, ResultSize.x, ResultSize.y + style.FramePadding.y * 2.0f * a_heightMult);
        const ImVec2 possize = { pos.x + size.x, pos.y + size.y };
        const ImRect bb(pos, possize);

        // Register the item and handle clipping
        ImGui::ItemSize(size, style.FramePadding.y);
        if (!ImGui::ItemAdd(bb, 0)) {
            return;
        }

        // Get the colors
        const ImU32 bg_color = ImGui::GetColorU32(ImGuiCol_FrameBg);
        ImU32 fill_color = a_gradCol1;

        float frame_rounding = a_flags & ImGuiExProgresbarFlag_Rounding ? a_roundingScale : 0.0f;

        // Create inner rectangle accounting for border thickness
        ImRect inner_bb(
            { bb.Min.x + a_borderThickness, bb.Min.y + a_borderThickness },
            { bb.Max.x - a_borderThickness, bb.Max.y - a_borderThickness }
        );

        // Render border
        if (a_borderThickness > 0.0f) {
            ImVec4 borderColorVec4 = ImGui::ColorConvertU32ToFloat4(a_borderCol);
            borderColorVec4.w *= style.Alpha;
            ImU32 borderColorWithAlpha = ImGui::ColorConvertFloat4ToU32(borderColorVec4);
            window->DrawList->AddRect(bb.Min, bb.Max, borderColorWithAlpha, frame_rounding, ImDrawFlags_None, a_borderThickness);
        }

        auto* dl = window->DrawList;

        // Render background (inner rectangle) with rounding
        if (frame_rounding > 0.0f) {
            dl->PathRect(inner_bb.Min, inner_bb.Max, frame_rounding);
            dl->PathFillConvex(bg_color);
        }
        else {
            dl->AddRectFilled(inner_bb.Min, inner_bb.Max, bg_color);
        }

        // Render filled portion with adjusted size for border
        if (a_fraction > 0.0f) {
            const float fill_width = ImMax((inner_bb.Max.x - inner_bb.Min.x) * a_fraction, 2.0f);
            ImRect fill_bb(inner_bb.Min, { inner_bb.Min.x + fill_width, inner_bb.Max.y });

            if (a_flags & ImGuiExProgresbarFlag_Gradient) {
                ImU32 startColor, endColor;

                if (a_flags & ImGuiExProgresbarFlag_DualColor) {
                    ImVec4 startColorVec4 = ImGui::ColorConvertU32ToFloat4(a_gradCol1);
                    ImVec4 endColorVec4 = ImGui::ColorConvertU32ToFloat4(a_gradCol2);

                    startColorVec4.w *= style.Alpha;
                    endColorVec4.w *= style.Alpha;

                    startColor = ImGui::ColorConvertFloat4ToU32(startColorVec4);
                    endColor = ImGui::ColorConvertFloat4ToU32(endColorVec4);
                }
                else {
                    ImVec4 baseColor = ImGui::ColorConvertU32ToFloat4(fill_color);
                    baseColor.w *= style.Alpha;

                    ImVec4 darkColor = {
                        ImClamp(baseColor.x * a_gradDarkFactor, 0.0f, 1.0f),
                        ImClamp(baseColor.y * a_gradDarkFactor, 0.0f, 1.0f),
                        ImClamp(baseColor.z * a_gradDarkFactor, 0.0f, 1.0f),
                        baseColor.w
                    };

                    ImVec4 lightColor = {
                        ImClamp(baseColor.x * a_gradLightFactor, 0.0f, 1.0f),
                        ImClamp(baseColor.y * a_gradLightFactor, 0.0f, 1.0f),
                        ImClamp(baseColor.z * a_gradLightFactor, 0.0f, 1.0f),
                        baseColor.w
                    };

                    startColor = ImGui::ColorConvertFloat4ToU32(darkColor);
                    endColor = ImGui::ColorConvertFloat4ToU32(lightColor);
                }

                if (a_flags & ImGuiExProgresbarFlag_SwapGradientDir) {
                    const ImU32 temp = startColor;
                    startColor = endColor;
                    endColor = temp;
                }

                DrawRoundedGradientRect(
                    dl,
                    fill_bb.Min,
                    fill_bb.Max,
                    startColor,
                    endColor,
                    frame_rounding
                );
            }
            else {
                // Apply alpha to fill color
                ImVec4 fillColorVec4 = ImGui::ColorConvertU32ToFloat4(fill_color);
                fillColorVec4.w *= style.Alpha;
                ImU32 fillColorWithAlpha = ImGui::ColorConvertFloat4ToU32(fillColorVec4);

                if (frame_rounding > 0.0f) {
                    dl->AddRectFilled(fill_bb.Min, fill_bb.Max, fillColorWithAlpha, frame_rounding);
                }
                else {
                    dl->AddRectFilled(fill_bb.Min, fill_bb.Max, fillColorWithAlpha);
                }
            }
        }

        // Render centered text
        if (a_ovlText != nullptr) {
            ImVec2 overlay_size = ImGui::CalcTextSize(a_ovlText);
            ImVec2 text_pos = {
                bb.Min.x + (size.x - overlay_size.x) * 0.5f,
                bb.Min.y + (size.y - overlay_size.y) * 0.5f
            };

            window->DrawList->AddText(ImVec2(text_pos.x + 1, text_pos.y + 1), IM_COL32(0, 0, 0, 128 * style.Alpha), a_ovlText);
            window->DrawList->AddText(text_pos, ImGui::GetColorU32(ImGuiCol_Text), a_ovlText);
        }
    }
}
