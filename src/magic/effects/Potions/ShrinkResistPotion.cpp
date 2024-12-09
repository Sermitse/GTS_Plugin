#include "managers/GtsManager.hpp"
#include "magic/effects/Potions/ShrinkResistPotion.hpp"
#include "magic/effects/common.hpp"
#include "magic/magic.hpp"
#include "scale/scale.hpp"
#include "data/runtime.hpp"
#include "timer.hpp"
#include "managers/Rumble.hpp"

namespace {
	void PlayGrowthAudio(Actor* giant, bool checkTimer) {
		if (checkTimer) {
			Rumbling::Once("ShrinkResistPotion", giant, 2.0f, 0.05f);
			float Volume = std::clamp(get_visual_scale(giant)/10.0f, 0.20f, 2.0f);
			Runtime::PlaySoundAtNode("growthSound", giant, Volume, 1.0f, "NPC Pelvis [Pelv]");
		}
	}
}

namespace Gts {
	std::string ShrinkResistPotion::GetName() {
		return "ShrinkResistPotion";
	}

	ShrinkResistPotion::ShrinkResistPotion(ActiveEffect* effect) : Magic(effect) {

		auto base_spell = GetBaseEffect();

		if (base_spell == Runtime::GetMagicEffect("ResistShrinkPotionWeak")) {
			this->Resistance = 0.2f;
		} else if (base_spell == Runtime::GetMagicEffect("ResistShrinkPotionNormal")) {
			this->Resistance = 0.4f;
		} else if (base_spell == Runtime::GetMagicEffect("ResistShrinkPotionStrong")) {
			this->Resistance = 0.6f;
		} else if (base_spell == Runtime::GetMagicEffect("ResistShrinkPotionExtreme")) {
			this->Resistance = 0.8f;
		} else if (base_spell == Runtime::GetMagicEffect("AlchResistShrink")) {
			auto active_effect = GetActiveEffect();
			this->Resistance = active_effect->magnitude > 0 ? active_effect->magnitude / 100.0f : 0.8f;
		}
	}

	void ShrinkResistPotion::OnStart() {
		auto caster = GetCaster();
		if (caster) {
			Potion_SetShrinkResistance(caster, this->Resistance);
            log::info("Setting shrink resistance to {}", this->Resistance);
			Potion_Penalty(caster);
		}
	}

	void ShrinkResistPotion::OnFinish() {
		auto caster = GetCaster();
		if (caster) {
			Potion_SetShrinkResistance(caster, 0.0f);
		}
	}
}