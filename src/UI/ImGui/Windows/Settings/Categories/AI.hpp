#pragma once

#include "UI/ImGUI/Core/ImCategorySplit.hpp"

namespace GTS{

    class CategoryAI : public ImCategorySplit {

        public:

        CategoryAI(){
            m_name = "AI";
        }

        void DrawLeft() override;
        void DrawRight() override;

    };

}
