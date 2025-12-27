#pragma once

namespace GTS {

	void Attachment_SetTargetNode(Actor* giant, AttachToNode Node);

	AttachToNode Attachment_GetTargetNode(Actor* giant);

	void SetBusyFoot(Actor* giant, BusyFoot Foot);

	BusyFoot GetBusyFoot(Actor* giant);

	void ControlAnother(Actor* target, bool reset);

	std::vector<Actor*> GetMaxActionableTinyCount(Actor* giant, const std::vector<Actor*>& actors);

	float GetProneAdjustment();

	void SpawnActionIcon(Actor* giant);

	void SetBeingHeld(Actor* tiny, bool enable);

	void SetProneState(Actor* giant, bool enable);

	void SetBetweenBreasts(Actor* actor, bool enable);

	void SetBeingEaten(Actor* tiny, bool enable);

	void SetBeingGrinded(Actor* tiny, bool enable);

	void ResetGrab(Actor* giant);

	void UpdateCrawlState(Actor* actor);

	void UpdateFootStompType(RE::Actor* a_actor);

	void UpdateSneakTransition(RE::Actor* a_actor);

	float GetButtCrushCost(Actor* actor, bool DoomOnly);

	bool IsGrowthSpurtActive(Actor* actor);

	bool HasGrowthSpurt(Actor* actor);

	void RecordSneaking(Actor* actor);

	Actor* GetPlayerOrControlled();

	bool IsUsingAlternativeStomp(Actor* giant);

	//RenameTo CanPerformActionOn
	bool CanPerformAnimationOn(Actor* giant, Actor* tiny, bool HugCheck);
}