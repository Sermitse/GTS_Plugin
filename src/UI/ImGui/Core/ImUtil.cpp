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

    ImVec2 GetCenteredTextPos(const ImVec2& a_iconPos, const ImVec2& a_iconSize, const char* a_text) {
        ImVec2 textSize = ImGui::CalcTextSize(a_text);
        return {
            (a_iconPos.x + a_iconSize.x * .5f)  -  (textSize.x * .5f),
            (a_iconPos.y + a_iconSize.y * .5f)  -  (textSize.y * .5f)
        };
    }

    void AdvanceCursorInline(float& a_startposX, float a_startPosY, uint32_t a_iconSize, float a_spacing) {
        a_startposX += a_iconSize + a_spacing;
        ImGui::SetCursorPos({ a_startposX, a_startPosY });
    }
}
