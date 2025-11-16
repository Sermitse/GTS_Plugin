#include "Papyrus/Plugin.hpp"

#include "Config/Config.hpp"

#include "Magic/Effects/Common.hpp"
#include "Utils/Actions/VoreUtils.hpp"

using namespace RE;
using namespace RE::BSScript;

namespace {

	constexpr std::string_view PapyrusClass = "GTSPlugin";

	void ResetQuestProgression(StaticFunctionTag*) {
		GTS::ResetQuest();
	}

	float Quest_GetProgression(StaticFunctionTag*, int stage) {
		return GTS::GetQuestProgression(stage);
	}

	bool WasDragonEaten(StaticFunctionTag*) {
		auto pc = PlayerCharacter::GetSingleton();
		auto transient = GTS::Transient::GetActorData(pc);
		if (transient) {
			return transient->DragonWasEaten;
		}
		return false;
	}

	void CallDevourmentCompatibility(StaticFunctionTag*, Actor* Pred, Actor* Prey, bool Digested) {
		if (GTS::Config::General.bDevourmentCompat) {
			GTS::Devourment_Compatibility(Pred, Prey, Digested);
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
