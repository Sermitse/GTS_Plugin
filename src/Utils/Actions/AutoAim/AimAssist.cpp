#include "Managers/Size_Killmoves/SizeKillMove.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Utils/Actions/AutoAim/AimAssist.hpp"
#include "Utils/Actions/AutoAim/AutoAimUtils.hpp"
#include "Managers/Cameras/CamUtil.hpp"
#include "Managers/Rumble.hpp"
#include "Config/Config.hpp"


using namespace GTS;

namespace Scan {
    // Result of probing one aim "zone" (close or farZone), without side effects.
    struct AimAttempt {
        bool hit = false;
        bool left = false;
        AimAssistResult result;
    };

    template <typename Fn>
    AimAttempt TryAim(Fn&& fn) {
        AimAttempt a;
        a.hit = fn(a.left, a.result);
        return a;
    }
    // A living victim always outranks a dead one, whichever zone found it.
    // If both/neither are alive, the closer zone keeps priority (ties go to Close).
    bool PreferFar(const AimAttempt& close, const AimAttempt& farZone) {
        if (!farZone.hit) return false;
        if (!close.hit) return true;
        if (farZone.result.alive && !close.result.alive) return true;
        return false;
    }

    // Applies whichever attempt wins (if any), updates left/hit accordingly.
    // Returns true if Close won (caller treats that as "under-stomp/-slam"),
    // false otherwise (Far won, or nobody hit).
    bool ApplyBestAim(Actor* giant, bool& left, bool& hit, const AimAttempt& close, const AimAttempt& farZone) {
        if (PreferFar(close, farZone)) {
            SetStompBlendValues(giant, farZone.result.blend_x, farZone.result.blend_y);
            left = farZone.left; hit = true;
            return false;
        }
        if (close.hit) {
            SetStompBlendValues(giant, close.result.blend_x, close.result.blend_y);
            left = close.left; hit = true;
            return true;
        }
        return false;
    }
    void TryKillMove(bool condition, Actor* giant, AimAttempt close, AimAttempt farZone, bool left, bool strong_Attack, bool hit, bool Understomp, bool sneak, float crush_mult, bool trample = false) {
        Actor* victim = Understomp ? close.result.victim : hit ? farZone.result.victim : nullptr;
        if (condition && victim) {
            float base_strong = Understomp ? Damage_Stomp_Under_Strong : Damage_Stomp_Strong;
            float base_light = Understomp ? Damage_Stomp_Under_Light : Damage_Stomp;
            float base_damage = strong_Attack ? base_strong : base_light;
            if (sneak) {
                base_damage = Damage_Stomp_Under_Light;
            } if (trample) {
                base_damage *= 5.0f; // Does 5 hits
            }
            const auto cause = left ? DamageSource::CrushedLeft : DamageSource::CrushedRight;
            const auto node = find_node(giant, left ? "NPC L Foot [Lft ]" : "NPC R Foot [Rft ]");
            StartKillmove(giant, victim, node, cause, base_damage, crush_mult, true);
        }
    }
    void StandingBranchCheck(Actor* giant, bool& left, bool strong_Attack, bool& Understomp, bool& hit, bool trample) {
        if (giant->IsSneaking()) return;
       
        auto close = TryAim([&](bool& l, AimAssistResult& r) {
            return AutoAim_Foot_Directional(giant, l, strong_Attack, &r);
        });
        auto farZone = TryAim([&](bool& l, AimAssistResult& r) {
            return AutoAim_Foot_Directional_FarStomp(giant, l, strong_Attack, &r);
        });
        Understomp = ApplyBestAim(giant, left, hit, close, farZone);
        TryKillMove(hit, giant, close, farZone, left, strong_Attack, hit, Understomp, false, 1.0f, trample);
    }
    void SneakBranchCheck(Actor* giant, bool& left, bool strong, bool& Understomp, bool& hit) {
        bool Sneaking = giant->IsSneaking() && !AnimationVars::Crawl::IsCrawling(giant);
        if (!Sneaking) return;

        auto close = TryAim([&](bool& l, AimAssistResult& r) {
            const bool Aim = strong ? AutoAim_Crawl_TryButtSlam(giant, l, &r) : AutoAim_Foot_Directional(giant, l, false, &r); // Otherwise try to hit someone with foot
            return Aim;
        });
        auto farZone = TryAim([&](bool& l, AimAssistResult& r) {
            const bool Aim = AutoAim_Hand_TryHandAim(giant, l, strong, &r); // Fallback: land a hand attack
            return Aim;
        });

        Understomp = ApplyBestAim(giant, left, hit, close, farZone);

        const bool canKillMove = !strong && hit && Understomp;
        TryKillMove(canKillMove, giant, close, farZone, left, strong, hit, Understomp, false, 1.0f);
    }
    void CrawlBranchCheck(Actor* giant, bool& left, bool strong, bool& Underslam, bool& hit) {
        bool Crawling = giant->IsSneaking() && AnimationVars::Crawl::IsCrawling(giant);
        if (!Crawling) return;

        auto close = TryAim([&](bool& l, AimAssistResult& r) {
            const bool Aim = strong ? AutoAim_Crawl_TryBreastSlam(giant, l, &r) : AutoAim_Hand_TryHandAim(giant, l, false, &r);
            return Aim;   // Try breast slam first                    
        });
        auto farZone = TryAim([&](bool& l, AimAssistResult& r) {
            const bool Aim = AutoAim_Hand_TryHandAim_Far(giant, l, strong, &r); // Far slam
            return Aim;
        });

        Underslam = ApplyBestAim(giant, left, hit, close, farZone);
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
        const bool autoAim = Config::AutoAim.bEnableAutoAim;
        if (giant->IsPlayerRef() && IsFreeCameraEnabled()) {
            return false;
        }
        bool Understomp = false; bool Hit = false;
        if (!AnimationVars::General::IsBusy(giant)) { 
            // Some key-binds fight other key-binds, so Tap E overrides Hold E as soon as you release E, overriding Blend we got, messing aim result
            if (autoAim || !giant->IsPlayerRef()) {
                Scan::StandingBranchCheck(giant, left, strong_Attack, Understomp, Hit, trample);
                Scan::SneakBranchCheck(giant, left, strong_Attack, Understomp, Hit);
                Scan::CrawlBranchCheck(giant, left, strong_Attack, Understomp, Hit);

                if (!Hit) {
                    RandomizeBlend(giant, left);
                    Understomp = RandomBool();
                }
            
                return Understomp;
            }
        }
        bool ManagedByConfig = Config::AutoAim.bPreventFarStomps && autoAim;
        return ManagedByConfig ? true : CrosshairUnderstomp(giant);
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