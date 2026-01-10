#pragma once

namespace GTS::AnimationVars {

	namespace Other {

		bool GetSpineRotationEnabled(RE::Actor* a_actor);
		bool SetSpineRotationEnabled(RE::Actor* a_actor, bool a_value);

		bool GetIsVanillaSneaking(RE::Actor* a_actor);
		bool SetIsVanillaSneaking(RE::Actor* a_actor, bool a_value);

		float GetStaggerMagnitude(RE::Actor* a_actor);
		bool SetStaggerMagnitude(RE::Actor* a_actor, float a_value);

		bool GetIsCollisionInstalled(RE::Actor* a_actor);
		bool SetIsCollisionInstalled(RE::Actor* a_actor, bool a_value);

		float GetCollisionPitchMult(RE::Actor* a_actor);
		bool SetCollisionPitchMult(RE::Actor* a_actor, float a_value);

		int GetCurrentDefaultState(RE::Actor* a_actor);
		bool SetCurrentDefaultState(RE::Actor* a_actor, int a_value);

		bool GetIsPandoraGenerated(RE::Actor* a_actor);
		bool GetIsNemesisGenerated(RE::Actor* a_actor);

	}

	namespace Utility {

		bool GetIsInstalled(RE::Actor* a_actor);
		bool SetIsInstalled(RE::Actor* a_actor, bool a_value);

	}

	namespace Stomp {

		bool GetEnableAlternativeStomp(RE::Actor* a_actor);
		bool SetEnableAlternativeStomp(RE::Actor* a_actor, bool a_value);
		
		bool GetIsAlternativeGrind(RE::Actor* a_actor);
		bool SetIsAlternativeGrind(RE::Actor* a_actor, bool a_value);

		bool GetIsTrampling(RE::Actor* a_actor);
		bool SetIsTrampling(RE::Actor* a_actor, bool a_value);

		bool GetStompBlend(RE::Actor* a_actor);
		bool SetStompBlend(RE::Actor* a_actor, float a_value);

	}

	namespace General {

		bool GetIsTransitioning(RE::Actor* a_actor);
		bool SetIsTransitioning(RE::Actor* a_actor, bool a_value);

		bool GetIsBusy(RE::Actor* a_actor);
		bool SetIsBusy(RE::Actor* a_actor, bool a_value);

		bool GetIsSwimming(RE::Actor* a_actor);
		bool SetIsSwimming(RE::Actor* a_actor, bool a_value);

		bool GetHHDisabled(RE::Actor* a_actor);
		bool SetHHDisabled(RE::Actor* a_actor, bool a_value);

		bool GetIsFollower(RE::Actor* a_actor);
		bool SetIsFollower(RE::Actor* a_actor, bool a_value);

		bool GetIsInDialogue(RE::Actor* a_actor);
		bool SetIsInDialogue(RE::Actor* a_actor, bool a_value);

		bool GetGender(RE::Actor* a_actor);
		bool SetGender(RE::Actor* a_actor, bool a_value);

		bool GetDisableSneakTransition(RE::Actor* a_actor);
		bool SetDisableSneakTransition(RE::Actor* a_actor, bool a_value);

		float GetGiantessVelocity(RE::Actor* a_actor);
		bool SetGiantessVelocity(RE::Actor* a_actor, float a_value);

		float GetGiantessScale(RE::Actor* a_actor);
		bool SetGiantessScale(RE::Actor* a_actor, float a_value);

		float GetSizeDifference(RE::Actor* a_actor);
		bool SetSizeDifference(RE::Actor* a_actor, float a_value);

		float GetPitchOverride(RE::Actor* a_actor);
		bool SetPitchOverride(RE::Actor* a_actor, float a_value);

		float GetHHOffset(RE::Actor* a_actor);
		bool SetHHOffset(RE::Actor* a_actor, float a_value);

		int GetDefState(RE::Actor* a_actor);
		bool SetDefState(RE::Actor* a_actor, int a_value);

	}

	namespace Hug {

		bool GetIsHugging(RE::Actor* a_actor);
		bool SetIsHugging(RE::Actor* a_actor, bool a_value);

		bool GetIsHugCrushing(RE::Actor* a_actor);
		bool SetIsHugCrushing(RE::Actor* a_actor, bool a_value);

		bool GetIsHugHealing(RE::Actor* a_actor);
		bool SetIsHugHealing(RE::Actor* a_actor, bool a_value);

		bool GetIsHugAbsorbing(RE::Actor* a_actor);
		bool SetIsHugAbsorbing(RE::Actor* a_actor, bool a_value);

		bool GetIsHuggingTeammate(RE::Actor* a_actor);
		bool SetIsHuggingTeammate(RE::Actor* a_actor, bool a_value);

