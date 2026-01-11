#include "Utils/Actor/FindActor.hpp"

#include "Magic/Effects/Common.hpp"

#include "Managers/AI/AIfunctions.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/TinyCalamity_Shrink.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Audio/MoansLaughs.hpp"
#include "Managers/Damage/CollisionDamage.hpp"
#include "Managers/Damage/LaunchPower.hpp"
#include "Managers/GTSSizeManager.hpp"
#include "Managers/HighHeel.hpp"
#include "Managers/Rumble.hpp"

#include "Config/Config.hpp"

/* GTS Utils
 * Contains general helper functions
 * for general Mod-related functionality
 */

namespace {

	using namespace GTS;

	struct SpringGrowData {
		Spring amount = Spring(0.0f, 1.0f);
		float addedSoFar = 0.0f;
		bool drain = false;
		ActorHandle actor;

		SpringGrowData(Actor* a_actor, float a_addAmt, float a_halfLife) : actor(a_actor->CreateRefHandle()) {
			amount.value = 0.0f;
			amount.target = a_addAmt;
			amount.halflife = a_halfLife;
		}
	};

	struct SpringShrinkData {
		Spring amount = Spring(0.0f, 1.0f);
		float addedSoFar = 0.0f;
		ActorHandle actor;

		SpringShrinkData(Actor* a_actor, float a_addAmt, float a_halfLife) : actor(a_actor->CreateRefHandle()) {
			amount.value = 0.0f;
			amount.target = a_addAmt;
			amount.halflife = a_halfLife;
		}
	};

	void ApplyAttributesOnShrink(float& a_health, float& a_magicka, float& a_stamina, float a_value, float a_limit) {
		switch (RandomInt(0, 2)) {
			case 0: { a_health  += (a_value); if (a_health  >= a_limit) { a_health  = a_limit; } break; }
			case 1: { a_magicka += (a_value); if (a_magicka >= a_limit) { a_magicka = a_limit; } break; }
			case 2: { a_stamina += (a_value); if (a_stamina >= a_limit) { a_stamina = a_limit; } break; }
			default: break ;
		}	
	}

	bool Utils_ManageTinyProtection(Actor* a_actor, bool a_doForceCancel, bool a_BalanceMode) {
		float sp = GetAV(a_actor, ActorValue::kStamina);

		if (!a_doForceCancel && a_BalanceMode) {
			float perk = Perk_GetCostReduction(a_actor);
			float damage = 0.08f * TimeScale() * perk;
			if (a_actor->formID != 0x14) {
				damage *= 0.5f; // less stamina drain for NPC's
			}
			DamageAV(a_actor, ActorValue::kStamina, damage);
		}

		if (sp <= 1.0f || a_doForceCancel) {
			float OldScale = AnimationVars::General::GetGiantessScale(a_actor);
			AnimationVars::General::SetGiantessScale(a_actor, 1.0f);

			if (!a_doForceCancel) {
				StaggerActor(a_actor, 0.25f);
			}
			float scale = get_visual_scale(a_actor);

			StaggerActor_Around(a_actor, 48.0f, false);

			auto node = find_node(a_actor, "NPC Root [Root]");
			Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundMagicBreak, a_actor, 1.0f, "NPC COM [COM ]");
			
			if (node) {
				NiPoint3 position = node->world.translate;

				std::string name_com = std::format("BreakProtect_{}", a_actor->formID);
				std::string name_root = std::format("BreakProtect_Root_{}", a_actor->formID);

				Rumbling::Once(name_com, a_actor, Rumble_Misc_FailTinyProtection, 0.20f, "NPC COM [COM ]", 0.0f);
				Rumbling::Once(name_root, a_actor, Rumble_Misc_FailTinyProtection, 0.20f, "NPC Root [Root]", 0.0f);

				SpawnParticle(a_actor, 6.00f, "GTS/Effects/TinyCalamity.nif", NiMatrix3(), position, scale * 3.4f, 7, nullptr); // Spawn it
			}
			AnimationVars::General::SetGiantessScale(a_actor, OldScale);

			return false;
		}
		return true;
	}

	void Task_AdjustHalfLifeTask(Actor* a_target, float a_halfLife, double a_revertAfterTime) {
		auto actor_data = Persistent::GetActorData(a_target);
		float old_halflife = 0.0f;
		if (actor_data) {
			old_halflife = actor_data->fHalfLife; // record old half life
			actor_data->fHalfLife = a_halfLife;
		}

		const double Start = Time::WorldTimeElapsed();
		ActorHandle tinyhandle = a_target->CreateRefHandle();
		std::string name = std::format("AdjustHalfLife_{}", a_target->formID);
		TaskManager::Run(name, [=](auto& progressData) {
			if (!tinyhandle) {
				return false;
			}
			const double timepassed = Time::WorldTimeElapsed() - Start;
			if (timepassed > a_revertAfterTime) {
				if (actor_data) {
					actor_data->fHalfLife = old_halflife;
				}
				return false;
			}
			return true;
		});
	}
}

