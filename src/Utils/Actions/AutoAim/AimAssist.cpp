#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Utils/Actions/AutoAim/AimAssist.hpp"
#include "Utils/Actions/AutoAim/AutoAimUtils.hpp"
#include "Managers/Cameras/CamUtil.hpp"
#include "Managers/Rumble.hpp"
#include "Config/Config.hpp"


using namespace GTS;

namespace {
    void RandomizeBlend(Actor* giant, bool left) {
        const float range_x = Config::AutoAim.fAimAssist_NoHitValueRandomRange;
        const float range_y = Config::AutoAim.fAimAssist_NoHitValueRandomRange;
        SetStompBlendValues(giant,
            RandomFloat(0.0f, range_x), 
            RandomFloat(0.0f, left ? -range_y : range_y)
        );
    }
}

namespace Scan {
    void StandingBranchCheck(Actor* giant, bool& left, bool strong_Attack, bool& Understomp, bool& hit) {
        if (giant->IsSneaking()) return;

        if (AutoAim_Foot_Directional(giant, left, strong_Attack)) {
            Understomp = true;  hit = true;
        } else if (AutoAim_Foot_Directional_FarStomp(giant, left, strong_Attack)) { // Couldn't find anyone, try far stomp now
            Understomp = false; hit = true;// Not an understomp
        }
    }
    void SneakBranchCheck(Actor* giant, bool& left, bool strong_Attack, bool& Understomp, bool& hit) {
        bool Sneaking = giant->IsSneaking() && !AnimationVars::Crawl::IsCrawling(giant);
        if (!Sneaking) return;
        
        if (strong_Attack && AutoAim_Butt_TryButtSlam(giant, left))  { // Strong Attack ?  First check if we can butt slam
            Understomp = true;  hit = true;// Always counts as UnderStomp
        } else if (!strong_Attack && AutoAim_Foot_Directional(giant, left, false)) { // Then try to hit someone with foot
            Understomp = true;  hit = true;// Count as under-stomp
        } else if (AutoAim_Hand_TryHandAim(giant, left, strong_Attack)) { // Then try to land hand attack
            Understomp = false; hit = true;// Should never be understomp
        }
    }
    void CrawlBranchCheck(Actor* giant, bool& left, bool strong_Attack, bool& Underslam, bool& hit) {
        bool Crawling = giant->IsSneaking() && AnimationVars::Crawl::IsCrawling(giant);
        if (!Crawling) return;

        if (strong_Attack && AutoAim_Butt_TryBreastSlam(giant, left))  { // Try breast slam first
            Underslam = true;  hit = true;// Always counts as Underslam
        } else if (!strong_Attack && AutoAim_Hand_TryHandAim(giant, left, false)) { 
            Underslam = true; hit = true;// In crawl case it should be underslam
        } else if (AutoAim_Hand_TryHandAim_Far(giant, left, strong_Attack)) {
            Underslam = false; hit = true; // Far slam, in both cases (Strong/Light attack) NOT an underslam
        }
    }
}

namespace GTS { 
    bool AutoAim_And_DetermineStompType(Actor* giant, bool& left, bool strong_Attack) {
        const bool autoAim = Config::AutoAim.bEnableAutoAim;
        if (giant->IsPlayerRef() && IsFreeCameraEnabled()) {
            return false;
        }
        bool Understomp = false; bool Hit = false;
        if (!AnimationVars::General::IsBusy(giant)) { 
            // Some key-binds fight other key-binds, so Tap E overrides Hold E as soon as you release E, overriding Blend we got, messing aim result
            if (autoAim || !giant->IsPlayerRef()) {
                Scan::StandingBranchCheck(giant, left, strong_Attack, Understomp, Hit);
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
            // Blend between "close" and "far" under-stomps
        }
        return allow;
    }
}