#pragma once

#include "UI/ImGUI/Core/ImCategorySplit.hpp"

namespace GTS {

    class CategoryInterface final : public ImCategorySplit {
        public:
        CategoryInterface(){
            m_name = "Interface";
        }

        void DrawLeft() override;
        void DrawRight() override;
    };

}
