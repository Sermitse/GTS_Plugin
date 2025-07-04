#pragma once

namespace GTS {
    void UpdateMaxScale();
    float GetExpectedMaxSize(RE::Actor* a_Actor);
    void UpdateGlobalSizeLimit();
    void VisualScale_CheckForSizeAdjustment(Actor* actor, float& ScaleMult);
}