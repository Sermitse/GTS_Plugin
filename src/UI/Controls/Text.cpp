#include "UI/Controls/Text.hpp"
#include "UI/Controls/ToolTip.hpp"
#include "UI/Core/ImUtil.hpp"

namespace ImGuiEx {

    inline ImU32 ApplyAlpha(ImU32 color) {
        float alpha = ImGui::GetStyle().Alpha;
        ImVec4 col = ImGui::ColorConvertU32ToFloat4(color);
        col.w *= alpha;
        return ImGui::ColorConvertFloat4ToU32(col);
    }

    // Helper to display a little (?) mark which shows a tooltip when hovered.
    void HelpText(const char* a_title, const char* a_tooltip) {
        ImGui::TextColored(ImUtil::Colors::Subscript,"%s (?)", a_title);
        Tooltip(a_tooltip, true);
    }

    void TextShadow(const char* a_cfmt, ...) {
        va_list args;
        va_start(args, a_cfmt);
        const char* text, * text_end;
        ImFormatStringToTempBufferV(&text, &text_end, a_cfmt, args);
        va_end(args);
        TextShadowImpl(text, text_end);

    }

    void TextShadowEx(ImVec2 a_pos, const char* a_cfmt, ...) {
        va_list args;
        va_start(args, a_cfmt);
        const char* text, * text_end;
        ImFormatStringToTempBufferV(&text, &text_end, a_cfmt, args);
        va_end(args);
        TextShadowImplEx(a_pos, text, text_end);
    }

    void TextShadowExNoDummy(ImVec2 a_pos, const char* a_cfmt, ...) {
        va_list args;
        va_start(args, a_cfmt);
        const char* text, * text_end;
        ImFormatStringToTempBufferV(&text, &text_end, a_cfmt, args);
        va_end(args);
        TextShadowImplExDummy(a_pos, text, text_end);
    }

    void TextColorShadow(ImVec4 a_col, const char* a_cfmt, ...) {
        va_list args;
        va_start(args, a_cfmt);
        const char* text, * text_end;
        ImFormatStringToTempBufferV(&text, &text_end, a_cfmt, args);
        va_end(args);
        TextShadowImpl(text, text_end, ImGui::ColorConvertFloat4ToU32(a_col));
    }

    void TextShadowImplNoDummy(const char* a_text, const char* a_textEnd, ImU32 a_textColor, ImU32 a_shadowColor, float a_shadowOffset) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetCursorScreenPos();

        // Apply current alpha to colors
        ImU32 shadowCol = ApplyAlpha(a_shadowColor);
        ImU32 textCol = ApplyAlpha(a_textColor);

        draw_list->AddText(ImVec2(pos.x + a_shadowOffset, pos.y + a_shadowOffset), shadowCol, a_text, a_textEnd);
        draw_list->AddText(pos, textCol, a_text, a_textEnd);

        ImVec2 textSize = ImGui::CalcTextSize(a_text, a_textEnd, false, 0.0f);
    }

    void TextShadowImplExDummy(ImVec2 a_pos, const char* a_text, const char* a_textEnd, ImU32 a_textColor, ImU32 a_shadowColor, float a_shadowOffset) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        ImU32 shadowCol = ApplyAlpha(a_shadowColor);
        ImU32 textCol = ApplyAlpha(a_textColor);

        draw_list->AddText(ImVec2(a_pos.x + a_shadowOffset, a_pos.y + a_shadowOffset), shadowCol, a_text, a_textEnd);
        draw_list->AddText(a_pos, textCol, a_text, a_textEnd);

        ImVec2 textSize = ImGui::CalcTextSize(a_text, a_textEnd, false, 0.0f);
    }

    void TextShadowImpl(const char* a_text, const char* a_textEnd, ImU32 a_textColor, ImU32 a_shadowColor, float a_shadowOffset) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetCursorScreenPos();

        // Apply current alpha to colors
        ImU32 shadowCol = ApplyAlpha(a_shadowColor);
        ImU32 textCol = ApplyAlpha(a_textColor);

        draw_list->AddText(ImVec2(pos.x + a_shadowOffset, pos.y + a_shadowOffset), shadowCol, a_text, a_textEnd);
        draw_list->AddText(pos, textCol, a_text, a_textEnd);

        ImVec2 textSize = ImGui::CalcTextSize(a_text, a_textEnd, false, 0.0f);
        ImGui::Dummy(textSize);
    }

    void TextShadowImplEx(ImVec2 a_pos, const char* a_text, const char* a_textEnd, ImU32 a_textColor, ImU32 a_shadowColor, float a_shadowOffset) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        ImU32 shadowCol = ApplyAlpha(a_shadowColor);
        ImU32 textCol = ApplyAlpha(a_textColor);

        draw_list->AddText(ImVec2(a_pos.x + a_shadowOffset, a_pos.y + a_shadowOffset), shadowCol, a_text, a_textEnd);
        draw_list->AddText(a_pos, textCol, a_text, a_textEnd);

        ImVec2 textSize = ImGui::CalcTextSize(a_text, a_textEnd, false, 0.0f);
        ImGui::Dummy(textSize);
    }
}
