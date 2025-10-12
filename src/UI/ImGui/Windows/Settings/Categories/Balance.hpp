#pragma once

#include "UI/ImGUI/Core/ImCategorySplit.hpp"

namespace GTS {

    class CategoryBalance final : public ImCategorySplit {
        public:

        CategoryBalance(){
            m_name = "Balance";
        }

        void DrawLeft() override;
        void DrawRight() override;

    };

}
