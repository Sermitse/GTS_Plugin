#pragma once
#include "UI/ImGUI/Core/ImCategory.hpp"
#include "UI/ImGUI/Core/ImCategorySplit.hpp"
#include "UI/ImGui/Core/ImWindow.hpp"

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
