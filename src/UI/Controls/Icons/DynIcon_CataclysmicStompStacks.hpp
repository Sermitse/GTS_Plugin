#pragma once
#include "UI/Controls/Icons/IDynIcon.hpp"


namespace ImGuiEx {

	class DynIconCataclysmicStompStacks final : public IDynIcon {
	public:
		explicit DynIconCataclysmicStompStacks(uint32_t a_size);
		bool Draw(int a_stacks, bool a_alwaysShow) const;
	};

}
