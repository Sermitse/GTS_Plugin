#include "UI/Controls/CollapsingTabHeader.hpp"

#include "Text.hpp"

namespace ImGuiEx {

	CollapsingTabHeader::TabData::TabData(const std::string& a_lbl): label(a_lbl) {}

	void CollapsingTabHeader::RenameTab(size_t index, std::string_view newLabel) {
		if (index < tabs.size() - 1) {
			tabs[index].label = newLabel;
		}
	}

	void CollapsingTabHeader::RenameTab(std::string_view oldLabel, std::string_view newLabel) {
		for (auto& tab : tabs) {
			if (tab.label == oldLabel) {
				tab.label = newLabel;
				break;
			}
		}
	}

	CollapsingTabHeader::CollapsingTabHeader(const std::string& a_label, const bool a_defaultOpen) : isOpen(a_defaultOpen), headerLabel(a_label) {
		id = ImGui::GetID(a_label.c_str());
	}

	CollapsingTabHeader::CollapsingTabHeader(const std::string& a_label, const std::vector<std::string>& a_categories, const bool a_defaultOpen) : isOpen(a_defaultOpen), headerLabel(a_label) {
		id = ImGui::GetID(a_label.c_str());
		for (const auto& category : a_categories) {
			tabs.emplace_back(category);
		}
	}

	CollapsingTabHeader::CollapsingTabHeader(const std::string& a_label, std::initializer_list<std::string> a_categories, const bool a_defaultOpen) : isOpen(a_defaultOpen), headerLabel(a_label) {
		id = ImGui::GetID(a_label.c_str());
		for (const auto& category : a_categories) {
			tabs.emplace_back(category);
		}
	}

	void CollapsingTabHeader::AddTab(const std::string& a_tabLabel) {
		tabs.emplace_back(a_tabLabel);
	}

	void CollapsingTabHeader::ClearTabs() {
		tabs.clear();
		activeTab = 0;
	}

	void CollapsingTabHeader::SetActiveTab(int a_index) {
		if (a_index >= 0 && a_index < static_cast<int>(tabs.size())) {
			activeTab = a_index;
		}
	}

	int CollapsingTabHeader::GetActiveTab() const {
		return activeTab;
	}

	const std::string& CollapsingTabHeader::GetActiveTabLabel() const {
		if (activeTab >= 0 && activeTab < static_cast<int>(tabs.size())) {
			return tabs[activeTab].label;
		}
		static const std::string empty = "";
		return empty;
	}

	void CollapsingTabHeader::SetExtraInfo(const std::string& a_infoText) {
		headerLabelExtra = a_infoText;
	}

	void CollapsingTabHeader::SetDisabledState(bool a_disabled) {
		isDisabled = a_disabled;
        isOpen = true;
	}

    bool CollapsingTabHeader::Begin() {

        if (isDisabled) {
            isOpen = false;
        }

        ImGui::BeginDisabled(isDisabled);
        ImGui::PushID(id);

        // Sizes and style
        const auto& style = ImGui::GetStyle();
        const ImVec2 contentRegion = ImGui::GetContentRegionAvail();
        const float baseHeaderHeight = ImGui::GetFrameHeight();
        const float tabHeight = tabs.empty() ? 0.0f : ImGui::GetFontSize() + style.FramePadding.y * 1.7f;


        // Calculate dynamic tab widths
        std::vector<float> tabWidths;
        float totalTabWidth = 0.0f;
        for (const auto& tab : tabs) {

            if (!tab.visible) {
	            continue;
            }

            const float textWidth = ImGui::CalcTextSize(tab.label.c_str()).x;
            const float width = textWidth + style.FramePadding.x * 4.0f;
            tabWidths.push_back(width);
            totalTabWidth += width;
        }

        // Scale tabs if they exceed available width
        if (totalTabWidth > contentRegion.x) {
            const float scale = contentRegion.x / totalTabWidth;
            for (float& w : tabWidths) w *= scale;
        }

        // Determine number of tab rows needed
        int tabRows = 1;
        if (!tabs.empty()) {
            float currentX = style.FramePadding.x;
            for (float w : tabWidths) {
                if (currentX + w > contentRegion.x - style.FramePadding.x) {
                    tabRows++;
                    currentX = style.FramePadding.x;
                }
                currentX += w;
            }
        }

        const float totalTabsHeight = tabs.empty() ? 0.0f : (tabHeight * tabRows + style.ItemSpacing.y * (tabRows - 1));
        const float totalHeaderHeight = baseHeaderHeight + totalTabsHeight + style.CellPadding.y * 1.5f;

        const ImVec2 headerPos = ImGui::GetCursorScreenPos();
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        const ImVec2 headerMin = headerPos;
        const ImVec2 headerMax = ImVec2(headerPos.x + contentRegion.x, headerPos.y + totalHeaderHeight);

        const ImU32 headerBgColor = ImGui::GetColorU32(ImGuiCol_Header);
        const ImU32 headerBgHoveredColor = ImGui::GetColorU32(ImGuiCol_HeaderHovered);
        const ImU32 borderColor = ImGui::GetColorU32(ImGuiCol_Border);

        const ImVec2 mousePos = ImGui::GetMousePos();
        const bool headerTitleHovered = mousePos.x >= headerMin.x && mousePos.x <= headerMax.x && mousePos.y >= headerMin.y && mousePos.y <= headerMin.y + baseHeaderHeight;

        // Draw header background
        const float rounding = style.FrameRounding;
        drawList->AddRectFilled(headerMin, headerMax, headerTitleHovered ? headerBgHoveredColor : headerBgColor, rounding);

        if (style.FrameBorderSize > 0.0f) {
	        drawList->AddRect(headerMin, headerMax, borderColor, rounding, 0, style.FrameBorderSize);
        }

        // Header click for expand/collapse
        ImGui::SetCursorScreenPos(headerPos);
        ImGui::BeginDisabled(isDisabled);

        ImGui::InvisibleButton("##header_title", ImVec2(contentRegion.x, baseHeaderHeight));
        if (ImGui::IsItemClicked()) {
	        isOpen = !isOpen;
        }

        ImGui::EndDisabled();

        // Header arrow and text
        const ImGuiContext& g = *GImGui;
        const ImVec2 padding = style.FramePadding;
        const float text_offset_x = g.FontSize + padding.x * 4.0f;
        const float text_offset_y = ImMax(padding.y, ImGui::GetCurrentWindow()->DC.CurrLineTextBaseOffset);
        const ImVec2 text_pos(headerPos.x + text_offset_x, headerPos.y + text_offset_y);
        const ImVec2 arrow_pos(headerPos.x + (padding.x * 2.0f), text_pos.y);
        const ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);
        ImGui::RenderArrow(drawList, arrow_pos, text_col, isOpen ? ImGuiDir_Down : ImGuiDir_Right, 1.0f);

