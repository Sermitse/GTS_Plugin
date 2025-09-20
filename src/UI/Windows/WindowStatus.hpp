#pragma once

#include "UI/ImGUI/ImWindow.hpp"
#include "Config/Config.hpp"

namespace GTS {

    class WindowStatus final : public ImWindowBase<WindowStatus> {

        public:

        struct LastShownData {
            float Scale = 0.0f;
            float MaxScale = 0.0f;
            float Aspect = 0.0f;
            double LastWorldTime = 0.0;
            float FadeAlpha = 1.0f;  // Add per-actor fade alpha
            std::string FadeTaskID;  // Store the task ID for this actor's fade
        };

        LastShownData* GetLastData(RE::Actor* a_actor);
        bool CheckFade(RE::Actor* a_actor);
	    void Show();
	    void ShowImmediate(RE::Actor* a_actor);
	    void StartFade(RE::Actor* a_actor);
	    WindowStatus();

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
	    bool Fading;


        std::string FadeTask = "StatsWindowFadeTask";
        std::string ShowTask = "StatsShowTask";
        volatile bool Busy = false;

        std::unordered_map<FormID, LastShownData> LastData = {};

        const SettingsHidden_t& sHidden = Config::Hidden;
        const WindowConfWidget_t& sUI= Config::UI.StatusWindow;

    };
}