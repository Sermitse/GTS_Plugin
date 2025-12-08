#include "Managers/Damage/SizeHitEffects.hpp"

#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/Grab.hpp"
#include "Managers/Animation/HugShrink.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"

#include "Managers/Rumble.hpp"
#include "Managers/CrushManager.hpp"
#include "Managers/GtsSizeManager.hpp"

#include "Magic/Effects/Common.hpp"

#include "Utils/DeathReport.hpp"
#include "Managers/Audio/MoansLaughs.hpp"

#include "Config/Config.hpp"

using namespace GTS;

namespace {

	void Prevent_Stagger(Actor* attacker, Actor* receiver) {
		float sizedifference = GetSizeDifference(receiver, attacker, SizeType::GiantessScale, true, false);
		receiver->SetGraphVariableFloat("GiantessScale", sizedifference); // Manages Stagger Resistance inside Behaviors.
		// Prevent stagger anims from playing on GTS, Behaviors read GiantessScale value and disallow stagger if value is > 1.5
	}

	float Hit_CalculateGrowth(float damage, float SizeHunger, float Gigantism) {
		float Growth = std::clamp((-damage/2000) * SizeHunger * Gigantism, 0.0f, 0.25f * Gigantism);
		Growth *= Config::GetGameplay().fHitGrowthPower;
		
		return Growth;
	}

	float Hit_CalculateShrink(Actor* attacker, float damage, float SizeHunger, float Gigantism, float resistance) {
		float sizebonus = std::clamp(get_visual_scale(attacker), 0.10f, 1.0f);
		float ShrinkValue = std::clamp(((-damage/600)/SizeHunger/Gigantism * sizebonus) * resistance, 0.0f, 0.25f / Gigantism); // affect limit by decreasing it

		return ShrinkValue;
	}

	bool Hit_ShouldGrow(Actor* receiver) {
		bool GrowthEnabled = Config::GetGameplay().bEnableGrowthOnHit;
		bool HasPerk = Runtime::HasPerkTeam(receiver, "GTSPerkHitGrowth");
		bool Teammate = (IsTeammate(receiver) || CountAsGiantess(receiver)) && IsFemale(receiver, true);
		bool IsPlayer = receiver->formID == 0x14;
		
		if (IsPlayer || Teammate) {
			if (IsHuman(receiver) && GrowthEnabled && HasPerk){
				return true;
			}
		}
		return false;
	}

	bool Hit_ShouldShrink(Actor* receiver) {
		
		const bool HasPerk = Runtime::HasPerk(receiver, "GTSPerkHitGrowth");
		const bool BalanceMode = SizeManager::BalancedMode();

		if (BalanceMode && receiver->formID == 0x14 && !HasPerk) {
			if (get_target_scale(receiver) > get_natural_scale(receiver, true)) {
				return true;
			}
		}
		return false;
	}

	void TinyAsShield(Actor* receiver, float a_damage) {
		auto grabbedActor = Grab::GetHeldActor(receiver);
		if (grabbedActor) {
			if (IsTeammate(grabbedActor)) {
				return; // Don't kill teammates
			}

			DamageAV(grabbedActor, ActorValue::kHealth, a_damage * 0.50f);
			if (grabbedActor->IsDead() || GetAV(grabbedActor, ActorValue::kHealth) < a_damage * 0.50f) {
				if (!IsBetweenBreasts(grabbedActor)) {
					ReportDeath(receiver, grabbedActor, DamageSource::BlockDamage);
				} else {
					ReportDeath(receiver, grabbedActor, DamageSource::Breast);
				}

				Grab::DetachActorTask(receiver);
				ModSizeExperience_Crush(receiver, grabbedActor, false);

				auto hand = find_node(receiver, "NPC L Hand [LHnd]");
				if (hand) {
					if (IsLiving(grabbedActor)) {
						SpawnParticle(receiver, 25.0f, "GTS/Damage/Explode.nif", hand->world.rotate, hand->world.translate, get_visual_scale(grabbedActor) * 5, 4, hand);
						SpawnParticle(receiver, 25.0f, "GTS/Damage/Crush.nif", hand->world.rotate, hand->world.translate, get_visual_scale(grabbedActor) * 5, 4, hand);
					} else {
						SpawnDustParticle(receiver, grabbedActor, "NPC L Hand [LHnd]", 3.0f);
					}
				}
				CrushManager::Crush(receiver, grabbedActor);

				if (!IsActionOnCooldown(receiver, CooldownSource::Emotion_Moan_Crush)) {
					ApplyActionCooldown(receiver, CooldownSource::Emotion_Moan_Crush);
					Sound_PlayLaughs(receiver, 1.0f, 0.14f, EmotionTriggerSource::Overkill);
					Task_FacialEmotionTask_Smile(receiver, 1.25f, "CrushSmile", RandomFloat(0.0f, 0.7f), 0.4f);
				}

				if (!LessGore()) {
					Runtime::PlaySoundAtNode("GTSSoundCrunchImpact", receiver, 1.0f, "NPC L Hand [LHnd]");
					Runtime::PlaySoundAtNode("GTSSoundCrunchImpact", receiver, 1.0f, "NPC L Hand [LHnd]");
					Runtime::PlaySoundAtNode("GTSSoundCrunchImpact", receiver, 1.0f, "NPC L Hand [LHnd]");
				} else {
					Runtime::PlaySoundAtNode("GTSSoundSoftHandAttack", receiver, 1.0f, "NPC L Hand [LHnd]");
				}
				Rumbling::Once("GrabAttackKill", receiver, 8.0f, 0.15f, "NPC L Hand [LHnd]", 0.0f);
				AnimationManager::StartAnim("GrabAbort", receiver); // Abort Grab animation
				Grab::Release(receiver);
			}
		}
	}
	

