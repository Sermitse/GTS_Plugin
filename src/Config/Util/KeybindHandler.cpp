#include "Config/Util/KeybindHandler.hpp"

namespace GTS {

	auto KeybindHandler::GetAllInputEvents() -> std::vector<BaseEventData_t> {
		std::vector<BaseEventData_t> result;
		result.reserve(DefaultEvents.size());
		for (const auto& item : DefaultEvents) {
			result.push_back(item.Event);
		}
		return result;
	}

	auto KeybindHandler::GetAllInputEventsView() {
		return DefaultEvents | std::views::transform([](const InputEvent_t& a_item) -> const BaseEventData_t& {
			return a_item.Event;
		});
	}

	auto KeybindHandler::GetInputEventsByCategory(LInputCategory_t a_category) {
		return DefaultEvents | std::views::filter([a_category](const InputEvent_t& item) {
			return item.UICategory == a_category;
		}) | std::views::transform([](const InputEvent_t& item) -> const BaseEventData_t& {
			return item.Event;
		});
	}


	bool KeybindHandler::ValidateInputEventArray(toml::ordered_array& a_tomlArray,
		const std::vector<BaseEventData_t>& a_defaultEvents, std::vector<BaseEventData_t>& a_inputEvents) {

		std::unordered_set<std::string> validEvents;

		for (const auto& bind : a_defaultEvents) {
			validEvents.insert(bind.Event);
		}

		// Process in reverse order for safe erasing
		for (std::size_t i = a_tomlArray.size(); i-- > 0; ) {
			if (!ValidateAndUpdateEntry(a_tomlArray, i, validEvents, a_inputEvents)) {
				a_tomlArray.erase(a_tomlArray.begin() + i);
			}
		}
		return true;
	}

	bool KeybindHandler::ValidateAndUpdateEntry(toml::ordered_array& a_tomlArray, size_t a_index,
		const std::unordered_set<std::string>& a_validEvents, std::vector<BaseEventData_t>& a_inputEvents) {

		const auto& entry = a_tomlArray[a_index];

		if (!entry.is_table()) {
			return false;
		}

		const auto& table = entry.as_table();
		const auto itEvent = table.find("Event");
		if (itEvent == table.end() || !itEvent->second.is_string()) {
			return false;
		}

		const std::string& eventName = itEvent->second.as_string();
		if (!a_validEvents.contains(eventName)) {
			return false;
		}

		auto bindIt = ranges::find_if(a_inputEvents, [&](const BaseEventData_t& a_ke) {
			return a_ke.Event == eventName;
		});

		if (bindIt == a_inputEvents.end()) {
			return false;
		}

		UpdateKeybindFromTable(*bindIt, table);

		return true;
	}

	void KeybindHandler::UpdateKeybindFromTable(BaseEventData_t& a_keybind,
		const toml::ordered_map<string, toml::basic_value<toml::ordered_type_config>>& a_tomlTable) {

		// Update Keys
		auto keysIt = a_tomlTable.find("Keys");
		if (keysIt != a_tomlTable.end() && keysIt->second.is_array()) {
			std::vector<std::string> keys;
			for (const auto& keyVal : keysIt->second.as_array()) {
				if (keyVal.is_string()) {
					keys.push_back(keyVal.as_string());
				}
			}
			a_keybind.Keys = keys;
		}

		// Update Exclusive
		auto exclusiveIt = a_tomlTable.find("Exclusive");
		if (exclusiveIt != a_tomlTable.end() && exclusiveIt->second.is_boolean()) {
			a_keybind.Exclusive = exclusiveIt->second.as_boolean();
		}

		// Update Trigger
		auto triggerIt = a_tomlTable.find("Trigger");
		if (triggerIt != a_tomlTable.end() && triggerIt->second.is_string()) {
			std::string triggerStr = triggerIt->second.as_string();
			if (auto optTrigger = magic_enum::enum_cast<LTriggerType_t>(triggerStr); optTrigger.has_value()) {
				a_keybind.Trigger = std::string(magic_enum::enum_name(*optTrigger));
			}
		}

		// Update Duration
		auto durationIt = a_tomlTable.find("Duration");
		if (durationIt != a_tomlTable.end() &&
			(durationIt->second.is_floating() || durationIt->second.is_integer())) {
			if (durationIt->second.is_floating())
				a_keybind.Duration = static_cast<float>(durationIt->second.as_floating());
			else
				a_keybind.Duration = static_cast<float>(durationIt->second.as_integer());
		}

		// Update BlockInput
		auto blockIt = a_tomlTable.find("BlockInput");
		if (blockIt != a_tomlTable.end() && blockIt->second.is_string()) {
			std::string blockStr = blockIt->second.as_string();
			if (auto optBlock = magic_enum::enum_cast<LBlockInputTypes_t>(blockStr); optBlock.has_value()) {
				a_keybind.BlockInput = std::string(magic_enum::enum_name(*optBlock));
			}
		}

		// Update Disabled
		auto disabledIt = a_tomlTable.find("Disabled");
		if (disabledIt != a_tomlTable.end() && disabledIt->second.is_boolean()) {
			a_keybind.Disabled = disabledIt->second.as_boolean();
		}
	}
}
