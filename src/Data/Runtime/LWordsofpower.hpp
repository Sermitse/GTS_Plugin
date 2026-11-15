#pragma once

namespace RuntimeData {

	struct WordsOfPower : iListable <RE::TESWordOfPower> {

		//Absorb Shout
		Entry(GTSWordAb2,          GTSP, 0x9120CB); // "Vor"
		Entry(GTSWordAb3,          GTSP, 0x9120CC); // "Aete"

		//Calamity Shout
		Entry(GTSWordCatastrophe2, GTSP, 0x9120CF); // "Eind"
		Entry(GTSWordCatastrophe3, GTSP, 0x9120D0); // "Wor"

	};
}