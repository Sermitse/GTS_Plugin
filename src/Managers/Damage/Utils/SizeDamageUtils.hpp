#pragma once

// Module for accurate size-related damage

namespace GTS {
	bool IsIdleDamage(DamageSource cause);
	bool StrongGore(DamageSource cause);
	bool Allow_Damage(Actor* giant, Actor* tiny, DamageSource cause, float difference);
	void ModVulnerability(Actor* giant, Actor* tiny, float damage);
	bool DamageAllowed(Actor* giant, Actor* tiny, DamageSource Cause);
	bool ApplyHighHeelBonus(Actor* giant, DamageSource cause);
	float HighHeels_PerkDamage(Actor* giant, DamageSource Cause);
	void CrushCheck(Actor* giant, Actor* tiny, float size_difference, float crush_threshold, DamageSource Cause);
	float CalculateSizeDamage(Actor* giant, Actor* tiny, DamageSource Cause, float damage);
	float BalanceSizeDamage(float sizeDifference);
}
