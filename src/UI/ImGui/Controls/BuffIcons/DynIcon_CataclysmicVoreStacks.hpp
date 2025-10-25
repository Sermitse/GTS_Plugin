#pragma once
#include "UI/ImGui/Controls/BuffIcons/IDynIcon.hpp"


namespace ImGuiEx {

	class DynIconCataclysmicVoreStacks final : IDynIcon {
	public:
		explicit DynIconCataclysmicVoreStacks(uint32_t a_size);
		bool Draw(int a_stacks) const;
	};

}
