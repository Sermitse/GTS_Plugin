#pragma once

#include "UI/ImGUI/ImCategorySplit.hpp"
#include "Config/Config.hpp"

namespace GTS {

    class CategoryGeneral final : public ImCategorySplit {

        public:

        CategoryGeneral(){
            title = "General";
        }

        void DrawLeft() override;
        void DrawRight() override;

        private:
        SettingsGeneral_t& Settings = Config::General;
    };

}