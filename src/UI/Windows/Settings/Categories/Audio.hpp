#pragma once

#include "UI/Core/ImCategorySplit.hpp"

namespace GTS {

    class CategoryAudio final : public ImCategorySplit {
        public:
        CategoryAudio();
        void DrawLeft() override;
        void DrawRight() override;

    };
}