#pragma once

#include "UI/ImGUI/Core/ImCategorySplit.hpp"

namespace GTS {

    class CategoryGeneral final : public ImCategorySplit {

        public:
        CategoryGeneral();
        void DrawLeft() override;
        void DrawRight() override;

    };

}
