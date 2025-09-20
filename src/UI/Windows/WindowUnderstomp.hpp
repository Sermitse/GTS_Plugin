#pragma once

#include "UI/ImGUI/ImWindow.hpp"
#include "Config/Config.hpp"

namespace GTS {

    class WindowUnderstomp final : public ImWindowBase<WindowUnderstomp> {

        public:
        void CheckFade(RE::Actor* a_actor);
        void Show();
        void ShowImmediate();
        void StartFade();
        WindowUnderstomp();

        void Draw() override;

        inline bool ShouldDraw() override {
            return sUI.bVisible;
        }

        inline float GetAlphaMult() override {
            return sUI.fAlpha * AutoFadeAlpha;
        }

        inline float GetBGAlphaMult() override {
            return sUI.fBGAlphaMult;
        }

        private:

        float AutoFadeAlpha = 1.0f;
        float Angle = 0.0;

        std::string_view FadeTask = "StompWindowFadeTask";
        std::string_view ShowTask = "StompShowTask";

        float LastAngle = 0.0f;
        double LastWorldTime = 0.0f;


        const WindowConfWidget_t& sUI = Config::UI.UnderstompWindow;


    };
}