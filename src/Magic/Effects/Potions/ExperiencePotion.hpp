#pragma once
#include "Magic/Magic.hpp"

namespace GTS {

	class ExperiencePotion : public Magic {

		public:
		using Magic::Magic;
		virtual std::string GetName() override;
		virtual void OnStart() override;
	};
}
