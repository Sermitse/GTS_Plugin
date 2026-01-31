#pragma once

namespace GTS::AnimationVars {

	namespace Other {

		bool IsSpineRotationEnabled(RE::Actor* a_actor);
		bool IsVanillaSneaking(RE::Actor* a_actor);
		bool IsCollisionInstalled(RE::Actor* a_actor);
		bool IsPandoraGenerated(RE::Actor* a_actor);
		bool IsNemesisGenerated(RE::Actor* a_actor);
		bool IsJumping(RE::Actor* a_actor);
		bool IsSynched(RE::Actor* a_actor);

		int CurrentDefaultState(RE::Actor* a_actor);

		float StaggerMagnitude(RE::Actor* a_actor);
		float CollisionPitchMult(RE::Actor* a_actor);

		bool SetSpineRotationEnabled(RE::Actor* a_actor, bool a_value);
		bool SetVanillaSneaking(RE::Actor* a_actor, bool a_value);
		bool SetCurrentDefaultState(RE::Actor* a_actor, int a_value);
		bool SetStaggerMagnitude(RE::Actor* a_actor, float a_value);
		bool SetCollisionPitchMult(RE::Actor* a_actor, float a_value);

	}

	namespace Utility {

		bool BehaviorsInstalled(RE::Actor* a_actor);

	}

	namespace Stomp {

		bool IsAlternativeStompEnabled(RE::Actor* a_actor);
		bool IsAlternativeGrindEnabled(RE::Actor* a_actor);
		bool IsTrampling(RE::Actor* a_actor);

		float UnderStompBlend(RE::Actor* a_actor);

		bool SetAlternativeStompEnabled(RE::Actor* a_actor, bool a_value);
		bool SetIsAlternativeGrindEnabled(RE::Actor* a_actor, bool a_value);
		bool SetIsTrampling(RE::Actor* a_actor, bool a_value);

		bool SetUnderStompBlend(RE::Actor* a_actor, float a_value);
	}

	namespace General {

		bool IsTransitioning(RE::Actor* a_actor);
		bool IsBusy(RE::Actor* a_actor);
		bool IsGTSBusy(Actor* a_actor);
		bool IsSwimming(RE::Actor* a_actor);
		bool IsHHDisabled(RE::Actor* a_actor);
		bool IsFollower(RE::Actor* a_actor);
		bool IsInDialogue(RE::Actor* a_actor);
		bool Gender(RE::Actor* a_actor);
		bool SneakTransitionsDisabled(RE::Actor* a_actor);
		bool CanDoPaired(RE::Actor* a_actor);
		bool CanCombo(RE::Actor* a_actor);

		float GiantessVelocity(RE::Actor* a_actor);
		float GiantessScale(RE::Actor* a_actor);
		float SizeDifference(RE::Actor* a_actor);
		float PitchOverride(RE::Actor* a_actor);
		float HHOffset(RE::Actor* a_actor);

		int DefState(RE::Actor* a_actor);

		bool SetIsTransitioning(RE::Actor* a_actor, bool a_value);
		bool SetIsBusy(RE::Actor* a_actor, bool a_value);
		bool SetIsSwimming(RE::Actor* a_actor, bool a_value);
		bool SetHHDisabled(RE::Actor* a_actor, bool a_value);
		bool SetIsFollower(RE::Actor* a_actor, bool a_value);
		bool SetIsInDialogue(RE::Actor* a_actor, bool a_value);
		bool SetGender(RE::Actor* a_actor, bool a_value);
		bool SetSneakTransitionsDisabled(RE::Actor* a_actor, bool a_value);

		bool SetDefState(RE::Actor* a_actor, int a_value);

		bool SetGiantessVelocity(RE::Actor* a_actor, float a_value);
		bool SetGiantessScale(RE::Actor* a_actor, float a_value);
		bool SetSizeDifference(RE::Actor* a_actor, float a_value);
		bool SetPitchOverride(RE::Actor* a_actor, float a_value);
		bool SetHHOffset(RE::Actor* a_actor, float a_value);

	}

	namespace Hug {

