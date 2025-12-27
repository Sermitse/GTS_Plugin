#include "Config/Config.hpp"

namespace GTS {
	
	void CallDevourment(Actor* a_Pred, Actor* a_Prey) {
		auto ProxyQuest = Runtime::GetQuest(Runtime::QUST.GTSQuestProxy);
		const auto& AllowEndo = Config::Gameplay.ActionSettings.bDVDoEndoOnTeam;
		bool DoEndo = false;

		if (AllowEndo && ((IsTeammate(a_Pred) || a_Pred->formID == 0x14) && (IsTeammate(a_Prey) || a_Prey->formID == 0x14))) {
			DoEndo = true;
		}

		if (ProxyQuest) {
			CallVMFunctionOn(ProxyQuest, "GTSProxy", "Proxy_DevourmentForceSwallow", a_Pred, a_Prey, DoEndo);
		}
	}

	void CallVampire() {
		auto progressionQuest = Runtime::GetQuest(Runtime::QUST.GTSQuestProxy);
		if (progressionQuest) {
			CallVMFunctionOn(progressionQuest, "GTSProxy", "Proxy_SatisfyVampire");
		}
	}

	void AddCalamityPerk() {
		auto progressionQuest = Runtime::GetQuest(Runtime::QUST.GTSQuestProxy);
		if (progressionQuest) {
			CallVMFunctionOn(progressionQuest, "GTSProxy", "Proxy_AddCalamityShout");
		}
	}


	//Unused
	void RemoveCalamityPerk() {
		auto progressionQuest = Runtime::GetQuest(Runtime::QUST.GTSQuestProxy);
		if (progressionQuest) {
			CallVMFunctionOn(progressionQuest, "GTSProxy", "Proxy_RemoveCalamityShout");
		}
	}

	//Ported From Papyrus
	void ApplyTalkToActor() {
		static Timer ApplyTalkTimer = Timer(1.0f);

		if (ApplyTalkTimer.ShouldRun()) {

			if (const auto& UtilEnableDialogue = Runtime::GetGlobal("GTSUtilEnableDialogue")) {

				const auto& PlayerCharacter = PlayerCharacter::GetSingleton();

				if (get_visual_scale(PlayerCharacter) < 1.2f) {
					UtilEnableDialogue->value = 0.0f;
					return;
				}

				if (PlayerCharacter->IsSneaking() && UtilEnableDialogue->value < 1.0f) {
					Runtime::GetFloat(Runtime::GLOB.GTSUtilEnableDialogue);
					UtilEnableDialogue->value = 1.0f;
				}
				else if (!PlayerCharacter->IsSneaking() && UtilEnableDialogue->value >= 1.0f) {
					UtilEnableDialogue->value = 0.0f;
				}
			}
		}
	}

	//Ported From Papyrus
	void CheckTalkPerk() {
		static Timer PerkCheckTimer = Timer(30.0f);
		if (PerkCheckTimer.ShouldRun()) {

			const auto& PlayerCharacter = PlayerCharacter::GetSingleton();

			if (!Runtime::HasPerk(PlayerCharacter, "GTSUtilTalkToActor")) {
				Runtime::AddPerk(PlayerCharacter, "GTSUtilTalkToActor");
			}
		}
	}

}