		bool GetIsHugStarting(RE::Actor* a_actor);
		bool SetIsHugStarting(RE::Actor* a_actor, bool a_value);

		bool GetIsHasAbsorbedTiny(RE::Actor* a_actor);
		bool SetIsHasAbsorbedTiny(RE::Actor* a_actor, bool a_value);

	}


	namespace Action {

		bool GetIsThighCrushing(RE::Actor* a_actor);
		bool SetIsThighCrushing(RE::Actor* a_actor, bool a_value);

		bool GetIsThighSandwiching(RE::Actor* a_actor);
		bool SetIsThighSandwiching(RE::Actor* a_actor, bool a_value);

		bool GetIsSitting(RE::Actor* a_actor);
		bool SetIsSitting(RE::Actor* a_actor, bool a_value);

		bool GetIsStomping(RE::Actor* a_actor);
		bool SetIsStomping(RE::Actor* a_actor, bool a_value);

		bool GetIsFootGrinding(RE::Actor* a_actor);
		bool SetIsFootGrinding(RE::Actor* a_actor, bool a_value);

		bool GetIsVoring(RE::Actor* a_actor);
		bool SetIsVoring(RE::Actor* a_actor, bool a_value);

		bool GetIsKicking(RE::Actor* a_actor);
		bool SetIsKicking(RE::Actor* a_actor, bool a_value);

		bool GetIsBoobing(RE::Actor* a_actor);
		bool SetIsBoobing(RE::Actor* a_actor, bool a_value);

		bool GetCleavageOverrideZ(RE::Actor* a_actor);
		bool SetCleavageOverrideZ(RE::Actor* a_actor, bool a_value);

		bool GetIsInGrabPlayState(RE::Actor* a_actor);
		bool SetIsInGrabPlayState(RE::Actor* a_actor, bool a_value);

		bool GetIsKissing(RE::Actor* a_actor);
		bool SetIsKissing(RE::Actor* a_actor, bool a_value);

		bool GetIsGrabPlaying(RE::Actor* a_actor);
		bool SetIsGrabPlaying(RE::Actor* a_actor, bool a_value);

		bool GetAllowInterupt(RE::Actor* a_actor);
		bool SetAllowInterupt(RE::Actor* a_actor, bool a_value);

		bool GetIsButtState(RE::Actor* a_actor);
		bool SetIsButtState(RE::Actor* a_actor, bool a_value);

		bool GetIsButtGrinding(RE::Actor* a_actor);
		bool SetIsButtGrinding(RE::Actor* a_actor, bool a_value);

		bool GetIsUnderGrinding(RE::Actor* a_actor);
		bool SetIsUnderGrinding(RE::Actor* a_actor, bool a_value);

		bool GetIsUnderTrampling(RE::Actor* a_actor);
		bool SetIsUnderTrampling(RE::Actor* a_actor, bool a_value);

		bool GetIsStoringTiny(RE::Actor* a_actor);
		bool SetIsStoringTiny(RE::Actor* a_actor, bool a_value);

	}

	namespace Grab {

		bool GetIsGrabAttacking(RE::Actor* a_actor);
		bool SetIsGrabAttacking(RE::Actor* a_actor, bool a_value);

		bool GetHasGrabbedTiny(RE::Actor* a_actor);
		bool SetHasGrabbedTiny(RE::Actor* a_actor, bool a_value);

		bool GetGrabState(RE::Actor* a_actor);
		bool SetGrabState(RE::Actor* a_actor, bool a_value);

	}


	namespace Crawl {

		bool IsCrawling(RE::Actor* a_actor);

		bool GetIsCrawlEnabled(RE::Actor* a_actor);
		bool SetIsCrawlEnabled(RE::Actor* a_actor, bool a_value);

		bool GetIsCrawlVoring(RE::Actor* a_actor);
		bool SetIsCrawlVoring(RE::Actor* a_actor, bool a_value);

		bool GetIsHandStomping(RE::Actor* a_actor);
		bool SetIsHandStomping(RE::Actor* a_actor, bool a_value);

		bool GetIsHandStompingStrong(RE::Actor* a_actor);
		bool SetIsHandStompingStrong(RE::Actor* a_actor, bool a_value);

		bool GetIsCrawlButtCrush(RE::Actor* a_actor);
		bool SetIsCrawlButtCrush(RE::Actor* a_actor, bool a_value);

		bool GetIsNotCrawling(RE::Actor* a_actor);
		bool SetIsNotCrawling(RE::Actor* a_actor, bool a_value);

		bool GetIsHandAttacking(RE::Actor* a_actor);
		bool SetIsHandAttacking(RE::Actor* a_actor, bool a_value);

	}

	namespace Prone {

