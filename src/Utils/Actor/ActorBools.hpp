#pragma once

namespace GTS {

	bool IsInSexlabAnim(Actor* actor_1, Actor* actor_2);
	bool IsStaggered(Actor* tiny);
	bool IsHumanoid(Actor* giant);
	bool CountAsGiantess(Actor* giant);
	bool IsVisible(Actor* giant);
	bool HasHeadTrackingTarget(Actor* giant);
	bool KnockedDown(Actor* giant);
	bool IsinRagdollState(Actor* giant);
	bool IsInsect(Actor* actor, bool performcheck);
	bool IsFemale(Actor* a_Actor, bool AllowOverride = false);
	bool IsDragon(Actor* actor);
	bool IsGiant(Actor* actor);
	bool IsMammoth(Actor* actor);
	bool IsLiving(Actor* actor);
	bool IsUndead(Actor* actor, bool PerformCheck);
	bool WasReanimated(Actor* actor);
	bool IsFlying(Actor* actor);
	bool IsHostile(Actor* giant, Actor* tiny);
	bool IsEssential(Actor* giant, Actor* actor);
	bool IsHeadtracking(Actor* giant);
	bool IsInGodMode(Actor* giant);
	bool CanDoDamage(Actor* giant, Actor* tiny, bool HoldCheck);
	bool IsTeammate(Actor* actor);
	bool IsEquipBusy(Actor* actor);
	bool IsRagdolled(Actor* actor);
	bool InBleedout(Actor* actor);
	bool IsMechanical(Actor* actor);
	bool IsHuman(Actor* actor);
	bool IsBlacklisted(Actor* actor);
	bool IsGTSTeammate(Actor* actor);
	bool TinyCalamityActive(Actor* giant);

}