
#include "Utils/Actions/AutoAim/AutoAimUtils_Calculation.hpp"
#include "Utils/Actions/AutoAim/AutoAimUtils.hpp"
#include "Utils/Actor/FindActor.hpp"
#include "Magic/Effects/Common.hpp"
#include "Config/Config.hpp"



namespace {
    using namespace GTS;
    constexpr float HALF_PI = 1.57079632679f;
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
    
    Actor* FindClosestTargetInRectangle(Actor* giant, const NiPoint3 origin, float width, float length) {
        Actor* bestVictim = nullptr;
        float bestScore = FLT_MAX;
        int bestTier = INT_MAX; // 0 = alive, 1 = dead

        NiPoint3 rectOrigin = origin;
        rectOrigin.z = 0.0f;

        const float halfWidth = width * 0.5f;

        for (auto target : find_actors()) {

            if (!target || target == giant) {
                continue;
            }

            if (!IsHostile(giant, target) && IsTeammate(target) && Config::General.bProtectFollowers) {
                continue;
            }

            NiPoint3 targetPos = target->GetPosition();
            targetPos.z = 0.0f;

            float forward;
            float right;

            GetRectangleCoordinates(giant, rectOrigin, targetPos, length, forward, right);

            if (std::abs(forward) > length * 0.5f)
                continue;

            if (std::abs(right) > width * 0.5f)
                continue;

            const bool dead = target->IsDead() || GetAV(target, ActorValue::kHealth) <= 0.0f;
            const int tier = dead ? 1 : 0;

            // Distance from origin of rectangle
            float score = forward * forward + right * right;

            if (dead) {
                score *= Config::AutoAim.fAimAssist_DeadPenalty;
            }

            // Targets behind giant get penalty
            if (forward < 0.0f) {
                score += (-forward) * Config::AutoAim.fAimAssist_BackPenalty;
            }

            if (tier < bestTier || (tier == bestTier && score < bestScore)) {
                bestTier = tier;
                bestScore = score;
                bestVictim = target;
            }
        }

        return bestVictim;
    }
    Actor* FindClosestTargetBetweenTwoPoints_Rhomb(Actor* giant, const NiPoint3 pointL, const NiPoint3 pointR, float maxSearchDistance, bool& leftFoot) {
        Actor* bestVictim = nullptr;
        float bestScore = FLT_MAX;
        int bestTier = INT_MAX;

        const float yaw = giant->data.angle.z;

        const NiPoint3 center = giant->GetPosition();
        const NiPoint3 forward(std::sin(yaw), std::cos(yaw), 0.0f);
        const NiPoint3 right(forward.y, -forward.x, 0.0f);

        NiPoint3 leftPoint = pointL;
        NiPoint3 rightPoint = pointR;
        leftPoint.z = 0.0f;
        rightPoint.z = 0.0f;

        for (auto target : find_actors()) {
            if (!target || target == giant) {
                continue;
            }

            if (!IsHostile(giant, target) && IsTeammate(target) && Config::General.bProtectFollowers) {
                continue;
            }

            const bool dead = target->IsDead() || GetAV(target, ActorValue::kHealth) <= 0.0f;
            const int tier = dead ? 1 : 0;
            const float deadPenalty = dead ? Config::AutoAim.fAimAssist_DeadPenalty : 1.0f;

            NiPoint3 targetPos = target->GetPosition();
            targetPos.z = 0.0f;

            auto EvaluatePoint = [&](const NiPoint3& point, float& score)
            {
                NiPoint3 delta = targetPos - point;
                delta.z = 0.0f;

                float localForward = delta.x * forward.x + delta.y * forward.y;
                float localRight = delta.x * right.x + delta.y * right.y;

                float diamondDistance = std::abs(localForward) + std::abs(localRight);

                if (diamondDistance > maxSearchDistance) {
                    score = FLT_MAX;
                    return;
                }

                score = diamondDistance * deadPenalty; 

                NiPoint3 centerDelta = targetPos - center;
                centerDelta.z = 0.0f;

                float centerForward = centerDelta.x * forward.x + centerDelta.y * forward.y;

                if (centerForward < 0.0f) {
                    score += centerForward * centerForward * Config::AutoAim.fAimAssist_BackPenalty;
                }
            };

            float scoreL;
            float scoreR;

            EvaluatePoint(leftPoint, scoreL);
            EvaluatePoint(rightPoint, scoreR);

            if (scoreL == FLT_MAX && scoreR == FLT_MAX) {
                continue;
            }

            bool useLeft = scoreL <= scoreR;
            float score = useLeft ? scoreL : scoreR;

            if (tier < bestTier || (tier == bestTier && score < bestScore)) {
                bestTier = tier;
                bestScore = score;
                bestVictim = target;
                leftFoot = useLeft;
            }
        }

        return bestVictim;
    }
    Actor* FindClosestTargetBetweenTwoPoints(Actor* giant, const NiPoint3 pointL, const NiPoint3 pointR, float maxSearchDistance, bool& leftFoot) {
        const bool Rhomb = Config::AutoAim.bUseRhombShape;
        Actor* bestVictim = nullptr;
        if (Rhomb) {
            bestVictim = FindClosestTargetBetweenTwoPoints_Rhomb(giant, pointL, pointR, maxSearchDistance, leftFoot);
            return bestVictim;
        }

        const float maxDistSq = maxSearchDistance * maxSearchDistance;
        float bestScore = FLT_MAX;
        int bestTier = INT_MAX;

        const float yaw = giant->data.angle.z;

        const NiPoint3 center = giant->GetPosition();
        const NiPoint3 forward(std::sin(yaw), std::cos(yaw), 0.0f);

        NiPoint3 leftPoint = pointL;
        NiPoint3 rightPoint = pointR;
        leftPoint.z = 0.0f;
        rightPoint.z = 0.0f;

        for (auto target : find_actors()) {
            if (!target || target == giant) {
                continue;
            }

            if (!IsHostile(giant, target) && IsTeammate(target) && Config::General.bProtectFollowers) {
                continue;
            }

            const bool dead = target->IsDead() || GetAV(target, ActorValue::kHealth) <= 0.0f;
            const int tier = dead ? 1 : 0;
            const float DeadPenalty = dead ? Config::AutoAim.fAimAssist_DeadPenalty : 1.0f;

            NiPoint3 targetPos = target->GetPosition();
            targetPos.z = 0.0f;

            NiPoint3 deltaL = targetPos - leftPoint;
            deltaL.z = 0.0f;
            float distSqL = deltaL.x * deltaL.x + deltaL.y * deltaL.y;

            NiPoint3 deltaR = targetPos - rightPoint;
            deltaR.z = 0.0f;
            float distSqR = deltaR.x * deltaR.x + deltaR.y * deltaR.y;

            bool useLeft = distSqL <= distSqR;
            float distSq = useLeft ? distSqL : distSqR;

            if (distSq > maxDistSq) {
                continue;
            }

            NiPoint3 centerDelta = targetPos - center;
            centerDelta.z = 0.0f;

            float localForward =
                centerDelta.x * forward.x +
                centerDelta.y * forward.y;

            float score = distSq * DeadPenalty;

            if (localForward < 0.0f) {
                score += localForward * localForward * Config::AutoAim.fAimAssist_BackPenalty;
            }

            if (tier < bestTier || (tier == bestTier && score < bestScore)) {
                bestTier = tier;
                bestScore = score;
                bestVictim = target;
                leftFoot = useLeft;
            }
        }

        return bestVictim;
    }
        
