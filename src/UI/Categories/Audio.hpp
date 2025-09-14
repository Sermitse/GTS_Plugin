#pragma once

#include "UI/ImGUI/ImCategorySplit.hpp"
#include "Config/Config.hpp"

namespace GTS {

    class CategoryAudio final : public ImCategorySplit {
        public:
        CategoryAudio(){
            title = "Audio";
        }

        void DrawLeft() override;
        void DrawRight() override;

        private:
        SettingsAudio_t& Settings = Config::Audio;
    };
}