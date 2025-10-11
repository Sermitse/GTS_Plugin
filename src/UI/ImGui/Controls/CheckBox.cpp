#include "UI/ImGui/Controls/CheckBox.hpp"

#include "UI/ImGui/Lib/imgui.h"


namespace ImGuiEx {

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
}
