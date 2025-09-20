#include "UI/ImGui/ImWindow.hpp"    

namespace GTS {

    ImVec2 IImWindow::GetAnchorPos(WindowAnchor a_position, ImVec2 a_padding, bool a_allowCenterY) {
        auto v = ImGui::GetMainViewport();
        auto s = ImGui::GetWindowSize();

        //Get Total size first then subtract the viewport position to cancel out any offsets
        ImVec2 Origin = v->Size;
        Origin.x -= v->Pos.x;
        Origin.y -= v->Pos.y;

        //Subtract the window size to get the top left corner of the window
        Origin.x -= s.x;
        Origin.y -= s.y;

        switch (a_position) {
            case WindowAnchor::kTopLeft:
                return { a_padding.x, a_padding.y };
            case WindowAnchor::kTopRight:
                return { Origin.x - a_padding.x, a_padding.y };
            case WindowAnchor::kBottomLeft:
                return { a_padding.x, Origin.y - a_padding.y };
            case WindowAnchor::kBottomRight:
                return { Origin.x - a_padding.x, Origin.y - a_padding.y };
            case WindowAnchor::kCenter: default:
                if (a_allowCenterY) {
                    return { Origin.x * 0.5f, Origin.y - a_padding.y };
                }
        	return { Origin.x * 0.5f, Origin.y * 0.5f };
        }
    }
}