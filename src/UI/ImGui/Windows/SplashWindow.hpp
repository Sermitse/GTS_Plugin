#pragma once

#include "UI/ImGUI/Core/ImWindow.hpp"
#include "UI/ImGui/Core/ImWindowManager.hpp"

namespace GTS {

    class SplashWindow final : public ImConfigurableWindow<SplashWindow> {
        using ImConfigurableWindow::ImConfigurableWindow;

        public:

        // Inherited via ImConfigurableWindow
        void Draw() override;
        bool ShouldDraw() override;
        void Init() override;
        float GetAlphaMult() override;
        float GetBGAlphaMult() override;
    };
}