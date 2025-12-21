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


	// void CompleteDragonQuest(Actor* tiny, ParticleType Type, bool dead) {
	// 	auto pc = PlayerCharacter::GetSingleton();
	// 	auto progressionQuest = Runtime::GetQuest(Runtime::QUST.GTSQuestProgression);
	// 	if (progressionQuest) {
	// 		auto stage = progressionQuest->GetCurrentStageID();
	// 		if (stage == 80) {
	// 			auto transient = Transient::GetActorData(pc);
	// 			if (transient) {
	// 				Cprint("Quest is Completed");
	// 				transient->DragonWasEaten = true;
	// 				SpawnCustomParticle(tiny, Type, NiPoint3(), "NPC Root [Root]", 1.0f);
	// 			}
	// 		}
	// 	}
	// }

	// void ResetQuest() {
	// 	Persistent::HugStealCount.value = 0.0f;
	// 	Persistent::StolenSize.value = 0.0f;
	// 	Persistent::CrushCount.value = 0.0f;
	// 	Persistent::STNCount.value = 0.0f;
	// 	Persistent::HandCrushed.value = 0.0f;
	// 	Persistent::VoreCount.value = 0.0f;
	// 	Persistent::GiantCount.value = 0.0f;
	// }
	//
	// float GetQuestProgression(int stage) {
	// 	QuestStage Stage = static_cast<QuestStage>(stage);
	//
	// 	switch (Stage) {
	// 	case QuestStage::HugSteal: 				// Stage 0: hug steal 2 meters of size
	// 		return Persistent::HugStealCount.value;
	// 	case QuestStage::HugSpellSteal: 		// Stage 1: hug/spell steal 5 meters of size
	// 		return Persistent::StolenSize.value;
	// 	case QuestStage::Crushing: 				// Stage 2: Crush 3 (*4 if dead) enemies
	// 		return Persistent::CrushCount.value;
	// 	case QuestStage::ShrinkToNothing:  		// Stage 3: Crush or Shrink to nothing 6 enemies in total
	// 		return Persistent::CrushCount.value - 3.0f + Persistent::STNCount.value;
	// 	case QuestStage::HandCrush: 			// Stage 4: hand crush 3 enemies
	// 		return Persistent::HandCrushed.value;
	// 	case QuestStage::Vore: 					// Stage 5: Vore 6 enemies
	// 		return Persistent::VoreCount.value;
	// 	case QuestStage::Giant:					// Stage 6: Vore/crush/shrink a Giant
	// 		return Persistent::GiantCount.value;
	// 		break;
	// 	}
	// 	return 0.0f;
	// }
	//
	// void AdvanceQuestProgression(Actor* giant, Actor* tiny, QuestStage stage, float value, bool vore) {
	// 	if (giant->formID == 0x14) { // Player Only
	// 		auto progressionQuest = Runtime::GetQuest(Runtime::QUST.GTSQuestProgression);
	//
	// 		if (progressionQuest) {
	// 			auto queststage = progressionQuest->GetCurrentStageID();
	//
	// 			if (queststage >= 100 || queststage < 10) {
	// 				return;
	// 			}
	//
	// 			switch (stage) {
	// 			case QuestStage::HugSteal: 				// Stage 0: hug steal 2 meters of size
	// 				Persistent::HugStealCount.value += value;
	// 				break;
	// 			case QuestStage::HugSpellSteal:			// Stage 1: hug/spell steal 5 meters of size
	// 				if (queststage == 20) {
	// 					Persistent::StolenSize.value += value;
	// 				}
	// 				break;
	// 			case QuestStage::Crushing:				// Stage 2: Crush 3 (*4 if dead) enemies
	// 				if (queststage >= 30 && queststage <= 40) {
	// 					Persistent::CrushCount.value += value;
	// 					if (value < 1) {
	// 						SpawnCustomParticle(tiny, ParticleType::DarkRed, NiPoint3(), "NPC Root [Root]", 1.0f);
	// 					}
	// 					else {
	// 						SpawnCustomParticle(tiny, ParticleType::Red, NiPoint3(), "NPC Root [Root]", 1.0f);
	// 					}
	//
	// 					float progression = GetQuestProgression(static_cast<int>(QuestStage::Crushing));
	// 					float goal = 3.0f;
	// 					if (queststage == 40) { // Print this if in STN stage
	// 						progression = GetQuestProgression(static_cast<int>(QuestStage::ShrinkToNothing));
	// 						goal = 6.0f;
	// 					}
	// 					Notify("Progress: {:.1f}/{:.1f}", progression, goal);
	// 				}
	// 				break;
	// 			case QuestStage::ShrinkToNothing:		// Stage 3: Crush or Shrink to nothing 6 enemies in total
	// 				if (queststage == 40) {
	// 					Persistent::STNCount.value += value;
	// 					if (value < 1) {
	// 						SpawnCustomParticle(tiny, ParticleType::DarkRed, NiPoint3(), "NPC Root [Root]", 1.0f);
	// 					}
	// 					else {
	// 						SpawnCustomParticle(tiny, ParticleType::Red, NiPoint3(), "NPC Root [Root]", 1.0f);
	// 					}
	// 					Notify("Progress: {:.1f}/{:.1f}", GetQuestProgression(static_cast<int>(QuestStage::ShrinkToNothing)), 6.0f);
	// 				}
	// 				break;
	// 			case QuestStage::HandCrush:				// Stage 4: hand crush 3 enemies
	// 				Persistent::HandCrushed.value += value;
	// 				SpawnCustomParticle(tiny, ParticleType::Red, NiPoint3(), "NPC Root [Root]", 1.0f);
	// 				Notify("Progress: {:.1f}/{:.1f}", GetQuestProgression(static_cast<int>(QuestStage::HandCrush)), 3.0f);
	// 				break;
	// 			case QuestStage::Vore:					// Stage 5: Vore 6 enemies
	// 				if (queststage == 60) {
	// 					Persistent::VoreCount.value += value;
	// 					SpawnCustomParticle(tiny, ParticleType::Blue, NiPoint3(), "NPC Root [Root]", 1.0f);
	// 					Notify("Progress: {:.1f}/{:.1f}", GetQuestProgression(static_cast<int>(QuestStage::Vore)), 6.0f);
	// 				}
	// 				break;
	// 			case QuestStage::Giant:					// Stage 6: Vore/crush/shrink a Giant
	// 				Persistent::GiantCount.value += value;
	// 				if (vore) {
	// 					SpawnCustomParticle(tiny, ParticleType::Blue, NiPoint3(), "NPC Root [Root]", 1.0f);
	// 				}
	// 				else {
	// 					SpawnCustomParticle(tiny, ParticleType::Red, NiPoint3(), "NPC Root [Root]", 1.0f);
	// 				}
	// 				break;
	// 			}
	// 		}
	// 	}
	// }

	// void AddPerkPoints(float a_Level) {
	//
	// 	const auto _Level = static_cast<int>(std::round(a_Level));
	//
	// 	auto GtsSkillPerkPoints = Runtime::GetGlobal(Runtime::GLOB.GTSSkillPerkPoints);
	//
	// 	if (!GtsSkillPerkPoints) {
	// 		return;
	// 	}
	//
	// 	if (static_cast<int>(_Level) % 5 == 0) {
	// 		Notify("You've learned a bonus perk point.");
	// 		GtsSkillPerkPoints->value += 1.0f;
	// 	}
	//
	// 	if (_Level == 20 || _Level == 40) {
	// 		GtsSkillPerkPoints->value += 2.0f;
	// 	}
	// 	else if (_Level == 60 || _Level == 80) {
	// 		GtsSkillPerkPoints->value += 3.0f;
	// 	}
	// 	else if (_Level == 100) {
	// 		GtsSkillPerkPoints->value += 4.0f;
	// 	}
	// }
	//
	// //Player Function
	// void CallVampire() {
	// 	auto progressionQuest = Runtime::GetQuest(Runtime::QUST.GTSQuestProxy);
	// 	if (progressionQuest) {
	// 		CallFunctionOn(progressionQuest, "GTSProxy", "Proxy_SatisfyVampire");
	// 	}
	// }
	//
	// void AddCalamityPerk() {
	// 	auto progressionQuest = Runtime::GetQuest(Runtime::QUST.GTSQuestProxy);
	// 	if (progressionQuest) {
	// 		CallFunctionOn(progressionQuest, "GTSProxy", "Proxy_AddCalamityShout");
	// 	}
	// }
	//
	//
	// //Unused
	// void RemoveCalamityPerk() {
	// 	auto progressionQuest = Runtime::GetQuest(Runtime::QUST.GTSQuestProxy);
	// 	if (progressionQuest) {
	// 		CallFunctionOn(progressionQuest, "GTSProxy", "Proxy_RemoveCalamityShout");
	// 	}
	// }
}