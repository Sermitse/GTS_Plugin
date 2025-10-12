#pragma once
#include "Config/Settings/SettingsKeybinds.hpp"

namespace GTS {

    class KeybindHandler {

        public:
	    static auto GetAllInputEvents() -> std::vector<BaseEventData_t>;
	    static auto GetAllInputEventsView();
	    static auto GetInputEventsByCategory(LInputCategory_t a_category);

	    static bool ValidateInputEventArray(
			toml::ordered_array& a_tomlArray, 
			const std::vector<BaseEventData_t>& a_defaultEvents, 
			std::vector<BaseEventData_t>& a_inputEvents
		);

		private:

        static bool ValidateAndUpdateEntry(
			toml::ordered_array& a_tomlArray, 
			size_t a_index, 
			const std::unordered_set<std::string>& a_validEvents, 
			std::vector<BaseEventData_t>& a_inputEvents
		);

	    static void UpdateKeybindFromTable(
			BaseEventData_t& a_keybind, 
			const toml::ordered_map<string, 
			toml::basic_value<toml::ordered_type_config>>& a_tomlTable
		);
    };
}
