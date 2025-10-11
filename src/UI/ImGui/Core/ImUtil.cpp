#include "UI/ImGui/Core/ImUtil.hpp"
#include "UI/ImGui/Lib/imgui_internal.h"

namespace ImUtil {

    bool ValidState() noexcept {
	    ImGuiContext* ctx = ImGui::GetCurrentContext();
	    return ctx && ctx->WithinFrameScope;
    }

    ImVec2 ScaleToViewport(float a_Percentage) {
	    ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
	    return { viewportSize.x * (a_Percentage / 100.f), viewportSize.y * (a_Percentage / 100.f) };
    }

}
