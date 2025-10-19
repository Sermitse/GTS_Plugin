#pragma once

#include "UI/ImGui/Core/ImGraphics.hpp"
#include "UI/ImGui/Lib/imgui.h"

namespace ImGuiEx {

    enum class LayoutDirection {
        Horizontal,
        Vertical
    };

    class StatusBar {

        public:
        struct Style {
            ImVec2 iconSize = { 64.0f, 64.0f };
            float spacing = 4.0f;
            LayoutDirection direction = LayoutDirection::Horizontal;
        };

        StatusBar() = default;

        void SetStyle(const Style& newStyle) {
            style = newStyle;
        }

        const Style& GetStyle() const {
            return style;
        }

        // Begin the widget container
        void Begin() {
            startPos = ImGui::GetCursorScreenPos();
            currentIndex = 0;
        }

        // Add an icon slot - returns the screen position where you should render
        ImVec2 AddIcon() {
            ImVec2 pos;

            if (style.direction == LayoutDirection::Horizontal) {
                pos = {
                    startPos.x + currentIndex * (style.iconSize.x + style.spacing),
                    startPos.y
                };
            }
            else {
                pos = {
                    startPos.x,
                    startPos.y + currentIndex * (style.iconSize.y + style.spacing)
                };
            }

            currentIndex++;
            return pos;
        }

        // End the widget - advances ImGui cursor
        void End() {
            if (currentIndex == 0) return;

            ImVec2 totalSize;
            if (style.direction == LayoutDirection::Horizontal) {
                totalSize = {
                    currentIndex * (style.iconSize.x + style.spacing) - style.spacing,
                    style.iconSize.y
                };
            }
            else {
                totalSize = {
                    style.iconSize.x,
                    currentIndex * (style.iconSize.y + style.spacing) - style.spacing
                };
            }

            ImGui::SetCursorScreenPos(startPos);
            ImGui::Dummy(totalSize);
        }

        // Helper: Get centered text position for an icon at the given position
        ImVec2 GetCenteredTextPos(const ImVec2& iconPos, const char* text) const {
            ImVec2 textSize = ImGui::CalcTextSize(text);
            return {
                iconPos.x + (style.iconSize.x - textSize.x) * 0.5f,
                iconPos.y + (style.iconSize.y - textSize.y) * 0.5f
            };
        }

        // Get the icon size for manual calculations
        ImVec2 GetIconSize() const {
            return style.iconSize;
        }

        private:
        Style style;
        ImVec2 startPos = { 0.0f, 0.0f };
        int currentIndex = 0;

        GTS::ImGraphics::ImageTransform transform;
        GTS::ImGraphics::ImageTransform transform2;
        GTS::ImGraphics::ImageTransform transform3;
        GTS::ImGraphics::ImageTransform transform4;





    };
}