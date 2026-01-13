#pragma once

#include "Managers/Animation/Grab.hpp"
#include "Managers/Animation/HugShrink.hpp"

#include "Utils/Animation/AnimationVars.hpp"

/* 
	Input Conditions.
	If a condition returns true it assumed a bound action can play or some other form of feedback will be displayed.
	eg. A cooldown message.
	If false is returned. Input manager assumes the bound action even if called won't do anyhing.
	effectively the idea is to move all the check logic here and only keep the cooldown/doing the action in the callback.
	
	If true is returned inputmanager assumes the bound trigger will lead to an action and thus will block the relevant key inputs from being passed on to the game.

*/

namespace GTS {

	//---------------------
	// ButtCrush
	//---------------------

	static bool ButtCrushCondition_Start() {
		Actor* target = GetPlayerOrControlled();
		auto player = PlayerCharacter::GetSingleton();

		if (IsPlayerFirstPerson(target) || AnimationVars::General::IsGTSBusy(target) || AnimationVars::Growth::IsChangingSize(target) || !CanDoActionBasedOnQuestProgress(target, QuestAnimationType::kGrabAndSandwich)) {
			return false;
		}

		auto grabbedActor = Grab::GetHeldActor(target);
		if (grabbedActor && !AnimationVars::Crawl::IsCrawling(target)) { // IF we have someone in hands, allow only when we crawl
			return false;
		}

		if (!target->IsPlayerRef()) {
			if (IsBeingHeld(target, player)) {
				return false;
			}
		}

		return true;
	}

	static bool ButtCrushCondition_Grow() {
		Actor* target = GetPlayerOrControlled();
		if (IsPlayerFirstPerson(target)) {
			return false;
		}
		if (AnimationVars::ButtCrush::IsButtCrushing(target) && !AnimationVars::Growth::IsChangingSize(target) && Runtime::HasPerkTeam(target, Runtime::PERK.GTSPerkButtCrushAug2)) {
			return true;
		}
		return false;
	}

	static bool ButtCrushCondition_Attack() {
		Actor* target = GetPlayerOrControlled();
		if (AnimationVars::ButtCrush::IsButtCrushing(target)) {
			return true;
		}
		return false;
	}


	//---------------------
	// Cleavage
	//---------------------


	static bool CleavageCondition() {
		Actor* target = GetPlayerOrControlled();
		if (target) {
			if (Runtime::HasPerkTeam(target, Runtime::PERK.GTSPerkBreastsIntro)) {
				Actor* tiny = Grab::GetHeldActor(target);
				if (tiny && IsBetweenBreasts(tiny)) {
					return true;
				}
			}
		}
		return false;
	}

	static bool CleavageDOTCondition() {
		Actor* target = GetPlayerOrControlled();
		if (target) {
			if (Runtime::HasPerkTeam(target, Runtime::PERK.GTSPerkBreastsStrangle)) {
				Actor* tiny = Grab::GetHeldActor(target);
				if (AnimationVars::Action::IsInCleavageState(target) && tiny && IsBetweenBreasts(tiny)) {
					return true;
				}
			}
		}
		return false;
	}

	//---------------------
	// Swipe
	//---------------------

	static bool SwipeCondition() {
		auto target = PlayerCharacter::GetSingleton();
		if (!CanDoActionBasedOnQuestProgress(target, QuestAnimationType::kStompsAndKicks) || AnimationVars::General::IsGTSBusy(target)) {
			return false;
		}
		if (!target->IsSneaking()) {
			return false;
		}
		return true;
	}

	//---------------------
	// Trample
	//---------------------

	static bool TrampleCondition() {
		auto target = PlayerCharacter::GetSingleton();
		if (!CanDoActionBasedOnQuestProgress(target, QuestAnimationType::kStompsAndKicks) || AnimationVars::General::IsGTSBusy(target)) {
			return false;
		}
		if (AnimationVars::Crawl::IsCrawling(target) || target->IsSneaking() || AnimationVars::Prone::IsProne(target)) {
			return false;
		}
		return true;
	}

	//---------------------
	// Hug
	//---------------------

	static bool HugCondition_Start() {
		auto target = GetPlayerOrControlled();
		if (!CanDoActionBasedOnQuestProgress(target, QuestAnimationType::kHugs)) {
			return false;
		}
		if (AnimationVars::General::IsGTSBusy(target)) {
			return false;
		}
		if (AnimationVars::General::CanDoPaired(target) && !AnimationVars::Other::IsSynched(target) && !AnimationVars::Grab::HasGrabbedTiny(target)) {
			return true;
		}
		return false;
	}

	static bool HugCondition_Action() {
		Actor* target = GetPlayerOrControlled();
		auto huggedActor = HugShrink::GetHuggiesActor(target);
		if (!huggedActor) {
			return false;
		}
		return true;
	}

	static bool HugCondition_Release() {
		Actor* target = GetPlayerOrControlled();
		auto huggedActor = HugShrink::GetHuggiesActor(target);
		if (!huggedActor || AnimationVars::Hug::IsHugCrushing(target) || AnimationVars::Hug::IsHugHealing(target)) {
			return false;
		}
		return true;
	}

