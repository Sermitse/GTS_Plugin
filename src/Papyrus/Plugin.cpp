#include "Papyrus/Plugin.hpp"
#include "Utils/Text/Text.hpp"

#include "Config/Config.hpp"

#include "Magic/Effects/Common.hpp"
#include "Utils/Actions/VoreUtils.hpp"

#include "Managers/AI/Vore/VoreAI.hpp"
#include "Managers/AI/Vore/DevourmentAI.hpp"
#include "Managers/AI/Thigh/ThighCrushAI.hpp"
#include "Managers/AI/ButtCrush/ButtCrushAI.hpp"
#include "Managers/AI/Thigh/ThighSandwichAI.hpp"
#include "Managers/AI/StompKick/StompKickSwipeAI.hpp"
#include "Managers/Animation/Grab.hpp"
#include "Managers/Animation/HugShrink.hpp"
#include "Managers/Animation/Controllers/VoreController.hpp"

#include "Managers/AI/Grab/GrabAI.hpp"
#include "Managers/AI/Hug/HugAI.hpp"

using namespace RE;
using namespace RE::BSScript;
using namespace GTS;

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

	void ForceStartSizeInteraction(StaticFunctionTag*, Actor* Pred, Actor* Prey, int Type) {
		// 0 = Vore
		// 1 = Devourment (Only with Devourment)
		// 2 = Stomp
		// 3 = Kicks
		// 4 = Thigh Sandwich
		// 5 = Thigh Crush
		// 6 = Butt Crush
		// 7 = Hugs
		// 8 = Grab
		if (Pred && Prey) {
			switch (Type) {
				case 0: VoreAI_StartVore(Pred, std::vector<Actor*> {Prey}); 			break;
				case 1: DevourmentAI_Start(Pred, std::vector<Actor*> {Prey});			break;
				case 2: StompAI_Start(Pred, Prey);										break;
				case 3: KickSwipeAI_Start(Pred);										break;
				case 4: ThighSandwichAI_Start(Pred, std::vector<Actor*> {Prey});		break;
				case 5: ThighCrushAI_Start(Pred);										break;
				case 6: ButtCrushAI_Start(Pred, Prey);									break;
				case 7: HugAI_Start(Pred, Prey);										break;
				case 8: GrabAI_Start(Pred, Prey);										break;
			}
		} else {
			if (!Pred) {
				Notify("Pred is null");
			}
			if (!Prey) {
				Notify("Prey is null");
			}
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
		vm->RegisterFunction("ForceStartSizeInteraction", PapyrusClass, ForceStartSizeInteraction);

		return true;
	}
}
