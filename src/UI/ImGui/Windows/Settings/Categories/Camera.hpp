#pragma once

#include "UI/ImGUI/Core/ImCategorySplit.hpp"

namespace GTS {

    class CategoryCamera: public ImCategorySplit {
        public:
        CategoryCamera(){
            m_name = "Camera";
        }

        void DrawLeft() override;
        void DrawRight() override;

    };

}
