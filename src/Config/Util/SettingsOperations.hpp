#pragma once

#include "Config/Settings/SettingsAdvanced.hpp"
#include "Config/Settings/SettingsAI.hpp"
#include "Config/Settings/SettingsAudio.hpp"
#include "Config/Settings/SettingsBalance.hpp"
#include "Config/Settings/SettingsCamera.hpp"
#include "Config/Settings/SettingsCollision.hpp"
#include "Config/Settings/SettingsGameplay.hpp"
#include "Config/Settings/SettingsGeneral.hpp"
#include "Config/Settings/SettingsUI.hpp"

#include "Config/Util/WindowSettingsRegistry.hpp"

namespace GTS {

    class SettingsOperations : SettingsHandler {

        public:
        template<typename Config>
        static bool SerializeAllStructsToTOML(Config& a_conf, toml::ordered_value& a_toml) {
            try {
                bool updateRes = true;
                a_toml = toml::ordered_table();

                // Serialize static settings
                updateRes &= a_conf.UpdateTOMLFromStruct(a_toml, a_conf.Hidden,    std::string(toml::refl::GetFriendlyName(a_conf.Hidden)));
                updateRes &= a_conf.UpdateTOMLFromStruct(a_toml, a_conf.Advanced,  std::string(toml::refl::GetFriendlyName(a_conf.Advanced)));
                updateRes &= a_conf.UpdateTOMLFromStruct(a_toml, a_conf.General,   std::string(toml::refl::GetFriendlyName(a_conf.General)));
                updateRes &= a_conf.UpdateTOMLFromStruct(a_toml, a_conf.Gameplay,  std::string(toml::refl::GetFriendlyName(a_conf.Gameplay)));
                updateRes &= a_conf.UpdateTOMLFromStruct(a_toml, a_conf.Balance,   std::string(toml::refl::GetFriendlyName(a_conf.Balance)));
                updateRes &= a_conf.UpdateTOMLFromStruct(a_toml, a_conf.Audio,     std::string(toml::refl::GetFriendlyName(a_conf.Audio)));
                updateRes &= a_conf.UpdateTOMLFromStruct(a_toml, a_conf.AI,        std::string(toml::refl::GetFriendlyName(a_conf.AI)));
                updateRes &= a_conf.UpdateTOMLFromStruct(a_toml, a_conf.Camera,    std::string(toml::refl::GetFriendlyName(a_conf.Camera)));
                updateRes &= a_conf.UpdateTOMLFromStruct(a_toml, a_conf.UI,        std::string(toml::refl::GetFriendlyName(a_conf.UI)));
                updateRes &= a_conf.UpdateTOMLFromStruct(a_toml, a_conf.Collision, std::string(toml::refl::GetFriendlyName(a_conf.Collision)));

                // Serialize window settings
                if (WindowSettingsRegistry::GetSingleton().HasWindowSettings()) {
                    bool windowRes = WindowSettingsRegistry::GetSingleton().SerializeAllWindowSettings(a_toml);
                    updateRes &= windowRes;
                    if (!windowRes) {
                        logger::error("One or more window settings could not be serialized to TOML");
                    }

                    // Clean up empty intermediate tables
                    WindowSettingsRegistry::RemoveEmptyTables(a_toml);
                }

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
        static bool DeserializeAllStructsFromTOML(Config& a_conf, const toml::ordered_value& a_toml) {
            try {
                bool loadRes = true;

                // Deserialize static settings
                loadRes &= a_conf.LoadStructFromTOML(a_toml, a_conf.Hidden,     std::string(toml::refl::GetFriendlyName(a_conf.Hidden)));
                loadRes &= a_conf.LoadStructFromTOML(a_toml, a_conf.Advanced,   std::string(toml::refl::GetFriendlyName(a_conf.Advanced)));
                loadRes &= a_conf.LoadStructFromTOML(a_toml, a_conf.General,    std::string(toml::refl::GetFriendlyName(a_conf.General)));
                loadRes &= a_conf.LoadStructFromTOML(a_toml, a_conf.Gameplay,   std::string(toml::refl::GetFriendlyName(a_conf.Gameplay)));
                loadRes &= a_conf.LoadStructFromTOML(a_toml, a_conf.Balance,    std::string(toml::refl::GetFriendlyName(a_conf.Balance)));
                loadRes &= a_conf.LoadStructFromTOML(a_toml, a_conf.Audio,      std::string(toml::refl::GetFriendlyName(a_conf.Audio)));
                loadRes &= a_conf.LoadStructFromTOML(a_toml, a_conf.AI,         std::string(toml::refl::GetFriendlyName(a_conf.AI)));
                loadRes &= a_conf.LoadStructFromTOML(a_toml, a_conf.Camera,     std::string(toml::refl::GetFriendlyName(a_conf.Camera)));
                loadRes &= a_conf.LoadStructFromTOML(a_toml, a_conf.UI,         std::string(toml::refl::GetFriendlyName(a_conf.UI)));
                loadRes &= a_conf.LoadStructFromTOML(a_toml, a_conf.Collision,  std::string(toml::refl::GetFriendlyName(a_conf.Collision)));

                // Deserialize window settings
                if (WindowSettingsRegistry::GetSingleton().HasWindowSettings()) {
                    bool windowRes = WindowSettingsRegistry::GetSingleton().DeserializeAllWindowSettings(a_toml);
                    if (!windowRes) {
                        logger::warn("Some window settings could not be deserialized");
                    }
                }

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
        static void ResetAllStructsToDefaults(Config& a_conf) {
            // Reset static settings
            a_conf.Advanced  = SettingsAdvanced_t{};
            a_conf.General   = SettingsGeneral_t{};
            a_conf.AI        = SettingsAI_t{};
            a_conf.Audio     = SettingsAudio_t{};
            a_conf.Balance   = SettingsBalance_t{};
            a_conf.Camera    = SettingsCamera_t{};
            a_conf.Gameplay  = SettingsGameplay_t{};
            a_conf.UI        = SettingsUI_t{};
            a_conf.Collision = SettingsCollision_t{};

            // Reset window settings
            WindowSettingsRegistry::GetSingleton().ResetAllWindowSettings();
        }
    };
}
