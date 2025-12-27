#pragma once

namespace GTS {

	bool IsStaggered(Actor* tiny);

	bool IsInSexlabAnim(Actor* actor_1, Actor* actor_2);

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

	bool IsDebugEnabled();

	bool CanDoDamage(Actor* giant, Actor* tiny, bool HoldCheck);

	bool AllowDevourment();

	bool AllowCameraTracking();

	bool LessGore();

	bool IsTeammate(Actor* actor);

	bool IsEquipBusy(Actor* actor);

	bool IsRagdolled(Actor* actor);

	bool IsGrowing(Actor* actor);

	bool IsChangingSize(Actor* actor);

	bool IsProning(Actor* actor);

	bool IsCrawling(Actor* actor);

	bool IsHugCrushing(Actor* actor);

	bool IsHugHealing(Actor* actor);

	bool IsVoring(Actor* giant);

	bool IsHuggingFriendly(Actor* actor);

	bool IsTransitioning(Actor* actor);

	bool IsFootGrinding(Actor* actor);

	bool IsJumping(Actor* actor);

	bool IsBeingHeld(Actor* giant, Actor* tiny);

	bool IsBetweenBreasts(Actor* actor);

	bool IsTransferingTiny(Actor* actor);

	bool IsUsingThighAnimations(Actor* actor);

	bool IsSynced(Actor* actor);

	bool CanDoPaired(Actor* actor);

	bool IsThighCrushing(Actor* actor);

	bool IsThighSandwiching(Actor* actor);

	bool IsBeingEaten(Actor* tiny);

	bool IsGtsBusy(Actor* actor);

	bool IsStomping(Actor* actor);

	bool IsInCleavageState(Actor* actor);

	bool IsCleavageZIgnored(Actor* actor);

	bool IsInsideCleavage(Actor* actor);

	bool IsKicking(Actor* actor);

	bool IsTrampling(Actor* actor);

	bool CanDoCombo(Actor* actor);

	bool IsCameraEnabled(Actor* actor);

	bool IsCrawlVoring(Actor* actor);

	bool IsButtCrushing(Actor* actor);

	bool ButtCrush_IsAbleToGrow(Actor* actor, float limit);

	bool IsBeingGrinded(Actor* actor);

	bool IsHugging(Actor* actor);

	bool IsBeingHugged(Actor* actor);

	bool CanDoButtCrush(Actor* actor, bool apply_cooldown);

	bool InBleedout(Actor* actor);

	bool IsMechanical(Actor* actor);

	bool IsHuman(Actor* actor);

	bool IsBlacklisted(Actor* actor);

	bool IsGtsTeammate(Actor* actor);

	bool HasSMT(Actor* giant);

}