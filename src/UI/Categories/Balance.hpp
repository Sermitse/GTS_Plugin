#pragma once

#include "UI/ImGUI/ImCategorySplit.hpp"
#include "Config/Config.hpp"

namespace GTS {

    class CategoryBalance final : public ImCategorySplit {
        public:
        CategoryBalance(){
            title = "Balance";
        }

        void DrawLeft() override;
        void DrawRight() override;

        private:
        SettingsBalance& Settings = Config::Balance;
    };

}