
#pragma once

#include "UI/ImGUI/ImCategory.hpp"
#include "Config/Config.hpp"

namespace GTS {

    class CategoryInfo final : public ImCategory {
        public:
        CategoryInfo(){
            title = "Info";
        }
        void Draw() override;

        private:
        SettingsHidden_t& Settings = Config::Hidden;
    };

}