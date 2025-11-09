#pragma once

namespace RuntimeData {

	struct Factions : iListable <RE::TESFaction> {

		Entry(FollowerFaction,         SKRM, 0x05C84E);
		Entry(DwarwenAutomatonFaction, SKRM, 0x043598);
		Entry(DraugrFaction,           SKRM, 0x02430D);

		Entry(SexLabAnimatingFaction,  SXLB, 0x00E50F);

	};
}