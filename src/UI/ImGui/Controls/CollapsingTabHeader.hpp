#pragma once

namespace ImGui {

    // Custom widget that combines collapsing header with tab bar
    class CollapsingTabHeader {

        public:
        struct TabData {
            std::string label;
            bool visible = true;

            TabData(const std::string& a_lbl) : label(a_lbl) {}
        };

        private:
        std::vector<TabData> tabs;
        int activeTab = 0;
        bool isOpen = false;
        std::string headerLabel;
        ImGuiID id;


    public:

        // Constructor with just header label
        CollapsingTabHeader(const std::string& a_label, const bool a_defaultOpen = true) : isOpen(a_defaultOpen), headerLabel(a_label) {
            id = ImGui::GetID(a_label.c_str());
        }

        // Constructor with header label and tab categories
        CollapsingTabHeader(const std::string& a_label, const std::vector<std::string>& a_categories, const bool a_defaultOpen = true) : isOpen(a_defaultOpen), headerLabel(a_label) {
            id = ImGui::GetID(a_label.c_str());
            for (const auto& category : a_categories) {
                tabs.emplace_back(category);
            }
        }

        // Constructor with header label and initializer list of tab categories
        CollapsingTabHeader(const std::string& a_label, std::initializer_list<std::string> a_categories, const bool a_defaultOpen = true) : isOpen(a_defaultOpen), headerLabel(a_label) {
            id = ImGui::GetID(a_label.c_str());
            for (const auto& category : a_categories) {
                tabs.emplace_back(category);
            }
        }

        // Add a tab to the widget
        void AddTab(const std::string& a_tabLabel) {
            tabs.emplace_back(a_tabLabel);
        }

        // Clear all tabs
        void ClearTabs() {
            tabs.clear();
            activeTab = 0;
        }

        // Set which tab is currently active
        void SetActiveTab(int a_index) {
            if (a_index >= 0 && a_index < static_cast<int>(tabs.size())) {
                activeTab = a_index;
            }
        }

        // Get the currently active tab index
        int GetActiveTab() const {
            return activeTab;
        }

        // Get the label of the currently active tab
        const std::string& GetActiveTabLabel() const {
            if (activeTab >= 0 && activeTab < static_cast<int>(tabs.size())) {
                return tabs[activeTab].label;
            }
            static const std::string empty = "";
            return empty;
        }

