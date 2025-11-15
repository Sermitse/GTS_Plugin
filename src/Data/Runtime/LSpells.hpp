#pragma once

namespace RuntimeData {

	struct Spells : iListable <RE::SpellItem> {

		//Spells
		Entry(GTSSpellAbsorb,                GTSP, 0x00B471);
		Entry(GTSSpellFear,                  GTSP, 0x026D15);
		Entry(GTSSpellGrowAlly,              GTSP, 0x0058D4);
		Entry(GTSSpellGrowAllyAdept,         GTSP, 0x452F11);
		Entry(GTSSpellGrowAllyExpert,        GTSP, 0x452F13);
		Entry(GTSSpellGrowth,                GTSP, 0x0022EC);
		Entry(GTSSpellGrowthAdept,           GTSP, 0x45D120);
		Entry(GTSSpellGrowthExpert,          GTSP, 0x45D122);
		Entry(GTSSpellRestoreSize,           GTSP, 0x005368);
		Entry(GTSSpellRestoreSizeOther,      GTSP, 0x00536B);
		Entry(GTSSpellShrink,                GTSP, 0x00284F);
		Entry(GTSSpellShrinkAlly,            GTSP, 0x0058D0);
		Entry(GTSSpellShrinkAllyAdept,       GTSP, 0x452F0D);
		Entry(GTSSpellShrinkAllyExpert,      GTSP, 0x452F0F);
		Entry(GTSSpellShrinkBolt,            GTSP, 0x3C5276);
		Entry(GTSSpellShrinkEnemy,           GTSP, 0x00387A);
		Entry(GTSSpellShrinkEnemyAOE,        GTSP, 0x0DCDC2);
		Entry(GTSSpellShrinkEnemyAOEMastery, GTSP, 0x0DCDC7);
		Entry(GTSSpellShrinkStorm,           GTSP, 0x3C527A);
		Entry(GTSSpellShrinkToNothing,       GTSP, 0x00997A);
		Entry(GTSSpellSlowGrowth,            GTSP, 0x019C38);
		Entry(GTSSpellTrueAbsorb,            GTSP, 0x22B0D6);

		//Survival Mode
		Entry(Survival_HungerStage0,         SRVM, 0x000876);
		Entry(Survival_HungerStage1,         SRVM, 0x00087E);
		Entry(Survival_HungerStage2,         SRVM, 0x000880);
		Entry(Survival_HungerStage3,         SRVM, 0x000881);
		Entry(Survival_HungerStage4,         SRVM, 0x000886);
		Entry(Survival_HungerStage5,         SRVM, 0x000885);

	};
}