#include "UI/Core/ImWindowManager.hpp"

#include "UI/Core/ImFontManager.hpp"
#include "UI/Core/ImStyleManager.hpp"
#include "UI/Core/ImUtil.hpp"

#include "UI/Windows/Other/DebugWindow.hpp"
#include "UI/Windows/Other/KillFeedWindow.hpp"
#include "UI/Windows/Other/SplashWindow.hpp"
#include "UI/Windows/Settings/SettingsWindow.hpp"
#include "UI/Windows/Widgets/SizeBarWindow.hpp"
#include "UI/Windows/Widgets/StatusBarWindow.hpp"
#include "UI/Windows/Widgets/USBarWindow.hpp"

namespace GTS {

    bool ImWindowManager::HasWindows() const {
        return Windows.size() > 0;
    }

    bool ImWindowManager::HasInputConsumers() {
		return HasActiveWindows() && Context->GetCurrentConfig().cursorEnabled;
    }

    bool ImWindowManager::HasActiveWindows() {
        return std::ranges::any_of(Windows, [](const auto& window) {
            return window->WantsToDraw() && !window->IsFadeComplete();
        });

    }

    bool ImWindowManager::CloseInputConsumers() const {
        ImGuiContext* ctx = ImGui::GetCurrentContext();
        ImGuiWindow* target = ImUtil::GetEffectiveFocusedWindow(ctx);

        if (!target) {
            for (const auto& window : Windows) {
                if (window->WantsToDraw() && window->m_windowType > ImWindow::WindowType::kWidget) {
                    window->RequestClose();
                    return true;
                }
            }
            return false;
        }

        for (const auto& window : Windows) {
            if (!window->WantsToDraw() || window->m_windowType <= ImWindow::WindowType::kWidget)
                continue;

            if (ImGuiWindow* native = ImGui::FindWindowByName(window->GetWindowName().c_str())) {
                if (native->RootWindow == target) {
                    window->RequestClose();
                    return true;
                }
            }
        }

        return false;
    }

    ImWindow::WindowType ImWindowManager::GetHighestVisibleWindowType() const {
        ImWindow::WindowType highestType = ImWindow::WindowType::kWidget;

        for (const auto& window : Windows) {
            if (window->WantsToDraw() && window->m_windowType > highestType) {
                highestType = window->m_windowType;
            }
        }

        return highestType;
    }

    void ImWindowManager::AddWindow(std::unique_ptr<ImWindow> a_window, ImWindow** a_accessor) {

        assert(a_window != nullptr);

        a_window->Init();

        for (const auto& window : Windows) {
            const auto& nam = a_window->GetWindowName();
            if (window->GetWindowName() == nam) {
                logger::error("Duplicate Window {}", nam);
                return;
            }
        }

        Windows.emplace_back(std::move(a_window));

        // assign out pointer if requested
        if (a_accessor) {
            *a_accessor = Windows.back().get();
        }

        logger::info("ImWindowManager::AddWindow {}", Windows.back()->GetWindowName());
       
    }

    void ImWindowManager::Update() {
        GTS_PROFILE_SCOPE("ImWindowManager Update");

        if (!HasWindows()) [[unlikely]] {
            return;
        }

		// Update cached teammate count once per update.
        m_cachedTeamMateList = FindTeammates();

        float deltaTime = GetDeltaTime();
        bool isDebugging = false;

        for (const auto& window : Windows) {

            window->UpdateFade(deltaTime);

            if (window->IsDebugging()) {
                isDebugging |= true;
                window->DebugDraw();
            }

            if (!window->WantsToDraw()) {
                continue;
            }

            const float FadeMult = window->GetFadingAlpha();
            const float BGAlpha = window->GetBackgroundAlpha() * FadeMult;
            const float AlphaMult = window->GetFullAlpha() * FadeMult;

            ImVec4 BorderCol = ImGui::GetStyle().Colors[ImGuiCol_Border];
            BorderCol.w *= BGAlpha;

            ImGui::SetNextWindowBgAlpha(BGAlpha * AlphaMult);

            ImGui::PushStyleColor(ImGuiCol_Border, BorderCol);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, AlphaMult);
            ImFontManager::Push(ImFontManager::ActiveFontType::kText);

            {
                ImGui::Begin(window->GetWindowName().c_str(), nullptr, window->m_flags);
                window->Draw();
                ImGui::End();
            }

            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
            ImFontManager::Pop();
        }

        const auto WindowType = GetHighestVisibleWindowType();
        Context->RequestSwitchForWindowType(isDebugging ? std::max(ImWindow::WindowType::kDebugNoInput, WindowType) : WindowType);
        if (Context->HasPendingSwitch()) {
            Context->ApplyPendingSwitch();
        }

    }

    float ImWindowManager::GetDeltaTime() const {
        float currentTime = ImGui::GetTime();
        float deltaTime = currentTime - m_lastFrameTime;
        const_cast<ImWindowManager*>(this)->m_lastFrameTime = currentTime;
        return deltaTime;
    }

    const std::vector<std::unique_ptr<ImWindow>>& ImWindowManager::GetWindows() const {
        return Windows;
    }

    void ImWindowManager::Init() {

        logger::info("ImContextManager Init");
        Context = new ImContextManager();

        logger::info("ImFontManager Init");
        ImFontManager::Init();

        logger::info("ImStyleManager Init");
        ImStyleManager::ApplyStyle();

        AddWindow(std::make_unique<SplashWindow>(),    &wSplash);
        AddWindow(std::make_unique<SettingsWindow>(),  &wSettings);
        AddWindow(std::make_unique<DebugWindow>(),     &wDebug);
        AddWindow(std::make_unique<USBarWindow>(),     &wUBar);
        AddWindow(std::make_unique<StatusBarWindow>(), &wStatusBar);
        AddWindow(std::make_unique<KillFeedWindow>(),  &wKillFeed);

        //Size Bar Windows
        AddWindow(std::make_unique<SizeBarWindow>("P"),   &wSBarP );
        AddWindow(std::make_unique<SizeBarWindow>("F1"),  &wSBarF1);
        AddWindow(std::make_unique<SizeBarWindow>("F2"),  &wSBarF2);
        AddWindow(std::make_unique<SizeBarWindow>("F3"),  &wSBarF3);
        AddWindow(std::make_unique<SizeBarWindow>("F4"),  &wSBarF4);
        AddWindow(std::make_unique<SizeBarWindow>("F5"),  &wSBarF5);

        logger::info("ImWindowManager Init OK");

    }

    int8_t ImWindowManager::GetDesiredPriority() const {
        return Context->GetCurrentConfig().depthPriority;
    }

    bool ImWindowManager::GetDesiredCursorState() const {
        return Context->GetCurrentConfig().cursorEnabled;
    }

    std::vector<Actor*> ImWindowManager::GetCachedTeamMateList(){
    	return m_cachedTeamMateList;
    }

	ImWindow* ImWindowManager::GetWindowByName(const std::string& a_name) const {
        for (const auto& window : Windows) {
            if (window->GetWindowName() == a_name) {
                return window.get();
            }
        }
        logger::error("ImWindowManager::GetWindowByName Name: {} does not exist", a_name);
        return nullptr;
    }
}
