#pragma once

#include "Config/Settings/SettingsAdvanced.hpp"
#include "Config/Settings/SettingsAI.hpp"
#include "Config/Settings/SettingsAudio.hpp"
#include "Config/Settings/SettingsBalance.hpp"
#include "Config/Settings/SettingsCamera.hpp"
#include "Config/Settings/SettingsGameplay.hpp"
#include "Config/Settings/SettingsGeneral.hpp"
#include "Config/Settings/SettingsUI.hpp"

namespace GTS {

    class SettingsOperations : TOMLHandler {
        public:
        template<typename Config>
        static bool SerializeAllStructsToTOML(Config& config, toml::ordered_value& tomlData) {
            try {
                bool updateRes = true;
                tomlData = toml::ordered_table();

                updateRes &= config.UpdateTOMLFromStruct(tomlData, config.Hidden);
                updateRes &= config.UpdateTOMLFromStruct(tomlData, config.Advanced);
                updateRes &= config.UpdateTOMLFromStruct(tomlData, config.General);
                updateRes &= config.UpdateTOMLFromStruct(tomlData, config.Gameplay);
                updateRes &= config.UpdateTOMLFromStruct(tomlData, config.Balance);
                updateRes &= config.UpdateTOMLFromStruct(tomlData, config.Audio);
                updateRes &= config.UpdateTOMLFromStruct(tomlData, config.AI);
                updateRes &= config.UpdateTOMLFromStruct(tomlData, config.Camera);
                updateRes &= config.UpdateTOMLFromStruct(tomlData, config.UI);

                if (!updateRes) {
                    logger::error("One or more structs could not be serialized to TOML");
                }
                return updateRes;
            }
            catch (const toml::exception& e) {
                logger::error("TOML Exception during serialization: {}", e.what());
                return false;
            }
            catch (...) {
                logger::error("Unknown exception during struct serialization");
                return false;
            }
        }

        template<typename Config>
        static bool DeserializeAllStructsFromTOML(Config& config, const toml::ordered_value& tomlData) {
            try {
                bool loadRes = true;

                loadRes &= config.LoadStructFromTOML(tomlData, config.Hidden);
                loadRes &= config.LoadStructFromTOML(tomlData, config.Advanced);
                loadRes &= config.LoadStructFromTOML(tomlData, config.General);
                loadRes &= config.LoadStructFromTOML(tomlData, config.Gameplay);
                loadRes &= config.LoadStructFromTOML(tomlData, config.Balance);
                loadRes &= config.LoadStructFromTOML(tomlData, config.Audio);
                loadRes &= config.LoadStructFromTOML(tomlData, config.AI);
                loadRes &= config.LoadStructFromTOML(tomlData, config.Camera);
                loadRes &= config.LoadStructFromTOML(tomlData, config.UI);

                if (!loadRes) {
                    logger::critical("One or more structs could not be deserialized");
                }
                return loadRes;
            }
            catch (const toml::exception& e) {
                logger::error("TOML Exception during deserialization: {}", e.what());
                return false;
            }
            catch (const std::exception& e) {
                logger::error("Exception during deserialization: {}", e.what());
                return false;
            }
            catch (...) {
                logger::error("Unknown exception during struct deserialization");
                return false;
            }
        }

        template<typename Config>
        static void ResetAllStructsToDefaults(Config& config) {
            config.Advanced = SettingsAdvanced_t{};
            config.General = SettingsGeneral_t{};
            config.AI = SettingsAI_t{};
            config.Audio = SettingsAudio_t{};
            config.Balance = SettingsBalance_t{};
            config.Camera = SettingsCamera_t{};
            config.Gameplay = SettingsGameplay_t{};
            config.UI = SettingsUI_t{};
        }
    };
}
