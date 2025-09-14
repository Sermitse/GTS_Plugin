#pragma once

#include "UI/ImGUI/ImCategorySplit.hpp"
#include "Config/Config.hpp"

namespace GTS {

    class CategoryActions final : public ImCategorySplit {

        public:

        CategoryActions() {
            title = "Actions";
        }

        void DrawLeft() override;
        void DrawRight() override;

        private:
        SettingsGeneral_t& SGeneral = Config::General;
        SettingsGameplay_t& SGameplay = Config::Gameplay;
    };

}