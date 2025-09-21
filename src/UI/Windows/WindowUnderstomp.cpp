#include "UI/ImGui/Lib/imgui.h"
#include "UI/Windows/WindowUnderstomp.hpp"

#include "Managers/Animation/Stomp_Under.hpp"
#include "Managers/Cameras/CamUtil.hpp"

#include "UI/Windows/WindowSettings.hpp"
#include "UI/ImGui/ImFontManager.hpp"
#include "UI/ImGui/ImUtil.hpp"
#include "UI/ImGui/ImWindowManager.hpp"

namespace {
    bool AreEqual(float a, float b, float epsilon = std::numeric_limits<float>::epsilon()) {
        return (fabs(a - b) <= epsilon * std::max(fabs(a), fabs(b)));
    }

    //Yes i know this is bad and 75% of is redundant
    bool CalcUnderstomp(RE::Actor* giant, float& a_outangle) {

        if (giant->formID == 0x14 && GTS::IsFreeCameraEnabled()) {
            return false;
        }
        //Range is between -1 (looking down) and 1 (looking up)
               //abs makes it become 1 -> 0 -> 1 for down -> middle -> up
        const float absPitch = abs(GTS::GetCameraRotation().entry[2][1]);
        //Remap our starting range
        constexpr float InvLookDownStartAngle = 0.9f; //Starting value of remap. Defines start angle for how down we are looking
        const float InvLookdownIntensity = std::clamp(GTS::AnimationUnderStomp::Remap(absPitch, 1.0f, InvLookDownStartAngle, 0.0f, 1.0f), 0.0f, 1.0f);

        bool allow = absPitch > InvLookDownStartAngle;
        // Allow to stomp when looking from above or below

        if (allow) {
            a_outangle = 1.0f - std::clamp(InvLookdownIntensity * 1.3f, 0.0f, 1.0f);
        }

        return allow;
    }

}

namespace GTS {

    void WindowUnderstomp::CheckFade(RE::Actor* a_actor) {


        bool ShouldShow = CalcUnderstomp(a_actor, Angle);

        if (!ShouldShow) {
            Angle = 0.0f;
        }

        if (auto Window = dynamic_cast<WindowSettings*>(ImWindowManager::GetSingleton().GetWindowByName("Settings"))) {
            if (Window->Show) {
                ShowImmediate();
                return;
            }
        }

        if (!sUI.bEnableFade) {
            ShowImmediate();
            return;
        }

        if (a_actor) {

            if (ShouldShow) {
                //LastData.Scale = Scale;
                Show();
                return;
            }

            if (AreEqual(AutoFadeAlpha, 0.0f)) {
                return;
            }


            if (Time::WorldTimeElapsed() - LastWorldTime > Config::UI.StatusWindow.fFadeAfter) {
                StartFade();
            }
        }
    }

    void WindowUnderstomp::ShowImmediate() {
        AutoFadeAlpha = 1.0;
    }

    void WindowUnderstomp::Show() {

        LastWorldTime = Time::WorldTimeElapsed();

        if (AreEqual(AutoFadeAlpha, 1.0f)) {
            return;
        }

        //Disable Fade
        TaskManager::RunFor(ShowTask, 0.2f, [this](auto& progressData) {
            AutoFadeAlpha = progressData.progress;
            return true;
        });

    }

    void WindowUnderstomp::StartFade() {
        constexpr float Duration = 0.6f;

        if (AreEqual(AutoFadeAlpha, 0.0f)) {
            return;
        }

        TaskManager::RunFor(FadeTask, Duration, [this](auto& progressData) {
            AutoFadeAlpha = 1.0f - progressData.progress;
            return true;
            });

    }

    void WindowUnderstomp::Init() {
        Title = "Understomp Angle";
        Name = "Understomp";
        this->flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoFocusOnAppearing;
        AnchorPos = WindowAnchor::kTopRight;
        ConsumeInput = false;
    }

    void WindowUnderstomp::Draw() {

        flags = (sUI.bLock ? (flags | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove) : (flags & ~ImGuiWindowFlags_NoResize & ~ImGuiWindowFlags_NoMove));

        //Handle Fixed Position and Size
        if (sUI.bLock) {
            //X,Y
            const ImVec2 Offset{ sUI.f2Offset[0], sUI.f2Offset[1] };
            ImGui::SetWindowPos(GetAnchorPos(StringToEnum<WindowAnchor>(sUI.sAnchor), Offset, true));

        }

        if (auto Player = RE::PlayerCharacter::GetSingleton()) {

            CheckFade(Player);

            ImFontManager::PushActiveFont(ImFontManager::ActiveFontType::kWidgetBody);

            std::string Text = "";

            if (sUI.iFlags > 0) {
				Text = fmt::format(
	                fmt::runtime([this]() -> std::string {
	                    if (AreEqual(Angle, 0.0, 0.01))
	                        return "Far Stomp";
	                    if (AreEqual(Angle, 1.0, 0.01))
	                        return "Under Stomp";
	                    return "Angle {:.2f}x";
	                }()),
	                Angle
                );
            }


            const float VisualProgress = Angle / 1.0f;
            const ImVec4 ColorAVec{ sUI.f3ColorA[0], sUI.f3ColorA[1], sUI.f3ColorA[2] ,1.0f };
            const ImVec4 ColorBVec{ sUI.f3ColorB[0], sUI.f3ColorB[1], sUI.f3ColorB[2] ,1.0f };
            const ImU32 ColorA32 = ImGui::ColorConvertFloat4ToU32(ColorAVec);
            const ImU32 ColorB32 = ImGui::ColorConvertFloat4ToU32(ColorBVec);
            const float BorderCol = sUI.fBorderLightness;
            const float BorderAlpha = sUI.fBorderAlpha;

            ImGui::PushStyleColor(ImGuiCol_Border, { BorderCol, BorderCol, BorderCol, BorderAlpha });

            // Visual Scale (Progress) Bar
            ImUtil::CenteredProgress(
                VisualProgress,
                { sUI.fFixedWidth, 0.0f },
                Text.c_str(),
                sUI.fSizeBarHeightMult,
                sUI.fBorderThickness,
                sUI.bUseGradient,
                sUI.fNormalGradientDarkMult,
                sUI.fNormalGradientLightMult,
                sUI.bEnableRounding,
                sUI.bUseCustomGradientColors,
                ColorA32,
                ColorB32,
                sUI.bFlipGradientDirection
            );

            ImGui::PopStyleColor();
            ImFontManager::PopActiveFont();
        }

        ImGui::SetWindowSize({ 0.0f,0.0f });
    }
}