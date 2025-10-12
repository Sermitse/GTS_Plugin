#pragma once

#include "UI/ImGUI/Core/ImCategorySplit.hpp"

namespace GTS {

    class CategoryAudio final : public ImCategorySplit {
        public:

        CategoryAudio(){
            m_name = "Audio";
        }

        void DrawLeft() override;
        void DrawRight() override;

    };
}