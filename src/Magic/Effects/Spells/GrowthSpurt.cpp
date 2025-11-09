#include "Magic/Effects/Spells/GrowthSpurt.hpp"
#include "Magic/Effects/Common.hpp"
#include "Managers/GtsSizeManager.hpp"
#include "Managers/Rumble.hpp"

using namespace GTS;

namespace {

	void PlayShrinkAudio(Actor* actor, bool timer_1, bool timer_2, float power) {
		float scale = get_visual_scale(actor);
		float falloff = 0.18f * scale;
		Rumbling::Once("GrowthSpurt", actor, Rumble_Shrink_GrowthSpurt, 0.05f);
		if (timer_1) {
			Runtime::PlaySoundAtNode_FallOff("GTSSoundRumble", actor, power/20, "NPC Pelvis [Pelv]", falloff);
		}
		if (timer_2) {
			float Volume = std::clamp(get_visual_scale(actor) * 0.10f, 0.10f, 1.0f);
			Runtime::PlaySoundAtNode_FallOff("GTSSoundShrink", actor, Volume, "NPC Pelvis [Pelv]", falloff);
		}
	}

	void PlayGrowthAudio(Actor* actor, bool timer_1, bool timer_2, float power) {
		float scale = get_visual_scale(actor);
		float falloff = 0.18f * scale;
		Rumbling::Once("GrowthSpurt", actor, Rumble_Growth_GrowthSpurt, 0.05f);
		if (timer_1) {
			Runtime::PlaySoundAtNode_FallOff("GTSSoundRumble", actor, power/20, "NPC Pelvis [Pelv]", falloff);
		}
		if (timer_2) {
			float Volume = std::clamp(scale * 0.10f, 0.20f, 1.0f);
			Runtime::PlaySoundAtNode_FallOff("GTSSoundGrowth", actor, Volume, "NPC Pelvis [Pelv]", falloff);
		}
	}

	float Get_Perk_Bonus(Actor* giant) {
		float bonus = 1.0f;
		float basic = 0.0f;
		
		if (Runtime::HasPerk(giant, "GTSPerkExtraGrowth1")) {
			basic += 0.50f;
		}
		if (Runtime::HasPerk(giant, "GTSPerkExtraGrowth2")) {
			float perkbonus = 1.0f + ((GetGtsSkillLevel(giant) * 0.015f) + (giant->GetLevel() * 0.030f));
			basic *= perkbonus;
		}
		return bonus + basic;
	}

	float Get_Growth_Limit(Actor* giant, int rank) {
		float basic = 1.0f * get_natural_scale(giant, true);
		float bonus = 0.0f;

		if (rank == 1) {
			bonus = 0.25f;
		} else if (rank == 2) {
			bonus = 0.50f;
		} else if (rank == 3) {
			bonus = 1.0f;
		}

		bonus *= Get_Perk_Bonus(giant);
		bonus *= get_natural_scale(giant, true);
		return basic + bonus;
	}

	void GrowthSpurt_RegenerateAttributes(Actor* caster) {
		float HpRegen = GetMaxAV(caster, ActorValue::kHealth) * 0.00020f;
		
		if (Runtime::HasPerk(caster, "GTSPerkGrowthAug2")) {
			HpRegen *= 2.0f;
		}

		caster->AsActorValueOwner()->RestoreActorValue(ACTOR_VALUE_MODIFIER::kDamage, ActorValue::kHealth, HpRegen * TimeScale());
	}
}

namespace GTS {

	std::string GrowthSpurt::GetName() {
		return "GrowthSpurt";
	}

	GrowthSpurt::GrowthSpurt(ActiveEffect* effect) : Magic(effect) {
	}

	void GrowthSpurt::OnStart() {
		Actor* caster = GetCaster();
		if (!caster) {
			return;
		}

		if (!Persistent::MSGSeenGrowthSpurt.value) {
			PrintMessageBox(GrowthSpurtMessage);
			Persistent::MSGSeenGrowthSpurt.value = true;
		}

		this->AllowStacking = true;
	}

	void GrowthSpurt::OnUpdate() {
		Actor* caster = GetCaster();
		if (!caster) {
			return;
		}
		constexpr float GROWTH_1_POWER = 0.00125f;
		constexpr float GROWTH_2_POWER = 0.00145f;
		constexpr float GROWTH_3_POWER = 0.00175f;

		auto base_spell = GetBaseEffect();

		
		if (base_spell == Runtime::GetMagicEffect("GTSEffectGrowthSpurt1")) {
			this->power = GROWTH_1_POWER * Get_Perk_Bonus(caster);
			this->grow_limit = Get_Growth_Limit(caster, 1);
		} else if (base_spell == Runtime::GetMagicEffect("GTSEffectGrowthSpurt2")) {
			this->power = GROWTH_2_POWER * Get_Perk_Bonus(caster);
			this->grow_limit = Get_Growth_Limit(caster, 2);
		} else if (base_spell == Runtime::GetMagicEffect("GTSEffectGrowthSpurt3")) {
			this->power = GROWTH_3_POWER * Get_Perk_Bonus(caster);
			this->grow_limit = Get_Growth_Limit(caster, 3);
		}

		float Gigantism = 1.0f + Ench_Aspect_GetPower(caster);
		float scale = get_target_scale(caster);

		float bonus = 1.0f;
		float limit = this->grow_limit * Gigantism;
		float MaxSize = get_max_scale(caster) - 0.004f;
		

		GrowthSpurt_RegenerateAttributes(caster);

		if (scale < limit && scale < MaxSize) {
			if (Runtime::HasMagicEffect(caster, "GTSPotionEffectSizeAmplify")) {
				bonus = get_visual_scale(caster) * 0.25f + 0.75f;
			}
			DoGrowth(caster, this->power * bonus);
		}
	}

	void GrowthSpurt::OnFinish() {
		Actor* caster = GetCaster();
		if (!caster) {
			return;
		}
		GrowthSpurt::DoShrink(caster);
	}

	void GrowthSpurt::DoGrowth(Actor* actor, float value) {
		float appliedscale = get_update_target_scale(actor, value, SizeEffectType::kGrow); // Grow
		if (SizeManager::GetSingleton().GetGrowthSpurt(actor) < (this->grow_limit - get_natural_scale(actor, true))) {
			if (this->AllowStacking) {
				SizeManager::GetSingleton().ModGrowthSpurt(actor, appliedscale);
			}
		} else {
			this->AllowStacking = false;
		}
		PlayGrowthAudio(actor, this->timer.ShouldRun(), this->timerSound.ShouldRunFrame(), this->power);
	}

	void GrowthSpurt::DoShrink(Actor* actor) {
		float value = SizeManager::GetSingleton().GetGrowthSpurt(actor);
		float naturalscale = get_natural_scale(actor, true);
		update_target_scale(actor, -value, SizeEffectType::kNeutral); // Do Shrink
		if (get_target_scale(actor) <= naturalscale) {
			set_target_scale(actor, naturalscale);
		}

		SizeManager::GetSingleton().SetGrowthSpurt(actor, 0.0f);

		this->AllowStacking = true;
		PlayShrinkAudio(actor, this->timer.ShouldRun(), this->timerSound.ShouldRunFrame(), this->power);
	}
}
