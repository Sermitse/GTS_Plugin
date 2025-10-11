#include "UI/ImGui/Lib/imgui.h"
#include "UI/ImGui/Lib/imgui_internal.h"
#include "UI/ImGui/Controls/Misc.hpp"

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

}