namespace GTS {

	//----------------------------------------------------
	// OTHER
	//----------------------------------------------------

	float GetAnimationSlowdown(Actor* giant) {

		if (giant) {

			auto& Gen = Config::General;
			auto& Adv = Config::Advanced;

			if (Gen.bDynamicAnimspeed) {

				if (giant->AsActorState()->GetSitSleepState() != SIT_SLEEP_STATE::kNormal) {
					return 1.0f; // For some reason makes furniture angles funny if there's anim slowdown. So we prevent that
				}

				const std::array<float, 5>& SpeedVars = Adv.fAnimSpeedFormula;

				const SoftPotential Speed = {
					SpeedVars[0],
					SpeedVars[1],
					SpeedVars[2],
					SpeedVars[3],
					SpeedVars[4]
				};

				float scale = get_visual_scale(giant);
				float speedmultcalc = soft_core(scale, Speed);
				speedmultcalc = std::clamp(speedmultcalc, Adv.fAnimspeedLowestBoundAllowed, 1.0f);

				if (IsGtsBusy(giant) && Adv.bGTSAnimsFullSpeed) {
					return 1.0f;
				}
				if (giant->formID == 0x14) {
					return Adv.fAnimSpeedAdjMultPlayer * speedmultcalc;
				}
				if (IsTeammate(giant)) {
					return Adv.fAnimSpeedAdjMultTeammate * speedmultcalc;
				}

				return speedmultcalc;
			}
		}
		return 1.0f;
	}

	float GetMovementModifier(Actor* a_target) {
		float modifier = 1.0f;
		if (a_target->AsActorState()->IsSprinting()) {
			modifier *= 1.33f;
		}
		if (a_target->AsActorState()->IsWalking()) {
			modifier *= 0.75f;
		}
		if (a_target->AsActorState()->IsSneaking()) {
			modifier *= 0.75f;
		}
		return modifier;
	}

	float GetRandomBoost() {
		float rng = (RandomFloat(0, 150));
		float random = rng / 100.f;
		return random;
	}

	void StartActorResetTask(Actor* a_target) {
		if (a_target->formID == 0x14) {
			return; //Don't reset Player
		}
		std::string name = std::format("ResetActor_{}", a_target->formID);
		double Start = Time::WorldTimeElapsed();
		ActorHandle tinyhandle = a_target->CreateRefHandle();
		TaskManager::Run(name, [=](auto&) {
			if (!tinyhandle) {
				return false;
			}
			auto tiny = tinyhandle.get().get();
			double Finish = Time::WorldTimeElapsed();
			double timepassed = Finish - Start;
			if (timepassed < 1.0) {
				return true; // not enough time has passed yet
			}
			EventDispatcher::DoResetActor(tiny);
			return false; // stop task, we reset the actor
		});
	}

	float GetFallModifier(Actor* a_target) {
		auto transient = Transient::GetActorData(a_target);
		float fallmod = 1.0f;
		if (transient) {
			fallmod = transient->FallTimer;
			//log::info("Fall mult :{}", transient->FallTimer);
		}
		return fallmod;
	}

	bool AllowStagger(Actor* a_target) {
		bool giantIsFriendly = (a_target->formID == 0x14 || IsTeammate(a_target));
		bool tinyIsFriendly = (a_target->formID == 0x14 || IsTeammate(a_target));

		//If Tiny is follower or player dont allow stagger
		if (tinyIsFriendly && giantIsFriendly) {
			return Config::Balance.bAllowFriendlyStagger;
		}

		//If tiny is Other npc return settings option
		return Config::Balance.bAllowOthersStagger;

	}

	void GainWeight(Actor* a_target, float a_value) {

		if (Config::Gameplay.ActionSettings.bVoreWeightGain) {

			if (a_target->formID == 0x14) {
				std::string_view name = "Vore_Weight";
				auto gianthandle = a_target->CreateRefHandle();
				TaskManager::RunOnce(name, [=](auto&) {
					if (!gianthandle) {
						return false;
					}
					auto giantref = gianthandle.get().get();
					float& original_weight = giantref->GetActorBase()->weight;
					if (original_weight >= 100.0f) {
						return false;
					}
					if (original_weight + a_value >= 100.0f) {
						original_weight = 100.0f;
					}
					else {
						original_weight += a_value;
					}
					giantref->DoReset3D(true);
					return false;
				});
			}
		}
	}

