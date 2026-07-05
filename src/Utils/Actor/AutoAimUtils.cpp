#include "Utils/Actor/AutoAimUtils_Calculation.hpp"
#include "Utils/Actor/AutoAimUtils.hpp"
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


    void DrawDebugSpheres(Actor* giant, NiPoint3 pointPos, Actor* victim, float max_distance, const glm::vec4& giantess_color) {
        logger::info("Char Angle: {}", giant->data.angle.z);
        const bool Rhomb = Config::AutoAim.bUseRhombShape;
        const float rotation = giant->data.angle.z;
        if (Config::AutoAim.bDebugAutoAim) {
            Rhomb ? DebugDraw::DrawRhomb(glm::vec3(pointPos.x, pointPos.y, pointPos.z), 6.0f * get_visual_scale(giant), rotation, 900, Origin_Point)// Initial search pos
            :       DebugDraw::DrawSphere(glm::vec3(pointPos.x, pointPos.y, pointPos.z), 6.0f * get_visual_scale(giant), 900, Origin_Point);

            Rhomb ? DebugDraw::DrawRhomb(glm::vec3(pointPos.x, pointPos.y, pointPos.z), max_distance, rotation, 500, giantess_color)                // Collider search radius
            :       DebugDraw::DrawSphere(glm::vec3(pointPos.x, pointPos.y, pointPos.z), max_distance, 500, giantess_color);

            if (victim) {
                auto victimPos = victim->GetPosition();
                Rhomb ? DebugDraw::DrawRhomb(glm::vec3(victimPos.x, victimPos.y, victimPos.z), 6.0f * get_visual_scale(giant), rotation, 300, {0.0f, 0.6f, 0.0f, 1.0f})
                :       DebugDraw::DrawSphere(glm::vec3(victimPos.x, victimPos.y, victimPos.z), 6.0f * get_visual_scale(giant), 300, {0.0f, 0.6f, 0.0f, 1.0f});
            }
        }
    }
    void RandomizeBlendValues(Actor* giant) {
        AnimationVars::Stomp::SetUnderStompBlend_Legacy(giant, RandomFloat(0.0f, Config::AutoAim.fAutoAim_NoHitValueRandomRange));
        AnimationVars::Stomp::SetUnderStompBlend_X(giant, RandomFloat(0.0f, Config::AutoAim.fAutoAim_NoHitValueRandomRange));
        AnimationVars::Stomp::SetUnderStompBlend_Y(giant, RandomFloat(0.0f, Config::AutoAim.fAutoAim_NoHitValueRandomRange));
    }
    void RandomizeBlend_Debug_FlipFootBool(Actor* giant, NiPoint3 footPos_L, NiPoint3 footPos_R, float max_distance, bool& left_foot, const glm::vec4& giantess_color) {
        RandomizeBlendValues(giant);
        DrawDebugSpheres(giant, footPos_L, nullptr, max_distance, giantess_color);
        DrawDebugSpheres(giant, footPos_R, nullptr, max_distance, giantess_color);
        left_foot = !left_foot; // No victim = randomize used leg by flipping ref bool
    }
    bool ShouldAutoAim(float final_distance, float max_distance, float dx) {
        return final_distance <= max_distance  && dx >= -(max_distance * Config::AutoAim.fAutoAim_IgnoreBehindAfter); 
        // Allows to auto-aim if enemy is a bit behind
    }
}

