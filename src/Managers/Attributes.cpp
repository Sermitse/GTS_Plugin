#include "Managers/Attributes.hpp"

#include "Config/Config.hpp"

#include "Managers/Damage/TinyCalamity.hpp"
#include "Managers/GtsSizeManager.hpp"

using namespace REL;
using namespace GTS;

// TODO move away from polling
namespace {

	constexpr SoftPotential speed_adjustment_walk { .k = 0.265f,.n = 1.11f,.s = 2.0f,.o = 1.0f,.a = 0.0f,};
	constexpr SoftPotential MS_adjustment{ .k = 0.132f,.n = 0.86f, .s = 1.12f, .o = 1.0f,.a = 0.0f,};

	float GetMovementSpeedFormula_Alternative(Actor* actor, float smt_speed, float MovementDebuff) {
		// In short: 1.0 * (size * animation slowdown) * SMT run speed
		float gts_speed = get_giantess_scale(actor);
		float MS_mult = soft_core(gts_speed, MS_adjustment);
		
		float power = 1.0f * (gts_speed * (smt_speed/2.0f + 1.0f) * MS_mult);
		return power * MovementDebuff;
	}

	float GetMovementSpeedFormula_Normal(Actor* actor, float smt_speed, float MovementDebuff) {
		// In short: (1.0 / Animation slowdown) * SMT run speed
		float gts_speed = get_giantess_scale(actor);
		float MS_mult = soft_core(gts_speed, MS_adjustment);
		float MS_mult_limit = std::clamp(MS_mult, 0.750f, 1.0f);
		float Multy = std::clamp(MS_mult, 0.70f, 1.0f);
		float speed_mult_walk = soft_core(gts_speed, speed_adjustment_walk);
		float bonusspeed = std::clamp(speed_mult_walk, 0.90f, 1.0f);

		float power = 1.0f * MovementDebuff * (smt_speed/2.0f + 1.0f)/MS_mult/MS_mult_limit/Multy/bonusspeed;
		if (gts_speed > 1.0f) {
			return power;
		} else {
			return gts_speed * MovementDebuff * (smt_speed/2.0f + 1.0f);
		}
	}

	float GetMovementSlowdown(Actor* tiny) {
		auto transient = Transient::GetSingleton().GetData(tiny);
		if (transient) {
			return transient->MovementSlowdown;
		}
		return 1.0f;
	}

	void ManagePerkBonuses(Actor* actor) {

		auto& SizeManager = SizeManager::GetSingleton();
		float BalanceModeDiv = SizeManager::BalancedMode() ? 2.0f : 1.0f;
		float gigantism = 1.0f + (Ench_Aspect_GetPower(actor) * 0.30f);

		float BaseGlobalDamage = SizeManager::GetSizeAttribute(actor, SizeAttribute::Normal);
		float BaseSprintDamage = SizeManager::GetSizeAttribute(actor, SizeAttribute::Sprint);
		float BaseFallDamage = SizeManager::GetSizeAttribute(actor, SizeAttribute::JumpFall);

		float ExpectedGlobalDamage = 1.0f;
		float ExpectedSprintDamage = 1.0f;
		float ExpectedFallDamage = 1.0f;

		// -- Normal Damage
		if (Runtime::HasPerkTeam(actor, "GTSPerkCruelty")) {
			ExpectedGlobalDamage += 0.15f/BalanceModeDiv;
		}
		if (Runtime::HasPerkTeam(actor, "GTSPerkRealCruelty")) {
			ExpectedGlobalDamage += 0.35f/BalanceModeDiv;
		}
		if (IsGrowthSpurtActive(actor)) {
			ExpectedGlobalDamage *= (1.0f + (0.35f/BalanceModeDiv));
		}
		if (Runtime::HasPerkTeam(actor, "GTSPerkMightOfGiants")) {
			ExpectedGlobalDamage *= 1.15f; // +15% damage
		}

		// -- Sprint Damage
		if (Runtime::HasPerkTeam(actor, "GTSPerkSprintDamageMult1")) {
			ExpectedSprintDamage += 0.25f/BalanceModeDiv;
		}
		// -- Fall Damage
		if (Runtime::HasPerkTeam(actor, "GTSPerkCruelFall")) {
			ExpectedFallDamage += 0.3f/BalanceModeDiv;
		}
		// -- Buff by enchantment
		ExpectedGlobalDamage *= gigantism;
		ExpectedSprintDamage *= gigantism;
		ExpectedFallDamage *= gigantism;

		if (BaseGlobalDamage != ExpectedGlobalDamage) {
			SizeManager.SetSizeAttribute(actor, ExpectedGlobalDamage, SizeAttribute::Normal);
		}
		if (BaseSprintDamage != ExpectedSprintDamage) {
			SizeManager.SetSizeAttribute(actor, ExpectedSprintDamage, SizeAttribute::Sprint);
		}
		if (BaseFallDamage != ExpectedFallDamage) {
			SizeManager.SetSizeAttribute(actor, ExpectedFallDamage, SizeAttribute::JumpFall);
		}
	}

