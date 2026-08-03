#include "Utils/Actions/AutoAim/AutoAimUtils_Calculation.hpp"
#include "Managers/Size_Killmoves/KillMoveParamObtainer.hpp"
#include "Managers/Size_Killmoves/SizeKillMove.hpp"
#include "Utils/Actions/AutoAim/AutoAimUtils.hpp"
#include "Utils/Actions/AutoAim/AimAssist.hpp"
#include "Utils/Actor/FindActor.hpp"
#include "Magic/Effects/Common.hpp"
#include "Config/Config.hpp"



namespace {
    using namespace GTS;
    constexpr glm::vec4 Breast_Color = {0.961f, 0.157f, 0.569f, 0.8f};
    constexpr glm::vec4 Close_Stomp_Color = {1.0f, 0.0f, 0.0f, 1.0f};
    constexpr glm::vec4 Far_Stomp_Color = {0.5f, 0.5f, 0.0f, 1.0f};
    constexpr glm::vec4 Origin_Point = {0.07f, 0.4f, 0.54f, 1.0f};
    constexpr glm::vec4 Kick_Color = {0.0f, 0.5f, 0.5f, 1.0f};

    void DrawRectangleShape(Actor* giant, NiPoint3 pointPos, Actor* victim, float width, float length, const glm::vec4& giantess_color) {
        const bool Rhomb = Config::AutoAim.bUseRhombShape;
        const float rotation = giant->data.angle.z;
        if (Config::AutoAim.bDebugAutoAim) {
            DebugDraw::DrawRectangle(rotation, glm::vec3(pointPos.x, pointPos.y, pointPos.z), width, length, 1500, giantess_color);
            if (victim) {
                auto victimPos = victim->GetPosition();
                Rhomb ? DebugDraw::DrawRhomb(glm::vec3(victimPos.x, victimPos.y, victimPos.z), 6.0f * get_visual_scale(giant), rotation, 2000, {0.0f, 0.6f, 0.0f, 1.0f})
                :       DebugDraw::DrawSphere(glm::vec3(victimPos.x, victimPos.y, victimPos.z), 6.0f * get_visual_scale(giant), 2000, {0.0f, 0.6f, 0.0f, 1.0f});
            }
        }
    }
    void DrawDebugShape(Actor* giant, NiPoint3 pointPos, Actor* victim, float max_distance, const glm::vec4& giantess_color) {
        const bool Rhomb = Config::AutoAim.bUseRhombShape;
        const float rotation = giant->data.angle.z;
        if (Config::AutoAim.bDebugAutoAim) {
            Rhomb ? DebugDraw::DrawRhomb(glm::vec3(pointPos.x, pointPos.y, pointPos.z), 6.0f * get_visual_scale(giant), rotation, 1500, Origin_Point)// Initial search pos
            :       DebugDraw::DrawSphere(glm::vec3(pointPos.x, pointPos.y, pointPos.z), 6.0f * get_visual_scale(giant), 1500, Origin_Point);

            Rhomb ? DebugDraw::DrawRhomb(glm::vec3(pointPos.x, pointPos.y, pointPos.z), max_distance, rotation, 1500, giantess_color)                // Collider search radius
            :       DebugDraw::DrawSphere(glm::vec3(pointPos.x, pointPos.y, pointPos.z), max_distance, 1500, giantess_color);

            if (victim) {
                auto victimPos = victim->GetPosition();
                Rhomb ? DebugDraw::DrawRhomb(glm::vec3(victimPos.x, victimPos.y, victimPos.z), 6.0f * get_visual_scale(giant), rotation, 2000, {0.0f, 0.6f, 0.0f, 1.0f})
                :       DebugDraw::DrawSphere(glm::vec3(victimPos.x, victimPos.y, victimPos.z), 6.0f * get_visual_scale(giant), 2000, {0.0f, 0.6f, 0.0f, 1.0f});
            }
        }
    }
    void DebugMissShape(Actor* giant, NiPoint3 footPos_L, NiPoint3 footPos_R, float max_distance, bool& left_foot, const glm::vec4& giantess_color) {
        DrawDebugShape(giant, footPos_L, nullptr, max_distance, giantess_color);
        DrawDebugShape(giant, footPos_R, nullptr, max_distance, giantess_color);
    }
    bool IsInRange(float final_distance, float max_distance) {
        // Usage: randomize attack side when enemy is not meant to be auto-aimed at but is still in range
        // Without it - Giantess uses same left/right attack without any variety
        return final_distance <= max_distance;
    }
    bool ShouldAutoAim(float final_distance, float max_distance, float dx, float modifier = 0.0f) {
        float ignore_past = std::clamp(Config::AutoAim.fAimAssist_IgnoreBehindAfter + modifier, 0.0f, 1.0f);
        return final_distance <= max_distance && dx >= -(max_distance * ignore_past); 
        // Allows to auto-aim if enemy is a bit behind
    }

