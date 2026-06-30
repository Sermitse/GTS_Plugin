#include "Managers/Animation/Stomp_Under.hpp"

#include "Config/Config.hpp"

#include "Managers/AI/StompKick/StompKickSwipeAI.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/AnimationManager.hpp"
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
    NiPoint3 GetPresetFootPosition(Actor* giant, bool left_foot, float foot_offset) {
        float yaw = giant->data.angle.z;
        NiPoint3 center = giant->GetPosition();
        NiPoint3 forward (std::sin(yaw),std::cos(yaw),0.0f);
        NiPoint3 right (forward.y,-forward.x,0.0f);

        return left_foot ? center - right * foot_offset : center + right * foot_offset;
    }

    Actor* FindClosestTargetForFoot(Actor* giant, NiPoint3 footPos, float maxSearchDistance) {
        Actor* bestVictim = nullptr;

        const float maxDistSq = maxSearchDistance * maxSearchDistance;
        float bestScore = FLT_MAX;

        const float yaw = giant->data.angle.z;
        const NiPoint3 forward(std::sin(yaw), std::cos(yaw), 0.0f);

        constexpr float BackPenalty = 3.0f; // Prioritizes targets in front

        for (auto target : find_actors()) {
            if (!target || target == giant) {
                continue;
            }

            if (!IsHostile(giant, target) && IsTeammate(target) && Config::General.bProtectFollowers) {
                continue;
            }

            NiPoint3 delta = target->GetPosition() - footPos;
            delta.z = 0.0f;

            float distSq = delta.x * delta.x + delta.y * delta.y;

            if (distSq > maxDistSq) {
                continue;
            }

            // Back/Forward
            float localForward = delta.x * forward.x + delta.y * forward.y;

            float score = distSq;

            // If target is behind us - give it decreased priority
            if (localForward < 0.0f) {
                score += localForward * localForward * BackPenalty;
            }

            if (score < bestScore) {
                bestScore = score;
                bestVictim = target;
            }
        }

        return bestVictim;
    }

    void CalculateForwardBlend(Actor* giant, const NiPoint3& footPos, const NiPoint3& targetPos, float maxDistance, float& outBlend,float& outForwardDistance, float& outDistance) {
        float yaw = giant->data.angle.z;

        NiPoint3 offset = targetPos - footPos;
        NiPoint3 forward(std::sin(yaw),std::cos(yaw),0.0f);
        offset.z = 0.0f;
        
        outDistance = offset.Length();

        float forwardDistance = offset.x * forward.x + offset.y * forward.y;

        float blend = std::clamp(forwardDistance / maxDistance, 0.0f, 1.0f);
        outForwardDistance = forwardDistance;
        outBlend = blend;
    }
    void CalculateDirectionalBlend2D(Actor* giant, const NiPoint3& footPos,const NiPoint3& targetPos,float maxDistance,float& outX, float& outY, float& outDistanceX,float& outDistanceY, float& outDistance) {
        float yaw = giant->data.angle.z;

        NiPoint3 offset = targetPos - footPos;
        offset.z = 0.0f;

        float distance = offset.Length();
        outDistance = distance;

        if (distance <= 0.001f) {
            outX = 0.0f;
            outY = 0.0f;
            outDistanceX = 0.0f;
            outDistanceY = 0.0f;
            return;
        }

        // Нормализованное направление к цели
        NiPoint3 dir = offset;
        dir /= distance;

        // Локальные оси гиганта
        NiPoint3 forward( std::sin(yaw),std::cos(yaw),0.0f);

        NiPoint3 right(forward.y, -forward.x, 0.0f);

        // Угловая составляющая (-1..1)
        float angleForward = dir.x * forward.x + dir.y * forward.y;

        float angleRight = dir.x * right.x + dir.y * right.y;

        // Вес расстояния (0..1)
        float distanceWeight = std::clamp(distance / maxDistance, 0.0f, 1.0f);

        // Итоговый blend
        outX = angleForward * distanceWeight;
        outY = angleRight * distanceWeight;

        // Оставляем для логики триггеров
        outDistanceX = offset.x * forward.x + offset.y * forward.y;
        outDistanceY = offset.x * right.x + offset.y * right.y;
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
    bool AnimationUnderStomp::AutoAim_Foot_Directional(Actor* giant, bool left_foot, bool forward_only) {
        if (!giant) return false;
        if (giant->IsPlayerRef() && IsFreeCameraEnabled()) return false;

        const float max_distance = 36.0f * get_visual_scale(giant); //37.5f;
        const float foot_offset = 12.5f * get_visual_scale(giant); // Instead of looking for R/L foot, we do position offset from center to right/left, based on left_foot bool

        NiPoint3 footPos = GetPresetFootPosition(giant, left_foot, foot_offset);
        auto victim = FindClosestTargetForFoot(giant, footPos, max_distance);
        if (!victim) {
            AnimationVars::Stomp::SetUnderStompBlend_X(giant, RandomFloat(0.0f, 0.25f));
            AnimationVars::Stomp::SetUnderStompBlend_Y(giant, RandomFloat(0.0f, 0.15f));
            return false;
        }

        NiPoint3 victimPos = victim->GetPosition();

        //if (DebugDraw::CanDraw(giant, DebugDraw::DrawTarget::kAnyGTS)) {
            DebugDraw::DrawSphere(glm::vec3(footPos.x, footPos.y, footPos.z), 6.0f * get_visual_scale(giant), 600, {0.0f, 0.5f, 1.0f, 1.0f});
			DebugDraw::DrawSphere(glm::vec3(footPos.x, footPos.y, footPos.z), max_distance, 300, {1.0f, 0.5f, 0.0f, 1.0f});
		//}

        //if (DebugDraw::CanDraw(giant, DebugDraw::DrawTarget::kAnyGTS)) {
			DebugDraw::DrawSphere(glm::vec3(victimPos.x, victimPos.y, victimPos.z), 6.0f * get_visual_scale(giant), 300, {0.0f, 0.6f, 0.0f, 1.0f});
		//}

        footPos.z = 0.0f;
        victimPos.z = 0.0f;

        float x = 0.0f; // forward (>0) /   back (<1)
        float y = 0.0f; // right (>0)   /   left (<1)

        float dx = 0.0f;
        float dy = 0.0f;

        float final_distance = 0.0f;

        if (forward_only) {
            CalculateForwardBlend(giant, footPos, victimPos, max_distance, x, dx, final_distance);
        } else {
            CalculateDirectionalBlend2D(giant, footPos, victimPos, max_distance, x, y, dx, dy, final_distance);
        }

        x = std::clamp(x * 1.15f, -1.0f, 1.0f); // Slightly increase power of auto-aiming
        y = std::clamp(y * 1.15f, -1.0f, 1.0f); // Slightly increase power of auto-aiming

        logger::info(
            "Blend2D X:{}, Y:{} | Victim:{}",
            x, y,
            victim->GetDisplayFullName()
        );

    bool ShouldAutoAim = final_distance <= max_distance  && 
        dx >= -(max_distance * 0.1f); // Allow to auto-aim if enemy is a bit behind
    if (ShouldAutoAim) {
        AnimationVars::Stomp::SetUnderStompBlend_X(giant, x); // We added new behavior variables, needs new Behaviors in order to work
        AnimationVars::Stomp::SetUnderStompBlend_Y(giant, y); // We added new behavior variables, needs new Behaviors in order to work
    } else { // Reset vars just in case with a bit of rng
        AnimationVars::Stomp::SetUnderStompBlend_X(giant, RandomFloat(0.0f, 0.15f));
        AnimationVars::Stomp::SetUnderStompBlend_Y(giant, RandomFloat(0.0f, 0.15f));
    }
    return ShouldAutoAim;
}

    bool AnimationUnderStomp::ShouldPerformUnderStomp(Actor* giant, bool autoAim, bool left) {
        if (giant->IsPlayerRef() && IsFreeCameraEnabled()) {
            return false;
        }
        if (autoAim) {
            if (AutoAim_Foot_Directional(giant, left, false)) {
                return true;
            }
            if (!giant->IsPlayerRef()) { // If NPC didn't hit anyone, prevent blend below
                return false;
            }
        }
        return CrosshairUnderstomp(giant);
    }

    bool AnimationUnderStomp::CrosshairUnderstomp(Actor* giant) {
        //Range is between -1 (looking down) and 1 (looking up)
        //abs makes it become 1 -> 0 -> 1 for down -> middle -> up
        const float absPitch = abs(GetCameraRotation().entry[2][1]);
        //Remap our starting range
        constexpr float InvLookDownStartAngle = 0.9f; //Starting value of remap. Defines start angle for how down we are looking
        const float InvLookdownIntensity = std::clamp(Remap(absPitch, 1.0f, InvLookDownStartAngle, 0.0f, 1.0f), 0.0f, 1.0f);

        bool allow = absPitch > InvLookDownStartAngle;
        // Allow to stomp when looking from above or below
        if (allow) {
            float blend = std::clamp(InvLookdownIntensity * 1.3f, 0.0f, 1.0f);
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