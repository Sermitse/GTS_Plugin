

#pragma once

#include "managers/InputManager.hpp"
#include "events.hpp"

using namespace std;
using namespace SKSE;
using namespace RE;
using namespace Gts;

namespace Gts {
    bool TinyCalamity_WrathfulCalamity(Actor* giant);
    
    void TinyCalamity_ShrinkActor(Actor* giant, Actor* tiny, float shrink);
    void TinyCalamity_SeekForShrink(Actor* giant, Actor* tiny, float damage, float maxFootDistance, DamageSource Cause, bool Right, bool ApplyCooldown, bool ignore_rotation);
    void TinyCalamity_ExplodeActor(Actor* giant, Actor* tiny);
    void TinyCalamity_StaggerActor(Actor* giant, Actor* tiny, float giantHp);
     
    void TinyCalamity_SeekActors(Actor* giant);
    void TinyCalamity_CrushCheck(Actor* giant, Actor* tiny);
    void TinyCalamity_BonusSpeed(Actor* giant);
}