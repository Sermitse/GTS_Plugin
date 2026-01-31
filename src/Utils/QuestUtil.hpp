#pragma once

namespace GTS {

	enum class QuestAnimationType {
		kHugs            = 0,
		kStompsAndKicks  = 1,
		kGrabAndSandwich = 2,
		kVore            = 3,
		kOthers          = 4,
	};

	void SkipProgressionQuest();
	void GiveAllPerksToPlayer();
	bool ProgressionQuestCompleted();
	void GiveAllSpellsToPlayer();
	void GiveAllShoutsToPlayer();
	float GetQuestProgression(int stage);
	void ResetQuest();
	void AdvanceQuestProgression(Actor* giant, Actor* tiny, QuestStage stage, float value, bool vore);
	bool CanDoActionBasedOnQuestProgress(Actor* giant, QuestAnimationType type);
	void CompleteDragonQuest(Actor* tiny, ParticleType Type);

}