		bool IsHugging(RE::Actor* a_actor);
		bool IsHugCrushing(RE::Actor* a_actor);
		bool IsHugHealing(RE::Actor* a_actor);
		bool IsHugAbsorbing(RE::Actor* a_actor);
		bool IsHuggingTeammate(RE::Actor* a_actor);
		bool IsHugStarting(RE::Actor* a_actor);
		bool IsHasAbsorbedTiny(RE::Actor* a_actor);

		bool SetIsHugging(RE::Actor* a_actor, bool a_value);
		bool SetIsHugCrushing(RE::Actor* a_actor, bool a_value);
		bool SetIsHugHealing(RE::Actor* a_actor, bool a_value);
		bool SetIsHugAbsorbing(RE::Actor* a_actor, bool a_value);
		bool SetIsHuggingTeammate(RE::Actor* a_actor, bool a_value);
		bool SetIsHugStarting(RE::Actor* a_actor, bool a_value);
		bool SetIsHasAbsorbedTiny(RE::Actor* a_actor, bool a_value);
	}


	namespace Action {

		bool IsThighCrushing(RE::Actor* a_actor);
		bool IsThighSandwiching(RE::Actor* a_actor);
		bool IsSitting(RE::Actor* a_actor);
		bool IsStomping(RE::Actor* a_actor);
		bool IsFootGrinding(RE::Actor* a_actor);
		bool IsVoring(RE::Actor* a_actor);
		bool IsKicking(RE::Actor* a_actor);
		bool IsInCleavageState(RE::Actor* a_actor);
		bool IsCleavageZOverrideEnabled(RE::Actor* a_actor);
		bool IsInGrabPlayState(RE::Actor* a_actor);
		bool IsKissing(RE::Actor* a_actor);
		bool IsGrabPlaying(RE::Actor* a_actor);
		bool IsInSecondSandwichBranch(RE::Actor* a_actor);
		bool IsThighGrinding(RE::Actor* a_actor);
		bool IsUnderGrinding(RE::Actor* a_actor);
		bool IsUnderTrampling(RE::Actor* a_actor);
		bool IsStoringTiny(RE::Actor* a_actor);

		bool AllowInterupt(RE::Actor* a_actor);

		bool SetIsThighCrushing(RE::Actor* a_actor, bool a_value);
		bool SetIsThighSandwiching(RE::Actor* a_actor, bool a_value);
		bool SetIsSitting(RE::Actor* a_actor, bool a_value);
		bool SetIsStomping(RE::Actor* a_actor, bool a_value);
		bool SetIsFootGrinding(RE::Actor* a_actor, bool a_value);
		bool SetIsVoring(RE::Actor* a_actor, bool a_value);
		bool SetIsKicking(RE::Actor* a_actor, bool a_value);
		bool SetIsBoobing(RE::Actor* a_actor, bool a_value);
		bool SetIsCleavageZOverrideEnabled(RE::Actor* a_actor, bool a_value);
		bool SetIsInGrabPlayState(RE::Actor* a_actor, bool a_value);
		bool SetIsKissing(RE::Actor* a_actor, bool a_value);
		bool SetIsGrabPlaying(RE::Actor* a_actor, bool a_value);
		bool SetIsThighGrinding(RE::Actor* a_actor, bool a_value);
		bool SetIsUnderGrinding(RE::Actor* a_actor, bool a_value);
		bool SetIsUnderTrampling(RE::Actor* a_actor, bool a_value);
		bool SetIsStoringTiny(RE::Actor* a_actor, bool a_value);

		bool SetAllowInterupt(RE::Actor* a_actor, bool a_value);

	}

	namespace Grab {

		bool IsGrabAttacking(RE::Actor* a_actor);
		bool HasGrabbedTiny(RE::Actor* a_actor);
		bool GrabState(RE::Actor* a_actor);

		bool SetIsGrabAttacking(RE::Actor* a_actor, bool a_value);
		bool SetHasGrabbedTiny(RE::Actor* a_actor, bool a_value);
		bool SetGrabState(RE::Actor* a_actor, bool a_value);

	}


	namespace Crawl {

