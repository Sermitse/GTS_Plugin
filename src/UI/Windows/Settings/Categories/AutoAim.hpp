#pragma once

#include "UI/Core/ImCategorySplit.hpp"

namespace GTS {

    class CategoryAutoAim final : public ImCategorySplit {
        public:
        CategoryAutoAim();
        void DrawLeft() override;
        void DrawRight() override;
    };

}
