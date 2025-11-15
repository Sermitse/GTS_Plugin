#pragma once

namespace RuntimeData {

	struct Globals : iListable <RE::TESGlobal> {

		//GTS - Skill Tree
		Entry(GTSSkillMenu,                GTSP, 0x1421FF);
		Entry(GTSSkillLevel,               GTSP, 0x142200);
		Entry(GTSSkillRatio,               GTSP, 0x142202);
		Entry(GTSSkillLegendary,           GTSP, 0x142203);
		Entry(GTSSkillProgress,            GTSP, 0x142201);
		Entry(GTSSkillPerkPoints,          GTSP, 0x2352E1);

		//Utility Perk
		Entry(GTSUtilEnableDialogue,       GTSP, 0x277000);

		//Survival Mode
		Entry(Survival_HungerNeedValue,    SRVM, 0x00081A);
		Entry(Survival_HungerStage1Value,  SRVM, 0x000806);
		Entry(Survival_HungerStage2Value,  SRVM, 0x000802);
		Entry(Survival_HungerStage3Value,  SRVM, 0x000803);
		Entry(Survival_HungerStage4Value,  SRVM, 0x000804);
		Entry(Survival_HungerStage5Value,  SRVM, 0x000805);
		Entry(Survival_ModeEnabled,        SRVM, 0x000826);

	};
}
