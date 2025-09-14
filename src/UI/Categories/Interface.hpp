#pragma once

#include "UI/ImGUI/ImCategorySplit.hpp"
#include "UI/ImGui/ImStyleManager.hpp"
#include "UI/ImGui/ImFontManager.hpp"

#include "Config/Config.hpp"

namespace GTS {

    class CategoryInterface final : public ImCategorySplit {
        public:
        CategoryInterface(){
            title = "Interface";
        }

        void DrawLeft() override;
        void DrawRight() override;
        
        private:
        ImStyleManager& StyleMgr = ImStyleManager::GetSingleton();
        ImFontManager& FontMgr = ImFontManager::GetSingleton();
        SettingsUI_t& Settings = Config::UI;
    };

}