#include "managers/animation/Utils/CooldownManager.hpp"
#include "magic/effects/Potions/EssencePotion.hpp"
#include "magic/effects/common.hpp"
#include "magic/magic.hpp"
#include "scale/scale.hpp"
#include "data/persistent.hpp"
#include "data/runtime.hpp"

// A potion that increases max possible size

namespace {
    void shake_screen_do_moan(Actor* giant, float power) {
		if (power >= 0.07f) {
			bool Blocked = IsActionOnCooldown(giant, CooldownSource::Emotion_Moan);
			if (!Blocked) {
				PlayMoanSound(giant, 1.0f);
				ApplyActionCooldown(giant, CooldownSource::Emotion_Moan);
			}
		}

		if (giant->formID == 0x14) {
			float shake = 12.0f * power;
			shake_camera(giant, 0.50f * shake, 0.38f * shake);
		}
    }
}

namespace Gts {
	std::string EssencePotion::GetName() {
		return "EssencePotion";
	}

    EssencePotion::EssencePotion(ActiveEffect* effect) : Magic(effect) {

		auto base_spell = GetBaseEffect();

		if (base_spell == Runtime::GetMagicEffect("EffectEssencePotionWeak")) {
			this->power = 0.02f;
		} else if (base_spell == Runtime::GetMagicEffect("EffectEssencePotionNormal")) {
			this->power = 0.04f;
		} else if (base_spell == Runtime::GetMagicEffect("EffectEssencePotionStrong")) {
			this->power = 0.06f;
		} else if (base_spell == Runtime::GetMagicEffect("EffectEssencePotionExtreme")) {
			this->power = 0.08f; 
		} else if (base_spell == Runtime::GetMagicEffect("AlchEffectEssence")) {
			auto active_effect = GetActiveEffect();
			this->power = active_effect->magnitude > 0 ? active_effect->magnitude / 100.0f : 0.08f;
		}
	}

	void EssencePotion::OnStart() {
		auto caster = GetCaster();

		if (caster) { // player exclusive
			if (caster->formID == 0x14) {
				float scale = get_visual_scale(caster);

				TESGlobal* BonusSize = Runtime::GetGlobal("ExtraPotionSize"); 
				// Bonus size is added on top of all size calculations through this global
				// Applied inside GtsManager.cpp (script)
				if (BonusSize) {
					BonusSize->value += this->power/1.82f; // convert to m
				}

				SpawnCustomParticle(caster, ParticleType::Red, NiPoint3(), "NPC COM [COM ]", scale * ((this->power < 0.10 ? this->power : 0.08f) * 25)); // Just some nice visuals
				shake_screen_do_moan(caster, (this->power < 0.10 ? this->power : 0.08f));
			}
			Potion_Penalty(caster);
        }
	}
}