	//------------------
	// Size Reseve
	//------------------

	static bool SizeReserveCondition() {
		/*auto target = PlayerCharacter::GetSingleton();
		return Runtime::HasPerk(target, Runtime::PERK.GTSPerkSizeReserve) && Persistent::GetActorData(target);
		*/
		return true;
	}

	//---------------------
	// Rappid Grow/Shrink
	//---------------------

	static bool RappidGrowShrinkCondition() {
		auto target = PlayerCharacter::GetSingleton();

		if (!Runtime::HasPerk(target, Runtime::PERK.GTSPerkGrowthDesireAug)) {
			return false;
		}

		return true;
	}


	//---------------------
	// Shrink Outburst
	//---------------------

	static bool ShrinkOutburstCondition() {
		auto target = PlayerCharacter::GetSingleton();
		if (target) {
			bool DarkArts = Runtime::HasPerk(target, Runtime::PERK.GTSPerkDarkArts);
			if (!DarkArts) {
				return false; // no perk, do nothing
			}
		}
		return true;
	}

	//---------------------
	// Protect Small Ones
	//---------------------

	static bool ProtectSmallOnesCondition() {
		auto target = PlayerCharacter::GetSingleton();

		if (target && CanDoActionBasedOnQuestProgress(target, QuestAnimationType::kOthers)) {
			return true;
		}
		return false;
	}



	//---------------------------
	// Total Control Grow/Shrink
	//---------------------------

	static bool TotalControlCondition() {
		auto target = PlayerCharacter::GetSingleton();

		if (!Runtime::HasPerk(target, Runtime::PERK.GTSPerkGrowthDesireAug)) {
			return false;
		}
		return true;
	}


	//---------------------------
	// Kicks
	//---------------------------

	static bool KickCondition() {
		auto target = PlayerCharacter::GetSingleton();
		if (!CanDoActionBasedOnQuestProgress(target, QuestAnimationType::kStompsAndKicks) || AnimationVars::General::IsGTSBusy(target)) {
			return false;
		}

		if (!target->IsSneaking() && !target->AsActorState()->IsSprinting()) {
			return true;
		}
		return false;
	}

	//---------------------------
	// Stomps
	//---------------------------

	static bool StompCondition() {
		auto target = PlayerCharacter::GetSingleton();
		if (!CanDoActionBasedOnQuestProgress(target, QuestAnimationType::kStompsAndKicks) || AnimationVars::General::IsGTSBusy(target)) {
			return false;
		}

		return true;
	}

	//---------------------------
	// ThighCrush
	//---------------------------

	static bool ThighCrushCondition_Start() {
		auto target = PlayerCharacter::GetSingleton();
		if (!CanDoActionBasedOnQuestProgress(target, QuestAnimationType::kGrabAndSandwich)) {
			return false;
		}
		return true;
	}

	//---------------------------
	// ThighSandwich
	//---------------------------

	static bool ThighSandwitchCondition_Start() {
		auto target = PlayerCharacter::GetSingleton();


		if (!CanDoActionBasedOnQuestProgress(target, QuestAnimationType::kGrabAndSandwich)) {
			return false;
		}
		if (AnimationVars::General::IsGTSBusy(target)) {
			return false;
		}
		if (AnimationVars::Crawl::IsCrawling(target)) {
			return false;
		}

		return true;
	}

	static bool ThighSandwichGrind_Start() {
		auto target = PlayerCharacter::GetSingleton();
		if (!AnimationVars::Action::IsInSecondSandwichBranch(target) || AnimationVars::Action::IsThighGrinding(target)) {
			return false;
		}
		return true;
	}
	static bool ThighSandwichGrind() {
		auto target = PlayerCharacter::GetSingleton();
		if (!AnimationVars::Action::IsThighGrinding(target)) {
			return false;
		}
		return true;
	}

	static bool SecondThighSandwichBranch_Start() {
		auto target = PlayerCharacter::GetSingleton();
		if (AnimationVars::Action::IsInSecondSandwichBranch(target) || !AnimationVars::Action::IsThighSandwiching(target)) {
			return false;
		}
		return true;
	}

	static bool SecondThighSandwichBranch() {
		auto target = PlayerCharacter::GetSingleton();
		if (!AnimationVars::Action::IsInSecondSandwichBranch(target)) {
			return false;
		}
		return true;
	}

	static bool UBCondition() {
		auto target = PlayerCharacter::GetSingleton();
		if (!AnimationVars::Action::IsThighSandwiching(target)) {
			return false;
		}
		return true;
	}

	//---------------------------
	// Grab
	//---------------------------

	static bool GrabCondition_Start() {
		auto target = GetPlayerOrControlled();
		auto grabbedActor = Grab::GetHeldActor(target);
		if (grabbedActor) { //If we have actor, don't pick anyone up.
			return false;
		}
		if (!CanDoActionBasedOnQuestProgress(target, QuestAnimationType::kStompsAndKicks)) {
			return false;
		}
		if (AnimationVars::General::IsGTSBusy(target) || IsEquipBusy(target) || AnimationVars::General::IsTransitioning(target)) {
			return false; // Disallow Grabbing if Behavior is busy doing other stuff.
		}
		return true;
	}

