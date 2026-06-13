#include "Utils/Actor/AV.hpp"
#include "Config/Config.hpp"

namespace GTS {

	float GetMaxAV(Actor* actor, ActorValue av) {

		if (actor) {
			if (ActorValueOwner* const avOwner = actor->AsActorValueOwner()) {
				const float baseValue = avOwner->GetBaseActorValue(av);
				const float permMod = actor->GetActorValueModifier(ACTOR_VALUE_MODIFIERS::kPermanent, av);
				const float tempMod = actor->GetActorValueModifier(ACTOR_VALUE_MODIFIERS::kTemporary, av);
				return baseValue + permMod + tempMod;
			}
		}

		return 0.f;
	}

	float GetAV(Actor* actor, ActorValue av) {
		// actor->GetActorValue(av); returns a cached value so we calc directly from mods, 
		// It also does not work on AE > .629 due to struct changes, will always ctd if called.
		
		if (actor) {
			const float max_av = GetMaxAV(actor, av);
			const float damageMod = actor->GetActorValueModifier(ACTOR_VALUE_MODIFIERS::kDamage, av);
			return max_av + damageMod;
		}

		return 0.0f;
	}

	void ModAV(Actor* actor, ActorValue av, float amount) {
		if (actor) {
			if (ActorValueOwner* const avOwner = actor->AsActorValueOwner()) {
				avOwner->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kTemporary, av, amount);
			}
		}
	}

	void SetAV(Actor* actor, ActorValue av, float amount) {
		if (actor) {
			const float currentValue = GetAV(actor, av);
			const float delta = amount - currentValue;
			ModAV(actor, av, delta);
		}
	}

	void DamageAV(Actor* actor, ActorValue av, float amount) {

		if (actor) {
			if (ActorValueOwner* const avOwner = actor->AsActorValueOwner()) {
				if (IsInGodMode(actor) && amount > 0) { // do nothing if TGM is on and value is > 0
					return;
				}
				if (!Config::Advanced.bDamageAV && actor->IsPlayerRef()) {
					return;
				}
				avOwner->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, av, -amount);
			}
		}
	}

	float GetPercentageAV(Actor* actor, ActorValue av) {
		return actor ? GetAV(actor, av) / GetMaxAV(actor, av) : 0.0f;
	}

	void SetPercentageAV(Actor* actor, ActorValue av, float target) {
		if (actor) {
			if (ActorValueOwner* const avOwner = actor->AsActorValueOwner()) {
				const float currentValue = GetAV(actor, av);
				const float maxValue = GetMaxAV(actor, av);
				const float targetValue = target * maxValue;
				const float delta = targetValue - currentValue;
				avOwner->RestoreActorValue(ACTOR_VALUE_MODIFIER::kDamage, av, delta);
			}
		}
	}

	float GetStaminaPercentage(Actor* actor) {
		return actor ? GetPercentageAV(actor, ActorValue::kStamina) : 0.f;
	}

	void SetStaminaPercentage(Actor* actor, float target) {
		if (actor) SetPercentageAV(actor, ActorValue::kStamina, target);
	}

	float GetHealthPercentage(Actor* actor) {
		return actor ? GetPercentageAV(actor, ActorValue::kHealth) : 0.0f;
	}

	void SetHealthPercentage(Actor* actor, float target) {
		if (actor) SetPercentageAV(actor, ActorValue::kHealth, target);
	}

	float GetMagikaPercentage(Actor* actor) {
		return actor ? GetPercentageAV(actor, ActorValue::kMagicka) : 0.f;
	}

	void SetMagickaPercentage(Actor* actor, float target) {
		if (actor) SetPercentageAV(actor, ActorValue::kMagicka, target);
	}

}
