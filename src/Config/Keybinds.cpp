#include "Config/Keybinds.hpp"
#include "Config/Util/KeybindHandler.hpp"
#include "Config/Util/FileUtils.hpp"

namespace GTS {

    bool Keybinds::LoadKeybinds() {

        std::lock_guard<std::mutex> lock(_ReadWriteLock);

        if (!FileUtils::CheckOrCreateFile(InputFile)) {
            return false;
        }

        const auto DefaultInputEvents = KeybindHandler::GetAllInputEvents();

        try {
            InputEvents = DefaultInputEvents;
            TomlData = toml::parse<toml::ordered_type_config>(InputFile);
        }
        catch (const toml::exception& e) {
            logger::error("Toml load exception: {}", e.what());
            TomlData = toml::basic_value<toml::ordered_type_config>();
            return false;
        }
        catch (const std::exception& e) {
            logger::error("std toml load exception: {}", e.what());
            TomlData = toml::basic_value<toml::ordered_type_config>();
            return false;
        }
        catch (...) {
            logger::error("Unk toml load exception");
            return false;
        }

        // Get or create InputEvent array
        toml::ordered_array& inputEventArray = [&]() -> auto& {
            if (TomlData.count("InputEvent") && TomlData["InputEvent"].is_array()) {
                return TomlData["InputEvent"].as_array();
            }
            TomlData["InputEvent"] = toml::ordered_array();
            return TomlData["InputEvent"].as_array();
        }();

        KeybindHandler::ValidateInputEventArray(inputEventArray, DefaultInputEvents, InputEvents);
        return true;
    }

    bool Keybinds::SaveKeybinds() {

        std::lock_guard<std::mutex> lock(_ReadWriteLock);

        if (!FileUtils::CheckOrCreateFile(InputFile)) {
            return false;
        }

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

        return SaveTOMLToFile(TomlData, InputFile);
    }

    void Keybinds::ResetKeybinds() {
        InputEvents = KeybindHandler::GetAllInputEvents();
        TomlData = toml::basic_value<toml::ordered_type_config>();
    }

    std::string Keybinds::DebugName() {
        return "::Keybinds";
    }

    void Keybinds::DataReady() {
        Keybinds::LoadKeybinds();
    }
}
