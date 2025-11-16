#pragma once

namespace GTS {

	float GetButtCrushSize(Actor* giant);
	float GetGrowthCount(Actor* giant);
	float GetGrowthLimit(Actor* actor);
	float GetButtCrushDamage(Actor* actor);
	void ModGrowthCount(Actor* giant, float value, bool reset);
	void RecordStartButtCrushSize(Actor* giant);
	void SetButtCrushSize(Actor* giant, float value, bool reset);
}