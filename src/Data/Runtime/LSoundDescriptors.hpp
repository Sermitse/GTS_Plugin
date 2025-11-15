#pragma once

namespace RuntimeData {

	struct SoundDescriptors : iListable <RE::BGSSoundDescriptorForm> {

		//Impact
		Entry(GTSSoundBreastImpact,               GTSP, 0x5E2E5F);
		Entry(GTSSoundSwingImpact,                GTSP, 0x68F0AD);
		Entry(GTSSoundThighSandwichImpact,        GTSP, 0x582B49);

		//Crunch
		Entry(GTSSoundCrunchImpact,               GTSP, 0x5D8C5C);
		Entry(GTSSoundCrunchKill,                 GTSP, 0x5D8C5D);
		Entry(GTSSoundCrushDefault,               GTSP, 0x01102F);
		Entry(GTSSoundCrushFootMulti3x8x,         GTSP, 0x82E332);
		Entry(GTSSoundCrushFootSingle8x,          GTSP, 0x833433);

		//Stomp Light
		Entry(GTSSoundFootstep_Stomp_Light_x2,    GTSP, 0x8DA56D);
		Entry(GTSSoundFootstep_Stomp_Light_x4,    GTSP, 0x8DA56E);
		Entry(GTSSoundFootstep_Stomp_Light_x8,    GTSP, 0x8DA56F);
		Entry(GTSSoundFootstep_Stomp_Light_x12,   GTSP, 0x8DA570);
		Entry(GTSSoundFootstep_Stomp_Light_x24,   GTSP, 0x8DA571);
		Entry(GTSSoundFootstep_Stomp_Light_x48,   GTSP, 0x8DA572);
		Entry(GTSSoundFootstep_Stomp_Light_x96,   GTSP, 0x8DA573);
		Entry(GTSSoundFootstep_Stomp_Light_Mega,  GTSP, 0x8DA574);

		//Stomp Strong
		Entry(GTSSoundFootstep_Stomp_Strong_x2,   GTSP, 0x8DA578);
		Entry(GTSSoundFootstep_Stomp_Strong_x4,   GTSP, 0x8DA57D);
		Entry(GTSSoundFootstep_Stomp_Strong_x8,   GTSP, 0x8DA57A);
		Entry(GTSSoundFootstep_Stomp_Strong_x12,  GTSP, 0x8DA577);
		Entry(GTSSoundFootstep_Stomp_Strong_x24,  GTSP, 0x8DA579);
		Entry(GTSSoundFootstep_Stomp_Strong_x48,  GTSP, 0x8DA57B);
		Entry(GTSSoundFootstep_Stomp_Strong_x96,  GTSP, 0x8DA57C);
		Entry(GTSSoundFootstep_Stomp_Strong_Mega, GTSP, 0x8DA576);

		//Footstep Normal
		Entry(GTSSoundFootstepNormal_2x,          GTSP, 0x61587E);
		Entry(GTSSoundFootstepNormal_4x,          GTSP, 0x61587F);
		Entry(GTSSoundFootstepNormal_8x,          GTSP, 0x615880);
		Entry(GTSSoundFootstepNormal_12x,         GTSP, 0x615881);
		Entry(GTSSoundFootstepNormal_24x,         GTSP, 0x615882);
		Entry(GTSSoundFootstepNormal_48x,         GTSP, 0x615883);
		Entry(GTSSoundFootstepNormal_96x,         GTSP, 0x615884);
		Entry(GTSSoundFootstepNormal_Mega,        GTSP, 0x615885);

		//Footstep Normal Land
		Entry(GTSSoundFootstepLandNormal_2x,      GTSP, 0x615886);
		Entry(GTSSoundFootstepLandNormal_4x,      GTSP, 0x615887);
		Entry(GTSSoundFootstepLandNormal_8x,      GTSP, 0x615888);
		Entry(GTSSoundFootstepLandNormal_12x,     GTSP, 0x615889);
		Entry(GTSSoundFootstepLandNormal_24x,     GTSP, 0x61588A);
		Entry(GTSSoundFootstepLandNormal_48x,     GTSP, 0x61588B);
		Entry(GTSSoundFootstepLandNormal_96x,     GTSP, 0x61588C);
		Entry(GTSSoundFootstepLandNormal_Mega,    GTSP, 0x61588D);

