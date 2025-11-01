#pragma once
#include "UI/ImGui/Controls/BuffIcons/IDynIcon.hpp"


namespace ImGuiEx {

	class DynIconSizeReserve final : public IDynIcon {
	public:
		explicit DynIconSizeReserve(uint32_t a_size);
		bool Draw(float a_amount) const;
	};

}
