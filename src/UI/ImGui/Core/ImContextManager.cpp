#include "UI/ImGui/Core/ImContextManager.hpp"
#include "UI/ImGui/Core/ImWindow.hpp"

namespace GTS {

    ImContextManager::ContextMode ImContextManager::GetModeForWindowType(int windowType) {

        if (UI::GetSingleton()->IsMenuOpen(RE::MainMenu::MENU_NAME)) {
            return ContextMode::kMainMenu;
        }

        switch (static_cast<ImWindow::WindowType>(windowType)) {
            case ImWindow::WindowType::kWidget:   return ContextMode::kHUD;
            case ImWindow::WindowType::kSettings: return ContextMode::kSettings;
            case ImWindow::WindowType::kDebug:    return ContextMode::kDebug;
            default:                              return ContextMode::kHUD;
        }
    }

    constexpr ImContextManager::ContextConfig ImContextManager::GetConfigForMode(ContextMode mode) {
        switch (mode) {
            case ContextMode::kHUD:      return { 0, false }; //We want to draw below everyting, because even though we don't request the cursor if a game menu renders at a lower priority our menu will steal input.
            case ContextMode::kMainMenu: return { 9, false }; //Main Menu draws at 8 if set above 10 input is stolen from it.
            case ContextMode::kSettings: return { 4, true };  //Highest normal menu is at 3 (FaderMenu) If somehow we enter a special mode the settings should be auto closed regardless.
            case ContextMode::kDebug:    return { 12, true }; //Imgui debug stuff should be above everything else, but below cursor which is at 13.
            default:                     return { 0, false };
        }
    }

    void ImContextManager::RequestSwitch(ContextMode mode) {
        if (mode != m_currentMode) {
            m_requestedMode = mode;
            m_pendingSwitch = true;
        }
    }

    void ImContextManager::RequestSwitchForWindowType(int windowType) {
        RequestSwitch(GetModeForWindowType(windowType));
    }

    bool ImContextManager::HasPendingSwitch() const {
        return m_pendingSwitch;
    }

    ImContextManager::ContextConfig ImContextManager::ApplyPendingSwitch() {
        if (!m_pendingSwitch) {
            return GetConfigForMode(m_currentMode);
        }

        m_pendingSwitch = false;
        m_currentMode = m_requestedMode;
        return GetConfigForMode(m_currentMode);
    }

    ImContextManager::ContextMode ImContextManager::GetCurrentMode() const {
        return m_currentMode;
    }

    ImContextManager::ContextConfig ImContextManager::GetCurrentConfig() const {
        return GetConfigForMode(m_currentMode);
    }

}