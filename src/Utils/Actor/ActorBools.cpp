#include "Utils/Actor/ActorBools.hpp"
#include "Config/Config.hpp"

namespace GTS {

	bool IsInSexlabAnim(Actor* actor_1, Actor* actor_2) {
		if (Runtime::IsSexlabInstalled()) {
			const auto& SLAnim = Runtime::FACT.SexLabAnimatingFaction;
			if (Runtime::InFaction(actor_1, SLAnim) && Runtime::InFaction(actor_2, SLAnim)) {
				return true;
			}
		}
		return false;
	}

	bool IsStaggered(Actor* tiny) {
		bool staggered = false;
		if (tiny) {
			staggered = static_cast<bool>(tiny->AsActorState()->actorState2.staggered);
		}
		return staggered;
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

		if (actor->IsPlayerRef()) {
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
		if (giant->IsPlayerRef()) {
			return HasHeadTrackingTarget(giant);
		}
		return false;
	}

	bool IsInGodMode(Actor* giant) {
		if (!giant->IsPlayerRef()) {
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
		if (NPCImmunity && giant->IsPlayerRef() && (IsTeammate(tiny)) && !hostile) {
			return false; // Protect NPC's against player size-related effects
		}
		if (NPCImmunity && (IsTeammate(giant)) && (IsTeammate(tiny))) {
			return false; // Disallow NPC's to damage each-other if they're following Player
		}
		if (PlayerImmunity && tiny->IsPlayerRef() && (IsTeammate(giant)) && !hostile) {
			return false; // Protect Player against friendly NPC's damage
		}
		return true;
	}

	bool IsTeammate(Actor* actor) {

		if (!actor) {
			return false;
		}

		//A player can't be their own teammate
		if (actor->IsPlayerRef()) {
			return false;
		}

		if ((Runtime::InFaction(actor, Runtime::FACT.FollowerFaction) ||
			actor->IsPlayerTeammate() ||
			IsGTSTeammate(actor)) &&
			IsHumanoid(actor)) { //Disallow Creature NPC's
			return true;
		}

		return false;
	}

	bool IsEquipBusy(Actor* actor) {
		int State = AnimationVars::Other::CurrentDefaultState(actor);
		if (State >= 10 && State <= 20) {
			return true;
		}
		return false;
	}

	bool IsRagdolled(Actor* actor) {
		bool ragdoll = actor->IsInRagdollState();
		return ragdoll;
	}

	bool InBleedout(Actor* actor) {
		return actor->AsActorState()->IsBleedingOut();
	}

	bool IsMechanical(Actor* actor) {
		bool dwemer = Runtime::HasKeyword(actor, Runtime::KYWD.DwemerKeyword);
		return dwemer;
	}

	bool IsHuman(Actor* actor) { // Check if Actor is humanoid or not. Currently used for Hugs Animation and for playing moans

		const bool humanoid = Runtime::HasKeyword(actor, Runtime::KYWD.ActorTypeNPC);
		if (humanoid) {
			return true;
		}

		const bool vampire = Runtime::HasKeyword(actor, Runtime::KYWD.VampireKeyword);
		const bool dragon = Runtime::HasKeyword(actor, Runtime::KYWD.DragonKeyword);
		const bool animal = Runtime::HasKeyword(actor, Runtime::KYWD.AnimalKeyword);
		const bool dwemer = Runtime::HasKeyword(actor, Runtime::KYWD.DwemerKeyword);
		const bool undead = Runtime::HasKeyword(actor, Runtime::KYWD.UndeadKeyword);
		const bool creature = Runtime::HasKeyword(actor, Runtime::KYWD.CreatureKeyword) || Runtime::IsRace(actor, Runtime::RACE.DLC1VampireBeastRace);

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

	bool IsGTSTeammate(Actor* actor) {
		return Runtime::HasKeyword(actor, Runtime::KYWD.GTSKeywordCountAsFollower);
	}

	bool HasSMT(Actor* giant) {
		if (Runtime::HasMagicEffect(giant, Runtime::MGEF.GTSEffectTinyCalamity)) {
			return true;
		}
		return false;
	}

}