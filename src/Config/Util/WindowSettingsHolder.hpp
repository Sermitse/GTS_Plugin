#pragma once

#include "Config/Util/SettingsHandler.hpp"
#include "Config/Util/DynamicSettings.hpp"

namespace GTS {

    // Interface for type-erased settings holder
    class IWindowSettingsHolder {
        public:
        virtual ~IWindowSettingsHolder() = default;
        virtual bool UpdateTOMLFromStruct(toml::ordered_value& toml) = 0;
        virtual bool LoadStructFromTOML(const toml::ordered_value& toml) = 0;
        virtual void ResetToDefaults() = 0;
        virtual std::string GetWindowTypeName() const = 0;
    };

    // Combined settings holder for a window
    template<typename WindowType>
    class WindowSettingsHolder : SettingsHandler {

        private:
        BaseWindowSettings_t m_baseSettings;
        std::unique_ptr<IDynamicSettings> m_customSettings;
        std::string m_windowTypeName;
        std::string m_instanceName;  // Meaningful instance name instead of ID
        std::string m_basePrefix;    // e.g., "UI" for UI windows

        //Name of the per class definable extra settings struct
        static constexpr const char* const m_extraSectionName = ".Extra";

        public:
        explicit WindowSettingsHolder(const std::string& a_instanceName, const std::string& a_basePreffix) : m_instanceName(a_instanceName), m_basePrefix(a_basePreffix) {
        	m_windowTypeName = typeid(WindowType).name();

        	// Clean up the type name for readability
            size_t pos = m_windowTypeName.find_last_of(':');
            if (pos != std::string::npos) {
                m_windowTypeName = m_windowTypeName.substr(pos + 1);
            }
        }

        explicit WindowSettingsHolder(const BaseWindowSettings_t& a_baseDefaults, const std::string& a_instanceName, const std::string& a_basePreffix) : m_baseSettings(a_baseDefaults), m_instanceName(a_instanceName), m_basePrefix(a_basePreffix) {
            m_windowTypeName = typeid(WindowType).name();
            size_t pos = m_windowTypeName.find_last_of(':');
            if (pos != std::string::npos) {
                m_windowTypeName = m_windowTypeName.substr(pos + 1);
            }
        }

        // Get the full table name in hierarchical format (e.g., "UI.SettingsWindow" or "UI.InventoryWindow.Player")
        std::string GetBaseTableName() const {
            std::string tableName = m_basePrefix + "." + m_windowTypeName;
            if (!m_instanceName.empty()) {
                tableName += "." + m_instanceName;
            }
            return tableName;
        }

        std::string GetInstanceName() const {
            return m_instanceName;
        }

        void SetInstanceName(const std::string& a_instanceName) {
            m_instanceName = a_instanceName;
        }

        std::string GetBasePrefix() const {
            return m_basePrefix;
        }

        void SetBasePrefix(const std::string& a_basePreffix) {
            m_basePrefix = a_basePreffix;
        }

        // Register custom settings struct
        template<typename CustomStruct>
        void RegisterCustomSettings(const CustomStruct& a_defaults = CustomStruct{}) {
            static_assert(std::is_default_constructible_v<CustomStruct>, "Custom settings struct must be default constructible");
            m_customSettings = std::make_unique<DynamicSettingsWrapper<CustomStruct>>(a_defaults);
        }

        // Access base settings
        BaseWindowSettings_t& GetBaseSettings() {
            return m_baseSettings;
        }
        const BaseWindowSettings_t& GetBaseSettings() const {
            return m_baseSettings;
        }

        // Access custom settings
        template<typename CustomStruct>
        CustomStruct& GetCustomSettings() {
            auto wrapper = dynamic_cast<DynamicSettingsWrapper<CustomStruct>*>(m_customSettings.get());
            if (!wrapper) {
                throw std::runtime_error("Custom settings struct type mismatch or not registered");
            }
            return wrapper->GetCustomSettings();
        }

        template<typename CustomStruct>
        const CustomStruct& GetCustomSettings() const {
            auto wrapper = dynamic_cast<const DynamicSettingsWrapper<CustomStruct>*>(m_customSettings.get());
            if (!wrapper) {
                throw std::runtime_error("Custom settings struct type mismatch or not registered");
            }
            return wrapper->GetCustomSettings();
        }

        // Check if custom settings are registered
        bool HasCustomSettings() const {
            return m_customSettings != nullptr;
        }

        // Helper function to create nested tables from dot-separated path
        static void CreateNestedTable(toml::ordered_value& a_toml, const std::string& a_tablePath, const toml::ordered_value& a_data) {
            // Ensure the root is a table
            if (!a_toml.is_table()) {
                a_toml = toml::ordered_table{};
            }

            std::vector<std::string> parts;
            std::stringstream ss(a_tablePath);
            std::string part;

            logger::info("Constructing Nested Table: {}", a_tablePath);

            // Split the path by dots
            while (std::getline(ss, part, '.')) {
                logger::trace("Part {}", part);
                parts.push_back(part);
            }

            // Navigate/create the nested structure
            toml::ordered_value* current = &a_toml;
            for (size_t i = 0; i < parts.size() - 1; ++i) {
                if (!current->contains(parts[i])) {
                    current->as_table()[parts[i]] = toml::ordered_table{};
                }
                current = &current->as_table()[parts[i]];
            }

            // Set the final value
            current->as_table()[parts.back()] = a_data;
        }

