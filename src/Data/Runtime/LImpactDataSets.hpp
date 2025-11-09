#pragma once

namespace RuntimeData {

	struct ImpactDataSets : iListable <RE::BGSImpactDataSet> {

		Entry(GTSBloodSprayImpactSet,             GTSP, 0x01CEA7);
		Entry(GTSBloodSprayImpactSetVore,         GTSP, 0x024578);
		Entry(GTSBloodSprayImpactSetVoreMedium,   GTSP, 0x024579);
		Entry(GTSBloodSprayImpactSetVoreSmallest, GTSP, 0x02457A);

	};
}