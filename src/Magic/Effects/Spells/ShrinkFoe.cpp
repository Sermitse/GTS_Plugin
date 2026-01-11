#include "Magic/Effects/Common.hpp"
#include "Magic/Effects/Spells/ShrinkFoe.hpp"
#include "Managers/GTSSizeManager.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"

using namespace GTS;

namespace {

	void ResetMovementSlowdown(Actor* tiny) {
		auto transient = Transient::GetActorData(tiny);
		if (transient) {
			transient->MovementSlowdown = 1.0f;
		}
	}

	void SetMovementSlowdown(Actor* giant, Actor* tiny) { 
		auto transient = Transient::GetActorData(tiny);
		if (transient) {
			float slow = 0.75f;
			if (Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkShrinkAdept)) {
				slow += 0.05f;
			}
			if (Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkShrinkExpert)) {
				slow += 0.05f;
			}
			transient->MovementSlowdown = slow;
		}
	}
}


namespace GTS {

	std::string ShrinkFoe::GetName() {
		return "ShrinkFoe";
	}

	ShrinkFoe::ShrinkFoe(ActiveEffect* effect) : Magic(effect) {

		constexpr float SHRINK_POWER = 2.15f; // Power = Shrink Power
		constexpr float SHRINK_EFFIC = 0.16f; // Efficiency = size steal efficiency.
		constexpr float SHRINK_AOE_POWER = 2.40f;
		constexpr float SHRINK_AOE_EFFIC = 0.18f;
		constexpr float SHRINK_AOE_MASTER_POWER = 2.70f;
		constexpr float SHRINK_AOE_MASTER_EFFIC = 0.20f;
		constexpr float SHRINK_BOLT_POWER = 6.0f;
		constexpr float SHRINK_BOLT_EFFIC = 0.06f;
		constexpr float SHRINK_STORM_POWER = 12.0f;
		constexpr float SHRINK_STORM_EFFIC = 0.03f;

		auto base_spell = GetBaseEffect();

		if (base_spell == Runtime::GetMagicEffect(Runtime::MGEF.GTSEffectShrinkEnemy)) {
			this->power = SHRINK_POWER;
			this->efficiency = SHRINK_EFFIC;
		} 
		else if (base_spell == Runtime::GetMagicEffect(Runtime::MGEF.GTSEffectShrinkEnemyAOE)) {
			this->power = SHRINK_AOE_POWER;
			this->efficiency = SHRINK_AOE_EFFIC;
		} 
		else if (base_spell == Runtime::GetMagicEffect(Runtime::MGEF.GTSEffectShrinkOtherAOEMastery)) {
			// ShrinkEnemyAOEMast
			this->power = SHRINK_AOE_MASTER_POWER;
			this->efficiency = SHRINK_AOE_MASTER_EFFIC;
		} 
		else if (base_spell == Runtime::GetMagicEffect(Runtime::MGEF.GTSEffectShrinkBolt)) {
			// ShrinkBolt
			this->power = SHRINK_BOLT_POWER;
			this->efficiency = SHRINK_BOLT_EFFIC;
			this->time_mult = 0.2f;
		} 
		else if (base_spell == Runtime::GetMagicEffect(Runtime::MGEF.GTSEffectShrinkStorm)) {
			// ShrinkBolt
			this->power = SHRINK_STORM_POWER;
			this->efficiency = SHRINK_STORM_EFFIC;
			this->time_mult = 0.2f;
		}
	}

	void ShrinkFoe::OnStart() {
		auto caster = GetCaster();
		if (!caster) {
			return;
		}
		auto target = GetTarget();
		if (!target) {
			return;
		}

		if (!IsEssential_WithIcons(caster, target)) {
			float sizediff = get_scale_difference(caster, target, SizeType::VisualScale, true, false);
			if (this->power >= 18.00f && sizediff > 4.0f) {
				StaggerActor(caster, target, 100.0f);
			}
			SetMovementSlowdown(caster, target);
		}
	}

	void ShrinkFoe::OnUpdate() {
		auto caster = GetCaster();
		if (!caster) {
			return;
		}
		auto target = GetTarget();
		if (!target) {
			return;
		}
		if (caster == target) {
			return;
		}

		if (IsEssential(caster, target)) {
			return; // Disallow shrinking Essentials
		}

		float SizeDifference = 1.0f;
		float bonus = 1.0f;
		float balancemodebonus = 1.0f;
		float shrink = this->power * 3.2f;
		float gainpower = this->efficiency;
		auto actor_data = Persistent::GetActorData(target);
		
		if (this->power >= 18.00f) {
			if (actor_data) {
				actor_data->fHalfLife = 0.25f; // Faster shrink, less smooth.
			}
			SizeDifference = 1.0f / std::clamp((get_visual_scale(target) * GetSizeFromBoundingBox(target)), 0.25f, 1.0f);
		} 
		else if (this->power >= 10.0f) {
			if (actor_data) {
				actor_data->fHalfLife = 0.50f; // Faster shrink, less smooth.
			}
			SizeDifference = 1.0f / std::clamp((get_visual_scale(target) * GetSizeFromBoundingBox(target)), 0.50f, 1.0f);
		} 
		else {
			if (actor_data) {
				actor_data->fHalfLife = 1.0f;
			}
		}

		if (target->IsDead()) {
			bonus = 2.5f;
			gainpower *= 0.20f;
		}

		if (caster->formID == 0x14 && SizeManager::BalancedMode()) { // This is checked only if Balance Mode is enabled.
			balancemodebonus = 0.5f;
		}

		float HealthPercent = std::clamp(GetHealthPercentage(target), 0.25f, 1.0f);
		shrink /= HealthPercent;

		target->Attacked(caster); // make it work like a hostile spell

		ChanceToScare(caster, target, 5.0f, 1200, true);

		if (ShrinkToNothing(caster, target, true, this->time_mult)) { // STN when size difference is met and when time ticks allow to

		} 
		else {
			if (get_target_scale(target) >= SHRINK_TO_NOTHING_SCALE / GetSizeFromBoundingBox(target)) {
				TransferSize(caster, target, IsDualCasting(), shrink * SizeDifference * bonus, gainpower * balancemodebonus, false, ShrinkSource::Magic);
			}
		}
	}

	void ShrinkFoe::OnFinish() {

		auto caster = GetCaster();
		if (!caster) {
			return;
		}

		auto target = GetTarget();
		if (!target) {
			return;
		}

		if (!IsEssential(caster, target)) {
			Task_TrackSizeTask(caster, target, "Spell", true, this->time_mult);
			ResetMovementSlowdown(target);
		}
	}
}
