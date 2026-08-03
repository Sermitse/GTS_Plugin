#include "Managers/Size_Killmoves/KillMoveParamObtainer.hpp"
#include "Managers/Size_Killmoves/KillMoveHelper.hpp"
#include "Managers/Size_Killmoves/SizeKillMove.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Utils/Actions/AutoAim/AimAssist.hpp"
#include "Utils/Actions/AutoAim/AutoAimUtils.hpp"
#include "Managers/Cameras/CamUtil.hpp"
#include "Managers/Rumble.hpp"
#include "Config/Config.hpp"

using namespace GTS;

namespace Scan {
    // A living victim always outranks a dead one, whichever zone found it.
    // If both/neither are alive, the closer zone keeps priority (ties go to Close).
    AimZone PickZone(const AimAttempt& close, const AimAttempt& farZone) {
        const bool farBeatsClose = farZone.hit && (!close.hit || (farZone.result.alive && !close.result.alive));
        if (farBeatsClose) {
            return AimZone::Far;
        }
        if (close.hit) {
            return AimZone::Close;
        }
        return AimZone::None;
    }

    // Applies the blend values for whichever zone won and reports the outcome
    // (including the resolved victim, so callers no longer need to re-derive
    // "which .result.victim do I want" from close/farZone/hit/Understomp by hand).
    AimOutcome ApplyBestAim(Actor* giant, const AimAttempt& close, const AimAttempt& farZone) {
        AimOutcome outcome;
        outcome.zone = PickZone(close, farZone);

        switch (outcome.zone) {
            case AimZone::Far:
                SetStompBlendValues(giant, farZone.result.blend_x, farZone.result.blend_y);
                outcome.left = farZone.left;
                outcome.hit = true;
                outcome.victim = farZone.result.victim;
                break;
            case AimZone::Close:
                SetStompBlendValues(giant, close.result.blend_x, close.result.blend_y);
                outcome.left = close.left;
                outcome.hit = true;
                outcome.victim = close.result.victim;
                break;
            case AimZone::None:
                break;
        }
        return outcome;
    }

    // Standing stomps: close = directional foot aim, far = far-stomp variant.
    // Killmove triggers off whichever zone wins.
    AimOutcome StandingBranchCheck(Actor* giant, bool strong_Attack, bool trample) {
        if (giant->IsSneaking()) {
            return {};
        }

        auto close = TryAim([&](bool& l, AimAssistResult& r) {
            return AutoAim_Foot_Directional(giant, l, strong_Attack, &r);
        });
        auto farZone = TryAim([&](bool& l, AimAssistResult& r) {
            return AutoAim_Foot_Directional_FarStomp(giant, l, strong_Attack, &r);
        });

        AimOutcome outcome = ApplyBestAim(giant, close, farZone);
        if (outcome.hit) {
            auto params = GetStandingKillMoveParams(outcome, strong_Attack, trample);
            TryKillMove(giant, outcome, params);
        }
        return outcome;
    }

    // Sneak (not crawling) stomps: close = butt slam (strong) or a regular foot
    // hit (light), far = a hand-aim fallback. Killmove only triggers off a
    // light, close-zone hit.
    AimOutcome SneakBranchCheck(Actor* giant, bool strong) {
        const bool sneaking = giant->IsSneaking() && !AnimationVars::Crawl::IsCrawling(giant);
        if (!sneaking) {
            return {};
        }

        auto close = strong
            ? TryAim([&](bool& l, AimAssistResult& r) { return AutoAim_Crawl_TryButtSlam(giant, l, &r); })
            : TryAim([&](bool& l, AimAssistResult& r) { return AutoAim_Foot_Directional(giant, l, false, &r); }); // otherwise try to hit someone with foot
        auto farZone = TryAim([&](bool& l, AimAssistResult& r) {
            return AutoAim_Hand_TryHandAim(giant, l, strong, &r); // fallback: land a hand attack
        });

        AimOutcome outcome = ApplyBestAim(giant, close, farZone);

        const bool canKillMove = outcome.zone == AimZone::Close;
        if (canKillMove) {
            TryKillMove(giant, outcome, GetSneakKillMoveParams(outcome, strong));
        } 
        return outcome;
    }

    // Crawling slams: close = breast slam (strong) or hand aim (light),
    // far = far hand-slam. This branch never triggers a killmove.
    AimOutcome CrawlBranchCheck(Actor* giant, bool strong) {
        const bool crawling = giant->IsSneaking() && AnimationVars::Crawl::IsCrawling(giant);
        if (!crawling) {
            return {};
        }
        bool isKillMoveAllowed = false; // Used only in Breast Crush bool calculation, idea is to allow KillMove only when enemy is in front 
        auto close = strong
            ? TryAim([&](bool& l, AimAssistResult& r) { 
                auto result = AutoAim_Crawl_TryBreastSlam(giant, l, &r); 
                isKillMoveAllowed = r.canKillMove;
                return result;
            }) // breast slam first
            : TryAim([&](bool& l, AimAssistResult& r) { return AutoAim_Hand_TryHandAim(giant, l, false, &r); });
        auto farZone = TryAim([&](bool& l, AimAssistResult& r) {return AutoAim_Hand_TryHandAim_Far(giant, l, strong, &r); }); // far slam

        AimOutcome outcome = ApplyBestAim(giant, close, farZone);

        if (strong) {
            const bool canKillMove = outcome.zone == AimZone::Close && isKillMoveAllowed;

            logger::info("CanKillMove: {}, KillMoveAllowed: {}", canKillMove, isKillMoveAllowed);
            if (canKillMove) {
                TryKillMove(giant, outcome, GetCrawlKillMoveParams(outcome));
            } 
        }

        return outcome;
    }
}

namespace GTS { 
    void RandomizeBlend(Actor* giant, bool left) {
        const float range_x = Config::AutoAim.fAimAssist_NoHitValueRandomRange;
        const float range_y = Config::AutoAim.fAimAssist_NoHitValueRandomRange;
        SetStompBlendValues(giant,
            RandomFloat(0.0f, range_x), 
            RandomFloat(0.0f, left ? -range_y : range_y)
        );
    }

    bool AutoAim_And_DetermineStompType(Actor* giant, bool& left, bool strong_Attack, bool trample) {
        if (giant->IsPlayerRef() && IsFreeCameraEnabled()) {
            return false;
        }
        const bool autoAim = Config::AutoAim.bEnableAutoAim;
        // Some key-binds fight other key-binds, so Tap E overrides Hold E as soon as you release E, overriding Blend we got, messing aim result
        if (!AnimationVars::General::IsBusy(giant)) {
            
            if (autoAim || !giant->IsPlayerRef()) {
                AimOutcome outcome = Scan::StandingBranchCheck(giant, strong_Attack, trample);
                if (!outcome.hit) outcome = Scan::SneakBranchCheck(giant, strong_Attack);
                if (!outcome.hit) outcome = Scan::CrawlBranchCheck(giant, strong_Attack);

                if (!outcome.hit) {
                    RandomizeBlend(giant, left);
                    return RandomBool();
                }

                left = outcome.left;
                return outcome.zone == AimZone::Close;
            }
        }

        const bool managedByConfig = Config::AutoAim.bPreventFarStomps && autoAim;
        return managedByConfig ? true : CrosshairUnderstomp(giant);
    }

    bool CrosshairUnderstomp(Actor* giant) { // Should be player exclusive
        if (!giant->IsPlayerRef()) { // NPC's shouldn't be able to use it
            return true;
        }
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
            SetStompBlendValues(giant, blend, 0.0f);
            // Blend between "close" and "farZone" under-stomps
        }
        return allow;
    }
}