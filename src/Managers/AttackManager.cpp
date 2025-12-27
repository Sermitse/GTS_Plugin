#include "Managers/AttackManager.hpp"

#include "Config/Config.hpp"

using namespace GTS;

namespace {

	void DisableAttacks_Melee(Actor* a_Giant, float a_SizeDiff, float a_Threshold, bool a_Reset) {

		if (a_Reset) {
			a_Giant->GetActorRuntimeData().boolFlags.reset(Actor::BOOL_FLAGS::kAttackingDisabled);
			return;
		}

		const float Random = RandomFloat(0.0f, 20.0f);
		if (a_SizeDiff >= 2.5f && Random <= a_Threshold) {
			a_Giant->GetActorRuntimeData().boolFlags.set(Actor::BOOL_FLAGS::kAttackingDisabled);
		}
		else {
			a_Giant->GetActorRuntimeData().boolFlags.reset(Actor::BOOL_FLAGS::kAttackingDisabled);
		}
	}
	void DisableAttacks_Magic(Actor* a_Giant, float a_SizeDiff, float a_Threshold, bool a_Reset) {

		if (a_Reset) {
			a_Giant->GetActorRuntimeData().boolFlags.reset(Actor::BOOL_FLAGS::kCastingDisabled);
			return;
		}

		const float Random = RandomFloat(0.0f, 20.0f);
		if (a_SizeDiff >= 2.5f && Random <= a_Threshold) {
			a_Giant->GetActorRuntimeData().boolFlags.set(Actor::BOOL_FLAGS::kCastingDisabled);
		}
		else {
			a_Giant->GetActorRuntimeData().boolFlags.reset(Actor::BOOL_FLAGS::kCastingDisabled);
		}
	}
}

namespace GTS {

	void AttackManager::PreventAttacks(Actor* a_Giant, Actor* a_Tiny) {

		if (a_Giant && a_Giant->formID != 0x14 && IsHumanoid(a_Giant)) {

			//If disabled in settings each call to this should always enable instead.
			if (!Config::AI.bDisableAttacks) {
				DisableAttacks_Melee(a_Giant, 0.0f, 0.0f, true);
				DisableAttacks_Magic(a_Giant, 0.0f, 0.0f, true);
				return;
			}

			if (Config::AI.bAlwaysDisableAttacks) { // If this option is on, always prevent attacks past 2.5x scale
				const float VisualScale = get_visual_scale(a_Giant);
				constexpr float Threshold = 2.5f;
				if (VisualScale >= Threshold) {
					// past threshold, disable all attacks
					a_Giant->GetActorRuntimeData().boolFlags.set(Actor::BOOL_FLAGS::kAttackingDisabled);
					a_Giant->GetActorRuntimeData().boolFlags.set(Actor::BOOL_FLAGS::kCastingDisabled);
				} else {
					// let RNG decide
					DisableAttacks_Melee(a_Giant, VisualScale, Threshold, false);
					DisableAttacks_Magic(a_Giant, VisualScale, Threshold, false);
				}
				return;
			}

			if (a_Tiny) {

				const float SizeDiff = get_scale_difference(a_Giant, a_Tiny, SizeType::VisualScale, true, false);
				const float Threshold = 2.5f * (SizeDiff - 2.5f);
				DisableAttacks_Melee(a_Giant, SizeDiff, Threshold, false);
				DisableAttacks_Magic(a_Giant, SizeDiff, Threshold, false);

			}
			// If Tiny is nullptr, we count it as 'enable attacks back'
			else {

				DisableAttacks_Melee(a_Giant, 0.0f, 0.0f, true);
				DisableAttacks_Magic(a_Giant, 0.0f, 0.0f, true);

			}
		}
	}
}
