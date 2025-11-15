#include "Magic/Effects/Spells/Shrink.hpp"
#include "Magic/Effects/Common.hpp"

namespace GTS {

	std::string Shrink::GetName() {
		return "Shrink";
	}

	void Shrink::OnUpdate() {
		const float BASE_POWER = 0.00360f;
		const float DUAL_CAST_BONUS = 2.0f;
		auto base_spell = GetBaseEffect();
		auto caster = GetCaster();
		if (!caster) {
			return;
		}
		auto GtsSkillLevel = GetGtsSkillLevel(caster);

		float SkillMult = 1.0f + (GtsSkillLevel * 0.01f);

		float power = BASE_POWER * SkillMult;

		float bonus = 1.0f;
		if (Runtime::HasMagicEffect(caster, Runtime::MGEF.GTSPotionEffectSizeAmplify)) {
			bonus = get_visual_scale(caster) * 0.25f + 0.75f;
		}

		if (IsDualCasting()) {
			power *= DUAL_CAST_BONUS;
		}
		if (get_target_scale(caster) > Minimum_Actor_Scale) {
			ShrinkActor(caster, 0.0f, power * bonus);
		} else {
			set_target_scale(caster, Minimum_Actor_Scale);
		}
	}
}
