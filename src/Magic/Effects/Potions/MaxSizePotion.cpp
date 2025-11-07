#include "Magic/Effects/Potions//MaxSizePotion.hpp"
#include "Magic/Effects/Common.hpp"

using namespace GTS;

// A potion that TEMPORARILY increases max possible size

namespace {

	void TempBonusMaxSize_Modify(Actor* giant, float value) {
		auto saved_data = Transient::GetActorData(giant);
		if (saved_data) {
			saved_data->PotionMaxSize += value;
		}
	}

}

namespace GTS {

	std::string MaxSizePotion::GetName() {
		return "MaxSizePotion";
	}


	MaxSizePotion::MaxSizePotion(ActiveEffect* effect) : Magic(effect) {

		auto base_spell = GetBaseEffect();

		if (base_spell == Runtime::GetMagicEffect("GTSPotionEffectSizeLimitExtreme")) {
			this->Power = 0.35f;
		} else if (base_spell == Runtime::GetMagicEffect("GTSPotionEffectSizeLimitStrong")) {
			this->Power = 0.20f;
		} else if (base_spell == Runtime::GetMagicEffect("GTSPotionEffectSizeLimitNormal")) {
			this->Power = 0.15f;
		} else if (base_spell == Runtime::GetMagicEffect("GTSPotionEffectSizeLimitWeak")) {
			this->Power = 0.10f;
		} else if (base_spell == Runtime::GetMagicEffect("GTSAlchEffectSizeLimit")) {
			RecordPotionMagnitude(GetActiveEffect(), this->Power, 0.35f);
		}
	}


	void MaxSizePotion::OnStart() {
		auto caster = GetCaster();
		if (caster) {
			TempBonusMaxSize_Modify(caster, this->Power);
			Potion_Penalty(caster);
		}
	}

	void MaxSizePotion::OnFinish() {
		auto caster = GetCaster();
		if (caster) {
			TempBonusMaxSize_Modify(caster, -this->Power);
		}
	}
}
