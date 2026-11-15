#include "Managers/Size_Killmoves/KillMoveParamObtainer.hpp"
#include "Managers/Size_Killmoves/KillMoveHelper.hpp"
#include "Managers/Size_Killmoves/SizeKillMove.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Utils/Actions/AutoAim/AimAssist.hpp"
#include "Utils/Actions/AutoAim/AutoAimUtils.hpp"
#include "Utils/Actions/AutoAim/AutoAimUtils_Calculation.hpp"
#include "Managers/Cameras/CamUtil.hpp"
#include "Systems/Events/EventData.hpp"
#include "Managers/Rumble.hpp"
#include "Config/Config.hpp"

#include "Managers/Animation/KillMoveStarters.hpp"

using namespace GTS;


namespace {
    void ClearKillMoveData(Actor* giant) {
        if (auto tranData = Transient::GetActorData(giant)) {
            tranData->lastAimOutcome = AimOutcome();
        }
    }
    void RecordKillMoveData(Actor* giant, AimOutcome outcome) {
        if (auto tranData = Transient::GetActorData(giant)) {
            tranData->lastAimOutcome = outcome;
        }
    }
    
}

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
                SetStompBlendValues(giant, farZone.result.blend_x, FixYBlend(farZone.left, farZone.result.blend_y, farZone.result.fixBlending), 
                    "Far Auto Aim Zone"
                );
                outcome.left = farZone.left;
                outcome.hit = true;
                outcome.victim = farZone.result.victim;
                outcome.victim_color = farZone.result.victim_color;

                outcome.shapeRange = farZone.result.shapeRange;
                outcome.isSideBlend = farZone.result.isSideBlend;
                outcome.fixBlending = farZone.result.fixBlending;
                outcome.startPos = farZone.result.startPos;
                outcome.trackEnemy = farZone.result.trackEnemy;
                outcome.positiveX = farZone.result.blend_x > 0.0f;
                outcome.inPercent_Forward = farZone.result.inPercent_Forward;
                outcome.inPercent_Side = farZone.result.inPercent_Side;
                break;
            case AimZone::Close:
                SetStompBlendValues(giant, close.result.blend_x, FixYBlend(close.left, close.result.blend_y, close.result.fixBlending), 
                    "Close Auto Aim Zone"
                );
                outcome.left = close.left;
                outcome.hit = true;
                outcome.victim = close.result.victim;
                outcome.victim_color = close.result.victim_color;

                outcome.shapeRange = close.result.shapeRange;
                outcome.isSideBlend = close.result.isSideBlend;
                outcome.fixBlending = close.result.fixBlending;
                outcome.startPos = close.result.startPos;
                outcome.trackEnemy = close.result.trackEnemy;
                outcome.positiveX = close.result.blend_x > 0.0f;
                outcome.inPercent_Forward = close.result.inPercent_Forward;
                outcome.inPercent_Side = close.result.inPercent_Side;
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
        AnimationKillMoveStarters::HandleStandingKillMove(giant, outcome, strong_Attack, trample); // Launches Kill-Move if conditions are met
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
            ? TryAim([&](bool& l, AimAssistResult& r) { return AutoAim_Sneak_TryButtSlam(giant, l, &r); })
            : TryAim([&](bool& l, AimAssistResult& r) { return AutoAim_Foot_Directional(giant, l, false, &r); }); // otherwise try to hit someone with foot
        auto farZone = TryAim([&](bool& l, AimAssistResult& r) {
            return AutoAim_Hand_TryHandAim_Close(giant, l, strong, &r); // fallback: land a hand attack
        });


        // Only some attacks can trigger sneak kill moves. See 'Managers/Animation/KillMoveStarters.hpp'
        AimOutcome outcome = ApplyBestAim(giant, close, farZone);
        return outcome;
    }

    // Crawling slams: close = breast slam (strong) or hand aim (light),
    // far = far hand-slam. This branch never triggers a killmove.
    AimOutcome CrawlBranchCheck(Actor* giant, bool strong) {
        const bool crawling = giant->IsSneaking() && AnimationVars::Crawl::IsCrawling(giant);
        if (!crawling) {
            return {};
        }
        auto close = strong
            ? TryAim([&](bool& l, AimAssistResult& r) { 
                return AutoAim_Crawl_TryBreastSlam(giant, &r); 
            }) // breast slam first
            : TryAim([&](bool& l, AimAssistResult& r) { return AutoAim_Hand_TryHandAim_Close(giant, l, false, &r); });

        auto farZone = TryAim([&](bool& l, AimAssistResult& r) {
            return AutoAim_Hand_TryHandAim_Far(giant, l, strong, &r);}
        ); // far slam

        // Only some attacks can trigger crawl kill moves. See 'Managers/Animation/KillMoveStarters.hpp'
        AimOutcome outcome = ApplyBestAim(giant, close, farZone);

        return outcome;
    }
}

