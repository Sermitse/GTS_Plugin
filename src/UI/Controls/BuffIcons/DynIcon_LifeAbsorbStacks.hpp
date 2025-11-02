#pragma once
#include "UI/Controls/BuffIcons/IDynIcon.hpp"

namespace ImGuiEx {

	class DynIconLifeabsorbStacks final : public IDynIcon {
		public:
		explicit DynIconLifeabsorbStacks(uint32_t a_size);
		bool Draw(int a_stacks) const;
	};

}