		//Footstep Highheels
		Entry(GTSSoundFootstepHighHeels_2x,       GTSP, 0x615875);
		Entry(GTSSoundFootstepHighHeels_4x,       GTSP, 0x615877);
		Entry(GTSSoundFootstepHighHeels_8x,       GTSP, 0x615878);
		Entry(GTSSoundFootstepHighHeels_12x,      GTSP, 0x615879);
		Entry(GTSSoundFootstepHighHeels_24x,      GTSP, 0x61587A);
		Entry(GTSSoundFootstepHighHeels_48x,      GTSP, 0x61587B);
		Entry(GTSSoundFootstepHighHeels_96x,      GTSP, 0x61587C);
		Entry(GTSSoundFootstepHighHeels_Mega,     GTSP, 0x61587D);

		//Footstep Highheels Alt Set
		Entry(GTSSoundFootstepHighHeels_1_5x_Alt, GTSP, 0x8EE97F);
		Entry(GTSSoundFootstepHighHeels_2x_Alt,   GTSP, 0x8D5467);
		Entry(GTSSoundFootstepHighHeels_4x_Alt,   GTSP, 0x8D5469);
		Entry(GTSSoundFootstepHighHeels_8x_Alt,   GTSP, 0x8D546A);
		Entry(GTSSoundFootstepHighHeels_12x_Alt,  GTSP, 0x8D5465);
		Entry(GTSSoundFootstepHighHeels_24x_Alt,  GTSP, 0x8D5466);
		Entry(GTSSoundFootstepHighHeels_48x_Alt,  GTSP, 0x8D5468);
		Entry(GTSSoundFootstepHighHeels_96x_Alt,  GTSP, 0x8D546B);
		Entry(GTSSoundFootstepHighHeels_Mega_Alt, GTSP, 0x8D546C);

		//Footstep Highheels Land
		Entry(GTSSoundFootstepLandHighHeels_2x,   GTSP, 0x61588E);
		Entry(GTSSoundFootstepLandHighHeels_4x,   GTSP, 0x61588F);
		Entry(GTSSoundFootstepLandHighHeels_8x,   GTSP, 0x615890);
		Entry(GTSSoundFootstepLandHighHeels_12x,  GTSP, 0x615891);
		Entry(GTSSoundFootstepLandHighHeels_24x,  GTSP, 0x615892);
		Entry(GTSSoundFootstepLandHighHeels_48x,  GTSP, 0x615893);
		Entry(GTSSoundFootstepLandHighHeels_96x,  GTSP, 0x615894);
		Entry(GTSSoundFootstepLandHighHeels_Mega, GTSP, 0x615895);

		//Footstep Legacy Sounds
		Entry(GTSSoundFootstep_S,                 GTSP, 0x1C5C73);
		Entry(GTSSoundFootstep_L,                 GTSP, 0x1E93AB);
		Entry(GTSSoundFootstep_XL,                GTSP, 0x271EF4);
		Entry(GTSSoundFootstep_XXL,               GTSP, 0x16FB25);
		Entry(GTSSoundFootstep_Sprint,            GTSP, 0x319060);
		Entry(GTSSoundFootstepLand_L,             GTSP, 0x183F43);

		//Laugh Crush
		Entry(GTSSoundLaugh_Crush,                GTSP, 0x09B0A9);
		Entry(GTSSoundLaugh_Crush_x2,             GTSP, 0x8D0357);
		Entry(GTSSoundLaugh_Crush_x4,             GTSP, 0x8D0358);
		Entry(GTSSoundLaugh_Crush_x8,             GTSP, 0x8D0359);
		Entry(GTSSoundLaugh_Crush_x12,            GTSP, 0x8D035A);
		Entry(GTSSoundLaugh_Crush_x24,            GTSP, 0x8D035B);
		Entry(GTSSoundLaugh_Crush_x48,            GTSP, 0x8D035C);
		Entry(GTSSoundLaugh_Crush_x96,            GTSP, 0x8D035D);

