#pragma once
#include "UI/ImGUI/Core/ImCategorySplit.hpp"

namespace GTS {

    class CategoryWidgets final : public ImCategorySplit {
        public:

        CategoryWidgets();

    private:
        static void DrawWidgetSettings();
        void DrawLeft() override;
        void DrawRight() override;

    };

}
