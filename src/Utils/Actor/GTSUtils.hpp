#pragma once

namespace GTS {



	void Task_AdjustHalfLifeTask(Actor* tiny, float halflife, double revert_after);

	void StartResetTask(Actor* tiny);

	void Potion_SetMightBonus(Actor* giant, float value, bool add);

	float Potion_GetMightBonus(Actor* giant);

	float Potion_GetSizeMultiplier(Actor* giant);

	void Potion_ModShrinkResistance(Actor* giant, float value);

	void Potion_SetShrinkResistance(Actor* giant, float value);

	float Potion_GetShrinkResistance(Actor* giant);

	void Potion_SetUnderGrowth(Actor* actor, bool set);

	bool Potion_IsUnderGrowthPotion(Actor* actor);

	float Ench_Aspect_GetPower(Actor* giant);

	float Ench_Hunger_GetPower(Actor* giant);


	float Perk_GetSprintShrinkReduction(Actor* actor);

	float GetPerkBonus_OnTheEdge(Actor* giant, float amt);

	float Perk_GetCostReduction(Actor* giant);

	float GetFallModifier(Actor* giant);

	float GetHighHeelsBonusDamage(Actor* actor, bool multiply);

	float GetHighHeelsBonusDamage(Actor* actor, bool multiply, float adjust);

	bool DisallowSizeDamage(Actor* giant, Actor* tiny);

	bool EffectsForEveryone(Actor* giant);

	bool BehaviorGraph_DisableHH(Actor* actor);

	bool AllowStagger(Actor* giant, Actor* tiny);

	void GainWeight(Actor* giant, float value);

	float GetStolenAttributeCap(Actor* giant);

	void AddStolenAttributes(Actor* giant, float value);

	void AddStolenAttributesTowards(Actor* giant, ActorValue type, float value);

	float GetStolenAttributes_Values(Actor* giant, ActorValue type);

	float GetStolenAttributes(Actor* giant);

	void DistributeStolenAttributes(Actor* giant, float value);

	//----------------------------------------------------
	// DAMAGE
	//----------------------------------------------------

	void DoDamageEffect(Actor* giant, float damage, float radius, int random, float bonedamage, FootEvent kind, float crushmult, DamageSource Cause);

	void DoDamageEffect(Actor* giant, float damage, float radius, int random, float bonedamage, FootEvent kind, float crushmult, DamageSource Cause, bool ignore_rotation);

	void TinyCalamityExplosion(Actor* giant, float radius);

	void ShrinkOutburst_Shrink(Actor* giant, Actor* tiny, float shrink, float gigantism);

	void Utils_ProtectTinies(bool Balance);

	void LaunchImmunityTask(Actor* giant, bool Balance);

	void DragonAbsorptionBonuses();

	void AddSMTDuration(Actor* actor, float duration, bool perk_check = true);

	void AddSMTPenalty(Actor* actor, float penalty);

	void ShrinkUntil(Actor* giant, Actor* tiny, float expected, float halflife, bool animation);

	void SpringGrow(Actor* actor, float amt, float halfLife, std::string_view naming, bool drain);

	void SpringShrink(Actor* actor, float amt, float halfLife, std::string_view naming);

	void InflictSizeDamage(Actor* attacker, Actor* receiver, float value);

	float GetMovementModifier(Actor* giant);

	float GetRandomBoost();
	
}
