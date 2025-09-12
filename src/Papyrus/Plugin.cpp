#include "Papyrus/Plugin.hpp"
#include "Data/Transient.hpp"
#include "Magic/Effects/Common.hpp"
#include "Managers/Gamemode/GameModeManager.hpp"
#include "Utils/VoreUtils.hpp"

using namespace GTS;
using namespace RE::BSScript;

namespace {

	constexpr std::string_view PapyrusClass = "GTSPlugin";

	void ResetQuestProgression(StaticFunctionTag*) {
		ResetQuest();
	}

	float Quest_GetProgression(StaticFunctionTag*, int stage) {
		return GetQuestProgression(stage);
	}

	bool WasDragonEaten(StaticFunctionTag*) {
		auto pc = PlayerCharacter::GetSingleton();
		auto transient = Transient::GetSingleton().GetData(pc);
		if (transient) {
			return transient->DragonWasEaten;
		}
		return false;
	}

	void CallDevourmentCompatibility(StaticFunctionTag*, Actor* Pred, Actor* Prey, bool Digested) {
		if (Config::General.bDevourmentCompat) {
			Devourment_Compatibility(Pred, Prey, Digested);
		}
	}
}

namespace GTS {

	bool register_papyrus_plugin(IVirtualMachine* vm) {

		//Quest
		vm->RegisterFunction("ResetQuestProgression", PapyrusClass, ResetQuestProgression);
		vm->RegisterFunction("Quest_GetProgression", PapyrusClass, Quest_GetProgression);
		vm->RegisterFunction("WasDragonEaten", PapyrusClass, WasDragonEaten);

		//Devourment
		vm->RegisterFunction("CallDevourmentCompatibility", PapyrusClass, CallDevourmentCompatibility);

		return true;
	}
}
