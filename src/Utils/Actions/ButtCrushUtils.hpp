#pragma once

namespace GTS {

	bool CanDoButtCrush(Actor* actor, bool apply_cooldown);
	bool ButtCrush_IsAbleToGrow(Actor* actor, float limit);
	float GetButtCrushSize(Actor* giant);
	float GetGrowthCount(Actor* giant);
	float GetGrowthLimit(Actor* actor);
	float GetButtCrushDamage(Actor* actor);
	void ModGrowthCount(Actor* giant, float value, bool reset);
	void RecordStartButtCrushSize(Actor* giant);
	void SetButtCrushSize(Actor* giant, float value, bool reset);

}