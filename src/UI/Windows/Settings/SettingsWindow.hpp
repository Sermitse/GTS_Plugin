#pragma once

#include "UI/Core/ImWindow.hpp"
#include "UI/Core/ImCategoryContainer.hpp"
#include "Managers/Input/InputManager.hpp"

namespace GTS {

    class SettingsWindow final : public ImConfigurableWindow<SettingsWindow> {

        public:
        void BuildFooterText();
        void DisableUIInteraction(bool a_disabled);
        void ShowErrorModal(bool* a_requestOpen);
        bool m_busy = false;
        bool m_disableUIInteraction = false;
        bool m_isConfiguringWidgets = false;
        bool m_MorphDataWasModified = false;
        bool m_show = false;

		private:
        bool LoadImpl();
        bool SaveImpl();
        void Draw() override;
        void Init() override;
        void RequestClose() override;
        bool WantsToDraw() override;
        static void OpenSettingsKeybindCallback([[maybe_unused]] const ManagedInputEvent& a_event);
        static void OpenSettingsConsoleCallback();
        void HandleOpenClose(bool a_open);

        ImCategoryContainer* CategoryMgr = nullptr;
        std::atomic_flag m_saveLoadBusy = ATOMIC_FLAG_INIT;
        std::string m_footerText;
        bool m_showErrorModal = false;
    };
}