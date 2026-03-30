#include "Utils/Actor/SkillUtils.hpp"
#include "Config/Config.hpp"

namespace GTS {

	float GetGtsSkillLevel(Actor* giant) {

		if (giant->IsPlayerRef()) {
			auto GtsSkillLevel = Runtime::GetFloat(Runtime::GLOB.GTSSkillLevel);
			return GtsSkillLevel;
		}

		if (auto data = Persistent::GetActorData(giant)) {
			return data->fGTSSkillLevel;
		}
		//Fallback
		float Alteration = std::clamp(GetAV(giant, ActorValue::kAlteration), 1.0f, 100.0f);
		return Alteration;

	}

	void AddPerkPoints(float a_Level) {

		const auto _Level = static_cast<int>(std::round(a_Level));
		auto GtsSkillPerkPoints = Runtime::GetGlobal(Runtime::GLOB.GTSSkillPerkPoints);

		if (!GtsSkillPerkPoints) {
			return;
		}

		if (static_cast<int>(_Level) % 5 == 0) {
			Notify("You've earned a bonus perk point.");
			GtsSkillPerkPoints->value += 1.0f;
		}

		if (_Level == 20 || _Level == 40) {
			GtsSkillPerkPoints->value += 2.0f;
		}
		else if (_Level == 60 || _Level == 80) {
			GtsSkillPerkPoints->value += 3.0f;
		}
		else if (_Level == 100) {
			GtsSkillPerkPoints->value += 4.0f;
		}
	}

	float GetLegendaryLevel(Actor* giant) {
		if (giant->IsPlayerRef()) {
			auto LegendaryLevel = Runtime::GetFloat(Runtime::GLOB.GTSSkillLegendary);
			return LegendaryLevel;
		}
		return 0.0f;
	}

	float GetXpBonus(float a_Legendary) {
		const auto& GtsLegendaryLevel = Runtime::GetGlobal(Runtime::GLOB.GTSSkillLegendary);
		if (GtsLegendaryLevel) {
			float Legendary_Level = std::clamp(GtsLegendaryLevel->value, 0.0f, 3.0f);
			a_Legendary += Legendary_Level * 0.15f;
		}
		return Config::Balance.fExpMult * a_Legendary;
	}
}
