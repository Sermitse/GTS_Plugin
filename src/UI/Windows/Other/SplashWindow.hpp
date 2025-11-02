#pragma once

#include "UI/Core/ImWindow.hpp"

namespace GTS {

    class SplashWindow final : public ImWindow, public EventListener {

		private:
        bool m_isInMainMenu = false;

        public:

        ~SplashWindow() override;

        // Inherited via ImConfigurableWindow
        void Draw() override;
        bool WantsToDraw() override;
        void Init() override;
        float GetFullAlpha() override;
        float GetBackgroundAlpha() override;
        std::string GetWindowName() override;
        void RequestClose() override;

        // Inherited via EventListener
        std::string DebugName() override;
        void MenuChange(const RE::MenuOpenCloseEvent* a_event) override;
    };
}