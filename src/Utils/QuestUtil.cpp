#include "Utils/QuestUtil.hpp"

namespace GTS {

	void SkipProgressionQuest() {

		auto progressionQuest = Runtime::GetQuest(Runtime::QUST.GTSQuestProgression);
		if (progressionQuest) {
			CallFunctionOn(progressionQuest, "GTSProgressionQuest", "Proxy_SkipQuest");
		}
	}

	bool ProgressionQuestCompleted() {
		auto Quest = Runtime::GetQuest(Runtime::QUST.GTSQuestProgression);

		if (Quest) {
			return Quest->GetCurrentStageID() >= 200;
		}

		return false;

	}

	void GiveAllPerksToPlayer() {
		auto Player = PlayerCharacter::GetSingleton();

		for (const auto& Perk : Runtime::PERK.List) {
			if (Perk.first.contains("GTSPerk")) {
				Runtime::AddPerk(Player, *Perk.second);
			}
		}

		auto GtsSkillLevel = Runtime::GetGlobal(Runtime::GLOB.GTSSkillLevel);
		auto GtsSkillRatio = Runtime::GetGlobal(Runtime::GLOB.GTSSkillRatio);
		if (GtsSkillLevel && GtsSkillRatio) {
			GtsSkillLevel->value = 100.0f;
			GtsSkillRatio->value = 0.0f;
		}

		Notify("All perks have been given.");
	}

	void GiveAllSpellsToPlayer() {
		auto Player = PlayerCharacter::GetSingleton();

		for (const auto& Spell: Runtime::SPEL.List) {
			//Do Not Add Survival Mode spells
			if (!Spell.first.contains("Survival_")) {
				Runtime::AddSpell(Player, *Spell.second);
			}
		}

		Notify("All spells have been given.");
	}

	void GiveAllShoutsToPlayer() {
		auto progressionQuest = Runtime::GetQuest(Runtime::QUST.GTSQuestProgression);
		if (progressionQuest) {
			CallFunctionOn(progressionQuest, "GTSProgressionQuest", "Proxy_GetAllShouts");
			Notify("All shouts have been given.");
		}
	}
}