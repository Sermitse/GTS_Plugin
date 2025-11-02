#pragma once
#include "UI/Controls/BuffIcons/IDynIcon.hpp"


namespace ImGuiEx {

	class DynIconEnchantment final : public IDynIcon {
	public:
		explicit DynIconEnchantment(uint32_t a_size);
		bool Draw(float a_percent) const;
	};

}
