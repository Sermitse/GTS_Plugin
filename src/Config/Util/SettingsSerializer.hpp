#pragma once
#include "Config/Util/TomlHandler.hpp"

namespace GTS {

	class SettingsSerializer : public TOMLHandler {

        public:
        template<typename Config>
        static bool SerializeToTOML(Config& config, toml::ordered_table& tomlData) {
            try {
                bool updateRes = true;
                tomlData = toml::ordered_table();

                updateRes &= UpdateTOMLFromStruct(tomlData, config.Hidden);
                updateRes &= UpdateTOMLFromStruct(tomlData, config.Advanced);
                updateRes &= UpdateTOMLFromStruct(tomlData, config.General);
                updateRes &= UpdateTOMLFromStruct(tomlData, config.Gameplay);
                updateRes &= UpdateTOMLFromStruct(tomlData, config.Balance);
                updateRes &= UpdateTOMLFromStruct(tomlData, config.Audio);
                updateRes &= UpdateTOMLFromStruct(tomlData, config.AI);
                updateRes &= UpdateTOMLFromStruct(tomlData, config.Camera);
                updateRes &= UpdateTOMLFromStruct(tomlData, config.UI);

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
        bool DeserializeFromTOML(Config& config, const toml::ordered_table& tomlData) {
            try {
                bool loadRes = true;

                loadRes &= LoadStructFromTOML(tomlData, config.Hidden);
                loadRes &= LoadStructFromTOML(tomlData, config.Advanced);
                loadRes &= LoadStructFromTOML(tomlData, config.General);
                loadRes &= LoadStructFromTOML(tomlData, config.Gameplay);
                loadRes &= LoadStructFromTOML(tomlData, config.Balance);
                loadRes &= LoadStructFromTOML(tomlData, config.Audio);
                loadRes &= LoadStructFromTOML(tomlData, config.AI);
                loadRes &= LoadStructFromTOML(tomlData, config.Camera);
                loadRes &= LoadStructFromTOML(tomlData, config.UI);

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
        void ResetToDefaults(Config& config) const {
            config.Advanced = SettingsAdvanced_t{};
            config.General = SettingsGeneral_t{};
            config.AI = SettingsAI_t{};
            config.Audio = SettingsAudio_t{};
            config.Balance = SettingsBalance_t{};
            config.Camera = SettingsCamera_t{};
            config.Gameplay = SettingsGameplay_t{};
            config.UI = SettingsUI_t{};
            //config.Hidden = SettingsHidden_t{};
        }
    };
}
