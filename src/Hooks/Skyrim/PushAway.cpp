#include "Hooks/Skyrim/PushAway.hpp"

namespace Hooks {

    void Hook_PushAway::Hook(Trampoline& trampoline) {

        log::info("Hook Papyrus PushAway");

        // Stagger is prevented inside:
        // SizeHitEffects -> prevent_Stagger

        static FunctionHook<void(uint64_t* a_unk1, uint32_t a_unk2, TESObjectREFR* a_source, Actor* a_target, float a_force)>ObjectRef_PushActorAway(
            // 996340 = 55682 (SE)
            // 9BF370 = 56213 (AE)
            // param_3 = Actor*
            // param_4 = Actor*
            REL::RelocationID(55682, 56213),
            [](uint64_t* a_unk1, uint32_t a_unk2, TESObjectREFR* a_source, Actor* a_target, float a_force) {

                if (a_source && a_target) {

                    const auto ActorAsSource = skyrim_cast<Actor*>(a_source);
                    float size_difference = 1.0f;

                    if (ActorAsSource) {
                        size_difference = GetSizeDifference(a_target, ActorAsSource, SizeType::VisualScale, false, false);
                    }

                    if (size_difference > 1.75f) {
                        return;
                    }
                    if (size_difference >= 1.25f && ActorAsSource) {
                        StaggerActor_Directional(ActorAsSource, a_force, a_target);
                        return;
                    }
                }

                ObjectRef_PushActorAway(a_unk1, a_unk2, a_source, a_target, a_force);
            }
        );
    }
}
