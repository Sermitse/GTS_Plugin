#pragma once
#include "UI/Core/ImCategory.hpp"

namespace GTS {

    //Interface for a splitter window
    class ImCategorySplit : public ImCategory {
        private:
        const ImGuiChildFlags flags = ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY;
        const float splitRatio = 2.17f;

        public:
    	void Draw() override;
        virtual ~ImCategorySplit() override = default;
        virtual void DrawLeft() = 0;
        virtual void DrawRight() = 0;
    };
}