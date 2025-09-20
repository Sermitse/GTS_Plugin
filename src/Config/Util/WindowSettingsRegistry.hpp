#pragma once

#include "Config/Util/WindowSettingsHolder.hpp"

namespace GTS {

    // Global registry for all window settings
    class WindowSettingsRegistry : public CInitSingleton<WindowSettingsRegistry> {
        private:
        std::unordered_map<std::type_index, std::unique_ptr<IWindowSettingsHolder>> m_settingsHolders;

        public:
        // Register a window type (automatically called by ImWindow constructor)
        template<typename WindowType>
        std::shared_ptr<WindowSettingsHolder<WindowType>> RegisterWindow(
            const BaseWindowSettings_t& baseDefaults = BaseWindowSettings_t{}) {

            std::type_index typeIndex(typeid(WindowType));

            // Check if already registered
            auto it = m_settingsHolders.find(typeIndex);
            if (it != m_settingsHolders.end()) {
                // Return existing holder
                auto holderImpl = static_cast<WindowSettingsHolderImpl<WindowType>*>(it->second.get());
                auto holder = static_cast<WindowSettingsHolder<WindowType>*>(holderImpl);
                return std::shared_ptr<WindowSettingsHolder<WindowType>>(holder, [](WindowSettingsHolder<WindowType>*) {});
            }

            // Create new holder
            auto holderImpl = std::make_unique<WindowSettingsHolderImpl<WindowType>>(baseDefaults);
            auto holder = static_cast<WindowSettingsHolder<WindowType>*>(holderImpl.get());

            m_settingsHolders[typeIndex] = std::move(holderImpl);

            return std::shared_ptr<WindowSettingsHolder<WindowType>>(holder, [](WindowSettingsHolder<WindowType>*) {});
        }

        // Get settings holder for a window type
        template<typename WindowType>
        std::shared_ptr<WindowSettingsHolder<WindowType>> GetWindowSettings() {
            std::type_index typeIndex(typeid(WindowType));
            auto it = m_settingsHolders.find(typeIndex);
            if (it != m_settingsHolders.end()) {
                auto holderImpl = static_cast<WindowSettingsHolderImpl<WindowType>*>(it->second.get());
                auto holder = static_cast<WindowSettingsHolder<WindowType>*>(holderImpl);
                return std::shared_ptr<WindowSettingsHolder<WindowType>>(holder, [](WindowSettingsHolder<WindowType>*) {});
            }
            return nullptr;
        }

        // Serialize all registered window settings
        bool SerializeAllWindowSettings(toml::ordered_value& mainToml) {
            bool success = true;

            for (const auto& holderPtr : m_settingsHolders | views::values) {
                try {
                    if (!holderPtr->UpdateTOMLFromStruct(mainToml)) {
                        logger::error("Failed to serialize settings for window type: {}", holderPtr->GetWindowTypeName());
                        success = false;
                    }
                }
                catch (const std::exception& e) {
                    logger::error("Exception serializing window settings for {}: {}", holderPtr->GetWindowTypeName(), e.what());
                    success = false;
                }
            }

            return success;
        }

        // Deserialize all registered window settings
        bool DeserializeAllWindowSettings(const toml::ordered_value& mainToml) {
            bool success = true;

            for (const auto& holderPtr : m_settingsHolders | views::values) {
                try {
                    if (!holderPtr->LoadStructFromTOML(mainToml)) {
                        logger::warn("Failed to deserialize settings for window type: {}", holderPtr->GetWindowTypeName());
                    }
                }
                catch (const std::exception& e) {
                    logger::error("Exception deserializing window settings for {}: {}", holderPtr->GetWindowTypeName(), e.what());
                    success = false;
                }
            }

            return success;
        }

        // Reset all window settings
        void ResetAllWindowSettings() {
            for (const auto& holderPtr : m_settingsHolders | views::values) {
                try {
                    holderPtr->ResetToDefaults();
                }
                catch (const std::exception& e) {
                    logger::error("Exception resetting window settings for {}: {}", holderPtr->GetWindowTypeName(), e.what());
                }
            }
        }

        bool HasWindowSettings() const {
            return !m_settingsHolders.empty();
        }
    };
}