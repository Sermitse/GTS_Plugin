#include "managers/animation/Utils/CooldownManager.hpp"
#include "managers/GtsSizeManager.hpp"
#include "managers/animation/Grab.hpp"
#include "data/persistent.hpp"
#include "ActionSettings.hpp"
#include "data/transient.hpp"
#include "data/runtime.hpp"
#include "actorBools.hpp"

// Planning to switch to it later and remove these functions from ActorUtils.cpp, delayed because it's hard to see important changes

/*namespace Gts {
   bool BehaviorGraph_DisableHH(Actor* actor) { // should .dll disable HH if Behavior Graph has HH Disable data?
		bool disable = false;
		actor->GetGraphVariableBool("GTS_DisableHH", disable);
		if (actor->formID == 0x14 && IsFirstPerson()) {
			return false;
		}
		bool anims = AnimationsInstalled(actor);
		if (!anims) {
			return false; // prevent hh from being disabled if there's no Nemesis Generation
		}

		return disable;
	}

	bool IsEquipBusy(Actor* actor) {
		auto profiler = Profilers::Profile("ActorUtils: IsEquipBusy");
		int State;
		actor->GetGraphVariableInt("currentDefaultState", State);
		if (State >= 10 && State <= 20) {
			return true;
		}
		return false;
	}

	bool IsRagdolled(Actor* actor) {
		bool ragdoll = actor->IsInRagdollState();
		return ragdoll;
	}
	bool IsGrowing(Actor* actor) {
		bool Growing = false;
		actor->GetGraphVariableBool("GTS_IsGrowing", Growing);
		return Growing;
	}
	
	bool IsChangingSize(Actor* actor) { // Used to disallow growth/shrink during specific animations
		bool Growing = false;
		bool Shrinking = false;
		actor->GetGraphVariableBool("GTS_IsGrowing", Growing);
		actor->GetGraphVariableBool("GTS_IsShrinking", Shrinking);

		return Growing || Shrinking;
	}

	bool IsProning(Actor* actor) {
		bool prone = false;
		if (actor) {
			auto transient = Transient::GetSingleton().GetData(actor);
			actor->GetGraphVariableBool("GTS_IsProne", prone);
			if (actor->formID == 0x14 && actor->IsSneaking() && IsFirstPerson() && transient) {
				return transient->FPProning; // Because we have no FP behaviors, 
				// ^ it is Needed to fix proning being applied to FP even when Prone is off
			}
		}
		return prone;
	}

	bool IsCrawling(Actor* actor) {
		bool crawl = false;
		if (actor) {
			auto transient = Transient::GetSingleton().GetData(actor);
			actor->GetGraphVariableBool("GTS_IsCrawling", crawl);
			if (actor->formID == 0x14 && actor->IsSneaking() && IsFirstPerson() && transient) {
				return transient->FPCrawling; // Needed to fix crawling being applied to FP even when Prone is off
			}
			return actor->IsSneaking() && crawl;
		}
		return false;
	}

	bool IsInBalanceMode() {
		return SizeManager::GetSingleton().BalancedMode() > 1.0f;
	}

	bool IsHugCrushing(Actor* actor) {
		bool IsHugCrushing = false;
		actor->GetGraphVariableBool("IsHugCrushing", IsHugCrushing);
		return IsHugCrushing;
	}

	bool IsHugHealing(Actor* actor) {
		bool IsHugHealing = false;
		actor->GetGraphVariableBool("GTS_IsHugHealing", IsHugHealing);
		return IsHugHealing;
	}

	bool IsVoring(Actor* giant) {
		bool Voring = false;
		giant->GetGraphVariableBool("GTS_IsVoring", Voring);
		return Voring;
	}

	bool IsHuggingFriendly(Actor* actor) {
		bool friendly = false;
		actor->GetGraphVariableBool("GTS_IsFollower", friendly);
		return friendly;
	}

	bool IsTransitioning(Actor* actor) { // reports sneak transition to crawl
		bool transition = false;
		actor->GetGraphVariableBool("GTS_Transitioning", transition);
		return transition;
	}

	bool IsFootGrinding(Actor* actor) {
		bool grind = false;
		actor->GetGraphVariableBool("GTS_IsFootGrinding", grind);
		return grind;
	}

	bool IsJumping(Actor* actor) {
		bool jumping = false;
		actor->GetGraphVariableBool("bInJumpState", jumping);
		return jumping;
	}

	bool IsBeingHeld(Actor* giant, Actor* tiny) {
		auto grabbed = Grab::GetHeldActor(giant);
		
		if (grabbed) {
			if (grabbed == tiny) {
				return true;
			}
		}
		
		auto transient = Transient::GetSingleton().GetData(tiny);
		if (transient) {
			return transient->being_held && !tiny->IsDead();
		}
		return false;
	}

	bool IsBetweenBreasts(Actor* actor) {
		auto transient = Transient::GetSingleton().GetData(actor);
		if (transient) {
			return transient->is_between_breasts;
		}
		return false;
	}

	bool IsTransferingTiny(Actor* actor) { // Reports 'Do we have someone grabed?'
		int grabbed = 0;
		actor->GetGraphVariableInt("GTS_GrabbedTiny", grabbed);
		return grabbed > 0;
	}

	bool IsUsingThighAnimations(Actor* actor) { // Do we currently use Thigh Crush / Thigh Sandwich?
		int sitting = false;
		actor->GetGraphVariableInt("GTS_Sitting", sitting);
		return sitting > 0;
	}

	bool IsSynced(Actor* actor) {
		bool sync = false;
		actor->GetGraphVariableBool("bIsSynced", sync);
		return sync;
	}

	bool CanDoPaired(Actor* actor) {
		bool paired = false;
		actor->GetGraphVariableBool("GTS_CanDoPaired", paired);
		return paired;
	}


	bool IsThighCrushing(Actor* actor) { // Are we currently doing Thigh Crush?
		int crushing = 0;
		actor->GetGraphVariableInt("GTS_IsThighCrushing", crushing);
		return crushing > 0;
	}

	bool IsThighSandwiching(Actor* actor) { // Are we currently Thigh Sandwiching?
		int sandwiching = 0;
		actor->GetGraphVariableInt("GTS_IsThighSandwiching", sandwiching);
		return sandwiching > 0;
	}

	bool IsBeingEaten(Actor* tiny) {
		auto transient = Transient::GetSingleton().GetData(tiny);
		if (transient) {
			return transient->about_to_be_eaten;
		}
		return false;
	}

	bool IsGtsBusy(Actor* actor) {
		auto profiler = Profilers::Profile("ActorUtils: IsGtsBusy"); 
		bool GTSBusy = false;
		actor->GetGraphVariableBool("GTS_Busy", GTSBusy);

		bool Busy = GTSBusy && !CanDoCombo(actor);
		return Busy;
	}

	bool IsStomping(Actor* actor) {
		bool Stomping = false;
		actor->GetGraphVariableBool("GTS_IsStomping", Stomping);

		return Stomping;
	}

	bool IsInCleavageState(Actor* actor) { // For GTS 
		bool Cleavage = false;

		actor->GetGraphVariableBool("GTS_IsBoobing", Cleavage);

		return Cleavage;
	}

	bool IsCleavageZIgnored(Actor* actor) {
		bool ignored = false;

		actor->GetGraphVariableBool("GTS_OverrideZ", ignored);

		return ignored;
	}

	bool IsInsideCleavage(Actor* actor) { // For tinies
		bool InCleavage = false;

		actor->GetGraphVariableBool("GTS_IsinBoobs", InCleavage);

		return InCleavage;
	}
	
	bool IsKicking(Actor* actor) {
		bool Kicking = false;
		actor->GetGraphVariableBool("GTS_IsKicking", Kicking);

		return Kicking;
	}

	bool IsTrampling(Actor* actor) {
		bool Trampling = false;
		actor->GetGraphVariableBool("GTS_IsTrampling", Trampling);

		return Trampling;
	}

	bool CanDoCombo(Actor* actor) {
		bool Combo = false;
		actor->GetGraphVariableBool("GTS_CanCombo", Combo);
		return Combo;
	}

	bool IsCameraEnabled(Actor* actor) {
		bool Camera = false;
		actor->GetGraphVariableBool("GTS_VoreCamera", Camera);
		return Camera;
	}

	bool IsCrawlVoring(Actor* actor) {
		bool Voring = false;
		actor->GetGraphVariableBool("GTS_IsCrawlVoring", Voring);
		return Voring;//Voring;
	}

	bool IsButtCrushing(Actor* actor) {
		bool ButtCrushing = false;
		actor->GetGraphVariableBool("GTS_IsButtCrushing", ButtCrushing);
		return ButtCrushing;
	}

	bool ButtCrush_IsAbleToGrow(Actor* actor, float limit) {
		auto transient = Transient::GetSingleton().GetData(actor);
		float stamina = GetAV(actor, ActorValue::kStamina);
		if (stamina <= 4.0f) {
			return false;
		}
		if (transient) {
			return transient->ButtCrushGrowthAmount < limit;
		}
		return false;
	}

	bool IsBeingGrinded(Actor* actor) {
		auto transient = Transient::GetSingleton().GetData(actor);
		bool grinded = false;
		actor->GetGraphVariableBool("GTS_BeingGrinded", grinded);
		if (transient) {
			return transient->being_foot_grinded;
		}
		return grinded;
	}

	bool IsHugging(Actor* actor) {
		bool hugging = false;
		actor->GetGraphVariableBool("GTS_Hugging", hugging);
		return hugging;
	}

	bool IsBeingHugged(Actor* actor) {
		bool hugged = false;
		actor->GetGraphVariableBool("GTS_BeingHugged", hugged);
		return hugged;
	}

	bool CanDoButtCrush(Actor* actor, bool apply_cooldown) {
		bool Allow = IsActionOnCooldown(actor, CooldownSource::Action_ButtCrush);

		if (!Allow && apply_cooldown) { // send it to cooldown if it returns 'not busy'
			ApplyActionCooldown(actor, CooldownSource::Action_ButtCrush);
		}

		return !Allow; // return flipped OnCooldown. By default it false, we flip it so it returns True (Can perform butt crush)
	}

	bool GetCameraOverride(Actor* actor) {
		if (actor->formID == 0x14) {
			auto transient = Transient::GetSingleton().GetData(actor);
			if (transient) {
				return transient->OverrideCamera;
			}
			return false;
		}
		return false;
	}



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                                 G T S   ST A T E S  O T H E R                                                                      //
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	bool IsGrowthSpurtActive(Actor* actor) {
		if (!Runtime::HasPerkTeam(actor, "GrowthOfStrength")) {
			return false;
		}
		if (HasGrowthSpurt(actor)) {
			return true;
		}
		return false;
	}

	bool HasGrowthSpurt(Actor* actor) {
		bool Growth1 = Runtime::HasMagicEffect(actor, "GrowthSpurt1");
		bool Growth2 = Runtime::HasMagicEffect(actor, "GrowthSpurt2");
		bool Growth3 = Runtime::HasMagicEffect(actor, "GrowthSpurt3");
		if (Growth1 || Growth2 || Growth3) {
			return true;
		} else {
			return false;
		}
	}

	bool InBleedout(Actor* actor) {
		return actor->AsActorState()->IsBleedingOut();
	}

	bool AllowStagger(Actor* giant, Actor* tiny) {
		if (Persistent::GetSingleton().allow_stagger == true) {
			return true; // Allow it
		} else if (Persistent::GetSingleton().allow_stagger == false) {
			bool ProtectGTS = giant->formID == 0x14 || IsTeammate(giant);
			bool ProtectTiny = tiny->formID == 0x14 || IsTeammate(tiny);
			//log::info("GTS {}: {}", giant->GetDisplayFullName(), ProtectGTS);
			//log::info("Tiny {}: {}", tiny->GetDisplayFullName(), ProtectTiny);
			if (ProtectGTS && ProtectTiny) {
				return false; // Protect
			}
			return true;
		}
		return true;
	}

	bool IsMechanical(Actor* actor) {
		bool dwemer = Runtime::HasKeyword(actor, "DwemerKeyword");
		return dwemer;
	}

	bool IsHuman(Actor* actor) { // Check if Actor is humanoid or not. Currently used for Hugs Animation and for playing moans
		bool vampire = Runtime::HasKeyword(actor, "VampireKeyword");
		bool dragon = Runtime::HasKeyword(actor, "DragonKeyword");
		bool animal = Runtime::HasKeyword(actor, "AnimalKeyword");
		bool dwemer = Runtime::HasKeyword(actor, "DwemerKeyword");
		bool undead = Runtime::HasKeyword(actor, "UndeadKeyword");
		bool creature = Runtime::HasKeyword(actor, "CreatureKeyword");
		if (!dragon && !animal && !dwemer && !undead && !creature) {
			return true; // Detect non-vampire
		} if (!dragon && !animal && !dwemer && !creature && undead && vampire) {
			return true; // Detect Vampire
		} else {
			return false;
		}
		return false;
	}

	bool IsBlacklisted(Actor* actor) {
		bool blacklist = Runtime::HasKeyword(actor, "BlackListKeyword");
		return blacklist;
	}

	bool IsGtsTeammate(Actor* actor) {
		return Runtime::HasKeyword(actor, "CountAsFollower");
	}
}*/