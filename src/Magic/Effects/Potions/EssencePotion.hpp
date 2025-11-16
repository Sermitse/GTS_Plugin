#pragma once
#include "Magic/Magic.hpp"

namespace GTS {

	class EssencePotion : public Magic {

		public:
		using Magic::Magic;
		virtual std::string GetName() override;
		virtual void OnStart() override;
		EssencePotion(ActiveEffect* effect);

		private:
		float power = 0.0f;	
	};
}
