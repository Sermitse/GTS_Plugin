#pragma once
#include "UI/ImGui/Lib/imgui.h"
#include "UI/ImGui/Lib/imgui_internal.h"

// RAII helper to push an ID on construction and pop it on destruction
struct ImGuiUniqueID {
    ImGuiUniqueID(int id) { ImGui::PushID(id); }
    ~ImGuiUniqueID() { ImGui::PopID(); }
};

#define CONCAT_IMPL(a, b) a##b
#define CONCAT(a, b) CONCAT_IMPL(a, b)

#define ImUtil_Unique                                                     \
    if (ImGuiUniqueID CONCAT(uniqueID_, __COUNTER__)(__COUNTER__); true)

#define Imutil_UniqueCall(func_call)                            \
    ImGuiUniqueID CONCAT(uniqueID_, __COUNTER__)(__COUNTER__);  \
    func_call

namespace ImUtil {

	//Predefined colors {R, G, B, A} (0.0 to 1.0f)
    namespace Colors {
        constexpr ImVec4 Error = { 1.0f, 0.35f, 0.30f, 0.9f };
        constexpr ImVec4 OK = { 0.30f, 1.0f, 0.35f, 0.9f };
        constexpr ImVec4 Subscript = { 1.0f, 1.0f, 1.0f, 0.5f };
        constexpr ImVec4 Disabled = { 0.4f, 0.1f, 0.1f, 1.0f };
        constexpr ImVec4 Message = { 1.0f, 0.5f, 0.0f, 1.0f };
    }

    //Predefined colors {R, G, B, A} (0.0 to 1.0f)
    namespace Format {
        PSString F0 = "%.0f";
        PSString F1 = "%.1f";
        PSString F2 = "%.2f";
        PSString F3 = "%.3f";
        PSString F4 = "%.4f";
    }

    constexpr uint32_t HeaderFlagsDefaultOpen = ImGuiTreeNodeFlags_DefaultOpen;

    [[nodiscard]] bool ValidState() noexcept;
    [[nodiscard]] ImVec2 ScaleToViewport(float a_Percentage);
    [[nodiscard]] ImGuiWindow* GetEffectiveFocusedWindow(ImGuiContext* a_ctx);
    [[nodiscard]] ImVec2 GetCenteredTextPos(const ImVec2& a_iconPos, const ImVec2& a_iconSize, const char* a_text);
    void AdvanceCursorInline(float& a_startposX, float a_startPosY, uint32_t a_iconSize, float a_spacing = 4.0f);

    void ToggleFlag(uint16_t& flags, uint32_t flag, bool state);
    void ToggleFlag(uint32_t& flags, uint32_t flag, bool state);

}

