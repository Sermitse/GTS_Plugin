#pragma once

#include "UI/ImGUI/Core/ImCategory.hpp"

namespace GTS {

    class CategoryStats final : public ImCategory {

        public:
        CategoryStats();
        void Draw() override;
    };
}