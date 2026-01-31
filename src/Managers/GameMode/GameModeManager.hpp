#pragma once
#include "Config/Settings/SettingsGameplay.hpp"

namespace GTS {

	enum class BalanceModeInfo {
		SizeGain_Penalty,
		ShrinkRate_Base,
		ShrinkRate_Combat,
	};

	class GameModeManager {
		public:
		static void ApplyGameMode(Actor* a_Actor, const LActiveGamemode_t& a_SelectedGameMode, const float& a_GrowthRate, const float& a_ShrinkRate);
		static void GameMode(Actor* actor);
	};
}
