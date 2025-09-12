#pragma once

#include "UI/ImGui/Lib/imgui.h"

namespace GTS {

    class ImWindow {

        public:

        enum class WindowAnchor {
            kTopLeft,
            kTopRight,
            kCenter,
            kBottomLeft,
            kBottomRight,
        };

        enum class WindowMessage {
            kUpdateStyle,
        };

        //Internal Flags
        bool Show = false;
        bool ConsumeInput = false;
        bool Busy = false;

        WindowAnchor AnchorPos = WindowAnchor::kCenter;

        std::string Name = "Default";   //Imgui Window Name (Must be unique)
        std::string Title = "Default";  //Window Title
        ImGuiWindowFlags flags = ImGuiWindowFlags_None;

        virtual ~ImWindow() noexcept = default;

        virtual void Draw() = 0;
        virtual inline bool ShouldDraw() = 0;
        virtual inline float GetAlphaMult() = 0;
        virtual inline float GetBGAlphaMult() = 0;

        static ImVec2 GetAnchorPos(WindowAnchor a_position, ImVec2 a_padding, bool a_allowCenterY);
    };
}