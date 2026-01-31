#include "UI/Core/ImContextManager.hpp"
#include "UI/Core/ImWindow.hpp"

namespace GTS {

    ImContextManager::ContextMode ImContextManager::GetModeForWindowType(int windowType) {

        if (UI::GetSingleton()->IsMenuOpen(RE::MainMenu::MENU_NAME)) {
            return ContextMode::kMainMenu;
        }

        switch (static_cast<ImWindow::WindowType>(windowType)) {
            case ImWindow::WindowType::kWidget:       return ContextMode::kHUD;
            case ImWindow::WindowType::kSettings:     return ContextMode::kSettings;
            case ImWindow::WindowType::kDebug:        return ContextMode::kDebug;
            case ImWindow::WindowType::kDebugNoInput: return ContextMode::kDebugNoInput;
            default:                                  return ContextMode::kHUD;
        }
    }

    constexpr ImContextManager::ContextConfig ImContextManager::GetConfigForMode(ContextMode mode) {
        switch (mode) {
			//Depth, Cursor, DrawOnPresent
            case ContextMode::kHUD:          return { 0,  false, false }; //We want to draw below everyting, because even though we don't request the cursor if a game menu renders at a lower priority our menu will steal input.
			case ContextMode::kMainMenu:     return { 0,  false, true  }; //Main Menu draws at 9 if set above 10 input is stolen from it. Due to messagebox shenanigans we must draw on present to avoid input highijacking.
            case ContextMode::kSettings:     return { 4,  true,  false }; //Highest normal menu is at 3 (FaderMenu) If somehow we enter a special mode the settings should be auto closed regardless.
            case ContextMode::kDebug:        return { 5,  true,  false }; //Imgui debug stuff should be above everything else, but below cursor which is at 13.
            case ContextMode::kDebugNoInput: return { 5,  false, false }; //Imgui debug stuff should be above everything else, but below cursor which is at 13.
            default:                         return { 0,  false, false };
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