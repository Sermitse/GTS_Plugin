#pragma once

#include "UI/ImGui/Core/ImWindow.hpp"
#include "UI/ImGui/Core/ImCategoryContainer.hpp"
#include "Config/Settings/SettingsUI.hpp"
#include "Managers/Input/InputManager.hpp"

namespace GTS {

    class SettingsWindow final : public ImConfigurableWindow<SettingsWindow> {

        public:
        void AsyncSave();
        void BuildFooterText();
        void SetDisabled(bool a_disabled);

		private:
        void LoadImpl();
        void SaveImpl();
        void Draw() override;
        void Init() override;
        bool WantsToDraw() override;
        static void OpenSettingsKeybindCallback([[maybe_unused]] const ManagedInputEvent& a_event);
        static void OpenSettingsConsoleCallback();
        void HandleOpenClose(bool a_open);

        ImCategoryContainer* CategoryMgr = nullptr;

        std::atomic<bool> m_saveLoadBusy = false;
        std::string m_footerText;
        bool m_inputDisabled = false;
        bool m_show = false;
        bool m_busy = false;
    }; 
}