    bool IsVictimAlive(Actor* victim) {
        return victim && !(victim->IsDead() || GetAV(victim, ActorValue::kHealth) <= 0.0f);
    }

    float ApplyMagnitude(float value, float multiplier = 1.0f) {
        return std::clamp(value * Config::AutoAim.fAimAssist_AimMagnitudeMultiplier * multiplier, -1.0f, 1.0f);
    }

    void DebugLogBlend(Actor* victim, float x, float y) {
        if (Config::AutoAim.bDebugAutoAim) {
            logger::info("Blend2D X:{}, Y:{} | Victim:{}", x, y, victim->GetDisplayFullName());
            Cprint("Blend2D X:{}, Y:{} | Victim:{}", x, y, victim->GetDisplayFullName());
        }
    }

    Actor* FindAndDebugTwoPointTarget(Actor* giant, NiPoint3 posL, NiPoint3 posR, float max_distance, bool& left, const glm::vec4& color, NiPoint3& outPos, NiPoint3& outVictimPos) {
        auto victim = FindClosestTargetBetweenTwoPoints(giant, posL, posR, max_distance, left); // Overrides `left`
        if (!victim) {
            DebugMissShape(giant, posL, posR, max_distance, left, color);
            return nullptr;
        }

        NiPoint3 pos = left ? posL : posR; // Pick which hand/foot should be used
        outVictimPos = victim->GetPosition();

        DrawDebugShape(giant, pos, victim, max_distance, color);

        pos.z = 0.0f;
        outVictimPos.z = 0.0f;
        outPos = pos;
        return victim;
    }

    Actor* FindAndDebugRectTarget(Actor* giant, NiPoint3 origin, float width, float length, const glm::vec4& color, NiPoint3& outOrigin, NiPoint3& outVictimPos) {
        auto victim = FindClosestTargetInRectangle(giant, origin, width, length);
        if (!victim) {
            DrawRectangleShape(giant, origin, nullptr, width, length, color);
            return nullptr;
        }

        outVictimPos = victim->GetPosition();
        DrawRectangleShape(giant, origin, victim, width, length, color);

        origin.z = 0.0f;
        outVictimPos.z = 0.0f;
        outOrigin = origin;
        return victim;
    }

    bool FinalizeAim(Actor* giant, bool& left, Actor* victim, AnimationBlendInfo& params, AimAssistResult* out_result, float modifier = 0.0f) {
        bool AutoAim = ShouldAutoAim(params.finalDistance, params.maxDistance, params.outDistanceX, modifier);
        if (AutoAim) {
            if (out_result) {
                out_result->hit = true;
                out_result->alive = IsVictimAlive(victim);
                out_result->blend_x = params.blendX;
                out_result->blend_y = params.blendY;
                out_result->distance = params.finalDistance;
                out_result->victim = victim;
            } else {
                SetStompBlendValues(giant, params.blendX, params.blendY);
            }
        } else if (IsInRange(params.finalDistance, params.maxDistance)) {
            left = RandomBool();
        }
        return AutoAim;
    }
}

