#pragma once

#include "UI/Core/ImCategorySplit.hpp"

namespace GTS {

    class CategoryActions final : public ImCategorySplit {

        public:
        CategoryActions();
        void DrawLeft() override;
        void DrawRight() override;

    };

}
