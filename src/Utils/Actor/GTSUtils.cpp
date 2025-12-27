#include "Utils/Actor/GTSUtils.hpp"
#include "Utils/Actor/FindActor.hpp"

#include "Magic/Effects/Common.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/TinyCalamity_Shrink.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Damage/CollisionDamage.hpp"
#include "Managers/Damage/LaunchPower.hpp"
#include "Managers/AI/AIfunctions.hpp"
#include "Managers/Rumble.hpp"
#include "Managers/GTSSizeManager.hpp"
#include "Managers/HighHeel.hpp"
#include "Managers/Audio/MoansLaughs.hpp"

#include "Config/Config.hpp"
#include "Debug/DebugDraw.hpp"


using namespace GTS;

namespace {

	
	

	//constexpr SoftPotential getspeed {
	//	// https://www.desmos.com/calculator/vyofjrqmrn
	//	.k = 0.142f, 
	//	.n = 0.82f,
	//	.s = 1.90f, 
	//	.o = 1.0f,
	//	.a = 0.0f,  //Default is 0
	//};

	void ApplyAttributesOnShrink(float& health, float& magick, float& stamin, float value, float limit) {
		switch (RandomInt(0, 2)) {
			case 0: { health += (value); if (health >= limit) { health = limit; } break; }
			case 1: { magick += (value); if (magick >= limit) { magick = limit; } break; }
			case 2: { stamin += (value); if (stamin >= limit) { stamin = limit; } break; }
			default: break ;
		}	
	}



	bool Utils_ManageTinyProtection(Actor* giantref, bool force_cancel, bool Balance) {
		float sp = GetAV(giantref, ActorValue::kStamina);

		if (!force_cancel && Balance) {
			float perk = Perk_GetCostReduction(giantref);
			float damage = 0.08f * TimeScale() * perk;
			if (giantref->formID != 0x14) {
				damage *= 0.5f; // less stamina drain for NPC's
			}
			DamageAV(giantref, ActorValue::kStamina, damage);
		}

		if (sp <= 1.0f || force_cancel) {
			float OldScale;
			giantref->GetGraphVariableFloat("GiantessScale", OldScale); // save old scale
			giantref->SetGraphVariableFloat("GiantessScale", 1.0f); // Needed to allow Stagger to play, else it won't work

			if (!force_cancel) {
				StaggerActor(giantref, 0.25f);
			}
			float scale = get_visual_scale(giantref);

			StaggerActor_Around(giantref, 48.0f, false);

			auto node = find_node(giantref, "NPC Root [Root]");
			Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundMagicBreak, giantref, 1.0f, "NPC COM [COM ]");
			
			if (node) {
				NiPoint3 position = node->world.translate;

				std::string name_com = std::format("BreakProtect_{}", giantref->formID);
				std::string name_root = std::format("BreakProtect_Root_{}", giantref->formID);

				Rumbling::Once(name_com, giantref, Rumble_Misc_FailTinyProtection, 0.20f, "NPC COM [COM ]", 0.0f);
				Rumbling::Once(name_root, giantref, Rumble_Misc_FailTinyProtection, 0.20f, "NPC Root [Root]", 0.0f);

				SpawnParticle(giantref, 6.00f, "GTS/Effects/TinyCalamity.nif", NiMatrix3(), position, scale * 3.4f, 7, nullptr); // Spawn it
			}
			giantref->SetGraphVariableFloat("GiantessScale", OldScale);

			return false;
		}
		return true;
	}

	float ShakeStrength(Actor* Source) {
		float Size = get_visual_scale(Source);
		float k = 0.065f;
		float n = 1.0f;
		float s = 1.12f;
		float Result = 1.0f/(pow(1.0f+pow(k*(Size),n*s),1.0f/s));
		return Result;
	}

	ExtraDataList* CreateExDataList() {
		size_t a_size;
		if (SKYRIM_REL_CONSTEXPR (REL::Module::IsAE()) && (REL::Module::get().version() >= SKSE::RUNTIME_SSE_1_6_629)) {
			a_size = 0x20;
		} else {
			a_size = 0x18;
		}
		auto memory = RE::malloc(a_size);
		std::memset(memory, 0, a_size);
		if (SKYRIM_REL_CONSTEXPR (REL::Module::IsAE()) && (REL::Module::get().version() >= SKSE::RUNTIME_SSE_1_6_629)) {
			// reinterpret_cast<std::uintptr_t*>(memory)[0] = a_vtbl; // Unknown vtable location add once REd
			REL::RelocateMember<BSReadWriteLock>(memory, 0x18) = BSReadWriteLock();
		} else {
			REL::RelocateMember<BSReadWriteLock>(memory, 0x10) = BSReadWriteLock();
		}
		return static_cast<ExtraDataList*>(memory);
	}

	struct SpringGrowData {
		Spring amount = Spring(0.0f, 1.0f);
		float addedSoFar = 0.0f;
		bool drain = false;
		ActorHandle actor;

		SpringGrowData(Actor* actor, float amountToAdd, float halfLife) : actor(actor->CreateRefHandle()) {
			amount.value = 0.0f;
			amount.target = amountToAdd;
			amount.halflife = halfLife;
		}
	};

	struct SpringShrinkData {
		Spring amount = Spring(0.0f, 1.0f);
		float addedSoFar = 0.0f;
		ActorHandle actor;

		SpringShrinkData(Actor* actor, float amountToAdd, float halfLife) : actor(actor->CreateRefHandle()) {
			amount.value = 0.0f;
			amount.target = amountToAdd;
			amount.halflife = halfLife;
		}
	};
}

