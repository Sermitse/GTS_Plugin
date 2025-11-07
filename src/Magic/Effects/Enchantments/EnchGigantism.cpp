#include "Magic/Effects/Enchantments/EnchGigantism.hpp"
#include "Managers/GtsSizeManager.hpp"

namespace GTS {

	std::string Gigantism::GetName() {
		return "Gigantism";
	}

	void Gigantism::OnStart() {
		auto caster = GetCaster();

		if (!caster) {
			return;
		}
		this->magnitude = GetActiveEffect()->magnitude;
		float GigantismPower = this->magnitude;
		SizeManager::GetSingleton().ModEnchantmentBonus(caster, GigantismPower);

		if (!Persistent::MSGSeenAspectOfGTS.value) {
			PrintMessageBox(AOGGuide);
			Persistent::MSGSeenAspectOfGTS.value = true;
		}

	}

	void Gigantism::OnFinish() {
		auto caster = GetCaster();
		if (!caster) {
			return;
		}
		float GigantismPower = this->magnitude;
		SizeManager::GetSingleton().ModEnchantmentBonus(caster, -GigantismPower);
	}
}
