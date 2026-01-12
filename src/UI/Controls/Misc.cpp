#include "UI/Controls/Misc.hpp"

namespace ImGuiEx {

    void SeperatorH(float a_sepHeight) {
        ImGui::Spacing();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, a_sepHeight);
        ImGui::Spacing();
    }

    void SeperatorV(float a_sepWidth) {
        ImGui::SameLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, a_sepWidth);
        ImGui::SameLine();
    }

    void SeperatorVFullLength() {

        ImGui::SameLine();

        const float height = ImGui::GetContentRegionAvail().y;
        const ImVec2 start = ImGui::GetCursorScreenPos();
        const ImVec2 end = { start.x, start.y + height };
        ImGui::GetWindowDrawList()->AddLine(start, end, ImGui::GetColorU32(ImGuiCol_Separator));

        // maintain layout
        ImGui::Dummy({ 1.0f, height });

        ImGui::SameLine();
    }

}