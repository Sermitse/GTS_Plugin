#pragma once

#include "UI/Core/ImCategorySplit.hpp"

namespace GTS {

    class CategoryInterface final : public ImCategorySplit {
        public:
        CategoryInterface();
        void DrawLeft() override;
        void DrawRight() override;
    };

}
