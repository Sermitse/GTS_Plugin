#include "UI/Controls/ConditionalHeader.hpp"

namespace ImGuiEx {

    bool ConditionalHeader(const std::string& a_label, const std::string& a_ConditionText, bool a_condition, bool a_defaultopen) {
        ImGui::BeginDisabled(!a_condition);
        auto flags = a_condition && a_defaultopen ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;
        const std::string& headerText = !a_ConditionText.empty() && !a_condition ? a_label + " - " + a_ConditionText : a_label;
        bool res = ImGui::CollapsingHeader(headerText.c_str(), flags);
        ImGui::EndDisabled();
        return res;
    }

}