	static bool GrabCondition_Attack() {
		auto target = GetPlayerOrControlled();

		if (AnimationVars::General::IsGTSBusy(target) && !AnimationVars::Action::IsSitting(target)) {
			return false;
		}
		if (AnimationVars::Action::IsStomping(target) && AnimationVars::General::IsTransitioning(target)) {
			return false;
		}
		return true;
	}

	static bool GrabCondition_Vore() {
		auto target = GetPlayerOrControlled();

		if (!CanDoActionBasedOnQuestProgress(target, QuestAnimationType::kVore)) {
			return false;
		}
		if (AnimationVars::General::IsGTSBusy(target) && !AnimationVars::Action::IsSitting(target)) {
			return false;
		}

		if (!AnimationVars::General::IsTransitioning(target)) {
			auto grabbedActor = Grab::GetHeldActor(target);
			if (!grabbedActor) {
				return false;
			}
			if (IsInsect(grabbedActor, true) || IsBlacklisted(grabbedActor) || IsUndead(grabbedActor, true)) {
				return false; // Same rules as with Vore
			}
			return true;
		}
		return false;
	}

	static bool GrabCondition_Throw() {
		auto target = GetPlayerOrControlled();

		if (AnimationVars::General::IsGTSBusy(target) && !AnimationVars::Action::IsSitting(target)) {
			return false;
		}

		if (!AnimationVars::General::IsTransitioning(target)) {
			auto grabbedActor = Grab::GetHeldActor(target);
			if (!grabbedActor) {
				return false;
			}
			return true;
		}
		return false;
	}


	static bool GrabCondition_Release() {
		auto target = GetPlayerOrControlled();

		auto grabbedActor = Grab::GetHeldActor(target);
		if (!grabbedActor) {
			return false;
		}
		if (AnimationVars::General::IsGTSBusy(target) && !AnimationVars::Action::IsSitting(target) || AnimationVars::General::IsTransitioning(target)) {
			return false;
		}
		if (!target->AsActorState()->IsWeaponDrawn()) {
			return true;
		}

		return false;
	}

	static bool GrabCondition_Breasts() {
		auto target = GetPlayerOrControlled();

		auto grabbedActor = Grab::GetHeldActor(target);
		if (!grabbedActor || AnimationVars::General::IsTransitioning(target)) {
			return false;
		}

		return true;
	}


	//---------------------------
	// Vore
	//---------------------------

	static bool VoreCondition() {
		auto target = GetPlayerOrControlled();

		if (!CanDoActionBasedOnQuestProgress(target, QuestAnimationType::kVore)) {
			return false;
		}
		return true;
	}


	//------------------------------
	// Grab Play
	//------------------------------

	static bool GrabPlayStartCondition() {
		auto target = GetPlayerOrControlled();

		if (Grab::GetHeldActor(target)) {
			if (!IsHumanoid(Grab::GetHeldActor(target))) {
				if (target->IsPlayerRef()) {
					std::string_view message = std::format("You don't want to play with {}", Grab::GetHeldActor(target)->GetDisplayFullName());
					NotifyWithSound(target, message);
				}
				return false;
			}
			return true;
		}
		return true;
	}

	static bool GrabPlayActionCondition() {
		auto target = GetPlayerOrControlled();
		if (!AnimationVars::Action::IsInGrabPlayState(target)) {
			return false;
		}
		return true;
	}

	//------------------------------
	// Follower Specific Ones
	//------------------------------

	static bool ButtCrushCondition_Follower() {
		auto player = PlayerCharacter::GetSingleton();

		if (!CanDoActionBasedOnQuestProgress(player, QuestAnimationType::kGrabAndSandwich)) {
			return false;
		}
		return true;
	}

	static bool GrabCondition_Follower() {
		auto player = PlayerCharacter::GetSingleton();

		if (!CanDoActionBasedOnQuestProgress(player, QuestAnimationType::kGrabAndSandwich)) {
			return false;
		}
		return true;
	}

	static bool HugCondition_Follower() {
		auto player = PlayerCharacter::GetSingleton();

		if (!CanDoActionBasedOnQuestProgress(player, QuestAnimationType::kHugs)) {
			return false;
		}
		return true;
	}

	static bool ThighSandwitchCondition_Follower() {
		auto player = PlayerCharacter::GetSingleton();

		if (!CanDoActionBasedOnQuestProgress(player, QuestAnimationType::kGrabAndSandwich)) {
			return false;
		}
		return true;
	}

	static bool VoreCondition_Follower() {
		auto player = PlayerCharacter::GetSingleton();

		if (!CanDoActionBasedOnQuestProgress(player, QuestAnimationType::kVore)) {
			return false;
		}

		return true;
	}

	static inline bool AlwaysBlock() {
		return true;
	}
}
