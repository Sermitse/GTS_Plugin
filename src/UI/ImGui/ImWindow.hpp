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
        std::string Name = "Default";
        std::string Title = "Default";
        ImGuiWindowFlags flags = ImGuiWindowFlags_None;

        virtual ~IImWindow() noexcept = default;
        virtual void Draw() = 0;
        virtual bool ShouldDraw() = 0;
        virtual float GetAlphaMult() = 0;
        virtual float GetBGAlphaMult() = 0;
        static ImVec2 GetAnchorPos(WindowAnchor a_position, ImVec2 a_padding, bool a_allowCenterY);
    };

    // Enhanced base window class with automatic settings registration
    template<typename Derived>
    class ImWindowBase : public IImWindow {
        protected:
        std::shared_ptr<WindowSettingsHolder<Derived>> m_settingsHolder;

        public:
        ImWindowBase() {
            // Automatically register this window type
            m_settingsHolder = WindowSettingsRegistry::GetSingleton().RegisterWindow<Derived>();
        }

        explicit ImWindowBase(const BaseWindowSettings_t& baseDefaults) {
            // Automatically register with custom base defaults
            m_settingsHolder = WindowSettingsRegistry::GetSingleton().RegisterWindow<Derived>(baseDefaults);
        }

        // Access to settings
        BaseWindowSettings_t& GetBaseSettings() {
            return m_settingsHolder->GetBaseSettings();
        }

        const BaseWindowSettings_t& GetBaseSettings() const {
            return m_settingsHolder->GetBaseSettings();
        }

        // Register custom settings struct for this window type
        template<typename CustomStruct>
        void RegisterCustomSettings(const CustomStruct& defaults = CustomStruct{}) {
            m_settingsHolder->template RegisterCustomSettings<CustomStruct>(defaults);
        }

        // Access custom settings
        template<typename CustomStruct>
        CustomStruct& GetCustomSettings() {
            return m_settingsHolder->template GetCustomSettings<CustomStruct>();
        }

        template<typename CustomStruct>
        const CustomStruct& GetCustomSettings() const {
            return m_settingsHolder->template GetCustomSettings<CustomStruct>();
        }
    };

    //// For backwards compatibility
    //class ImWindow : public ImWindowBase<ImWindow> {
    //    public:
    //    using ImWindowBase::ImWindowBase;
    //};
}