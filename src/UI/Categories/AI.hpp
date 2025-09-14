#pragma once

#include "UI/ImGUI/ImCategorySplit.hpp"
#include "Config/Config.hpp"

namespace GTS{

    class CategoryAI final : public ImCategorySplit {
        public:
        CategoryAI(){
            title = "AI";
        }

        void DrawLeft() override;
        void DrawRight() override;

        private:
        SettingsAI_t& Settings = Config::AI;
    };

}