#pragma once

namespace RuntimeData {

	struct Keywords : iListable <RE::BGSKeyword> {

		Entry(GTSKeywordBlackListActor,  GTSP, 0x6A85AF);
		Entry(GTSKeywordCountAsFollower, GTSP, 0x88E644);
		Entry(EnforceGiantessKeyword,    GTSP, 0x8C614E);

		Entry(UndeadKeyword,             SKRM, 0x013796);
		Entry(DwemerKeyword,             SKRM, 0x01397A);
		Entry(DragonKeyword,             SKRM, 0x035D59);
		Entry(VampireKeyword,            SKRM, 0x0A82BB);
		Entry(AnimalKeyword,             SKRM, 0x013798);
		Entry(CreatureKeyword,           SKRM, 0x013795);
		Entry(DarkSouls,                 SKRM, 0x0C1E8F);
		Entry(SummonUndead,              SKRM, 0x02482B);
		Entry(ActorTypeNPC,              SKRM, 0x013794);

	};
}
