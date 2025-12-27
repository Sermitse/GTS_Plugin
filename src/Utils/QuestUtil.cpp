#include "Utils/QuestUtil.hpp"

namespace GTS {

	void SkipProgressionQuest() {
		if (auto progressionQuest = Runtime::GetQuest(Runtime::QUST.GTSQuestProgression)) {
			CallVMFunctionOn(progressionQuest, "GTSProgressionQuest", "Proxy_SkipQuest");
		}
	}

	bool ProgressionQuestCompleted() {
		if (auto Quest = Runtime::GetQuest(Runtime::QUST.GTSQuestProgression)) {
			return Quest->GetCurrentStageID() >= 200;
		}
		return false;
	}

	void GiveAllPerksToPlayer() {
		const auto& Player = PlayerCharacter::GetSingleton();

		for (const auto& Perk : Runtime::PERK.List) {
			if (Perk.first.contains("GTSPerk")) {
				Runtime::AddPerk(Player, *Perk.second);
			}
		}

		Runtime::SetFloat(Runtime::GLOB.GTSSkillLevel, 100.0f);
		Runtime::SetFloat(Runtime::GLOB.GTSSkillRatio, 0.0f);

		Notify("All perks have been given.");
	}

	void GiveAllSpellsToPlayer() {
		const auto& Player = PlayerCharacter::GetSingleton();
		for (const auto& Spell: Runtime::SPEL.List) {
			//Do Not Add Survival Mode spells
			if (!Spell.first.contains("Survival_")) {
				Runtime::AddSpell(Player, *Spell.second);
			}
		}

		Notify("All spells have been given.");
	}

	void GiveAllShoutsToPlayer() {
		if (const auto& progressionQuest = Runtime::GetQuest(Runtime::QUST.GTSQuestProgression)) {
			CallVMFunctionOn(progressionQuest, "GTSProgressionQuest", "Proxy_GetAllShouts");
			Notify("All shouts have been given.");
		}
	}

	float GetQuestProgression(int stage) {
		const QuestStage Stage = static_cast<QuestStage>(stage);

		switch (Stage) {
			case QuestStage::HugSteal:        return Persistent::HugStealCount.value;                           // Stage 0: hug steal 2 meters of size
			case QuestStage::HugSpellSteal:   return Persistent::StolenSize.value;                              // Stage 1: hug/spell steal 5 meters of size
			case QuestStage::Crushing:        return Persistent::CrushCount.value;                              // Stage 2: Crush 3 (*4 if dead) enemies
			case QuestStage::ShrinkToNothing: return Persistent::CrushCount.value + Persistent::STNCount.value; // Stage 3: Crush or Shrink to nothing 6 more enemies in total
			case QuestStage::HandCrush:       return Persistent::HandCrushed.value;                             // Stage 4: hand crush 3 enemies
			case QuestStage::Vore:            return Persistent::VoreCount.value;                               // Stage 5: Vore 6 enemies
			case QuestStage::Giant:	          return Persistent::GiantCount.value;                              // Stage 6: Vore/crush/shrink a Giant
			default: break;
		}
		return 0.0f;
	}

	void ResetQuest() {
		Persistent::HugStealCount.value = 0.0f;
		Persistent::StolenSize.value    = 0.0f;
		Persistent::CrushCount.value    = 0.0f;
		Persistent::STNCount.value      = 0.0f;
		Persistent::HandCrushed.value   = 0.0f;
		Persistent::VoreCount.value     = 0.0f;
		Persistent::GiantCount.value    = 0.0f;
	}
	

