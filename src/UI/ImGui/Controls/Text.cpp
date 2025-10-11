#include "UI/ImGui/Controls/Text.hpp"
#include "UI/ImGui/Lib/imgui_internal.h"

namespace ImGuiEx {

    // Helper to display a little (?) mark which shows a tooltip when hovered.
    void HelpMarker(const char* a_desc) {

        ImGui::Text("[?]");
        if (ImGui::BeginItemTooltip()) {

            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(a_desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    void TextShadow(const char* a_cfmt, ...) {
        va_list args;
        va_start(args, a_cfmt);
        const char* text, * text_end;
        ImFormatStringToTempBufferV(&text, &text_end, a_cfmt, args);
        va_end(args);
        TextShadowImpl(text, text_end);

    }

    void TextColorShadow(ImVec4 a_col, const char* a_cfmt, ...) {
        va_list args;
        va_start(args, a_cfmt);
        const char* text, * text_end;
        ImFormatStringToTempBufferV(&text, &text_end, a_cfmt, args);
        va_end(args);
        TextShadowImpl(text, text_end, ImGui::ColorConvertFloat4ToU32(a_col));
    }

    void TextShadowImpl(const char* a_text, const char* a_textEnd, ImU32 a_textColor, ImU32 a_shadowColor, float a_shadowOffset) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetCursorScreenPos(); // Get the absolute screen position

        // Draw shadow text
        draw_list->AddText(ImVec2(pos.x + a_shadowOffset, pos.y + a_shadowOffset), a_shadowColor, a_text, a_textEnd);
        // Draw main text
        draw_list->AddText(pos, a_textColor, a_text, a_textEnd);

        // Calculate the text size
        ImVec2 textSize = ImGui::CalcTextSize(a_text, a_textEnd, false, 0.0f);
        // Move the cursor by adding an invisible dummy widget of the same size as the text
        ImGui::Dummy(textSize);
    }
}
