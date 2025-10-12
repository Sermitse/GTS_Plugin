#pragma once

#include "Config/Config.hpp"
#include "UI/ImGUI/Core/ImCategorySplit.hpp"

namespace GTS {

    class CategoryAdvanced final : public ImCategorySplit {

        public:
        CategoryAdvanced();

        bool IsVisible() const override;
        void SetVisible(bool a_visible) override;

        void DrawLeft() override;
        void DrawRight() override;

    };

}
