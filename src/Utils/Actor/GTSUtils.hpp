#pragma once

namespace GTS {

	//----------------------------------------------------
	// OTHER
	//----------------------------------------------------
	float GetMovementModifier(Actor* a_target);
	float GetRandomBoost();
	void StartActorResetTask(Actor* a_target);
	float GetFallModifier(Actor* a_target);
	bool AllowStagger(Actor* a_target);
	void GainWeight(Actor* a_target, float a_value);
	bool DisallowSizeDamage(Actor* a_source, Actor* a_target);
	bool EffectsForEveryone(Actor* a_target);

	//----------------------------------------------------
	// MAGIC
	//----------------------------------------------------
	void Potion_SetMightBonus(Actor* a_target, float a_value, bool a_Add);
	float Potion_GetMightBonus(Actor* a_target);
	float Potion_GetSizeMultiplier(Actor* a_target);
	void Potion_ModShrinkResistance(Actor* a_target, float a_value);
	void Potion_SetShrinkResistance(Actor* a_target, float a_value);
	float Potion_GetShrinkResistance(Actor* a_target);
	void Potion_SetUnderGrowth(Actor* a_target, bool a_set);
	bool Potion_IsUnderGrowthPotion(Actor* a_actor);
	float Ench_Aspect_GetPower(Actor* a_actor);
	float Ench_Hunger_GetPower(Actor* a_actor);

	//----------------------------------------------------
	// PERKS
	//----------------------------------------------------
	float Perk_GetSprintShrinkReduction(Actor* actor);
	float GetPerkBonus_OnTheEdge(Actor* giant, float amt);
	float Perk_GetCostReduction(Actor* giant);
	void DragonAbsorptionBonuses();

	//----------------------------------------------------
	// HIGH HEEL
	//----------------------------------------------------
	float GetHighHeelsBonusDamage(Actor* actor, bool multiply);
	float GetHighHeelsBonusDamage(Actor* actor, bool multiply, float adjust);
	bool BehaviorGraph_DisableHH(Actor* actor);

	//----------------------------------------------------
	// ATTRIBUTES
	//----------------------------------------------------
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
	void InflictSizeDamage(Actor* attacker, Actor* receiver, float value);

	//----------------------------------------------------
	// TINY CALAMITY
	//----------------------------------------------------
	void TinyCalamityExplosion(Actor* giant, float radius);
	void AddSMTDuration(Actor* actor, float duration, bool perk_check = true);
	void AddSMTPenalty(Actor* actor, float penalty);

	//----------------------------------------------------
	// IMMUNITY
	//----------------------------------------------------
	void Utils_ProtectTinies(bool Balance);
	void LaunchImmunityTask(Actor* giant, bool Balance);

	//----------------------------------------------------
	// SIZE RELATED
	//----------------------------------------------------
	void ShrinkOutburst_Shrink(Actor* giant, Actor* tiny, float shrink, float gigantism);
	void ShrinkUntil(Actor* giant, Actor* tiny, float expected, float halflife, bool animation);
	void SpringGrow(Actor* a_actor, float a_amt, float a_halfLife, std::string_view a_taskName, bool a_drain);
	void SpringShrink(Actor* a_actor, float a_amt, float a_halfLife, std::string_view a_taskName);
	
}
