#include "Utils/AV.hpp"
#include "Config/Config.hpp"

namespace GTS {

	float GetMaxAV(Actor* actor, ActorValue av) {
		auto baseValue = actor->AsActorValueOwner()->GetBaseActorValue(av);
		auto permMod = actor->GetActorValueModifier(ACTOR_VALUE_MODIFIERS::kPermanent, av);
		auto tempMod = actor->GetActorValueModifier(ACTOR_VALUE_MODIFIERS::kTemporary, av);
		return baseValue + permMod + tempMod;
	}
	float GetAV(Actor* actor, ActorValue av) {
		// actor->GetActorValue(av); returns a cached value so we calc directly from mods
		float max_av = GetMaxAV(actor, av);
		auto damageMod = actor->GetActorValueModifier(ACTOR_VALUE_MODIFIERS::kDamage, av);
		return max_av + damageMod;
	}
	void ModAV(Actor* actor, ActorValue av, float amount) {
		actor->AsActorValueOwner()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kTemporary, av, amount);
	}
	void SetAV(Actor* actor, ActorValue av, float amount) {
		float currentValue = GetAV(actor, av);
		float delta = amount - currentValue;
		ModAV(actor, av, delta);
	}

	void DamageAV(Actor* actor, ActorValue av, float amount) {
		if (IsInGodMode(actor) && amount > 0) { // do nothing if TGM is on and value is > 0
			return;
		}

		if (!Config::Advanced.bDamageAV && actor->formID == 0x14) {
			return;
		}

		actor->AsActorValueOwner()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, av, -amount);
	}

	float GetPercentageAV(Actor* actor, ActorValue av) {
		return GetAV(actor, av)/GetMaxAV(actor, av);
	}

	void SetPercentageAV(Actor* actor, ActorValue av, float target) {
		float currentValue = GetAV(actor, av);
		float maxValue = GetMaxAV(actor, av);
		float percentage = currentValue/maxValue;
		float targetValue = target * maxValue;
		float delta = targetValue - currentValue;
		actor->AsActorValueOwner()->RestoreActorValue(ACTOR_VALUE_MODIFIER::kDamage, av, delta);
	}

	float GetStaminaPercentage(Actor* actor) {
		return GetPercentageAV(actor, ActorValue::kStamina);
	}

	void SetStaminaPercentage(Actor* actor, float target) {
		SetPercentageAV(actor, ActorValue::kStamina, target);
	}

	float GetHealthPercentage(Actor* actor) {
		return GetPercentageAV(actor, ActorValue::kHealth);
	}

	void SetHealthPercentage(Actor* actor, float target) {
		GTS_PROFILE_SCOPE("AV: SetHealthPercentage");
		SetPercentageAV(actor, ActorValue::kHealth, target);
	}

	float GetMagikaPercentage(Actor* actor) {
		return GetPercentageAV(actor, ActorValue::kMagicka);
	}

	void SetMagickaPercentage(Actor* actor, float target) {
		SetPercentageAV(actor, ActorValue::kMagicka, target);
	}

}
