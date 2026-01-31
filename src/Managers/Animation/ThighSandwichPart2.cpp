#include "Managers/Animation/ThighSandwichPart2.hpp"

#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/Controllers/ThighSandwichController.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Audio/GoreAudio.hpp"
#include "Managers/Input/InputManager.hpp"
#include "Managers/CrushManager.hpp"
#include "Managers/GtsSizeManager.hpp"
#include "Managers/Rumble.hpp"
#include "Magic/Effects/Common.hpp"
#include "Utils/Actions/ButtCrushUtils.hpp"
#include "Utils/Actions/InputConditions.hpp"

using namespace GTS;

namespace AnimEvents {

	//Used in the Intro to Butt State when the GTS sits back down softly on the Tiny
	void GTS_TSB_SitDownSoft(AnimationEventData& data) {}

	//Used in the exit of the butt state when the gts's sits down onto the rune with no tiny underneath
	void GTS_TSB_SitDown(AnimationEventData& data) {}

	//Used in the exit when the Tiny falls back onto the GTS's thigh
	void GTS_TSB_TinyThigh(AnimationEventData& data) {}

	//Used when the GTS starts to lift her self up from sitting down mostly for sfx
	void GTS_TSB_Stand(AnimationEventData& data) {}

	//Used when the GTS starts to fall down to butt crush
	void GTS_TSB_Fall(AnimationEventData& data) {}

	//Used when the GTS lands the first two hits during the Butt state finisher, doesn't do damage
	void GTS_TSB_LandMid(AnimationEventData& data) {}

	//Used when the GTS lands the light butt attack
	void GTS_TSB_LandSmall(AnimationEventData& data) {
		//Do light damage
	}

	//Used when the GTS lands the heavy butt attack
	void GTS_TSB_LandHeavy(AnimationEventData& data) {
		//Do heavy damage

	}

	//Used for the final hit when the GTS kills the tiny and cracks the butt rune
	void GTS_TSB_LandFinisher(AnimationEventData& data) {
		//Deal Lethal damage

	}

	//Used when the GTS lands on the Floor after cracking the butt rune
	void GTS_TSB_LandFloor(AnimationEventData& data) {
		Actor* caster = &data.giant;
		bool HasEffect = Runtime::HasMagicEffect(caster, Runtime::MGEF.GTSEffectThighRune);
		SpellItem* Spell = Runtime::GetSpell(Runtime::SPEL.GTSSpellThighRune);

		if (HasEffect && Spell) {
			ActorHandle handle = caster->CreateRefHandle();
			caster->AsMagicTarget()->DispelEffect(Spell, handle);
		}
	}

	//Used when the GTS dust off her butt after standing up from the Finisher mostly for SFX
	void GTS_TSB_DustButt(AnimationEventData& data) {}

	//Triggered when the Tiny is initially inserted
	void GTS_TSB_TinyInserted(AnimationEventData& data) {}

	//Triggered when the GTS Pushes the Tiny inside
	void GTS_TSB_TinyPushStart(AnimationEventData& data) {}

	//Triggered when the tiny is fully inside
	void GTS_TSB_TinyFullyIn(AnimationEventData& data) {}

	//Triggered at the end when the tiny is fully inside Kills the Tiny
	void GTS_TSB_TinyKill(AnimationEventData& data) {}

	//Triggered When the Tiny is being pushed in (a frame later than Pushstart) this was here for the devourment mod but I guess I just made the event an official one *shruging asci art*
	void GTS_TSB_UBStart(AnimationEventData& data) {}

	//riggered when the Tiny is fully in and the GTS's head is back and she's praising Diabella's blessings(O facing)
	void GTS_TSB_UBEnjoy(AnimationEventData& data) {}

	//Triggered when the ub is over nad the GTS has stopped enjoying her self and is returning to Idle
	void GTS_TSB_UBEnd(AnimationEventData& data) {}

	//Triggered after TinyKill, an event for the Devourment mod for when the tiny can be deleted
	void GTS_TSB_UBAbsorb(AnimationEventData& data) {}


}

namespace {

	void PerformAnimations(std::string_view owner_anim, std::string_view receiver_anim = "") {
		auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(PlayerCharacter::GetSingleton());
		AnimationManager::StartAnim(owner_anim, PlayerCharacter::GetSingleton());

		for (auto tiny : sandwichdata.GetActors()) {
			if (tiny && receiver_anim.size() > 1) {
				AnimationManager::StartAnim(receiver_anim, tiny);
			}
		}
	}

