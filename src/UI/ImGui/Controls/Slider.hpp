#pragma once

namespace ImGuiEx {

    bool SliderF (const char* a_label, float* a_value, float a_min = 0.0f, float a_max = 1.0f, 
        const char* a_tooltip = nullptr, const char* a_cfmt = "%.2f", bool a_disabled = false, bool a_alwaysClamp = true);

    bool SliderF2(const char* a_label, float* a_value, float a_min = 0.0f, float a_max = 1.0f, 
        const char* a_tooltip = nullptr, const char* a_cfmt = "%.2f", bool a_disabled = false, bool a_alwaysClamp = true);

    bool SliderF3(const char* a_label, float* a_value, float a_min = 0.0f, float a_max = 1.0f, 
        const char* a_tooltip = nullptr, const char* a_cfmt = "%.2f", bool a_disabled = false, bool a_alwaysClamp = true);

    bool SliderU16(const char* a_label, uint16_t* a_value, uint16_t a_min, uint16_t a_max, const char* a_tooltip, const char* a_cfmt, bool a_disabled = false, bool a_alwaysClamp = true);
}



