#pragma once

namespace GTS {

	bool IsBeingHeld(Actor* giant, Actor* tiny);
	bool IsBetweenBreasts(Actor* actor);

	void Attachment_SetTargetNode(Actor* giant, AttachToNode Node);
	AttachToNode Attachment_GetTargetNode(Actor* giant);

	void SetBusyFoot(Actor* giant, BusyFoot Foot);
	BusyFoot GetBusyFoot(Actor* giant);

	Actor* GetPlayerOrControlled();
	void ControlAnother(Actor* target, bool reset);

	void SetProneState(Actor* giant, bool enable);
	float GetProneAdjustment();

	void SpawnActionIcon(Actor* giant);

	void SetBeingHeld(Actor* tiny, bool enable);
	void SetBetweenBreasts(Actor* actor, bool enable);
	void SetBeingEaten(Actor* tiny, bool enable);
	void SetBeingGrinded(Actor* tiny, bool enable);
	void ResetGrab(Actor* giant);
	void RecordSneaking(Actor* actor);
	void UpdateCrawlState(Actor* actor);
	void UpdateFootStompType(RE::Actor* a_actor);
	void UpdateSneakTransition(RE::Actor* a_actor);

	float GetButtCrushCost(Actor* actor, bool DoomOnly);
	bool IsGrowthSpurtActive(Actor* actor);
	bool HasGrowthSpurt(Actor* actor);

	std::vector<Actor*> GetMaxActionableTinyCount(Actor* giant, const std::vector<Actor*>& actors);
	bool CanPerformActionOn(Actor* giant, Actor* tiny, bool HugCheck);
}