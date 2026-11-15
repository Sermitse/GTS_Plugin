#pragma once
#include "Magic/Magic.hpp"

namespace GTS {

	class Absorb : public Magic {

		public:
		Absorb(ActiveEffect* effect);
		virtual void OnStart() override;
		virtual void OnUpdate() override;
		virtual void OnFinish() override;
		virtual std::string GetName() override;

		float time_mult = 1.25f;
		float power = 1.0f;
	};
}
