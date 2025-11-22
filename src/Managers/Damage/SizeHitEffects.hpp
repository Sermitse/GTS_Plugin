#pragma once

namespace GTS {

	class SizeHitEffects {
		public:
		static void ApplyEverything(Actor* attacker, Actor* receiver, float damage);
		static void PerformInjuryDebuff(Actor* giant, Actor* tiny, float damage, int random);
	};
}