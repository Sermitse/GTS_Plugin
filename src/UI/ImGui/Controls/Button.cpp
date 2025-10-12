#include "UI/ImGui/Controls/ToolTip.hpp"
#include "UI/ImGui/Core/ImGraphics.hpp"
#include "UI/ImGui/Lib/imgui.h"

namespace ImGuiEx {

    bool Button(const char* a_label, const char* a_toolTip, const bool a_disabled, const float a_padding) {

        const float paddingX = ImGui::GetStyle().FramePadding.x;
        const float paddingY = ImGui::GetStyle().FramePadding.y;
        const ImVec2 textSize = ImGui::CalcTextSize(a_label);
        const ImVec2 buttonSize = ImVec2(textSize.x + a_padding * (paddingX * 2), textSize.y + a_padding * (paddingY * 2));

        bool res;
        {
            ImGui::BeginDisabled(a_disabled);
			res = ImGui::Button(a_label, buttonSize);
            Tooltip(a_toolTip);
            ImGui::EndDisabled();
        }

        return res;
    }

    bool ImageButtonEx(const char* a_label, ImTextureID a_texture, const char* a_toolTip, const bool a_disabled, ImVec2 a_imgSize) {

        ImGui::BeginDisabled(a_disabled);

        const bool res = ImGui::ImageButton(a_label, a_texture, a_imgSize);
        Tooltip(a_toolTip);

        ImGui::EndDisabled();

        return res;
    }

    bool ImageButton(const char* a_label, const std::string& a_TexName, uint16_t a_size, const char* a_Tooltip, const bool a_disabled) {

        const float size = a_size * ImGui::GetStyle().FontScaleMain;
        const auto [texID, _] = GTS::ImGraphics::GetAsImGuiTexture(a_TexName);
        const bool res = ImageButtonEx(a_label, texID, a_Tooltip, a_disabled, { size, size });

        return res;
    }
}