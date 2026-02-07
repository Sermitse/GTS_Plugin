#pragma once

namespace RuntimeData {

	struct Perks : iListable <RE::BGSPerk> {

		Entry(GTSPerkAcceleration,          GTSP, 0x87F342); //Acceleration (75);
		Entry(GTSPerkAdditionalGrowth,      GTSP, 0x151518); //Additional Growth (80);
		Entry(GTSPerkBendTheLifeless,       GTSP, 0x7913D9); //Bend The Lifeless (60);

		Entry(GTSPerkBreastsAbsorb,         GTSP, 0x856B39); //Alluring Softness (50);
		Entry(GTSPerkBreastsIntro,          GTSP, 0x898845); //Personal Approach (15);
		Entry(GTSPerkBreastsMastery1,       GTSP, 0x856B3B); //Cleavage Mastery (40);
		Entry(GTSPerkBreastsMastery2,       GTSP, 0x856B3D); //Predominance (60);
		Entry(GTSPerkBreastsStrangle,       GTSP, 0x8ACC4C); //Succulent Strangle (25);
		Entry(GTSPerkBreastsSuffocation,    GTSP, 0x856B3A); //Breathtaking Bust (25);
		Entry(GTSPerkBreastsVore,           GTSP, 0x856B38); //Nimble Vore (35);

		Entry(GTSPerkButtCrush,             GTSP, 0x5FC366); //Deadly Body (15);
		Entry(GTSPerkButtCrushAug1,         GTSP, 0x5FC36A); //No Escape (25);
		Entry(GTSPerkButtCrushAug2,         GTSP, 0x5FC367); //Growing Disaster (35);
		Entry(GTSPerkButtCrushAug3,         GTSP, 0x5FC368); //Horrendous Growth (60);
		Entry(GTSPerkButtCrushAug4,         GTSP, 0x5FC369); //Looming Doom (100);

		Entry(GTSPerkHugs,                  GTSP, 0x60656E); //Gentle Hugs (25);
		Entry(GTSPerkHugsGreed,             GTSP, 0x606571); //Thirsty Cuddles (40);
		Entry(GTSPerkHugsOfDeath,           GTSP, 0x629C98); //Hugs Of Death (90);
		Entry(GTSPerkHugsLovingEmbrace,     GTSP, 0x7598CE); //Loving Embrace (50);
		Entry(GTSPerkHugMightyCuddles,      GTSP, 0x60656F); //Mighty Cuddles (80);
		Entry(GTSPerkHugsToughGrip,         GTSP, 0x606570); //Full Concentration (50);

		Entry(GTSPerkDarkArts,              GTSP, 0x64319B); //Shrink Outburst (15);
		Entry(GTSPerkDarkArtsAug1,          GTSP, 0x64319C); //Empowered Shrink (30);
		Entry(GTSPerkDarkArtsAug2,          GTSP, 0x64319D); //Additional Burst (50);
		Entry(GTSPerkDarkArtsAug3,          GTSP, 0x19D462); //Burst Resistance (60);
		Entry(GTSPerkDarkArtsAug4,          GTSP, 0x63E09A); //Pressured Outburst (80);
		Entry(GTSPerkDarkArtsLegendary,     GTSP, 0x8F3A83); //Shrink Surge (100, Legendary Rank 1);

		Entry(GTSPerkTinyCalamity,          GTSP, 0x18E161); //Tiny Calamity (65);
		Entry(GTSPerkTinyCalamityRefresh,   GTSP, 0x5ED064); //Eternal Calamity (80);
		Entry(GTSPerkTinyCalamityAug,       GTSP, 0x2E663B); //Approaching Disaster (90);
		Entry(GTSPerkTinyCalamitySizeSteal, GTSP, 0x2496E8); //Life Steal (85);
		Entry(GTSPerkTinyCalamityRage,      GTSP, 0x856B3C); //Wrathful Calamity (70);

		Entry(GTSPerkRandomGrowth,          GTSP, 0x35AD69); //Pleasurable Growth (65);
		Entry(GTSPerkRandomGrowthAug,       GTSP, 0x865E3E); //Breaching Growth (75);
		Entry(GTSPerkRandomGrowthTerror,    GTSP, 0x865E3F); //Terrifying Growth (85);

		Entry(GTSPerkSizeReserve,           GTSP, 0x4204CE); //Size Reserve (50);
		Entry(GTSPerkSizeReserveAug1,       GTSP, 0x4C7638); //Critical Reserve (60);
		Entry(GTSPerkSizeReserveAug2,       GTSP, 0x4255D0); //Healing Reserve (65);

		Entry(GTSPerkSizeManipulation1,     GTSP, 0x128CF2); //Size Manipulation (10);
		Entry(GTSPerkSizeManipulation2,     GTSP, 0x128CF3); //Expanded Growth (35);
		Entry(GTSPerkSizeManipulation3,     GTSP, 0x128CF4); //Adapting To Growth (70);

		Entry(GTSPerkCruelty,               GTSP, 0x128CF8); //Strengthened Might (20);
		Entry(GTSPerkRealCruelty,           GTSP, 0x142205); //Cruelty (80);

