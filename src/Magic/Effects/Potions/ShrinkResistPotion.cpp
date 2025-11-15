#include "Magic/Effects/Potions/ShrinkResistPotion.hpp"
#include "Magic/Effects/Common.hpp"
#include "Managers/Rumble.hpp"

using namespace GTS;

namespace {
	void PlayGrowthAudio(Actor* giant, bool checkTimer) {
		if (checkTimer) {
			Rumbling::Once("ShrinkResistPotion", giant, 2.0f, 0.05f);
			float Volume = std::clamp(get_visual_scale(giant)/10.0f, 0.20f, 2.0f);
			Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundGrowth, giant, Volume, "NPC Pelvis [Pelv]");
		}
	}
}

namespace GTS {
	std::string ShrinkResistPotion::GetName() {
		return "ShrinkResistPotion";
	}

	ShrinkResistPotion::ShrinkResistPotion(ActiveEffect* effect) : Magic(effect) {

		auto base_spell = GetBaseEffect();

		if (base_spell == Runtime::GetMagicEffect(Runtime::MGEF.GTSPotionEffectResistShrinkWeak)) {
			this->Resistance = 0.2f;
		} 
		else if (base_spell == Runtime::GetMagicEffect(Runtime::MGEF.GTSPotionEffectResistShrinkNormal)) {
			this->Resistance = 0.4f;
		} 
		else if (base_spell == Runtime::GetMagicEffect(Runtime::MGEF.GTSPotionEffectResistShrinkStrong)) {
			this->Resistance = 0.6f;
		} 
		else if (base_spell == Runtime::GetMagicEffect(Runtime::MGEF.GTSPotionEffectResistShrinkExtreme)) {
			this->Resistance = 0.8f;
		} 
		else if (base_spell == Runtime::GetMagicEffect(Runtime::MGEF.GTSAlchEffectResistShrink)) {
			RecordPotionMagnitude(GetActiveEffect(), this->Resistance, 0.8f);
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