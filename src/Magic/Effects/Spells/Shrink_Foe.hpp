#pragma once

#include "Magic/Magic.hpp"

namespace GTS {

	class ShrinkFoe : public Magic {
		public:
			virtual void OnStart() override;

			virtual void OnUpdate() override;

			virtual void OnFinish() override;

			virtual std::string GetName() override;

			ShrinkFoe(ActiveEffect* effect);

		private:
			float power = 0.0f;
			float efficiency = 0.0f;
			float time_mult = 1.0f;
	};
}