		Entry(GTSPerkVoreAbility,           GTSP, 0x355C68); //Effortless Vore (15);
		Entry(GTSPerkVoreHeal,              GTSP, 0x33C764); //Voracious Vore (60);

		Entry(GTSPerkSprintDamageMult1,     GTSP, 0x123BEA); //Quick Approach (25);
		Entry(GTSPerkSprintDamageMult2,     GTSP, 0x123BEB); //Devastating Sprint (60);

		Entry(GTSPerkExtraGrowth1,          GTSP, 0x332563); //Strong Spurt (50);
		Entry(GTSPerkExtraGrowth2,          GTSP, 0x397972); //True Growth (60);

		Entry(GTSPerkShrinkAdept,           GTSP, 0x16081F); //Shrink Adept (35);
		Entry(GTSPerkShrinkExpert,          GTSP, 0x160820); //Shrink Expert (50);

		Entry(GTSPerkGrowthDesire,          GTSP, 0x128CF6); //Growth Desire (60);
		Entry(GTSPerkGrowthDesireAug,       GTSP, 0x829231); //Manual Growth (85);

		Entry(GTSPerkGrowthAug1,            GTSP, 0x151519); //Growth Of Strength (30);
		Entry(GTSPerkGrowthAug2,            GTSP, 0x18E160); //Growing Regeneration (40);

		Entry(GTSPerkMightOfDragons,        GTSP, 0x67FDAC); //Might Of Dragons (40);
		Entry(GTSPerkMightOfGiants,         GTSP, 0x6AD6B1); //Might Of Giants (Quest);

		Entry(GTSPerkGrowingPressure,       GTSP, 0x1E93A9); //Growing Pressure (75);
		Entry(GTSPerkDisastrousTremmor,     GTSP, 0x61FA97); //Disastrous Tremor (100);
		Entry(GTSPerkRuinousStride,         GTSP, 0x8F3A81); //Ruinous Stride (100, Legendary Rank 1);
		Entry(GTSPerkMassActions,           GTSP, 0x462228); //Enhanced Capacity (65);
		Entry(GTSPerkDestructionBasics,     GTSP, 0x587C4A); //Destruction Basics (10);
		Entry(GTSPerkHitGrowth,             GTSP, 0x30EE52); //Hit Growth (50);
		Entry(GTSPerkHealthGate,            GTSP, 0x4E0B39); //Second Wind (55);
		Entry(GTSPerkHighHeels,             GTSP, 0x271EF1); //Heels Of Destruction (50);
		Entry(GTSPerkThighAbilities,        GTSP, 0x587C4B); //Killer Thighs (10);
		Entry(GTSPerkDeadlyRumble,          GTSP, 0x2496E9); //Deadly Rumble (90);
		Entry(GTSPerkLifeAbsorption,        GTSP, 0x87F341); //Life Absorption (75);
		Entry(GTSPerkCruelFall,             GTSP, 0x123BE9); //Puissant Fall (25);
		Entry(GTSPerkOnTheEdge,             GTSP, 0x4399E3); //On The Edge (65);
		Entry(GTSPerkRavagingInjuries,      GTSP, 0x128CF5); //Ravaging Injuries (40);
		Entry(GTSPerkColossalGrowth,        GTSP, 0x128CF7); //Colossal Growth (100);
		Entry(GTSPerkOverindulgence,        GTSP, 0x8F3A80); //Overindulgence (100, Legendary Rank 2);
		Entry(GTSPerkRumblingFeet,          GTSP, 0x1CAD91); //Overwhelming Steps (65);
		Entry(GTSPerkShrinkingGaze,         GTSP, 0x889543); //Shrinking Gaze (90);
		Entry(GTSPerkCataclysmicStomp,      GTSP, 0x610774); //Cataclysmic Stomp (60);
		Entry(GTSPerkFullAssimilation,      GTSP, 0x33C765); //Full Assimilation (60);
		Entry(GTSPerkExperiencedGiantess,   GTSP, 0x61A996); //Experienced Giantess (60);
		Entry(GTSPerkOvergrowth, 			GTSP, 0x90CFC7); //Overgrowth (100), Legendary Rank 1)

		Entry(GTSUtilTalkToActor,           GTSP, 0x5F7265); //TalkToActorWhenSneaking (Internal);

		//NPC Only Perks
		Entry(GTSNPCPerkSkilled10,          GTSP, 0x907EBD); //GTSNPCSKilled10
		Entry(GTSNPCPerkSkilled30,          GTSP, 0x907EBE); //GTSNPCSKilled30
		Entry(GTSNPCPerkSkilled50,          GTSP, 0x907EBF); //GTSNPCSKilled50
		Entry(GTSNPCPerkSkilled70,          GTSP, 0x907EC0); //GTSNPCSKilled70
		Entry(GTSNPCPerkSkilled90,          GTSP, 0x907EC1); //GTSNPCSKilled90
		Entry(GTSNPCPerkSkilled100,         GTSP, 0x907EC2); //GTSNPCSKilled100

	};

}