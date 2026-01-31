#pragma once
#include "UI/Controls/Icons/IDynIcon.hpp"


namespace ImGuiEx {

	class DynIconVoreBeingAbsorbed final : public IDynIcon {
	public:
		explicit DynIconVoreBeingAbsorbed(uint32_t a_size);
		bool Draw(int a_stacks, bool a_alwaysShow) const;
	};

}
