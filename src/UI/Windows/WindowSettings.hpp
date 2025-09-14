#pragma once

#include "UI/ImGUI/ImWindow.hpp"
#include "UI/ImGUI/ImCategoryContainer.hpp"
#include "UI/ImGUI/ImFontManager.hpp"
#include "UI/ImGUI/ImStyleManager.hpp"

#include "Config/Config.hpp"
#include "Config/Keybinds.hpp"

namespace GTS {

    class WindowSettings : public ImWindow {

        public:
        
        WindowSettings();

        void Draw() override;

        inline bool ShouldDraw() override {
            return this->Show;
        }

        inline float GetAlphaMult() override {
            return sUI.fAlpha;
        }

        inline float GetBGAlphaMult() override {
            return sUI.fBGAlphaMult;
        }

        inline void SetDisabled(bool a_Disabled) {
            Disabled = a_Disabled;
        }

        void AsyncSave() {

            if (SaveLoadBusy.load()) {
                return;
            }

            //TODO Use std::async instead of spawning a new thread. So we can get a return value.
            SaveLoadBusy.store(true);
            std::thread(&WindowSettings::SaveImpl, this).detach();
        }

        //Unused
        /*void AsyncLoad() {

            if (SaveLoadBusy.load()) {
                return;
            }

            //TODO Use std::async instead of spawning a new thread. So we can get a return value.
            SaveLoadBusy.store(true);
            std::thread(&WindowSettings::LoadImpl, this).detach();
        }*/


        private:
        void LoadImpl();
        void SaveImpl();

        std::atomic<bool> SaveLoadBusy = false;

        ImCategoryManager& CatMgr = ImCategoryManager::GetSingleton();
        ImFontManager& FontMgr = ImFontManager::GetSingleton();
        ImStyleManager& StyleMgr = ImStyleManager::GetSingleton();

        const SettingsHidden_t& sHidden = Config::Hidden;
        const WindowConfSettings_t& sUI= Config::UI.SettingsWindow;

        bool Disabled = false;
    };
}