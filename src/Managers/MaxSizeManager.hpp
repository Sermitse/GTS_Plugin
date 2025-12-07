#pragma once

namespace GTS {
    void UpdateMaxScale(Actor* a_actor);
    void UpdateGlobalSizeLimit(Actor* a_actor);

    void VisualScale_CheckForSizeAdjustment(Actor* actor, float& ScaleMult);
    void Ench_Potions_ApplyBonuses(Actor* actor, float& value);

    float GetExpectedMaxSize(RE::Actor* a_Actor, float start_value = 1.0f);
    float MassMode_GetValuesForMenu(Actor* actor);

}