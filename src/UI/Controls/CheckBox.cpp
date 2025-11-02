#include "UI/Controls/CheckBox.hpp"
#include "UI/Controls/ToolTip.hpp"

#include "UI/Lib/imgui.h"


namespace ImGuiEx {

    bool CheckBox(const char* a_label, bool* a_state, const char* a_toolTip, const bool a_disabled) {
        ImGui::BeginDisabled(a_disabled);

        const bool res = ImGui::Checkbox(a_label, a_state);
        Tooltip(a_toolTip);

        ImGui::EndDisabled();
        return res;
    }

}
