#include "Hooks/Papyrus/PushAway.hpp"
#include "Hooks/Util/HookUtil.hpp"

namespace Hooks {

    // Stagger is prevented inside:
	// SizeHitEffects -> prevent_Stagger

    struct ObjectRefPushAway {

        // 996340 = 55682 (SE)
		// 9BF370 = 56213 (AE)
		// param_3 = Actor*
		// param_4 = Actor*

	    static void thunk(uint64_t* a_unk1, uint32_t a_unk2, TESObjectREFR* a_source, Actor* a_target, float a_force) {

            GTS_PROFILE_ENTRYPOINT("PapyrusPush::ObjectRefPushAway");

            if (a_source && a_target) {

                const auto ActorAsSource = skyrim_cast<Actor*>(a_source);
                float size_difference = 1.0f;

                if (ActorAsSource) {
                    size_difference = GetSizeDifference(a_target, ActorAsSource, SizeType::VisualScale, false, false);
                }
                //Else source was not an actor, assume 1x scale
                else {
                    size_difference = get_visual_scale(a_target);
                }

                if (size_difference > 1.75f) {
                    return;
                }
                if (size_difference >= 1.25f && ActorAsSource) {
                    StaggerActor_Directional(ActorAsSource, a_force, a_target);
                    return;
                }

            }

            func(a_unk1, a_unk2, a_source, a_target, a_force);
	    }

        FUNCTYPE_DETOUR func;

    };

    void Hook_PushAway::Install() {
        log::info("Installing Papyrus ObjectRefPushAway Detour Hook...");
        stl::write_detour<ObjectRefPushAway>(REL::RelocationID(55682, 56213, NULL));
    }

}
