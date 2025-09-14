#pragma once
#include "Config/Settings/SettingsKeybinds.hpp"

namespace GTS {

    class KeybindHandler {

        public:

	    static auto GetAllInputEvents() -> std::vector<BaseEventData_t> {
            std::vector<BaseEventData_t> result;
            result.reserve(DefaultEvents.size());
            for (const auto& item : DefaultEvents) {
                result.push_back(item.Event);
            }
            return result;
        }

	    static auto GetAllInputEventsView() {
            return DefaultEvents | std::views::transform([](const InputEvent_t& item) -> const BaseEventData_t& {
                return item.Event;
            });
        }

	    static auto GetInputEventsByCategory(LInputCategory_t category) {
            return DefaultEvents | std::views::filter([category](const InputEvent_t& item) {
                return item.Category == category;
                }) | std::views::transform([](const InputEvent_t& item) -> const BaseEventData_t& {
                    return item.Event;
            });
        }

        static bool ValidateInputEventArray(toml::ordered_array& inputEventArray, const std::vector<BaseEventData_t>& defaultEvents, std::vector<BaseEventData_t>& inputEvents) {

            std::unordered_set<std::string> validEvents;

            for (const auto& bind : defaultEvents) {
                validEvents.insert(bind.Event);
            }

            // Process in reverse order for safe erasing
            for (std::size_t i = inputEventArray.size(); i-- > 0; ) {
                if (!ValidateAndUpdateEntry(inputEventArray, i, validEvents, inputEvents)) {
                    inputEventArray.erase(inputEventArray.begin() + i);
                }
            }
            return true;
        }

        private:
        static bool ValidateAndUpdateEntry(toml::ordered_array& inputEventArray, size_t index, const std::unordered_set<std::string>& validEvents, std::vector<BaseEventData_t>& inputEvents) {

            const auto& entry = inputEventArray[index];

            if (!entry.is_table()) {
                return false;
            }

            const auto& table = entry.as_table();
            const auto itEvent = table.find("Event");
            if (itEvent == table.end() || !itEvent->second.is_string()) {
                return false;
            }

            const std::string& eventName = itEvent->second.as_string();
            if (!validEvents.contains(eventName)) {
                return false;
            }

            auto bindIt = ranges::find_if(inputEvents, [&](const BaseEventData_t& ke) {
            	return ke.Event == eventName;
            });

            if (bindIt == inputEvents.end()) {
                return false;
            }

            UpdateKeybindFromTable(*bindIt, table);

            return true;
        }

        static void UpdateKeybindFromTable(BaseEventData_t& bind, const toml::ordered_map<string, toml::basic_value<toml::ordered_type_config>>& table) {
            // Update Keys
            auto keysIt = table.find("Keys");
            if (keysIt != table.end() && keysIt->second.is_array()) {
                std::vector<std::string> keys;
                for (const auto& keyVal : keysIt->second.as_array()) {
                    if (keyVal.is_string()) {
                        keys.push_back(keyVal.as_string());
                    }
                }
                bind.Keys = keys;
            }

            // Update Exclusive
            auto exclusiveIt = table.find("Exclusive");
            if (exclusiveIt != table.end() && exclusiveIt->second.is_boolean()) {
                bind.Exclusive = exclusiveIt->second.as_boolean();
            }

            // Update Trigger
            auto triggerIt = table.find("Trigger");
            if (triggerIt != table.end() && triggerIt->second.is_string()) {
                std::string triggerStr = triggerIt->second.as_string();
                if (auto optTrigger = magic_enum::enum_cast<LTriggerType_t>(triggerStr); optTrigger.has_value()) {
                    bind.Trigger = std::string(magic_enum::enum_name(*optTrigger));
                }
            }

            // Update Duration
            auto durationIt = table.find("Duration");
            if (durationIt != table.end() &&
                (durationIt->second.is_floating() || durationIt->second.is_integer())) {
                if (durationIt->second.is_floating())
                    bind.Duration = static_cast<float>(durationIt->second.as_floating());
                else
                    bind.Duration = static_cast<float>(durationIt->second.as_integer());
            }

            // Update BlockInput
            auto blockIt = table.find("BlockInput");
            if (blockIt != table.end() && blockIt->second.is_string()) {
                std::string blockStr = blockIt->second.as_string();
                if (auto optBlock = magic_enum::enum_cast<LBlockInputTypes_t>(blockStr); optBlock.has_value()) {
                    bind.BlockInput = std::string(magic_enum::enum_name(*optBlock));
                }
            }

            // Update Disabled
            auto disabledIt = table.find("Disabled");
            if (disabledIt != table.end() && disabledIt->second.is_boolean()) {
                bind.Disabled = disabledIt->second.as_boolean();
            }
        }
    };
}