	void UpdateActors(Actor* actor) {
		if (actor) {
			ManagePerkBonuses(actor);
			if (actor->formID == 0x14) {
				TinyCalamity_BonusSpeed(actor); // Manages SMT bonuses
			}
		}
	}
}


namespace GTS {

	std::string AttributeManager::DebugName() {
		return "::AttributeManager";
	}

	void AttributeManager::Update() {

		static Timer timer = Timer(0.5);

		if (timer.ShouldRunFrame()) { // Run once per 0.5 sec
			for (auto actor: find_actors()) {
				if (actor) {
					if (actor->Is3DLoaded()) {
						UpdateActors(actor);
					}
				}
			}
		}
	}

	void AttributeManager::OverrideSMTBonus(float Value) {
		auto ActorAttributes = Persistent::GetSingleton().GetData(PlayerCharacter::GetSingleton());
		if (ActorAttributes) {
			ActorAttributes->smt_run_speed = Value;
		}
	}

	float AttributeManager::GetAttributeBonus(Actor* actor, ActorValue av) {

		GTS_PROFILE_SCOPE("AttributeManager: GetAttributeBonus");

		if (!actor) {
			return 1.0f;
		}

		float BalancedMode = SizeManager::BalancedMode() ? 2.0f : 1.0f;
		float natural_scale = get_natural_scale(actor, true);
		float scale = get_giantess_scale(actor);
		if (scale <= 0) {
			scale = 1.0f;
		} 
		if (scale < 1.0f) {
			scale /= natural_scale; 
			// Fix: negative bonuses when natural scale is < 1.0
			// No Fix: 0.91/1.0 = 0.91   (0.91 is just example of current size)
			// Fix:    0.91/0.91(natural size) = 1.0
			// Problem: DR resets after reaching 1.0. Not sure how to fix
		}
		switch (av) {

			case ActorValue::kHealth: {
				float might = 1.0f + Potion_GetMightBonus(actor);

				if (actor->formID == 0x14) {
					if (HasSMT(actor)) {
						scale += 1.0f;
					}
					if (actor->AsActorState()->IsSprinting() && Runtime::HasPerk(actor, "GTSPerkSprintDamageMult1")) {
						scale *= 1.30f;
					}
				}

				scale *= might;
				float resistance = std::clamp(1.0f / scale, 0.001f, 3.0f); // 0.001% as max resistance, -300% is a max vulnerability.
				return resistance;

			}

			case ActorValue::kCarryWeight: {
				const float BonusCarryMult = Config::Balance.fStatBonusCarryWeightMult;
				float power = (BonusCarryMult/BalancedMode);

				float might = 1.0f + Potion_GetMightBonus(actor);

				if (actor->formID == 0x14 && HasSMT(actor)) {
					scale += 3.0f;
				}
				if (scale > 1.0f) {
					return (power*scale*might) + 1.0f - power;
				} else {
					return 1.0f * might; // Don't reduce it if scale is < 1.0
				}
			}

			case ActorValue::kSpeedMult: {
				auto actorData = Persistent::GetSingleton().GetData(actor);
				float MovementDebuff = GetMovementSlowdown(actor);
				float smt_speed = 1.0f;

				if (actorData) {
					smt_speed = actorData->smt_run_speed;
				}
				bool AlternativeSpeed = Config::General.bAlternativeSpeedFormula;
				float MovementSpeed = AlternativeSpeed ? 
				GetMovementSpeedFormula_Alternative(actor, smt_speed, MovementDebuff)
				:
				GetMovementSpeedFormula_Normal(actor, smt_speed, MovementDebuff);

				return MovementSpeed;
			}

			case ActorValue::kAttackDamageMult: {
				if (actor->formID == 0x14 && HasSMT(actor)) {
					scale += 1.0f;
				}
				const float BonusDamageMult = Config::Balance.fStatBonusDamageMult;
				const float DamageStorage = 1.0f + ((BonusDamageMult) * (scale - 1.0f));

				float might = 1.0f + Potion_GetMightBonus(actor);

				if (scale > 1.0f) {
					return DamageStorage * might;
				} else {
					return scale * might;
				}
			}

			case ActorValue::kJumpingBonus: { // Used through MCM only (display bonus jump height)
				float power = 1.0f;
				float defaultjump = 1.0f + (1.0f * (scale - 1) * power);
				float might = 1.0f + Potion_GetMightBonus(actor);
				if (scale > 1.0f) {
					return defaultjump * might;
				} else {
					return scale * might;
				}
			}
			default: {
				return 1.0f;
			}
		}
	}

