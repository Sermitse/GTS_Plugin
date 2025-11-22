#pragma once

// Module that handles AttributeAdjustment

namespace GTS {
	class EmotionManager {
		public:
		static void SetEmotionBusy(Actor* giant, CharEmotionType Type, bool lock);
		static bool IsEmotionBusy(Actor* giant, CharEmotionType Type);
		static float GetEmotionValue(Actor* giant, CharEmotionType Type, std::uint32_t emotion_value);
		static void OverridePhenome(Actor* giant, int number, float mfg_speed, float target);
		static void OverrideModifier(Actor* giant, int number, float mfg_speed, float target);
	};
}