		//Laugh Struggle
		Entry(GTSSoundLaugh_Struggle,             GTSP, 0x5D3B5B);
		Entry(GTSSoundLaugh_Struggle_x2,          GTSP, 0x8D035E);
		Entry(GTSSoundLaugh_Struggle_x4,          GTSP, 0x8D035F);
		Entry(GTSSoundLaugh_Struggle_x8,          GTSP, 0x8D0360);
		Entry(GTSSoundLaugh_Struggle_x12,         GTSP, 0x8D0361);
		Entry(GTSSoundLaugh_Struggle_x24,         GTSP, 0x8D0362);
		Entry(GTSSoundLaugh_Struggle_x48,         GTSP, 0x8D0363);
		Entry(GTSSoundLaugh_Struggle_x96,         GTSP, 0x8D0364);

		//Laugh Overkill
		Entry(GTSSoundLaugh_Overkill,             GTSP, 0x8F8B9C);
		Entry(GTSSoundLaugh_Overkill_x2,          GTSP, 0x8F8B9D);
		Entry(GTSSoundLaugh_Overkill_x4,          GTSP, 0x8F8B9E);
		Entry(GTSSoundLaugh_Overkill_x8,          GTSP, 0x8F8B9F);
		Entry(GTSSoundLaugh_Overkill_x12,         GTSP, 0x8F8BA0);
		Entry(GTSSoundLaugh_Overkill_x24,         GTSP, 0x8F8BA1);
		Entry(GTSSoundLaugh_Overkill_x48,         GTSP, 0x8F8BA2);
		Entry(GTSSoundLaugh_Overkill_x96,         GTSP, 0x8F8BA1);

		//Laugh Superiority
		Entry(GTSSoundLaugh_Superiority,          GTSP, 0x8F8BA4);
		Entry(GTSSoundLaugh_Superiority_x2,       GTSP, 0x8F8BA5);
		Entry(GTSSoundLaugh_Superiority_x4,       GTSP, 0x8F8BA6);
		Entry(GTSSoundLaugh_Superiority_x8,       GTSP, 0x8F8BA7);
		Entry(GTSSoundLaugh_Superiority_x12,      GTSP, 0x8F8BA8);
		Entry(GTSSoundLaugh_Superiority_x24,      GTSP, 0x8F8BA9);
		Entry(GTSSoundLaugh_Superiority_x48,      GTSP, 0x8F8BAA);
		Entry(GTSSoundLaugh_Superiority_x96,      GTSP, 0x8F8BAB);

		//Moan Absorb
		Entry(GTSSoundMoan_Absorption,            GTSP, 0x09B0AC);
		Entry(GTSSoundMoan_Absorption_x2,         GTSP, 0x8D0350);
		Entry(GTSSoundMoan_Absorption_x4,         GTSP, 0x8D0351);
		Entry(GTSSoundMoan_Absorption_x8,         GTSP, 0x8D0352);
		Entry(GTSSoundMoan_Absorption_x12,        GTSP, 0x8D0353);
		Entry(GTSSoundMoan_Absorption_x24,        GTSP, 0x8D0354);
		Entry(GTSSoundMoan_Absorption_x48,        GTSP, 0x8D0355);
		Entry(GTSSoundMoan_Absorption_x96,        GTSP, 0x8D0356);

