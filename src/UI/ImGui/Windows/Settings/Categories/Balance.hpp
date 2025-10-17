#pragma once

#include "UI/ImGUI/Core/ImCategorySplit.hpp"

namespace GTS {

    class CategoryBalance final : public ImCategorySplit {
        public:
        CategoryBalance();
        void DrawLeft() override;
        void DrawRight() override;

    };

}
