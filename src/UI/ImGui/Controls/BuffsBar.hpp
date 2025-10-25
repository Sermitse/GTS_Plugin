#pragma once
#include "UI/ImGui/Core/ImGraphics.hpp"
#include "UI/ImGui/Lib/imgui.h"

namespace ImGuiEx {

	class StatusBar {
		public:
		enum class LayoutDirection {
			Horizontal,
			Vertical
		};

		struct Style {
			ImVec2 iconSize = { 64.0f, 64.0f };
			float spacing = 4.0f;
			LayoutDirection direction = LayoutDirection::Horizontal;
		};

		explicit StatusBar(const Style& style = {}) :
			m_style(style) {
		}

		void Draw(const std::vector<IDynIcon*>& icons, const ImVec2& startPos) const {
			ImVec2 pos = startPos;
			for (auto* icon : icons) {
				if (!icon) continue;
				ImGui::SetCursorPos(pos);
				icon->Draw(0);

				if (m_style.direction == LayoutDirection::Horizontal)
					pos.x += m_style.iconSize.x + m_style.spacing;
				else
					pos.y += m_style.iconSize.y + m_style.spacing;
			}
		}

		void SetStyle(const Style& style) { m_style = style; }
		const Style& GetStyle() const { return m_style; }

		private:
		Style m_style;
	};
}
