#pragma once

#include "Config/Util/SettingsHandler.hpp"
#include "Config/Util/DynamicSettings.hpp"

namespace GTS {

    // Base class for type-erased settings holder
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

        public:
        explicit WindowSettingsHolder() {
            m_windowTypeName = typeid(WindowType).name();
            // Clean up the type name for readability
            size_t pos = m_windowTypeName.find_last_of(':');
            if (pos != std::string::npos) {
                m_windowTypeName = m_windowTypeName.substr(pos + 1);
            }
        }

        // Initialize with base defaults
        explicit WindowSettingsHolder(const BaseWindowSettings_t& baseDefaults)
            : m_baseSettings(baseDefaults) {
            m_windowTypeName = typeid(WindowType).name();
            size_t pos = m_windowTypeName.find_last_of(':');
            if (pos != std::string::npos) {
                m_windowTypeName = m_windowTypeName.substr(pos + 1);
            }
        }

        // Register custom settings struct
        template<typename CustomStruct>
        void RegisterCustomSettings(const CustomStruct& defaults = CustomStruct{}) {
            static_assert(std::is_default_constructible_v<CustomStruct>,
                "Custom settings struct must be default constructible");
            m_customSettings = std::make_unique<DynamicSettingsWrapper<CustomStruct>>(defaults);
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

        // Serialization
        virtual bool UpdateTOMLFromStruct(toml::ordered_value& toml) {

            // Serialize base settings with window type name
            std::string baseSectionName = m_windowTypeName + "_Base";

            // Temporarily change the registered name for base settings
            auto originalName = toml::refl::GetFriendlyName(m_baseSettings);

            // Serialize base settings using the existing handler
            try {
                if (!SettingsHandler::UpdateTOMLFromStruct(toml, m_baseSettings, baseSectionName)) {
                    return false;
                }
            }
            catch (const std::exception& e) {
                logger::error("Failed to serialize base settings for {}: {}", m_windowTypeName, e.what());
                return false;
            }

            // Serialize custom settings if they exist
            if (m_customSettings) {
                try {
                    if (!m_customSettings->UpdateTOMLFromStruct(toml)) {
                        logger::error("Failed to serialize custom settings for {}", m_windowTypeName);
                        return false;
                    }
                }
                catch (const std::exception& e) {
                    logger::error("Exception serializing custom settings for {}: {}", m_windowTypeName, e.what());
                    return false;
                }
            }

            return true;
        }


        virtual bool LoadStructFromTOML(const toml::ordered_value& toml) {
            bool result = true;

            // Load base settings
            std::string baseSectionName = m_windowTypeName + "_Base";
            try {
                if (toml.contains(baseSectionName)) {
                    m_baseSettings = toml::get<BaseWindowSettings_t>(toml.at(baseSectionName));
                }
            }
            catch (const std::exception& e) {
                logger::warn("Failed to load base settings for {}: {}", m_windowTypeName, e.what());
                // Don't mark as failure - use defaults
            }

            // Load custom settings if they exist
            if (m_customSettings) {
                try {
                    if (!m_customSettings->LoadStructFromTOML(toml)) {
                        logger::warn("Failed to load custom settings for {}", m_windowTypeName);
                        // Don't mark as failure - use defaults
                    }
                }
                catch (const std::exception& e) {
                    logger::warn("Exception loading custom settings for {}: {}", m_windowTypeName, e.what());
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
            return m_windowTypeName;
        }
    };

    // Template specialization that implements the interface
    template<typename WindowType>
    class WindowSettingsHolderImpl final : public IWindowSettingsHolder, public WindowSettingsHolder<WindowType> {
        public:
        explicit WindowSettingsHolderImpl() : WindowSettingsHolder<WindowType>() {}
        explicit WindowSettingsHolderImpl(const BaseWindowSettings_t& baseDefaults) : WindowSettingsHolder<WindowType>(baseDefaults) {}

        bool UpdateTOMLFromStruct(toml::ordered_value& toml) override {
            return WindowSettingsHolder<WindowType>::UpdateTOMLFromStruct(toml);
        }

        bool LoadStructFromTOML(const toml::ordered_value& toml) override {
            return WindowSettingsHolder<WindowType>::LoadStructFromTOML(toml);
        }

        void ResetToDefaults() override {
            WindowSettingsHolder<WindowType>::ResetToDefaults();
        }

        std::string GetWindowTypeName() const override {
            return WindowSettingsHolder<WindowType>::GetWindowTypeName();
        }
    };

}