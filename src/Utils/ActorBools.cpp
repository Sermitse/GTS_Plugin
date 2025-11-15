#include "Utils/ActorBools.hpp"

// I'm planning to switch to it later and remove these functions from ActorUtils.cpp
// delayed because it's hard to see important changes since there's like 100+ file changes when switching to it

namespace GTS {

	bool IsStaggered(Actor* tiny) {
		bool staggered = false;
		if (tiny) {
			staggered = static_cast<bool>(tiny->AsActorState()->actorState2.staggered);
		}
		return staggered;
	}

	bool IsHandStomping_L(Actor* actor) { // Returns true when doing Hand Slam attacks during sneak and crawl states
		bool stomping = false;
		actor->GetGraphVariableBool("GTS_IsHandStomping", stomping); // When we do either the right or left LIGHT hand slam(Sneak/crawl Stomp)
		return stomping;
	}

	bool IsHandStomping_H(Actor* actor) { // Returns true when doing Hand Slam attacks during sneak and crawl states
		bool stomping = false;
		actor->GetGraphVariableBool("GTS_IsHandStomping_Strong", stomping); // Enabled when we do either the right or left HEAVY hand slam(Sneak/crawl Stomp)
		return stomping;
	}
	
	bool IsBeingKilledWithMagic(Actor* tiny) { // Returns true when performing Wrathful Calamity animation on someone
		bool shrinking = false;
		if (tiny) {
			tiny->GetGraphVariableBool("GTS_Being_Shrunk", shrinking);
		}
		return shrinking;
	}

	bool IsGrabAttacking(Actor* actor) {
		bool Attacking = false;
		actor->GetGraphVariableBool("GTS_IsGrabAttacking", Attacking);

		return Attacking;
	}

	bool IsStrangling(Actor* giant) { // returns true When performing DOT on someone
		bool strangling = false;
		giant->GetGraphVariableBool("GTS_Isboobs_doting", strangling);
		return strangling;
	}

	bool IsExitingStrangle(Actor* giant) { // returns true when GTS leaves DOT state through another anim
		bool leaving = false;
		giant->GetGraphVariableBool("GTS_Isboobs_leaving", leaving);
		return leaving;
	}

	bool IsUnderGrinding(Actor* giant) {
		bool undergrinding = false;
		giant->GetGraphVariableBool("GTS_IsUnderGrinding", undergrinding);
		return undergrinding;
	}

	bool IsUnderTrampling(Actor* giant) {
		bool undertrampling = false;
		giant->GetGraphVariableBool("GTS_IsUnderTrampling", undertrampling);
		return undertrampling;
	}

	bool IsInGrabPlayState(Actor* giant) { // Returns true when we enter Play State from Grab (Grab state where we play with tiny)
		bool InPlayState = false;
		giant->GetGraphVariableBool("GTS_IsInGrabPlayState", InPlayState);
		return InPlayState;
	}

	bool IsInSexlabAnim(Actor* actor_1, Actor* actor_2) {
		if (Runtime::IsSexlabInstalled()) {
			const auto& SLAnim = Runtime::FACT.SexLabAnimatingFaction;
			if (Runtime::InFaction(actor_1, SLAnim) && Runtime::InFaction(actor_2, SLAnim)) {
				return true;
			}
		}
		return false;
	}

	bool DontAlterDamage(Actor* a_this, float dmg, float Damage_Add) { // Used inside Damage.cpp (hook), a way to fix almost unkillable player in some cases
		if (a_this->formID == 0x14) {
			float currentHP = GetAV(a_this, ActorValue::kHealth);
			bool ShouldBeKilled = GetHealthPercentage(a_this) <= 0.05f && dmg + Damage_Add >= currentHP;
			return ShouldBeKilled;
		}
		return false;
	}

	bool IsHumanoid(Actor* giant) {
		const bool Human = Runtime::HasKeyword(giant, Runtime::KYWD.ActorTypeNPC);
		return Human;
	}

	bool CountAsGiantess(Actor* giant) {
		const bool Giantess = Runtime::HasKeyword(giant, Runtime::KYWD.EnforceGiantessKeyword);
		return Giantess;
	}

	bool IsVisible(Actor* giant) {
		if (giant) {
			return giant->GetAlpha() > 0.1f;
		}
		return false;
	}

	bool HasHeadTrackingTarget(Actor* giant) {
		auto process = giant->GetActorRuntimeData().currentProcess;
		if (process) {
			auto high = process->high;
			if (high) {
				if (process->GetHeadtrackTarget()) {
					return true;
				} else {
					return false;
				}
			}
		}
		return false;
	}

	bool KnockedDown(Actor* giant) {
		return static_cast<int>(giant->AsActorState()->GetKnockState()) != 0; // Another way of checking ragdoll just in case
	}

	bool IsinRagdollState(Actor* giant) {
		bool ragdolled = IsRagdolled(giant) || KnockedDown(giant);
		return ragdolled;
	}

}