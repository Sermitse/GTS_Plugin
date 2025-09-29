#pragma once

#include "Config/Settings/SettingsUI.hpp"
#include "Config/Util/WindowSettingsHolder.hpp"
#include "Config/Util/WindowSettingsRegistry.hpp"
#include "UI/ImGui/Lib/imgui.h"

namespace GTS {

    // Interface that defines a window object
    class ImWindow {

        protected:
        std::string m_name = "Default";
        std::string m_title = "Default";

	    private:
	    volatile bool m_show = false;

        public:
        enum class WindowAnchor {
            kTopLeft,
            kTopRight,
            kCenter,
            kBottomLeft,
            kBottomRight,
        };

        enum class DrawLevel {
            kHUD,     //HUDMenu PostDisplay
            kCursor,  //CursorMenu PostDisplay
            kPresent, //DXGISwapChainPresent
        };

        // Internal Flags

        bool m_consumeInput = false;
        bool m_busy = false;
        DrawLevel m_drawLevel = DrawLevel::kHUD;
        WindowAnchor m_anchorPos = WindowAnchor::kCenter;
        ImGuiWindowFlags m_flags = ImGuiWindowFlags_None;

        virtual ~ImWindow() noexcept = default;
        virtual void Draw() = 0;                 //Draw Window Contents Callback
        virtual bool ShouldDraw() = 0;           //Should this window be drawn
        virtual std::string GetWindowName() = 0; //Get window name callback
        virtual bool RequestShow() = 0;          //Request Window Show
        virtual bool RequestHide() = 0;          //Request window Hide
        virtual void Init() = 0;                 //Window Init Callback, Runs on creation.

        virtual void OnWindowShow() = 0;         //Callback When Window is Shown m_show = true
        virtual void OnWindowHide() = 0;         //Callback When Window is Hidden m_show = false Alpha does not affect this

        //Style
        virtual float GetAlphaMult() = 0;        //Get current window alpha callback
        virtual float GetBGAlphaMult() = 0;      //Get current windows background alpha callback

        // Check if this window needs the game's cursor to be visible
    	// kPresent layer uses ImGui's built-in Win32 cursor instead
        virtual bool NeedsGameCursor() const {
            // Only HUD and Cursor layers that consume input need the game cursor
            // kPresent layer windows use ImGui's own cursor system
            if (m_drawLevel == DrawLevel::kPresent) {
                return false;  // Uses ImGui Win32 cursor
            }
            // HUD and Cursor layers need game cursor if they consume input
            return m_consumeInput;
        }

        //Position
        static ImVec2 GetAnchorPos(WindowAnchor a_position, ImVec2 a_padding, bool a_allowCenterY);
        void SetVisible(bool a_state);
        bool IsVisible() const;

    };

    // Enhanced window class with automatic settings registration
    template<typename Derived>
    class ImConfigurableWindow : public ImWindow {

        protected:
        std::shared_ptr<WindowSettingsHolder<Derived>> m_settingsHolder;
        std::string m_instanceName;

        public:
        // Constructor with instance name
        ImConfigurableWindow(const std::string& a_instanceName = "", const std::string& a_basePreffix = "UI") : m_instanceName(a_instanceName) {
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
                return m_name + "." + m_instanceName;
            }
            return m_name;
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