		bool IsProne(RE::Actor* a_actor);

		bool GetIsSBECrawling(RE::Actor* a_actor);
		bool SetIsSBECrawling(RE::Actor* a_actor, bool a_value);

		bool GetIsProneRolling(RE::Actor* a_actor);
		bool SetIsProneRolling(RE::Actor* a_actor, bool a_value);

		bool GetIsProneDiving(RE::Actor* a_actor);
		bool SetIsProneDiving(RE::Actor* a_actor, bool a_value);

	}

	namespace ButtCrush {

		bool GetIsButtCrushing(RE::Actor* a_actor);
		bool SetIsButtCrushing(RE::Actor* a_actor, bool a_value);

	}

	namespace Stomp {

		bool GetIsTrampling(RE::Actor* a_actor);
		bool SetIsTrampling(RE::Actor* a_actor, bool a_value);

		bool GetStompBlend(RE::Actor* a_actor);
		bool SetStompBlend(RE::Actor* a_actor, float a_value);
	}

	namespace Tiny {

		bool GetIsBeingHugged(RE::Actor* a_actor);
		bool SetIsBeingHugged(RE::Actor* a_actor, bool a_value);

		bool GetIsBeingGrinded(RE::Actor* a_actor);
		bool SetIsBeingGrinded(RE::Actor* a_actor, bool a_value);

		bool GetIsinBoobs(RE::Actor* a_actor);
		bool SetIsinBoobs(RE::Actor* a_actor, bool a_value);

		bool GetIsBeingSneakHugged(RE::Actor* a_actor);
		bool SetIsBeingSneakHugged(RE::Actor* a_actor, bool a_value);

		bool GetIsBeingCrawlHugged(RE::Actor* a_actor);
		bool SetIsBeingCrawlHugged(RE::Actor* a_actor, bool a_value);

		bool GetIsBeingShrunk(RE::Actor* a_actor);
		bool SetIsBeingShrunk(RE::Actor* a_actor, bool a_value);

		bool GetIsUnderButt(RE::Actor* a_actor);
		bool SetIsUnderButt(RE::Actor* a_actor, bool a_value);

		bool GetIsInThighs(RE::Actor* a_actor);
		bool SetIsInThighs(RE::Actor* a_actor, bool a_value);

	}

	namespace Growth {

		bool GetIsGrowing(RE::Actor* a_actor);
		bool SetIsGrowing(RE::Actor* a_actor, bool a_value);

		bool GetIsShrinking(RE::Actor* a_actor);
		bool SetIsShrinking(RE::Actor* a_actor, bool a_value);

		int GetGrowthRoll(RE::Actor* a_actor);
		bool SetGrowthRoll(RE::Actor* a_actor, int a_value);

		// Used to disallow growth/shrink during specific animations
		bool IsChangingSize(RE::Actor* a_actor);
	}

	namespace Spell {

		bool GetIsCastingShrink(RE::Actor* a_actor);
		bool SetIsCastingShrink(RE::Actor* a_actor, bool a_value);

	}

	namespace Cleavage {

		bool GetIsBoobsDoting(RE::Actor* a_actor);
		bool SetIsBoobsDoting(RE::Actor* a_actor, bool a_value);

		bool GetIsSuffocating(RE::Actor* a_actor);
		bool SetIsSuffocating(RE::Actor* a_actor, bool a_value);

		bool GetIsExitingStrangle(RE::Actor* a_actor);
		bool SetIsExitingStrangle(RE::Actor* a_actor, bool a_value);

	}

	namespace Hands {

		bool GetIsHandCrushing(RE::Actor* a_actor);
		bool SetIsHandCrushing(RE::Actor* a_actor, bool a_value);

		bool GetCanAim(RE::Actor* a_actor);
		bool SetCanAim(RE::Actor* a_actor, bool a_value);

	}

	

}

namespace GTS {

	bool IsHugCrushing(Actor* actor);
	bool IsHugHealing(Actor* actor);
	bool IsVoring(Actor* giant);
	bool IsHuggingFriendly(Actor* actor);
	bool IsTransitioning(Actor* actor);
	bool IsFootGrinding(Actor* actor);
	bool IsJumping(Actor* actor);
	bool IsTransferingTiny(Actor* actor);
	bool IsUsingThighAnimations(Actor* actor);
	bool IsSynced(Actor* actor);
	bool CanDoPaired(Actor* actor);
	bool IsThighCrushing(Actor* actor);
	bool IsThighSandwiching(Actor* actor);
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
	bool IsBeingGrinded(Actor* actor);
	bool IsHugging(Actor* actor);
	bool IsBeingHugged(Actor* actor);
	bool IsUsingAlternativeStomp(Actor* giant);

}
