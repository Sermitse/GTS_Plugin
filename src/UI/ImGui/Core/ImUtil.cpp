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

    ImGuiWindow* GetEffectiveFocusedWindow(ImGuiContext* a_ctx) {
        if (a_ctx->NavWindow)
            return a_ctx->NavWindow->RootWindow;
        if (a_ctx->ActiveIdWindow)
            return a_ctx->ActiveIdWindow->RootWindow;
        if (a_ctx->HoveredWindow)
            return a_ctx->HoveredWindow->RootWindow;
        return nullptr;
    }
}
