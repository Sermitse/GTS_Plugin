#pragma once

#include "UI/ImGUI/ImCategorySplit.hpp"
#include "Config/Config.hpp"

namespace GTS {

    class CategoryCamera: public ImCategorySplit {
        public:
        CategoryCamera(){
            title = "Camera";
        }

        void DrawLeft() override;
        void DrawRight() override;

        private:
        SettingsCamera& Settings = Config::Camera;
    };

}