        // Helper function to get nested table from dot-separated path
        static bool GetNestedTable(const toml::ordered_value& a_toml, const std::string& a_tablePath, toml::ordered_value& a_result) {
            std::vector<std::string> parts;
            std::stringstream ss(a_tablePath);
            std::string part;

            // Split the path by dots
            while (std::getline(ss, part, '.')) {
                logger::trace("Part {}", part);
                parts.push_back(part);
            }

            logger::info("Deconstructing Nested Table: {}", a_tablePath);

            // Navigate the nested structure
            const toml::ordered_value* current = &a_toml;
            for (const auto& pathPart : parts) {
                if (!current->contains(pathPart)) {
                    return false;
                }
                current = &current->at(pathPart);
            }

            a_result = *current;
            return true;
        }

        // Serialization with proper nested table structure
        virtual bool UpdateTOMLFromStruct(toml::ordered_value& toml) {
            std::string baseTableName = GetBaseTableName();

            try {
                // Serialize base settings directly to a temporary section name
                const std::string tempSectionName = "TempTable";
                if (!SettingsHandler::UpdateTOMLFromStruct(toml, m_baseSettings, tempSectionName)) {
                    return false;
                }

                // Move the data to the correct nested location
                if (toml.contains(tempSectionName)) {
                    CreateNestedTable(toml, baseTableName, toml.at(tempSectionName));
                    //Remove the temporary section
                    toml.as_table().erase(tempSectionName);
                }
            }
            catch (const std::exception& e) {
                logger::error("Failed to serialize base settings for {}: {}", baseTableName, e.what());
                return false;
            }

            // Serialize custom settings if they exist
            if (m_customSettings) {
                try {
                    std::string customTableName = baseTableName + m_extraSectionName;

                    toml::ordered_value tempToml = toml::ordered_table{};
                    if (!m_customSettings->UpdateTOMLFromStruct(tempToml)) {
                        logger::error("Failed to serialize custom settings for {}", baseTableName);
                        return false;
                    }

                    // Get the custom settings section and create nested structure
                    std::string originalSectionName = m_customSettings->GetSectionName();
                    if (tempToml.contains(originalSectionName)) {
                        CreateNestedTable(toml, customTableName, tempToml.at(originalSectionName));
                    }
                }
                catch (const std::exception& e) {
                    logger::error("Exception serializing custom settings for {}: {}", baseTableName, e.what());
                    return false;
                }
            }

            return true;
        }

        virtual bool LoadStructFromTOML(const toml::ordered_value& toml) {
            bool result = true;
            std::string baseTableName = GetBaseTableName();

            // Load base settings using nested table lookup
            try {
                toml::ordered_value baseData;
                if (GetNestedTable(toml, baseTableName, baseData)) {
                    // Create temporary TOML with expected structure
                    toml::ordered_value tempToml{ toml::table{} };
                    tempToml.as_table()["TempTable"] = baseData;
                    m_baseSettings = toml::get<BaseWindowSettings_t>(tempToml.at("temp"));
                }
            }
            catch (const std::exception& e) {
                logger::warn("Failed to load base settings for {}: {}", baseTableName, e.what());
            }

            // Load custom settings if they exist
            if (m_customSettings) {
                try {
                    std::string customTableName = baseTableName + m_extraSectionName;
                    toml::ordered_value customData;

                    if (GetNestedTable(toml, customTableName, customData)) {
                        // Create a temporary TOML with the expected section name
                        toml::ordered_value tempToml{ toml::table{} };
                        std::string originalSectionName = m_customSettings->GetSectionName();
                        tempToml.as_table()[originalSectionName] = customData;

                        if (!m_customSettings->LoadStructFromTOML(tempToml)) {
                            logger::warn("Failed to load custom settings for {}", customTableName);
                        }
                    }
                }
                catch (const std::exception& e) {
                    logger::warn("Exception loading custom settings for {}: {}", baseTableName, e.what());
                }
            }

            return result;
        }

        virtual void ResetToDefaults() {
            m_baseSettings = BaseWindowSettings_t{};
            if (m_customSettings) {
                m_customSettings->ResetToDefaults();
            }
        }

        virtual std::string GetWindowTypeName() const {
            return GetBaseTableName();
        }
    };

    // Template specialization that implements the interface
    template<typename WindowType>
    class WindowSettingsHolderImpl final : public IWindowSettingsHolder, public WindowSettingsHolder<WindowType> {

    	public:
        explicit WindowSettingsHolderImpl(const std::string& a_instanceName, const std::string& a_basePreffix) : WindowSettingsHolder<WindowType>(a_instanceName, a_basePreffix) {}
        explicit WindowSettingsHolderImpl(const BaseWindowSettings_t& a_baseDefaults, const std::string& a_instanceName, const std::string& a_basePreffix) : WindowSettingsHolder<WindowType>(a_baseDefaults, a_instanceName, a_basePreffix) {}

        bool UpdateTOMLFromStruct(toml::ordered_value& a_toml) override {
            return WindowSettingsHolder<WindowType>::UpdateTOMLFromStruct(a_toml);
        }

        bool LoadStructFromTOML(const toml::ordered_value& a_toml) override {
            return WindowSettingsHolder<WindowType>::LoadStructFromTOML(a_toml);
        }

        void ResetToDefaults() override {
            WindowSettingsHolder<WindowType>::ResetToDefaults();
        }

        std::string GetWindowTypeName() const override {
            return WindowSettingsHolder<WindowType>::GetWindowTypeName();
        }
    };

}