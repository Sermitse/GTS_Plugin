#pragma once

#include "Config/Settings/SettingsUI.hpp"
#include "Config/Util/WindowSettingsHolder.hpp"
#include "Config/Util/WindowSettingsRegistry.hpp"

#include "UI/ImGui/Lib/imgui.h"

namespace GTS {

    class ImWindow {

        protected:
        std::string m_name = "Default";
        std::string m_title = "Default";

		private:
        struct FadeSettings {
            bool enabled = false;
            float visibilityDuration = 5.0f;
            float fadeDuration = 1.0f;
            float fadeAlpha = 1.0f;
            float visibilityTimer = 0.0f;
            bool isFading = false;
        };

        public:
        enum class WindowAnchor {
            kTopLeft,
            kTopRight,
            kCenter,
            kBottomLeft,
            kBottomRight,
        };

        enum WindowType {
            kWidget,
            kSettings,
            kDebug,
        };

        WindowType m_windowType = WindowType::kWidget;
        WindowAnchor m_anchorPos = WindowAnchor::kCenter;
        ImGuiWindowFlags m_flags = ImGuiWindowFlags_None;
        FadeSettings m_fadeSettings = {};

        virtual ~ImWindow() noexcept = default;

        virtual void Draw() = 0;
        virtual bool WantsToDraw() = 0;
        virtual std::string GetWindowName() = 0;
        virtual void Init() = 0;

        virtual float GetFullAlpha() = 0;
        virtual float GetBackgroundAlpha() = 0;

        float GetFadingAlpha() const;
        void ResetFadeState();
        void UpdateFade(float deltaTime);
        bool IsFadeComplete() const;

        static ImVec2 GetAnchorPos(WindowAnchor a_position, ImVec2 a_padding, bool a_allowCenterY);
    };

    template<typename Derived>
    class ImConfigurableWindow : public ImWindow {

        protected:
        std::shared_ptr<WindowSettingsHolder<Derived>> m_settingsHolder;
        std::string m_instanceName;

        public:
        ImConfigurableWindow(const std::string& a_instanceName = "", const std::string& a_basePreffix = "UI") : m_instanceName(a_instanceName) {
            m_settingsHolder = WindowSettingsRegistry::GetSingleton().RegisterWindow<Derived>(BaseWindowSettings_t{}, a_instanceName, a_basePreffix);
        }

        std::string GetInstanceName() const {
            return m_instanceName;
        }

        void SetInstanceName(const std::string& instanceName) {
            m_instanceName = instanceName;
            m_settingsHolder->SetInstanceName(instanceName);
        }

        virtual std::string GetWindowName() override {
            if (!m_instanceName.empty()) {
                return m_name + "." + m_instanceName;
            }
            return m_name;
        }

        BaseWindowSettings_t& GetBaseSettings() {
            return m_settingsHolder->GetBaseSettings();
        }

        const BaseWindowSettings_t& GetBaseSettings() const {
            return m_settingsHolder->GetBaseSettings();
        }

        template<typename CustomStruct>
        void RegisterExtraSettings(const CustomStruct& defaults = CustomStruct{}) {
            m_settingsHolder->template RegisterCustomSettings<CustomStruct>(defaults);
        }

        template<typename CustomStruct>
        CustomStruct& GetExtraSettings() {
            return m_settingsHolder->template GetCustomSettings<CustomStruct>();
        }

        template<typename CustomStruct>
        const CustomStruct& GetExtraSettings() const {
            return m_settingsHolder->template GetCustomSettings<CustomStruct>();
        }

        float GetFullAlpha() override {
			return GetBaseSettings().fAlpha;
        };

        float GetBackgroundAlpha() override {
			return GetBaseSettings().fBGAlphaMult;
        }
    };
}