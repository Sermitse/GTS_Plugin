#include "Managers/Animation/Stomp_Under.hpp"

#include "Config/Config.hpp"

#include "Managers/AI/StompKick/StompKickSwipeAI.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Utils/Actor/AutoAimUtils.hpp"
#include "Managers/Cameras/CamUtil.hpp"
#include "Managers/Audio/Footstep.hpp"
#include "Managers/Audio/Stomps.hpp"
#include "Managers/Rumble.hpp"

using namespace GTS;

namespace {

	constexpr std::string_view RNode = "NPC R Foot [Rft ]";
	constexpr std::string_view LNode = "NPC L Foot [Lft ]";

    void DoImpactRumble(Actor* giant, std::string_view node, std::string_view name) {
		float shake_power = Rumble_Stomp_Under_Light;
		float smt = TinyCalamityActive(giant) ? 1.5f : 1.0f;
		smt *= GetHighHeelsBonusDamage(giant, true);
		Rumbling::Once(name, giant, shake_power * smt, 0.0f, node, 1.25f);
	}

    void UnderStomp_CheckForFootGrind(Actor* giant, bool right, FootActionType Type) {
        if (!AnimationVars::Crawl::IsCrawling(giant)) { // There's no anim for Crawling state
            const float PerformChancePlayer = Config::Gameplay.ActionSettings.fPlayerUnderstompGrindChance;
            const float PerformChanceNPC = Config::AI.Stomp.fUnderstompGrindProbability;
        	bool IsPlayer = giant->IsPlayerRef();

            if (RandomBool(IsPlayer ? PerformChancePlayer : PerformChanceNPC)) {
                FootGrindCheck(giant, Radius_Trample, right, Type);
            }
        }
    }

    void UnderStomp_DoEverything(Actor* giant, float animSpeed, bool right, FootEvent Event, DamageSource Source, std::string_view Node, std::string_view rumble) {
		float perk = GetPerkBonus_Basics(giant);
		float SMT = 1.0f;
		float damage = 1.0f;

        if (giant->IsSneaking()) {
            damage *= 0.75f;
        }

		if (TinyCalamityActive(giant)) {
			SMT = 1.75f; // Larger Dust
			damage *= 1.25f;
		}


        DoDamageEffect(giant, Damage_Stomp_Under_Light * damage * perk, Radius_Stomp_Strong, 8, 0.30f, Event, 1.0f, Source, false);
        DoImpactRumble(giant, Node, rumble);
        DoDustExplosion(giant, 1.0f * (SMT), Event, Node);

        DrainStamina(giant, "StaminaDrain_Stomp", Runtime::PERK.GTSPerkDestructionBasics, false, 1.4f);

        StompManager::PlayNewOrOldStomps(giant, SMT, Event, Node, false);

        LaunchTask(giant, 0.825f * perk, 2.10f, Event);

        FootStepManager::PlayVanillaFootstepSounds(giant, right);

        SetBusyFoot(giant, BusyFoot::None);
	}

    void GTS_UnderStomp_CamOnR(AnimationEventData& data) {
        DrainStamina(&data.giant, "StaminaDrain_Stomp", Runtime::PERK.GTSPerkDestructionBasics, true, 1.4f);
        ManageCamera(&data.giant, true, CameraTracking::R_Foot);
        SetBusyFoot(&data.giant, BusyFoot::RightFoot);
    }

    void GTS_UnderStomp_CamOnL(AnimationEventData& data) {
        DrainStamina(&data.giant, "StaminaDrain_Stomp", Runtime::PERK.GTSPerkDestructionBasics, true, 1.4f);
        ManageCamera(&data.giant, true, CameraTracking::L_Foot);
        SetBusyFoot(&data.giant, BusyFoot::LeftFoot);
    }

    void GTS_UnderStomp_CamOffR(AnimationEventData& data) {ManageCamera(&data.giant, false, CameraTracking::R_Foot);}
    void GTS_UnderStomp_CamOffL(AnimationEventData& data) {ManageCamera(&data.giant, false, CameraTracking::L_Foot);}

