#include "Managers/Size_Killmoves/KillMoveParamObtainer.hpp"
#include "Utils/Actions/AutoAim/AutoAimUtils.hpp"
#include "Utils/Actions/AutoAim/AimAssist.hpp"

using namespace GTS;

namespace GTS {
    KillMoveParameters GetStandingKillMoveParams(AimOutcome outcome, bool strongAttack, bool trample, KillMoveParameters params) {
        const bool close            = outcome.zone == AimZone::Close;
        params.nodeLookups          = {outcome.left ? "NPC L Foot [Lft ]" : "NPC R Foot [Rft ]"};
        params.damageSource         = outcome.left ? DamageSource::CrushedLeft : DamageSource::CrushedRight;
        params.isStrongAttack       = strongAttack;
        params.isFootAttack         = true;

        const float baseStrong      = close ? Damage_Stomp_Under_Strong : Damage_Stomp_Strong;
        const float baseLight       = close ? Damage_Stomp_Under_Light : Damage_Stomp;
        const float baseDamage      = strongAttack ? baseStrong : baseLight;

        params.baseDamage           = trample ? baseDamage * 5.0f : baseDamage;
        // Trample does 5 hits

        return params;
    }

    KillMoveParameters GetSneakKillMoveParams(AimOutcome outcome, bool strongAttack, KillMoveParameters params) {
        if (strongAttack) {
            params.nodeLookups      = {"NPC R Butt", "NPC L Butt"};
            params.damageSource     = DamageSource::Booty;
        } else {
            params.damageSource     = outcome.left ? DamageSource::CrushedLeft : DamageSource::CrushedRight;
            params.nodeLookups      = { outcome.left ? "NPC L Foot [Lft ]" : "NPC R Foot [Rft ]"};
        }
        
        params.isStrongAttack       = strongAttack;
        params.isFootAttack         = !strongAttack; // Don't apply heel height to Butt Crush

        params.baseDamage           = strongAttack ? Damage_ButtCrush_ButtImpact : Damage_Stomp_Under_Light;
        params.lookAtNodeDistance   = strongAttack ? 70.0f : 30.0f;
        params.deathHoldDistance    = strongAttack ? 80.0f : 40.0f;
        params.crushThreshold       = strongAttack ? 0.925f : 1.0f;

        return params;
    }

    KillMoveParameters GetCrawlKillMoveParams(AimOutcome outcome, KillMoveParameters params) {      
        params.nodeLookups          = {"L Breast01","R Breast01"};
        params.damageSource         = DamageSource::BreastImpact;
        params.isStrongAttack       = true;
        params.baseDamage           = Damage_BreastCrush_BreastImpact;
        params.lookAtNodeDistance   = 45.0f;
        params.deathHoldDistance    = 50.0f;
        params.crushThreshold       = 0.8f;
        params.orbitAngle           = 45.0f;
        params.orbitTime            = 12.0f;
        return params;
    }

    KillMoveParameters GetKickKillMoveParams(bool left, bool strong, KillMoveParameters params) {
        params.nodeLookups          = { left ? "NPC L Foot [Lft ]" : "NPC R Foot [Rft ]"};
        params.damageSource         = left ? DamageSource::KickedLeft : DamageSource::KickedRight;
        params.baseDamage           = strong ? Damage_Kick_Strong : Damage_Kick;
        params.isStrongAttack       = strong;
        params.crushThreshold       = 1.8f;
        return params;
    }
}