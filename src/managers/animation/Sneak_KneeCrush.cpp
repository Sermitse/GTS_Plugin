#include "managers/animation/Controllers/ButtCrushController.hpp"
#include "managers/animation/Utils/AnimationUtils.hpp"
#include "managers/animation/Utils/CrawlUtils.hpp"
#include "managers/animation/AnimationManager.hpp"
#include "managers/damage/CollisionDamage.hpp"
#include "managers/animation/Sneak_KneeCrush.hpp"
#include "managers/damage/LaunchActor.hpp"
#include "managers/ai/aifunctions.hpp"
#include "managers/animation/Grab.hpp"
#include "managers/GtsSizeManager.hpp"
#include "managers/InputManager.hpp"
#include "managers/CrushManager.hpp"
#include "managers/explosion.hpp"
#include "managers/audio/footstep.hpp"
#include "managers/highheel.hpp"
#include "utils/actorUtils.hpp"
#include "data/persistent.hpp"
#include "managers/Rumble.hpp"
#include "managers/tremor.hpp"
#include "ActionSettings.hpp"
#include "data/runtime.hpp"
#include "scale/scale.hpp"
#include "data/time.hpp"
#include "node.hpp"

using namespace std;
using namespace SKSE;
using namespace RE;
using namespace Gts;

// Butt Crush alternative for sneaking
// Has Knee and Butt crush

namespace {

    const std::string_view RNode = "NPC R Foot [Rft ]";
	const std::string_view LNode = "NPC L Foot [Lft ]";

    void TrackKnee(Actor* giant, bool enable) {
		if (AllowCameraTracking()) {
			auto& sizemanager = SizeManager::GetSingleton();
			sizemanager.SetTrackedBone(giant, enable, CameraTracking::ObjectB);
		}
	}

    void TrackBooty(Actor* giant, bool enable) {
		if (AllowCameraTracking()) {
			auto& sizemanager = SizeManager::GetSingleton();
			sizemanager.SetTrackedBone(giant, enable, CameraTracking::Mid_Butt_Legs);
		}
	}

    void DoFootsteps(Actor* giant, float power, bool right) {
        float perk = GetPerkBonus_Basics(giant);
		float dust = 1.0;
		float smt = 1.0;

        if (HasSMT(giant)) {
			dust = 1.25;
			smt = 2.0;
		} 

		float shake_power = Rumble_KneeCrush_FootImpact * power;
		smt *= GetHighHeelsBonusDamage(giant, true);
        
        if (right) {
            Rumbling::Once("FST_R", giant, shake_power * smt, 0.0, RNode, 0.0);
            DoDamageEffect(giant, Damage_Walk_Defaut * power, Radius_Walk_Default, 10, 0.25, FootEvent::Right, 1.0, DamageSource::CrushedRight);
            DoFootstepSound(giant, 1.10 * power, FootEvent::Right, RNode);
            DoDustExplosion(giant, dust * power, FootEvent::Right, RNode);
            DoLaunch(giant, 0.65 * perk * power, 1.35 * power, FootEvent::Right);
        } else {
            Rumbling::Once("FST_L", giant, shake_power * smt, 0.0, LNode, 0.0);
            DoDamageEffect(giant, Damage_Walk_Defaut * power, Radius_Walk_Default, 10, 0.25, FootEvent::Left, 1.0, DamageSource::CrushedLeft);
            DoFootstepSound(giant, 1.10 * power, FootEvent::Left, LNode);
            DoDustExplosion(giant, dust * power, FootEvent::Left, LNode);
            DoLaunch(giant, 0.65 * perk * power, 1.35 * power, FootEvent::Left);
        }

		FootStepManager::PlayVanillaFootstepSounds(giant, right);
    }


