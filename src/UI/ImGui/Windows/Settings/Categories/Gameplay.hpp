#pragma once

#include "UI/ImGUI/Core/ImCategorySplit.hpp"
#include "Config/Settings/SettingsGameplay.hpp"

namespace GTS {

    class CategoryGameplay final : public ImCategorySplit {
        public:
        CategoryGameplay();
        void DrawLeft() override;
        void DrawRight() override;

        private:
        static void GameModeOptions(GameplayActorSettings_t* a_Settings);
    };

}
