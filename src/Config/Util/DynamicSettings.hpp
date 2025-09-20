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
    class DynamicSettingsWrapper final : public IDynamicSettings {
        private:
        DynamicStruct m_dynamicSettings;
        SettingsHandler m_handler;

        public:
        explicit DynamicSettingsWrapper() = default;
        explicit DynamicSettingsWrapper(const DynamicStruct& defaults) : m_dynamicSettings(defaults) {}

        DynamicStruct& GetCustomSettings() {
            return m_dynamicSettings;
        }

        const DynamicStruct& GetCustomSettings() const {
            return m_dynamicSettings;
        }

        bool UpdateTOMLFromStruct(toml::ordered_value& toml) override {
            return m_handler.UpdateTOMLFromStruct(toml, m_dynamicSettings);
        }

        bool LoadStructFromTOML(const toml::ordered_value& toml) override {
            return m_handler.LoadStructFromTOML(toml, m_dynamicSettings);
        }

        void ResetToDefaults() override {
            m_dynamicSettings = DynamicStruct{};
        }

        std::string GetSectionName() const override {
            return std::string(toml::refl::GetFriendlyName(m_dynamicSettings));
        }
    };
}