	void DropTinyChance(Actor* receiver, float damage, float scale) {
		static Timer DropTimer = Timer(0.33); // Check once per .33 sec
		float bonus = 1.0f;
		if (Runtime::HasPerkTeam(receiver, "GTSPerkHugsOfDeath")) {
			return; // Full immunity
		}
		if (Runtime::HasPerkTeam(receiver, "GTSPerkHugsGreed")) {
			bonus = 4.0f; // 4 times bigger damage threshold to cancel hugs
		}
		if (Runtime::HasPerkTeam(receiver, "GTSPerkHugsToughGrip")) {
			float GetHP = GetHealthPercentage(receiver);
			if (GetHP >= 0.85f) {
				return; // Drop only if hp is < 85%
			}
		}
		if (damage < 6.0f * bonus * scale) {
			return;
		}
		HugShrink::CallRelease(receiver); // Else release
	}

	void DoHitShake(Actor* receiver, float value) {
		if (IsFirstPerson()) {
			value *= 0.05f;
		}
		Rumbling::Once("HitGrowth", receiver, value, 0.15f);
	}

	void HitGrowth(Actor* receiver, Actor* attacker, float GrowthValue, float SizeDifference, float BalanceMode) {
		//log::info("Growth Value: {}", GrowthValue);

		int LaughChance = RandomInt(0, 12);
		int ShrinkChance = RandomInt(0, 5);

		float particlescale = 1.0f;
		float shrinkmult = 1.0f;

		static Timer soundtimer = Timer(1.5);

		float Adjustment = 1.0f * GetSizeFromBoundingBox(attacker);

		bool BlockParticle = IsActionOnCooldown(attacker, CooldownSource::Misc_BeingHit);
		bool LaughBlocked = IsActionOnCooldown(receiver, CooldownSource::Emotion_Laugh);

		DoHitShake(receiver, GrowthValue * 10);
		update_target_scale(receiver, GrowthValue, SizeEffectType::kNeutral);
		
		if (soundtimer.ShouldRunFrame()) {
			Runtime::PlaySoundAtNode("GTSSoundGrowth", receiver, GrowthValue * 2, "NPC Pelvis [Pelv]");
		}
		if (ShrinkChance >= 2) {
			if (get_target_scale(attacker) >= 0.06f/Adjustment) {
				if (SizeDifference >= 2.5f && LaughChance >= 5 && !LaughBlocked) {
					Task_FacialEmotionTask_Smile(receiver, 1.4f, "HitGrowthSmile", 0.25f);
					ApplyActionCooldown(receiver, CooldownSource::Emotion_Laugh);
					Sound_PlayLaughs(receiver, 1.0f, 0.14f, EmotionTriggerSource::Superiority);
					particlescale = 2.2f;
					shrinkmult = 6.0f;
				}
			
				update_target_scale(attacker, -GrowthValue/(3.0f * Adjustment*BalanceMode) * shrinkmult, SizeEffectType::kShrink); // Shrink Attacker
				update_target_scale(receiver, GrowthValue/(3.0f * Adjustment*BalanceMode), SizeEffectType::kGrow); // Grow receiver

				if (!BlockParticle) {
					SpawnCustomParticle(attacker, ParticleType::Red, NiPoint3(), "NPC Root [Root]", particlescale);
					ApplyActionCooldown(attacker, CooldownSource::Misc_BeingHit);
				}
			}
		}
	}