	float AttributeManager::AlterCarryWeightAV(Actor* actor, ActorValue av, float originalValue) {

		switch (av) {
			case ActorValue::kCarryWeight: {
				const float bonus = AttributeManager::GetAttributeBonus(actor, av);
				auto transient = Transient::GetSingleton().GetData(actor);
				if (transient != nullptr) {
					transient->CarryWeightBoost = (originalValue * bonus) - originalValue;
				}
				return originalValue * bonus;
			}
			default: return originalValue;

		}

	}

	float AttributeManager::AlterGetBaseAv(Actor* actor, ActorValue av, float originalValue) {

		switch (av) {

			case ActorValue::kHealth: { // 27.03.2024: Health boost is still applied, but for Player only and only if having matching perks

				float perkbonus = GetStolenAttributes_Values(actor, ActorValue::kHealth); // calc health from the perk bonuses
				float finalValue = originalValue + perkbonus; // add flat health on top
				auto transient = Transient::GetSingleton().GetData(actor);
				if (transient) {
					transient->HealthBoost = finalValue - originalValue;
				}
				return finalValue;
			}
			case ActorValue::kMagicka: {
				float perkbonus = GetStolenAttributes_Values(actor, ActorValue::kMagicka);
				return originalValue + perkbonus;
			}
			case ActorValue::kStamina: {
				float perkbonus = GetStolenAttributes_Values(actor, ActorValue::kStamina);
				return originalValue + perkbonus;
			}

			default: return originalValue;

		}
	}

	float AttributeManager::AlterSetBaseAv(Actor* actor, ActorValue av, float originalValue) {

		switch (av) {

			case ActorValue::kHealth: {
				auto transient = Transient::GetSingleton().GetData(actor);
				if (transient) {
					float lastEdit = transient->HealthBoost;
					if (originalValue - lastEdit > 0.0f) {
						originalValue -= lastEdit;
						return originalValue;
					}
				}
			}

			default: return originalValue;

		}


	}

	float AttributeManager::AlterMovementSpeed(Actor* actor) {
		if (actor) {
			return AttributeManager::GetAttributeBonus(actor, ActorValue::kSpeedMult);
		}
		return 1.0f;
	}
}
