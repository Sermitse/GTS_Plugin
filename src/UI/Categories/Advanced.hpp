#pragma once

#include "UI/ImGUI/ImCategorySplit.hpp"
#include "Config/Config.hpp"

namespace GTS {

    class CategoryAdvanced final : public ImCategorySplit {
        public:
        CategoryAdvanced(){
            title = "Advanced";
        }

        void DrawLeft() override;
        void DrawRight() override;

        private:
        SettingsAdvanced_t& Settings = Config::Advanced;
    };

}