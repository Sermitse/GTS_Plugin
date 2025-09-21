#pragma once

#include "Config/Settings/SettingsUI.hpp"
#include "Config/Util/WindowSettingsHolder.hpp"
#include "Config/Util/WindowSettingsRegistry.hpp"
#include "UI/ImGui/Lib/imgui.h"

namespace GTS {

    // Non-template interface for window management
    class IImWindow {

        public:

        enum class WindowAnchor {
            kTopLeft,
            kTopRight,
            kCenter,
            kBottomLeft,
            kBottomRight,
        };

        enum class WindowMessage {
            kUpdateStyle,
        };

        // Internal Flags
        bool Show = false;
        bool ConsumeInput = false;
        bool Busy = false;
        WindowAnchor AnchorPos = WindowAnchor::kCenter;
        ImGuiWindowFlags flags = ImGuiWindowFlags_None;

		protected:
        std::string Name = "Default";
        std::string Title = "Default";

		public:
        virtual ~IImWindow() noexcept = default;
        virtual void Draw() = 0;
        virtual bool ShouldDraw() = 0;
        virtual float GetAlphaMult() = 0;
        virtual float GetBGAlphaMult() = 0;
        virtual std::string GetWindowName() = 0;
        static ImVec2 GetAnchorPos(WindowAnchor a_position, ImVec2 a_padding, bool a_allowCenterY);
    };

    // Enhanced base window class with automatic settings registration
    template<typename Derived>
    class ImWindowBase : public IImWindow {
        protected:
        std::shared_ptr<WindowSettingsHolder<Derived>> m_settingsHolder;
        std::string m_instanceName;

        public:
        // Constructor with instance name
        ImWindowBase(const std::string& a_instanceName = "", const std::string& a_basePreffix = "UI") : m_instanceName(a_instanceName) {
            m_settingsHolder = WindowSettingsRegistry::GetSingleton().RegisterWindow<Derived>(BaseWindowSettings_t{}, a_instanceName, a_basePreffix);
        }

        // Get instance name for this window
        std::string GetInstanceName() const {
            return m_instanceName;
        }

        // Set instance name (useful for dynamic assignment)
        void SetInstanceName(const std::string& instanceName) {
            m_instanceName = instanceName;
            m_settingsHolder->SetInstanceName(instanceName);
        }

        // Set instance name (useful for dynamic assignment)
        virtual std::string GetWindowName() override {
            if (!m_instanceName.empty()) {
                return Name + "." + m_instanceName;
            }
            return Name;
        }

        // Access to settings
        BaseWindowSettings_t& GetBaseSettings() {
            return m_settingsHolder->GetBaseSettings();
        }

        const BaseWindowSettings_t& GetBaseSettings() const {
            return m_settingsHolder->GetBaseSettings();
        }

        template<typename CustomStruct>
        void RegisterCustomSettings(const CustomStruct& defaults = CustomStruct{}) {
            m_settingsHolder->template RegisterCustomSettings<CustomStruct>(defaults);
        }

        template<typename CustomStruct>
        CustomStruct& GetCustomSettings() {
            return m_settingsHolder->template GetCustomSettings<CustomStruct>();
        }

        template<typename CustomStruct>
        const CustomStruct& GetCustomSettings() const {
            return m_settingsHolder->template GetCustomSettings<CustomStruct>();
        }
    };

}