    void DoButtDamage(Actor* giant) {
		float perk = GetPerkBonus_Basics(giant);
		float dust = 1.0;

		if (HasSMT(giant)) {
			dust = 1.25;
		}

		SetButtCrushSize(giant, 0.0, true);

		float damage = GetButtCrushDamage(giant);
		auto ThighL = find_node(giant, "NPC L Thigh [LThg]");
		auto ThighR = find_node(giant, "NPC R Thigh [RThg]");
		auto ButtR = find_node(giant, "NPC R Butt");
		auto ButtL = find_node(giant, "NPC L Butt");
		if (ButtR && ButtL) {
			if (ThighL && ThighR) {
				DoDamageAtPoint(giant, Radius_ButtCrush_Impact, Damage_ButtCrush_ButtImpact * damage, ThighL, 4, 0.70, 0.85, DamageSource::Booty);
				DoDamageAtPoint(giant, Radius_ButtCrush_Impact, Damage_ButtCrush_ButtImpact * damage, ThighR, 4, 0.70, 0.85, DamageSource::Booty);
				DoDustExplosion(giant, 1.45 * dust * damage, FootEvent::Butt, "NPC R Butt");
				DoDustExplosion(giant, 1.45 * dust * damage, FootEvent::Butt, "NPC L Butt");
				DoFootstepSound(giant, 1.25, FootEvent::Right, RNode);
				DoLaunch(giant, 1.30 * perk, 5.0, FootEvent::Butt);
				Rumbling::Once("Butt_L", giant, 3.60 * damage, 0.05, "NPC R Butt", 0.0);
				Rumbling::Once("Butt_R", giant, 3.60 * damage, 0.05, "NPC L Butt", 0.0);
			}
		} else {
			if (!ButtR) {
				Notify("Error: Missing Butt Nodes"); // Will help people to troubleshoot it. Not everyone has 3BB/XPMS32 body.
				Notify("Error: effects not inflicted");
				Notify("install 3BBB/XP32 Skeleton");
			}
			if (!ThighL) {
				Notify("Error: Missing Thigh Nodes");
				Notify("Error: effects not inflicted");
				Notify("install 3BBB/XP32 Skeleton");
			}
		}
		ModGrowthCount(giant, 0, true); // Reset limit
    }

    void DoKneeDamage(Actor* giant) {

		float perk = GetPerkBonus_Basics(giant);
		float dust = 1.0;

		if (HasSMT(giant)) {
			dust = 1.25;
		}

		SetButtCrushSize(giant, 0.0, true); // Actually reset scale

		float damage = GetButtCrushDamage(giant);
		auto LeftKnee = find_node(giant, "NPC L Calf [LClf]");
		auto RightKnee = find_node(giant, "NPC R Calf [RClf]");
		if (LeftKnee && RightKnee) {
            DoDamageAtPoint(giant, Radius_Sneak_KneeCrush, Damage_KneeCrush * damage, LeftKnee, 4, 0.70, 0.85, DamageSource::KneeLeft);
            DoDamageAtPoint(giant, Radius_Sneak_KneeCrush, Damage_KneeCrush * damage, RightKnee, 4, 0.70, 0.85, DamageSource::KneeRight);

            DoDustExplosion(giant, 1.45 * dust * damage, FootEvent::Left, "NPC L Calf [LClf]");
            DoDustExplosion(giant, 1.45 * dust * damage, FootEvent::Right, "NPC R Calf [RClf]");
            
            DoFootstepSound(giant, 1.25, FootEvent::Left, "NPC L Calf [LClf]");
            DoFootstepSound(giant, 1.25, FootEvent::Right, "NPC R Calf [RClf]");

            LaunchActor::GetSingleton().LaunchAtNode(giant, 1.30 * perk, 4.20, "NPC L Calf [LClf]");
            LaunchActor::GetSingleton().LaunchAtNode(giant, 1.30 * perk, 4.20, "NPC R Calf [RClf]");

            Rumbling::Once("Knee_L", giant, 3.60 * damage, 0.05, "NPC L Calf [LClf]", 0.0);
            Rumbling::Once("Knee_R", giant, 3.60 * damage, 0.05, "NPC R Calf [RClf]", 0.0);
		} else {
			if (!LeftKnee) {
				Notify("Error: Missing Knee Nodes"); // Will help people to troubleshoot it. Not everyone has 3BB/XP32 body.
				Notify("Error: effects not inflicted");
				Notify("install 3BBB/XP32 Skeleton");
			}
		}
		ModGrowthCount(giant, 0, true); // Reset limit
    }


