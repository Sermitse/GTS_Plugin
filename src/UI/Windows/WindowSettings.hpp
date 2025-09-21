#pragma once

#include "UI/ImGUI/ImWindow.hpp"
#include "UI/ImGUI/ImCategoryContainer.hpp"
#include "UI/ImGUI/ImFontManager.hpp"
#include "UI/ImGUI/ImStyleManager.hpp"

#include "Config/Config.hpp"

struct TestStruct_t {
    float fTestValue = 99.0f;
};
TOML_SERIALIZABLE(TestStruct_t);

namespace GTS {

    class WindowSettings final : public ImWindowBase<WindowSettings> {

        public:
        WindowSettings(const std::string& a_name = "", const std::string& a_preffix = "UI") : ImWindowBase(a_name, a_preffix) {
            Init();
        }

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

        private:
        void LoadImpl();
        void SaveImpl();
        void Init();

        std::atomic<bool> SaveLoadBusy = false;

        ImCategoryManager& CatMgr = ImCategoryManager::GetSingleton();
        ImFontManager& FontMgr = ImFontManager::GetSingleton();
        ImStyleManager& StyleMgr = ImStyleManager::GetSingleton();

        const SettingsHidden_t& sHidden = Config::Hidden;
        const WindowConfSettings_t& sUI= Config::UI.SettingsWindow;
        TestStruct_t TestStruct = {};

        bool Disabled = false;
    };
}