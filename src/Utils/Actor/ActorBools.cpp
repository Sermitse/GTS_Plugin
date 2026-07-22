#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Utils/Actions/VoreUtils.hpp"
#include "Utils/Actor/ActorBools.hpp"
#include "Config/Config.hpp"

namespace GTS {

	bool IsInSexlabAnim(Actor* actor_1, Actor* actor_2) {
		if (actor_1 && actor_2){
			if (Runtime::IsSexlabInstalled()) {
				const auto& SLAnim = Runtime::FACT.SexLabAnimatingFaction;
				if (Runtime::InFaction(actor_1, SLAnim) && Runtime::InFaction(actor_2, SLAnim)) {
					return true;
				}
			}
		}
		return false;
	}

	bool IsStaggered(Actor* tiny) {

		if (tiny) {
			if (ActorState* acState = tiny->AsActorState()) {
				return static_cast<bool>(acState->actorState2.staggered);
			}
		}
		return false;
	}


	bool IsHumanoid(Actor* giant) {
		return giant ? Runtime::HasKeyword(giant, Runtime::KYWD.ActorTypeNPC) : false;
	}

	bool CountAsGiantess(Actor* giant) {
		return giant ? Runtime::HasKeyword(giant, Runtime::KYWD.EnforceGiantessKeyword) : false;
	}

	bool IsVisible(Actor* giant) {
		if (giant) {
			return giant->GetAlpha() > 0.1f;
		}
		return false;
	}


	bool IsInvisible_Devourment(Actor* giant) {

        if(!IsDevourmentEnabled()){
            return false; //Always Fail, DV not installed
        }
        if(!giant){
            return true; //Assume Invisible
        }
        if (!giant->Is3DLoaded()) {
            return true; //No 3D, so Invisible
        }
        return !IsVisible(giant); //Return inverse of visible, so if is Invisible.
    }

	bool HasHeadTrackingTarget(Actor* giant) {
		 
		if (!giant) return false;

		if (AIProcess* process = giant->GetActorRuntimeData().currentProcess) {
			if (process->high) {
				if (process->GetHeadtrackTarget()) {
					return true;
				} 
			}
		}
		return false;
	}

	bool KnockedDown(Actor* giant) {
		if (giant) {
			if (ActorState* acState = giant->AsActorState()) {
				return static_cast<int>(acState->GetKnockState()) != 0; // Another way of checking ragdoll just in case
			}
		}
		return false;
	}

	bool IsinRagdollState(Actor* giant) {
		return giant ? (IsRagdolled(giant) || KnockedDown(giant)) : false;
	}

