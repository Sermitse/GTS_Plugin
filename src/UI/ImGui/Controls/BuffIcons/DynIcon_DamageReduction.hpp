#pragma once
#include "UI/ImGui/Controls/BuffIcons/IDynIcon.hpp"


namespace ImGuiEx {

	class DynIconDamageReduction final : IDynIcon {
	public:
		explicit DynIconDamageReduction(uint32_t a_size);
		bool Draw(float a_percent) const;
	};

}
