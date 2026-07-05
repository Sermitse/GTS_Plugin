
#include "Utils/Actor/AutoAimUtils_Calculation.hpp"
#include "Utils/Actor/FindActor.hpp"
#include "Magic/Effects/Common.hpp"
#include "Config/Config.hpp"



namespace {
    using namespace GTS;
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
    Actor* FindClosestTargetBetweenTwoPoints_Rhomb(Actor* giant, const NiPoint3 pointL, const NiPoint3 pointR, float maxSearchDistance, bool& leftFoot) {
        Actor* bestVictim = nullptr;
        float bestScore = FLT_MAX;

        const float yaw = giant->data.angle.z;

        const NiPoint3 center = giant->GetPosition();
        const NiPoint3 forward(std::sin(yaw), std::cos(yaw), 0.0f);
        const NiPoint3 right(forward.y, -forward.x, 0.0f);

        for (auto target : find_actors()) {
            if (!target || target == giant) {
                continue;
            }

            if (!IsHostile(giant, target) && IsTeammate(target) && Config::General.bProtectFollowers) {
                continue;
            }

            const bool dead = target->IsDead() || GetAV(target, ActorValue::kHealth) <= 0.0f;
            const float deadPenalty = dead ? Config::AutoAim.fAutoAim_DeadPenalty : 1.0f;

            NiPoint3 targetPos = target->GetPosition();
            targetPos.z = 0.0f;

            auto EvaluatePoint = [&](const NiPoint3& point, float& score)
            {
                NiPoint3 delta = targetPos - point;
                delta.z = 0.0f;

                float localForward = delta.x * forward.x + delta.y * forward.y;

                float localRight = delta.x * right.x + delta.y * right.y;

                // Diamond (L1) distance
                float diamondDistance = std::abs(localForward) + std::abs(localRight);

                if (diamondDistance > maxSearchDistance) {
                    score = FLT_MAX;
                    return;
                }

                score = diamondDistance;

                // Penalty for targets behind the actor
                NiPoint3 centerDelta = targetPos - center;
                centerDelta.z = 0.0f;

                float centerForward = centerDelta.x * forward.x + centerDelta.y * forward.y;

                if (centerForward < 0.0f) {
                    score += centerForward * centerForward * deadPenalty * Config::AutoAim.fAutoAim_BackPenalty;
                }
            };

            float scoreL;
            float scoreR;

            EvaluatePoint(pointL, scoreL);
            EvaluatePoint(pointR, scoreR);

            bool useLeft = scoreL <= scoreR;
            float score = useLeft ? scoreL : scoreR;

            if (score < bestScore) {
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
                logger::info("using Rhomb search");
                return bestVictim;
            }
            
            const float maxDistSq = maxSearchDistance * maxSearchDistance;
            float bestScore = FLT_MAX;

            const float yaw = giant->data.angle.z;

            const NiPoint3 center = giant->GetPosition();
            const NiPoint3 forward(std::sin(yaw), std::cos(yaw), 0.0f);

            for (auto target : find_actors()) {
                if (!target || target == giant) {
                    continue;
                }

                if (!IsHostile(giant, target) && IsTeammate(target) && Config::General.bProtectFollowers) {
                    continue;
                }

                const bool Dead = target->IsDead() || GetAV(target, ActorValue::kHealth) <= 0.0f;
                float DeadPenalty = Dead ? Config::AutoAim.fAutoAim_DeadPenalty : 1.0f;

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
                    score += localForward * localForward * DeadPenalty * Config::AutoAim.fAutoAim_BackPenalty;
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
    }