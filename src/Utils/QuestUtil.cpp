#include "Utils/QuestUtil.hpp"

namespace {

	const std::vector<std::string> AllPerks {
		"GTSPerkAcceleration",
		"GTSPerkAdditionalGrowth",
		"GTSPerkBendTheLifeless",
		"GTSPerkBreastsAbsorb",
		"GTSPerkBreastsIntro",
		"GTSPerkBreastsMastery1",
		"GTSPerkBreastsMastery2",
		"GTSPerkBreastsSuffocation",
		"GTSPerkBreastsVore",
		"GTSPerkButtCrushAug2",
		"GTSPerkButtCrush",
		"GTSPerkButtCrushAug4",
		"GTSPerkButtCrushAug1",
		"GTSPerkButtCrushAug3",
		"GTSPerkTinyCalamity",
		"GTSPerkColossalGrowth",
		"GTSPerkCruelty",
		"GTSPerkDarkArts",
		"GTSPerkDarkArtsAug1",
		"GTSPerkDarkArtsAug2",
		"GTSPerkDarkArtsAug3",
		"GTSPerkDarkArtsAug4",
		"GTSPerkDarkArtsLegendary",
		"GTSPerkDestructionBasics",
		"GTSPerkSprintDamageMult2",
		"GTSPerkDisastrousTremmor",
		"GTSPerkMassActions",
		"GTSPerkTinyCalamityRefresh",
		"GTSPerkExtraGrowth1",
		"GTSPerkExtraGrowth2",
		"GTSPerkShrinkAdept",
		"GTSPerkVoreHeal",
		"GTSPerkGrowingPressure",
		"GTSPerkGrowthDesire",
		"GTSPerkGrowthDesireAug",
		"GTSPerkGrowthAug1",
		"GTSPerkHitGrowth",
		"GTSPerkHealthGate",
		"GTSPerkGrowthAug2",
		"GTSPerkHighHeels",
		"GTSPerkHugs",
		"GTSPerkHugsGreed",
		"GTSPerkHugsOfDeath",
		"GTSPerkHugsLovingEmbrace",
		"GTSPerkHugMightyCuddles",
		"GTSPerkHugsToughGrip",
		"GTSPerkThighAbilities",
		"GTSPerkDeadlyRumble",
		"GTSPerkShrinkExpert",
		"GTSPerkLifeAbsorption",
		"GTSPerkTinyCalamitySizeSteal",
		"GTSPerkMightOfDragons",
		"GTSPerkMightOfGiants",
		"GTSPerkCruelFall",
		"GTSPerkTinyCalamityAug",
		"GTSPerkOnTheEdge",
		"GTSPerkSprintDamageMult1",
		"GTSPerkRandomGrowth",
		"GTSPerkRandomGrowthAug",
		"GTSPerkRandomGrowthTerror",
		"GTSPerkRavagingInjuries",
		"GTSPerkRealCruelty",
		"GTSPerkRumblingFeet",
		"GTSPerkShrinkingGaze",
		"GTSPerkSizeConversion",
		"GTSPerkSizeReserve",
		"GTSPerkSizeReserveAug1",
		"GTSPerkSizeReserveAug2",
		"GTSPerkExperiencedGiantess",
		"GTSPerkFullAssimilation",
		"GTSUtilTalkToActor",
		"GTSPerkSizeManipulation3",
		"GTSPerkVoreAbility",
		"GTSPerkTinyCalamityRage",
		"GTSPerkSizeManipulation1",
		"GTSPerkSizeManipulation2",
		"GTSPerkBreastsStrangle",
		"GTSPerkOverindulgence",
		"GTSPerkRuinousStride"
	};

	const std::vector<std::string> AllSpells {
		"GTSSpellAbsorb",
		"GTSSpellGrowAlly",
		"GTSSpellGrowAllyAdept",
		"GTSSpellGrowAllyExpert",
		"GTSSpellGrowth",
		"GTSSpellGrowthAdept",
		"GTSSpellGrowthExpert",
		"GTSSpellRestoreSize",
		"GTSSpellRestoreSizeOther",
		"GTSSpellShrink",
		"GTSSpellShrinkAlly",
		"GTSSpellShrinkAllyAdept",
		"GTSSpellShrinkAllyExpert",
		"GTSSpellShrinkBolt",
		"GTSSpellShrinkEnemy",
		"GTSSpellShrinkEnemyAOE",
		"GTSSpellShrinkEnemyAOEMastery",
		"GTSSpellShrinkStorm",
		"GTSSpellShrinkToNothing",
		"GTSSpellSlowGrowth",
		"GTSSpellTrueAbsorb"
	};
}


namespace GTS {

	void SkipProgressionQuest() {

		auto progressionQuest = Runtime::GetQuest("GTSQuestProgression");
		if (progressionQuest) {
			CallFunctionOn(progressionQuest, "GTSProgressionQuest", "Proxy_SkipQuest");
		}
	}

	bool ProgressionQuestCompleted() {
		auto Quest = Runtime::GetQuest("GTSQuestProgression");

		if (Quest) {
			return Quest->GetCurrentStageID() >= 200;
		}

		return false;

	}

	void GiveAllPerksToPlayer() {
		auto Player = PlayerCharacter::GetSingleton();

		for (auto& Perk : AllPerks) {
			Runtime::AddPerk(Player, Perk);
		}

		auto GtsSkillLevel = Runtime::GetGlobal("GTSSkillLevel");
		auto GtsSkillRatio = Runtime::GetGlobal("GTSSkillRatio");
		if (GtsSkillLevel && GtsSkillRatio) {
			GtsSkillLevel->value = 100.0f;
			GtsSkillRatio->value = 0.0f;
		}

		Notify("All perks have been given.");
	}

	void GiveAllSpellsToPlayer() {
		auto Player = PlayerCharacter::GetSingleton();

		for (auto& Perk : AllSpells) {
			Runtime::AddSpell(Player, Perk);
		}

		Notify("All spells have been given.");
	}

	void GiveAllShoutsToPlayer() {
		auto progressionQuest = Runtime::GetQuest("GTSQuestProgression");
		if (progressionQuest) {
			CallFunctionOn(progressionQuest, "GTSProgressionQuest", "Proxy_GetAllShouts");
			Notify("All shouts have been given.");
		}
	}
}