		//Moan Crush
		Entry(GTSSoundMoan_Crush,                 GTSP, 0x8F8B84);
		Entry(GTSSoundMoan_Crush_x2,              GTSP, 0x8F8B85);
		Entry(GTSSoundMoan_Crush_x4,              GTSP, 0x8F8B86);
		Entry(GTSSoundMoan_Crush_x8,              GTSP, 0x8F8B87);
		Entry(GTSSoundMoan_Crush_x12,             GTSP, 0x8F8B88);
		Entry(GTSSoundMoan_Crush_x24,             GTSP, 0x8F8B89);
		Entry(GTSSoundMoan_Crush_x48,             GTSP, 0x8F8B8A);
		Entry(GTSSoundMoan_Crush_x96,             GTSP, 0x8F8B8B);

		//Moan Vore
		Entry(GTSSoundMoan_Vore,                  GTSP, 0x8F8B94);
		Entry(GTSSoundMoan_Vore_x2,               GTSP, 0x8F8B95);
		Entry(GTSSoundMoan_Vore_x4,               GTSP, 0x8F8B96);
		Entry(GTSSoundMoan_Vore_x8,               GTSP, 0x8F8B97);
		Entry(GTSSoundMoan_Vore_x12,              GTSP, 0x8F8B98);
		Entry(GTSSoundMoan_Vore_x24,              GTSP, 0x8F8B99);
		Entry(GTSSoundMoan_Vore_x48,              GTSP, 0x8F8B9A);
		Entry(GTSSoundMoan_Vore_x96,              GTSP, 0x8F8B9B);

		//Moan Growth
		Entry(GTSSoundMoan_Growth,                GTSP, 0x8F8B8C);
		Entry(GTSSoundMoan_Growth_x2,             GTSP, 0x8F8B8D);
		Entry(GTSSoundMoan_Growth_x4,             GTSP, 0x8F8B8E);
		Entry(GTSSoundMoan_Growth_x8,             GTSP, 0x8F8B8F);
		Entry(GTSSoundMoan_Growth_x12,            GTSP, 0x8F8B90);
		Entry(GTSSoundMoan_Growth_x24,            GTSP, 0x8F8B91);
		Entry(GTSSoundMoan_Growth_x48,            GTSP, 0x8F8B92);
		Entry(GTSSoundMoan_Growth_x96,            GTSP, 0x8F8B93);

		//Moan Clothing Rip
		Entry(GTSSoundMoan_RipCloth,              GTSP, 0x8F8BAC);
		Entry(GTSSoundMoan_RipCloth_x2,           GTSP, 0x8F8BAD);
		Entry(GTSSoundMoan_RipCloth_x4,           GTSP, 0x8F8BAE);
		Entry(GTSSoundMoan_RipCloth_x8,           GTSP, 0x8F8BAF);
		Entry(GTSSoundMoan_RipCloth_x12,          GTSP, 0x8F8BB0);
		Entry(GTSSoundMoan_RipCloth_x24,          GTSP, 0x8F8BB1);
		Entry(GTSSoundMoan_RipCloth_x48,          GTSP, 0x8F8BB2);
		Entry(GTSSoundMoan_RipCloth_x96,          GTSP, 0x8F8BB3);

		//Moan Hug Drain
		Entry(GTSSoundMoan_HugDrain,              GTSP, 0x8FDCB4);
		Entry(GTSSoundMoan_HugDrain_x2,           GTSP, 0x8FDCB5);
		Entry(GTSSoundMoan_HugDrain_x4,           GTSP, 0x8FDCB6);
		Entry(GTSSoundMoan_HugDrain_x8,           GTSP, 0x8FDCB7);
		Entry(GTSSoundMoan_HugDrain_x12,          GTSP, 0x8FDCB8);
		Entry(GTSSoundMoan_HugDrain_x24,          GTSP, 0x8FDCB9);
		Entry(GTSSoundMoan_HugDrain_x48,          GTSP, 0x8FDCBA);
		Entry(GTSSoundMoan_HugDrain_x96,          GTSP, 0x8FDCBB);

