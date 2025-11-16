#include "Magic/Effects/Potions/ShrinkResistPotion.hpp"
#include "Magic/Effects/Common.hpp"

using namespace GTS;

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