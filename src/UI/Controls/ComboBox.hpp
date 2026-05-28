#pragma once

#include "UI/Controls/ToolTip.hpp"

namespace ImGuiEx {

	template <typename T>
	bool ComboEx(const char* a_label, std::string& a_currentValue, const char* a_toolTip = nullptr, bool a_disabled = false, bool a_hasTotal = false) {
		// Retrieve enum metadata
		constexpr auto enumNames = magic_enum::enum_names<T>();
		constexpr auto enumValues = magic_enum::enum_values<T>();

		// Build the items string with pretty-printed names
		std::ostringstream itemsStream;
		const int offset = a_hasTotal ? 2 : 1;
		for (size_t i = 0; i <= enumNames.size() - offset; i++) {
			itemsStream << GTS::HumanizeString(enumNames[i]) << '\0';
		}
		std::string items = itemsStream.str();

		// Find current enum index
		auto currentEnum = magic_enum::enum_cast<T>(a_currentValue);
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
			a_currentValue = std::string(enumNames[0]);
		}

		ImGui::BeginDisabled(a_disabled);

		bool res = ImGui::Combo(a_label, &currentIndex, items.c_str());
		Tooltip(a_toolTip);

		ImGui::EndDisabled();

		if (res) {
			T selectedEnum = enumValues[currentIndex];
			a_currentValue = std::string(magic_enum::enum_name(selectedEnum));
		}

		return res;
	}

	template <typename T>
	bool ComboExFiltered(const char* a_label, std::string& a_currentValue, std::function<bool(T)> a_isDisabled = nullptr, std::function<bool(T)> a_isHidden = nullptr, const char* a_toolTip = nullptr, bool a_disabled = false) {
		constexpr auto enumNames = magic_enum::enum_names<T>();
		constexpr auto enumValues = magic_enum::enum_values<T>();

		auto currentEnum = magic_enum::enum_cast<T>(a_currentValue);
		if (!currentEnum.has_value())
			a_currentValue = std::string(enumNames[0]);

		std::string previewLabel = GTS::HumanizeString(a_currentValue);

		ImGui::BeginDisabled(a_disabled);
		bool changed = false;

		if (ImGui::BeginCombo(a_label, previewLabel.c_str())) {

			// Build sorted index list: unlocked entries first, disabled after
			std::vector<size_t> order(enumNames.size());
			std::iota(order.begin(), order.end(), 0);
			std::stable_partition(order.begin(), order.end(), [&](size_t i) {
				T val = enumValues[i];
				const bool hidden = a_isHidden && a_isHidden(val);
				const bool disabled = a_isDisabled && a_isDisabled(val);
				return !hidden && !disabled;
			});

			for (size_t i : order) {
				T val = enumValues[i];

				if (a_isHidden && a_isHidden(val)) continue;

				const bool disabled = a_isDisabled && a_isDisabled(val);
				const bool selected = currentEnum.has_value() && currentEnum.value() == val;
				std::string label = GTS::HumanizeString(enumNames[i]);

				ImGui::BeginDisabled(disabled);
				if (ImGui::Selectable(label.c_str(), selected)) {
					a_currentValue = std::string(enumNames[i]);
					changed = true;
				}
				ImGui::EndDisabled();

				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		Tooltip(a_toolTip);
		ImGui::EndDisabled();

		return changed;
	}

	template <typename T>
	bool IComboEx(const char* a_label, int* a_currentIndex, const char* a_toolTip = nullptr, bool a_disabled = false, bool a_hasTotal = false) {
		// Retrieve enum metadata
		constexpr auto enumNames = magic_enum::enum_names<T>();
		// No need for enumValues if you don't update a string

		// Build the items string with pretty-printed names
		std::ostringstream itemsStream;
		const int offset = a_hasTotal ? 2 : 1;
		for (size_t i = 0; i <= enumNames.size() - offset; i++) {
			itemsStream << GTS::HumanizeString(enumNames[i]) << '\0';
		}
		std::string items = itemsStream.str();

		ImGui::BeginDisabled(a_disabled);
		bool res = ImGui::Combo(a_label, a_currentIndex, items.c_str());
		Tooltip(a_toolTip);
		ImGui::EndDisabled();

		return res;
	}
}
