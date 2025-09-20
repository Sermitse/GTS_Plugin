#include "UI/ImGui/Lib/imgui.h"
#include "UI/Windows/WindowStatus.hpp"
#include "UI/Windows/WindowSettings.hpp"
#include "UI/Windows/GTSInfo.hpp"
#include "UI/ImGui/ImFontManager.hpp"
#include "UI/ImGui/ImWindowManager.hpp"


namespace {
    bool AreEqual(float a, float b, float epsilon = std::numeric_limits<float>::epsilon()) {
        return (fabs(a - b) <= epsilon * std::max(fabs(a), fabs(b)));
    }
}

namespace GTS {

    WindowStatus::LastShownData* WindowStatus::GetLastData(RE::Actor* a_actor) {
        if (!a_actor) {
            return nullptr;
        }

        try {
            // If the actor isn't in our map, add a new entry
            if (!LastData.contains(a_actor->formID)) {
                LastData.insert_or_assign(a_actor->formID, LastShownData());
            }

            // Return a pointer to the data in the map
            auto& data = LastData.at(a_actor->formID);
            return &data;
        }
        catch (const std::exception&) {
            return nullptr;
        }
    }

    bool WindowStatus::CheckFade(RE::Actor* a_actor) {
        if (auto Window = dynamic_cast<WindowSettings*>(ImWindowManager::GetSingleton().GetWindowByName("Settings"))) {
            if (Window->Show) {
                ShowImmediate(a_actor);
                return true;
            }
        }

        if (!sUI.bEnableFade) {
            ShowImmediate(a_actor);
            return true;
        }

        if (a_actor) {
            const GTSInfoFeatures flags = static_cast<GTSInfoFeatures>(sUI.iFlags);
            const auto Data = GetLastData(a_actor);

            if (!Data) return false;

            const float Scale = get_visual_scale(a_actor);
            if (!AreEqual(Data->Scale, Scale, sUI.fFadeDelta)) {
                Data->Scale = Scale;
                Data->LastWorldTime = Time::WorldTimeElapsed(); // Update this actor's time
                Show();
                return true;
            }

            const float MaxScale = get_max_scale(a_actor);
            if (!AreEqual(Data->MaxScale, MaxScale, 0.1f) && hasFlag(flags, GTSInfoFeatures::kMaxSize)) {
                Data->MaxScale = MaxScale;
                Data->LastWorldTime = Time::WorldTimeElapsed(); // Update this actor's time
                Show();
                return true;
            }

            const float Ench = Ench_Aspect_GetPower(a_actor);
            if (!AreEqual(Data->Aspect, Ench) && hasFlag(flags, GTSInfoFeatures::kAspect)) {
                Data->Aspect = Ench;
                Data->LastWorldTime = Time::WorldTimeElapsed(); // Update this actor's time
                Show();
                return true;
            }

            // Check if should start fade
            if (AreEqual(AutoFadeAlpha, 0.0f)) {
                return false;
            }

            if (Time::WorldTimeElapsed() - Data->LastWorldTime > Config::UI.StatusWindow.fFadeAfter) {
                StartFade(a_actor);
            }

            return false;
        }

        return false;
    }

    void WindowStatus::ShowImmediate(RE::Actor* a_actor) {
        const double currentTime = Time::WorldTimeElapsed();

        if (a_actor) {
            // Update the specific actor's time if provided
            const auto Data = GetLastData(a_actor);
            if (Data) {
                Data->LastWorldTime = currentTime;
            }
        }
        else {
            // Update all entries in the map
            for (auto& data : LastData | views::values) {
                data.LastWorldTime = currentTime;
            }
        }

        // Show the window immediately
        AutoFadeAlpha = 1.0f;
    }

    void WindowStatus::Show() {
        // Update all entries in the map with current time
        const double currentTime = Time::WorldTimeElapsed();
        for (auto& data : LastData | views::values) {
            data.LastWorldTime = currentTime;
        }

        if (AreEqual(AutoFadeAlpha, 1.0f)) {
            return;
        }

        // Cancel any existing fade task
        if (!Busy) {
            // Cancel any existing task
            TaskManager::Cancel(FadeTask);

            // Use the global task for showing
            Busy = true;
            TaskManager::RunFor(ShowTask, 0.2f, [this](auto& progressData) {
                AutoFadeAlpha = static_cast<float>(progressData.progress);
                if (progressData.progress >= 1.0f) {
                    Busy = false;
                }
                return true;
                });
        }
    }

    void WindowStatus::StartFade(RE::Actor* a_actor) {
        // Check if any actor should keep the window visible
        bool anyVisible = false;
        const double currentTime = Time::WorldTimeElapsed();
        const float fadeThreshold = Config::UI.StatusWindow.fFadeAfter;

        for (auto& actorData : LastData | views::values) {
            if (currentTime - actorData.LastWorldTime <= fadeThreshold) {
                anyVisible = true;
                break;
            }
        }

        // Only start the fade if no actors need to be visible
        if (!anyVisible && !AreEqual(AutoFadeAlpha, 0.0f) && !Busy) {
            constexpr float Duration = 0.6f;

            Busy = true;
            TaskManager::RunFor(FadeTask, Duration, [this](auto& progressData) {
                AutoFadeAlpha = 1.0f - static_cast<float>(progressData.progress);
                if (progressData.progress >= 1.0f) {
                    Busy = false;
                }
                return true;
                });
        }
    }

    WindowStatus::WindowStatus() {
        Title = "Player Info";
        Name = "Status";
        this->flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoFocusOnAppearing;
        AnchorPos = WindowAnchor::kTopRight;
        ConsumeInput = false;
        Fading = false;
    }

    void WindowStatus::Draw() {
        flags = (sUI.bLock ? (flags | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove) : (flags & ~ImGuiWindowFlags_NoResize & ~ImGuiWindowFlags_NoMove));

        //Handle Fixed Position and Size
        if (sUI.bLock) {
            //X,Y
            const ImVec2 Offset{ sUI.f2Offset[0], sUI.f2Offset[1] };
            ImGui::SetWindowPos(GetAnchorPos(StringToEnum<WindowAnchor>(sUI.sAnchor), Offset, true));
        }

        {
            if (auto Player = RE::PlayerCharacter::GetSingleton()) {

                ImFontManager::PushActiveFont(ImFontManager::ActiveFontType::kWidgetBody);

                for (const auto Teamate : FindTeammates()) {
                    if (const auto ActorData = Persistent::GetSingleton().GetData(Teamate)) {
                        if (ActorData->ShowSizebarInUI) {
                            CheckFade(Teamate);
                            DrawGTSSizeBar(Config::UI.StatusWindow.iFlags, Teamate, true);
                        }
                    }
                }

                CheckFade(Player);
                DrawGTSInfo(Config::UI.StatusWindow.iFlags, Player, true);

                ImFontManager::PopActiveFont();
            }
        }

        ImGui::SetWindowSize({ 0.0f,0.0f });
    }
}