	bool DisallowSizeDamage(Actor* a_source, Actor* a_target) {
		auto transient = Transient::GetActorData(a_source);
		if (transient) {
			if (transient->Protection == false) {
				return false;
			}

			bool Hostile = IsHostile(a_source, a_target);
			return transient->Protection && !Hostile;
		}

		return false;
	}

	// determines if we want to apply size effects for literally every single actor
	bool EffectsForEveryone(Actor* a_target) { 
		if (a_target->formID == 0x14) { // don't enable for Player
			return false;
		}
		bool dead = a_target->IsDead();
		bool everyone = Config::General.bAllActorSizeEffects || CountAsGiantess(a_target);
		if (!dead && everyone) {
			return true;
		}
		return false;
	}

	//----------------------------------------------------
	// MAGIC
	//----------------------------------------------------

	void Potion_SetMightBonus(Actor* a_target, float a_value, bool a_Add) {
		if (TransientActorData* transient = Transient::GetActorData(a_target)) {
			a_Add ? transient->MightValue += a_value : transient->MightValue = a_value;
		}
	}

	float Potion_GetMightBonus(Actor* a_target) {
		if (TransientActorData* transient = Transient::GetActorData(a_target)) {
			return transient->MightValue; // return raw bonus
		}
		return 0.0f;
	}

	float Potion_GetSizeMultiplier(Actor* a_target) {
		if (TransientActorData* transient = Transient::GetActorData(a_target)) {
			return 1.0f + std::clamp(transient->PotionMaxSize, 0.0f, 10.0f);
		}
		return 1.0f;
	}

	void Potion_ModShrinkResistance(Actor* a_target, float a_value) {
		if (TransientActorData* transient = Transient::GetActorData(a_target)) {
			transient->ShrinkResistance += a_value;
		}
	}

	void Potion_SetShrinkResistance(Actor* a_target, float a_value) {
		if (TransientActorData* transient = Transient::GetActorData(a_target)) {
			transient->ShrinkResistance = a_value;
		}
	}

	float Potion_GetShrinkResistance(Actor* a_target) {
		float Resistance = 1.0f;
		if (TransientActorData* transient = Transient::GetActorData(a_target)) {
			Resistance -= transient->ShrinkResistance;
		}
		return std::clamp(Resistance, 0.05f, 1.0f);
	}

	void Potion_SetUnderGrowth(Actor* a_target, bool a_set) {
		if (TransientActorData* transient = Transient::GetActorData(a_target)) {
			transient->GrowthPotion = a_set;
		}
	}

	bool Potion_IsUnderGrowthPotion(Actor* a_actor) {
		bool UnderGrowth = false;
		if (TransientActorData* transient = Transient::GetActorData(a_actor)) {
			UnderGrowth = transient->GrowthPotion;
		}
		return UnderGrowth;
	}

	float Ench_Aspect_GetPower(Actor* a_actor) {
		return SizeManager::GetSingleton().GetEnchantmentBonus(a_actor) * 0.01f;

	}

	float Ench_Hunger_GetPower(Actor* a_actor) {
		return SizeManager::GetSingleton().GetSizeHungerBonus(a_actor) * 0.01f;
	}

	//----------------------------------------------------
	// PERKS
	//----------------------------------------------------

	float Perk_GetSprintShrinkReduction(Actor* actor) {
		float resistance = 1.0f;
		if (actor->AsActorState()->IsSprinting() && Runtime::HasPerkTeam(actor, Runtime::PERK.GTSPerkSprintDamageMult1)) {
			resistance -= 0.20f;
		}
		return resistance;
	}

