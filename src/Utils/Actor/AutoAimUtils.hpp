#pragma once

namespace GTS {
    //-----------------------------------------------------
	// FOOT OR HAND AUTO AIM
	//-----------------------------------------------------
	NiPoint3 GetPresetAimPosition(Actor* giant, bool left_foot, float side_offset, float forward_offset);
	Actor* FindClosestTargetBetweenTwoPoints(Actor* giant, const NiPoint3 pointL, const NiPoint3 pointR, float maxSearchDistance, bool& leftFoot);
	void CalculateForwardBlend(Actor* giant, const NiPoint3& footPos, const NiPoint3& targetPos, float maxDistance, float& outBlend,float& outForwardDistance, float& outDistance);
	void CalculateDirectionalBlend2D(Actor* giant, const NiPoint3& footPos,const NiPoint3& targetPos,float maxDistance,float& outX, float& outY, float& outDistanceX,float& outDistanceY, float& outDistance);
	bool AutoAim_Kick_DeterminePreferredKick(Actor* giant);
	bool AutoAim_Hand_TryHandAim(Actor* giant, bool& left_hand, bool &hitTarget); 
	bool AutoAim_Foot_Directional(Actor* giant, bool& left_foot, bool forward_only);
	bool AutoAim_IsSneakingOrCrawling(Actor* giant);
	bool AutoAim_SetUpDefaultSide(Actor* giant);
}