#pragma once

#include "Managers/Input/ManagedInputEvent.hpp"
#include "UI/Core/ImWindow.hpp"

namespace GTS {

    class DebugWindow final : public ImWindow {

        public:
        void Draw() override;
        bool WantsToDraw() override;
        void HandleOpenClose(bool a_open);
        static void OpenSettingsKeybindCallback(const ManagedInputEvent& a_event);
        void Init() override;
        float GetFullAlpha() override;
        float GetBackgroundAlpha() override;
        std::string GetWindowName() override;
        void RequestClose() override;
        void DebugDraw() override;
        bool IsDebugging() override;

    	bool m_show = false;

        //Imgui Stuff
        bool m_showDemoWindow = false;
        bool m_showStackWindow = false;
        bool m_showMetricsWindow = false;

        //GTS Profiler
        bool m_showProfiler = false;

    };
}