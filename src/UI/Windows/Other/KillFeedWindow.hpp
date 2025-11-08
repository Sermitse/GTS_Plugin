#pragma once

#include "UI/Core/ImWindow.hpp"

namespace GTS {

    class KillFeedWindow final : public ImConfigurableWindow<KillFeedWindow> {

		private:
        void Draw() override;
        void Init() override;
        void RequestClose() override;
        bool WantsToDraw() override;
        virtual float GetBackgroundAlpha() override;

        WindowSettingsKillFeed_t m_extraSettings;

        const bool* m_isConfiguring = nullptr;
        const bool* m_settingsVisible = nullptr;
    };
}