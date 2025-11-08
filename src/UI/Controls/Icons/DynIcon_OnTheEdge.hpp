#pragma once
#include "UI/Controls/Icons/IDynIcon.hpp"


namespace ImGuiEx {

	class DynIconOnTheEdge final : public IDynIcon {
	public:
		explicit DynIconOnTheEdge(uint32_t a_size);
		bool Draw(float a_percent) const;
	};

}
