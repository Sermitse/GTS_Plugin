#include "Managers/Animation/Sneak_KneeCrush.hpp"

#include "Config/Config.hpp"

#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/CrawlUtils.hpp"
#include "Managers/Damage/LaunchActor.hpp"
#include "Managers/Audio/Footstep.hpp"
#include "Managers/GTSSizeManager.hpp"
#include "Managers/Rumble.hpp"
#include "Utils/Actions/ButtCrushUtils.hpp"


using namespace GTS;

// Butt Crush alternative for sneaking
// Has Knee and Butt crush

namespace {

	constexpr std::string_view RNode = "NPC R Foot [Rft ]";
	constexpr std::string_view LNode = "NPC L Foot [Lft ]";

    void TrackKnee(Actor* giant, bool enable) {
		if (Config::General.bTrackBonesDuringAnim) {
			auto& sizemanager = SizeManager::GetSingleton();
			sizemanager.SetTrackedBone(giant, enable, CameraTracking::ObjectB);
		}
	}

    void TrackBooty(Actor* giant, bool enable) {
		if (Config::General.bTrackBonesDuringAnim) {
			auto& sizemanager = SizeManager::GetSingleton();
			sizemanager.SetTrackedBone(giant, enable, CameraTracking::Mid_Butt_Legs);
		}
	}

    void DoFootsteps(Actor* giant, float power, bool right) {
        float perk = GetPerkBonus_Basics(giant);
		float dust = 1.0f;
		float smt = 1.0f;

        if (TinyCalamityActive(giant)) {
			dust = 1.25f;
			smt = 2.0f;
		} 

		float shake_power = Rumble_KneeCrush_FootImpact * power;
		smt *= GetHighHeelsBonusDamage(giant, true);
        
        if (right) {
            Rumbling::Once("FST_R", giant, shake_power * smt, 0.0f, RNode, 0.0f);
            DoDamageEffect(giant, Damage_Walk_Defaut * power, Radius_Walk_Default, 10, 0.25f, FootEvent::Right, 1.0f, DamageSource::CrushedRight);
            DoFootstepSound(giant, 1.10f * power, FootEvent::Right, RNode);
            DoDustExplosion(giant, dust * power, FootEvent::Right, RNode);
            DoLaunch(giant, 0.65f * perk * power, 1.35f * power, FootEvent::Right);
        } else {
            Rumbling::Once("FST_L", giant, shake_power * smt, 0.0f, LNode, 0.0f);
            DoDamageEffect(giant, Damage_Walk_Defaut * power, Radius_Walk_Default, 10, 0.25f, FootEvent::Left, 1.0f, DamageSource::CrushedLeft);
            DoFootstepSound(giant, 1.10f * power, FootEvent::Left, LNode);
            DoDustExplosion(giant, dust * power, FootEvent::Left, LNode);
            DoLaunch(giant, 0.65f * perk * power, 1.35f * power, FootEvent::Left);
        }

		FootStepManager::PlayVanillaFootstepSounds(giant, right);
    }


    void DoButtDamage(Actor* giant) {
		float perk = GetPerkBonus_Basics(giant);
		float dust = 1.0f;

		if (TinyCalamityActive(giant)) {
			dust = 1.25f;
		}

		SetButtCrushSize(giant, 0.0f, true);

		float damage = GetButtCrushDamage(giant);
		auto ThighL = find_node(giant, "NPC L Thigh [LThg]");
		auto ThighR = find_node(giant, "NPC R Thigh [RThg]");
		auto ButtR = find_node(giant, "NPC R Butt");
		auto ButtL = find_node(giant, "NPC L Butt");
		if (ButtR && ButtL) {
			if (ThighL && ThighR) {
				DoDamageAtPoint(giant, Radius_ButtCrush_Impact, Damage_ButtCrush_ButtImpact * damage, ThighL, 4, 0.70f, 0.85f, DamageSource::Booty);
				DoDamageAtPoint(giant, Radius_ButtCrush_Impact, Damage_ButtCrush_ButtImpact * damage, ThighR, 4, 0.70f, 0.85f, DamageSource::Booty);
				DoDustExplosion(giant, 1.45f * dust * damage, FootEvent::Butt, "NPC R Butt");
				DoDustExplosion(giant, 1.45f * dust * damage, FootEvent::Butt, "NPC L Butt");
				DoFootstepSound(giant, 1.25f, FootEvent::Right, RNode);
				DoLaunch(giant, 1.30f * perk, 5.0f, FootEvent::Butt);
				Rumbling::Once("Butt_L", giant, 3.60f * damage, 0.05f, "NPC R Butt", 0.0f);
				Rumbling::Once("Butt_R", giant, 3.60f * damage, 0.05f, "NPC L Butt", 0.0f);
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
		float dust = 1.0f;

		if (TinyCalamityActive(giant)) {
			dust = 1.25f;
		}

		SetButtCrushSize(giant, 0.0f, true); // Actually reset scale

		float damage = GetButtCrushDamage(giant);
		auto LeftKnee = find_node(giant, "NPC L Calf [LClf]");
		auto RightKnee = find_node(giant, "NPC R Calf [RClf]");
		if (LeftKnee && RightKnee) {
            DoDamageAtPoint(giant, Radius_Sneak_KneeCrush, Damage_KneeCrush * damage, LeftKnee, 4, 0.70f, 0.85f, DamageSource::KneeLeft);
            DoDamageAtPoint(giant, Radius_Sneak_KneeCrush, Damage_KneeCrush * damage, RightKnee, 4, 0.70f, 0.85f, DamageSource::KneeRight);

            DoDustExplosion(giant, 1.45f * dust * damage, FootEvent::Left, "NPC L Calf [LClf]");
            DoDustExplosion(giant, 1.45f * dust * damage, FootEvent::Right, "NPC R Calf [RClf]");
            
            DoFootstepSound(giant, 1.25f, FootEvent::Left, "NPC L Calf [LClf]");
            DoFootstepSound(giant, 1.25f, FootEvent::Right, "NPC R Calf [RClf]");

            LaunchActor::LaunchAtNode(giant, 1.30f * perk, 4.20f, "NPC L Calf [LClf]");
            LaunchActor::LaunchAtNode(giant, 1.30f * perk, 4.20f, "NPC R Calf [RClf]");

            Rumbling::Once("Knee_L", giant, 3.60f * damage, 0.05f, "NPC L Calf [LClf]", 0.0f);
            Rumbling::Once("Knee_R", giant, 3.60f * damage, 0.05f, "NPC R Calf [RClf]", 0.0f);
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
    void GTS_SneakCrush_FootStepL(AnimationEventData& data) {DoFootsteps(&data.giant, 1.0f, false); data.HHspeed = 4.0f; data.disableHH = false;}
    void GTS_SneakCrush_FootStepR(AnimationEventData& data) {DoFootsteps(&data.giant, 1.0f, true); ApplyButtCrushCooldownTask(&data.giant);}

    void GTS_SneakCrush_FootStep_SilentL(AnimationEventData& data) {DoFootsteps(&data.giant, 1.0f, false); TrackKnee(&data.giant, true); SetButtCrushSize(&data.giant, 0.0f, false);}
    void GTS_SneakCrush_FootStep_SilentR(AnimationEventData& data) {DoFootsteps(&data.giant, 1.0f, true);}

	void GTS_DisableHH(AnimationEventData& data) {
		data.stage = 2; data.HHspeed = 2.25f; data.disableHH = true;
	
	}
	void GTS_EnableHH(AnimationEventData& data) {data.stage = 2; data.HHspeed = 1.0f; data.disableHH = false;}
}

namespace GTS
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