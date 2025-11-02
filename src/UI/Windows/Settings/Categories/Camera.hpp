#pragma once

#include "UI/Core/ImCategorySplit.hpp"

namespace GTS {

    class CategoryCamera: public ImCategorySplit {
        public:
        CategoryCamera();
        void DrawLeft() override;
        void DrawRight() override;

    };

}