    /////////////////////////////////////////////////////////////////////////////////////////
    ///                             E V E N T S
    ////////////////////////////////////////////////////////////////////////////////////////

    void GTS_SneakCrush_Knee_CamOn(AnimationEventData& data) {TrackKnee(&data.giant, true);} // Record scale to reset to later
    void GTS_SneakCrush_Knee_CamOff(AnimationEventData& data) {TrackKnee(&data.giant, false);}
    // Knee / Butt camera tracking
    void GTS_SneakCrush_Butt_CamOn(AnimationEventData& data) {TrackBooty(&data.giant, true); } 
    void GTS_SneakCrush_Butt_CamOff(AnimationEventData& data) {TrackBooty(&data.giant, false);}
    // Knee / Butt impacts
    void GTS_SneakCrush_Knee_FallDownImpact(AnimationEventData& data) {DoKneeDamage(&data.giant);}
    void GTS_SneakCrush_Butt_FallDownImpact(AnimationEventData& data) {DoButtDamage(&data.giant);}

    // footsteps 
    void GTS_SneakCrush_FootStepL(AnimationEventData& data) {DoFootsteps(&data.giant, 1.0, false); data.HHspeed = 4.0; data.disableHH = false;}
    void GTS_SneakCrush_FootStepR(AnimationEventData& data) {DoFootsteps(&data.giant, 1.0, true); ApplyButtCrushCooldownTask(&data.giant);}

    void GTS_SneakCrush_FootStep_SilentL(AnimationEventData& data) {DoFootsteps(&data.giant, 1.0, false); TrackKnee(&data.giant, true); SetButtCrushSize(&data.giant, 0.0, false);}
    void GTS_SneakCrush_FootStep_SilentR(AnimationEventData& data) {DoFootsteps(&data.giant, 1.0, true);}

	void GTS_DisableHH(AnimationEventData& data) {data.stage = 2; data.HHspeed = 2.25; data.disableHH = true;}
	void GTS_EnableHH(AnimationEventData& data) {data.stage = 2; data.HHspeed = 1.0; data.disableHH = false;}
}

namespace Gts
{
	void AnimationSneakCrush::RegisterEvents() {
		AnimationManager::RegisterEvent("GTS_SneakCrush_Knee_CamOn", "SneakCrush", GTS_SneakCrush_Knee_CamOn);
		AnimationManager::RegisterEvent("GTS_SneakCrush_Knee_CamOff", "SneakCrush", GTS_SneakCrush_Knee_CamOff);
		AnimationManager::RegisterEvent("GTS_SneakCrush_Butt_CamOn", "SneakCrush", GTS_SneakCrush_Butt_CamOn);
		AnimationManager::RegisterEvent("GTS_SneakCrush_Butt_CamOff", "SneakCrush", GTS_SneakCrush_Butt_CamOff);
		AnimationManager::RegisterEvent("GTS_SneakCrush_Knee_FallDownImpact", "SneakCrush", GTS_SneakCrush_Knee_FallDownImpact);
		AnimationManager::RegisterEvent("GTS_SneakCrush_Butt_FallDownImpact", "SneakCrush", GTS_SneakCrush_Butt_FallDownImpact);
		AnimationManager::RegisterEvent("GTS_SneakCrush_FootStep_SilentL", "SneakCrush", GTS_SneakCrush_FootStep_SilentL);
		AnimationManager::RegisterEvent("GTS_SneakCrush_FootStep_SilentR", "SneakCrush", GTS_SneakCrush_FootStep_SilentR);
		AnimationManager::RegisterEvent("GTS_SneakCrush_FootStepL", "SneakCrush", GTS_SneakCrush_FootStepL);
		AnimationManager::RegisterEvent("GTS_SneakCrush_FootStepR", "SneakCrush", GTS_SneakCrush_FootStepR);

		AnimationManager::RegisterEvent("GTS_DisableHH", "SneakCrush", GTS_DisableHH);
		AnimationManager::RegisterEvent("GTS_EnableHH", "SneakCrush", GTS_EnableHH);
	}
}