namespace GTS {
        bool AutoAim_Kick_DeterminePreferredKick(Actor* giant) {
            if (!giant) return RandomBool();

            bool left = AutoAim_SetUpDefaultSide(giant);
            float foot_offset_side = Config::AutoAim.fAutoAim_Foot_OffsetDistance * get_visual_scale(giant);
            float foot_offset_forward = Config::AutoAim.fAutoAim_Kick_OffsetDistance_Forward * get_visual_scale(giant);
            float max_distance = Config::AutoAim.fAutoAim_Range_Kick * get_visual_scale(giant);

            if (AutoAim_IsSneakingOrCrawling(giant)) {
                logger::info("Applying sneak attacks");
                foot_offset_side = Config::AutoAim.fAutoAim_Hand_OffsetDistance_Side * get_visual_scale(giant);
                foot_offset_forward = Config::AutoAim.fAutoAim_Hand_OffsetDistance_Forward_Sneak * get_visual_scale(giant);
                max_distance = Config::AutoAim.fAutoAim_Range_Kick_Sneak * get_visual_scale(giant);
            }

            NiPoint3 footPos_L = GetPresetAimPosition(giant, true, foot_offset_side, foot_offset_forward);
            NiPoint3 footPos_R = GetPresetAimPosition(giant, false, foot_offset_side, foot_offset_forward);

            auto victim = FindClosestTargetBetweenTwoPoints(giant, footPos_L, footPos_R, max_distance, left);

            if (!victim) {
                logger::info("No target found");
                DrawDebugSpheres(giant, footPos_L, nullptr, max_distance, Kick_Color);
                DrawDebugSpheres(giant, footPos_R, nullptr, max_distance, Kick_Color);

                return left;
            }
            NiPoint3 victimPos = victim->GetPosition();
            NiPoint3 footPos = left ? footPos_L : footPos_R;

            DrawDebugSpheres(giant, footPos, victim, max_distance, Kick_Color);

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
        bool AutoAim_Butt_TryBreastSlam(Actor* giant, bool& left_hand) {
            if (!giant) return false;
            if (giant->IsPlayerRef() && IsFreeCameraEnabled()) return false;

            const float max_distance = Config::AutoAim.fAutoAim_Range_ButtSlam * get_visual_scale(giant);
            const float breast_offset_side = Config::AutoAim.fAutoAim_Breast_OffsetDistance_Side * get_visual_scale(giant);
            const float breast_offset_forward = Config::AutoAim.fAutoAim_Breast_OffsetDistance_Forward * get_visual_scale(giant);

            NiPoint3 breastPos_L = GetPresetAimPosition(giant, true, breast_offset_side, breast_offset_forward);
            NiPoint3 breastPos_R = GetPresetAimPosition(giant, false, breast_offset_side, breast_offset_forward);
            auto victim = FindClosestTargetBetweenTwoPoints_Rhomb(giant, breastPos_L, breastPos_R, max_distance, left_hand); // Overrides left_hand bool

            if (!victim) {
                NiPoint3 breastPos_L_1 = GetPresetAimPosition(giant, true, breast_offset_side, -breast_offset_forward);
                NiPoint3 breastPos_R_1 = GetPresetAimPosition(giant, false, breast_offset_side, -breast_offset_forward);
                auto victim_1 = FindClosestTargetBetweenTwoPoints_Rhomb(giant, breastPos_L_1, breastPos_R_1, max_distance, left_hand); //Try again
                if (victim_1) {
                    victim = victim_1;
                } else {
                    DrawDebugSpheres(giant, breastPos_L, nullptr, max_distance, Breast_Color);
                    DrawDebugSpheres(giant, breastPos_R, nullptr, max_distance, Breast_Color);
                    DrawDebugSpheres(giant, breastPos_L_1, nullptr, max_distance, Breast_Color);
                    DrawDebugSpheres(giant, breastPos_R_1, nullptr, max_distance, Breast_Color);
                    return false; // No victim found
                } 
            }
            NiPoint3 breastPos = left_hand ? breastPos_L : breastPos_R;
            NiPoint3 victimPos = victim->GetPosition();

            DrawDebugSpheres(giant, breastPos, victim, max_distance, Breast_Color);

            breastPos.z = 0.0f;
            victimPos.z = 0.0f;
            float x = 0.0f;
            float dx = 0.0f;
            float final_distance = 0.0f;
            CalculateForwardBlend(giant, breastPos, victimPos, max_distance, x, dx, final_distance);
            return final_distance <= max_distance;
        }
        bool AutoAim_Butt_TryButtSlam(Actor* giant, bool& left_hand) {
            if (!giant) return false;
            if (giant->IsPlayerRef() && IsFreeCameraEnabled()) return false;

            const float max_distance = Config::AutoAim.fAutoAim_Range_ButtSlam * get_visual_scale(giant);
            const float butt_offset_side = Config::AutoAim.fAutoAim_Butt_OffsetDistance_Side * get_visual_scale(giant);
            const float butt_offset_forward = Config::AutoAim.fAutoAim_Butt_OffsetDistance_Forward * get_visual_scale(giant);

            NiPoint3 buttPos_L = GetPresetAimPosition(giant, true, butt_offset_side, butt_offset_forward);
            NiPoint3 buttPos_R = GetPresetAimPosition(giant, false, butt_offset_side, butt_offset_forward);
            auto victim = FindClosestTargetBetweenTwoPoints_Rhomb(giant, buttPos_L, buttPos_R, max_distance, left_hand); // Overrides left_hand bool

            NiPoint3 buttPos = left_hand ? buttPos_L : buttPos_R; // Pick which hand should be used

            if (!victim) {
                DrawDebugSpheres(giant, buttPos_L, nullptr, max_distance, Breast_Color);
                DrawDebugSpheres(giant, buttPos_R, nullptr, max_distance, Breast_Color);
                return false; // No victim found
            }
            NiPoint3 victimPos = victim->GetPosition();

            DrawDebugSpheres(giant, buttPos, victim, max_distance, Breast_Color);

            buttPos.z = 0.0f;
            victimPos.z = 0.0f;
            float x = 0.0f;
            float dx = 0.0f;
            float final_distance = 0.0f;
            CalculateForwardBlend(giant, buttPos, victimPos, max_distance, x, dx, final_distance);
            return final_distance <= max_distance;
        }
        bool AutoAim_Hand_TryHandAim(Actor* giant, bool& left_hand) {
            if (!giant) return false;
            if (giant->IsPlayerRef() && IsFreeCameraEnabled()) return false;

            float max_distance = Config::AutoAim.fAutoAim_Range_Hand * get_visual_scale(giant);
            float hand_offset_side = Config::AutoAim.fAutoAim_Hand_OffsetDistance_Side * get_visual_scale(giant);
            float hand_offset_forward = Config::AutoAim.fAutoAim_Hand_OffsetDistance_Forward * get_visual_scale(giant);

            if (AnimationVars::Crawl::IsCrawling(giant)) { // Replace with Crawl version
                max_distance = Config::AutoAim.fAutoAim_Range_Hand_Crawl * get_visual_scale(giant);
                hand_offset_side = Config::AutoAim.fAutoAim_Hand_Crawl_OffsetDistance_Side * get_visual_scale(giant);
                hand_offset_forward = Config::AutoAim.fAutoAim_Hand_Crawl_OffsetDistance_Forward * get_visual_scale(giant);
            }

            NiPoint3 handPos_L = GetPresetAimPosition(giant, true, hand_offset_side, hand_offset_forward);
            NiPoint3 handPos_R = GetPresetAimPosition(giant, false, hand_offset_side, hand_offset_forward);
            auto victim = FindClosestTargetBetweenTwoPoints_Rhomb(giant, handPos_L, handPos_R, max_distance, left_hand); // Overrides left_hand bool

            NiPoint3 handPos = left_hand ? handPos_L : handPos_R; // Pick which hand should be used

            if (!victim) {
                DrawDebugSpheres(giant, handPos_L, nullptr, max_distance, Far_Stomp_Color);
                DrawDebugSpheres(giant, handPos_R, nullptr, max_distance, Far_Stomp_Color);
                return false;
            }
            NiPoint3 victimPos = victim->GetPosition();

            DrawDebugSpheres(giant, handPos, victim, max_distance, Far_Stomp_Color);

            handPos.z = 0.0f;
            victimPos.z = 0.0f;
            float x = 0.0f;
            float dx = 0.0f;
            float final_distance = 0.0f;
            CalculateForwardBlend(giant, handPos, victimPos, max_distance, x, dx, final_distance);
            return final_distance <= max_distance;
        }

        bool AutoAim_Foot_Directional(Actor* giant, bool& left_foot, bool forward_only) {
            if (!giant) return false;
            if (giant->IsPlayerRef() && IsFreeCameraEnabled()) return false;

            float max_distance = Config::AutoAim.fAutoAim_Range_Stomp * get_visual_scale(giant);
            float foot_offset = Config::AutoAim.fAutoAim_Foot_OffsetDistance * get_visual_scale(giant); 
            // ^ Instead of looking for R/L foot, we do position offset from center of Char to right/left, based on left_foot bool

            NiPoint3 footPos_L = GetPresetAimPosition(giant, true, foot_offset, 0.0f);
            NiPoint3 footPos_R = GetPresetAimPosition(giant, false, foot_offset, 0.0f);
            auto victim = FindClosestTargetBetweenTwoPoints_Rhomb(giant, footPos_L, footPos_R, max_distance, left_foot); // Overrides left_foot bool
            if (!victim) {
                RandomizeBlend_Debug_FlipFootBool(giant, footPos_L, footPos_R, max_distance, left_foot, Close_Stomp_Color);
                return false;
            }

            NiPoint3 victimPos = victim->GetPosition();
            NiPoint3 footPos = left_foot ? footPos_L : footPos_R; // Pick which foot should be used

            DrawDebugSpheres(giant, footPos, victim, max_distance, Close_Stomp_Color);
            
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

            x = std::clamp(x * Config::AutoAim.fAutoAim_AimMagnitudeMultiplier, -1.0f, 1.0f); // Slightly increase power of auto-aiming
            y = std::clamp(y * Config::AutoAim.fAutoAim_AimMagnitudeMultiplier, -1.0f, 1.0f); // Slightly increase power of auto-aiming

            if (Config::AutoAim.bDebugAutoAim) {
                logger::info("Blend2D X:{}, Y:{} | Victim:{}",x, y,victim->GetDisplayFullName());
                Cprint("Blend2D X:{}, Y:{} | Victim:{}",x, y,victim->GetDisplayFullName());
            }

            auto rng = RandomFloat(0.0f, Config::AutoAim.fAutoAim_NoHitValueRandomRange);
            bool AutoAim = ShouldAutoAim(final_distance, max_distance, dx);

            AnimationVars::Stomp::SetUnderStompBlend_Legacy(giant, AutoAim ? x : rng); // Old one stays for compatibility reasons
            AnimationVars::Stomp::SetUnderStompBlend_X(giant, AutoAim ? x : rng); // We added new behavior variables, needs new Behaviors in order to work
            AnimationVars::Stomp::SetUnderStompBlend_Y(giant, AutoAim ? y : rng); // We added new behavior variables, needs new Behaviors in order to work

            return AutoAim;
        }

        bool AutoAim_Foot_Directional_FarStomp(Actor* giant, bool& left_foot, bool strong_stomp) {
            if (!giant) return RandomBool();
            if (giant->IsPlayerRef() && IsFreeCameraEnabled()) return RandomBool();

            float max_distance = Config::AutoAim.fAutoAim_Range_FarStomp * get_visual_scale(giant);
            const float foot_offset = Config::AutoAim.fAutoAim_Foot_OffsetDistance * get_visual_scale(giant); 
            const float foot_offset_far = Config::AutoAim.fAutoAim_Foot_OffsetDistance_FarStomp * get_visual_scale(giant); 
            // ^ Instead of looking for R/L foot, we do position offset from center of Char to right/left, based on left_foot bool
            if (strong_stomp) {
                max_distance = Config::AutoAim.fAutoAim_Range_FarStomp_Strong * get_visual_scale(giant); // Strong version has shorter range because that's how it was animated
            }
            NiPoint3 footPos_L = GetPresetAimPosition(giant, true, foot_offset, foot_offset_far);
            NiPoint3 footPos_R = GetPresetAimPosition(giant, false, foot_offset, foot_offset_far);
            auto victim = FindClosestTargetBetweenTwoPoints_Rhomb(giant, footPos_L, footPos_R, max_distance, left_foot); // Overrides left_foot bool
            if (!victim) {
                RandomizeBlend_Debug_FlipFootBool(giant, footPos_L, footPos_R, max_distance, left_foot, Far_Stomp_Color);
                return false;
            }

            NiPoint3 victimPos = victim->GetPosition();
            NiPoint3 footPos = left_foot ? footPos_L : footPos_R; // Pick which foot should be used

            DrawDebugSpheres(giant, footPos, victim, max_distance, Far_Stomp_Color);
            
            footPos.z = 0.0f;
            victimPos.z = 0.0f;

            float x = 0.0f; // forward (>0) /   back (<1)
            float y = 0.0f; // right (>0)   /   left (<1)

            float dx = 0.0f;
            float dy = 0.0f;

            float final_distance = 0.0f;

            CalculateDirectionalBlend2D(giant, footPos, victimPos, max_distance, x, y, dx, dy, final_distance);

            x = std::clamp(x * Config::AutoAim.fAutoAim_AimMagnitudeMultiplier, -1.0f, 1.0f); // Slightly increase power of auto-aiming
            y = std::clamp(y * Config::AutoAim.fAutoAim_AimMagnitudeMultiplier, -1.0f, 1.0f); // Slightly increase power of auto-aiming

            if (Config::AutoAim.bDebugAutoAim) {
                logger::info("Blend2D X:{}, Y:{} | Victim:{}",x, y,victim->GetDisplayFullName());
                Cprint("Blend2D X:{}, Y:{} | Victim:{}",x, y,victim->GetDisplayFullName());
            }

            auto rng = RandomFloat(0.0f, Config::AutoAim.fAutoAim_NoHitValueRandomRange);
            bool AutoAim = ShouldAutoAim(final_distance, max_distance, dx);
            
            AnimationVars::Stomp::SetUnderStompBlend_Legacy(giant, AutoAim ? x : rng); // Old one stays for compatibility reasons
            AnimationVars::Stomp::SetUnderStompBlend_X(giant, AutoAim ? x : rng); // We added new behavior variables, needs new Behaviors in order to work
            AnimationVars::Stomp::SetUnderStompBlend_Y(giant, AutoAim ? y : rng); // We added new behavior variables, needs new Behaviors in order to work

            return AutoAim;
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