		//Other
		Entry(SKSoundBloodGush,                   SKRM, 0x10F78C);
		Entry(GTSSoundFail,                       GTSP, 0x851A37);
		Entry(GTSSoundRipClothes,                 GTSP, 0x30EE54);
		Entry(GTSSoundSoftHandAttack,             GTSP, 0x5E2E60);
		Entry(GTSSoundSwallow,                    GTSP, 0x56E747);
		Entry(GTSSoundEat,                        GTSP, 0x011031);
		Entry(GTSSoundKissing,                    GTSP, 0x8DF67E);
		Entry(GTSSoundHeavyStomp,                 GTSP, 0x596F4E);

		//Rumble
		Entry(GTSSoundRumble,                     GTSP, 0x36A06D);
		Entry(GTSSoundWalkAirRumble,              GTSP, 0x50423E);
		Entry(GTSSoundGraybeardRumble,            GTSP, 0x1CAD96);

		//Size
		Entry(GTSSoundGrowth,                     GTSP, 0x271EF6);
		Entry(GTSSoundShrink,                     GTSP, 0x364F6A);
		Entry(GTSSoundShrinkOutburst,             GTSP, 0x6524A3);
		Entry(GTSSoundShrinkToNothing,            GTSP, 0x5BA659);

		//Perks
		Entry(GTSSoundTriggerHG,                  GTSP, 0x578948);
		Entry(GTSSoundMagicBreak,                 GTSP, 0x777ED6);
		Entry(GTSSoundMagicProctectTinies,        GTSP, 0x772DD4);

		//Tiny Calamity
		Entry(GTSSoundTinyCalamity,               GTSP, 0x65C6AA);
		Entry(GTSSoundTinyCalamity_Absorb,        GTSP, 0x7B9BE6);
		Entry(GTSSoundTinyCalamity_Crush,         GTSP, 0x76DCD3);
		Entry(GTSSoundTinyCalamity_FingerSnap,    GTSP, 0x8A7B4B);
		Entry(GTSSoundTinyCalamity_Impact,        GTSP, 0x76DCD2);
		Entry(GTSSoundTinyCalamity_ReachedSpeed,  GTSP, 0x7820D7);
		Entry(GTSSoundTinyCalamity_RuneReady,     GTSP, 0x7B9BE5);
		Entry(GTSSoundTinyCalamity_SpawnRune,     GTSP, 0x7B9BE4);

		//Sexlab Alt Voices
		Entry(SLVoiceF1Mil, SXLB, 0x066FC0); //Mild
		Entry(SLVoiceF1Mid, SXLB, 0x067523); //Middle
		Entry(SLVoiceF1Hot, SXLB, 0x067524); //Hot

		Entry(SLVoiceF2Mil, SXLB, 0x067525);
		Entry(SLVoiceF2Mid, SXLB, 0x067527);
		Entry(SLVoiceF2Hot, SXLB, 0x067526);

		Entry(SLVoiceF3Mil, SXLB, 0x06752A);
		Entry(SLVoiceF3Mid, SXLB, 0x067529);
		Entry(SLVoiceF3Hot, SXLB, 0x067528);

		Entry(SLVoiceF4Mil, SXLB, 0x06752D);
		Entry(SLVoiceF4Mid, SXLB, 0x06752C);
		Entry(SLVoiceF4Hot, SXLB, 0x06752B);

		Entry(SLVoiceF5Mil, SXLB, 0x067530);
		Entry(SLVoiceF5Mid, SXLB, 0x06752F);
		Entry(SLVoiceF5Hot, SXLB, 0x06752E);

		Entry(SLVoiceF6Mil, SXLB, 0x067533);
		Entry(SLVoiceF6Mid, SXLB, 0x067532);
		Entry(SLVoiceF6Hot, SXLB, 0x067531);

		Entry(SLVoiceF7Mil, SXLB, 0x067536);
		Entry(SLVoiceF7Mid, SXLB, 0x067535);
		Entry(SLVoiceF7Hot, SXLB, 0x067534);

		Entry(SLVoiceF8Mil, SXLB, 0x067539);
		Entry(SLVoiceF8Mid, SXLB, 0x067538);
		Entry(SLVoiceF8Hot, SXLB, 0x067537);

	};
}