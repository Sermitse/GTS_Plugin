#include "Managers/Animation/Stomp_Under_Slam.hpp"
#include "Managers/Animation/AnimationManager.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/CrawlUtils.hpp"

using namespace GTS;

namespace {
    void GTS_UnderStomp_Crawl_CamOnR(AnimationEventData& data) {
		DrainStamina(&data.giant,"StaminaDrain_HandSlam", Runtime::PERK.GTSPerkDestructionBasics, true, 1.5f);
		ManageCamera(&data.giant, true, CameraTracking::Hand_Right);
	}
    void GTS_UnderStomp_Crawl_CamOnL(AnimationEventData& data) {
		DrainStamina(&data.giant,"StaminaDrain_HandSlam", Runtime::PERK.GTSPerkDestructionBasics, true, 1.5f);
		ManageCamera(&data.giant, true, CameraTracking::Hand_Left);
	}
    
    void GTS_UnderStomp_Crawl_ImpactR(AnimationEventData& data) {
        DoCrawlingFunctions(&data.giant, get_visual_scale(&data.giant), 1.05f, Damage_Sneak_HandSlam_Sneak, CrawlEvent::RightHand, "RightHandRumble", 0.9f, Radius_Sneak_HandSlam, 1.3f, DamageSource::HandSlamRight);
        DrainStamina(&data.giant,"StaminaDrain_HandSlam", Runtime::PERK.GTSPerkDestructionBasics, false, 1.5f);
    }
    void GTS_UnderStomp_Crawl_ImpactL(AnimationEventData& data) {
        DoCrawlingFunctions(&data.giant, get_visual_scale(&data.giant), 1.05f, Damage_Sneak_HandSlam_Sneak, CrawlEvent::LeftHand, "RightHandRumble", 0.9f, Radius_Sneak_HandSlam, 1.3f, DamageSource::HandSlamLeft);
        DrainStamina(&data.giant,"StaminaDrain_HandSlam", Runtime::PERK.GTSPerkDestructionBasics, false, 1.5f);
    }

    void GTS_UnderStomp_Crawl_CamOffR(AnimationEventData& data) {
		ManageCamera(&data.giant, false, CameraTracking::Hand_Right);
	}
    void GTS_UnderStomp_Crawl_CamOffL(AnimationEventData& data) {
		ManageCamera(&data.giant, false, CameraTracking::Hand_Left);
	}
}
namespace GTS {
    void AnimationUnderStompSlam::RegisterEvents() {
        AnimationManager::RegisterEvent("GTS_UnderStomp_Crawl_CamOnR", "UnderStompSlam", GTS_UnderStomp_Crawl_CamOnR);
        AnimationManager::RegisterEvent("GTS_UnderStomp_Crawl_CamOnL", "UnderStompSlam", GTS_UnderStomp_Crawl_CamOnL);

        AnimationManager::RegisterEvent("GTS_UnderStomp_Crawl_CamOffR", "UnderStompSlam", GTS_UnderStomp_Crawl_CamOffR);
        AnimationManager::RegisterEvent("GTS_UnderStomp_Crawl_CamOffL", "UnderStompSlam", GTS_UnderStomp_Crawl_CamOffL);
        
        AnimationManager::RegisterEvent("GTS_UnderStomp_Crawl_ImpactR", "UnderStompSlam", GTS_UnderStomp_Crawl_ImpactR);
        AnimationManager::RegisterEvent("GTS_UnderStomp_Crawl_ImpactL", "UnderStompSlam", GTS_UnderStomp_Crawl_ImpactL);
	}
}