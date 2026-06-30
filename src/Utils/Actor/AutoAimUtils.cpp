#include "Utils/Actor/AutoAimUtils.hpp"
#include "Utils/Actor/FindActor.hpp"
#include "Magic/Effects/Common.hpp"
#include "Config/Config.hpp"

namespace {
    using namespace GTS;

	constexpr float autoAim_Range_Stomp = 37.5f;
	constexpr float autoAim_Range_Hand = 15.0f;
    constexpr float autoAim_Range_Kick = 36.0f;
	constexpr float autoAim_FootOffsetDistance = 10.0f;
	constexpr float autoAim_Hand_OffsetDistance_Side = 14.5f;
	constexpr float autoAim_Hand_OffsetDistance_Forward = 50.0f;

    constexpr float autoAim_Hand_OffsetDistance_Forward_Sneak = 35.0f;
    constexpr float autoAim_Kick_OffsetDistance_Forward = 20.0f;
	constexpr float autoAim_BackPenalty = 3.0f;        
	constexpr float autoAim_IgnoreBehindAfter = 0.1f;

    void DrawDebugSpheres(Actor* giant, NiPoint3 pointPos, NiPoint3 victimPos, float max_distance) {
        //if (DebugDraw::CanDraw(giant, DebugDraw::DrawTarget::kAnyGTS)) {
            DebugDraw::DrawSphere(glm::vec3(pointPos.x, pointPos.y, pointPos.z), 6.0f * get_visual_scale(giant), 600, {0.0f, 0.5f, 1.0f, 1.0f});
            DebugDraw::DrawSphere(glm::vec3(pointPos.x, pointPos.y, pointPos.z), max_distance, 300, {1.0f, 0.5f, 0.0f, 1.0f});

            DebugDraw::DrawSphere(glm::vec3(victimPos.x, victimPos.y, victimPos.z), 6.0f * get_visual_scale(giant), 300, {0.0f, 0.6f, 0.0f, 1.0f});
        //}
    }
}

