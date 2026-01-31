#pragma once

namespace RuntimeData {

	struct MagicEffects : iListable <RE::EffectSetting> {

		Entry(GTSEffectTinyCalamity,              GTSP, 0x1A2568);

		Entry(GTSEffectGrowthSpurt1,              GTSP, 0x007928);
		Entry(GTSEffectGrowthSpurt2,              GTSP, 0x1E42A5);
		Entry(GTSEffectGrowthSpurt3,              GTSP, 0x1E42A6);

		Entry(GTSEffectRestoreSize,               GTSP, 0x005369);
		Entry(GTSEffectRestoreSizeOther,          GTSP, 0x00536C);

		Entry(GTSEffectShrink,                    GTSP, 0x002850);
		Entry(GTSEffectShrinkAlly,                GTSP, 0x0058D1);
		Entry(GTSEffectShrinkAllyAdept,           GTSP, 0x452F17);
		Entry(GTSEffectShrinkAllyExpert,          GTSP, 0x452F18);

		Entry(GTSEffectShrinkEnemy,               GTSP, 0x00387B);
		Entry(GTSEffectShrinkEnemyAOE,            GTSP, 0x0DCDC5);
		Entry(GTSEffectShrinkOtherAOEMastery,     GTSP, 0x0DCDCA);

		Entry(GTSEffectShrinkBolt,                GTSP, 0x3C5278);
		Entry(GTSEffectShrinkStorm,               GTSP, 0x3C527C);
		Entry(GTSEffectShrinkToNothing,           GTSP, 0x009979);

		Entry(GTSEnchSwordAbsorbSize,             GTSP, 0x00FA9E);
		Entry(GTSEnchGigantism,                   GTSP, 0x3EDAC5);

		Entry(GTSEffectSlowGrowth,                GTSP, 0x019C3D);
		Entry(GTSEffectSlowGrowthDual,            GTSP, 0x086C8D);

		Entry(GTSEffectGrowth,                    GTSP, 0x0022EB);
		Entry(GTSEffectGrowthAdept,               GTSP, 0x45D124);
		Entry(GTSEffectGrowthExpert,              GTSP, 0x45D125);

		Entry(GTSEffectGrowAlly,                  GTSP, 0x0058D5);
		Entry(GTSEffectGrowAllyAdept,             GTSP, 0x452F15);
		Entry(GTSEffectGrowAllyExpert,            GTSP, 0x452F16);

		Entry(GTSEffectAbsorb,                    GTSP, 0x00B470);
		Entry(GTSEffectAbsorbTrue,                GTSP, 0x22B0D5);

		Entry(GTSPotionEffectGrowthWeak,          GTSP, 0x3D4582);
		Entry(GTSPotionEffectGrowthNormal,        GTSP, 0x7C8F02);
		Entry(GTSPotionEffectGrowthStrong,        GTSP, 0x7C8F03);
		Entry(GTSPotionEffectGrowthExtreme,       GTSP, 0x7C8F04);

		Entry(GTSPotionEffectMightWeak,           GTSP, 0x7D8215);
		Entry(GTSPotionEffectMightNormal,         GTSP, 0x7D8216);
		Entry(GTSPotionEffectMightStrong,         GTSP, 0x7D8217);
		Entry(GTSPotionEffectMightExtreme,        GTSP, 0x7D8218);

		Entry(GTSPotionEffectEssenceWeak,         GTSP, 0x7EC62B);
		Entry(GTSPotionEffectEssenceNormal,       GTSP, 0x7EC62C);
		Entry(GTSPotionEffectEssenceStrong,       GTSP, 0x7EC62D);
		Entry(GTSPotionEffectEssenceExtreme,      GTSP, 0x7EC62E);

		Entry(GTSPotionEffectResistShrinkWeak,    GTSP, 0x3D4583);
		Entry(GTSPotionEffectResistShrinkNormal,  GTSP, 0x7C8F09);
		Entry(GTSPotionEffectResistShrinkStrong,  GTSP, 0x7C8F0A);
		Entry(GTSPotionEffectResistShrinkExtreme, GTSP, 0x7C8F0B);

		Entry(GTSPotionEffectSizeLimitWeak,       GTSP, 0x3E38BB);
		Entry(GTSPotionEffectSizeLimitNormal,     GTSP, 0x3E38BC);
		Entry(GTSPotionEffectSizeLimitStrong,     GTSP, 0x3E38BD);
		Entry(GTSPotionEffectSizeLimitExtreme,    GTSP, 0x3E38C0);

		Entry(GTSPotionEffectSizeExperienceBasic, GTSP, 0x7CE013);
		Entry(GTSPotionEffectSizeHunger,          GTSP, 0x42F7D6);
		Entry(GTSPotionEffectSizeAmplify,         GTSP, 0x452F1D);
		Entry(GTSPoisonEffectShrinking,           GTSP, 0x5B5557);
		Entry(GTSPotionEffectSizeDrain,           GTSP, 0x5B5552);

		Entry(GTSAlchEffectGrowth,                GTSP, 0x89D948);
		Entry(GTSAlchEffectSizeLimit,             GTSP, 0x89D94A);
		Entry(GTSAlchEffectEssence,               GTSP, 0x89D947);
		Entry(GTSAlchEffectMight,                 GTSP, 0x89D946);
		Entry(GTSAlchEffectResistShrink,          GTSP, 0x89D949);

		Entry(GTSEffectThighRune,                 GTSP, 0x907EC4);

	};

}