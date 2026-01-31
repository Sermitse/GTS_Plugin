#pragma once

#include "Magic/Magic.hpp"

namespace GTS {

	class Shrink_Poison : public Magic {
		public:
		using Magic::Magic;
		virtual void OnStart() override;
		virtual void OnUpdate() override;
		virtual std::string GetName() override;
	};
}
