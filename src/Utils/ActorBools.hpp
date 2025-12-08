#pragma once

namespace GTS {

	bool IsStaggered(Actor* tiny);
	bool IsHandStomping_L(Actor* actor);
	bool IsHandStomping_H(Actor* actor);
	bool IsBeingKilledWithMagic(Actor* tiny);
	bool IsGrabAttacking(Actor* actor);
	bool IsInSecondSandwichBranch(Actor* actor);
	bool IsThighGrinding(Actor* actor);
	bool IsStrangling(Actor* giant);
	bool IsExitingStrangle(Actor* giant);
	bool IsUnderGrinding(Actor* giant);
	bool IsUnderTrampling(Actor* giant);
	bool IsInGrabPlayState(Actor* giant);
	bool IsInSexlabAnim(Actor* actor_1, Actor* actor_2);
	bool DontAlterDamage(Actor* a_this, float dmg, float Health_Reduction);
	bool IsHumanoid(Actor* giant);
	bool CountAsGiantess(Actor* giant);
	bool IsVisible(Actor* giant);
	bool HasHeadTrackingTarget(Actor* giant);
	bool KnockedDown(Actor* giant);
	bool IsinRagdollState(Actor* giant);
    /*bool BehaviorGraph_DisableHH(Actor* actor);
	bool IsEquipBusy(Actor* actor);
	
	bool IsRagdolled(Actor* actor);
	bool IsGrowing(Actor* actor);
	bool IsChangingSize(Actor* actor);

	bool IsFootGrinding(Actor* actor);
	bool IsProning(Actor* actor);
	bool IsCrawling(Actor* actor);
	bool IsInBalanceMode();
	bool IsHugCrushing(Actor* actor);
	bool IsHugHealing(Actor* actor);
	bool IsVoring(Actor* giant);
	bool IsHuggingFriendly(Actor* actor);
	bool IsTransitioning(Actor* actor);
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
	bool GetCameraOverride(Actor* actor);
	// GTS State Bools End

	// Gts Bools
	bool IsGrowthSpurtActive(Actor* actor);
	bool HasGrowthSpurt(Actor* actor);
	bool InBleedout(Actor* actor);
	bool AllowStagger(Actor* giant, Actor* tiny);
	bool IsMechanical(Actor* actor);
	bool IsHuman(Actor* actor);
	bool IsBlacklisted(Actor* actor);

    bool IsGtsTeammate(Actor* actor);*/
}