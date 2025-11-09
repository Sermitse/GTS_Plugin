
#include "Magic/Effects/Common.hpp"
#include "Magic/Effects/Spells/Shrink_Other.hpp"

namespace GTS {

	std::string ShrinkOther::GetName() {
		return "ShrinkOther";
	}

	void ShrinkOther::OnUpdate() {

		constexpr float BASE_POWER = 0.00180f;
		constexpr float DUAL_CAST_BONUS = 2.0f;

		auto caster = GetCaster();
		if (!caster) {
			return;
		}
		auto target = GetTarget();
		if (!target) {
			return;
		}

		float power = BASE_POWER;

		auto base_spell = GetBaseEffect();

		if (base_spell == Runtime::GetMagicEffect("GTSEffectShrinkAllyAdept")) {
			power *= 1.33f;
		} else if (base_spell == Runtime::GetMagicEffect("GTSEffectShrinkAllyExpert")) {
			power *= 1.75f;
		}

		float target_scale = get_visual_scale(target);

		if (IsDualCasting()) {
			power *= DUAL_CAST_BONUS;
		}

		if (target_scale > Minimum_Actor_Scale) {
			if (!IsHostile(target, caster)) {
				ShrinkActor(target, power*0.10f, 0);
			}
		} else {
			set_target_scale(target, Minimum_Actor_Scale);
		}
	}
}
