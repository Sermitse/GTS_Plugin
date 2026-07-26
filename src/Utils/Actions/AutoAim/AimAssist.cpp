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
    void FlipNextAttack(bool& left) {
        left = !left;
    }
}

namespace GTS { 
    bool AutoAim_And_DetermineStompType(Actor* giant, bool& left, bool strong_Attack) {
        const bool autoAim = Config::AutoAim.bEnableAutoAim;
        if (giant->IsPlayerRef() && IsFreeCameraEnabled()) {
            return false;
        }
        if (autoAim || !giant->IsPlayerRef()) {
            if (AutoAim_IsSneakingOrCrawling(giant)) { // Sneak or Crawl branch
                if (strong_Attack && giant->IsSneaking() && AutoAim_Butt_TryButtSlam(giant, left))  { // Strong Attack ?  First check if we can butt slam
                    return true; // Always counts as UnderStomp
                }
                if (strong_Attack && AnimationVars::Crawl::IsCrawling(giant) && AutoAim_Butt_TryBreastSlam(giant, left)) { // Strong Attack ? Try to breast slam
                    return true; // Count as under-stomp
                }
                if (!AnimationVars::Crawl::IsCrawling(giant) && AutoAim_Foot_Directional(giant, left, false)) { // Try to foot stomp under self next
                    return true; // Count as under-stomp
                }
                if (AutoAim_Hand_TryHandAim(giant, left)) { // Didn't find anyone for butt, breast or stomp attacks, try hand now
                    return false; // Hand shouldn't count as Understomp, else actor uses foot to attack instead
                }
                FlipNextAttack(left);
                RandomizeBlend(giant, left);
                return RandomBool(); // Everything failed, just rng it
            } 
            else if (AutoAim_Foot_Directional(giant, left, false)) { // Not sneaking or crawling/didn't find any hand targets, try normal understomp
                return true;
            } 
            else if (AutoAim_Foot_Directional_FarStomp(giant, left, strong_Attack)) { // Couldn't find anyone, try far stomp now
                return false; // Not an understomp
            }
            FlipNextAttack(left);
            RandomizeBlend(giant, left);
        }

        return Config::AutoAim.bPreventFarStomps ? true : CrosshairUnderstomp(giant);
    }

    bool CrosshairUnderstomp(Actor* giant) { // Should be player exclusive
        if (!giant->IsPlayerRef()) { // NPC's shouldn't be able to use it
            return false;
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