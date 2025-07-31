#include "Config/Keybinds.hpp"

namespace GTS {

    bool Keybinds::LoadKeybinds() {

        std::lock_guard<std::mutex> lock(_ReadWriteLock);

        if (!CheckFile(InputFile)) {
            return false;
        }

        const auto DefaultInputEvents = GetAllInputEvents();

        try {
            // Parse the TOML file and initialize with default keybinds.
            InputEvents = DefaultInputEvents;
            TomlData = toml::parse<toml::ordered_type_config>(InputFile);
        }
        catch (const std::exception& e) {
            TomlData = toml::basic_value<toml::ordered_type_config>();
            return false;
        }
        catch (...) {
            return false;
        }

        // Get (or create) the "InputEvent" array from the TOML data.
        toml::ordered_array& inputEventArray = [&]() -> auto& {
            if (TomlData.count("InputEvent") && TomlData["InputEvent"].is_array()) {
                return TomlData["InputEvent"].as_array();
            }
            TomlData["InputEvent"] = toml::ordered_array();
            return TomlData["InputEvent"].as_array();
        }();

        // Build a set of valid event names from the default keybinds.
        std::unordered_set<std::string> validEvents;
        for (const auto& bind : DefaultInputEvents) {
            validEvents.insert(bind.Event);
        }

        // Process the array entries in reverse order (so that erasing invalid entries is safe).
        for (std::size_t i = inputEventArray.size(); i-- > 0; ) {
            const auto& entry = inputEventArray[i];
            if (!entry.is_table()) {
                inputEventArray.erase(inputEventArray.begin() + i);
                continue;
            }

            const auto& table = entry.as_table();
            const auto itEvent = table.find("Event");
            if (itEvent == table.end() || !itEvent->second.is_string()) {
                inputEventArray.erase(inputEventArray.begin() + i);
                continue;
            }
            const std::string& eventName = itEvent->second.as_string();
            if (!validEvents.contains(eventName)) {
                inputEventArray.erase(inputEventArray.begin() + i);
                continue;
            }

            // Locate the corresponding keybind from the defaults.
            auto bindIt = ranges::find_if(InputEvents,[&](const GTSInputEvent& ke) {
	            return ke.Event == eventName;
            });
            if (bindIt == InputEvents.end())
                continue; // Should not happen.

            // Update Keys if provided.
            auto keysIt = table.find("Keys");
            if (keysIt != table.end() && keysIt->second.is_array()) {
                std::vector<std::string> keys;
                for (const auto& keyVal : keysIt->second.as_array()) {
                    if (keyVal.is_string()) {
                        keys.push_back(keyVal.as_string());
                    }
                }
                bindIt->Keys = keys;
            }

            // Update Exclusive flag if provided.
            auto exclusiveIt = table.find("Exclusive");
            if (exclusiveIt != table.end() && exclusiveIt->second.is_boolean()) {
                bindIt->Exclusive = exclusiveIt->second.as_boolean();
            }

            // Validate and update Trigger.
            auto triggerIt = table.find("Trigger");
            if (triggerIt != table.end() && triggerIt->second.is_string()) {
                std::string triggerStr = triggerIt->second.as_string();
                if (auto optTrigger = magic_enum::enum_cast<TriggerType>(triggerStr); optTrigger.has_value()) {
                    // Update with the canonical name (or you might store the enum itself).
                    bindIt->Trigger = std::string(magic_enum::enum_name(*optTrigger));
                }
                else {
                    // Invalid trigger string; remove this entry.
                    inputEventArray.erase(inputEventArray.begin() + i);
                    continue;
                }
            }

            // Update Duration. Accept both integer and floating-point values.
            auto durationIt = table.find("Duration");
            if (durationIt != table.end() &&
                (durationIt->second.is_floating() || durationIt->second.is_integer())) {
                if (durationIt->second.is_floating())
                    bindIt->Duration = static_cast<float>(durationIt->second.as_floating());
                else
                    bindIt->Duration = static_cast<float>(durationIt->second.as_integer());
            }

            // Validate and update BlockInput.
            auto blockIt = table.find("BlockInput");
            if (blockIt != table.end() && blockIt->second.is_string()) {
                std::string blockStr = blockIt->second.as_string();
                if (auto optBlock = magic_enum::enum_cast<BlockInputTypes>(blockStr); optBlock.has_value()) {
                    bindIt->BlockInput = std::string(magic_enum::enum_name(*optBlock));
                }
                else {
                    // Invalid BlockInput string; remove this entry.
                    inputEventArray.erase(inputEventArray.begin() + i);
                    continue;
                }
            }

            // Update Disabled flag if provided.
            auto disabledIt = table.find("Disabled");
            if (disabledIt != table.end() && disabledIt->second.is_boolean()) {
                bindIt->Disabled = disabledIt->second.as_boolean();
            }
        }
        return true;
    }

    bool Keybinds::SaveKeybinds() {

        std::lock_guard<std::mutex> lock(_ReadWriteLock);

        //Check File
        if (!CheckFile(InputFile)) {
            return false;
        }

        //Try TOML Serialization
        try {
            TomlData["InputEvent"] = InputEvents;
        }
        catch (const toml::exception& e) {
            logger::error("TOML Exception when saving InputEvents: {}", e.what());
            return false;
        }
        catch (...) {
            logger::error("SaveKeybinds() -> Unknown Exception");
            return false;
        }

        //Try Writing to File
        try {

            //Create a output file stream and enable exceptions for it.
            std::ofstream file(InputFile);
            file.exceptions(std::ofstream::failbit);

            //Check if the file is writable...
            if (file.is_open()) {
                file << toml::format(TomlData);
                file.close();
                return true;
            }

            logger::error("Could not open Input.toml for writing. Settings not saved!");
            return false;

        }
        catch (toml::exception& e) {
            logger::error("Could not parse TomlData when trying to save: {}", e.what());
            return false;
        }
        catch (const std::ios_base::failure& e) {
            logger::error("File System Error: {}", e.what());
            return false;
        }
        catch (const std::exception& e) {
            logger::error("SaveKeybinds() -> Misc Exception: {}", e.what());
            return false;
        }
        catch (...) {
            logger::error("SaveKeybinds() -> Unknown Exception");
            return false;
        }
    }

    void Keybinds::ResetKeybinds() {
        //Reset Vector
        InputEvents = GetAllInputEvents();

        //Reset TOML Array
        TomlData = toml::basic_value<toml::ordered_type_config>();
    }
}
