#pragma once

#include "Config/Util/WindowSettingsHolder.hpp"

namespace GTS {

    // Registry key combining type, instance name, and base prefix
    struct WindowRegistryKey {
        std::type_index typeIndex;
        std::string instanceName;
        std::string basePrefix;

        bool operator==(const WindowRegistryKey& a_other) const {
            return typeIndex == a_other.typeIndex &&
                instanceName == a_other.instanceName &&
                basePrefix == a_other.basePrefix;
        }
    };

    // Custom hash function for WindowRegistryKey
    struct WindowRegistryKeyHash {
        std::size_t operator()(const WindowRegistryKey& a_key) const {
            auto h1 = std::hash<std::type_index>{}(a_key.typeIndex);
            auto h2 = std::hash<std::string>{}(a_key.instanceName);
            auto h3 = std::hash<std::string>{}(a_key.basePrefix);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };

    // Settings Registry
    class WindowSettingsRegistry : public CInitSingleton<WindowSettingsRegistry> {

        private:
        std::unordered_map<WindowRegistryKey, std::unique_ptr<IWindowSettingsHolder>, WindowRegistryKeyHash> m_settingsHolders;

        static WindowRegistryKey MakeKey(const std::type_index& a_typeIndex, const std::string& a_instanceName, const std::string& a_basePreffix) {
            return { a_typeIndex, a_instanceName, a_basePreffix };
        }

        public:
        // Register a window type with optional instance name and base prefix
        template<typename WindowType>
        std::shared_ptr<WindowSettingsHolder<WindowType>> RegisterWindow(const WindowSettingsBase_t& a_baseDefaults = WindowSettingsBase_t{}, const std::string& a_instanceName = "", const std::string& a_basePrefix = "UI") {

            auto key = MakeKey(std::type_index(typeid(WindowType)), a_instanceName, a_basePrefix);

            // Check if already registered
            auto it = m_settingsHolders.find(key);
            if (it != m_settingsHolders.end()) {
                // Return existing holder
                auto holderImpl = static_cast<WindowSettingsHolderImpl<WindowType>*>(it->second.get());
                auto holder = static_cast<WindowSettingsHolder<WindowType>*>(holderImpl);
                return std::shared_ptr<WindowSettingsHolder<WindowType>>(holder, [](WindowSettingsHolder<WindowType>*) {});
            }

            // Create new holder with instance name and base prefix
            auto holderImpl = std::make_unique<WindowSettingsHolderImpl<WindowType>>(a_baseDefaults, a_instanceName, a_basePrefix);
            auto holder = static_cast<WindowSettingsHolder<WindowType>*>(holderImpl.get());

            m_settingsHolders[key] = std::move(holderImpl);

            return std::shared_ptr<WindowSettingsHolder<WindowType>>(holder, [](WindowSettingsHolder<WindowType>*) {});
        }

        // Get settings holder for a window type and instance
        template<typename WindowType>
        std::shared_ptr<WindowSettingsHolder<WindowType>> GetWindowSettings(const std::string& a_instanceName, const std::string& a_basePrefix) {
            auto key = MakeKey(std::type_index(typeid(WindowType)), a_instanceName, a_basePrefix);
            auto it = m_settingsHolders.find(key);
            if (it != m_settingsHolders.end()) {
                auto holderImpl = static_cast<WindowSettingsHolderImpl<WindowType>*>(it->second.get());
                auto holder = static_cast<WindowSettingsHolder<WindowType>*>(holderImpl);
                return std::shared_ptr<WindowSettingsHolder<WindowType>>(holder, [](WindowSettingsHolder<WindowType>*) {});
            }
            return nullptr;
        }

        // Get all instances of a specific window type
        template<typename WindowType>
        std::vector<std::shared_ptr<WindowSettingsHolder<WindowType>>> GetAllWindowInstances() {
            std::vector<std::shared_ptr<WindowSettingsHolder<WindowType>>> instances;
            std::type_index targetType(typeid(WindowType));

            for (const auto& [key, holderPtr] : m_settingsHolders) {
                if (key.typeIndex == targetType) {
                    auto holderImpl = static_cast<WindowSettingsHolderImpl<WindowType>*>(holderPtr.get());
                    auto holder = static_cast<WindowSettingsHolder<WindowType>*>(holderImpl);
                    instances.emplace_back(holder, [](WindowSettingsHolder<WindowType>*) {});
                }
            }

            return instances;
        }

        // Unregister a specific instance
        template<typename WindowType>
        bool UnregisterWindow(const std::string& a_instanceName, const std::string& a_basePrefix) {
            auto key = MakeKey(std::type_index(typeid(WindowType)), a_instanceName, a_basePrefix);
            auto it = m_settingsHolders.find(key);
            if (it != m_settingsHolders.end()) {
                m_settingsHolders.erase(it);
                return true;
            }
            return false;
        }

        // Get all registered instance names for a window type
        template<typename WindowType>
        std::vector<std::string> GetInstanceNames(const std::string& a_basePrefix) const {
            std::vector<std::string> instanceNames;
            std::type_index targetType(typeid(WindowType));

            for (const auto& key : m_settingsHolders | views::keys) {
                if (key.typeIndex == targetType && key.basePrefix == a_basePrefix) {
                    instanceNames.push_back(key.instanceName);
                }
            }

            return instanceNames;
        }

        // Get all base prefixes used
        std::vector<std::string> GetAllBasePrefixes() const {
            std::set<std::string> prefixes;
            for (const auto& key : m_settingsHolders | views::keys) {
                prefixes.insert(key.basePrefix);
            }
            return std::vector<std::string>(prefixes.begin(), prefixes.end());
        }

        // Serialize all registered window settings
        bool SerializeAllWindowSettings(toml::ordered_value& a_mainToml) {
            bool success = true;

            for (const auto& holderPtr : m_settingsHolders | views::values) {
                try {
                    if (!holderPtr->UpdateTOMLFromStruct(a_mainToml)) {
                        logger::error("Failed to serialize settings for window: {}", holderPtr->GetWindowTypeName());
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
        bool DeserializeAllWindowSettings(const toml::ordered_value& a_mainToml) {
            bool success = true;

            for (const auto& holderPtr : m_settingsHolders | views::values) {
                try {
                    if (!holderPtr->LoadStructFromTOML(a_mainToml)) {
                        logger::warn("Failed to deserialize settings for window: {}", holderPtr->GetWindowTypeName());
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

        // Reset specific window type instances
        template<typename WindowType>
        void ResetWindowSettings(const std::string& a_instanceName, const std::string& a_basePrefix) {
            auto settings = GetWindowSettings<WindowType>(a_instanceName, a_basePrefix);
            if (settings) {
                settings->ResetToDefaults();
            }
        }

        bool HasWindowSettings() const {
            return !m_settingsHolders.empty();
        }

        size_t GetRegisteredWindowCount() const {
            return m_settingsHolders.size();
        }

        // Remove empty tables from TOML (called after all serialization)
        static void RemoveEmptyTables(toml::ordered_value& a_toml) {
            if (!a_toml.is_table()) return;

            auto& table = a_toml.as_table();
            std::vector<std::string> keysToRemove;

            for (auto& [key, value] : table) {
                if (value.is_table()) {
                    RemoveEmptyTables(value);
                    if (value.as_table().empty()) {
                        keysToRemove.push_back(key);
                    }
                }
            }

            for (const auto& key : keysToRemove) {
                table.erase(key);
            }
        }

    };
}