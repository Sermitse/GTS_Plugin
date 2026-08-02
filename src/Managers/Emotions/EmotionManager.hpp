#pragma once

// Module that handles facial emotions (phenomes/modifiers/expressions).
//
// Busy-tracking is per (Type, index) - e.g. Phenome #5 and Phenome #0 can
// each be mid-ramp at the same time without one blocking the other, which is
// what lets two different Task_FacialEmotionTask_* calls blend together on
// the same actor. If a NEW call targets an index that's already mid-ramp, it
// doesn't get dropped and doesn't snap - it redirects the existing ramp
// in-place (see EmotionRampState in TransientData.hpp), continuing smoothly
// from whatever that slot's live value currently is toward the new target.
//
// SetEmotionBusy/IsEmotionBusy are managed internally by OverridePhenome/
// OverrideModifier (see EmotionManager.cpp) - callers normally never need to
// touch them directly, they're public mainly for introspection.

namespace GTS {
	class EmotionManager {
		public:
		static void SetEmotionBusy(Actor* giant, CharEmotionType Type, std::uint32_t index, bool lock);
		static bool IsEmotionBusy(Actor* giant, CharEmotionType Type, std::uint32_t index);
		static float GetEmotionValue(Actor* giant, CharEmotionType Type, std::uint32_t emotion_value);
		static void OverridePhenome(Actor* giant, int number, float mfg_speed, float target);
		static void OverrideModifier(Actor* giant, int number, float mfg_speed, float target);
	};
}