		bool IsCrawling(RE::Actor* a_actor);
		bool IsCrawlEnabled(RE::Actor* a_actor);
		bool IsCrawlVoring(RE::Actor* a_actor);
		bool IsHandStomping(RE::Actor* a_actor);
		bool IsHandStompingStrong(RE::Actor* a_actor);
		bool IsCrawlButtCrush(RE::Actor* a_actor);
		bool IsNotCrawling(RE::Actor* a_actor);
		bool IsHandAttacking(RE::Actor* a_actor);

		bool SetIsCrawlEnabled(RE::Actor* a_actor, bool a_value);
		bool SetIsCrawlVoring(RE::Actor* a_actor, bool a_value);
		bool SetIsHandStomping(RE::Actor* a_actor, bool a_value);
		bool SetIsHandStompingStrong(RE::Actor* a_actor, bool a_value);
		bool SetIsCrawlButtCrush(RE::Actor* a_actor, bool a_value);
		bool SetIsNotCrawling(RE::Actor* a_actor, bool a_value);
		bool SetIsHandAttacking(RE::Actor* a_actor, bool a_value);
	}

	namespace Prone {

		bool IsProne(RE::Actor* a_actor);
		bool IsSBECrawling(RE::Actor* a_actor);
		bool IsProneRolling(RE::Actor* a_actor);
		bool IsProneDiving(RE::Actor* a_actor);

		bool SetIsSBECrawling(RE::Actor* a_actor, bool a_value);
		bool SetIsProneRolling(RE::Actor* a_actor, bool a_value);
		bool SetIsProneDiving(RE::Actor* a_actor, bool a_value);

	}

	namespace ButtCrush {

		bool IsButtCrushing(RE::Actor* a_actor);

		bool SetIsButtCrushing(RE::Actor* a_actor, bool a_value);

	}

	namespace Tiny {

		bool IsBeingGrinded(RE::Actor* a_actor);
		bool IsBeingHugged(RE::Actor* a_actor);
		bool IsInBoobs(RE::Actor* a_actor);
		bool IsBeingSneakHugged(RE::Actor* a_actor);
		bool IsBeingCrawlHugged(RE::Actor* a_actor);
		bool IsBeingShrunk(RE::Actor* a_actor);
		bool IsUnderButt(RE::Actor* a_actor);
		bool IsInThighs(RE::Actor* a_actor);

		bool SetIsBeingHugged(RE::Actor* a_actor, bool a_value);
		bool SetIsBeingSneakHugged(RE::Actor* a_actor, bool a_value);
		bool SetIsBeingCrawlHugged(RE::Actor* a_actor, bool a_value);
		bool SetIsBeingShrunk(RE::Actor* a_actor, bool a_value);
		bool SetIsUnderButt(RE::Actor* a_actor, bool a_value);
		bool SetIsInThighs(RE::Actor* a_actor, bool a_value);

	}

	namespace Growth {

		bool IsGrowing(RE::Actor* a_actor);
		bool IsShrinking(RE::Actor* a_actor);
		bool IsChangingSize(RE::Actor* a_actor);

		int GrowthRoll(RE::Actor* a_actor);

		bool SetIsGrowing(RE::Actor* a_actor, bool a_value);
		bool SetIsShrinking(RE::Actor* a_actor, bool a_value);

		bool SetGrowthRoll(RE::Actor* a_actor, int a_value);
	}

	namespace Spell {

		bool IsCastingShrink(RE::Actor* a_actor);

		bool SetIsCastingShrink(RE::Actor* a_actor, bool a_value);

	}

	namespace Cleavage {

		bool IsBoobsDoting(RE::Actor* a_actor);
		bool IsSuffocating(RE::Actor* a_actor);
		bool IsExitingStrangle(RE::Actor* a_actor);

		bool SetIsBoobsDoting(RE::Actor* a_actor, bool a_value);
		bool SetIsSuffocating(RE::Actor* a_actor, bool a_value);
		bool SetIsExitingStrangle(RE::Actor* a_actor, bool a_value);

	}

	namespace Hands {

		bool IsHandCrushing(RE::Actor* a_actor);
		bool CanAim(RE::Actor* a_actor);

		bool SetIsHandCrushing(RE::Actor* a_actor, bool a_value);
		bool SetCanAim(RE::Actor* a_actor, bool a_value);

	}

}

