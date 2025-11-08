#pragma once
#include "UI/Controls/Icons/IDynIcon.hpp"


namespace ImGuiEx {

	class DynIconCataclysmicVoreStacks final : public IDynIcon {
	public:
		explicit DynIconCataclysmicVoreStacks(uint32_t a_size);
		bool Draw(int a_stacks) const;
	};

}
