#pragma once

#include "UI/ImGui/Lib/imgui.h"

namespace ImGuiEx {
    void HelpMarker(const char* a_desc);
    void TextShadow(const char* a_cfmt, ...);
    void TextColorShadow(ImVec4 a_col, const char* a_cfmt, ...);

    void TextShadowEx(ImVec2 a_pos, const char* a_cfmt, ...);

    void TextShadowExNoDummy(ImVec2 a_pos, const char* a_cfmt, ...);

    void TextShadowImpl(const char* a_text, const char* a_textEnd, ImU32 a_textColor = IM_COL32(255, 255, 255, 255), 
        ImU32 a_shadowColor = IM_COL32(0, 0, 0, 192 * ImGui::GetStyle().Alpha), float a_shadowOffset = 1.2f);

    void TextShadowImplEx(ImVec2 a_pos, const char* a_text, const char* a_textEnd, ImU32 a_textColor = IM_COL32(255, 255, 255, 255),
        ImU32 a_shadowColor = IM_COL32(0, 0, 0, 192 * ImGui::GetStyle().Alpha), float a_shadowOffset = 1.2f);

    void TextShadowImplNoDummy(const char* a_text, const char* a_textEnd, ImU32 a_textColor = IM_COL32(255, 255, 255, 255),
        ImU32 a_shadowColor = IM_COL32(0, 0, 0, 192 * ImGui::GetStyle().Alpha), float a_shadowOffset = 1.2f);

    void TextShadowImplExDummy(ImVec2 a_pos, const char* a_text, const char* a_textEnd, ImU32 a_textColor = IM_COL32(255, 255, 255, 255),
        ImU32 a_shadowColor = IM_COL32(0, 0, 0, 192 * ImGui::GetStyle().Alpha), float a_shadowOffset = 1.2f);

}
