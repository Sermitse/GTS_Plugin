#pragma once
#include "Config/Util/SettingsHandler.hpp"

namespace GTS {

    // Type-erased interface for custom settings
    class IDynamicSettings {
        public:
        virtual ~IDynamicSettings() = default;
        virtual bool UpdateTOMLFromStruct(toml::ordered_value& a_toml) = 0;
        virtual bool LoadStructFromTOML(const toml::ordered_value& a_toml) = 0;
        virtual void ResetToDefaults() = 0;
        virtual std::string GetSectionName() const = 0;
    };

    // Template wrapper for custom settings structs
    template<typename DynamicStruct>
    class DynamicSettingsWrapper final : public IDynamicSettings, SettingsHandler {

        private:
        DynamicStruct m_dynamicSettings;
        DynamicStruct m_defaults;

        public:
        explicit DynamicSettingsWrapper() = default;
        explicit DynamicSettingsWrapper(const DynamicStruct& a_defaults) : m_dynamicSettings(a_defaults), m_defaults(a_defaults) {}

        void SetDefaults(const DynamicStruct& a_defaults) {
            m_defaults = a_defaults;
        }

        void ResetToDefaults() override {
            m_dynamicSettings = m_defaults;
        }

        DynamicStruct& GetCustomSettings() {
            return m_dynamicSettings;
        }

        const DynamicStruct& GetCustomSettings() const {
            return m_dynamicSettings;
        }

        bool UpdateTOMLFromStruct(toml::ordered_value& a_toml) override {
            std::string sectionName = GetSectionName();
            return SettingsHandler::UpdateTOMLFromStruct(a_toml, m_dynamicSettings, sectionName);
        }

        bool LoadStructFromTOML(const toml::ordered_value& a_toml) override {
            std::string sectionName = GetSectionName();
            return SettingsHandler::LoadStructFromTOML(a_toml, m_dynamicSettings, sectionName);
        }

        std::string GetSectionName() const override {
            return std::string(toml::refl::GetFriendlyName(m_dynamicSettings));
        }
    };
}