namespace GTS {
        bool AutoAim_Kick_DeterminePreferredKick(Actor* giant, bool& left, bool strong) {
            if (!giant) return RandomBool();

            float foot_offset_side = Config::AutoAim.fAimAssist_OffsetDistance_Foot * get_visual_scale(giant);
            float foot_offset_forward = Config::AutoAim.fAimAssist_OffsetDistance_Kick_Forward * get_visual_scale(giant);
            float max_distance = Config::AutoAim.fAimAssist_Range_Kick * get_visual_scale(giant);

            if (AutoAim_IsSneakingOrCrawling(giant)) {
                foot_offset_side = Config::AutoAim.fAimAssist_OffsetDistance_Hand_Sneak_Side * get_visual_scale(giant);
                foot_offset_forward = Config::AutoAim.fAimAssist_OffsetDistance_Hand_Sneak_Forward_Sneak * get_visual_scale(giant);
                max_distance = Config::AutoAim.fAimAssist_Range_Kick_Sneak * get_visual_scale(giant);
            }

            NiPoint3 footPos_L = GetPresetAimPosition(giant, true, foot_offset_side, foot_offset_forward);
            NiPoint3 footPos_R = GetPresetAimPosition(giant, false, foot_offset_side, foot_offset_forward);

            NiPoint3 footPos, victimPos;
            auto victim = FindAndDebugTwoPointTarget(giant, footPos_L, footPos_R, max_distance, left, Kick_Color, footPos, victimPos);
            if (!victim) {
                left = !left;
                return left;
            }
            AnimationBlendInfo params;
            params.maxDistance = max_distance;

            CalculateForwardBlend(giant, footPos, victimPos, params);

            if (!giant->IsSneaking() && !victim->IsDead()) {
                auto params = GetKickKillMoveParams(left, strong);
                const auto node = find_node(giant, params.nodeLookups[0]);
                StartKillmove(giant, victim, node, params.damageSource, params);
            }

            return left;
        }
        bool AutoAim_Crawl_TryBreastSlam(Actor* giant, bool& left_hand, AimAssistResult* out_result) {
            if (!giant) return false;
            if (giant->IsPlayerRef() && IsFreeCameraEnabled()) return false;

            const float forward_offset = Config::AutoAim.fAimAssist_OffsetDistance_Breasts_Forward * get_visual_scale(giant);
            const float width = Config::AutoAim.fAimAssist_OffsetDistance_Breasts_Width * get_visual_scale(giant);
            const float length = Config::AutoAim.fAimAssist_OffsetDistance_Breasts_Length * get_visual_scale(giant);
            const float blend_offset = Config::AutoAim.fAimAssist_Value_Breasts_BlendOffset;

            NiPoint3 breastPos = GetPresetAimPosition(giant, true, 0.0f, forward_offset);

            NiPoint3 origin, victimPos;
            auto victim = FindAndDebugRectTarget(giant, breastPos, width, length, Breast_Color, origin, victimPos);
            if (!victim) return false; // No victim found

            AnimationBlendInfo params;
            params.blendOffset = blend_offset;
            params.length = length;
            params.width = width;

            CalculateRectangleBlend(giant, origin, victimPos, params);
            params.blendX = std::clamp(params.blendX * 1.1f, 0.0f, 1.0f); // Slightly stronger blend

            

            if (!params.isInsideRectangle) return false;
            bool InFrontOfGTS = true; 

            if (params.blendX <= 0.01f) { // Add some variety if actor is behind
                const float range = Config::AutoAim.fAimAssist_NoHitValueRandomRange;
                params.blendX = RandomFloat(0.0f, range);
                params.blendY= RandomFloat(0.0f, range);
                InFrontOfGTS = false; // Disallow Killmove, it's supposed to trigger near breasts, not in butt or legs area.
            }

            if (Config::AutoAim.bDebugAutoAim) {
                logger::info("Blend2D X:{} |  Victim:{}", params.blendX, victim->GetDisplayFullName());
                logger::info("FinalDist: {}", params.finalDistance);
                Cprint("Blend2D X:{} | Victim:{}", params.blendX, victim->GetDisplayFullName());
            }
            const bool AllowKillMove = InFrontOfGTS && params.inPercent_Side >= 0.3f;
            // Actor must be at least 30% 'deep' inside rectangle (so breasts don't strike air) and in front part of it

            if (out_result) {
                out_result->hit = true;
                out_result->alive = IsVictimAlive(victim);
                out_result->canKillMove = AllowKillMove;  
                out_result->blend_x = params.blendX;
                out_result->blend_y = params.blendY;
                out_result->distance = params.finalDistance;
                out_result->victim = victim;
            } else {
                SetStompBlendValues(giant, params.blendX, params.blendY);
            }
            return true;
        }
        bool AutoAim_Crawl_TryButtSlam(Actor* giant, bool& left_butt, AimAssistResult* out_result) {
            if (!giant) return false;
            if (giant->IsPlayerRef() && IsFreeCameraEnabled()) return false;

            const float max_distance = Config::AutoAim.fAimAssist_Range_ButtSlam * get_visual_scale(giant);
            const float butt_offset_side = Config::AutoAim.fAimAssist_OffsetDistance_Butt_Side * get_visual_scale(giant);
            const float butt_offset_forward = Config::AutoAim.fAimAssist_OffsetDistance_Butt_Forward * get_visual_scale(giant);

            NiPoint3 buttPos_L = GetPresetAimPosition(giant, true, butt_offset_side, butt_offset_forward);
            NiPoint3 buttPos_R = GetPresetAimPosition(giant, false, butt_offset_side, butt_offset_forward);

            NiPoint3 buttPos, victimPos;
            auto victim = FindAndDebugTwoPointTarget(giant, buttPos_L, buttPos_R, max_distance, left_butt, Breast_Color, buttPos, victimPos);
            if (!victim) return false; // No victim found

            AnimationBlendInfo params;
            params.maxDistance = max_distance;
            
            CalculateDirectionalBlend2D(giant, buttPos, victimPos, params);

            params.blendX = ApplyMagnitude(params.blendX);
            params.blendY = ApplyMagnitude(params.blendY, 1.5f);
            DebugLogBlend(victim, params.blendX, params.blendY);

            return FinalizeAim(giant, left_butt, victim, params, out_result, 0.25f);
        }
        bool AutoAim_Hand_TryHandAim_Far(Actor* giant, bool& left_hand, bool strong_Attack, AimAssistResult* out_result) {
            if (!giant) return false;
            if (giant->IsPlayerRef() && IsFreeCameraEnabled()) return false;

            float max_distance = Config::AutoAim.fAimAssist_Range_Hand_Crawl_Far * get_visual_scale(giant);
            float hand_offset_side = Config::AutoAim.fAimAssist_OffsetDistance_Hand_Crawl_Side_Far * get_visual_scale(giant);
            float hand_offset_forward = Config::AutoAim.fAimAssist_OffsetDistance_Hand_Crawl_Forward_Far * get_visual_scale(giant);

            if (strong_Attack) {
                max_distance = Config::AutoAim.fAimAssist_Range_Hand_Crawl_Strong * get_visual_scale(giant);
                hand_offset_side = Config::AutoAim.fAimAssist_OffsetDistance_Hand_Crawl_Side_Strong* get_visual_scale(giant);
                hand_offset_forward = Config::AutoAim.fAimAssist_OffsetDistance_Hand_Crawl_Forward_Strong * get_visual_scale(giant);
            }

            NiPoint3 handPos_L = GetPresetAimPosition(giant, true, hand_offset_side, hand_offset_forward);
            NiPoint3 handPos_R = GetPresetAimPosition(giant, false, hand_offset_side, hand_offset_forward);

            NiPoint3 handPos, victimPos;
            auto victim = FindAndDebugTwoPointTarget(giant, handPos_L, handPos_R, max_distance, left_hand, Far_Stomp_Color, handPos, victimPos);
            if (!victim) return false;

            AnimationBlendInfo params;
            params.maxDistance = max_distance;

            CalculateAngleBasedSideBlend(giant, handPos, victimPos, params);

            params.blendX = ApplyMagnitude(params.blendX);
            params.blendY = ApplyMagnitude(params.blendY);
            DebugLogBlend(victim, params.blendX, params.blendY);

            return FinalizeAim(giant, left_hand, victim, params, out_result);
        }
        bool AutoAim_Hand_TryHandAim(Actor* giant, bool& left_hand, bool strong_Attack, AimAssistResult* out_result) {
            if (!giant) return false;
            if (giant->IsPlayerRef() && IsFreeCameraEnabled()) return false;

            float max_distance = Config::AutoAim.fAimAssist_Range_Hand_Sneak_Slam * get_visual_scale(giant);
            float hand_offset_side = Config::AutoAim.fAimAssist_OffsetDistance_Hand_Sneak_Side * get_visual_scale(giant);
            float hand_offset_forward = Config::AutoAim.fAimAssist_OffsetDistance_Hand_Sneak_Forward * get_visual_scale(giant);

            if (AnimationVars::Crawl::IsCrawling(giant)) { // Replace with Crawl version
                max_distance = Config::AutoAim.fAimAssist_Range_Hand_Crawl_Close * get_visual_scale(giant);
                hand_offset_side = Config::AutoAim.fAimAssist_OffsetDistance_Hand_Crawl_Side * get_visual_scale(giant);
                hand_offset_forward = Config::AutoAim.fAimAssist_OffsetDistance_Hand_Crawl_Forward * get_visual_scale(giant);
            }

            if (strong_Attack) {
                max_distance = Config::AutoAim.fAimAssist_Range_Hand_Sneak_Slam_Strong * get_visual_scale(giant); // Strong version has further range
            }

            NiPoint3 handPos_L = GetPresetAimPosition(giant, true, hand_offset_side, hand_offset_forward);
            NiPoint3 handPos_R = GetPresetAimPosition(giant, false, hand_offset_side, hand_offset_forward);

            NiPoint3 handPos, victimPos;
            auto victim = FindAndDebugTwoPointTarget(giant, handPos_L, handPos_R, max_distance, left_hand, Far_Stomp_Color, handPos, victimPos);
            if (!victim) return false;

            float x = 0.0f;
            float y = 0.0f;
            float dx = 0.0f;
            float dy = 0.0f;
            float final_distance = 0.0f;
            AnimationBlendInfo params;
            params.maxDistance = max_distance;

            if (AnimationVars::Crawl::IsCrawling(giant)) {
                CalculateDirectionalBlend2D(giant, handPos, victimPos, params);
            } else {
                CalculateAngleBasedSideBlend(giant, handPos, victimPos, params);
            }

            params.blendX = ApplyMagnitude(params.blendX);
            params.blendY = ApplyMagnitude(params.blendY);
            DebugLogBlend(victim, params.blendX, params.blendY);

            return FinalizeAim(giant, left_hand, victim, params, out_result);
        }

