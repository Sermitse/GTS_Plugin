#pragma once

namespace GTS {

	static std::string GetAllyEssentialText(const bool Teammate);
	static const char* GetIconPath(bool Teammate);

	float TimeScale();
	float Shrink_GetPower(Actor* giant, Actor* tiny);
	float SizeSteal_GetPower(Actor* giant, Actor* tiny);
	float CalcEffeciency(Actor* caster, Actor* target);
	float CalcPower(Actor* actor, float scale_factor, float bonus, bool shrink);

	bool CanBendLifeless(Actor* giant);
	bool IsEssential_WithIcons(Actor* giant, Actor* tiny);
	bool Revert(Actor* actor, float scale_factor, float bonus);
	bool BlockShrinkToNothing(Actor* giant, Actor* tiny, float time_mult);
	bool ShrinkToNothing(Actor* caster, Actor* target, bool check_ticks, float time_mult, float mass_mult = 1.0f, bool Calamity_PlayLaugh = false, bool ShrinkOutburst_Absorb = false);

	void RecordPotionMagnitude(ActiveEffect* effect, float& power, float IfFalse);
	void AdvanceSkill(Actor* giant, ActorValue Attribute, float points, float multiplier);
	void Potion_Penalty(Actor* giant);
	void AdjustSizeReserve(Actor* giant, float value);
	void ModSizeExperience(Actor* Caster, float value);
	void ModSizeExperience_Crush(Actor* giant, Actor* tiny, bool check);
	void AdjustSizeLimit(float value, Actor* caster);
	void AdjustMassLimit(float value, Actor* caster);
	void Grow(Actor* actor, float scale_factor, float bonus);
	void ShrinkActor(Actor* actor, float scale_factor, float bonus);
	void Grow_Ally(Actor* from, Actor* to, float receiver, float caster);
	void Steal(Actor* from, Actor* to, float scale_factor, float bonus, float effeciency, ShrinkSource source);
	void TransferSize(Actor* caster, Actor* target, bool dual_casting, float power, float transfer_effeciency, bool smt, ShrinkSource source);
	void CrushBonuses(Actor* caster, Actor* target);
}