	void ButtStart(const ManagedInputEvent& data) {
		auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(PlayerCharacter::GetSingleton());
		if (sandwichdata.GetActors().size() > 0) {
			PerformAnimations("Sandwich_ButtStart", "Sandwich_ButtStart_T");
		}
	}

	void ButtStop(const ManagedInputEvent& data) {
		PerformAnimations("Sandwich_ButtStop", "Sandwich_ButtStop_T");
	}

	void ButtLightAttack(const ManagedInputEvent& data) {
		PerformAnimations("Sandwich_LightAttack", "Sandwich_LightAttack_T");
	}

	void ButtHeavyAttack(const ManagedInputEvent& data) {
		PerformAnimations("Sandwich_HeavyAttack", "Sandwich_HeavyAttack_T");
	}

	void ButtFinisher(const ManagedInputEvent& data) {
		PerformAnimations("Sandwich_Finisher", "Sandwich_Finisher_T");
	}

	void ButtGrowth(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
		float GrowthCount = GetGrowthLimit(player);
		bool CanGrow = ButtCrush_IsAbleToGrow(player, GrowthCount);
		if (CanGrow) {
			PerformAnimations("Sandwich_Grow", "Sandwich_Grow_T");
		}
		else {
			PerformAnimations("Sandwich_HeavyAttack", "Sandwich_HeavyAttack_T");
			NotifyWithSound(player, "Your body can't grow any further");
		}

	}

	void ButtGrindStart(const ManagedInputEvent& data) {
		PerformAnimations("Sandwich_GrindStart", "Sandwich_GrindStart_T");
	}

	void ButtGrindStop(const ManagedInputEvent& data) {
		PerformAnimations("Sandwich_GrindStop", "Sandwich_GrindStop_T");
	}

	void ButtGrindAbort(const ManagedInputEvent& data) {
		PerformAnimations("Sandwich_GrindAbort", "Sandwich_GrindAbort_T");
	}

	void ButtUB(const ManagedInputEvent& data) {
		PerformAnimations("Sandwich_UB", "Sandwich_UB_T");
	}

}

namespace GTS
{
	void AnimationThighSandwich_P2::RegisterEvents() {
		
		//Input Events
		InputManager::RegisterInputEvent("SandwichButtStart", ButtStart, SecondThighSandwichBranch_Start);
		InputManager::RegisterInputEvent("SandwichButtStop", ButtStop, SecondThighSandwichBranch);
		InputManager::RegisterInputEvent("SandwichLightAttack", ButtLightAttack, SecondThighSandwichBranch);
		InputManager::RegisterInputEvent("SandwichHeavyAttack", ButtHeavyAttack, SecondThighSandwichBranch);
		InputManager::RegisterInputEvent("SandwichGrindStart", ButtGrindStart, ThighSandwichGrind_Start);
		InputManager::RegisterInputEvent("SandwichGrindStop", ButtGrindStop, ThighSandwichGrind);
		InputManager::RegisterInputEvent("SandwichGrindAbort", ButtGrindAbort, ThighSandwichGrind);
		InputManager::RegisterInputEvent("SandwichUB", ButtUB, UBCondition);
		InputManager::RegisterInputEvent("SandwichFinisher", ButtFinisher, SecondThighSandwichBranch);
		InputManager::RegisterInputEvent("SandwichGrowth", ButtGrowth, SecondThighSandwichBranch);

		//Animation Events
		AnimationManager::RegisterEvent("GTS_TSB_SitDownSoft", "ThighSandwich", AnimEvents::GTS_TSB_SitDownSoft);
		AnimationManager::RegisterEvent("GTS_TSB_SitDown", "ThighSandwich", AnimEvents::GTS_TSB_SitDown);
		AnimationManager::RegisterEvent("GTS_TSB_TinyThigh", "ThighSandwich", AnimEvents::GTS_TSB_TinyThigh);
		AnimationManager::RegisterEvent("GTS_TSB_Stand", "ThighSandwich", AnimEvents::GTS_TSB_Stand);
		AnimationManager::RegisterEvent("GTS_TSB_Fall", "ThighSandwich", AnimEvents::GTS_TSB_Fall);
		AnimationManager::RegisterEvent("GTS_TSB_LandSmall", "ThighSandwich", AnimEvents::GTS_TSB_LandSmall);
		AnimationManager::RegisterEvent("GTS_TSB_LandHeavy", "ThighSandwich", AnimEvents::GTS_TSB_LandHeavy);
		AnimationManager::RegisterEvent("GTS_TSB_LandFinisher", "ThighSandwich", AnimEvents::GTS_TSB_LandFinisher);
		AnimationManager::RegisterEvent("GTS_TSB_LandFloor", "ThighSandwich", AnimEvents::GTS_TSB_LandFloor);
		AnimationManager::RegisterEvent("GTS_TSB_DustButt", "ThighSandwich", AnimEvents::GTS_TSB_DustButt);
		AnimationManager::RegisterEvent("GTS_TSB_TinyInserted", "ThighSandwich", AnimEvents::GTS_TSB_TinyInserted);
		AnimationManager::RegisterEvent("GTS_TSB_TinyPushStart", "ThighSandwich", AnimEvents::GTS_TSB_TinyPushStart);
		AnimationManager::RegisterEvent("GTS_TSB_TinyFullyIn", "ThighSandwich", AnimEvents::GTS_TSB_TinyFullyIn);
		AnimationManager::RegisterEvent("GTS_TSB_TinyKill", "ThighSandwich", AnimEvents::GTS_TSB_TinyKill);
		AnimationManager::RegisterEvent("GTS_TSB_UBStart", "ThighSandwich", AnimEvents::GTS_TSB_UBStart);
		AnimationManager::RegisterEvent("GTS_TSB_UBEnjoy", "ThighSandwich", AnimEvents::GTS_TSB_UBEnjoy);
		AnimationManager::RegisterEvent("GTS_TSB_UBEnd", "ThighSandwich", AnimEvents::GTS_TSB_UBEnd);
		AnimationManager::RegisterEvent("GTS_TSB_UBAbsorb", "ThighSandwich", AnimEvents::GTS_TSB_UBAbsorb);
	}

