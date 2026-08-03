#pragma once

namespace GTS {
    //-----------------------------------------------------
	// FOOT OR HAND AUTO AIM
	//-----------------------------------------------------
	bool AutoAim_Kick_DeterminePreferredKick(Actor* giant, bool& left, bool strong);
	bool AutoAim_Crawl_TryBreastSlam(Actor* giant, bool& left_hand, AimAssistResult* out_result = nullptr);
	bool AutoAim_Crawl_TryButtSlam(Actor* giant, bool& left_butt, AimAssistResult* out_result = nullptr);
	bool AutoAim_Hand_TryHandAim_Far(Actor* giant, bool& left_hand, bool strong_Attack, AimAssistResult* out_result = nullptr);
	bool AutoAim_Hand_TryHandAim(Actor* giant, bool& left_hand, bool strong_Attack, AimAssistResult* out_result = nullptr);
	bool AutoAim_Foot_Directional(Actor* giant, bool& left_foot, bool strong_Attack, AimAssistResult* out_result = nullptr);
	bool AutoAim_Foot_Directional_FarStomp(Actor* giant, bool& left_foot, bool strong_stomp, AimAssistResult* out_result = nullptr);
	bool AutoAim_IsSneakingOrCrawling(Actor* giant);
	bool AutoAim_Miss_GetNextStompSide(Actor* giant, StompAimType type);

	void SetStompBlendValues(Actor* giant, float x, float y);
}