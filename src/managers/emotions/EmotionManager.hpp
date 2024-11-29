#pragma once
// Module that handles AttributeAdjustment
#include "events.hpp"
#include "spring.hpp"

using namespace std;
using namespace SKSE;
using namespace RE;

namespace Gts {
	class EmotionManager : public EventListener {
		public:
			[[nodiscard]] static EmotionManager& GetSingleton() noexcept;
			

			virtual std::string DebugName() override;

			static void SetEmotionBusy(Actor* giant, CharEmotionType Type, bool lock);
			static bool IsEmotionBusy(Actor* giant, CharEmotionType Type);

			static float GetEmotionValue(Actor* giant, CharEmotionType Type, std::uint32_t emotion_value);
			void OverridePhenome(Actor* giant, int number, float halflife, float target);
			void OverrideModifier(Actor* giant, int number, float halflife, float target);
	};
}