	void HitShrink(Actor* receiver, float ShrinkValue) {
		float scale = get_target_scale(receiver);
		float naturalscale = get_natural_scale(receiver, true);

		const float lossmod = Config::GetBalance().fBMShrinkOnHitMult;
		float modifier = std::clamp(lossmod, 0.10f, 25.0f); // * size loss value

		ShrinkValue *= modifier;

		log::info("Shrink Value: {}", -ShrinkValue);

		if (scale - ShrinkValue < naturalscale) {
			set_target_scale(receiver, naturalscale);
		} else {
			update_target_scale(receiver, -ShrinkValue, SizeEffectType::kShrink);
		}
	}

	void ApplyHitGrowth(Actor* attacker, Actor* receiver, float damage) {
		if (attacker == receiver) {
			return;
		}

		const float BalanceMode = SizeManager::BalancedMode() ? 2.0f : 1.0f;

		const float SizeHunger = 1.0f + Ench_Hunger_GetPower(receiver);
		const float Gigantism = 1.0f + Ench_Aspect_GetPower(receiver);
		
		const float SizeDifference = GetSizeDifference(receiver, attacker, SizeType::VisualScale, true, true);
		const float resistance = Potion_GetShrinkResistance(receiver);
	
		if (Hit_ShouldGrow(receiver)) { // if has perk. Wins over balance mode if true
			HitGrowth(receiver, attacker, Hit_CalculateGrowth(damage, SizeHunger, Gigantism), SizeDifference, BalanceMode);
		}
		else if (Hit_ShouldShrink(receiver)) { // else, if balance mode is on, Shrink us
			HitShrink(receiver, Hit_CalculateShrink(attacker, damage, SizeHunger, Gigantism, resistance));
		}
	}

	void ApplyToTinies(Actor* attacker, Actor* receiver, float damage) {
		float sizedifference = GetSizeDifference(receiver, attacker, SizeType::VisualScale, true, true);
		DropTinyChance(receiver, -damage, sizedifference);
		TinyAsShield(receiver, -damage);
	}
}


namespace GTS {

	SizeHitEffects& SizeHitEffects::GetSingleton() noexcept {
		static SizeHitEffects instance;
		return instance;
	}

	std::string SizeHitEffects::DebugName() {
		return "::SizeHitEffects";
	}

	void SizeHitEffects::ApplyEverything(Actor* attacker, Actor* receiver, float damage) {
		ApplyHitGrowth(attacker, receiver, damage);
		ApplyToTinies(attacker, receiver, damage);
		Prevent_Stagger(attacker, receiver);
	}

	void SizeHitEffects::PerformInjuryDebuff(Actor* giant, Actor* tiny, float damage, int random) { // Used as a debuff
		if (!tiny->IsDead() && Runtime::HasPerkTeam(giant, "GTSPerkRavagingInjuries")) {
			if (random > 0) {

				if (Runtime::HasPerkTeam(giant, "GTSPerkSprintDamageMult2") && giant->AsActorState()->IsSprinting() && !IsGtsBusy(giant)) {
					damage *= 3.0f;
					random = 1; // always apply
				}

				int rng = (RandomInt(0, random));
				if (rng <= 2) {
					float sizediff = GetSizeDifference(giant, tiny, SizeType::VisualScale, true, true);
					if (sizediff < 3.0f) {
						return;
					}

					if (!IsLiving(tiny)) {
						SpawnDustParticle(giant, tiny, "NPC Root [Root]", 1.0f);
					} else {
						auto root = find_node(tiny, "NPC Root [Root]");
						if (root) {
							SpawnParticle(tiny, 0.20f, "GTS/Damage/Explode.nif", root->world.rotate, root->world.translate, get_visual_scale(tiny), 7, root);
						}
					}
					if (damage > 0) {
						InflictSizeDamage(giant, tiny, damage * 1.5f);
					}
				}
			}
		}
	}
}
