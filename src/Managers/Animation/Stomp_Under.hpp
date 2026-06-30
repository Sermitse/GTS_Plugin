#pragma once

namespace GTS {

	class AnimationUnderStomp {
		public:
			static bool AutoAim_Foot_Directional(Actor* giant, bool left_foot, bool forward_only);
            static bool ShouldPerformUnderStomp(Actor* giant, bool autoAim = false, bool left = false);
			static bool CrosshairUnderstomp(Actor* giant);

			static void RegisterEvents();

			static void RegisterTriggers();


			static inline float Remap(float x, float in_min, float in_max, float out_min, float out_max) {
				return out_min + (x - in_min) * (out_max - out_min) / (in_max - in_min);
			}

	};
}