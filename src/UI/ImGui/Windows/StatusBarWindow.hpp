#pragma once

#include "UI/ImGui/Core/ImWindow.hpp"
#include "UI/ImGui/Core/ImCategoryContainer.hpp"
#include "Managers/Input/InputManager.hpp"

namespace GTS {

    class StatusBarWindow final : public ImConfigurableWindow<StatusBarWindow> {

		private:
        void Draw() override;
        void Init() override;
        void RequestClose() override;
        bool WantsToDraw() override;
        virtual float GetBackgroundAlpha() override;

        std::unique_ptr<ImGuiEx::StatusBar> m_buffs;
        WindowSettingsStatusBar_t m_extraSettings = {};

        float m_prevCombiValue = 0.0f;
        const bool* m_isConfiguring = nullptr;
        const bool* m_settingsVisible = nullptr;

    };
}