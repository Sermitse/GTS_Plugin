#pragma once

#include "UI/ImGui/Core/ImWindow.hpp"

namespace GTS {

    class USBarWindow final : public ImConfigurableWindow<USBarWindow> {

		private:
        void Draw() override;
        void Init() override;
        void RequestClose() override;
        bool WantsToDraw() override;
        float GetBackgroundAlpha() override;

        WindowSettingsUnderstompBar_t m_extraSettings = {};
        float m_prevAngle = 0.0f;
        const bool* m_isConfiguring = nullptr;
        const bool* m_settingsVisible = nullptr;
    };
}