	void AdvanceQuestProgression(Actor* giant, Actor* tiny, QuestStage stage, float value, bool vore) {

		if (giant->IsPlayerRef()) { // Player Only

			if (static const auto& quest = Runtime::GetQuest(Runtime::QUST.GTSQuestProgression)) {
				static constexpr std::string_view root_bone_name = "NPC Root [Root]";
				const auto queststage = quest->GetCurrentStageID();

				if (queststage >= 100 || queststage < 10) {
					return;
				}

				switch (stage) {

					// Stage 0: hug steal 2 meters of size
					case QuestStage::HugSteal: 
					{
						Persistent::HugStealCount.value += value;
					} break;

					// Stage 1: hug/spell steal 5 meters of size
					case QuestStage::HugSpellSteal:
					{
						if (queststage == 20) {
							Persistent::StolenSize.value += value;
						}
					} break;

					// Stage 2: Crush 3 ( * 1/4th if dead ) enemies
					case QuestStage::Crushing:
					{
						if (queststage >= 30 && queststage <= 40) {

							Persistent::CrushCount.value += value;

							SpawnCustomParticle(tiny, (value < 1) ? ParticleType::DarkRed : ParticleType::Red, NiPoint3(), root_bone_name, 1.0f);

							const float progression = GetQuestProgression((queststage == 40)  ? static_cast<int>(QuestStage::ShrinkToNothing) : static_cast<int>(QuestStage::Crushing));
							const float goal = queststage == 40 ?  9.0f : 3.0f;
							Notify("Progress: {:.1f}/{:.1f}", progression, goal);
						}
					} break;

					// Stage 3: Crush or Shrink to nothing 6 enemies in total
					case QuestStage::ShrinkToNothing:
					{
						if (queststage == 40) {
							Persistent::STNCount.value += value;
							SpawnCustomParticle(tiny, (value < 1) ? ParticleType::DarkRed : ParticleType::Red, NiPoint3(), root_bone_name, 1.0f);
							Notify("Progress: {:.1f}/{:.1f}", GetQuestProgression(static_cast<int>(QuestStage::ShrinkToNothing)), 9.0f);
						}
					} break;

					// Stage 4: hand crush 3 enemies
					case QuestStage::HandCrush: 
					{
						Persistent::HandCrushed.value += value;
						SpawnCustomParticle(tiny, ParticleType::Red, NiPoint3(), root_bone_name, 1.0f);
						Notify("Progress: {:.1f}/{:.1f}", GetQuestProgression(static_cast<int>(QuestStage::HandCrush)), 3.0f);
					} break;

					// Stage 5: Vore 6 enemies
					case QuestStage::Vore: 
					{
						if (queststage == 60) {
							Persistent::VoreCount.value += value;
							SpawnCustomParticle(tiny, ParticleType::Blue, NiPoint3(), root_bone_name, 1.0f);
							Notify("Progress: {:.1f}/{:.1f}", GetQuestProgression(static_cast<int>(QuestStage::Vore)), 6.0f);
						}
					} break;

					// Stage 6: Vore/crush/shrink a Giant
					case QuestStage::Giant:
					{
						Persistent::GiantCount.value += value;
						SpawnCustomParticle(tiny, vore ? ParticleType::Blue : ParticleType::Red, NiPoint3(), root_bone_name, 1.0f);
					} break;

					default: break;
				}
			}
		}
	}

	// Needed for smooth animation unlocks during quest progression
	bool CanDoActionBasedOnQuestProgress(Actor* giant, QuestAnimationType type) {

		if (!giant->IsPlayerRef()) {
			return true;
		}

		if (TESQuest* const& progressionQuest = Runtime::GetQuest(Runtime::QUST.GTSQuestProgression)) {

			const uint16_t queststage = progressionQuest->GetCurrentStageID();

			logger::debug("CanDoActionBasedOnQuestProgress (Stage: {} / Type: {})", queststage, static_cast<int>(type));

			switch (type) {
				case QuestAnimationType::kHugs:            return queststage >= 10;  // allow hugs
				case QuestAnimationType::kStompsAndKicks:  return queststage >= 30;  // allow stomps and kicks
				case QuestAnimationType::kGrabAndSandwich: return queststage >= 50;  // Allow grabbing and sandwiching
				case QuestAnimationType::kVore:            return queststage >= 60;  // Allow Vore
				case QuestAnimationType::kOthers:          return queststage >= 100; // When quest is completed
				default: break;
			}
		}
		return false;
	}

	void CompleteDragonQuest(Actor* tiny, ParticleType Type) {
		
		if (const auto& progressionQuest = Runtime::GetQuest(Runtime::QUST.GTSQuestProgression)) {
			const auto stage = progressionQuest->GetCurrentStageID();
			if (stage == 80) {
				if (auto data = Transient::GetActorData(PlayerCharacter::GetSingleton())) {
					data->DragonWasEaten = true;
					SpawnCustomParticle(tiny, Type, NiPoint3(), "NPC Root [Root]", 1.0f);
				}
			}
		}
	}
}