        bool AutoAim_Foot_Directional(Actor* giant, bool& left_foot, bool strong_Attack, AimAssistResult* out_result) {
            if (!giant) return false;
            if (giant->IsPlayerRef() && IsFreeCameraEnabled()) return false;

            float side_offset = Config::AutoAim.fAimAssist_OffsetDistance_Foot * get_visual_scale(giant); 
            float forward_offset_R = 0.0f; float forward_offset_L = 0.0f;
            // ^ Instead of looking for R/L foot, we do position offset from center of Char to right/left, based on left_foot bool
            float max_distance = Config::AutoAim.fAimAssist_Range_Stomp * get_visual_scale(giant);
            if (giant->IsSneaking()) {
                forward_offset_R = Config::AutoAim.fAimAssist_OffsetDistance_Stomp_Sneak_Forward_R  * get_visual_scale(giant);
                forward_offset_L = Config::AutoAim.fAimAssist_OffsetDistance_Stomp_Sneak_Forward_L  * get_visual_scale(giant);
                side_offset = Config::AutoAim.fAimAssist_OffsetDistance_Stomp_Sneak_Side * get_visual_scale(giant); 
                max_distance = Config::AutoAim.fAimAssist_Range_Stomp_Sneak * get_visual_scale(giant);
            } else if (strong_Attack) {
                max_distance = Config::AutoAim.fAimAssist_Range_Stomp_Strong * get_visual_scale(giant);
            }

            NiPoint3 footPos_L = GetPresetAimPosition(giant, true, side_offset, forward_offset_L);
            NiPoint3 footPos_R = GetPresetAimPosition(giant, false, side_offset, forward_offset_R);

            NiPoint3 footPos, victimPos;
            auto victim = FindAndDebugTwoPointTarget(giant, footPos_L, footPos_R, max_distance, left_foot, Close_Stomp_Color, footPos, victimPos);
            if (!victim) return false;

            AnimationBlendInfo params;
            params.maxDistance = max_distance;

            CalculateDirectionalBlend2D(giant, footPos, victimPos, params);

            params.blendX = ApplyMagnitude(params.blendX);
            params.blendY = ApplyMagnitude(params.blendY);
            DebugLogBlend(victim, params.blendX, params.blendY);

            return FinalizeAim(giant, left_foot, victim, params, out_result);
        }

