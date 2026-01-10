#include "Utils/Actor/ActorBools.hpp"

#include "Config/Config.hpp"

#include "Managers/Animation/Grab.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"

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

	bool IsInSexlabAnim(Actor* actor_1, Actor* actor_2) {
		if (Runtime::IsSexlabInstalled()) {
			const auto& SLAnim = Runtime::FACT.SexLabAnimatingFaction;
			if (Runtime::InFaction(actor_1, SLAnim) && Runtime::InFaction(actor_2, SLAnim)) {
				return true;
			}
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
		if (auto process = giant->GetActorRuntimeData().currentProcess) {
			if (process->high) {
				if (process->GetHeadtrackTarget()) {
					return true;
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

	bool IsInsect(Actor* actor, bool performcheck) {
		bool Check = Config::Gameplay.ActionSettings.bAllowInsects;

		if (performcheck && Check) {
			return false;
		}

		bool Spider = Runtime::IsRace(actor, Runtime::RACE.FrostbiteSpiderRace);
		bool SpiderGiant = Runtime::IsRace(actor, Runtime::RACE.FrostbiteSpiderRaceGiant);
		bool SpiderLarge = Runtime::IsRace(actor, Runtime::RACE.FrostbiteSpiderRaceLarge);
		bool ChaurusReaper = Runtime::IsRace(actor, Runtime::RACE.ChaurusReaperRace);
		bool Chaurus = Runtime::IsRace(actor, Runtime::RACE.ChaurusRace);
		bool ChaurusHunterDLC = Runtime::IsRace(actor, Runtime::RACE.DLC1ChaurusHunterRace);
		bool ChaurusDLC = Runtime::IsRace(actor, Runtime::RACE.DLC1_BF_ChaurusRace);
		bool ExplSpider = Runtime::IsRace(actor, Runtime::RACE.DLC2ExpSpiderBaseRace);
		bool ExplSpiderPackMule = Runtime::IsRace(actor, Runtime::RACE.DLC2ExpSpiderPackmuleRace);
		bool AshHopper = Runtime::IsRace(actor, Runtime::RACE.DLC2AshHopperRace);

		if (Spider || SpiderGiant || SpiderLarge || ChaurusReaper || Chaurus || ChaurusHunterDLC || ChaurusDLC || ExplSpider || ExplSpiderPackMule || AshHopper) {
			return true;
		}
		return false;

	}

	bool IsFemale(Actor* a_Actor, bool AllowOverride) {
		if (AllowOverride) {
			GTS_PROFILE_SCOPE("ActorUtils: IsFemale");

			if (Config::General.bEnableMales) {
				return true;
			}
		}

		auto base = a_Actor->GetActorBase();
		int sex = 0;
		if (base) {
			if (base->GetSex()) {
				sex = base->GetSex();
				//log::info("{} Is Female: {}", actor->GetDisplayFullName(), static_cast<bool>(sex));
			}
		}
		return sex; // Else return false
	}

	bool IsDragon(Actor* actor) {
		if (Runtime::HasKeyword(actor, Runtime::KYWD.DragonKeyword)) {
			return true;
		}
		if (Runtime::IsRace(actor, Runtime::RACE.DragonRace)) {
			return true;
		}
		return false;
	}

	bool IsGiant(Actor* actor) {
		return Runtime::IsRace(actor, Runtime::RACE.GiantRace);
	}

	bool IsMammoth(Actor* actor) {
		return Runtime::IsRace(actor, Runtime::RACE.MammothRace);
	}

	bool IsLiving(Actor* actor) {
		bool IsDraugr = Runtime::HasKeyword(actor, Runtime::KYWD.UndeadKeyword);
		bool IsDwemer = Runtime::HasKeyword(actor, Runtime::KYWD.DwemerKeyword);
		bool IsVampire = Runtime::HasKeyword(actor, Runtime::KYWD.VampireKeyword);
		if (IsVampire) {
			return true;
		}
		if (IsDraugr || IsDwemer) {
			return false;
		}
		else {
			return true;
		}
		return true;
	}

	bool IsUndead(Actor* actor, bool PerformCheck) {
		bool IsDraugr = Runtime::HasKeyword(actor, Runtime::KYWD.UndeadKeyword);
		bool Check = Config::Gameplay.ActionSettings.bAllowUndead;
		if (Check && PerformCheck) {
			return false;
		}
		return IsDraugr;
	}

	bool WasReanimated(Actor* actor) { // must be called while actor is still alive, else it will return false.
		bool reanimated = false;
		auto transient = Transient::GetActorData(actor);
		if (transient) {
			reanimated = transient->WasReanimated;
		}
		return reanimated;
	}

	bool IsFlying(Actor* actor) {
		bool flying = false;
		if (actor) {
			flying = actor->AsActorState()->IsFlying();
		}
		return flying;
	}

	bool IsHostile(Actor* giant, Actor* tiny) {
		return tiny->IsHostileToActor(giant);
	}



	bool IsEssential(Actor* giant, Actor* actor) {

		auto& Settings = Config::General;

		const bool ProtectEssential = actor->IsEssential() && Settings.bProtectEssentials;
		const bool ProtectFollowers = Settings.bProtectFollowers;
		const bool Teammate = IsTeammate(actor);

		if (actor->formID == 0x14) {
			return false; // we don't want to make the player immune
		}

		//If Not a follower and protection is on.
		if (!Teammate && ProtectEssential) {
			return true;
		}

		//If Follower and protected
		if (Teammate && ProtectFollowers) {

			//If Actors are hostile to each other
			if (IsHostile(giant, actor) || IsHostile(actor, giant)) {
				return false;
			}

			return true;
		}
		return false;
	}

	bool IsHeadtracking(Actor* giant) { // Used to report True when we lock onto something, should be Player Exclusive.
		//Currently used to fix TDM mesh issues when we lock on someone.
		/*bool tracking = false;
		if (giant->formID == 0x14) {
			giant->GetGraphVariableBool("TDM_TargetLock", tracking); // get HT value, requires newest versions of TDM to work properly
		}
		*/ // Old TDM Method, a bad idea since some still run old version for some reason.
		if (giant->formID == 0x14) {
			return HasHeadTrackingTarget(giant);
		}
		return false;
	}

	bool IsInGodMode(Actor* giant) {
		if (giant->formID != 0x14) {
			return false;
		}
		REL::Relocation<bool*> singleton{ RELOCATION_ID(517711, 404238) };
		return *singleton;
	}

	bool CanDoDamage(Actor* giant, Actor* tiny, bool HoldCheck) {
		if (HoldCheck) {
			if (IsBeingHeld(giant, tiny)) {
				return false;
			}
		}

		bool hostile = (IsHostile(giant, tiny) || IsHostile(tiny, giant));

		const auto& Settings = Config::Balance;

		bool NPCImmunity = Settings.bFollowerFriendlyImmunity;
		bool PlayerImmunity = Settings.bPlayerFriendlyImmunity;

		if (hostile) {
			return true;
		}
		if (NPCImmunity && giant->formID == 0x14 && (IsTeammate(tiny)) && !hostile) {
			return false; // Protect NPC's against player size-related effects
		}
		if (NPCImmunity && (IsTeammate(giant)) && (IsTeammate(tiny))) {
			return false; // Disallow NPC's to damage each-other if they're following Player
		}
		if (PlayerImmunity && tiny->formID == 0x14 && (IsTeammate(giant)) && !hostile) {
			return false; // Protect Player against friendly NPC's damage
		}
		return true;
	}


	bool AllowDevourment() {
		return Config::General.bDevourmentCompat && Runtime::IsDevourmentInstalled();
	}

	bool AllowCameraTracking() {
		return Config::General.bTrackBonesDuringAnim;
	}

	bool LessGore() {
		return Config::General.bLessGore;
	}

	bool IsTeammate(Actor* actor) {

		if (!actor) {
			return false;
		}

		//A player can't be their own teammate
		if (actor->formID == 0x14) {
			return false;
		}

		if ((Runtime::InFaction(actor, Runtime::FACT.FollowerFaction) ||
			actor->IsPlayerTeammate() ||
			IsGtsTeammate(actor)) &&
			IsHumanoid(actor)) { //Disallow Creature NPC's
			return true;
		}

		return false;
	}

	bool IsEquipBusy(Actor* actor) {
		GTS_PROFILE_SCOPE("ActorUtils: IsEquipBusy");
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
			auto transient = Transient::GetActorData(actor);
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
			auto transient = Transient::GetActorData(actor);
			actor->GetGraphVariableBool("GTS_IsCrawling", crawl);
			if (actor->formID == 0x14 && actor->IsSneaking() && IsFirstPerson() && transient) {
				return transient->FPCrawling; // Needed to fix crawling being applied to FP even when Prone is off
			}
			return actor->IsSneaking() && crawl;
		}
		return false;
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

		auto transient = Transient::GetActorData(tiny);
		if (transient) {
			return transient->BeingHeld && !tiny->IsDead();
		}
		return false;
	}

	bool IsBetweenBreasts(Actor* actor) {
		auto transient = Transient::GetActorData(actor);
		if (transient) {
			return transient->BetweenBreasts;
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
		auto transient = Transient::GetActorData(tiny);
		if (transient) {
			return transient->AboutToBeEaten;
		}
		return false;
	}

	bool IsGtsBusy(Actor* actor) {
		GTS_PROFILE_SCOPE("ActorUtils: IsGtsBusy");
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
		if (!IsHuman(actor)) return true; // Bypass incase someone uses creatures...
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
		auto transient = Transient::GetActorData(actor);
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
		auto transient = Transient::GetActorData(actor);
		bool grinded = false;
		actor->GetGraphVariableBool("GTS_BeingGrinded", grinded);
		if (transient) {
			return transient->BeingFootGrinded;
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

	bool InBleedout(Actor* actor) {
		return actor->AsActorState()->IsBleedingOut();
	}

	bool IsMechanical(Actor* actor) {
		bool dwemer = Runtime::HasKeyword(actor, Runtime::KYWD.DwemerKeyword);
		return dwemer;
	}

	bool IsHuman(Actor* actor) { // Check if Actor is humanoid or not. Currently used for Hugs Animation and for playing moans
		bool vampire = Runtime::HasKeyword(actor, Runtime::KYWD.VampireKeyword);
		bool dragon = Runtime::HasKeyword(actor, Runtime::KYWD.DragonKeyword);
		bool animal = Runtime::HasKeyword(actor, Runtime::KYWD.AnimalKeyword);
		bool dwemer = Runtime::HasKeyword(actor, Runtime::KYWD.DwemerKeyword);
		bool undead = Runtime::HasKeyword(actor, Runtime::KYWD.UndeadKeyword);
		bool creature = Runtime::HasKeyword(actor, Runtime::KYWD.CreatureKeyword);
		bool humanoid = Runtime::HasKeyword(actor, Runtime::KYWD.ActorTypeNPC);
		if (humanoid) {
			return true;
		}
		if (!dragon && !animal && !dwemer && !undead && !creature) {
			return true; // Detect non-vampire
		}
		if (!dragon && !animal && !dwemer && !creature && undead && vampire) {
			return true; // Detect Vampire
		}
		return false;
	}

	bool IsBlacklisted(Actor* actor) {
		bool blacklist = Runtime::HasKeyword(actor, Runtime::KYWD.GTSKeywordBlackListActor);
		return blacklist;
	}

	bool IsGtsTeammate(Actor* actor) {
		return Runtime::HasKeyword(actor, Runtime::KYWD.GTSKeywordCountAsFollower);
	}

	bool HasSMT(Actor* giant) {
		if (Runtime::HasMagicEffect(giant, Runtime::MGEF.GTSEffectTinyCalamity)) {
			return true;
		}
		return false;
	}

}