	float GetPerkBonus_OnTheEdge(Actor* giant, float amt) {
		// When health is < 60%, empower growth by up to 50%. Max value at 10% health.
		float bonus = 1.0f;
		bool perk = Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkOnTheEdge);
		if (perk) {
			float hpFactor = std::clamp(GetHealthPercentage(giant) + 0.4f, 0.5f, 1.0f);
			bonus = (amt > 0.0f) ? (2.0f - hpFactor) : hpFactor;
			// AMT > 0 = increase size gain ( 1.5 at low hp )
			// AMT < 0 = decrease size loss ( 0.5 at low hp )
		}
		return bonus;
	}

	float Perk_GetCostReduction(Actor* giant) {
		float cost = 1.0f;
		float reduction_1 = 0.0f;
		float reduction_2 = 1.0f;
		if (Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkExperiencedGiantess)) {
			reduction_1 += std::clamp(GetGtsSkillLevel(giant) * 0.0035f, 0.0f, 0.35f);
		}
		if (giant->formID == 0x14 && HasGrowthSpurt(giant)) {
			if (Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkGrowthAug1)) {
				reduction_2 -= 0.10f;
			}
			if (Runtime::HasPerk(giant, Runtime::PERK.GTSPerkExtraGrowth1)) {
				reduction_2 -= 0.30f;
			}
		}
		cost -= reduction_1;
		cost *= reduction_2;
		cost *= (1.0f - Potion_GetMightBonus(giant));
		cost *= (1.0f - std::clamp(GetGtsSkillLevel(giant) * 0.0020f, 0.0f, 0.20f)); // Based on skill tree progression
		return cost;
	}

	void DragonAbsorptionBonuses() { // The function is ugly but im a bit lazy to make it look pretty
		int rng = RandomInt(0, 6);
		int dur_rng = RandomInt(0, 3);
		float size_increase = 0.12f / Characters_AssumedCharSize; // +12 cm;
		float size_boost = 1.0f;

		Actor* player = PlayerCharacter::GetSingleton();

		if (!Runtime::HasPerk(player, Runtime::PERK.GTSPerkMightOfDragons)) {
			return;
		}

		if (Runtime::HasPerk(player, Runtime::PERK.GTSPerkColossalGrowth)) {
			size_boost = 1.2f;
		}

		if (auto data = Persistent::GetActorData(PlayerCharacter::GetSingleton())) {
			data->fExtraPotionMaxScale += size_increase * size_boost;
		}

		ModSizeExperience(player, 0.45f);
		Notify("You feel like something is filling you");

		if (rng <= 1) {
			Sound_PlayMoans(player, 1.0f, 0.14f, EmotionTriggerSource::Absorption);
			Task_FacialEmotionTask_Moan(player, 1.6f, "DragonVored");
			shake_camera(player, 0.5f, 0.33f);
		}

		SpawnCustomParticle(player, ParticleType::Red, NiPoint3(), "NPC COM [COM ]", get_visual_scale(player) * 1.6f);

		ActorHandle gianthandle = player->CreateRefHandle();
		std::string name = std::format("DragonGrowth_{}", player->formID);

		float HpRegen = GetMaxAV(player, ActorValue::kHealth) * 0.00125f;
		float Gigantism = 1.0f + Ench_Aspect_GetPower(player);

		float duration = 6.0f + dur_rng;

		TaskManager::RunFor(name, duration, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			auto giantref = gianthandle.get().get();
			ApplyShakeAtNode(giantref, Rumble_Misc_MightOfDragons, "NPC COM [COM ]");
			update_target_scale(giantref, 0.0026f * Gigantism * TimeScale(), SizeEffectType::kGrow);
			giantref->AsActorValueOwner()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, ActorValue::kHealth, HpRegen * TimeScale());
			return true;
			});
	}

	//----------------------------------------------------
	// HIGH HEEL
	//----------------------------------------------------

	float GetHighHeelsBonusDamage(Actor* actor, bool multiply) {
		return GetHighHeelsBonusDamage(actor, multiply, 1.0f);
	}

	float GetHighHeelsBonusDamage(Actor* actor, bool multiply, float adjust) {
		GTS_PROFILE_SCOPE("ActorUtils: GetHighHeelsBonusDamage");
		float value;
		float hh = 0.0f;

		if (actor) {
			if (Runtime::HasPerkTeam(actor, Runtime::PERK.GTSPerkHighHeels)) {
				hh = HighHeelManager::GetInitialHeelHeight(actor);
			}
		} if (multiply) {
			value = 1.0f + (hh * 5.0f * adjust);
		} else {
			value = hh;
		}
		//log::info("For Actor: {}: {}", actor->GetDisplayFullName(), value);
		return value;
	}

	bool BehaviorGraph_DisableHH(Actor* actor) { // should .dll disable HH if Behavior Graph has HH Disable data?
		bool disable = AnimationVars::General::GetHHDisabled(actor);

		if (actor->formID == 0x14 && IsFirstPerson()) {
			return false;
		}
		if (!AnimationVars::Utility::GetIsInstalled(actor)) {
			return false; // prevent hh from being disabled if there's no Nemesis Generation
		}

		return disable;
	}

	//----------------------------------------------------
	// ATTRIBUTES
	//----------------------------------------------------


	float GetStolenAttributeCap(Actor* giant) {
		const uint16_t Level = giant->GetLevel();
		const float modifier = Config::Gameplay.fFullAssimilationLevelCap;
		const float cap = static_cast<float>(Level) * modifier * 2.0f;

		return cap;
	}

	void AddStolenAttributes(Actor* giant, float value) {
		if (giant->formID == 0x14 && Runtime::HasPerk(giant, Runtime::PERK.GTSPerkFullAssimilation)) {
			auto attributes = Persistent::GetActorData(giant);
			if (attributes) {
				const float cap = GetStolenAttributeCap(giant);

				float& health = attributes->fStolenHealth;
				float& magick = attributes->fStolenMagicka;
				float& stamin = attributes->fStolenStamina;

				if (health >= cap && magick >= cap && stamin >= cap) { // If we're at limit, don't add them
					attributes->fStolenAttibutes = 0.0f;
					return;
				} else { // Add in all other cases
					attributes->fStolenAttibutes += value;
					attributes->fStolenAttibutes = std::max(attributes->fStolenAttibutes, 0.0f);
				}
			}
		}
	}

	void AddStolenAttributesTowards(Actor* giant, ActorValue type, float value) {
		if (giant->formID == 0x14) {
			auto Persistent = Persistent::GetActorData(giant);
			if (Persistent) {
				float& health = Persistent->fStolenHealth;
				float& magick = Persistent->fStolenMagicka;
				float& stamin = Persistent->fStolenStamina;
				const float limit = GetStolenAttributeCap(giant);

				if (type == ActorValue::kHealth && health < limit) {
					health += value;
					health = std::min(health, limit);
					//log::info("Adding {} to health, health: {}", value, health);
				} else if (type == ActorValue::kMagicka && magick < limit) {
					magick += value;
					magick = std::min(magick, limit);
					//log::info("Adding {} to magick, magicka: {}", value, magick);
				} else if (type == ActorValue::kStamina && stamin < limit) {
					stamin += value;
					stamin = std::min(stamin, limit);
					//log::info("Adding {} to stamina, stamina: {}", value, stamin);
				}
			}
		}
	}

	float GetStolenAttributes_Values(Actor* giant, ActorValue type) {
		if (giant->formID == 0x14) {
			auto Persistent = Persistent::GetActorData(giant);
			if (Persistent) {
				float max = GetStolenAttributeCap(giant);
				if (type == ActorValue::kHealth) {
					return std::min(Persistent->fStolenHealth, max);
				} else if (type == ActorValue::kMagicka) {
					return std::min(Persistent->fStolenMagicka, max);
				} else if (type == ActorValue::kStamina) {
					return std::min(Persistent->fStolenStamina, max);
				} else {
					return 0.0f;
				}
			}
			return 0.0f;
		}
		return 0.0f;
	}

	float GetStolenAttributes(Actor* giant) {
		auto persist = Persistent::GetActorData(giant);
		if (persist) {
			return persist->fStolenAttibutes;
		}
		return 0.0f;
	}

	void DistributeStolenAttributes(Actor* giant, float value) {
		if (value > 0 && giant->formID == 0x14 && Runtime::HasPerk(giant, Runtime::PERK.GTSPerkFullAssimilation)) { // Permamently increases random AV after shrinking and stuff
			float scale = std::clamp(get_visual_scale(giant), 0.01f, 1000000.0f);
			float Storage = GetStolenAttributes(giant);
			float limit = GetStolenAttributeCap(giant);

			auto Persistent = Persistent::GetActorData(giant);
			if (!Persistent) {
				return;
			}
			//log::info("Adding {} to attributes", value);
			float& health = Persistent->fStolenHealth;
			float& magick = Persistent->fStolenMagicka;
			float& stamin = Persistent->fStolenStamina;

			value = std::clamp(value, 0.0f, Storage); // Can't be stronger than storage bonus

			if (Storage > 0.0f) {
				ApplyAttributesOnShrink(health, magick, stamin, value, limit);
				AddStolenAttributes(giant, -value); // reduce it
			}
		}
	}

	//----------------------------------------------------
	// DAMAGE
	//----------------------------------------------------

	void DoDamageEffect(Actor* giant, float damage, float radius, int random, float bonedamage, FootEvent kind, float crushmult, DamageSource Cause) {
		DoDamageEffect(giant, damage, radius, random, bonedamage, kind, crushmult, Cause, false);
	}

	void DoDamageEffect(Actor* giant, float damage, float radius, int random, float bonedamage, FootEvent kind, float crushmult, DamageSource Cause, bool ignore_rotation) {
		if (kind == FootEvent::Left) {
			CollisionDamage::DoFootCollision(giant, damage, radius, random, bonedamage, crushmult, Cause, false, false, ignore_rotation, true);
		}
		if (kind == FootEvent::Right) {
			CollisionDamage::DoFootCollision(giant, damage, radius, random, bonedamage, crushmult, Cause, true, false, ignore_rotation, true);
			//                                                                                  ^         ^         ^ - - - - Normal Crush
			//                                                       Chance to trigger bone crush   Damage of            Threshold multiplication
			//                                                                                      Bone Crush
		}
	}

	void InflictSizeDamage(Actor* attacker, Actor* receiver, float value) {

		if (attacker->formID == 0x14 && IsTeammate(receiver)) {
			if (Config::Balance.bFollowerFriendlyImmunity) {
				return;
			}
		}

		if (receiver->formID == 0x14 && IsTeammate(attacker)) {
			if (Config::Balance.bPlayerFriendlyImmunity) {
				return;
			}
		}

		if (!receiver->IsDead()) {
			float HpPercentage = GetHealthPercentage(receiver);
			float difficulty = 2.0f; // taking Legendary Difficulty as a base
			float levelbonus = 1.0f + ((GetGtsSkillLevel(attacker) * 0.01f) * 0.50f);
			value *= levelbonus;

			if (receiver->formID != 0x14) { // Mostly a warning to indicate that actor dislikes it (They don't always aggro right away, with mods at least)
				if (value >= GetAV(receiver, ActorValue::kHealth) * 0.50f || HpPercentage < 0.70f) { // in that case make hostile
					if (!IsTeammate(receiver) && !IsHostile(attacker, receiver)) {
						StartCombat(receiver, attacker); // Make actor hostile and add bounty of 40 (can't be configured, needs different hook probably). 
					}
				}
				if (value > 1.0f) { // To prevent aggro when briefly colliding
					Attacked(receiver, attacker);
				}
			}

			ApplyDamage(attacker, receiver, value * difficulty * Config::Balance.fSizeDamageMult);
		}
		else if (receiver->IsDead()) {
			Task_InitHavokTask(receiver);
			// ^ Needed to fix this issue:
			//   https://www.reddit.com/r/skyrimmods/comments/402b69/help_looking_for_a_bugfix_dead_enemies_running_in/
		}

	}

	//----------------------------------------------------
	// TINY CALAMITY
	//----------------------------------------------------

	void TinyCalamityExplosion(Actor* giant, float radius) { // Meant to just stagger actors
		if (!giant) {
			return;
		}
		auto node = find_node(giant, "NPC Root [Root]");
		if (!node) {
			return;
		}
		float giantScale = get_visual_scale(giant);
		NiPoint3 NodePosition = node->world.translate;
		const float maxDistance = radius;
		float totaldistance = maxDistance * giantScale;
		// Make a list of points to check
		if (DebugDraw::CanDraw(giant, DebugDraw::DrawTarget::kPlayerOnly)) {
			DebugDraw::DrawSphere(glm::vec3(NodePosition.x, NodePosition.y, NodePosition.z), totaldistance, 600, {0.0f, 1.0f, 0.0f, 1.0f});
		}

		NiPoint3 giantLocation = giant->GetPosition();

		for (auto otherActor: find_actors()) {
			if (otherActor != giant) {
				NiPoint3 actorLocation = otherActor->GetPosition();
				if ((actorLocation-giantLocation).Length() < (maxDistance*giantScale * 3.0f)) {
					int nodeCollisions = 0;
					auto model = otherActor->GetCurrent3D();
					if (model) {
						VisitNodes(model, [&nodeCollisions, NodePosition, totaldistance](NiAVObject& a_obj) {
							float distance = (NodePosition - a_obj.world.translate).Length();
							if (distance < totaldistance) {
								nodeCollisions += 1;
								return false;
							}
							return true;
						});
					}
					if (nodeCollisions > 0) {
						float sizedifference = giantScale/get_visual_scale(otherActor);
						if (sizedifference <= 1.6f) {
							StaggerActor(giant, otherActor, 0.75f);
						} else {
							PushActorAway(giant, otherActor, 1.0f * GetLaunchPowerFor(giant, sizedifference, LaunchType::Actor_Towards));
						}
					}
				}
			}
		}
	}

	void AddSMTDuration(Actor* actor, float duration, bool perk_check) {
		if (HasSMT(actor)) {
			if (!perk_check || Runtime::HasPerk(actor, Runtime::PERK.GTSPerkTinyCalamityRefresh)) {
				auto transient = Transient::GetActorData(actor);
				if (transient) {
					transient->SMTBonusDuration += duration;
					//log::info("Adding perk duration");
				}
			}
		}
	}

	void AddSMTPenalty(Actor* actor, float penalty) {
		auto transient = Transient::GetActorData(actor);
		if (transient) {
			float skill_level = (GetGtsSkillLevel(actor) * 0.01f) - 0.65f;
			float level_bonus = std::clamp(skill_level, 0.0f, 0.35f) * 2.0f;
			float reduction = 1.0f - level_bonus; // up to 70% reduction of penalty

			transient->SMTPenaltyDuration += penalty * reduction;
		}
	}

	//----------------------------------------------------
	// IMMUNITY
	//----------------------------------------------------

	void Utils_ProtectTinies(bool Balance) { // This is used to avoid damaging friendly actors in towns and in general
		auto player = PlayerCharacter::GetSingleton();

		for (auto actor: find_actors()) {
			if (actor == player || IsTeammate(actor)) {
				float scale = get_visual_scale(actor);

				SpawnCustomParticle(actor, ParticleType::Red, NiPoint3(), "NPC Root [Root]", scale * 1.15f);
				Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundMagicProctectTinies, actor, 1.0f, "NPC COM [COM ]");

				std::string name_com = std::format("Protect_{}", actor->formID);
				std::string name_root = std::format("Protect_Root_{}", actor->formID);

				Rumbling::Once(name_com, actor, 4.0f, 0.20f, "NPC COM [COM ]", 0.0f);
				Rumbling::Once(name_root, actor, 4.0f, 0.20f, "NPC Root [Root]", 0.0f);
				
				LaunchImmunityTask(actor, Balance);
			}
		}
	}

	void LaunchImmunityTask(Actor* giant, bool Balance) {
		auto transient = Transient::GetActorData(giant);
		if (transient) {
			transient->Protection = true;
		}

		std::string name = std::format("Protect_{}", giant->formID);
		std::string name_1 = std::format("Protect_1_{}", giant->formID);

		TaskManager::Cancel(name); // Stop old task if it's been running

		Rumbling::Once(name, giant, Rumble_Misc_EnableTinyProtection, 0.20f, "NPC COM [COM ]", 0.0f);
		Rumbling::Once(name_1, giant, Rumble_Misc_EnableTinyProtection, 0.20f, "NPC Root [Root]", 0.0f);

		double Start = Time::WorldTimeElapsed();
		ActorHandle gianthandle = giant->CreateRefHandle();
		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}

			double Finish = Time::WorldTimeElapsed();
			double timepassed = Finish - Start;
			if (timepassed < 180.0f) {
				auto giantref = gianthandle.get().get();
				if (Utils_ManageTinyProtection(giant, false, Balance)) {
					return true; // Disallow to check further
				}
			}
			if (transient) {
				transient->Protection = false; // reset protection to default value
			}
			return Utils_ManageTinyProtection(giant, true, Balance); // stop task, immunity has ended
		});
	}

	//----------------------------------------------------
	// SIZE RELATED
	//----------------------------------------------------

	void ShrinkOutburst_Shrink(Actor* giant, Actor* tiny, float shrink, float gigantism) {
		if (IsEssential_WithIcons(giant, tiny)) { // Protect followers/essentials
			return;
		}
		bool DarkArts1 = Runtime::HasPerk(giant, Runtime::PERK.GTSPerkDarkArtsAug1);
		bool DarkArts2 = Runtime::HasPerk(giant, Runtime::PERK.GTSPerkDarkArtsAug2);
		bool DarkArts_Legendary = Runtime::HasPerk(giant, Runtime::PERK.GTSPerkDarkArtsLegendary);

		float shrinkpower = (shrink * 0.35f) * (1.0f + (GetGtsSkillLevel(giant) * 0.005f)) * CalcEffeciency(giant, tiny);

		float Adjustment = GetSizeFromBoundingBox(tiny);

		float sizedifference = get_scale_difference(giant, tiny, SizeType::VisualScale, false, false);
		if (DarkArts1) {
			giant->AsActorValueOwner()->RestoreActorValue(ACTOR_VALUE_MODIFIER::kDamage, ActorValue::kHealth, 8.0f);
		}
		if (DarkArts2 && (IsGrowthSpurtActive(giant) || HasSMT(giant))) {
			shrinkpower *= 1.40f;
		}

		update_target_scale(tiny, -(shrinkpower * gigantism), SizeEffectType::kShrink);
		Attacked(tiny, giant);

		ModSizeExperience(giant, (shrinkpower * gigantism) * 0.60f);

		float MinScale = SHRINK_TO_NOTHING_SCALE / Adjustment;

		if (get_target_scale(tiny) <= MinScale) {
			set_target_scale(tiny, MinScale);
			if (DarkArts_Legendary && ShrinkToNothing(giant, tiny, true, 0.01f, 0.75f, false, true)) {
				return;
			}
		}
		if (!IsBetweenBreasts(tiny)) {
			if (sizedifference >= 0.9f) { // Stagger or Push
				float stagger = std::clamp(sizedifference, 1.0f, 4.0f);
				StaggerActor(giant, tiny, 0.25f * stagger);
			}
		}
	}

	void ShrinkUntil(Actor* giant, Actor* tiny, float expected, float halflife, bool animation) {
		if (HasSMT(giant)) {
			float Adjustment_Gts = GetSizeFromBoundingBox(giant);
			float Adjustment_Tiny = GetSizeFromBoundingBox(tiny);
			float predscale = get_visual_scale(giant) * Adjustment_Gts;
			float preyscale = get_target_scale(tiny) * Adjustment_Tiny;
			float targetScale = predscale/(expected * Adjustment_Tiny);

			/*log::info("Trying to Shrink {}", tiny->GetDisplayFullName());
			log::info("----Adjustment: GTS: {}", Adjustment_Gts);
			log::info("----Adjustment: Tiny: {}", Adjustment_Tiny);
			log::info("----Pred scale: {}", predscale);
			log::info("----Prey scale: {}", preyscale);
			log::info("----Targeted Scale: {}", targetScale);
			log::info("----Get Target Scale: {}", get_target_scale(tiny));
			*/

			if (preyscale > targetScale) { // Apply ONLY if target is bigger than requirement

				if (animation) {
					Animation_TinyCalamity::AddToData(giant, tiny, expected);
					AnimationManager::StartAnim("Calamity_ShrinkOther", giant);
					tiny->StopMoving(1.2f);
					return;
				}

				Task_AdjustHalfLifeTask(tiny, halflife, 1.2); // to make them shrink faster
				AddSMTPenalty(giant, 5.0f * Adjustment_Tiny);
				set_target_scale(tiny, targetScale);
				StartCombat(tiny, giant);
				
			}
		}
	}

	void SpringGrow(Actor* a_actor, float a_amt, float a_halfLife, std::string_view a_taskName, bool a_drain) {
		if (!a_actor) {
			return;
		}

		auto growData = std::make_shared<SpringGrowData>(a_actor, a_amt, a_halfLife);
		std::string name = std::format("SpringGrow_{}_{}", a_taskName, a_actor->formID);
		const float DURATION = a_halfLife * 3.2f;
		growData->drain = a_drain;

		TaskManager::RunFor(DURATION, [ growData ](const auto&) {
			float totalScaleToAdd = growData->amount.value;
			float prevScaleAdded = growData->addedSoFar;
			float deltaScale = totalScaleToAdd - prevScaleAdded;
			bool drain_stamina = growData->drain;
			Actor* actor = growData->actor.get().get();

			if (actor) {
				if (drain_stamina) {
					float stamina = std::clamp(GetStaminaPercentage(actor), 0.05f, 1.0f);
					DamageAV(actor, ActorValue::kStamina, 0.55f * (get_visual_scale(actor) * 0.5f + 0.5f) * stamina * TimeScale());
				}
				auto actorData = Persistent::GetActorData(actor);
				if (actorData) {
					float scale = get_target_scale(actor);
					float max_scale = get_max_scale(actor);
					if (scale < max_scale) {
						if (!drain_stamina) { // Apply only to growth with animation
							actorData->fVisualScale += deltaScale;
						}
						actorData->fTargetScale += deltaScale;
						growData->addedSoFar = totalScaleToAdd;
					}
				}
			}
			return fabs(growData->amount.value - growData->amount.target) > 1e-4;
		});
	}

	void SpringShrink(Actor* a_actor, float a_amt, float a_halfLife, std::string_view a_taskName) {
		if (!a_actor) {
			return;
		}

		shared_ptr<SpringShrinkData> growData = std::make_shared<SpringShrinkData>(a_actor, a_amt, a_halfLife);
		std::string name = std::format("SpringShrink_{}_{}", a_taskName, a_actor->formID);
		const float DURATION = a_halfLife * 3.2f;
		TaskManager::RunFor(DURATION,[ growData ](const auto&) {
			float totalScaleToAdd = growData->amount.value;
			float prevScaleAdded = growData->addedSoFar;
			float deltaScale = totalScaleToAdd - prevScaleAdded;
			Actor* actor = growData->actor.get().get();

			if (actor) {
				float stamina = std::clamp(GetStaminaPercentage(actor), 0.05f, 1.0f);
				DamageAV(actor, ActorValue::kStamina, 0.35f * (get_visual_scale(actor) * 0.5f + 0.5f) * stamina * TimeScale());
				auto actorData = Persistent::GetActorData(actor);
				if (actorData) {
					actorData->fTargetScale += deltaScale;
					actorData->fVisualScale += deltaScale;
					growData->addedSoFar = totalScaleToAdd;
				}
			}
			return fabs(growData->amount.value - growData->amount.target) > 1e-4;
		});
	}
}