        bool AutoAim_Foot_Directional_FarStomp(Actor* giant, bool& left_foot, bool strong_stomp, AimAssistResult* out_result) {
            if (!giant) return RandomBool();
            if (giant->IsPlayerRef() && IsFreeCameraEnabled()) return RandomBool();

            float max_distance = Config::AutoAim.fAimAssist_Range_FarStomp * get_visual_scale(giant);
            const float foot_offset = Config::AutoAim.fAimAssist_OffsetDistance_Foot * get_visual_scale(giant); 
            const float foot_offset_far = Config::AutoAim.fAimAssist_OffsetDistance_Foot_FarStomp * get_visual_scale(giant); 
            // ^ Instead of looking for R/L foot, we do position offset from center of Char to right/left, based on left_foot bool
            if (strong_stomp) {
                max_distance = Config::AutoAim.fAimAssist_Range_FarStomp_Strong * get_visual_scale(giant); // Strong version has further range
            }

            NiPoint3 footPos_L = GetPresetAimPosition(giant, true, foot_offset, foot_offset_far);
            NiPoint3 footPos_R = GetPresetAimPosition(giant, false, foot_offset, foot_offset_far);

            NiPoint3 footPos, victimPos;
            auto victim = FindAndDebugTwoPointTarget(giant, footPos_L, footPos_R, max_distance, left_foot, Far_Stomp_Color, footPos, victimPos);
            if (!victim) return false;
            AnimationBlendInfo params;
            params.maxDistance = max_distance;
            //---------- Needed for back-stomps, we just want to fill X value here
            CalculateDirectionalBlend2D(giant, footPos, victimPos, params); 
            // Fill Y value, Far Stomps have only Side Blends
            CalculateAngleBasedSideBlend(giant, footPos, victimPos, params);

            params.blendX = ApplyMagnitude(params.blendX);
            params.blendY = ApplyMagnitude(params.blendY);
            DebugLogBlend(victim, params.blendX, params.blendY);

            return FinalizeAim(giant, left_foot, victim, params, out_result);
        }

