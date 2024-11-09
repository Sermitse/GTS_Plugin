#pragma once
// Module that handles AttributeValues
#include "events.hpp"

using namespace std;
using namespace SKSE;
using namespace RE;

namespace Gts {

	class ClothManager : public EventListener {
		public:
			[[nodiscard]] static ClothManager& GetSingleton() noexcept;
			virtual std::string DebugName() override;

			void CheckClothingRip(Actor* a_actor);
			bool ShouldPreventReEquip(Actor* a_actor, RE::TESBoundObject* a_object);
			float ReConstructOffset(Actor* a_actor, float scale);
			float ReConstructLastScale(Actor* a_actor);

			const float rip_threshold = 1.5;                //Threshold Singular Clothes get unequiped
			const float rip_tooBig = 2.5;                       //Threshold All Clothes get unequiped
			const float rip_randomOffsetMax = 0.10;

	};
}
