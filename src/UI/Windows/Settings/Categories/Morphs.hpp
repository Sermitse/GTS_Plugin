#pragma once

#include "UI/Core/ImCategorySplit.hpp"

namespace GTS {

    class CategoryMorphs final : public ImCategorySplit {

    public:
        CategoryMorphs();
        void DrawLeft() override;
        void DrawRight() override;
    };

}
