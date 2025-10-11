#include "UI/ImGui/Controls/ConditionalHeader.hpp"
#include "UI/ImGui/Lib/imgui.h"

namespace ImGuiEx {

    bool ConditionalHeader(const std::string& a_label, const std::string& a_ConditionText, bool a_condition, bool a_defaultopen) {
        ImGui::BeginDisabled(!a_condition);
        auto flags = a_condition && a_defaultopen ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;

        std::string fullText = a_condition ?
            std::format("{}##", a_label) :
            std::format("{} [{}]", a_label, a_ConditionText);

        bool res = ImGui::CollapsingHeader(fullText.c_str(), flags);
        ImGui::EndDisabled();
        return res;
    }

}