    void GTS_UnderStomp_ImpactR(AnimationEventData& data) {
        UnderStomp_DoEverything(&data.giant, data.animSpeed, true, FootEvent::Right, DamageSource::CrushedRight, RNode, "HeavyStompR");
        UnderStomp_CheckForFootGrind(&data.giant, true, FootActionType::Grind_UnderStomp);
    }

    void GTS_UnderStomp_ImpactL(AnimationEventData& data) {
        UnderStomp_DoEverything(&data.giant, data.animSpeed, false, FootEvent::Left, DamageSource::CrushedLeft, LNode, "HeavyStompL");
        UnderStomp_CheckForFootGrind(&data.giant, false, FootActionType::Grind_UnderStomp);
    }

}
namespace GTS {
    bool AnimationUnderStomp::PerformUnderstompOrAutoAim(Actor* giant, bool autoAim, bool& left) {
        if (giant->IsPlayerRef() && IsFreeCameraEnabled()) {
            return false;
        }
        if (autoAim) {
            if (AutoAim_IsSneakingOrCrawling(giant)) {
                bool hitTarget = false; // Don't do understomps if HIT the target
                if (AutoAim_Hand_TryHandAim(giant, left, hitTarget)) {
                    return !hitTarget; // Hand shouldn't count as Understomp, else actor uses foot to attack instead
                }
            } else if (AutoAim_Foot_Directional(giant, left, false)) {
                return true;
            }
            if (!giant->IsPlayerRef()) { // If NPC didn't hit anyone, prevent look-up-down based blend below
                return false;
            }
        }
        return CrosshairUnderstomp(giant);
    }

    bool AnimationUnderStomp::CrosshairUnderstomp(Actor* giant) { // Should be player exclusive
        //Range is between -1 (looking down) and 1 (looking up)
        //abs makes it become 1 -> 0 -> 1 for down -> middle -> up
        const float absPitch = abs(GetCameraRotation().entry[2][1]);
        //Remap our starting range
        constexpr float InvLookDownStartAngle = 0.9f; //Starting value of remap. Defines start angle for how down we are looking
        const float InvLookdownIntensity = std::clamp(Remap(absPitch, 1.0f, InvLookDownStartAngle, 0.0f, 1.0f), 0.0f, 1.0f);

        bool allow = absPitch > InvLookDownStartAngle;
        // Allow to stomp when looking from above or below
        if (allow) {
            float blend = std::clamp(InvLookdownIntensity * 1.2f, 0.0f, 1.0f);
            AnimationVars::Stomp::SetUnderStompBlend_Legacy(giant, blend);
            AnimationVars::Stomp::SetUnderStompBlend_X(giant, blend);
            AnimationVars::Stomp::SetUnderStompBlend_Y(giant, 0.0f);
            // Blend between "close" and "far" under-stomps
        }
        return allow;
    }

    void AnimationUnderStomp::RegisterEvents() {
        AnimationManager::RegisterEvent("GTS_UnderStomp_CamOnR", "UnderStomp", GTS_UnderStomp_CamOnR);
		AnimationManager::RegisterEvent("GTS_UnderStomp_CamOnL", "UnderStomp", GTS_UnderStomp_CamOnL);

        AnimationManager::RegisterEvent("GTS_UnderStomp_CamOffR", "UnderStomp", GTS_UnderStomp_CamOffR);
		AnimationManager::RegisterEvent("GTS_UnderStomp_CamOffL", "UnderStomp", GTS_UnderStomp_CamOffL);
        

		AnimationManager::RegisterEvent("GTS_UnderStomp_ImpactR", "UnderStomp", GTS_UnderStomp_ImpactR);
		AnimationManager::RegisterEvent("GTS_UnderStomp_ImpactL", "UnderStomp", GTS_UnderStomp_ImpactL);
	}

	void AnimationUnderStomp::RegisterTriggers() {
		AnimationManager::RegisterTrigger("UnderStompRight", "Stomp", "GTSBeh_UnderStomp_StartR");
		AnimationManager::RegisterTrigger("UnderStompLeft", "Stomp", "GTSBeh_UnderStomp_StartL");

        AnimationManager::RegisterTrigger("UnderGrindR", "Stomp", "GTSBEH_StartUnderGrindR");
        AnimationManager::RegisterTrigger("UnderGrindL", "Stomp", "GTSBEH_StartUnderGrindL");
	}
}