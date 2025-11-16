#pragma once

namespace GTS {

	float GetMaxAV(Actor* actor, ActorValue av);
	float GetAV(Actor* actor, ActorValue av);
	void ModAV(Actor* actor, ActorValue av, float amount);
	void SetAV(Actor* actor, ActorValue av, float amount);

	void DamageAV(Actor* actor, ActorValue av, float amount);

	float GetPercentageAV(Actor* actor, ActorValue av);

	void SetPercentageAV(Actor* actor, ActorValue av, float target);

	float GetStaminaPercentage(Actor* actor);

	void SetStaminaPercentage(Actor* actor, float target);

	float GetHealthPercentage(Actor* actor);

	void SetHealthPercentage(Actor* actor, float target);

	float GetMagikaPercentage(Actor* actor);

	void SetMagickaPercentage(Actor* actor, float target);

}
