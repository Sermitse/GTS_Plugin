#pragma once
#include "UI/Controls/BuffIcons/IDynIcon.hpp"


namespace ImGuiEx {

	class DynIconDamageReduction final : public IDynIcon {
	public:
		explicit DynIconDamageReduction(uint32_t a_size);
		bool Draw(float a_percent) const;
	};

}
