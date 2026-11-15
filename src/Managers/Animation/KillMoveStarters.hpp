#pragma once

namespace GTS {
	class AnimationKillMoveStarters {
		public:
			static void RegisterEvents();
			static void HandleStandingKillMove(Actor* giant, AimOutcome outcome, bool strong_Attack, bool trample);
	};
}
