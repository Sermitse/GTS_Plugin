#pragma once

#include "UI/Core/ImCategorySplit.hpp"

namespace GTS {

    class CategoryCollision : public ImCategorySplit {
    public:
        CategoryCollision();
        void DrawLeft() override;
        void DrawRight() override;

    };

}
