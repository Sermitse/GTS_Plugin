#pragma once
#include "magic/magic.hpp"
#include "timer.hpp"
// Module that handles footsteps


using namespace std;
using namespace SKSE;
using namespace RE;

namespace Gts {
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