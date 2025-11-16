#include "Magic/Effects/Potions/SizeHunger.hpp"
#include "Magic/Effects/Common.hpp"
#include "Managers/GtsSizeManager.hpp"

// A potion that amplifies size gain

namespace GTS {

	std::string SizeHunger::GetName() {
		return "SizeHunger";
	}

	void SizeHunger::OnStart() {

		auto caster = GetCaster();
		if (!caster) {
			return;
		}

		float Power = GetActiveEffect()->magnitude;
		SizeManager::GetSingleton().SetSizeHungerBonus(caster, Power);
		Potion_Penalty(caster);

	}

	void SizeHunger::OnFinish() {

		auto caster = GetCaster();
		if (!caster) {
			return;
		}

		float Power = SizeManager::GetSingleton().GetSizeHungerBonus(caster);
		SizeManager::GetSingleton().ModSizeHungerBonus(caster, -Power);
	}
}