        // Main render function - returns true if the content area should be rendered
        bool Begin() {
            ImGui::PushID(id);

            // Calculate sizes
            ImVec2 contentRegion = ImGui::GetContentRegionAvail();
            float baseHeaderHeight = ImGui::GetFrameHeight();
            float tabHeight = tabs.empty() ? 0.0f : ImGui::GetFrameHeight() * 1.3f;
            float totalHeaderHeight = baseHeaderHeight + tabHeight + ImGui::GetStyle().CellPadding.y;

            // Calculate dynamic tab widths based on text content
            std::vector<float> tabWidths;
            float totalTabWidth = 0.0f;
            if (!tabs.empty()) {
                for (const auto& tab : tabs) {
                    if (tab.visible) {
                        float textWidth = ImGui::CalcTextSize(tab.label.c_str()).x;
                        float tabWidth = textWidth + ImGui::GetStyle().FramePadding.x * 4.0f;
                        tabWidths.push_back(tabWidth);
                        totalTabWidth += tabWidth;
                    }
                }

                // If tabs would exceed available width, scale them down proportionally
                if (totalTabWidth > contentRegion.x) {
                    float scale = contentRegion.x / totalTabWidth;
                    for (float& width : tabWidths) {
                        width *= scale;
                    }
                    totalTabWidth = contentRegion.x;
                }
            }

            // Store cursor position for the main header
            ImVec2 headerPos = ImGui::GetCursorScreenPos();

            // Get style references
            ImGuiStyle& style = ImGui::GetStyle();
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 headerMin = headerPos;
            ImVec2 headerMax = ImVec2(headerPos.x + contentRegion.x, headerPos.y + totalHeaderHeight);

            // Colors
            ImU32 headerBgColor = ImGui::GetColorU32(ImGuiCol_Header);
            ImU32 headerBgHoveredColor = ImGui::GetColorU32(ImGuiCol_HeaderHovered);
            ImU32 borderColor = ImGui::GetColorU32(ImGuiCol_Border);

            // Check if mouse is over the title area (top part of header, not tabs)
            ImVec2 mousePos = ImGui::GetMousePos();
            bool headerTitleHovered = mousePos.x >= headerMin.x && mousePos.x <= headerMax.x && mousePos.y >= headerMin.y && mousePos.y <= headerMin.y + baseHeaderHeight;

            // Draw main header background with proper rounding
            float rounding = style.FrameRounding;
            drawList->AddRectFilled(headerMin, headerMax, headerTitleHovered ? headerBgHoveredColor : headerBgColor, rounding);

            // Draw border if enabled
            if (style.FrameBorderSize > 0.0f) {
                drawList->AddRect(headerMin, headerMax, borderColor, rounding, 0, style.FrameBorderSize);
            }

            // Handle header title click for expand/collapse
            ImGui::SetCursorScreenPos(headerPos);
            ImGui::InvisibleButton("##header_title", ImVec2(contentRegion.x, baseHeaderHeight));
            if (ImGui::IsItemClicked()) {
                isOpen = !isOpen;
            }

            ImGuiContext& g = *GImGui;
            const ImVec2 padding = style.FramePadding;

            float text_offset_x = g.FontSize + padding.x * 4.0f;
            float text_offset_y = ImMax(padding.y, ImGui::GetCurrentWindow()->DC.CurrLineTextBaseOffset);

            ImVec2 text_pos(headerPos.x + text_offset_x, headerPos.y + text_offset_y);
            ImVec2 arrow_pos(headerPos.x + (padding.x * 2.0f), text_pos.y + g.FontSize * 0.15f);
            ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);

            ImGui::RenderArrow(drawList, arrow_pos, text_col, isOpen ? ImGuiDir_Down : ImGuiDir_Right, 1.00f);

            ImGui::RenderText(text_pos, headerLabel.c_str());

            // Draw tabs within the header rectangle if they exist
            if (!tabs.empty()) {
                ImVec2 tabStartPos = ImVec2(headerPos.x, headerPos.y + baseHeaderHeight);
                float tabPadding = style.ItemInnerSpacing.x * 0.5f;
                float tabRounding = ImMin(style.TabRounding, style.FrameRounding * 0.5f);

                //// Draw a subtle separator line between title and tabs
                //ImVec2 separatorStart = ImVec2(headerPos.x + style.FramePadding.x, tabStartPos.y);
                //ImVec2 separatorEnd = ImVec2(headerPos.x + contentRegion.x - style.FramePadding.x, tabStartPos.y);
                //drawList->AddLine(separatorStart, separatorEnd, ImGui::GetColorU32(ImGuiCol_Separator), 1.0f);

                float currentX = tabStartPos.x + style.FramePadding.x;
                int visibleTabIndex = 0;

                for (int i = 0; i < static_cast<int>(tabs.size()); ++i) {
                    if (!tabs[i].visible) continue;

                    float tabWidth = tabWidths[visibleTabIndex];

                    ImVec2 tabMin = ImVec2(currentX + tabPadding,tabStartPos.y + style.ItemSpacing.y * 0.5f);
                    ImVec2 tabMax = ImVec2(currentX + tabWidth - tabPadding, tabStartPos.y + tabHeight - style.ItemSpacing.y * 0.5f);

                    bool tabHovered = mousePos.x >= tabMin.x && mousePos.x <= tabMax.x && mousePos.y >= tabMin.y && mousePos.y <= tabMax.y;

                    // Tab colors - use more subtle colors since they're inside the header
                    ImU32 tabColor;
                    if (i == activeTab) {
                        // Active tab uses TabActive color or slightly brighter header
                        if (style.Colors[ImGuiCol_TabSelected].w > 0.0f) {
                            tabColor = ImGui::GetColorU32(ImGuiCol_TabSelected);
                        }
                        else {
                            ImVec4 baseColor = ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive);
                            baseColor.x = ImMin(baseColor.x + 0.1f, 1.0f);
                            baseColor.y = ImMin(baseColor.y + 0.1f, 1.0f);
                            baseColor.z = ImMin(baseColor.z + 0.1f, 1.0f);
                            tabColor = ImGui::ColorConvertFloat4ToU32(baseColor);
                        }
                    }
                    else if (tabHovered) {
                        // Hovered tab uses TabHovered color or slightly darker header
                        if (style.Colors[ImGuiCol_TabHovered].w > 0.0f) {
                            tabColor = ImGui::GetColorU32(ImGuiCol_TabHovered);
                        }
                        else {
                            ImVec4 baseColor = ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered);
                            baseColor.x *= 0.9f;
                            baseColor.y *= 0.9f;
                            baseColor.z *= 0.9f;
                            tabColor = ImGui::ColorConvertFloat4ToU32(baseColor);
                        }
                    }
                    else {
                        // Normal tab uses Tab color or darker header
                        if (style.Colors[ImGuiCol_Tab].w > 0.0f) {
                            tabColor = ImGui::GetColorU32(ImGuiCol_Tab);
                        }
                        else {
                            ImVec4 baseColor = ImGui::GetStyleColorVec4(ImGuiCol_Header);
                            baseColor.x *= 0.8f;
                            baseColor.y *= 0.8f;
                            baseColor.z *= 0.8f;
                            tabColor = ImGui::ColorConvertFloat4ToU32(baseColor);
                        }
                    }

