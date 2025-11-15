#include "Managers/Animation/Sneak_Slam_Strong.hpp"
#include "Managers/Animation/Sneak_Slam_FingerGrind.hpp"
#include "Managers/Animation/AnimationManager.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/CrawlUtils.hpp"

#include "Managers/Animation/Grab.hpp"

using namespace GTS;

namespace {
    void GTS_Sneak_SlamStrong_Raise_Arm_R(AnimationEventData& data) {
        Utils_UpdateHighHeelBlend(&data.giant, false);
        TrackMatchingHand(&data.giant, CrawlEvent::RightHand, true); // OFF is handled inside Sneak_Slam.cpp
		DrainStamina(&data.giant, "StaminaDrain_StrongSneakSlam", Runtime::PERK.GTSPerkDestructionBasics, true, 2.2f);
	} 
	void GTS_Sneak_SlamStrong_Raise_Arm_L(AnimationEventData& data) {
        Utils_UpdateHighHeelBlend(&data.giant, false);
        TrackMatchingHand(&data.giant, CrawlEvent::LeftHand, true); // OFF is handled inside Sneak_Slam.cpp
		DrainStamina(&data.giant, "StaminaDrain_StrongSneakSlam", Runtime::PERK.GTSPerkDestructionBasics, true, 2.2f);
	}

    void GTS_Sneak_SlamStrong_Lower_Arm_R(AnimationEventData& data) {}
	void GTS_Sneak_SlamStrong_Lower_Arm_L(AnimationEventData& data) {}

    void GTS_Sneak_SlamStrong_Impact_R(AnimationEventData& data) {
        float scale = get_visual_scale(&data.giant);
        DoCrawlingFunctions(&data.giant, scale, 2.75f, Damage_Sneak_HandSlam_Strong, CrawlEvent::RightHand, "RightHandRumble", 1.6f, Radius_Sneak_HandSlam_Strong, 1.0f, DamageSource::HandSlamRight);
        DrainStamina(&data.giant, "StaminaDrain_StrongSneakSlam", Runtime::PERK.GTSPerkDestructionBasics, false, 2.2f);
    }  
	void GTS_Sneak_SlamStrong_Impact_L(AnimationEventData& data) {
        float scale = get_visual_scale(&data.giant);
        DoCrawlingFunctions(&data.giant, scale, 2.75f, Damage_Sneak_HandSlam_Strong, CrawlEvent::LeftHand, "RightHandRumble", 1.6f, Radius_Sneak_HandSlam_Strong, 1.0f, DamageSource::HandSlamLeft);
        DrainStamina(&data.giant, "StaminaDrain_StrongSneakSlam", Runtime::PERK.GTSPerkDestructionBasics, false, 2.2f);
        Grab::DamageActorInHand(&data.giant, Damage_Sneak_HandSlam_Strong * 0.6f);
    } 

    void GTS_Sneak_SlamStrong_Impact_Secondary_R(AnimationEventData& data) {
        float scale = get_visual_scale(&data.giant) * 0.8f;
        DoCrawlingFunctions(&data.giant, scale, 0.85f, Damage_Sneak_HandSlam_Strong_Secondary, CrawlEvent::RightHand, "RightHandRumble", 0.8f, Radius_Sneak_HandSlam_Strong_Recover, 2.0f, DamageSource::HandSlamRight);
    }
    void GTS_Sneak_SlamStrong_Impact_Secondary_L(AnimationEventData& data) {
        float scale = get_visual_scale(&data.giant) * 0.8f;
        DoCrawlingFunctions(&data.giant, scale, 0.85f, Damage_Sneak_HandSlam_Strong_Secondary, CrawlEvent::LeftHand, "RightHandRumble", 0.8f, Radius_Sneak_HandSlam_Strong_Recover, 2.0f, DamageSource::HandSlamRight);
        Grab::DamageActorInHand(&data.giant, Damage_Sneak_HandSlam_Strong_Secondary * 0.6f);
    }
}

namespace GTS {
    
    void Animation_SneakSlam_Strong::RegisterEvents() {
        AnimationManager::RegisterEvent("GTS_Sneak_SlamStrong_Raise_Arm_R", "Sneak", GTS_Sneak_SlamStrong_Raise_Arm_R);
		AnimationManager::RegisterEvent("GTS_Sneak_SlamStrong_Raise_Arm_L", "Sneak", GTS_Sneak_SlamStrong_Raise_Arm_L);

        AnimationManager::RegisterEvent("GTS_Sneak_SlamStrong_Lower_Arm_R", "Sneak", GTS_Sneak_SlamStrong_Lower_Arm_R);
		AnimationManager::RegisterEvent("GTS_Sneak_SlamStrong_Lower_Arm_L", "Sneak", GTS_Sneak_SlamStrong_Lower_Arm_L);

        AnimationManager::RegisterEvent("GTS_Sneak_SlamStrong_Impact_R", "Sneak", GTS_Sneak_SlamStrong_Impact_R);
		AnimationManager::RegisterEvent("GTS_Sneak_SlamStrong_Impact_L", "Sneak", GTS_Sneak_SlamStrong_Impact_L);

        AnimationManager::RegisterEvent("GTS_Sneak_SlamStrong_Impact_Secondary_R", "Sneak", GTS_Sneak_SlamStrong_Impact_Secondary_R);
		AnimationManager::RegisterEvent("GTS_Sneak_SlamStrong_Impact_Secondary_L", "Sneak", GTS_Sneak_SlamStrong_Impact_Secondary_L);
    }
}