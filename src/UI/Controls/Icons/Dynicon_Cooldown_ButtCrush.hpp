#pragma once
#include "UI/Controls/Icons/IDynIcon.hpp"


namespace ImGuiEx {

	class DynIconCooldownButtCrush final : public IDynIcon {
	public:
		explicit DynIconCooldownButtCrush(uint32_t a_size);
		bool Draw(float a_percent, float a_totalCooldown, bool a_alwaysShow) const;
	};
}
