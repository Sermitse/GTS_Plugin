#pragma once

namespace GTS {
	constexpr glm::vec4 Breast_Color = {0.961f, 0.157f, 0.569f, 0.8f};
    constexpr glm::vec4 Close_Stomp_Color = {1.0f, 0.0f, 0.0f, 1.0f};
    constexpr glm::vec4 Far_Stomp_Color = {0.5f, 0.5f, 0.0f, 1.0f};
    constexpr glm::vec4 Origin_Point = {0.07f, 0.4f, 0.54f, 1.0f};
    constexpr glm::vec4 Kick_Color = {0.0f, 0.5f, 0.5f, 1.0f};
    //-----------------------------------------------------
	// FOOT OR HAND AUTO AIM
	//-----------------------------------------------------
	bool AutoAim_Kick_DeterminePreferredKick(Actor* giant, bool& left, bool strong);
	bool AutoAim_Crawl_TryBreastSlam(Actor* giant, AimAssistResult* out_result = nullptr);
	bool AutoAim_Sneak_TryButtSlam(Actor* giant, bool& left_butt, AimAssistResult* out_result = nullptr);
	bool AutoAim_Hand_TryHandAim_Far(Actor* giant, bool& left_hand, bool strong_Attack, AimAssistResult* out_result = nullptr);
	bool AutoAim_Hand_TryHandAim_Close(Actor* giant, bool& left_hand, bool strong_Attack, AimAssistResult* out_result = nullptr);
	bool AutoAim_Foot_Directional(Actor* giant, bool& left_foot, bool strong_Attack, AimAssistResult* out_result = nullptr);
	bool AutoAim_Foot_Directional_FarStomp(Actor* giant, bool& left_foot, bool strong_stomp, AimAssistResult* out_result = nullptr);
	bool AutoAim_IsSneakingOrCrawling(Actor* giant);
	bool AutoAim_Miss_GetNextStompSide(Actor* giant, StompAimType type);

	void SetStompBlendValues(Actor* giant, float x, float y, std::string_view source = "Default", bool print = true);
}