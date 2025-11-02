#pragma once
#include "UI/Core/ImCategory.hpp"
#include "UI/Core/ImCategorySplit.hpp"
#include "UI/Core/ImWindow.hpp"

namespace GTS {

    class CategoryWidgets final : public ImCategorySplit {

        public:
        CategoryWidgets();
        void Draw() override;
        void DrawOptions();
        void DrawLeft() override;
        void DrawRight() override;

		private:
        uint32_t IndexToDraw = 0;



    };

}
