#pragma once

#include "UI/Core/ImCategorySplit.hpp"

namespace GTS{

    class CategoryAI : public ImCategorySplit {

        public:
        CategoryAI();
        void DrawLeft() override;
        void DrawRight() override;

    };

}
