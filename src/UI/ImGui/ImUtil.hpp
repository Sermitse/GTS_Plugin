#pragma once

#include "UI/ImGui/Lib/imgui.h"
#include "UI/ImGui/Lib/imgui_internal.h"


// RAII helper to push an ID on construction and pop it on destruction
// So i don't have to remember to use push pop constantly...
struct ImGuiUniqueID {
    ImGuiUniqueID(int id) { ImGui::PushID(id); }
    ~ImGuiUniqueID() { ImGui::PopID(); }
};

#define CONCAT_IMPL(a, b) a##b
#define CONCAT(a, b) CONCAT_IMPL(a, b)

#define ImUtil_Unique \
    if (ImGuiUniqueID CONCAT(uniqueID_, __COUNTER__)(__COUNTER__); true)

#define Imutil_UniqueCall(func_call) \
    ImGuiUniqueID CONCAT(uniqueID_, __COUNTER__)(__COUNTER__); \
    func_call

namespace ImUtil {
    
    //-------------------
    //  Constants
    //-------------------

    //Predefined colors {R, G, B, A} (0.0 to 1.0f)
    constexpr ImVec4 ColorError = {1.0f, 0.35f, 0.30f, 0.9f};
    constexpr ImVec4 ColorOK = { 0.30f, 1.0f, 0.35f, 0.9f };
    constexpr ImVec4 ColorSubscript = {1.0f, 1.0f, 1.0f, 0.5f};
    constexpr ImVec4 ColorDisabled = { 0.4f, 0.1f, 0.1f, 1.0f };
    constexpr ImVec4 ColorMessage = { 1.0f, 0.5f, 0.0f, 1.0f };
    constexpr uint32_t HeaderFlagsDefaultOpen = ImGuiTreeNodeFlags_DefaultOpen;
    
    constexpr float TooltipDelay = 0.45f; //sec

    [[nodiscard]] static inline bool ValidState() noexcept {
        ImGuiContext* ctx = ImGui::GetCurrentContext();
        return ctx && ctx->WithinFrameScope;
    }

    static void RenderDropShadow(ImTextureID tex_id, float size, ImU8 opacity);

