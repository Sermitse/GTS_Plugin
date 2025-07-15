#include "Hooks/Skyrim/PushAway.hpp"

namespace Hooks {

    void Hook_PushAway::Hook(Trampoline& trampoline) {

        log::info("Hook Papyrus PushAway");

        // Stagger is prevented inside:
        // SizeHitEffects -> prevent_Stagger

        static FunctionHook<void(uintptr_t* param_1, uintptr_t param_2, Actor* actor_1, Actor* actor_2, uintptr_t param_5)>ObjectRef_PushActorAway(
            // 996340 = 55682 (SE)
            // 9BF370 = 56213 (AE)
            // param_3 = Actor*
            // param_4 = Actor*
            REL::RelocationID(55682, 56213),
            [](auto* param_1, auto param_2, auto* actor_1, auto* actor_2, auto param_5) {

                if (actor_1 && actor_2) {
                    //log::trace<>("Param 3: {}", actor_1->GetDisplayFullName());
                    //log::trace("Param 4: {}", actor_2->GetDisplayFullName());
                    float size_difference = GetSizeDifference(actor_2, actor_1, SizeType::GiantessScale, false, false);
                    //log::trace("Size difference: {}", size_difference);

                    if (size_difference > 1.75f) {
                        //log::trace("> 1.75f, can't be pushed away");
                        return ObjectRef_PushActorAway(param_1, param_2, nullptr, nullptr, param_5);
                    }
                    else if (size_difference >= 1.25f) {
                        StaggerActor_Directional(actor_1, 0.25f, actor_2);
                        //log::trace("> 1.25f, can't be pushed away");
                        return ObjectRef_PushActorAway(param_1, param_2, nullptr, nullptr, param_5);
                    }
                }

                return ObjectRef_PushActorAway(param_1, param_2, actor_1, actor_2, param_5);
            }
        );
    }
}
