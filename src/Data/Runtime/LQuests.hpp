#pragma once

namespace RuntimeData {

	struct Quests : iListable <RE::TESQuest> {

		Entry(GTSQuestProgression, GTSP, 0x005E3A);
		Entry(GTSQuestProxy,       GTSP, 0x8C104D);

	};
}