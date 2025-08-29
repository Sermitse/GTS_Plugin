#pragma once

namespace GTS {
    void UpdateMaxScale();
    float GetExpectedMaxSize(RE::Actor* a_Actor, float start_value = 1.0f);
    void UpdateGlobalSizeLimit();
    void VisualScale_CheckForSizeAdjustment(Actor* actor, float& ScaleMult);
    void ApplyPotionsAndEnchantments(Actor* actor, float& value);

    float MassMode_GetVisualValues(Actor* actor);
}