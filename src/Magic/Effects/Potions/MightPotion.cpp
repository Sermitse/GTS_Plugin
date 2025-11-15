#include "Magic/Effects/Potions/MightPotion.hpp"
#include "Magic/Effects/Common.hpp"

namespace GTS {

	std::string MightPotion::GetName() {
		return "MightPotion";
	}

	MightPotion::MightPotion(ActiveEffect* effect) : Magic(effect) {

		auto base_spell = GetBaseEffect();

		if (base_spell == Runtime::GetMagicEffect(Runtime::MGEF.GTSPotionEffectMightExtreme)) {
			this->Power = 0.45f;
		} 
		else if (base_spell == Runtime::GetMagicEffect(Runtime::MGEF.GTSPotionEffectMightStrong)) {
			this->Power = 0.30f;
		} 
		else if (base_spell == Runtime::GetMagicEffect(Runtime::MGEF.GTSPotionEffectMightNormal)) {
			this->Power = 0.20f;
		} 
		else if (base_spell == Runtime::GetMagicEffect(Runtime::MGEF.GTSPotionEffectMightWeak)) {
			this->Power = 0.10f;
		} 
		else if (base_spell == Runtime::GetMagicEffect(Runtime::MGEF.GTSAlchEffectMight)) {
			RecordPotionMagnitude(GetActiveEffect(), this->Power, 0.45f);
		}
	}


	void MightPotion::OnStart() {
		auto caster = GetCaster();
		if (caster) {
			Potion_SetMightBonus(caster, this->Power, true); // Disallow potions to stack
			Potion_Penalty(caster);
		}
	}

	void MightPotion::OnFinish() {
		auto caster = GetCaster();
		if (caster) {
			Potion_SetMightBonus(caster, -this->Power, true);
		}
	}
}