	bool IsInsect(Actor* actor, bool performcheck) {
		bool Check = Config::Gameplay.ActionSettings.bAllowInsects;

		if ((performcheck && Check) || !actor) {
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

		TESNPC* base = a_Actor->GetActorBase();
		int sex = 0;
		if (base) {
			if (base->GetSex()) {
				sex = base->GetSex();
				//log::info("{} Is Female: {}", actor->GetDisplayFullName(), static_cast<bool>(sex));
			}
		}
		return static_cast<bool>(sex); // Else return false
	}

	bool IsDragon(Actor* actor) {

		if (actor) {
			if (Runtime::HasKeyword(actor, Runtime::KYWD.DragonKeyword) ||
				Runtime::IsRace(actor, Runtime::RACE.DragonRace)) {
				return true;
			}
		}
		return false;
	}

	bool IsGiant(Actor* actor) {
		return actor ? Runtime::IsRace(actor, Runtime::RACE.GiantRace) : false;
	}

	bool IsMammoth(Actor* actor) {
		return actor ? Runtime::IsRace(actor, Runtime::RACE.MammothRace) : false;
	}

	bool IsLiving(Actor* actor) {

		if (!actor) return false;

		const bool IsDraugr = Runtime::HasKeyword(actor, Runtime::KYWD.UndeadKeyword);
		const bool IsDwemer = Runtime::HasKeyword(actor, Runtime::KYWD.DwemerKeyword);
		const bool IsVampire = Runtime::HasKeyword(actor, Runtime::KYWD.VampireKeyword);

		if ((IsDraugr || IsDwemer) && !IsVampire) {
			return false;
		}
		return true;
;
	}

	bool IsUndead(Actor* actor, bool PerformCheck) {
		
		if (!actor) return false;

		const bool IsDraugr = Runtime::HasKeyword(actor, Runtime::KYWD.UndeadKeyword);
		const bool Check = Config::Gameplay.ActionSettings.bAllowUndead;
		if (Check && PerformCheck) {
			return false;
		}
		return IsDraugr;
	}

	bool WasReanimated(Actor* actor) { // must be called while actor is still alive, else it will return false.
		if (actor) {
			if (TransientActorData* data = Transient::GetActorData(actor)) {
				return data->WasReanimated;
			}
		}
		return false;
	}

	bool IsFlying(Actor* actor) {

		if (actor) {
			if (auto acState = actor->AsActorState()) {
				return acState->IsFlying();
			}
		}
		return false;
	}

	bool IsHostile(Actor* giant, Actor* tiny) {
		return (!giant || !tiny) ? false : tiny->IsHostileToActor(giant);
	}

	bool IsEssential(Actor* giant, Actor* actor) {

		if (!giant || !actor) return false;

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
		if (giant) {
			if (giant->IsPlayerRef()) {
				return HasHeadTrackingTarget(giant);
			}
		}
		return false;
	}

	bool IsInGodMode(Actor* giant) {
		if (giant) {
			if (giant->IsPlayerRef()) {
				static const REL::Relocation<bool*> singleton{ REL::RelocationID(517711, 404238, NULL) };
				return *singleton;
			}
		}
		return false;
	}
	bool CanBeCrushed(Actor* actor) {
		auto transient = Transient::GetActorData(actor);
		if (transient) {
			return transient->CanBeCrushed;
		}
		return false;
	}
	bool CanDoDamage(Actor* giant, Actor* tiny, bool HoldCheck) {

		if (!giant || !tiny) return false;

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

		if (!actor) return false;

		int State = AnimationVars::Other::CurrentDefaultState(actor);
		if (State >= 10 && State <= 20) {
			return true;
		}
		return false;
	}

	bool IsRagdolled(Actor* actor) {

		if (!actor) return false;

		bool ragdoll = actor->IsInRagdollState();
		return ragdoll;
	}

	bool InBleedout(Actor* actor) {
		if (actor) {
			if (auto acState = actor->AsActorState()) {
				return acState->IsBleedingOut();
			}
		}
		return false;
	}

	bool IsMechanical(Actor* actor) {
		return actor ? Runtime::HasKeyword(actor, Runtime::KYWD.DwemerKeyword) : false;
	}

	bool IsHuman(Actor* actor) { // Check if Actor is humanoid or not. Currently used for Hugs Animation and for playing moans

		if (!actor) return false;

		const bool vampirelord = Runtime::IsRace(actor, Runtime::RACE.DLC1VampireBeastRace);
		const bool werewolf = Runtime::IsRace(actor, Runtime::RACE.WerewolfBeastRace);

		if (vampirelord || werewolf) {
			return false;
		}

		const bool creature = Runtime::HasKeyword(actor, Runtime::KYWD.CreatureKeyword);
		const bool animal = Runtime::HasKeyword(actor, Runtime::KYWD.AnimalKeyword);
		const bool dragon = Runtime::HasKeyword(actor, Runtime::KYWD.DragonKeyword);

		const bool vampire = Runtime::HasKeyword(actor, Runtime::KYWD.VampireKeyword);
		const bool undead = Runtime::HasKeyword(actor, Runtime::KYWD.UndeadKeyword);

		const bool dwemer = Runtime::HasKeyword(actor, Runtime::KYWD.DwemerKeyword);

		if (!dragon && !animal && !dwemer && !undead && !creature) {
			return true; // Detect non-vampire
		}
		if (!dragon && !animal && !dwemer && !creature && undead && vampire) {
			return true; // Detect Vampire
		}
		if (IsHumanoid(actor)) {
			return true;
		}

		return false;
	}

	bool IsBlacklisted(Actor* actor) {
		return actor ? Runtime::HasKeyword(actor, Runtime::KYWD.GTSKeywordBlackListActor) : true;
	}

	bool IsGTSTeammate(Actor* actor) {
		return actor ? Runtime::HasKeyword(actor, Runtime::KYWD.GTSKeywordCountAsFollower) : false;
	}

	bool TinyCalamityActive(Actor* giant) { // Tiny Calamity should work only on Humanoids
		if (auto TranData = Transient::GetActorData(giant)) { // Should save fps a bit compared to checking MGEF
			return TranData->TinyCalamityActive;
		}
		return IsHumanoid(giant) ? Runtime::HasMagicEffect(giant, Runtime::MGEF.GTSEffectTinyCalamity) : false;
	}

	bool TinyCalamity_ShouldShrinkFirst(Actor* giant, Actor* tiny, float size_diff_requirement, float shrink_until, float shrink_rate_normal, float shrink_rate_sneak) {
		if (!giant->IsPlayerRef() && !Config::AI.bCalamityShrinksFirst) {
			return false; // Allow cursed and weird ass looking anims to happen
		}
		if (get_scale_difference(giant, tiny, SizeType::VisualScale, false, false) < size_diff_requirement) {
			if (!IsActionOnCooldown(giant, CooldownSource::Misc_TinyCalamity_Shrink)) {
				const float shrinkrate = giant->IsSneaking() ? shrink_rate_sneak : shrink_rate_normal;
				ShrinkUntil(giant, tiny, shrink_until, shrinkrate, true);
			} else {
				if (giant->IsPlayerRef()) {
					std::string message = std::format("Shrink is on a cooldown: {:.1f} sec", GetRemainingCooldown(giant, CooldownSource::Misc_TinyCalamity_Shrink));
                    shake_camera(giant, 0.45f, 0.30f);
                    NotifyWithSound(giant, message);
				} else {
					Cprint("{}'s Shrink is on a cooldown:", giant->GetDisplayFullName(), GetRemainingCooldown(giant, CooldownSource::Misc_TinyCalamity_Shrink));
					logger::info("{}'s Shrink is on a cooldown:", giant->GetDisplayFullName(), GetRemainingCooldown(giant, CooldownSource::Misc_TinyCalamity_Shrink));
				}
			}
			return true;
		}
		return false;
	}

}