    inline void SeperatorH(){
        ImGui::Spacing();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.f);
        ImGui::Spacing();
    }

    inline void SeperatorV(){
        ImGui::SameLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 1.f);
        ImGui::SameLine();
    }

    [[nodiscard]] inline ImVec2 ScaleToViewport(float a_Percentage) {
        ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
        //std::clamp(a_Percentage, 10.0f, 100.0f);
        return { viewportSize.x * (a_Percentage / 100.f), viewportSize.y * (a_Percentage / 100.f) };
    }

    const std::string HumanizeString(std::string_view name);

    const bool Button(const char* a_label, const char* a_Tooltip = nullptr, bool a_disabled = false, float a_padding = 1.0f);
    const bool ImageButtonEx(const char* a_label, ImTextureID a_texture, const char* a_Tooltip = nullptr, bool a_disabled = false, ImVec2 a_imgSize = {32,32});
    const bool ImageButton(const char* a_label, const std::string& a_TexName, const int a_size = 32, const char* a_Tooltip = nullptr, const bool a_disabled = false);
    const bool CheckBox(const char* a_label, bool* a_state, const char* a_Tooltip = nullptr, const bool a_disabled = false);
    const bool SliderF(const char* a_label, float* a_value, float a_min, float a_max, const char* a_Tooltip = nullptr, const char* fmt = "%.2f", const bool a_disabled = false, const bool a_alwaysclamp = false);
    const bool SliderF2(const char* a_label, float* a_value, float a_min, float a_max, const char* a_Tooltip = nullptr, const char* fmt = "%.2f", const bool a_disabled = false);
    const bool SliderF3(const char* a_label, float* a_value, float a_min, float a_max, const char* a_Tooltip = nullptr, const char* fmt = "%.2f", const bool a_disabled = false);
    
    void CenteredProgress(float fraction, const ImVec2& size_arg, const char* overlay, const float heightmult,
        float borderThickness = 1.0f, bool useGradient = false,
        float gradientDarkFactor = 0.7f, float gradientLightFactor = 1.3f,
        bool useRounding = true,
        bool useCustomGradientColors = false,
        ImU32 gradientStartColor = IM_COL32(255, 0, 0, 255),
        ImU32 gradientEndColor = IM_COL32(0, 255, 0, 255),
        bool flipGradientDirection = false);
    
    void Bitfield(const char* a_label, uint32_t* a_bitfield);

    void HelpMarker(const char* a_desc);

    const bool ConditionalHeader(const std::string a_label, const std::string a_ConditionText, const bool a_condition, const bool a_defaultopen = true);

    inline void Tooltip(const char* a_Tip, bool a_NoDelay = false){
        if(!a_Tip) return;
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && (GImGui->HoveredIdTimer > TooltipDelay || a_NoDelay)){
            ImGui::SetTooltip(a_Tip);
        }
    }


    template <typename T>
    bool ComboEx(const char* a_label, std::string& currentValue, const char* a_Tooltip = nullptr, bool a_disabled = false, bool a_hasTotal = false) {
        // Retrieve enum metadata
        constexpr auto enumNames = magic_enum::enum_names<T>();
        constexpr auto enumValues = magic_enum::enum_values<T>();

        // Build the items string with pretty-printed names
        std::ostringstream itemsStream;
        const int offset = a_hasTotal ? 2 : 1;
        for (size_t i = 0; i <= enumNames.size() - offset; i++) {
            itemsStream << HumanizeString(enumNames[i]) << '\0';
        }
        std::string items = itemsStream.str();

        // Find current enum index
        auto currentEnum = magic_enum::enum_cast<T>(currentValue);
        int currentIndex = 0;
        
        if (currentEnum.has_value()) {
            // Find the index of the current enum value
            const auto it = std::find(enumValues.begin(), enumValues.end(), currentEnum.value());
            if (it != enumValues.end()) {
                currentIndex = static_cast<int>(std::distance(enumValues.begin(), it));
            }
        } 
        else {
            // Reset to first value if invalid
            currentValue = std::string(enumNames[0]);
        }

        ImGui::BeginDisabled(a_disabled);

        bool res = ImGui::Combo(a_label, &currentIndex, items.c_str());
        Tooltip(a_Tooltip);

        ImGui::EndDisabled();

        if(res){
            T selectedEnum = enumValues[currentIndex];
            currentValue = std::string(magic_enum::enum_name(selectedEnum));
        }

        return res;
    }

    template <typename T>
    bool IComboEx(const char* a_label, int* currentIndex, const char* a_Tooltip = nullptr, bool a_disabled = false, bool a_hasTotal = false) {
        // Retrieve enum metadata
        constexpr auto enumNames = magic_enum::enum_names<T>();
        // No need for enumValues if you don't update a string

        // Build the items string with pretty-printed names
        std::ostringstream itemsStream;
        const int offset = a_hasTotal ? 2 : 1;
        for (size_t i = 0; i <= enumNames.size() - offset; i++) {
            itemsStream << HumanizeString(enumNames[i]) << '\0';
        }
        std::string items = itemsStream.str();

        ImGui::BeginDisabled(a_disabled);
        bool res = ImGui::Combo(a_label, currentIndex, items.c_str());
        Tooltip(a_Tooltip);
        ImGui::EndDisabled();

        return res;
    }

    template <typename EnumT>
    void Bitfield(std::underlying_type_t<EnumT>* a_bitfield) {
        static_assert(std::is_enum_v<EnumT>, "EnumT must be an enum type");

        using Underlying = std::underlying_type_t<EnumT>;

        // First, collect all enum values and their display names
        std::vector<std::pair<EnumT, std::string>> enumLabels;
        for (auto flag : magic_enum::enum_values<EnumT>()) {
            std::string checkboxLabel = HumanizeString(magic_enum::enum_name(flag));
            enumLabels.emplace_back(flag, checkboxLabel);
        }

        // Calculate max text width for each column
        constexpr int numColumns = 3;
        std::vector<float> columnWidths(numColumns, 0.0f);

        for (size_t i = 0; i < enumLabels.size(); ++i) {
            int columnIndex = i % numColumns;
            float textWidth = ImGui::CalcTextSize(enumLabels[i].second.c_str()).x;
            textWidth += 70.0f; 
            columnWidths[columnIndex] = std::max(columnWidths[columnIndex], textWidth);
        }

        // Calculate cumulative widths for SameLine positioning
        std::vector<float> cumulativeWidths(numColumns, 0.0f);
        for (int i = 1; i < numColumns; ++i) {
            cumulativeWidths[i] = cumulativeWidths[i - 1] + columnWidths[i - 1] + ImGui::GetStyle().FramePadding.x;
        }

        // Display checkboxes with dynamic column widths
        for (size_t i = 0; i < enumLabels.size(); ++i) {
            auto& [flag, checkboxLabel] = enumLabels[i];
            Underlying flagValue = static_cast<Underlying>(flag);
            bool bit = ((*a_bitfield) & flagValue) != 0;

            ImGui::Checkbox(checkboxLabel.c_str(), &bit);

            // Use SameLine with calculated positions, but only between columns (not at end of row)
            if ((i + 1) % numColumns != 0 && i < enumLabels.size() - 1) {
                ImGui::SameLine(cumulativeWidths[(i + 1) % numColumns]);
            }

            // If the checkbox state has changed, update the bitfield
            if (bit != (((*a_bitfield) & flagValue) != 0)) {
                *a_bitfield ^= flagValue;
            }
        }
    }

    [[nodiscard]] bool ContainsString(const std::string& a1, const std::string& a2);

    void TextShadow(const char* fmt, ...);
    void TextShadowImpl(const char* text, const char* textend, ImU32 text_color = IM_COL32(255, 255, 255, 255), ImU32 shadow_color = IM_COL32(0, 0, 0, 192 * ImGui::GetStyle().Alpha), float shadow_offset = 2.0f);

    
}

