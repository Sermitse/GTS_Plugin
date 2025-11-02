#pragma once
#include "UI/ImGui/Lib/imgui.h"

namespace ImGuiEx {

    bool Button(const char* a_label, const char* a_Tooltip = nullptr, bool a_disabled = false, float a_padding = 1.0f);
    bool ImageButtonEx(const char* a_label, ImTextureID a_texture, const char* a_toolTip = nullptr, bool a_disabled = false, ImVec2 a_imgSize = { 32,32 });
    bool ImageButton(const char* a_label, const std::string& a_TexName, uint16_t a_size = 32, const char* a_Tooltip = nullptr, bool a_disabled = false);
    bool ImageButtonTransform(const char* a_label, const std::string& a_TexName, const GTS::ImGraphics::ImageTransform& a_transform, uint16_t a_size = 32, const char* a_Tooltip = nullptr, const bool a_disabled = false);

}
