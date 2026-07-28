#pragma once

namespace GTS {
    //-----------------------------------------------------
	// FOOT OR HAND AUTO AIM
	//-----------------------------------------------------
	enum class StompAimType {
		T1,
		T2,
		T3,
		T4,
	};
	bool AutoAim_Kick_DeterminePreferredKick(Actor* giant, bool& left);
	bool AutoAim_Butt_TryBreastSlam(Actor* giant, bool& left_hand);
	bool AutoAim_Butt_TryButtSlam(Actor* giant, bool& left_butt);
	bool AutoAim_Hand_TryHandAim_Far(Actor* giant, bool& left_hand, bool strong_Attack);
	bool AutoAim_Hand_TryHandAim(Actor* giant, bool& left_hand, bool strong_Attack); 
	bool AutoAim_Foot_Directional(Actor* giant, bool& left_foot, bool strong_Attack);
	bool AutoAim_Foot_Directional_FarStomp(Actor* giant, bool& left_foot, bool strong_stomp);
	bool AutoAim_IsSneakingOrCrawling(Actor* giant);
	bool AutoAim_Miss_GetNextStompSide(Actor* giant, StompAimType type);

	void SetStompBlendValues(Actor* giant, float x, float y);
}