        bool AutoAim_IsSneakingOrCrawling(Actor* giant) {
            return (giant->IsSneaking() || AnimationVars::Crawl::IsCrawling(giant));
        }
        bool AutoAim_Miss_GetNextStompSide(Actor* giant, StompAimType type) {
            auto tranData = Transient::GetActorData(giant);
            if (tranData) {
                switch (type) { 
                    // Some actions call this function each frame while the key-bind is pressed, resulting in never switching next animation side
                    // For example: some Kicks/Trample key-binds
                    // 'Fix' is to have unique bools reserved for each action, so bool isn't getting overriden by other keybinds
                    // Im unsure of better fix for it, and i have no desire to spend any more time on fixing this issue
                    // So i'm keeping what works
                    case StompAimType::T1:
                        tranData->AutoAim_T1 = !tranData->AutoAim_T1;
                    return tranData->AutoAim_T1;
                    case StompAimType::T2:
                        tranData->AutoAim_T2 = !tranData->AutoAim_T2;
                    return tranData->AutoAim_T2;
                    case StompAimType::T3:
                        tranData->AutoAim_T3 = !tranData->AutoAim_T3;
                    return tranData->AutoAim_T3;
                    case StompAimType::T4:
                        tranData->AutoAim_T4 = !tranData->AutoAim_T4;
                    return tranData->AutoAim_T4;
                    default: return RandomBool();
                }
            }
            return RandomBool(); 
        }

        void SetStompBlendValues(Actor* giant, float x, float y) {
            AnimationVars::Stomp::SetUnderStompBlend_Legacy(giant, x); // Old one stays for compatibility reasons
            AnimationVars::Stomp::SetUnderStompBlend_X(giant, x); // We added new behavior variables, needs new Behaviors in order to work
            AnimationVars::Stomp::SetUnderStompBlend_Y(giant, y); // We added new behavior variables, needs new Behaviors in order to work
        }
    }