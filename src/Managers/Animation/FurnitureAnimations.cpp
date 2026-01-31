
#include "Managers/Animation/FurnitureAnimations.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/FurnitureManager.hpp"

using namespace GTS;

namespace {

    void idleChairGetUp(AnimationEventData& data) { // When standing up, leaving chair
        if (IsFemale(&data.giant)) {
            FurnitureManager::GetSingleton().Furniture_EnableButtHitboxes(&data.giant, FurnitureDamageSwitch::DisableDamage);
        }
    }

    void ChairIdle(AnimationEventData& data) { // When sit anim fires, initial impact
        if (IsFemale(&data.giant)) {
		    FurnitureManager::GetSingleton().Furniture_EnableButtHitboxes(&data.giant, FurnitureDamageSwitch::EnableDamage);
        }
	}
}

namespace GTS
{
	void FurnitureAnimations::RegisterEvents() {
		AnimationManager::RegisterEvent("idleChairGetUp", "Furniture", idleChairGetUp);
		AnimationManager::RegisterEvent("ChairIdle", "Furniture", ChairIdle);
	}
}