        void CalculateForwardBlend(Actor* giant, const NiPoint3& footPos, const NiPoint3& targetPos, AnimationBlendInfo& info) {
            float yaw = giant->data.angle.z;

            NiPoint3 offset = targetPos - footPos;
            NiPoint3 forward(std::sin(yaw),std::cos(yaw),0.0f);
            offset.z = 0.0f;
            
            info.finalDistance = offset.Length();

            float forwardDistance = offset.x * forward.x + offset.y * forward.y;

            float blend = std::clamp(forwardDistance / info.maxDistance, 0.0f, 1.0f);
            info.outDistanceX = forwardDistance;
            info.blendX = blend;
        }

        void CalculateDirectionalBlend2D(Actor* giant, const NiPoint3& footPos, const NiPoint3& targetPos, AnimationBlendInfo& info) {
            float yaw = giant->data.angle.z;

            NiPoint3 offset = targetPos - footPos;
            offset.z = 0.0f;

            float distance = offset.Length();
            info.finalDistance = distance;

            if (distance <= 0.001f) {
                info.blendX = 0.0f;
                info.blendY = 0.0f;
                info.outDistanceX = 0.0f;
                info.outDistanceY = 0.0f;
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
            float distanceWeight = std::clamp(distance / info.maxDistance, 0.0f, 1.0f);
            // 
            info.blendX = angleForward * distanceWeight;
            info.blendY = angleRight * distanceWeight;
            // 
            info.outDistanceX = offset.x * forward.x + offset.y * forward.y;
            info.outDistanceY = offset.x * right.x + offset.y * right.y;
        }

        void CalculateAngleBasedSideBlend(Actor* giant, const NiPoint3& footPos, const NiPoint3& targetPos, AnimationBlendInfo& info) {
            const float yaw = giant->data.angle.z;

            NiPoint3 forward(std::sin(yaw), std::cos(yaw), 0.0f);
            NiPoint3 right(forward.y, -forward.x, 0.0f);

            NiPoint3 offset = targetPos - footPos;
            offset.z = 0.0f;

            const float length = offset.Length();
            info.finalDistance = length;

            if (length < 0.001f) {
                info.blendY = 0.0f;
                info.outDistanceY = 0.0f;
                info.outDistanceX = 0.0f;
                return;
            }

            info.outDistanceY   = offset.x * right.x   + offset.y * right.y;
            info.outDistanceX = offset.x * forward.x + offset.y * forward.y;
            
            const float angle = std::atan2(info.outDistanceY, info.outDistanceX);

            // 0° -> 0
            // ±90° -> ±1
            // ±180° -> 0
            float side = 1.0f - std::abs(std::abs(angle) - HALF_PI) / HALF_PI;
            info.blendY = std::copysign(std::clamp(side, 0.0f, 1.0f), angle);
        }

        void CalculateRectangleBlend(Actor* giant, const NiPoint3& origin, const NiPoint3& target, AnimationBlendInfo& info) {
            GetRectangleCoordinates(giant, origin, target, info.length, info.outDistanceX, info.outDistanceY);
            info.finalDistance = std::hypot(info.outDistanceX, info.outDistanceY);

            const float halfLength = info.length * 0.5f;
            const float halfWidth = info.width * 0.5f;

            float begin = halfLength * info.blendOffset;

            info.blendX = std::clamp((info.outDistanceX - begin) / (halfLength - begin), 0.0f, 1.0f);
            info.isInsideRectangle = std::abs(info.outDistanceX) <= halfLength && std::abs(info.outDistanceY) <= halfWidth;

            if (info.isInsideRectangle) {
                info.inPercent_Directional = 1.0f - std::abs(info.outDistanceX) / halfLength;
                info.inPercent_Side        = 1.0f - std::abs(info.outDistanceY) / halfWidth;
            } 
        }
        
        void GetRectangleCoordinates(Actor* giant,const NiPoint3& origin, const NiPoint3& target, float length, float& forwardDist, float& rightDist) {
            const float yaw = giant->data.angle.z;

            const NiPoint3 forward( std::sin(yaw), std::cos(yaw), 0.0f);
            const NiPoint3 right(forward.y, -forward.x,0.0f);

            NiPoint3 delta = target - origin;
            delta.z = 0.0f;

            forwardDist = delta.x * forward.x + delta.y * forward.y;
            rightDist = delta.x * right.x + delta.y * right.y;
        }
    }