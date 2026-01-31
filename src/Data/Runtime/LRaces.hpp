#pragma once

namespace RuntimeData {

	struct Races : iListable <RE::TESRace> {

		Entry(ChaurusReaperRace,         SKRM, 0x0A5601);
		Entry(ChaurusRace,               SKRM, 0x0131EB);
		Entry(DragonRace,                SKRM, 0x012E82);
		Entry(FrostbiteSpiderRace,       SKRM, 0x0131F8);
		Entry(FrostbiteSpiderRaceGiant,  SKRM, 0x04E507);
		Entry(FrostbiteSpiderRaceLarge,  SKRM, 0x053477);
		Entry(GiantRace,                 SKRM, 0x0131F9);
		Entry(IcewraithRace,             SKRM, 0x0131FE);
		Entry(MammothRace,               SKRM, 0x0131FF);
		Entry(WerewolfBeastRace,         SKRM, 0x0CDD84);

		Entry(DLC1ChaurusHunterRace,     DWNG, 0x0051FB);
		Entry(DLC1_BF_ChaurusRace,       DWNG, 0x015136);
		Entry(DLC1VampireBeastRace,      DWNG, 0x00283A);


		Entry(DLC2ExpSpiderBaseRace,     DRAG, 0x014449);
		Entry(DLC2ExpSpiderPackmuleRace, DRAG, 0x027483);
		Entry(DLC2AshHopperRace,         DRAG, 0x01B658);

	};

}