namespace GTS {
        NiPoint3 GetPresetAimPosition(Actor* giant, bool left_foot, float side_offset, float forward_offset) {
            float yaw = giant->data.angle.z;

            NiPoint3 center = giant->GetPosition();

            NiPoint3 forward(std::sin(yaw), std::cos(yaw), 0.0f);
            NiPoint3 right(forward.y, -forward.x, 0.0f);

            NiPoint3 footPos = center;

            // Right/Left
            footPos += (left_foot ? -right : right) * side_offset;

            // Back/Forward
            footPos += forward * forward_offset;

            return footPos;
        }
        Actor* FindClosestTargetBetweenTwoPoints(Actor* giant, const NiPoint3 pointL, const NiPoint3 pointR, float maxSearchDistance, bool& leftFoot) {
            Actor* bestVictim = nullptr;

            const float maxDistSq = maxSearchDistance * maxSearchDistance;
            float bestScore = FLT_MAX;

            const float yaw = giant->data.angle.z;

            const NiPoint3 center = giant->GetPosition();
            const NiPoint3 forward(std::sin(yaw), std::cos(yaw), 0.0f);

            for (auto target : find_actors()) {
                if (!target || target == giant) {
                    continue;
                }

                if (!IsHostile(giant, target) &&
                    IsTeammate(target) &&
                    Config::General.bProtectFollowers) {
                    continue;
                }

                NiPoint3 targetPos = target->GetPosition();
                targetPos.z = 0.0f;

                // Distance to left point
                NiPoint3 deltaL = targetPos - pointL;
                deltaL.z = 0.0f;
                float distSqL = deltaL.x * deltaL.x + deltaL.y * deltaL.y;

                // Distance to right point
                NiPoint3 deltaR = targetPos - pointR;
                deltaR.z = 0.0f;
                float distSqR = deltaR.x * deltaR.x + deltaR.y * deltaR.y;

                bool useLeft = distSqL <= distSqR;
                float distSq = useLeft ? distSqL : distSqR;

                if (distSq > maxDistSq) {
                    continue;
                }

                //
                // IMPORTANT:
                // Front/back priority is calculated relative to the actor,
                // not relative to the hand/foot search point.
                //
                NiPoint3 centerDelta = targetPos - center;
                centerDelta.z = 0.0f;

                float localForward =
                    centerDelta.x * forward.x +
                    centerDelta.y * forward.y;

                float score = distSq;

                // Penalize targets behind the actor
                if (localForward < 0.0f) {
                    score += localForward * localForward * autoAim_BackPenalty;
                }

                if (score < bestScore) {
                    bestScore = score;
                    bestVictim = target;
                    leftFoot = useLeft;
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
            // 
            NiPoint3 dir = offset;
            dir /= distance;
            // 
            NiPoint3 forward( std::sin(yaw),std::cos(yaw),0.0f);
            NiPoint3 right(forward.y, -forward.x, 0.0f);
            // 
            float angleForward = dir.x * forward.x + dir.y * forward.y;
            float angleRight = dir.x * right.x + dir.y * right.y;
            // 
            float distanceWeight = std::clamp(distance / maxDistance, 0.0f, 1.0f);
            // 
            outX = angleForward * distanceWeight;
            outY = angleRight * distanceWeight;
            // 
            outDistanceX = offset.x * forward.x + offset.y * forward.y;
            outDistanceY = offset.x * right.x + offset.y * right.y;
        }

        bool AutoAim_Kick_DeterminePreferredKick(Actor* giant) {
            if (!giant) return RandomBool();
            bool left = AutoAim_SetUpDefaultSide(giant);
            float foot_offset_side = autoAim_FootOffsetDistance * get_visual_scale(giant);
            float foot_offset_forward = autoAim_Kick_OffsetDistance_Forward * get_visual_scale(giant);
            float max_distance = autoAim_Range_Kick * get_visual_scale(giant);

            if (AutoAim_IsSneakingOrCrawling(giant)) {
                logger::info("Applying sneak attacks");
                foot_offset_side = autoAim_Hand_OffsetDistance_Side * get_visual_scale(giant);
                foot_offset_forward = autoAim_Hand_OffsetDistance_Forward_Sneak * get_visual_scale(giant);
                max_distance *= 1.5f;
            }

            NiPoint3 footPos_L = GetPresetAimPosition(giant, true, foot_offset_side, foot_offset_forward);
            NiPoint3 footPos_R = GetPresetAimPosition(giant, false, foot_offset_side, foot_offset_forward);

            auto victim = FindClosestTargetBetweenTwoPoints(giant, footPos_L, footPos_R, max_distance, left);
            if (!victim) {
                logger::info("No target found");
                return left;
            }
            NiPoint3 victimPos = victim->GetPosition();
            NiPoint3 footPos = left ? footPos_L : footPos_R;

            DrawDebugSpheres(giant, footPos, victimPos, max_distance);

            footPos.z = 0.0f;
            victimPos.z = 0.0f;
            float x = 0.0f;
            float dx = 0.0f;
            float final_distance = 0.0f;
            CalculateForwardBlend(giant, footPos, victimPos, max_distance, x, dx, final_distance);
            if (final_distance >= max_distance) {
                return AutoAim_SetUpDefaultSide(giant);
            }

            return left;
        }

        bool AutoAim_Hand_TryHandAim(Actor* giant, bool& left_hand, bool &hitTarget) {
            if (!giant) return false;
            if (giant->IsPlayerRef() && IsFreeCameraEnabled()) return false;

            if (AutoAim_Foot_Directional(giant, left_hand, false)) {
                return true; // Always prioritize under-stomps
            }

            const float max_distance = autoAim_Range_Hand * get_visual_scale(giant);
            const float hand_offset_side = autoAim_Hand_OffsetDistance_Side * get_visual_scale(giant);
            const float hand_offset_forward = autoAim_Hand_OffsetDistance_Forward * get_visual_scale(giant);

            NiPoint3 handPos_L = GetPresetAimPosition(giant, true, hand_offset_side, hand_offset_forward);
            NiPoint3 handPos_R = GetPresetAimPosition(giant, false, hand_offset_side, hand_offset_forward);
            auto victim = FindClosestTargetBetweenTwoPoints(giant, handPos_L, handPos_R, max_distance, left_hand); // Overrides left_hand bool

            NiPoint3 handPos = left_hand ? handPos_L : handPos_R; // Pick which hand should be used

            if (!victim) {
                // If no victims, just hand slam randomly
                return false; 
            }
            NiPoint3 victimPos = victim->GetPosition();

            DrawDebugSpheres(giant, handPos, victimPos, max_distance);

            handPos.z = 0.0f;
            victimPos.z = 0.0f;
            float x = 0.0f;
            float dx = 0.0f;
            float final_distance = 0.0f;
            CalculateForwardBlend(giant, handPos, victimPos, max_distance, x, dx, final_distance);
            hitTarget = final_distance <= max_distance;
            return final_distance <= max_distance;
        }

        bool AutoAim_Foot_Directional(Actor* giant, bool& left_foot, bool forward_only) {
            if (!giant) return false;
            if (giant->IsPlayerRef() && IsFreeCameraEnabled()) return false;

            const float max_distance = autoAim_Range_Stomp * get_visual_scale(giant); //37.5f;
            const float foot_offset = autoAim_FootOffsetDistance * get_visual_scale(giant); // Instead of looking for R/L foot, we do position offset from center to right/left, based on left_foot bool

            NiPoint3 footPos_L = GetPresetAimPosition(giant, true, foot_offset, 0.0f);
            NiPoint3 footPos_R = GetPresetAimPosition(giant, false, foot_offset, 0.0f);
            auto victim = FindClosestTargetBetweenTwoPoints(giant, footPos_L, footPos_R, max_distance, left_foot); // Overrides left_foot bool
            if (!victim) {
                AnimationVars::Stomp::SetUnderStompBlend_X(giant, RandomFloat(0.0f, 0.25f));
                AnimationVars::Stomp::SetUnderStompBlend_Y(giant, RandomFloat(0.0f, 0.15f));
                left_foot = !left_foot; // Flip it
                return false;
            }

            NiPoint3 victimPos = victim->GetPosition();
            NiPoint3 footPos = left_foot ? footPos_L : footPos_R; // Pick which foot should be used

            DrawDebugSpheres(giant, footPos, victimPos, max_distance);
            
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

            x = std::clamp(x * 1.25f, -1.0f, 1.0f); // Slightly increase power of auto-aiming
            y = std::clamp(y * 1.25f, -1.0f, 1.0f); // Slightly increase power of auto-aiming

            logger::info("Blend2D X:{}, Y:{} | Victim:{}",x, y,victim->GetDisplayFullName());
            Cprint("Blend2D X:{}, Y:{} | Victim:{}",x, y,victim->GetDisplayFullName());

            bool ShouldAutoAim = final_distance <= max_distance  && 
                dx >= -(max_distance * autoAim_IgnoreBehindAfter); // Allow to auto-aim if enemy is a bit behind
            if (ShouldAutoAim) {
                AnimationVars::Stomp::SetUnderStompBlend_X(giant, x); // We added new behavior variables, needs new Behaviors in order to work
                AnimationVars::Stomp::SetUnderStompBlend_Y(giant, y); // We added new behavior variables, needs new Behaviors in order to work
            } else { // Reset vars just in case with a bit of rng
                AnimationVars::Stomp::SetUnderStompBlend_X(giant, RandomFloat(0.0f, 0.15f));
                AnimationVars::Stomp::SetUnderStompBlend_Y(giant, RandomFloat(0.0f, 0.15f));
            }
            return ShouldAutoAim;
        }
        bool AutoAim_IsSneakingOrCrawling(Actor* giant) {
            return (giant->IsSneaking() || AnimationVars::Crawl::IsCrawling(giant));
        }
        bool AutoAim_SetUpDefaultSide(Actor* giant) {
            auto tranData = Transient::GetActorData(giant);
            if (tranData) {
                tranData->AutoAim_TargetLeft = !tranData->AutoAim_TargetLeft;
                return tranData->AutoAim_TargetLeft;
            }
            return RandomBool(); 
        }
    }