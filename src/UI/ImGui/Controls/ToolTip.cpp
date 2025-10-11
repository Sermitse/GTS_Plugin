#include "UI/ImGui/Controls/ToolTip.hpp"
#include "UI/ImGui/Lib/imgui.h"
#include "UI/ImGui/Lib/imgui_internal.h"

namespace ImGuiEx {

    constexpr float TooltipDelay = 0.45f; //sec

    void Tooltip(const char* a_tip, bool a_noDelay) {
        if (!a_tip) return;
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && (GImGui->HoveredIdTimer > TooltipDelay || a_noDelay)) {
            ImGui::SetTooltip(a_tip);
        }
    }
}
