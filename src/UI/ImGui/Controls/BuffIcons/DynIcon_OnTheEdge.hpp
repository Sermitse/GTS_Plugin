#pragma once
#include "UI/ImGui/Controls/BuffIcons/IDynIcon.hpp"


namespace ImGuiEx {

	class DynIconOnTheEdge final : IDynIcon {
	public:
		explicit DynIconOnTheEdge(uint32_t a_size);
		bool Draw(float a_percent) const;
	};

}
