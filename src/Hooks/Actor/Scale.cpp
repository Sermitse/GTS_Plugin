#include "Hooks/Actor/Scale.hpp"
#include "Hooks/Util/HookUtil.hpp"

using namespace GTS;

namespace {
	float affect_by_scale(TESObjectREFR* ref, float original) {
		Actor* giant = skyrim_cast<Actor*>(ref);
		if (giant) {
			float scale = get_giantess_scale(giant);
			return scale;
		}
		return original;
	}
}

namespace Hooks {

    struct AlterDashDistanceScale {

        // This hook affects Dodge distance / Movement through weapon attacks
    	// (when you perform a forward power attack for example)

        // ---------------SE:
		// 0x1404E6360 : 31949
		// 0x1404e68ba - 0x1404E6360 = 0x55A 
		//----------------AE:
		// 0x1404fe820 : 32703
		// 0x1404fed71 - 0x1404fe820 = 0x551

        static float thunk([[maybe_unused]] TESObjectREFR* a_this) {

            GTS_PROFILE_ENTRYPOINT("ActorScale::AlterDashDistanceScale");

            // Always return 1.0, We don't want to scale that: breaks dodge mods
            // And we alter it by havok hook anyway.
            return 1.0f; 
        }

        FUNCTYPE_CALL func;
    };

    struct AlterMovementSpeedScale {

        // Movement speed alteration

		// ---------------SE:
        // sub_14060EEF0 : 37013            
        // 0x14060ef0a - 0x14060EEF0 = 0x1A      
        // ---------------AE:
        // FUN_140630510 :  37943
        // 0x140630561 - 0x140630510 = 0x51

		//This function is used as a resulting multiplier to calculate what the final movement speed will be.
        static float thunk([[maybe_unused]] TESObjectREFR* a_this) {

            GTS_PROFILE_ENTRYPOINT("ActorScale::AlterMovementSpeedScale");

            return 1.f;
        }

        FUNCTYPE_CALL func;
    };

    struct AlterAnimSpeedScale {

        // something bone related
        // Affects Animation speed of: Walk Speed, Sneak Speed
        // There's vanilla bug: If you save the game at SetScale of 2.0 (for example)
    	// Then load a save and perform SetScale of 1.0 = your animations will look slower. Bethesda.
    	// Anyway, we want to always force it to 1.0 since we manage animation speed anyway.
    	// This hook seems to be called only once, on save file load.

        // ---------------SE:
        // 0x14071b230 : 41683
        // 0x14071b261 - 0x14071b230 = 0x31
        // ---------------AE:
        // FUN_140746b40 : 42768
        // 0x140746b71 - 0x140746b40 = 0x31   // wow it's the same

        static float thunk([[maybe_unused]] TESObjectREFR* a_this) {

            GTS_PROFILE_ENTRYPOINT("ActorScale::AlterAnimSpeedScale");

            // force it to 1.0. We DON'T want the SetScale() to affect it.
            //Only runs once on actor load
            return 1.0f;
        }

        FUNCTYPE_CALL func;
    };

    
    void Hook_Scale::Install() {

        logger::info("Installing Scale Hooks...");

        stl::write_call<AlterDashDistanceScale>(REL::RelocationID(31949, 32703, NULL), REL::VariantOffset(0x55A, 0x551, NULL));
        stl::write_call<AlterMovementSpeedScale>(REL::RelocationID(37013, 37943, NULL), REL::VariantOffset(0x1A, 0x51, NULL));
        stl::write_call<AlterAnimSpeedScale>(REL::RelocationID(41683, 42768), REL::VariantOffset(0x31, 0x31, NULL));

    }
}