	void AnimationThighSandwich_P2::RegisterTriggers() {
		AnimationManager::RegisterTrigger("Sandwich_ButtStart", "ThighSandwich", "GTSBEH_ButtState_Start");
		AnimationManager::RegisterTrigger("Sandwich_ButtStop", "ThighSandwich", "GTSBEH_ButtState_Stop");
		AnimationManager::RegisterTrigger("Sandwich_LightAttack", "ThighSandwich", "GTSBEH_ButtState_LightAttack");
		AnimationManager::RegisterTrigger("Sandwich_HeavyAttack", "ThighSandwich", "GTSBEH_ButtState_HeavyAttack");
		AnimationManager::RegisterTrigger("Sandwich_Finisher", "ThighSandwich", "GTSBEH_ButtState_Finisher");
		AnimationManager::RegisterTrigger("Sandwich_Grow", "ThighSandwich", "GTSBEH_ButtState_Grow");
		AnimationManager::RegisterTrigger("Sandwich_GrindStart", "ThighSandwich", "GTSBEH_ButtState_GrindStart");
		AnimationManager::RegisterTrigger("Sandwich_GrindStop", "ThighSandwich", "GTSBEH_ButtState_GrindStop");
		AnimationManager::RegisterTrigger("Sandwich_GrindAbort", "ThighSandwich", "GTSBEH_ButtState_Abort");
		AnimationManager::RegisterTrigger("Sandwich_UB", "ThighSandwich", "GTSBEH_Sandwich_UB");
		AnimationManager::RegisterTrigger("Sandwich_ButtStart_T", "ThighSandwich", "GTSBEH_T_ButtState_Start");
		AnimationManager::RegisterTrigger("Sandwich_ButtStop_T", "ThighSandwich", "GTSBEH_T_ButtState_Stop");
		AnimationManager::RegisterTrigger("Sandwich_LightAttack_T", "ThighSandwich", "GTSBEH_T_ButtState_LightAttack");
		AnimationManager::RegisterTrigger("Sandwich_HeavyAttack_T", "ThighSandwich", "GTSBEH_T_ButtState_HeavyAttack");
		AnimationManager::RegisterTrigger("Sandwich_Finisher_T", "ThighSandwich", "GTSBEH_T_ButtState_Finisher");
		AnimationManager::RegisterTrigger("Sandwich_Grow_T", "ThighSandwich", "GTSBEH_T_ButtState_Grow");
		AnimationManager::RegisterTrigger("Sandwich_GrindStart_T", "ThighSandwich", "GTSBEH_T_ButtState_GrindStart");
		AnimationManager::RegisterTrigger("Sandwich_GrindStop_T", "ThighSandwich", "GTSBEH_T_ButtState_GrindStop");
		AnimationManager::RegisterTrigger("Sandwich_GrindAbort_T", "ThighSandwich", "GTSBEH_T_ButtState_Abort");
		AnimationManager::RegisterTrigger("Sandwich_UB_T", "ThighSandwich", "GTSBEH_T_Sandwich_UB");
	}
}