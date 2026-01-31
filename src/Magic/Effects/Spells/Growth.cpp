#include "Magic/Effects/Spells/Growth.hpp"
#include "Magic/Effects/Common.hpp"

namespace GTS {

	std::string Growth::GetName() {
		return "Growth";
	}

	void Growth::OnUpdate() {

		constexpr float BASE_POWER = 0.00150f;
		constexpr float DUAL_CAST_BONUS = 2.0f;

		auto caster = GetCaster();
		if (!caster) {
			return;
		}

		float GtsSkillLevel = GetGtsSkillLevel(caster);

		float SkillMult = 1.0f + (GtsSkillLevel * 0.01f);
		float HpRegen = GetMaxAV(caster, ActorValue::kHealth) * 0.00005f;

		float bonus = 1.0f;

		if (Runtime::HasMagicEffect(caster, Runtime::MGEF.GTSPotionEffectSizeAmplify)) {
			bonus = get_visual_scale(caster) * 0.25f + 0.75f;
		}

		float power = BASE_POWER * SkillMult;

		auto base_spell = GetBaseEffect();

		if (base_spell == Runtime::GetMagicEffect(Runtime::MGEF.GTSEffectGrowthAdept)) {
			power *= 1.33f;
		} else if (base_spell == Runtime::GetMagicEffect(Runtime::MGEF.GTSEffectGrowthExpert)) {
			power *= 1.75f;
			caster->AsActorValueOwner()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, ActorValue::kHealth, HpRegen * TimeScale());
		}


		if (IsDualCasting()) {
			power *= DUAL_CAST_BONUS;
		}

		Grow(caster, 0.0f, power * bonus);


		float mult = 0.15f;
		if (caster->IsInCombat()) {
			mult = 1.0f;
		}

		AdvanceSkill(caster, ActorValue::kAlteration, power * bonus, 1.0f); // Gain vanilla Alteration xp
	}
}
