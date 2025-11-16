#pragma once
#include "Magic/Magic.hpp"

namespace GTS {

	class GrowthPotion : public Magic {

		public:
		using Magic::Magic;
		virtual void OnStart() override;
		virtual void OnUpdate() override;
		virtual void OnFinish() override;
		virtual std::string GetName() override;
		GrowthPotion(ActiveEffect* effect);

		private:
		Timer timer = Timer(2.33); // Run every 2.33s or as soon as we can
		float power = 0.0f;
	};
}
