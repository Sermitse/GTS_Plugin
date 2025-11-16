#pragma once
#include "Magic/Magic.hpp"

namespace GTS {

	class ShrinkResistPotion : public Magic {

		public:
		using Magic::Magic;
		virtual void OnStart() override;
		virtual void OnFinish() override;
		virtual std::string GetName() override;
		ShrinkResistPotion(ActiveEffect* effect);

		private:
		float Resistance = 0.0f;
	};
}