        const std::string& headerText = !headerLabelExtra.empty() && isDisabled ? headerLabel + " - " + headerLabelExtra : headerLabel;
        ImGui::RenderText(text_pos, headerText.c_str());

        // Tabs
        ImGui::BeginDisabled(isDisabled || !isOpen);
        {
            if (!tabs.empty()) {
                const ImVec2 tabStartPos = { headerPos.x, headerPos.y + baseHeaderHeight };
                const float tabPadding = style.ItemInnerSpacing.x * 0.5f;
                const float tabRounding = ImMin(style.TabRounding, style.FrameRounding * 0.5f);
                float currentX = tabStartPos.x + style.FramePadding.x;
                float currentY = tabStartPos.y;
                const float rowHeight = tabHeight;
                int visibleTabIndex = 0;

                for (int i = 0; i < static_cast<int>(tabs.size()); ++i) {

                    if (!tabs[i].visible) {
	                    continue;
                    }

                    const float tabWidth = tabWidths[visibleTabIndex];

                    if (currentX + tabWidth > headerPos.x + contentRegion.x - style.FramePadding.x) {
                        currentX = tabStartPos.x + style.FramePadding.x;
                        currentY += rowHeight + style.ItemSpacing.y; // only once per row
                    }

                    const ImVec2 tabMin = { currentX + tabPadding, currentY };
                    const ImVec2 tabMax = { currentX + tabWidth - tabPadding, currentY + rowHeight };

                    const bool tabHovered = !isDisabled && mousePos.x >= tabMin.x && mousePos.x <= tabMax.x && mousePos.y >= tabMin.y && mousePos.y <= tabMax.y;

                    ImU32 tabColor;
                    if (i == activeTab) tabColor = ImGui::GetColorU32(ImGuiCol_TabSelected);
                    else if (tabHovered) tabColor = ImGui::GetColorU32(ImGuiCol_TabHovered);
                    else tabColor = ImGui::GetColorU32(ImGuiCol_Tab);

                    drawList->AddRectFilled(tabMin, tabMax, tabColor, tabRounding);
                    if (style.FrameBorderSize > 0.0f) {
	                    drawList->AddRect(tabMin, tabMax, ImGui::GetColorU32(ImGuiCol_Border), tabRounding, 0, style.FrameBorderSize);
                    }

                    ImGui::SetCursorScreenPos(tabMin);
                    if (ImGui::InvisibleButton(("##tab" + std::to_string(i)).c_str(), ImVec2(tabMax.x - tabMin.x, tabMax.y - tabMin.y))) {
	                    activeTab = i;
                    }

                    const ImVec2 textSize = ImGui::CalcTextSize(tabs[i].label.c_str());
                    const ImVec2 tabTextPos = {
                        tabMin.x + ((tabMax.x - tabMin.x) - textSize.x) * 0.5f,
                        tabMin.y + ((tabMax.y - tabMin.y) - textSize.y) * 0.5f
                    };

                    TextShadowEx(tabTextPos, tabs[i].label.c_str());
                    currentX += tabWidth;
                    visibleTabIndex++;
                }
            }
        }
        ImGui::EndDisabled();


        ImGui::SetCursorScreenPos(ImVec2(headerPos.x, headerPos.y + totalHeaderHeight));
        ImGui::Dummy(ImVec2(0, 0));
        ImGui::Indent(style.IndentSpacing);
        ImGui::EndDisabled();

        return isOpen;
    }


	void CollapsingTabHeader::End() {

		ImGui::Unindent(ImGui::GetStyle().IndentSpacing);
		ImGui::PopID();
	}

	bool BeginCollapsingTabHeader(CollapsingTabHeader& header) {
		return header.Begin();
	}

	void EndCollapsingTabHeader(CollapsingTabHeader& header) {
		header.End();
	}
}
