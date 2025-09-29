#include "UI/ImGui/ImWindowManager.hpp"
#include "UI/UIManager.hpp"
#include "UI/ImGui/ImUtil.hpp"
#include <UI/Windows/TestWindow.hpp>

namespace GTS {

    bool ImWindowManager::HasWindows() const {
        return m_windows.size() > 0;
    }

    bool ImWindowManager::HasInputConsumers() {
        return std::ranges::any_of(m_windows, [](const auto& window) {
            return window->IsVisible() && window->m_consumeInput;
        });

    }

    bool ImWindowManager::HasActiveWindows() {
        return std::ranges::any_of(m_windows, [](const auto& window) {
            return window->IsVisible();
        });

    }

    ImWindow::DrawLevel ImWindowManager::GetHighestDrawLevel() const {
        ImWindow::DrawLevel hDrawLevel = ImWindow::DrawLevel::kHUD;
        for (const auto& window : m_windows) {
            if (window->IsVisible() && window->m_drawLevel > hDrawLevel) {
                hDrawLevel = window->m_drawLevel;
            }
        }
        return hDrawLevel;

    }

    void ImWindowManager::AddWindow(std::unique_ptr<ImWindow> a_window) {

        assert(a_window != nullptr);

        for (const auto& window : m_windows) {
            const auto& nam = a_window->GetWindowName();
            if (window->GetWindowName() == nam) {
                logger::error("Duplicate Window {}", nam);
                return;
            }
        }

        m_windows.push_back(std::move(a_window));
        logger::info("ImWindowManager::AddWindow {}", m_windows.back()->GetWindowName());
    }

    void ImWindowManager::Update() const {

        GTS_PROFILE_SCOPE("ImWindowManager Update");

        if (!HasWindows()) [[unlikely]] {
            return;
        }

        // Check if any windows need the game cursor
        // Present layer windows use ImGui's Win32 cursor instead
        bool anyWindowNeedsGameCursor = false;
        for (const auto& window : m_windows) {
            if (window->ShouldDraw() && window->NeedsGameCursor()) {
                anyWindowNeedsGameCursor = true;
                break;
            }
        }

        // Update game cursor requirement for overlay
        if (UIManager::Input) {
            UIManager::Input->SetOverlayWantsCursor(anyWindowNeedsGameCursor);
        }

        // Draw all windows that should be drawn
        // Note: kHUD and kCursor layer windows only draw if game cursor is visible
        // kPresent layer windows always draw (they use ImGui's Win32 cursor)
        for (const auto& window : m_windows) {

            if (!window->ShouldDraw()) {
                continue;
            }

            // Check if this window can be drawn based on its layer
            if (window->m_drawLevel != ImWindow::DrawLevel::kPresent) {
                if (UIManager::Input && !UIManager::Input->IsCursorVisible()) {
                    continue;
                }
            }

            const float BGAlpha = window->GetBGAlphaMult();
            const float AlphaMult = window->GetAlphaMult();

            ImVec4 BorderCol = ImGui::GetStyle().Colors[ImGuiCol_Border];
            BorderCol.w *= BGAlpha;

            ImGui::SetNextWindowBgAlpha(BGAlpha * AlphaMult);
            ImGui::PushStyleColor(ImGuiCol_Border, BorderCol);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, AlphaMult);
            FontManager->PushFont(ImFontManager::ActiveFontType::kText);

            {
                ImGui::Begin(window->GetWindowName().c_str(), nullptr, window->m_flags);
                window->Draw();
                ImGui::End();
            }

            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
            ImGui::PopFont();
        }
    }

    void ImWindowManager::Init() {

        FontManager = new ImFontManager();
        FontManager->Init();

        StyleManager = new ImStyleManager();
        StyleManager->ApplyStyle();

        AddWindow(std::make_unique<TestWindow>());

        AddWindow(std::make_unique<TestWindow>("Win1"));

        for (const auto& window : m_windows) {
            window->Init();
        }

        m_windows.front()->m_drawLevel = ImWindow::DrawLevel::kPresent;

        logger::info("Window Manager Init");

    }

    void ImWindowManager::ApplyStyle() const {
        StyleManager->ApplyStyle();
    }

    void ImWindowManager::SetFont(ImFontManager::ActiveFontType a_fontType) const {
        FontManager->PushFont(a_fontType);
    }

    void ImWindowManager::PopFont() {
        ImGui::PopFont();
    }

    ImWindow* ImWindowManager::GetWindowByName(const std::string& a_name) const {
        for (const auto& window : m_windows) {
            if (window->GetWindowName() == a_name) {
                return window.get();
            }
        }
        logger::error("ImWindowManager::GetWindowByName Name: {} does not exist", a_name);
        return nullptr;
    }
}