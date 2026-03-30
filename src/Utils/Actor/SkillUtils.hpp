#pragma once

namespace GTS {

	float GetGtsSkillLevel(Actor* giant);
	void AddPerkPoints(float a_Level);
	float GetLegendaryLevel(Actor* giant);
	float GetXpBonus(float a_Legendary = 1.0f);

}