namespace GTS { 
    void RandomizeBlend(Actor* giant, bool left) {
        const float range_x = std::max(0.005f, Config::AutoAim.fAimAssistP_NoHitValueRandomRange);
        const float range_y = std::max(0.005f, Config::AutoAim.fAimAssistP_NoHitValueRandomRange);
        SetStompBlendValues(giant,
            RandomFloat(0.0025f, range_x), 
            RandomFloat(left ? -0.0025f : 0.0025f, left ? -range_y : range_y),
            "Randomize Blend"
        );
    }

    bool AutoAim_And_DetermineStompType(Actor* giant, bool& left, bool strong_Attack, bool trample) {
        if (giant->IsPlayerRef() && IsFreeCameraEnabled()) {
            return false;
        }
        const bool managedByConfig = Config::AutoAim.bPreventFarStomps;
        const bool onCooldown = IsActionOnCooldown(giant, CooldownSource::Misc_AutoAim_Cooldown); // Another layer of fixing this bs
        // Some key-binds fight other key-binds, so Tap E overrides Hold E as soon as you release E, overriding Blend we got, messing aim result
        if (!AnimationVars::General::IsBusy(giant) && !onCooldown) {
            
            ClearKillMoveData(giant);
            ApplyActionCooldown(giant, CooldownSource::Misc_AutoAim_Cooldown);
            AimOutcome outcome = Scan::StandingBranchCheck(giant, strong_Attack, trample);
            if (!outcome.hit) outcome = Scan::SneakBranchCheck(giant, strong_Attack);
            if (!outcome.hit) outcome = Scan::CrawlBranchCheck(giant, strong_Attack);
            if (outcome.hit && outcome.victim) {    
                AutoAimTask_TrackTinyTask(giant, outcome.victim, outcome);
                HighlightVictim(giant, outcome.victim, outcome);
                RecordKillMoveData(giant, outcome);
            }
            if (!outcome.hit) {
                left = !left;
                RandomizeBlend(giant, left);
                if (managedByConfig) {
                    return true;
                }
                return RandomBool();
            }
            
            left = outcome.left;
            return outcome.zone == AimZone::Close;
        }

        
        return managedByConfig ? true : RandomBool();
    }

    void HighlightVictim(Actor* giant, Actor* victim, const AimOutcome& outcome) {
        if (Config::AutoAim.bDebugAutoAim) {
            const bool Rhomb = Config::AutoAim.bUseRhombShape;
            const float rotation = giant->data.angle.z;
            if (victim) {
                auto color = outcome.victim_color;
                color[3] *= 1.5f;

                auto victimPos = victim->GetPosition();
            
                Rhomb ? DebugDraw::DrawRhomb(glm::vec3(victimPos.x, victimPos.y, victimPos.z), 6.0f * get_visual_scale(giant), rotation, 2000, color)
                :       DebugDraw::DrawSphere(glm::vec3(victimPos.x, victimPos.y, victimPos.z), 6.0f * get_visual_scale(giant), 2000, color);
            }
        }
    }
}