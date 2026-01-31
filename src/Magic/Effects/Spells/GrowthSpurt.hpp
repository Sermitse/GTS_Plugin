#pragma once
#include "Magic/magic.hpp"

namespace GTS {

	class GrowthSpurt : public Magic {
		public:
		virtual void OnUpdate() override;
		virtual void OnStart() override;
		virtual void OnFinish() override;
		virtual std::string GetName() override;
		virtual void DoGrowth(Actor* actor, float value);
		virtual void DoShrink(Actor* actor);
		GrowthSpurt(ActiveEffect* effect);

		private:
		float power = 0.0f;
		bool AllowStacking = true;
		float grow_limit = 1.0f;
		Timer timer = Timer(2.33);
		Timer timerSound = Timer(0.7);

		inline PSString GrowthSpurtMessage = 
			"Growth Spurt slowly regenerates your health and increases your size until you reach maximum size.\n"
			"Growth Spurt doubles the amount of size you can steal during hugs and provides huge resistance against natural shrink sources.\n"
			"Unlock Strong Spurt-related perks to increase the maximum size of Growth Spurt.";
	};
}