                    // Draw tab background with proper rounding
                    drawList->AddRectFilled(tabMin, tabMax, tabColor, tabRounding);

                    // Draw tab border if active and borders are enabled
                    if (i == activeTab && style.FrameBorderSize > 0.0f) {
                        drawList->AddRect(tabMin, tabMax, ImGui::GetColorU32(ImGuiCol_Border),
                            tabRounding, 0, style.FrameBorderSize);
                    }

                    // Handle tab click
                    ImGui::SetCursorScreenPos(tabMin);
                    if (ImGui::InvisibleButton(("##tab" + std::to_string(i)).c_str(),
                        ImVec2(tabMax.x - tabMin.x, tabMax.y - tabMin.y))) {
                        activeTab = i;
                    }

                    // Draw tab text (centered) with proper text color
                    ImVec2 textSize = ImGui::CalcTextSize(tabs[i].label.c_str());
                    ImVec2 tabTextPos = ImVec2(
                        tabMin.x + ((tabMax.x - tabMin.x) - textSize.x) * 0.5f,
                        tabMin.y + ((tabMax.y - tabMin.y) - textSize.y) * 0.5f
                    );

                    // Use appropriate text color based on tab state
                    ImU32 textColor;
                    if (i == activeTab) {
                        textColor = ImGui::GetColorU32(ImGuiCol_Text);
                    }
                    else {
                        textColor = ImGui::GetColorU32(ImGuiCol_TextDisabled);
                    }

                    drawList->AddText(tabTextPos, textColor, tabs[i].label.c_str());

                    // Move to next tab position
                    currentX += tabWidth;
                    visibleTabIndex++;
                }
            }

            // Move cursor past the entire header with proper dummy
            ImGui::SetCursorScreenPos(ImVec2(headerPos.x, headerPos.y + totalHeaderHeight));
            ImGui::Dummy(ImVec2(0, 0)); // Prevent ImGui assertions
            ImGui::Indent(ImGui::GetStyle().IndentSpacing);

            return isOpen;
        }

        static void End() {
            ImGui::Unindent(ImGui::GetStyle().IndentSpacing);
            ImGui::PopID();
        }
    };

    // Helper function for easier usage
    inline bool BeginCollapsingTabHeader(CollapsingTabHeader& header) {
		return header.Begin();
    }

    inline void EndCollapsingTabHeader(CollapsingTabHeader& header) {
        header.End();
    }
}