//RE::ExtraDataList::~ExtraDataList() = default;

namespace GTS {

	void Task_AdjustHalfLifeTask(Actor* tiny, float halflife, double revert_after) {
		auto actor_data = Persistent::GetActorData(tiny);
		float old_halflife = 0.0f;
		if (actor_data) {
			old_halflife = actor_data->fHalfLife; // record old half life
			actor_data->fHalfLife = halflife;
		}

		double Start = Time::WorldTimeElapsed();
		ActorHandle tinyhandle = tiny->CreateRefHandle();
		std::string name = std::format("AdjustHalfLife_{}", tiny->formID);
		TaskManager::Run(name, [=](auto& progressData) {
			if (!tinyhandle) {
				return false;
			}
			auto tinyref = tinyhandle.get().get();
			double timepassed = Time::WorldTimeElapsed() - Start;
			if (timepassed > revert_after) {
				if (actor_data) {
					actor_data->fHalfLife = old_halflife;
				}
				return false;
			}

			return true;
		});
	}

	void StartResetTask(Actor* tiny) {
		if (tiny->formID == 0x14) {
			return; //Don't reset Player
		}
		std::string name = std::format("ResetActor_{}", tiny->formID);
		double Start = Time::WorldTimeElapsed();
		ActorHandle tinyhandle = tiny->CreateRefHandle();
		TaskManager::Run(name, [=](auto& progressData) {
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

	//----------------------------------------------------
	// MAGIC
	//----------------------------------------------------

	void Potion_SetMightBonus(Actor* giant, float value, bool add) {
		auto transient = Transient::GetActorData(giant);
		if (transient) {
			if (add) {
				transient->MightValue += value;
			} else {
				transient->MightValue = value;
			}
		}
	}

	float Potion_GetMightBonus(Actor* giant) {
		auto transient = Transient::GetActorData(giant);
		if (transient) {
			return transient->MightValue; // return raw bonus
		}
		return 0.0f;
	}

	float Potion_GetSizeMultiplier(Actor* giant) {
		auto transient = Transient::GetActorData(giant);
		if (transient) {
			float bonus = std::clamp(transient->PotionMaxSize, 0.0f, 10.0f);
			return 1.0f + bonus;
		}
		return 1.0f;
	}

	void Potion_ModShrinkResistance(Actor* giant, float value) {
		auto transient = Transient::GetActorData(giant);
		if (transient) {
			transient->ShrinkResistance += value;
		}
	}

	void Potion_SetShrinkResistance(Actor* giant, float value) {
		auto transient = Transient::GetActorData(giant);
		if (transient) {
			transient->ShrinkResistance = value;
		}
	}

	float Potion_GetShrinkResistance(Actor* giant) {
		auto transient = Transient::GetActorData(giant);
		float Resistance = 1.0f;

		if (transient) {
			Resistance -= transient->ShrinkResistance;
		}
		return std::clamp(Resistance, 0.05f, 1.0f);
	}

	void Potion_SetUnderGrowth(Actor* actor, bool set) {
		auto transient = Transient::GetActorData(actor);
		if (transient) {
			transient->GrowthPotion = set;
		}
	}

	bool Potion_IsUnderGrowthPotion(Actor* actor) {
		bool UnderGrowth = false;
		auto transient = Transient::GetActorData(actor);
		if (transient) {
			UnderGrowth = transient->GrowthPotion;
		}
		return UnderGrowth;
	}

	float Ench_Aspect_GetPower(Actor* giant) {
		float aspect = SizeManager::GetSingleton().GetEnchantmentBonus(giant) * 0.01f;
		return aspect;
	}

	float Ench_Hunger_GetPower(Actor* giant) {
		float hunger = SizeManager::GetSingleton().GetSizeHungerBonus(giant) * 0.01f;
		return hunger;
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


	//----------------------------------------------------
	// PERKS
	//----------------------------------------------------

	float GetFallModifier(Actor* giant) {
		auto transient = Transient::GetActorData(giant);
		float fallmod = 1.0f;
		if (transient) {
			fallmod = transient->FallTimer;
			//log::info("Fall mult :{}", transient->FallTimer);
		}
		return fallmod;
	}

	//----------------------------------------------------
	// MAGIC
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

	bool DisallowSizeDamage(Actor* giant, Actor* tiny) {
		auto transient = Transient::GetActorData(giant);
		if (transient) {
			if (transient->Protection == false) {
				return false;
			} 

			bool Hostile = IsHostile(giant, tiny);
			return transient->Protection && !Hostile;
		}
		
		return false;
	}

	bool EffectsForEveryone(Actor* giant) { // determines if we want to apply size effects for literally every single actor
		if (giant->formID == 0x14) { // don't enable for Player
			return false;
		}
		bool dead = giant->IsDead();
		bool everyone = Config::General.bAllActorSizeEffects || CountAsGiantess(giant);
		if (!dead && everyone) {
			return true;
		}
		return false;
	}

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

	bool AllowStagger(Actor* giant, Actor* tiny) {
		const auto& Settings = Config::Balance;

		bool giantIsFriendly = (tiny->formID == 0x14 || IsTeammate(tiny));
		bool tinyIsFriendly = (tiny->formID == 0x14 || IsTeammate(tiny));

		//If Tiny is follower or player dont allow stagger
		if (tinyIsFriendly && giantIsFriendly) {
			return Settings.bAllowFriendlyStagger;
		}

		//If tiny is Other npc return settings option
		return Settings.bAllowOthersStagger;

	}

	void GainWeight(Actor* giant, float value) {

		if (Config::Gameplay.ActionSettings.bVoreWeightGain) {

			if (giant->formID == 0x14) {
				std::string_view name = "Vore_Weight";
				auto gianthandle = giant->CreateRefHandle();
				TaskManager::RunOnce(name, [=](auto& progressData) {
					if (!gianthandle) {
						return false;
					}
					auto giantref = gianthandle.get().get();
					float& original_weight = giantref->GetActorBase()->weight;
					if (original_weight >= 100.0f) {
						return false;
					} 
					if (original_weight + value >= 100.0f) {
						original_weight = 100.0f;
					} else {
						original_weight += value;
					}
					giantref->DoReset3D(true);
					return false;
				});
			}
		}
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
			//                                                                                  ^        ^           ^ - - - - Normal Crush
			//                                                       Chance to trigger bone crush   Damage of            Threshold multiplication
			//                                                                                      Bone Crush
		}
	}

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
		if (IsDebugEnabled() && (giant->formID == 0x14 || IsTeammate(giant))) {
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

	void SpringGrow(Actor* actor, float amt, float halfLife, std::string_view naming, bool drain) {
		if (!actor) {
			return;
		}

		auto growData = std::make_shared<SpringGrowData>(actor, amt, halfLife);
		std::string name = std::format("SpringGrow_{}_{}", naming, actor->formID);
		const float DURATION = halfLife * 3.2f;
		growData->drain = drain;

		TaskManager::RunFor(DURATION,
		                    [ growData ](const auto& progressData) {
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
					float max_scale = get_max_scale(actor);// * get_natural_scale(actor);
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

	void SpringShrink(Actor* actor, float amt, float halfLife, std::string_view naming) {
		if (!actor) {
			return;
		}

		auto growData = std::make_shared<SpringShrinkData>(actor, amt, halfLife);
		std::string name = std::format("SpringShrink_{}_{}", naming, actor->formID);
		const float DURATION = halfLife * 3.2f;
		TaskManager::RunFor(DURATION,
		                    [ growData ](const auto& progressData) {
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
	// OTHER
	//----------------------------------------------------

	float GetMovementModifier(Actor* giant) {
		float modifier = 1.0f;
		if (giant->AsActorState()->IsSprinting()) {
			modifier *= 1.33f;
		}
		if (giant->AsActorState()->IsWalking()) {
			modifier *= 0.75f;
		}
		if (giant->AsActorState()->IsSneaking()) {
			modifier *= 0.75f;
		}
		return modifier;
	}

	float GetRandomBoost() {
		float rng = (RandomFloat(0, 150));
		float random = rng / 100.f;
		return random;
	}

}