#pragma once

#include "UI/ImGUI/ImWindow.hpp"
#include "UI/ImGui/ImWindowManager.hpp"

namespace GTS {

    class ManagedInputEvent;

    class TestWindow final : public ImConfigurableWindow<TestWindow> {
        using ImConfigurableWindow::ImConfigurableWindow;

        public:
        bool RequestShow() override;
        bool RequestHide() override;
        void OnWindowShow() override;
        void OnWindowHide() override;

        void OpenSettingsK(const GTS::ManagedInputEvent& data);

        // Inherited via ImConfigurableWindow
        void Draw() override;
        bool ShouldDraw() override;
        void Init() override;
        float GetAlphaMult() override;
        float GetBGAlphaMult() override;
    };
}